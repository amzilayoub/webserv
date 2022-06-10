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
typedef struct	s_cgi
{
	std::string				extension;
	std::string				path;
	std::list<std::string>	allow_methods;

}				t_cgi;

class Store
{
	/************************ MEMBER ATTRIBUTES ************************/
	public:
		std::string							host;
		unsigned int						port;
		std::string							root;
		std::string							server_name;
		std::list<std::string>				allow_methods;
		std::string							upload_path;
		std::list<std::string>				index;
		std::map<int, std::string>			error_page;
		bool								autoindex;
		bool								is_autoindex_set;
		unsigned int						client_max_body_size;
		std::map<std::string, std::string>	redirection;
		std::list<t_cgi>					cgi_list;
		t_cgi								cgi;
		std::string							location;
		std::list<Store>					location_object;

	/************************ CONSTRUCOTRS ************************/
	public:
		Store(void);
		Store(webserv::Store const &rhs);
		~Store(void);
	
	/************************ MEMBER FUNCTIONS ************************/
	void	print() const;
	void	clear();
	bool	check();
	bool	error(std::string error);
	void	attach_location(webserv::Store const &rhs);

	/************************ OPERATOR OVERLOAD ************************/
	webserv::Store &operator=(webserv::Store const &rhs);
};

}

# endif