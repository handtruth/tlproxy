#include "portal.hpp"

#include <ekutils/log.hpp>

namespace tlproxy {

const std::size_t step = 2048;

int connection_counter;

void gate::send() {
	try {
		int sent = stream->write(input.data(), input.size());
		if (sent == -1)
			return;
		input.move(sent);
	} catch (const std::system_error & e) {
		if (std::errc(e.code().value()) == std::errc::broken_pipe)
			return;
		throw;
	}
}

void gate::copy_to(gate & other) {
	while (true) {
		other.input.asize(step);
		int count = stream->read(other.input.data() + other.input.size() - step, step);
		if (count == -1) {
			other.input.ssize(step);
			return;
		}
		if (static_cast<std::size_t>(count) < step) {
			other.input.ssize(step - count);
			other.send();
			return;
		}
	}
}

portal::portal(ekutils::epoll_d & selectable, std::unique_ptr<ekutils::io_stream_d> && source,
		std::unique_ptr<ekutils::io_stream_d> && dest) : epoll(selectable), src { std::move(source) }, dst { std::move(dest) } {
	using namespace ekutils::actions;
	log_info("connections #" + std::to_string(++connection_counter));
	epoll.add(*src.stream, in | out | rdhup | err | hup | et, [this](auto &, std::uint32_t events) -> void {
		on_event(src, dst, events);
	});
	epoll.add(*dst.stream, in | out | rdhup | err | hup | et, [this](auto &, std::uint32_t events) -> void {
		on_event(dst, src, events);
	});
}

void portal::on_event(gate & actor, gate & other, std::uint32_t events) {
	if (events & ekutils::actions::in) {
		log_debug("data in");
		actor.copy_to(other);
	}
	if (events & ekutils::actions::out) {
		log_debug("data out");
		actor.send();
	}
	if (events & (ekutils::actions::err | ekutils::actions::hup | ekutils::actions::rdhup)) {
		log_info("disconnect, connections #" + std::to_string(--connection_counter));
		epoll.remove(*actor.stream);
		epoll.remove(*other.stream);
		epoll.later([this]() -> void { delete this; });
	}
}

} // namespace tlproxy
