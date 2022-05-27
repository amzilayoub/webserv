/***********************************************************************
* FILENAME :        Server.cpp
*
* DESCRIPTION :
*       This File is the implementation of the functions
*		Defined in Server.hpp
*
**/

# include "./Server.hpp"
# include "../../OutputColors.hpp"
# include <fcntl.h>
# include <errno.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <fcntl.h>
# include <unistd.h>
# include <arpa/inet.h>
# include <sys/types.h>
# include <sys/event.h>
# include <sys/time.h>
# include <sys/socket.h>
# include "../../Logger/Logger.hpp"
# include <fstream>

/************************ CONSTRUCTORS/DESTRUCTOR ************************/
webserv::Server::Server(webserv::Config &config) : config(config)
{
	this->sock.create_socket(AF_INET, SOCK_STREAM, 0);
	this->sock.bind_socket(inet_addr(config.config.front().host.c_str()), config.config.front().port);
	this->sock.listen_socket(__MAX_BACKLOG__);
}

webserv::Server::~Server() {}

/************************ MEMBER FUNCTION ************************/
void	webserv::Server::lunch()
{
	webserv::Request req;
	
	this->kq.create_event(this->sock.getSocket(), EVFILT_READ);
	webserv::Logger::info("Starting webserv worker");
    while (1)
		this->_lunch_worker(req);
}

void	webserv::Server::_lunch_worker(webserv::Request &req)
{
	int ev_count = this->kq.get_event();

	for (int i = 0; i < ev_count; i++) {
		int fd = this->kq.get_fd(i);

		if (fd < 0) continue;
		if (fd == this->sock.getSocket()) {
			int clientfd = this->sock.accept_socket();

			webserv::Logger::info("New connection arrived");
			this->kq.test_error(clientfd, "Lunch worker");
			this->kq.create_event(clientfd, EVFILT_READ);
			if (fcntl(clientfd, F_SETFL, O_NONBLOCK) < 0)
			{
				perror("fcntl");
				close(clientfd);
				close(fd);
			}
			this->clients[clientfd].set_fd(clientfd);
			this->clients[clientfd].req.set_config(this->config.config.front());
			this->clients[clientfd].set_config(this->config);
		} else if (this->kq.is_read_available(i))
		{
			int ret;

			webserv::Logger::info("Request delivered !");
			ret = this->clients[fd].handle_request();
			std::cout << "RET = " << ret << std::endl;
			if (ret == __REQUEST_ERROR__ || ret == __REQUEST_DONE__)
				this->kq.create_event(fd, EVFILT_WRITE, EV_ADD | EV_ONESHOT);
		}
		else if (this->kq.is_write_available(i))
		{
			webserv::Logger::info("Sending response !");
			this->clients[fd].handle_response();
			close(fd);
		}
	}
}
