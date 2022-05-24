
# include <iostream>
# include "./src/Webserv.includes.hpp"


int main()
{
	webserv::Config config;

	signal(SIGPIPE, SIG_IGN);
	config.parse("./src/ConfigFiles/default.conf");


	webserv::Server server(config);

	server.lunch();
}