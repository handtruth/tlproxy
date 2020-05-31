#include <iostream>
#include <typeinfo>

#include <ekutils/log.hpp>
#include <signal.h>

#include "config.hpp"
#include "shadow.hpp"

using namespace tlproxy;

int main(int argc, char *argv[]) {
	std::cout
		<< "group: " << config::group << std::endl
		<< "project: " << config::project << std::endl
		<< "version: " << config::version << std::endl
		<< "maintainer: " << config::maintainer << std::endl
		<< "build: " << config::build << std::endl
		<< "-------------------------------------------" << std::endl;
	if (argc != 3) {
		std::cerr << "required exactly 2 arguments" << std::endl;
		return 1;
	}
	ekutils::log = new ekutils::stdout_log(ekutils::log_level::debug);
	try {
		signal(SIGPIPE, SIG_IGN);
		sock_specs src(argv[1]);
		sock_specs dst(argv[2]);
		shadow server(src, dst);
		server.run();
	} catch (const std::exception & e) {
		log_error(typeid(e).name() + std::string(e.what()));
	}
}
