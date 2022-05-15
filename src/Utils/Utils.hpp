/***********************************************************************
* FILENAME :        Utils.hpp
*
* DESCRIPTION :
*       here we're going to have some helper functions
*
**/



# ifndef __UTILS__HPP__
#  define __UTILS__HPP__

#  include <list>

namespace webserv
{
	std::list<std::string> *split(std::string const &str, std::string const &delimeter);
}


# endif