#include "shadow.hpp"

#include <ekutils/socket_d.hpp>
#include <ekutils/log.hpp>

namespace tlproxy {

class tcp_listener_wrapper : public listener_wrapper {
	ekutils::tcp_listener_d sock;
public:
	tcp_listener_wrapper(const std::string & address, const std::string & port) :
		sock(address, port, ekutils::sock_flags::non_blocking) {};
	virtual ekutils::descriptor & listener() override {
		return sock;
	}
	virtual void listen() override {
		sock.start();
	}
	virtual std::unique_ptr<ekutils::io_stream_d> accept() override {
		return std::make_unique<ekutils::tcp_socket_d>(sock.accept());
	}
};

class unix_listener_wrapper : public listener_wrapper {
	ekutils::unix_stream_listener_d sock;
	std::filesystem::path path;
public:
	unix_listener_wrapper(const std::filesystem::path & address) :
		sock(address, ekutils::sock_flags::non_blocking), path(address) {};
	virtual ekutils::descriptor & listener() override {
		return sock;
	}
	virtual void listen() override {
		sock.start();
	}
	virtual std::unique_ptr<ekutils::io_stream_d> accept() override {
		return std::make_unique<ekutils::unix_stream_socket_d>(sock.accept());
	}
	virtual ~unix_listener_wrapper() {
		std::filesystem::remove(path);
	}
};

class tcp_connection_factory : public connection_factory {
	std::vector<ekutils::connection_info> addresses;
public:
	tcp_connection_factory(const std::string & address, const std::string & port) :
		addresses(ekutils::connection_info::resolve(address, port)) {}
	virtual std::unique_ptr<ekutils::io_stream_d> connect() override {
		return std::make_unique<ekutils::tcp_socket_d>(addresses, ekutils::sock_flags::non_blocking);
	}
};

class unix_connection_factory : public connection_factory {
	std::filesystem::path path;
public:
	unix_connection_factory(const std::filesystem::path & address) : path(address) {}
	virtual std::unique_ptr<ekutils::io_stream_d> connect() override {
		return std::make_unique<ekutils::unix_stream_socket_d>(path, ekutils::sock_flags::non_blocking);
	}
};

std::unique_ptr<listener_wrapper> resolve_listener(const sock_specs & specs) {
	if (specs.type == sock_specs::sock_t::tcp) {
		return std::make_unique<tcp_listener_wrapper>(
			specs.address.empty() ? "localhost" : specs.address,
			specs.port.empty() ? "0" : specs.port
		);
	} else if (specs.type == sock_specs::sock_t::un) {
		return std::make_unique<unix_listener_wrapper>(
			specs.address.empty() ? throw std::runtime_error("address of unix socket can't be empty") : specs.address
		);
	} else {
		throw std::runtime_error("unsupported operation");
	}
}

std::unique_ptr<connection_factory> resolve_connection_factory(const sock_specs & specs) {
	if (specs.type == sock_specs::sock_t::tcp) {
		return std::make_unique<tcp_connection_factory>(
			specs.address.empty() ? "localhost" : specs.address,
			specs.port.empty() ? throw std::runtime_error("remote server tcp port is unspecified") : specs.port
		);
	} else if (specs.type == sock_specs::sock_t::un) {
		return std::make_unique<unix_connection_factory>(
			specs.address.empty() ? throw std::runtime_error("address of unix socket can't be empty") : specs.address
		);
	} else {
		throw std::runtime_error("unsupported operation");
	}
}

shadow::shadow(const sock_specs & src, const sock_specs & dst) :
		epoll(), listener(resolve_listener(src)), connections(resolve_connection_factory(dst)) {
	listener->listen();
	epoll.add(listener->listener(), [this](auto &, std::uint32_t events) { on_new_connection(events); });
}

[[noreturn]]
void shadow::run() {
	while (true) {
		log_verbose("cycle");
		epoll.wait(-1);
	}
}

void shadow::on_new_connection(std::uint32_t events) {
	if (events & ekutils::actions::in) {
		log_info("new connection");
		new portal(epoll, listener->accept(), connections->connect());
	}
}

} // namespace tlproxy
