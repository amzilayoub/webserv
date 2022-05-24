/***********************************************************************
* FILENAME :        Response.hpp
*
* DESCRIPTION :
*       Response class to handle the client Response
*
**/


# ifndef __RESPONSE__HPP__
#  define __RESPONSE__HPP__

#  include <map>
#  include <string>
#  include "../Header/Header.hpp"

namespace webserv
{

class Response
{
	/************************ MEMBER TYPE ************************/
	public:
		typedef std::map<std::string, std::string>::iterator hr_iterator;

	/************************ MEMBER ATTRIBUTES ************************/
	public:
		std::map<int, std::string> status_code_list;

	private:
		webserv::Header _header;
		std::string		_body;
		std::string		_status_code;
		std::string		_status_string;
		std::string		_response;
	
	
	/************************ CONSTRUCTOR/DESTRUCTIR ************************/
	public:
		Response();
	
	/************************ MEMBER FUNCTIONS ************************/
	public:
		void		error(int status_code);
		std::string	&serialize(void);

	/************************ GETTERS/SETTERS ************************/
	std::map<std::string, std::string>	&get_headers(void);
	std::string &get_body();
};

};


# endif