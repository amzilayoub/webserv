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
#  include <sys/stat.h>
#  include <netinet/in.h>

#  define __REQUEST_DONE__ 0
#  define __REQUEST_IN_PROGRESS__ 1
#  define __REQUEST_ERROR__ 2
#  define __CANNOT_EXECUTE_CGI__ 5

#  define __RESPONSE_DONE__ __REQUEST_DONE__
#  define __RESPONSE_IN_PROGRESS__ __REQUEST_IN_PROGRESS__
#  define __REMOVE_CLIENT__ 3

#  define __PATH_IS_DIR__ 1
#  define __PATH_IS_FILE__ 2

#  define __MB_IN_BYTE__ 1048576
#  define __BYTE_TO_READ__ 1024

#  define DIR_LISTING_START "<!DOCTYPE html>\
								<html>\
								<head>\
									<title>${title}</title>\
								</head>\
								<body>\
									<h1>Index of ${title}</h1>\
									<hr />\
									<section>\
									<table style=\"width: 100%\">"

#  define DIR_LISTING_END "</table>\
							</section>\
							<hr />\
						</body>\
						</html>"

#  define DIR_LISTING_ROW "<tr>\
							<td style=\"width: 60%\"><a href=\"${link}\">${name}</a></td>\
							<td style=\"width: 20%\">${date}</td>\
							<td style=\"width: 20%\">${size}</td>\
							</tr>"
namespace webserv
{

class Client
{

	/************************ MEMBER ATTRIBUTES ************************/
	private:
		int										_fd;
		int										_offset;
		int										_content_length;
		std::list<std::string>					_methods;
		std::string								_full_path;
		std::string								_file_name;
		int										_port;
		struct sockaddr_in						_sin;
		std::map<std::string, webserv::Store>	_servers_list;
		std::string								_cgi_file;
		std::string								_cgi_path_info;

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
		int			handle_response(void);
		std::string	get_file_type(std::string path);
		void		match_config(void);
	
	private:
		void		_fill_methods(void);
		std::string	_get_file_name(void);
		bool		_save_file(std::string path_to_upload);
		int			_open_file(std::string path_to_upload);
		bool		_file_exists(char const *str);
		bool		_check_for_read(char const *str);
		int			_handle_folder(void);
		int			_get_dir_html_tree();
		int			_delete_folder();
		int			_delete_file(std::string path);
		int			_post(void);
		int			_get(void);
		int			_delete(void);
		void		_url_decode();
		bool		_handle_redirection();
		int			_get_path_type();
		std::string _get_index_file(std::string extension = "");
		std::string	_add_slash(std::string const &str);

	/************************ MEMBER FUNCTIONS(ERROR HANDLING) ************************/
	public:
		bool		check_allowed_methods(void);
		bool		check_resources_exists(void);
		bool		check_entity_length(void);
		bool		check_supported_media_type(void);
		int			check_for_cgi();
		int			handle_cgi();
		char		**prepare_cgi_env();
		int			execute_cgi(int fd);
		int			handle_cgi_response(int fd);

	/************************ GETTERS/SETTERS ************************/
	public:
		void			set_fd(int fd);
		void			set_config(webserv::Config &config, std::map<std::string, webserv::Store> &servers_list);
		std::string		&get_full_path(void);

	private:
		void			_set_port(void);
};

}

# endif