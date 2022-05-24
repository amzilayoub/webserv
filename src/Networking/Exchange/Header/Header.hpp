/***********************************************************************
* FILENAME :        Header.hpp
*
* DESCRIPTION :
*       This file will contains a class Header,
*		it will contains all the method to
*		manipulate the header file
*
**/

# ifndef __HEADER__HPP__
#  define __HEADER__HPP__

#  include <map>
#  include <string>

namespace webserv
{

class Header
{
	/************************ CONSTRUCTOR/DESTRUCTIR ************************/
	public:
		std::string							method;
		std::string							path;
		std::string							protocol_version;

	private:
		bool								_is_done;
		std::string							_raw_string;
		std::map<std::string, std::string>	_headers;
		std::string							_body;

	/************************ CONSTRUCTOR/DESTRUCTIR ************************/
	public:
		Header(void);
	
	/************************ CONSTRUCTOR/DESTRUCTIR ************************/
	public:
		void		parse(std::string &str);
		bool		is_done() const;
		std::string	serialize();
		void		clear();
	
	private:
		void	_get_headers(std::string &str);
	
	/************************ GETTERS AND SETTERS ************************/
	public:
		std::string							&get_body();
		std::map<std::string, std::string>	&get_headers();
};

}

# endif