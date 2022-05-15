/***********************************************************************
* FILENAME :        split.hpp
*
* DESCRIPTION :
*       A boring split functions
*
**/

# include "./Utils.hpp"
# include <string>

std::list<std::string> *webserv::split(std::string const &str, std::string const &delimeter)
{
	std::string				text(str);
	size_t					pos;
	std::list<std::string>	*words = new std::list<std::string>;

	while ((pos = text.find(delimeter)) != std::string::npos)
	{
		words->push_back(text.substr(0, pos));
		text.erase(0, pos + delimeter.length());
	}
	if (!text.empty())
		words->push_back(text);
	
	return (words);
}