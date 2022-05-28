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
		std::cout << "REQ DONE" << std::endl;
		if (this->req.get_headers().find("content-length") != this->req.get_headers().end())
			this->_content_length = std::stoi(this->req.get_headers()["content-length"]);
		if (!this->check_allowed_methods())
			return (__REQUEST_ERROR__);
		if (!this->check_resources_exists())
			return (__REQUEST_ERROR__);
		if (this->req.get_header_obj().method == "post")
		{
			if (!this->check_entity_length())
				return (__REQUEST_ERROR__);
			if (!this->check_supported_media_type())
				return (__REQUEST_ERROR__);
			this->_save_file();
			this->res.set_header("Location", this->req.get_header_obj().path);
			this->res.set_status(SEE_OTHER);
			return (__REQUEST_DONE__);
		}
		else if (this->req.get_header_obj().method == "get")
		{
			return (__REQUEST_DONE__);
		}
	}
	return (__REQUEST_IN_PROGRESS__);
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

void	webserv::Client::handle_response()
{
	int len = 0;
	static const char* index_html = "HTTP/1.0 405 OK\r\n" \
								"Content-Length: 22\r\n\r\n" \
								"405 Method not allowed\r\n";

	
	std::cout << "RESPONSE = " << this->res.serialize() << std::endl;
	if ((len = send(this->_fd, this->res.serialize().c_str(), this->res.serialize().length(), 0)) != 0) {
	}
	// if ((len = send(this->_fd, index_html, strlen(index_html), 0)) != 0) {
	// }
	this->req.clear();
	this->res.clear();
}


void	webserv::Client::_fill_methods()
{
	this->_methods.push_back("get");
	this->_methods.push_back("post");
	this->_methods.push_back("delete");
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
	return (true);
}

bool	webserv::Client::check_resources_exists()
{
	std::fstream fs;

	fs.open (this->req.config.root + this->req.get_header_obj().path, std::fstream::in);
	if (!fs.is_open())
	{
		this->res.error(NOT_FOUND);
		fs.close();
		return (false);
	}
	fs.close();
	return (true);
}

bool	webserv::Client::check_entity_length()
{
	if (this->_content_length > this->req.config.client_max_body_size)
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
	if (it == this->req.get_headers().end())
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

/************************ GETTERS/SETTERS ************************/
void	webserv::Client::set_fd(int fd)
{
	this->_fd = fd;
}

void	webserv::Client::set_config(webserv::Config &config)
{
	this->config = config;
}