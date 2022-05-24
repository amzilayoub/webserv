/***********************************************************************
* FILENAME :        Client.cpp
*
* DESCRIPTION :
*       This file is to manipulate the reuqest by client
*
**/

# ifndef __CLIENT_H__
#  define __CLIENT_H__

#  include "../Exchange/Request/Request.hpp"
#  include "../Exchange/Response/Response.hpp"

namespace webserv
{

class Client
{

	/************************ MEMBER ATTRIBUTES ************************/
	private:
		int					_fd;
		int					_offset;
		int					_content_length;

	public:
		webserv::Request	req;
		webserv::Response	res;

	/************************ CONSTRUCTOR/DESTRUCTOR ************************/
	public:
		Client(void);
		Client(int fd);
		~Client();

	/************************ MEMBER FUNCTIONS ************************/
	public:
		void	handle_request(void);
		void	handle_response(void);

	/************************ MEMBER FUNCTIONS(ERROR HANDLING) ************************/
	public:
		bool	check_allowed_methods(void);

	/************************ GETTERS/SETTERS ************************/
	public:
		void	set_fd(int fd);
};

}

# endif