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

/************************ CONSTRUCTOR/DESTRUCTOR ************************/
webserv::Client::Client(void)
{
}
webserv::Client::Client(int fd) : _fd(fd), _offset(0)
{
}

webserv::Client::~Client(void)
{
	// close (this->_fd);
}

/************************ MEMBER FUNCTIONS ************************/
void	webserv::Client::handle_request()
{
	int len;
	char buf[1000];

	if ((len = recv(this->_fd, buf, 999, 0)) == 0) {
		close(this->_fd);
	}
	else if (len > 0)
	{
		buf[len] = '\0';
		std::string buffer = buf;

		this->req.parse(buffer);
	}
	if (this->req.get_header_obj().is_done())
	{
		if (this->req.get_headers().find("content-length") != this->req.get_headers().end())
		{
			this->_content_length = std::stoi(this->req.get_headers()["content-length"]);
			// std::cout << "CONTENT LENGTH = " << this->_content_length << std::endl;
		}
	}
}

void	webserv::Client::handle_response()
{
	int len = 0;
	static const char* index_html = "HTTP/1.0 200 OK\r\n" \
								"Content-Length: 86\r\n\r\n" \
								"<!DOCTYPE html>" \
								"<html><head>Hello, world!</head><body><h1>cdn-ish...</h1></body></html>\r\n";

	if ((len = send(this->_fd, index_html, strlen(index_html), 0)) != 0) {
	}
	this->req.clear();
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
		//set error
	}
	return (true);
}

/************************ GETTERS/SETTERS ************************/
void	webserv::Client::set_fd(int fd)
{
	this->_fd = fd;
}
