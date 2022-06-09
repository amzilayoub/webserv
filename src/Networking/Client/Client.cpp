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
# include <fcntl.h>
# include <list>

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
		return (__REMOVE_CLIENT__);
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
		this->_url_decode();
		this->match_config();
		this->_content_length = 0;
		this->res.set_header("Host", this->req.config.host + ":" + std::to_string(this->req.config.port));
		this->res.set_header("Server", this->req.config.server_name);

		webserv::Request::hr_iterator it = this->req.get_headers().find("connection");
		if (it != this->req.get_headers().end())
			this->res.set_header("Connection", it->second);
		this->_full_path = this->get_full_path();
		if (this->_handle_redirection())
			return (__REQUEST_DONE__);
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
		if (this->check_for_cgi())
		{
			int ret;

			ret = this->handle_cgi();
			if (ret == __REQUEST_ERROR__ || ret == __REQUEST_DONE__)
				return (ret);
		}
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
	webserv::Store									default_serv;
	
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
	default_serv = possible_servers.front();
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
	if (possible_servers.size() == 0)
	{
		this->req.set_config(webserv::Store(default_serv));
		this->res.set_config(webserv::Store(default_serv));

	}
	else
	{
		this->req.set_config(webserv::Store(possible_servers.front()));
		this->res.set_config(webserv::Store(possible_servers.front()));
	}
	this->req.handle_location();
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
	int	type;

	webserv::Logger::info(std::string("GET: ") + this->req.get_header_obj().path);
	type = this->_get_path_type();

	if (type == __PATH_IS_DIR__)
		return (this->_handle_folder());
	else if (type == __PATH_IS_FILE__)
	{
		this->res.set_file(this->_full_path, this->get_file_type(this->_full_path));
		this->res.set_status(OK);
		return (__REQUEST_DONE__);
	}
	return (type);
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
	std::string index_file = this->_get_index_file();

	std::cout << "index_file = " << index_file << std::endl;
	if (index_file != "")
	{
		this->res.set_file(index_file, this->get_file_type(index_file));
		this->res.set_status(OK);
		return (__REQUEST_DONE__);
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

	this->_file_name = path_to_upload + "/" + filename + extension;
	myfile.open(this->_file_name, std::ios::binary | std::ios::out);
	if(!myfile.is_open())
	{
		this->res.error(INTERNAL_SERVER_ERROR);
		return (false);
	}
	myfile.write(this->req.get_body().c_str(), this->req.content_length);
	myfile.close();
	return 0;
}

