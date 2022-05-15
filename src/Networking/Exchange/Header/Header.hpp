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
	private:
		std::string							_method;
		std::string							_path;
		std::string							_protocol_version;
		std::map<std::string, std::string>	_headers;
		bool								_is_done;
		std::string							_body;
		std::string							_raw_string;

	/************************ CONSTRUCTOR/DESTRUCTIR ************************/
	public:
		Header(void);
	
	/************************ CONSTRUCTOR/DESTRUCTIR ************************/
	public:
		void	parse(std::string &str);
		bool	is_done() const;
	
	private:
		void	_get_headers(std::string &str);
	
	/************************ GETTERS AND SETTERS ************************/
	std::string &get_method() const;
	std::string &get_path() const;
	std::string &get_protocol_version() const;
	std::string &get_body();
};

}

# endif