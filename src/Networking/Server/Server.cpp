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

	int kq;

	// creating kqueue instance
	kq = kqueue();

	/*
	** Creating event list,
	** ev_list for the received list that a action happend
	** ev_set, is the list of action to monitor
	*/
	struct kevent ev_list[100], ev_set;

	// The return value of kqueue, means the number of fd that an event happend on
	int n_ev;

	// Set the event set and associate it with the socket
	EV_SET(&ev_set, this->sock.getSocket(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);

	// add the events to the kqueue
	this->sock.test_error(kevent(kq, &ev_set, 1, NULL, 0, NULL));
	while(1)
	{
		// register the events in ev_list
		n_ev = kevent(kq, NULL, 0, ev_list, 100, NULL);
		this->sock.test_error(n_ev);
		for (int i = 0; i < n_ev; i++)
		{
			// When the client disconnects an EOF is sent
			if (ev_list[i].flags & EV_EOF)
			{
				std::cout << "Client disconnected" << std::endl;
				close(ev_list[i].ident);
			}
			/*
			** If the new event's file descriptor is the same as the listening
            ** socket's file descriptor, we are sure that a new client wants 
            ** to connect to our socket.
			*/
			else if (ev_list[i].ident == this->sock.getSocket())
			{
				address_size = sizeof(this->sock.getAddress());
				std::cout << "--- SERVER STARTED ---" << std::endl;
				accept_sock = accept(
						this->sock.getSocket(),
						(struct sockaddr*)&this->sock.getAddress(),
						(socklen_t *)&address_size
						);
				this->sock.test_error(accept_sock);
				EV_SET(&ev_set, accept_sock, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
				this->sock.test_error(kevent(kq, &ev_set, 1, NULL, 0, NULL));


			}
			else if (ev_list[i].filter & EVFILT_READ)
			{
				size_t bytes_read = recv(ev_list[i].ident, buff, 1000, 0);
				std::cout << buff << std::endl;
				std::cout << "READ : " << bytes_read << std::endl;
			}

		}


	}
}