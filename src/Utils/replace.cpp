/***********************************************************************
* FILENAME :        replace.hpp
*
* DESCRIPTION :
*       A boring replace function
*
**/

# include "./Utils.hpp"
# include <string>

bool	webserv::replace(std::string &str, std::string const target, std::string const to_replace)
{
	size_t	index;

	index = str.find(target);
	if (index == std::string::npos)
		return (false);
	str.replace(index, target.length(), to_replace);
	return (true);
}
