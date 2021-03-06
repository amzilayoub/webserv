
# include <iostream>
# include "./src/Webserv.includes.hpp"


int main(int argc, char **argv)
{
	std::string		config_file;
	webserv::Config	config;

	if (argc < 1 || argc > 2)
		webserv::Logger::warning("Number of argument is incorrect\nUSAGE: webserv config_file");
	else if (argc == 2)
		config_file = argv[1];
	else
		config_file = "./src/ConfigFiles/default.conf";

	signal(SIGPIPE, SIG_IGN);
	config.parse(config_file);

	webserv::Server server(config);
	server.lunch();
}