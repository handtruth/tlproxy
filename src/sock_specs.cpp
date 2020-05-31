#include "sock_specs.hpp"

namespace tlproxy {

sock_specs::sock_specs(const std::string & spec) {
	auto i = spec.find(':');
	if (i == std::numeric_limits<std::size_t>::max()) {
		set_type(spec);
		return;
	}
	set_type(std::string_view(spec.c_str(), i));
	++i;
	auto j = spec.find(':', i);
	if (j == std::numeric_limits<std::size_t>::max()) {
		address = spec.substr(i);
		return;
	}
	address = spec.substr(i, j - i);
	port = spec.substr(j + 1);
}

void sock_specs::set_type(const std::string_view & str) {
	if (str == "tcp")
		type = sock_t::tcp;
	else if (str == "unix")
		type = sock_t::un;
	else
		throw std::runtime_error("unsupported socket type: " + std::string(str));
}

} // namespace tlproxy
