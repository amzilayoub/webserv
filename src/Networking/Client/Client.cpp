/***********************************************************************
* FILENAME :        Client.cpp
*
* DESCRIPTION :
*       This File is the implementation of the functions
*		Defined in Client.hpp
*
**/

# include "./Client.hpp"
# include <unistd.h>
# include <sys/socket.h>
# include <iostream>
# include "../Exchange/HttpStatusCode.hpp"
# include <algorithm>
# include <fstream>
# include "../../Logger/Logger.hpp"
# include "../../Utils/Utils.hpp"
# include <dirent.h>
# define _POSIX_C_SOURCE
# include <stdio.h>
# include <ftw.h>


/************************ CONSTRUCTOR/DESTRUCTOR ************************/
webserv::Client::Client(void)
{
	this->_fill_methods();
}
webserv::Client::Client(int fd) : _fd(fd), _offset(0)
{
	this->_fill_methods();
	this->_set_port();
}

webserv::Client::~Client(void)
{
	close (this->_fd);
}

/************************ MEMBER FUNCTIONS ************************/
int	webserv::Client::handle_request()
{
	int len;
	char buf[__BYTE_TO_READ__];

	if ((len = recv(this->_fd, buf, __BYTE_TO_READ__ - 1, 0)) == 0) {
		webserv::Logger::warning("Client disconnected...");
		close(this->_fd);
	}
	else if (len > 0)
	{
		std::string buffer(buf, len);
		
		if (!this->req.parse(buffer, len))
		{
			this->res.error(BAD_REQUEST);
			return (__REQUEST_ERROR__);
		}
	}
	else if (len < 0)
	{
		this->res.error(INTERNAL_SERVER_ERROR);
		return (__REQUEST_ERROR__);
	}
	// std::cout << std::string(buf, len) << std::endl;
	if (this->req.is_done())
	{
		this->match_config();
		// std::cout << "REQ DONE" << std::endl;
		this->_content_length = 0;
		this->res.set_header("Host", this->req.config.host + ":" + std::to_string(this->req.config.port));
		this->res.set_header("Server", this->req.config.server_name);

		webserv::Request::hr_iterator it = this->req.get_headers().find("connection");
		if (it != this->req.get_headers().end())
			this->res.set_header("Connection", it->second);
		this->_full_path = this->get_full_path();
		if (this->req.get_headers().find("content-length") != this->req.get_headers().end())
			this->_content_length = std::stoi(this->req.get_headers()["content-length"]);
		if (!this->check_allowed_methods())
			return (__REQUEST_ERROR__);
		if (!this->check_resources_exists())
			return (__REQUEST_ERROR__);
		if (!this->check_entity_length())
			return (__REQUEST_ERROR__);
		if (!this->check_supported_media_type())
			return (__REQUEST_ERROR__);
		this->_save_file("/tmp");
		if (this->req.get_header_obj().method == "post")
			return (this->_post());
		else if (this->req.get_header_obj().method == "get")
			return (this->_get());
		else if (this->req.get_header_obj().method == "delete")
			return (this->_delete());
	}
	return (__REQUEST_IN_PROGRESS__);
}

void		webserv::Client::match_config()
{
	std::list<webserv::Store>						possible_servers;
	std::map<std::string, webserv::Store>::iterator	it;
	std::list<webserv::Store>::iterator				it_poss_serv;
	webserv::Response::hr_iterator					it_header;
	
	it = this->_servers_list.begin();
	/*
	** Gets all the servers that has the same targeted port
	*/
	for (; it != this->_servers_list.end(); it++)
	{
		std::string key;

		key = ":" + std::to_string(this->_port) + "_";
		if ((it->first.find(key) != std::string::npos) && it->second.port == this->_port)
			possible_servers.push_back(it->second);
	}
	
	/*
	** Then filter them by host
	*/
	it_header = this->req.get_headers().find("host");
	if (it_header != this->req.get_headers().end())
	{
		it_poss_serv = possible_servers.begin();
		for (; it_poss_serv != possible_servers.end(); it_poss_serv++)
		{

			if (it_header->second.find(":") != std::string::npos)
			{
				std::string str;

				str = it_poss_serv->host + ":" + std::to_string(it_poss_serv->port);
				if (str.find(it_header->second) == std::string::npos)
				{
					possible_servers.erase(it_poss_serv);
					it_poss_serv = possible_servers.begin();
					continue ;
				}
			}
		}
	}

	/*
	** Then filter them by server
	*/
	it_header = this->req.get_headers().find("server");
	if (it_header != this->req.get_headers().end())
	{
		it_poss_serv = possible_servers.begin();
		for (; it_poss_serv != possible_servers.end(); it_poss_serv++)
		{
			std::string server_name;

			server_name = it_poss_serv->server_name;
			if (webserv::str_to_lower(server_name) != webserv::str_to_lower(it_header->second))
			{
				possible_servers.erase(it_poss_serv);
				it_poss_serv = possible_servers.begin();
				continue ;
			}

		}
	}
	std::cout << "================ MATCHED SERVER ==============" << std::endl;
	possible_servers.front().print();
	std::cout << "================ MATCHED SERVER ==============" << std::endl;
	this->req.set_config(possible_servers.front());
}


