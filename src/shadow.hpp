#ifndef SHADOW_HEAD_BRFWWFV422FGF
#define SHADOW_HEAD_BRFWWFV422FGF

#include "portal.hpp"
#include "sock_specs.hpp"

namespace tlproxy {

struct listener_wrapper {
	virtual ekutils::descriptor & listener() = 0;
	virtual void listen() = 0;
	virtual std::unique_ptr<ekutils::io_stream_d> accept() = 0;
	virtual ~listener_wrapper() {}
};

struct connection_factory {
	virtual std::unique_ptr<ekutils::io_stream_d> connect() = 0;
	virtual ~connection_factory() {}
};

class shadow {
	ekutils::epoll_d epoll;
	std::unique_ptr<listener_wrapper> listener;
	std::unique_ptr<connection_factory> connections;
public:
	shadow(const sock_specs & src, const sock_specs & dst);
	void run();
private:
	void on_new_connection(std::uint32_t events);
};

} // namespace tlproxy

#endif
