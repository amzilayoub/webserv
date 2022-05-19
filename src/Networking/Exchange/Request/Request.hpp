/***********************************************************************
* FILENAME :        Request.hpp
*
* DESCRIPTION :
*       Request class to handle the client request
*
**/


# ifndef __REQUEST__HPP__
#  define __REQUEST__HPP__

#  include <map>
#  include <string>
#  include "../Header/Header.hpp"

namespace webserv
{

class Request
{
	/************************ MEMBER TYPE ************************/
	public:
		typedef std::map<std::string, std::string>::iterator hr_iterator;

	/************************ MEMBER ATTRIBUTES ************************/
	private:
		webserv::Header _header;
		std::string		_body;
		bool			_headers_done;
	
	/************************ CONSTRUCTOR/DESTRUCTIR ************************/
	public:
		Request();
	
	/************************ MEMBER FUNCTIONS ************************/
	public:
		void parse(std::string &str);

	/************************ GETTERS/SETTERS ************************/
	std::map<std::string, std::string>	&get_headers(void);
	std::string &get_body();
};

};


# endif