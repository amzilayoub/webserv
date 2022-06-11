/***********************************************************************
* FILENAME :        Socket.cpp
*
* DESCRIPTION :
*       This File is the implementation of the functions
*		Defined in Socket.hpp
*
**/

# include "./Socket.hpp"
# include "../../Logger/Logger.hpp"
# include <arpa/inet.h>

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
}

/************************ MEMBER FUNCTIONS ************************/
void	webserv::Socket::create_socket(int domain, int type, int protocol)
{
	int tmp;

	tmp = 1;
	this->_domain = domain;
	this->_type = type;
	this->_protocol = protocol;

	this->_socket = socket(domain, type, protocol);
	this->test_error(this->_socket, "create_socket functions");
	this->test_error(
		setsockopt(this->_socket, SOL_SOCKET, SO_REUSEADDR, &tmp, sizeof(int)),
		"Setsockopt function");
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
	this->test_error(ret, "bind_Socket function");
}

void	webserv::Socket::listen_socket(int backlog)
{
	this->_backlog = backlog;
	this->test_error(listen(this->_socket, backlog), "listten_socket function");
}

int		webserv::Socket::accept_socket()
{
	int	address_size;
	int client_fd;

	address_size = sizeof(this->_address);
	client_fd = accept(this->_socket, (sockaddr *)&this->_address, (socklen_t *)&address_size);
	this->test_error(client_fd, "accept_socket function");
	return (client_fd);
}

void	webserv::Socket::test_error(int fd, std::string const &str) const
{
	if (fd < 0)
		webserv::Logger::error(str);
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