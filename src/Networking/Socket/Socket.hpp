/***********************************************************************
* FILENAME :        Socket.hpp
*
* DESCRIPTION :
*       This file will contains a class socket,
*		that has some the attributes/functions that
*		we will need in our Socket
*
**/

# ifndef __SOCKET__HPP__
#  define __SOCKET__HPP__

#  include <cstdio>
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <cstring>
#  include <unistd.h>
#  include <cstdlib>
#  include <iostream>

namespace webserv
{

class Socket
{

	/************************ MEMBER ATTRIBUTES ************************/
	private:
		int					_domain;
		int					_type;
		int					_protocol;
		int					_socket;
		struct sockaddr_in	_address;
		int					_port;
		int					_backlog;

	/************************ CONSTRUCTORS/DESTRUCTOR ************************/
	public:
		Socket(int domain, int type, int protocol);
		~Socket();

	/************************ MEMBER FUNCTIONS ************************/
	public:
		void	bind_socket(int addr, int port);
		void	listen_socket(int backlog);
		void	lunch();
	
	private:
		void	_test_error(int fd) const;

};

};

# endif