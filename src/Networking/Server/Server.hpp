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

#  include "../Networking.includes.hpp"
#  include "../Exchange/Request/Request.hpp"
#  include "../Client/Client.hpp"
#  include <map>
#  include "../../Config/Config.hpp"

#  define __MAX_BACKLOG__ 128

namespace webserv
{

class Server
{
	/************************ MEMBER ATTRIBUTES ************************/
	public:
		Socket								sock;
		Kqueue								kq;
		std::map<int, webserv::Client>		clients;
		webserv::Config						&config;


	/************************ CONSTRUCTORS/DESTRUCTOR ************************/
	public:
		Server(webserv::Config &config);
		~Server();
	
	/************************ MEMBER FUNCTIONS ************************/
	public:
		void	lunch();
	
	private:
		void	_lunch_worker(webserv::Request &req);
};

}

# endif