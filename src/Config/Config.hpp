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

namespace webserv
{

class	Config
{
	/************************ MEMBER ATTRIBUTES ************************/
	public:
		std::list<Store> config;

	/************************ CONSTRUCOTRS ************************/
	public:
		Config(void);

	/************************ MEMBER FUNCTIONS ************************/
	public:
		void	parse(std::string const &path);
};

}

# endif