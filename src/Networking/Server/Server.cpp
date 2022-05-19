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

/************************ CONSTRUCTORS/DESTRUCTOR ************************/
webserv::Server::Server(int addr, int port)
{
	this->sock.create_socket(AF_INET, SOCK_STREAM, 0);
	this->sock.bind_socket(addr, port);
	this->sock.listen_socket(__MAX_BACKLOG__);
}

webserv::Server::~Server() {}

/************************ MEMBER FUNCTION ************************/
void	webserv::Server::lunch()
{
	webserv::Request req;
	
	this->kq.create_event(this->sock.getSocket(), EVFILT_READ);
    while (1)
		this->_lunch_worker(req);
}

void	webserv::Server::_lunch_worker(webserv::Request &req)
{
	int ev_count = this->kq.get_event();
	static const char* index_html = "HTTP/1.0 200 OK\r\n" \
									"Content-Length: 86\r\n\r\n" \
									"<!DOCTYPE html>" \
									"<html><head>Hello, world!</head><body><h1>cdn-ish...</h1></body></html>\r\n";
	char buf[10000];

	for (int i = 0; i < ev_count; i++) {
		int fd = this->kq.get_fd(i);

		if (fd < 0) continue;
		if (fd == this->sock.getSocket()) {
			int clientfd = this->sock.accept_socket();

			webserv::Logger::info("New connection arrived");
			this->kq.test_error(clientfd, "Lunch worker");
			this->kq.create_event(clientfd, EVFILT_READ);
			if (fcntl(clientfd, F_SETFL, O_NONBLOCK) < 0) {
				perror("fcntl");
				close(clientfd);
				close(fd);
			}
		} else if (this->kq.is_read_available(i)) {
			int len;
			if ((len = recv(fd, buf, sizeof(buf), 0)) == 0) {
				close(fd);
			}
			else if (len > 0)
			{
				std::string buffer = buf;

				req.parse(buffer);
				std::cout << buffer << std::endl;
				std::cout << "XXXXXXXXXXXXXXXX" << std::endl;

				webserv::Request::hr_iterator it = req.get_headers().begin();
				for (; it != req.get_headers().end(); it++)
					std::cout << (*it).first << " : " << (*it).second << std::endl;
				
				// std::cout << 

			}
			this->kq.create_event(fd, EVFILT_WRITE, EV_ADD | EV_ONESHOT);
		} else if (this->kq.is_write_available(i)) {
			int len = 0;
			if ((len = send(fd, index_html, strlen(index_html), 0)) != 0) {
			}
			close(fd);
		}
	}
}
