/***********************************************************************
* FILENAME :        Client.cpp
*
* DESCRIPTION :
*       This file is to manipulate the reuqest by client
*
**/

# ifndef __CLIENT_H__
#  define __CLIENT_H__

#  include "../Exchange/Request/Request.hpp"
#  include "../Exchange/Response/Response.hpp"
#  include "../../Config/Config.hpp"

#  define __REQUEST_DONE__ 0
#  define __REQUEST_IN_PROGRESS__ 1
#  define __REQUEST_ERROR__ 2
#  define __MB_IN_BYTE__ 1048576
#  include <sys/stat.h>
namespace webserv
{

class Client
{

	/************************ MEMBER ATTRIBUTES ************************/
	private:
		int						_fd;
		int						_offset;
		int						_content_length;
		std::list<std::string>	_methods;
		std::string				_full_path;

	public:
		webserv::Request	req;
		webserv::Response	res;
		webserv::Config		config;

	/************************ CONSTRUCTOR/DESTRUCTOR ************************/
	public:
		Client(void);
		Client(int fd);
		~Client();

	/************************ MEMBER FUNCTIONS ************************/
	public:
		int			handle_request(void);
		bool		handle_response(void);
		std::string	get_file_type(std::string path);
	
	private:
		void		_fill_methods(void);
		std::string	_get_file_name(void);
		bool		_save_file(void);
		bool		_file_exists(char const *str);
		int			_handle_folder(void);
		std::string	_get_dir_html_tree();
		int			_post(void);
		int			_get(void);

	/************************ MEMBER FUNCTIONS(ERROR HANDLING) ************************/
	public:
		bool		check_allowed_methods(void);
		bool		check_resources_exists(void);
		bool		check_entity_length(void);
		bool		check_supported_media_type(void);

	/************************ GETTERS/SETTERS ************************/
	public:
		void			set_fd(int fd);
		void			set_config(webserv::Config &config);
		std::string		&get_full_path(void);
};

}

# endif