int		webserv::Client::_post()
{
	webserv::Logger::info(std::string("POST: ") + this->req.get_header_obj().path);

	if (this->req.content_length > 0)
		this->_save_file(this->req.config.upload_path);
	this->res.set_one_shot(true);
	this->res.set_header("Location", this->req.get_header_obj().path);
	this->res.set_status(SEE_OTHER);
	return (__REQUEST_DONE__);
}

int		webserv::Client::_get()
{
	struct stat s;

	webserv::Logger::info(std::string("GET: ") + this->req.get_header_obj().path);

	if (lstat(this->_full_path.c_str(), &s) == 0)
	{
		/*
		** if directory
		*/
		if (!this->_check_for_read(this->_full_path.c_str()))
		{
			this->res.error(FORBIDDEN);
			return (__REQUEST_ERROR__);
		}
		else if (S_ISDIR(s.st_mode))
			return (this->_handle_folder());
		/*
		** if file
		*/
		else if (S_ISREG(s.st_mode))
		{
			this->res.set_file(this->_full_path, this->get_file_type(this->_full_path));
			this->res.set_status(OK);
			return (__REQUEST_DONE__);
		}
	} else
	{
		this->res.error(INTERNAL_SERVER_ERROR);
		return (__REQUEST_ERROR__);
	}
	// if (this->req.config.autoindex)

	return (__REQUEST_DONE__);
}

int		webserv::Client::_delete()
{
struct stat s;

	webserv::Logger::info(std::string("DELETE: ") + this->req.get_header_obj().path);

	if (lstat(this->_full_path.c_str(), &s) == 0)
	{
		/*
		** if directory
		*/
		if (!this->_check_for_read(this->_full_path.c_str()))
		{
			this->res.error(FORBIDDEN);
			return (__REQUEST_ERROR__);
		}
		// delete the whole folder
		else if (S_ISDIR(s.st_mode))
			return (this->_delete_folder());
		/*
		** if file
		*/
		else if (S_ISREG(s.st_mode))
			return (this->_delete_file(this->_full_path.c_str()));
	} else
	{
		this->res.error(INTERNAL_SERVER_ERROR);
		return (__REQUEST_ERROR__);
	}
	// if (this->req.config.autoindex)

	return (__REQUEST_DONE__);
}

int		webserv::Client::_delete_file(std::string path)
{
	if (!remove(path.c_str()))
	{
		this->res.set_one_shot(true);
		this->res.set_status(NO_CONTENT);
		return (__REQUEST_DONE__);
	}
	this->res.error(INTERNAL_SERVER_ERROR);
	return (__REQUEST_ERROR__);
}

int		webserv::Client::_delete_folder()
{
	DIR				*dir;
	std::string		path;

	path = this->_full_path;

	if (path.back() != '/')
		path += "/";


	if ((dir = opendir(path.c_str())) != NULL)
	{
		closedir(dir);
		if (nftw(path.c_str(), unlink_file, 64, FTW_DEPTH | FTW_PHYS))
		{
			this->res.error(INTERNAL_SERVER_ERROR);
			return (__REQUEST_ERROR__);
		}
	}
	else
	{
		this->res.error(FORBIDDEN);
		return (__REQUEST_ERROR__);
	}
	this->res.set_one_shot(true);
	this->res.set_status(NO_CONTENT);

	return (__REQUEST_DONE__);
}

