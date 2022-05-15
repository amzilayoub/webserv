
# include <iostream>
# include "./src/Webserv.includes.hpp"


int main()
{
	// webserv::Config config;
	// webserv::Socket sock(AF_INET, SOCK_STREAM, 0);
	
	// sock.bind_socket(INADDR_ANY, 80);
	// sock.listen_socket(10);
	// sock.lunch();
	// config.parse("./src/ConfigFiles/default.conf");

	signal(SIGPIPE, SIG_IGN);
	webserv::Server server(INADDR_ANY, 8080);

	server.lunch();

}