int	webserv::Client::_open_file(std::string path_to_upload)
{
	char					filename[100];
	std::string extension = ".cgi";
	time_t					t = time(0);
	struct tm				*now = localtime(&t);
	int						myfile;
	std::string				full_name;

	strftime(filename, 100, "%Y-%m-%d-%H-%M-%S", now);

	full_name = path_to_upload + "/" + filename + extension;
	this->_cgi_output_file = full_name;
	myfile = open(full_name.c_str(), O_WRONLY | O_CREAT, 0666);
	if(myfile < 0)
	{
		this->res.error(INTERNAL_SERVER_ERROR);
		return (false);
	}
	return (myfile);
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
	std::string		used_html_path;

	page = DIR_LISTING_START;
	if (!this->req.html_path.empty())
		used_html_path = this->req.html_path;
	else
		used_html_path = this->req.get_header_obj().path;

	webserv::replace(page, "${title}", used_html_path);
	webserv::replace(page, "${title}", used_html_path);
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
				link = used_html_path + this->_add_slash(used_html_path) + name;
				strftime(date, 100, "%Y-%m-%d %H:%M:%S", localtime(&(attr.st_mtime)));
				if (std::string(ent->d_name) == "..")
				{
					std::string						path;
					std::string::reverse_iterator	it;
					std::string						current_dir;
				
					path = used_html_path;
					for (it = path.rbegin(); it != path.rend(); it++)
					{
						if ((*it) == '/' && !current_dir.empty())
							break ;
						current_dir = (*it) + current_dir;
					}
					webserv::replace_last(path, current_dir, "");
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
		closedir(dir);
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
	// std::cout << "TARGET = " << this->req.config.root + this->req.get_header_obj().path << std::endl; 
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

void		webserv::Client::_url_decode()
{
	std::map<std::string, int>::iterator	it;
	std::string								token;
	size_t									index;

	while ((index = this->req.get_header_obj().path.find("%")) != std::string::npos)
	{
		token = this->req.get_header_obj().path.substr(index, 3);

		it = this->config.url_encoding.find(token);
		if (it == this->config.url_encoding.end())
			webserv::Logger::error("This type of encoding is not supported");
		webserv::replace(
				this->req.get_header_obj().path,
				token,
				std::string(1, static_cast<char>(it->second)));
	}
}

bool		webserv::Client::_file_exists(char const *str)
{
	return (access(str, F_OK) != -1);
}

bool		webserv::Client::_check_for_read(char const *str)
{
	return (access(str, R_OK) != -1);
}

bool		webserv::Client::_handle_redirection(void)
{
	std::map<std::string, std::string>::iterator	it;
	std::string										path;

	path = this->req.get_header_obj().path;
	it = this->req.config.redirection.find(path);
	if (it != this->req.config.redirection.end())
	{
		this->res.set_one_shot(true);
		this->res.set_header("Location", it->second);
		this->res.set_status(MOVED_PERMANENTLY);
		return (true);
	}
	return (false);
}

std::string		webserv::Client::_add_slash(std::string const &str)
{
	if (str.back() == '/')
		return ("");
	return ("/");
}

int		webserv::Client::check_for_cgi(void)
{
	int type;

	if (this->req.config.cgi.extension.empty())
		return (false);
	type = this->_get_path_type();
	if (type == __PATH_IS_DIR__)
	{
		this->_cgi_file = this->_get_index_file(this->req.config.cgi.extension);
		if (this->_cgi_file == "")
			return (false);
		return (true);
	}
	else if (type == __PATH_IS_FILE__)
	{
		this->_cgi_file = this->_full_path;
		if (webserv::ends_with(this->req.get_header_obj().path, this->req.config.cgi.extension))
			return (true);
	}
	/*
	** try to check if it's a file + path_info
	*/
	size_t index;

	index = this->_full_path.find(this->req.config.cgi.extension);
	if (index != std::string::npos)
	{
		this->_cgi_file = this->_full_path.substr(0, index + this->req.config.cgi.extension.length());
		this->_cgi_path_info = this->_full_path.substr(index + this->req.config.cgi.extension.length());
		return (true);
	}
	return (false);
}

int		webserv::Client::handle_cgi(void)
{
	int fd;

	if (!this->_check_for_read(this->_file_name.c_str()))
	{
		this->res.set_one_shot(true);
		this->res.error(FORBIDDEN);
		return (__REQUEST_ERROR__);
	}
	fd = open(this->_file_name.c_str(), O_RDONLY);
	if (fd < 0)
	{
		this->res.set_one_shot(true);
		this->res.error(INTERNAL_SERVER_ERROR);
		return (__REQUEST_ERROR__);
	}
	return (this->execute_cgi(fd));
}

char	**webserv::Client::prepare_cgi_env()
{
	char											**arg;
	int												i;
	std::map<std::string, std::string>::iterator	it;
	std::string										file_without_root;
	std::list<std::string>							args_list;

	file_without_root = this->_cgi_file;
	webserv::replace(file_without_root, this->req.config.root, "");
	// need to change the size here later on
	i = -1;
	arg = (char**)malloc(sizeof(char*) * 20);
	args_list.push_back((std::string("DOCUMENT_ROOT=") + this->req.config.root));
	args_list.push_back((std::string("CONTENT_LENGTH=") + std::to_string(this->req.content_length)));

	it = this->req.get_headers().find("content-type");
	if (it != this->req.get_headers().end())
		args_list.push_back((std::string("CONTENT_TYPE=") + it->second));
	args_list.push_back(std::string("GATEWAY_INTERFACE=CGI/1.1"));
	args_list.push_back((std::string("PATH_INFO=") + this->_cgi_path_info));
	args_list.push_back((std::string("PATH_TRANSLATED=") + this->req.config.root + this->_cgi_path_info));
	args_list.push_back((std::string("QUERY_STRING=") + this->req.get_header_obj().query_string));
	args_list.push_back(std::string("REMOTE_ADDR=0.0.0.0"));
	args_list.push_back((std::string("REQUEST_METHOD=") + webserv::str_to_upper(this->req.get_header_obj().method)));
	args_list.push_back((std::string("REQUEST_URI=") + file_without_root + "?" + this->req.get_header_obj().query_string));
	args_list.push_back((std::string("SCRIPT_NAME=") + file_without_root));
	args_list.push_back((std::string("SERVER_NAME=") + this->req.config.host));
	args_list.push_back((std::string("SERVER_PORT=") + std::to_string(this->req.config.port)));
	args_list.push_back((std::string("SERVER_PROTOCOL=HTTP/1.1")));

	args_list.push_back((std::string("SCRIPT_FILENAME=") + this->_cgi_file));
	args_list.push_back((std::string("HTTP_HOST=127.0.0.1:") + std::to_string(this->req.config.port)));
	args_list.push_back((std::string("REDIRECT_STATUS=200")));

	std::list<std::string>::iterator it_l = args_list.begin();
	for (; it_l != args_list.end(); it_l++)
		arg[++i] = strdup(it_l->c_str());
	arg[++i] = NULL;
	return (arg);
}

int		webserv::Client::execute_cgi(int fd)
{
	pid_t	pid;
	int		status;
	char	**env;
	char	**arg;
	int		output;
	int		i;
	int		ret;

	i = -1;
	ret = __CANNOT_EXECUTE_CGI__;
	output = this->_open_file("/tmp/cgi");
	if (output < 0)
	{
		this->res.set_one_shot(true);
		this->res.error(INTERNAL_SERVER_ERROR);
		return (__REQUEST_ERROR__);
	}
	env = this->prepare_cgi_env();
	arg = (char**)malloc(sizeof(char*) * 3);
	arg[0] = const_cast<char*>(this->req.config.cgi.path.c_str());
	arg[1] = const_cast<char*>(this->_cgi_file.c_str());
	arg[2] = NULL;
	pid = fork();
	if (pid < 0)
	{
		this->res.set_one_shot(true);
		this->res.error(INTERNAL_SERVER_ERROR);
		return (__REQUEST_ERROR__);
	}
	else if (pid == 0)
	{
		dup2(fd, 0);
		dup2(output, 1);
		execve(arg[0], arg, env);
		exit(1);
	}
	else
	{
		waitpid(pid, &status, 0);
		while (env[++i])
			free(env[i]);
		free(env);
		free(arg);
		if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
			ret = this->handle_cgi_response();
		close(fd);
		close(output);
	}
	return (ret);
}

int	webserv::Client::handle_cgi_response(void)
{
	char					filename[100];
	time_t					t = time(0);
	struct tm				*now = localtime(&t);
	std::ofstream			myfile;
	webserv::Header			hr;
	char					buf[10000];
	int						ret;
	std::string				status_code;
	std::string				status_code_string;
	std::string				full_name;
	std::string				content_type;
	int						file_fd;

	strftime(filename, 100, "%Y-%m-%d-%H-%M-%S", now);
	file_fd = open(this->_cgi_output_file.c_str(), O_RDONLY, 0666);
	full_name = std::string("/tmp/") + filename + ".cgi";
	myfile.open(full_name, std::ios::binary | std::ios::out);
	if(!myfile.is_open())
	{
		this->res.error(INTERNAL_SERVER_ERROR);
		return (INTERNAL_SERVER_ERROR);
	}
	while ((ret = read(file_fd, buf, 9999)) > 0)
	{
		std::string tmp;

		tmp.append(buf, ret);
		/*
		** This is a little hack since I'm expected the first line to be something like: GET / HTTP/1.1
		*/
		if (!hr.is_done())
		{
			int start_index;
			int end_index;

			if (status_code.empty() && (start_index = tmp.find("Status: ")) != std::string::npos)
			{

				tmp.erase(start_index, strlen("Status: "));
				start_index = tmp.find(" ");
				if (start_index != std::string::npos)
				{
					end_index = tmp.find("\r\n", start_index);
					status_code_string = tmp.substr(start_index + 1, end_index - start_index - 1);
					status_code = tmp.substr(0, start_index);
				}
			}
			tmp.clear();
			tmp.append(buf, ret);
			if (hr.path.empty())
			{
				tmp = "tmp tmp tmp\r\n" + tmp;
				ret += 13;
			}
			hr.parse(tmp, ret);
		}
		else
			break;
	}
	if (!status_code.empty() && !status_code_string.empty())
		this->res.set_status_code(status_code, status_code_string);
	else
		this->res.set_status(OK);
	webserv::Response::hr_iterator it;

	it = hr.get_headers().begin();
	for (; it != hr.get_headers().end(); it++)
		this->res.get_headers()[it->first] = it->second;

	myfile.write(hr.get_body().c_str(), hr.get_body().length());
	myfile.write(buf, ret);
	while ((ret = read(file_fd, buf, 9999)) > 0)
		myfile.write(buf, ret);

	it = (hr.get_headers().find("content-type"));
	if (it != hr.get_headers().end())
		this->res.set_file(full_name, it->second);
	myfile.close();
	close(file_fd);
	return (__REQUEST_DONE__);
}

std::string	webserv::Client::_get_index_file(std::string extension)
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
			if (this->_check_for_read(filepath.c_str()))
			{
				if (extension == "" || webserv::ends_with((*it), extension))
					return (filepath);
			}
		}
	}
	return ("");
}

int	webserv::Client::_get_path_type()
{
	struct stat s;

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
			return (__PATH_IS_DIR__);
		/*
		** if file
		*/
		else if (S_ISREG(s.st_mode))
			return (__PATH_IS_FILE__);
	}
	this->res.error(INTERNAL_SERVER_ERROR);
	return (__REQUEST_ERROR__);
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