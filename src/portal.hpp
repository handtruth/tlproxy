#ifndef PORTAL_HEAD_JTHWDFG233GD
#define PORTAL_HEAD_JTHWDFG233GD

#include <memory>

#include <ekutils/io_stream_d.hpp>
#include <ekutils/expandbuff.hpp>
#include <ekutils/epoll_d.hpp>

namespace tlproxy {

struct gate {
	std::unique_ptr<ekutils::io_stream_d> stream;
	ekutils::expandbuff input = ekutils::expandbuff();

	void send();
	void copy_to(gate & other);
};

class portal {
	ekutils::epoll_d & epoll;
	gate src;
	gate dst;

public:
	portal(ekutils::epoll_d & selectable, std::unique_ptr<ekutils::io_stream_d> && source,
			std::unique_ptr<ekutils::io_stream_d> && dest);

private:
	void on_event(gate & actor, gate & other, std::uint32_t events);
};

}

#endif // PORTAL_HEAD_JTHWDFG233GD
