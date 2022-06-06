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
# include <algorithm>

/************************ CONSTRUCTORS/DESTRUCTOR ************************/
webserv::Server::Server(webserv::Config &config) : config(config)
{
	std::list<Store>::iterator it = this->config.config.begin();

	for (; it != this->config.config.end(); it++)
	{
		webserv::Socket									tmp_sock;
		std::string										virt_serv;
		std::map<std::string, webserv::Store>::iterator	virt_it;
		std::list<int>::iterator						port_it;

		virt_serv = it->host + ":" + std::to_string(it->port) + "_" + it->server_name;
		
		virt_it = this->virt_serv_config.find(virt_serv);
		port_it = std::find(this->used_ports.begin(), this->used_ports.end(), it->port);
		/*
		** In case we have same host and port and server name not set,
		** We will use the default one (the first one)
		*/
		if (virt_it == this->virt_serv_config.end())
			this->virt_serv_config[virt_serv] = (*it);
		if (port_it != this->used_ports.end())
			continue ;
		tmp_sock.create_socket(AF_INET, SOCK_STREAM, 0);
		tmp_sock.bind_socket(INADDR_ANY, it->port);
		tmp_sock.listen_socket(__MAX_BACKLOG__);
		this->used_ports.push_back(it->port);
		this->sockets.push_back(tmp_sock);
	}
}

webserv::Server::~Server() {}

/************************ MEMBER FUNCTION ************************/
void	webserv::Server::lunch()
{
	std::list<webserv::Socket>::iterator	it;

	for (it = this->sockets.begin(); it != this->sockets.end(); it++)
		this->kq.create_event(it->getSocket(), EVFILT_READ);
	webserv::Logger::info("Starting webserv worker");
    while (1)
		this->_lunch_worker();
}

void	webserv::Server::_lunch_worker()
{
	int										ev_count;
	std::list<webserv::Socket>::iterator	it;

	webserv::Logger::info("Waiting for event....");
	ev_count = this->kq.get_event();
	for (int i = 0; i < ev_count; i++) {
		int fd = this->kq.get_fd(i);

		it = this->sockets.begin();

		if (fd < 0) continue;
		for (; it != this->sockets.end(); it++)
		{
			if (fd == it->getSocket()) {
				int clientfd = it->accept_socket();

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
				this->clients[clientfd].res.set_config(this->config.config.front());
				this->clients[clientfd].set_config(this->config, this->virt_serv_config);
				continue ;
			}
		}
		if (this->kq.is_read_available(i))
		{
			int ret;

			webserv::Logger::info("Request delivered !");
			ret = this->clients[fd].handle_request();
			// std::cout << "RET = " << ret << std::endl;
			if (ret == __REQUEST_ERROR__ || ret == __REQUEST_DONE__)
				this->kq.create_event(fd, EVFILT_WRITE, EV_ADD);
			else if (ret == __REMOVE_CLIENT__)
				this->clients.erase(fd);

		}
		else if (this->kq.is_write_available(i))
		{
			int ret;

			webserv::Logger::info("Sending response !");
			ret = this->clients[fd].handle_response();
			if (ret == __REMOVE_CLIENT__)
				this->clients.erase(fd);
			if (ret == __RESPONSE_DONE__)
				this->kq.create_event(fd, EVFILT_WRITE, EV_DELETE);
		}
	}
}
