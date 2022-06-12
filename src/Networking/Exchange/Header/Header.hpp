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
		std::string							header_requested_path;
		std::string							query_string;
		std::string							protocol_version;
		std::string							path_info;

	private:
		bool								_is_done;
		std::string							_raw_string;
		std::map<std::string, std::string>	_headers;
		std::string							_body;
		size_t								_raw_header_len;

	/************************ CONSTRUCTOR/DESTRUCTIR ************************/
	public:
		Header(void);
	
	/************************ CONSTRUCTOR/DESTRUCTIR ************************/
	public:
		void		parse(std::string &str, int len);
		bool		is_done() const;
		std::string	serialize();
		void		clear();
		void		print();
	
	private:
		void	_get_headers(std::string &str);
		void	_remove_hash_id(void);
	
	/************************ GETTERS AND SETTERS ************************/
	public:
		std::string							&get_body();
		std::map<std::string, std::string>	&get_headers();
		size_t								&get_raw_header_len() const;
};

}

# endif