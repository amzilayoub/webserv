/***********************************************************************
* FILENAME :        Config.hpp
*
* DESCRIPTION :
*       This file includes all the config file of the server
*
**/

# ifndef __CONFIG__HPP__
#  define __CONFIG__HPP__

#  include "../Parser/Parser.includes.hpp"
#  include <string>
#  include <list>
#  include <iostream>
#  include <map>

namespace webserv
{

class	Config
{
	/************************ MEMBER ATTRIBUTES ************************/
	public:
		std::list<Store> config;
		std::map<std::string, std::string> mime_types;
		std::map<std::string, int> url_encoding;

	/************************ CONSTRUCOTRS ************************/
	public:
		Config(void);

	/************************ MEMBER FUNCTIONS ************************/
	public:
		void		parse(std::string const &path);
		void		get_mime_types_list(void);
		void		get_url_encoding(void);
		std::string	get_file_extension(std::string &content_type);
};

}

# endif