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
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/socket.h>

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
	enum { PORT = 80, BACKLOG = 16384, EV_LEN = 2048, BUF_LEN = 65535 };
	static const char* index_html = "HTTP/1.0 200 OK\r\n" \
									"Content-Length: 86\r\n\r\n" \
									"<!DOCTYPE html>" \
									"<html><head>Hello, world!</head><body><h1>cdn-ish...</h1></body></html>";

	static char buf[BUF_LEN] = { 0, };
	static struct kevent *ev_list;

	int fd, kqfd, serverfd, clientfd, ev_count, i, flags;
    struct kevent ev_set;
    struct sockaddr_in serveraddr, clientaddr;
    int clientaddr_size;
	
	serverfd = this->sock.getSocket();


	this->kq.create_event(this->sock.getSocket(), EVFILT_READ);
	kqfd = this->kq.get_kq();

    for (;;) {
        ev_count = this->kq.get_event();
		ev_list = this->kq.get_event_list();

        for (i = 0; i < ev_count; i++) {
            fd = this->kq.get_fd(i);

            if (fd < 0) continue;
            if (fd == serverfd) {
				
				clientaddr_size = sizeof(this->sock.getAddress());

                clientfd = accept(fd, (struct sockaddr*)&this->sock.getAddress(), (socklen_t *)&clientaddr_size);
                if (clientfd < 0) {
                    if (errno == EMFILE) {
                        continue;
                    }
                    perror("accept");
                    close(fd);
                    return ;
                }
				this->kq.create_event(clientfd, EVFILT_READ);
                if (fcntl(clientfd, F_SETFL, O_NONBLOCK) < 0) {
                    perror("fcntl");
                    close(clientfd);
                    close(fd);
                }
				this->kq.create_event(clientfd, EVFILT_WRITE, EV_ADD | EV_ONESHOT);
            } else if (this->kq.is_read_available(i)) {
                int len;
				std::cout << "read" << std::endl;
                // memset(buf, 0, sizeof(buf));
                if ((len = recv(fd, buf, sizeof(buf), 0)) != 0) {
                }
				close(fd);
            } else if (this->kq.is_write_available(i)) {
                int len = 0;
                if ((len = send(fd, index_html, strlen(index_html), 0)) != 0) {
                }
            }
        }
    }
}

// void	webserv::Server::lunch()
// {

// 	// Set the event set and associate it with the socket
// 	// and add the events to the kqueue
// 	this->kq.create_event(this->sock.getSocket(), EVFILT_READ);

// 	while(1)
// 		this->_lunch_worker();
// }

void	webserv::Server::_lunch_worker(void)
{
	int		n_ev;
	int		accept_sock;
	int		address_size;
	char	buff[1000];
	webserv::Header header;

	// register the events in ev_list
	std::cout << GREEN << "---- WAITING FOR CONNECTION ----" << RESET << std::endl;
	n_ev = this->kq.get_event();
	for (int i = 0; i < n_ev; i++)
	{
		if (this->kq.get_fd(i) < 0)
			continue;
		// When the client disconnects an EOF is sent
		// if (this->kq.isEOF(i))
		// {
		// 	std::cout << MAGENTA << "Client disconnected" << RESET << std::endl;
		// 	close(this->kq.get_fd(i));
		// }
		/*
		** If the new event's file descriptor is the same as the listening
		** socket's file descriptor, we are sure that a new client wants 
		** to connect to our socket.
		*/
		if (this->kq.get_fd(i) == this->sock.getSocket())
		{
			std::cout << "--- RECEIVED NEW CONNECTION ---" << std::endl;
			address_size = sizeof(this->sock.getAddress());
			accept_sock = accept(
					this->sock.getSocket(),
					(struct sockaddr*)&this->sock.getAddress(),
					(socklen_t *)&address_size
					);
			this->sock.test_error(accept_sock);
			this->kq.create_event(accept_sock, EVFILT_READ);
			int flags;
			if ((flags = fcntl(accept_sock, F_GETFL, 0)) < 0) {
				perror("fcntl");
				close(accept_sock);
				close(this->sock.getSocket());
            }
			if (fcntl(accept_sock, F_SETFL, flags | O_NONBLOCK) < 0) {
				perror("fcntl");
				close(accept_sock);
				close(this->sock.getSocket());
			}
			this->kq.create_event(accept_sock, EVFILT_WRITE, EV_ADD | EV_ONESHOT);
		}
		else if (this->kq.is_read_available(i))
		{
			int bytes_read;
			std::cout << "START: is_read_available" << std::endl;
			if ((bytes_read = recv(this->kq.get_fd(i), buff, 999, 0)) > 0)
			{
				// bytes_read = recv(this->kq.get_fd(i), buff, 50, 0);
				// std::cout << buff << std::endl;
				// buff[bytes_read] = '\0';
				// std::string l = buff;
				// if (!header.is_done())
				// 	header.parse(l);
				// std::cout << std::endl << "---- STRING OBJ ----" << std::endl << std::string(buff) << std::endl;
			}
			// while((bytes_read = recv(this->kq.get_fd(i), buff, 50, 0)) > 0)
			// {
			// 	// std::cout << buff << std::endl;
			// }
			// std::cout << "----------- end reading --------" << std::endl;
			// std::string hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
			// size_t ss = send(this->kq.get_fd(i), hello.c_str(), hello.length(), 0);
			// std::cout << "SS " << ss << std::endl;
			// close(this->kq.get_fd(i));
			// std::cout << "READ : " << this->kq.get_fd(i) << std::endl;
			// std::string hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
			// if (send(this->kq.get_fd(i), hello.c_str(), hello.length(), 0) != 0)
			// {
			// 	if (errno == EAGAIN)
			// 	{
			// 		std::cout << "TEST1" << std::endl;
			// 		this->kq.create_event(this->kq.get_fd(i), EVFILT_WRITE, EV_ADD | EV_ONESHOT);
			// 	}
			// 	else
			// 	{
			// 		std::cout << "TEST2" << std::endl;
			// 		this->kq.set_event(this->kq.get_fd(i), EVFILT_WRITE, EV_DELETE);
			// 		this->kq.get_event_list()[i].ident = -1;
			// 		close(this->kq.get_fd(i));
			// 	}
			// }
			// std::cout << "END: is_write_available" << std::endl;
		}
		else if (this->kq.is_write_available(i))
		{
			std::string hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
			if (send(this->kq.get_fd(i), hello.c_str(), hello.length(), 0) != 0)
			{
				// if (errno == EAGAIN)
				// {
				// 	std::cout << "TEST1" << std::endl;
				// 	this->kq.create_event(this->kq.get_fd(i), EVFILT_WRITE, EV_ADD | EV_ONESHOT);
				// }
				// else
				// {
					std::cout << "TEST2" << std::endl;
					this->kq.set_event(this->kq.get_fd(i), EVFILT_WRITE, EV_DELETE);
					this->kq.get_event_list()[i].ident = -1;
					close(this->kq.get_fd(i));
				// }
			}
			std::cout << "END: is_write_available" << std::endl;
		}
		// else
		// 	close(this->kq.get_fd(i));

	}
}

