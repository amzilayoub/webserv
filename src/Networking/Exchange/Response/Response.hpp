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
#  include <fstream>

#  define __CHUNK_TO_READ__ 1024
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
		std::map<int, std::string> error_pages;

	private:
		webserv::Header _header;
		std::string		_body;
		std::string		_status_code;
		std::string		_status_string;
		std::string		_response;
		bool			_has_error;
		std::ifstream	_file;
		std::string		_content_type;
		bool			_start_chunked;
		bool			_is_done;
	
	
	/************************ CONSTRUCTOR/DESTRUCTIR ************************/
	public:
		Response();
	
	/************************ MEMBER FUNCTIONS ************************/
	public:
		void		error(int status_code);
		void		set_header(std::string key, std::string value);
		void		set_status(int status_code);
		void		clear();
		void		set_file(std::string path, std::string content_type);
		std::string	serialize(void);
		std::string	to_hexa(int number);
		bool		is_done(void);

	/************************ GETTERS/SETTERS ************************/
	std::map<std::string, std::string>	&get_headers(void);
	std::string &get_body();
	bool		has_error(void);
	bool		isEOF(void);
};

};


# endif