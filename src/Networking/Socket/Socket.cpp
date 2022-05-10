/***********************************************************************
* FILENAME :        Socket.cpp
*
* DESCRIPTION :
*       This File is the implementation of the functions
*		Defined in Socket.hpp
*
**/

# include "./Socket.hpp"

/************************ CONSTRUCTORS/DESTRUCTOR ************************/
webserv::Socket::Socket(int domain, int type, int protocol) :
						_domain(domain), _type(type), _protocol(protocol)
{
	this->create_socket(domain, type, protocol);
}

webserv::Socket::Socket()
{
}

webserv::Socket::~Socket()
{
	close(this->_socket);
}

/************************ MEMBER FUNCTIONS ************************/
void	webserv::Socket::create_socket(int domain, int type, int protocol)
{
	this->_domain = domain;
	this->_type = type;
	this->_protocol = protocol;

	this->_socket = socket(domain, type, protocol);
	this->test_error(this->_socket);
}

void	webserv::Socket::bind_socket(int addr, int port)
{
	int ret;

	this->_port = port;
	this->_address.sin_family = this->_domain;
	this->_address.sin_addr.s_addr = addr;
	this->_address.sin_port = htons(port);

	memset(this->_address.sin_zero, 0, sizeof(this->_address.sin_zero));
	ret = bind(this->_socket, (struct sockaddr *)&this->_address, sizeof(this->_address));
	this->test_error(ret);
}

void	webserv::Socket::listen_socket(int backlog)
{
	this->_backlog = backlog;
	this->test_error(listen(this->_socket, backlog));
}

// This function to be removed later on
void	webserv::Socket::lunch()
{
	// int		accept_sock;
	// size_t	address_size;
	// char buff[1000];

	// while(1)
	// {
	// 	address_size = sizeof(this->_address);
	// 	std::cout << "ACCEPT" << std::endl;
	// 	accept_sock = accept(
	// 			this->_socket,
	// 			(struct sockaddr*)&this->_address,
	// 			(socklen_t *)&address_size
	// 			);
	// 	this->test_error(accept_sock);
	// 	read(accept_sock, buff, 1000);
	// 	printf("%s\n", buff);
	// 	close(accept_sock);

	// }
}

void	webserv::Socket::test_error(int fd) const
{
	if (fd < 0)
	{
		perror("The following error occured: ");
		exit(EXIT_FAILURE);
	}
}

/************************ GETTERS/SETTERS ************************/
int					&webserv::Socket::getSocket()
{
	return (this->_socket);
}

struct sockaddr_in	&webserv::Socket::getAddress()
{
	return (this->_address);
}