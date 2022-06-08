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

std::string	webserv::str_to_upper(std::string str)
{
	std::string tmp = str;

	std::string::iterator it = tmp.begin();

	for (; it != tmp.end(); it++)
		(*it) = std::toupper((*it));
	
	return (tmp);
}