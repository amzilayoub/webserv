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

# include "../Networking.includes.hpp"

#  define __MAX_BACKLOG__ 128

namespace webserv
{

class Server
{
	/************************ MEMBER ATTRIBUTES ************************/
	public:
		Socket		sock;
		Kqueue		kq;

	/************************ CONSTRUCTORS/DESTRUCTOR ************************/
	public:
		Server(int addr, int port);
		~Server();
	
	/************************ MEMBER FUNCTIONS ************************/
	public:
		void	lunch();
	
	private:
		void	_lunch_worker(void);
};

}

# endif