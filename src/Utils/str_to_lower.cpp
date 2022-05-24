/***********************************************************************
* FILENAME :        str_to_lower.hpp
*
* DESCRIPTION :
*       A boring str_to_lower function
*
**/

# include "./Utils.hpp"
# include <string>
# include <cctype>

std::string	&webserv::str_to_lower(std::string &str)
{
	std::string::iterator it = str.begin();

	for (; it != str.end(); it++)
		(*it) = std::tolower((*it));
	
	return (str);
}