int		webserv::Client::_handle_folder(void)
{
	std::string							path = this->_full_path;
	std::list<std::string>::iterator	it = this->req.config.index.begin();

	if (path.back() != '/')
		path += "/";
	for (; it != this->req.config.index.end(); it++)
	{
		std::string filepath = path + (*it);
		if (this->_file_exists(filepath.c_str()))
		{
			if (!this->_check_for_read(filepath.c_str()))
				continue ;
			this->res.set_file(filepath, this->get_file_type(filepath));
			this->res.set_status(OK);
			return (__REQUEST_DONE__);
		}
	}
	if (!this->req.config.autoindex)
	{
		this->res.error(FORBIDDEN);
		return (__REQUEST_ERROR__);
	}
	return (this->_get_dir_html_tree());
}

bool	webserv::Client::_save_file(std::string path_to_upload)
{
	char					filename[100];
	std::string extension = this->config.get_file_extension(this->req.get_headers()["content-type"]);
	time_t					t = time(0);
	struct tm				*now = localtime(&t);
	std::ofstream			myfile;

	strftime(filename, 100, "%Y-%m-%d-%H-%M-%S", now);

	this->_file_name = filename + extension;
	myfile.open(path_to_upload + "/" + this->_file_name, std::ios::binary | std::ios::out);
	if(!myfile.is_open())
	{
		this->res.error(INTERNAL_SERVER_ERROR);
		return (false);
	}
	myfile.write(this->req.get_body().c_str(), this->req.content_length);
	myfile.close();
	return 0;
return true;
}

int		webserv::Client::handle_response()
{
	int			len = 0;
	std::string	buf;
	int			action;
	static const char* index_html = "HTTP/1.0 405 OK\r\n" \
								"Content-Length: 22\r\n\r\n" \
								"405 Method not allowed\r\n";

	action = __RESPONSE_IN_PROGRESS__;
	buf = this->res.serialize();
	// std::cout << "RESPONSE = " << buf << std::endl;
	if ((len = send(this->_fd, buf.c_str(), buf.length(), 0)) != 0) {
	}
	if (this->res.is_done())
	{
		action = __RESPONSE_DONE__;
		webserv::Request::hr_iterator it = this->req.get_headers().find("connection");
		if (it != this->req.get_headers().end())
		{
			if (webserv::str_to_lower(it->second) != "keep-alive")
			{
				action = __REMOVE_CLIENT__;
				webserv::Logger::warning("Client disconnected...");
				close(this->_fd);
			}
		}
		this->req.clear();
		this->res.clear();
	}
	return (action);
}


void	webserv::Client::_fill_methods()
{
	this->_methods.push_back("get");
	this->_methods.push_back("post");
	this->_methods.push_back("delete");
}

int	webserv::Client::_get_dir_html_tree()
{
	std::string		page;
	std::string		row;
	DIR				*dir;
	struct dirent	*ent;

	page = DIR_LISTING_START;
	webserv::replace(page, "${title}", this->req.get_header_obj().path);
	if ((dir = opendir(this->_full_path.c_str())) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			struct stat attr;

			if (std::string(ent->d_name) == ".")
				continue ;
			if (lstat((this->_full_path + "/" + ent->d_name).c_str(), &attr) == 0)
			{
				char		date[100];
				std::string	name;
				std::string	link;

				row = DIR_LISTING_ROW;
				name = std::string(ent->d_name) + (S_ISDIR(attr.st_mode) ? "/" : "");
				link = name;
				strftime(date, 100, "%Y-%m-%d %H:%M:%S", localtime(&(attr.st_mtime)));
				if (std::string(ent->d_name) == "..")
				{
					std::string						path = this->req.get_header_obj().path;
					std::string::reverse_iterator	it = path.rbegin();
					std::string						current_dir;
				
					for (; it != path.rend(); it++)
					{
						if ((*it) == '/' && !current_dir.empty())
							break ;
						current_dir = (*it) + current_dir;
					}
					webserv::replace(path, current_dir, "");
					name = "../";
					link = path;
				}
				webserv::replace(row, "${name}", name);
				webserv::replace(row, "${link}", link);
				webserv::replace(row, "${date}", date);
				webserv::replace(row, "${size}", (S_ISDIR(attr.st_mode) ? "-" : std::to_string(attr.st_size)));
				page += row;
			}
		}
	}
	else
	{
		this->res.error(FORBIDDEN);
		return (__REQUEST_ERROR__);
	}
	page += DIR_LISTING_END;

	this->res.set_status(MULTIPLE_CHOICES);
	this->res.set_header("Content-Length", std::to_string(page.length()));
	this->res.set_one_shot(true);
	this->res.set_body(page);

	return (__REQUEST_DONE__);
}

