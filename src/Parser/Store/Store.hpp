/***********************************************************************
* FILENAME :        Store.hpp
*
* DESCRIPTION :
*       This file will contains the attributes that
*		we will need in our Parser
*
**/

# ifndef __STORE__HPP__
#  define __STORE__HPP__

#  include <map>
#  include <string>

namespace webserv
{

class Store
{
	/************************ MEMBER ATTRIBUTES ************************/
	public:
		std::string	type;
		void		*value;

	/************************ CONSTRUCOTRS ************************/
	public:
		Store(void);
		~Store(void);
	
	/************************ CONSTRUCOTRS ************************/

};

}
# endif