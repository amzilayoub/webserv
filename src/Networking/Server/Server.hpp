/***********************************************************************
* FILENAME :        Server.hpp
*
* DESCRIPTION :
*       This file will contains a class Server,
*		and will use the socket class
*
**/

# ifndef __SERVER__HPP__
#  define __SERVER__HPP__

#  define __MAX_BACKLOG__ 128

#  include "../Socket/Socket.hpp"

namespace webserv
{

class Server
{
	/************************ CONSTRUCTORS/DESTRUCTOR ************************/
	public:
		Socket sock;

	/************************ CONSTRUCTORS/DESTRUCTOR ************************/
	public:
		Server(int addr, int port);
		~Server();
	
	/************************ CONSTRUCTORS/DESTRUCTOR ************************/
	public:
		void	lunch();
};

}

# endif