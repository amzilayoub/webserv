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

/************************ CONSTRUCTOR/DESTRUCTOR ************************/
webserv::Client::Client(void)
{
	this->_fill_methods();
}
webserv::Client::Client(int fd) : _fd(fd), _offset(0)
{
	this->_fill_methods();
}

webserv::Client::~Client(void)
{
	// close (this->_fd);
}

/************************ MEMBER FUNCTIONS ************************/
int	webserv::Client::handle_request()
{
	int len;
	char buf[1000];

	if ((len = recv(this->_fd, buf, 999, 0)) == 0) {
		close(this->_fd);
	}
	else if (len > 0)
	{
		std::string buffer(buf, len);
		
		// std::cout << "REQ =" << std::string(buf, 0, len) << std::endl;
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
	// std::cout << buf << std::endl;
	if (this->req.is_done())
	{
		// std::cout << "REQ DONE" << std::endl;
		this->_full_path = this->get_full_path();
		if (this->req.get_headers().find("content-length") != this->req.get_headers().end())
			this->_content_length = std::stoi(this->req.get_headers()["content-length"]);
		if (!this->check_allowed_methods())
			return (__REQUEST_ERROR__);
		if (!this->check_resources_exists())
			return (__REQUEST_ERROR__);
		if (this->req.get_header_obj().method == "post")
			return (this->_post());
		else if (this->req.get_header_obj().method == "get")
			return (this->_get());
	}
	return (__REQUEST_IN_PROGRESS__);
}

int		webserv::Client::_post()
{
	webserv::Logger::info(std::string("POST: ") + this->req.get_header_obj().path);

	if (!this->check_entity_length())
		return (__REQUEST_ERROR__);
	if (!this->check_supported_media_type())
		return (__REQUEST_ERROR__);
	if (this->req.content_length > 0)
		this->_save_file();
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
		if (S_ISDIR(s.st_mode))
			return (this->_handle_folder());
		/*
		** if file
		*/
		if (S_ISREG(s.st_mode))
		{
			if (!this->_check_for_read(this->_full_path.c_str()))
			{
				this->res.error(FORBIDDEN);
				return (__REQUEST_ERROR__);
			}
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

int		webserv::Client::_handle_folder(void)
{
	std::string							path = this->_full_path;
	std::list<std::string>::iterator	it = this->req.config.index.begin();
	bool								file_found = false;

	if (path.back() != '/')
		path.pop_back();
	for (; it != this->req.config.index.end(); it++)
	{
		std::string filepath = path + (*it);
		if (this->_file_exists(filepath.c_str()))
		{
			file_found = true;
			if (!this->_check_for_read(filepath.c_str()))
				continue ;
			this->res.set_file(filepath, this->get_file_type(filepath));
			this->res.set_status(OK);
			return (__REQUEST_DONE__);
		}
	}
	if (file_found && !this->config.config.front().autoindex)
	{
		this->res.error(FORBIDDEN);
		return (__REQUEST_ERROR__);
	}
	return (this->_get_dir_html_tree());
}

bool	webserv::Client::_save_file()
{
	char					filename[100];
	std::string extension = this->config.get_file_extension(this->req.get_headers()["content-type"]);
	time_t					t = time(0);
	struct tm				*now = localtime(&t);
	std::ofstream			myfile;

	strftime(filename, 100, "%Y-%m-%d-%H-%M-%S", now);
	std::cout << filename << std::endl;
	std::cout << extension << std::endl;

	myfile.open(this->req.config.upload_path + "/" + filename + extension, std::ios::binary | std::ios::out);
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

bool	webserv::Client::handle_response()
{
	int len = 0;
	std::string buf;
	static const char* index_html = "HTTP/1.0 405 OK\r\n" \
								"Content-Length: 22\r\n\r\n" \
								"405 Method not allowed\r\n";

	
	buf = this->res.serialize();
	if ((len = send(this->_fd, buf.c_str(), buf.length(), 0)) != 0) {
	}
	if (this->res.is_done())
	{
		webserv::Request::hr_iterator it = this->req.get_headers().find("connection");
		if (it != this->req.get_headers().end())
		{
			if (webserv::str_to_lower(it->second) != "Keep-Alive")
				close(this->_fd);
		}
		this->req.clear();
		this->res.clear();
		return (true);
	}
	return (false);
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
					std::cout << "current_dir " << current_dir << std::endl;
					std::cout << "path " << path << std::endl;
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

	std::cout << this->_content_length / __MB_IN_BYTE__ << " " << this->req.config.client_max_body_size << std::endl;
	if ((this->_content_length / __MB_IN_BYTE__) > this->req.config.client_max_body_size)
	{
		this->res.error(PAYLOAD_TOO_LARGE);
		return (false);
	}
	return (true);
}

bool	webserv::Client::check_supported_media_type()
{
	// this->req.get_header_obj().print();
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
}

void	webserv::Client::set_config(webserv::Config &config)
{
	this->config = config;
}

std::string		&webserv::Client::get_full_path(void)
{
	this->_full_path = this->req.config.root;

	if (this->_full_path.back() == '/')
		this->_full_path.pop_back();
	this->_full_path += this->req.get_header_obj().path;
	
	return (this->_full_path);
}