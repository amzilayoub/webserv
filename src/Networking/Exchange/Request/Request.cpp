/***********************************************************************
* FILENAME :        Request.cpp
*
* DESCRIPTION :
*       This File is the implementation of the functions
*		Defined in Request.hpp
*
**/

# include "./Request.hpp"
# include <regex>
# include <iostream>

/************************ MEMBER ATTRIBUTES ************************/
webserv::Request::Request()
{
	this->_headers_done = false;
	this->_has_error = false;
	this->_header_special_char = "!#$%&'*+-.^_`|~";
	this->content_length = 0;
	this->_request_length = 0;
}

/************************ MEMBER FUNCTIONS ************************/
bool webserv::Request::parse(std::string &str, int len)
{
	this->_request_length += len;
	while (!this->_headers_done)
	{
		this->_header.parse(str, len);
		if (this->_header.is_done())
		{
			this->_headers_done = true;
			this->_body = this->_header.get_body();
			// std::cout << "get_body" << this->_body << std::endl;
			std::map<std::string, std::string>::iterator it = this->_header.get_headers().find("content-length");
			if (it == this->_header.get_headers().end())
				return true;
			this->content_length = std::stoi(it->second);
			return (true);
		}
	}
	this->_body += std::string(str, 0, len);
	// std::cout << str << std::endl;
	return (true);
}

void webserv::Request::clear()
{
	this->_header.clear();
	this->_body.clear();
	this->_headers_done = false;
	this->_has_error = false;
	this->_request_length = 0;
	this->content_length = 0;
}

bool	webserv::Request::is_done(void)
{
	// std::cout << "BODY LENGTH = " << this->_request_length << std::endl;
	// std::cout << "content_length = " << this->content_length << std::endl;
	// std::cout << "this->get_header_obj().method = " << this->_header.method << std::endl;
	/*
	**the -4 is for \r\n\r\n
	*/
	size_t body_length = this->_request_length - this->get_header_obj().get_raw_header_len() - 4;
	return ((body_length) == this->content_length || this->content_length == 0);
}

/************************ GETTERS/SETTERS ************************/
std::map<std::string, std::string>	&webserv::Request::get_headers(void)
{
	return (this->_header.get_headers());
}

std::string &webserv::Request::get_body()
{
	return (this->_body);
}

bool	webserv::Request::header_is_done()
{
	return (this->_headers_done);
}

webserv::Header &webserv::Request::get_header_obj(void)
{
	return (this->_header);
}

void	webserv::Request::set_config(webserv::Store config)
{
	this->config = config;
}

void	webserv::Request::add_request_length(int size)
{
	this->_request_length += size;
}