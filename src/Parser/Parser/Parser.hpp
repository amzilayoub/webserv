/***********************************************************************
* FILENAME :        Parser.hpp
*
* DESCRIPTION :
*       This file will contains all the functions definition
*       That we will need to Parse our config file
*
*
* NOTE :
*		We will use Recursive descent implementation
*
**/

# ifndef __PARSER__HPP__
#  define __PARSER__HPP__

#  include <map>
namespace webserv
{

class Parser
{
	/************************ MEMBER ATTRIBUTES ************************/
	// std::map<std::string, std::map<std::string, void*> > _map

	/************************ CONSTRUCOTRS ************************/
	public:
		Parser(void);
		~Parser(void);
	
	/************************ CONSTRUCOTRS ************************/
	public:
		void	parse(std::string &str);

};

}
# endif