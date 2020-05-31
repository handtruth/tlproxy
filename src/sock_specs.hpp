#ifndef SOCK_SPECS_HEAD_YTRDCVBVC
#define SOCK_SPECS_HEAD_YTRDCVBVC

#include <string>
#include <string_view>
#include <limits>
#include <stdexcept>

namespace tlproxy {

struct sock_specs {
	enum class sock_t {
		un, tcp
	} type;
	std::string address;
	std::string port;

	explicit sock_specs(const std::string & spec);

private:
	void set_type(const std::string_view & str);
};

} // namespace tlproxy

#endif // SOCK_SPECS_HEAD_YTRDCVBVC
