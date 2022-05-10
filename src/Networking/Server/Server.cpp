/***********************************************************************
* FILENAME :        Server.cpp
*
* DESCRIPTION :
*       This File is the implementation of the functions
*		Defined in Server.hpp
*
**/

# include "./Server.hpp"

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
	int		accept_sock;
	size_t	address_size;
	char buff[1000];
	int		n_ev;

	// Set the event set and associate it with the socket
	this->kq.set_event(this->sock.getSocket());

	// add the events to the kqueue
	this->kq.add_event();
	while(1)
	{
		// register the events in ev_list
		n_ev = this->kq.get_event();
		for (int i = 0; i < n_ev; i++)
		{
			// When the client disconnects an EOF is sent
			if (this->kq.isEOF(i))
			{
				std::cout << "Client disconnected" << std::endl;
				close(this->kq.get_fd(i));
			}
			/*
			** If the new event's file descriptor is the same as the listening
            ** socket's file descriptor, we are sure that a new client wants 
            ** to connect to our socket.
			*/
			else if (this->kq.get_fd(i) == this->sock.getSocket())
			{
				address_size = sizeof(this->sock.getAddress());
				std::cout << "--- SERVER STARTED ---" << std::endl;
				accept_sock = accept(
						this->sock.getSocket(),
						(struct sockaddr*)&this->sock.getAddress(),
						(socklen_t *)&address_size
						);
				this->sock.test_error(accept_sock);
				// EV_SET(&ev_set, accept_sock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
				this->kq.set_event(accept_sock);

				// this->sock.test_error(kevent(kq, &ev_set, 1, NULL, 0, NULL));
				this->kq.add_event();

			}
			else if (this->kq.is_read_available(i))
			{
				size_t bytes_read = recv(this->kq.get_fd(i), buff, 1000, 0);
				std::cout << buff << std::endl;
				std::cout << "READ : " << bytes_read << std::endl;
			}
		}


	}
}