std::string	webserv::Client::get_file_type(std::string path)
{
	std::string::reverse_iterator					it = path.rbegin();
	std::map<std::string, std::string>::iterator	map_it = this->config.mime_types.begin();
	std::string										extension;

	for (; it != path.rend(); it++)
	{
		extension = (*it) + extension;
		if ((*it) == '.')
			break;
	}
	for (; map_it != this->config.mime_types.end(); map_it++)
	{
		if (map_it->second == extension)
			return (map_it->first);
	}
	return (this->config.mime_types["default_type"]);
}

/************************ MEMBER FUNCTIONS(ERROR HANDLING) ************************/
bool	webserv::Client::check_allowed_methods()
{
	std::list<std::string>::iterator it = std::find(
											this->req.config.allow_methods.begin(),
											this->req.config.allow_methods.end(),
											this->req.get_header_obj().method);

	if (it == this->req.config.allow_methods.end())
	{
		it = std::find(
			this->_methods.begin(),
			this->_methods.end(),
			this->req.get_header_obj().method
		);

		/*
		** check if known method
		*/
		if (it != this->_methods.end())
		{
			this->res.error(METHOD_NOT_ALLOWED);
			return (false);
		}
		/*
		** Otherwise, not implemented
		*/
		this->res.error(METHOD_NOT_IMPLEMENTED);
		return (false);
	}
	// std::cout << "METHOD = " << (*it) << std::endl;
	return (true);
}

bool	webserv::Client::check_resources_exists()
{
	if (!this->_file_exists((this->req.config.root + this->req.get_header_obj().path).c_str()))
	{
		this->res.error(NOT_FOUND);
		return (false);
	}
	return (true);
}

bool	webserv::Client::check_entity_length()
{
	if ((this->_content_length / __MB_IN_BYTE__) > this->req.config.client_max_body_size)
	{
		this->res.error(PAYLOAD_TOO_LARGE);
		return (false);
	}
	return (true);
}

bool	webserv::Client::check_supported_media_type()
{
	webserv::Request::hr_iterator it = this->req.get_headers().find("content-type");
	if (it == this->req.get_headers().end() && this->req.content_length > 0)
	{
		this->res.error(BAD_REQUEST);
		return (false);
	}
	else if (this->req.config.upload_path.empty())
	{
		this->res.error(INTERNAL_SERVER_ERROR);
		return (false);
	}
	return (true);
}

bool		webserv::Client::_file_exists(char const *str)
{
	return (access(str, F_OK) != -1);
}

bool		webserv::Client::_check_for_read(char const *str)
{
	return (access(str, R_OK) != -1);
}
/************************ GETTERS/SETTERS ************************/
void	webserv::Client::set_fd(int fd)
{
	this->_fd = fd;
	this->_set_port();
}

void	webserv::Client::_set_port(void)
{
	socklen_t len = sizeof(this->_sin);

	if (getsockname(this->_fd, (struct sockaddr *)&this->_sin, &len) == -1)
		webserv::Logger::error("set_port function: getsockname");
	this->_port = ntohs(this->_sin.sin_port);
}

void	webserv::Client::set_config(webserv::Config &config, std::map<std::string, webserv::Store>& servers_list)
{
	this->config = config;
	this->_servers_list = servers_list;
}

std::string		&webserv::Client::get_full_path(void)
{
	this->_full_path = this->req.config.root;

	if (this->_full_path.back() == '/')
		this->_full_path.pop_back();
	this->_full_path += this->req.get_header_obj().path;
	
	return (this->_full_path);
}