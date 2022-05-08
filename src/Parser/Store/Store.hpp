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
#  include <list>
#  include <string>

namespace webserv
{
class Store
{
	/************************ MEMBER ATTRIBUTES ************************/
	public:
		std::string							host;
		unsigned int						port;
		std::string							root;
		std::list<std::string>				allow_methods;
		std::string							upload_path;
		std::list<std::string>				index;
		std::map<int, std::string>			error_page;
		bool								autoindex;
		unsigned int						client_max_body_size;
		std::map<std::string, std::string>	redirection;
		std::string							location;
		std::list<Store>					location_object;

	/************************ CONSTRUCOTRS ************************/
	public:
		Store(void);
		~Store(void);
	
	/************************ MEMBER FUNCTIONS ************************/
	void	print() const;
	void	clear();
};

}

# endif