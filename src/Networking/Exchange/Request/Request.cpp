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
# include <list>
# include "../../../Utils/Utils.hpp"

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
	this->html_path.clear();
}

void	webserv::Request::handle_location(void)
{
	std::list<Store>::iterator	it;
	webserv::Store				target;
	std::string					target_location;
	std::string					location;
	std::string					path;
	int							longest_macth;

	it = this->config.location_object.begin();
	longest_macth = 0;
	if (!this->_header.path.empty())
		while (webserv::replace(this->_header.path, "//", "/"));
	/*
	** assuming it's a directory
	*/
	path = this->_header.path + "/";
	for (; it != this->config.location_object.end(); it++)
	{
		location = it->location;
		if (location.back() != '/')
			location += "/";
		if (path.find(location) != std::string::npos)
		{
			/*
			** Checking if empty because in the previous check we did remove '/'
			** from the end of the location
			*/
			if (longest_macth > location.length() && !location.empty())
				continue ;
			// std::cout << location << std::endl;
			longest_macth = location.length();
			target = (*it);
			target_location = location;
		}
	}
	if (longest_macth)
	{
		this->html_path = this->_header.path;
		if (target_location.back() == '/')
			target_location.pop_back();
		webserv::replace(this->_header.path, target_location, "");
		this->config.attach_location(target);
		std::cout << "this->_header.path = " << this->_header.path << std::endl;
		std::cout << "target.root = " << target.root << std::endl;
		// std::cout << longest_macth << "|" << target.location << std::endl;
	}
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
	return (body_length == this->content_length || this->content_length == 0);
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