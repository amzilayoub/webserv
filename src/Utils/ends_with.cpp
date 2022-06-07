/***********************************************************************
* FILENAME :        ends_with.hpp
*
* DESCRIPTION :
*       A boring ends_with function
*
**/

# include "./Utils.hpp"
# include <string>

bool	webserv::ends_with(std::string const & value, std::string const & ending)
{
	size_t index;

	if (ending.length() > value.length())
		return (false);
	index = value.rfind(ending);
	if (index == std::string::npos)
		return (false);
	if ((value.length() - index) == ending.length())
		return (true);
	return (false);
}