/***********************************************************************
* FILENAME :        Request.cpp
*
* DESCRIPTION :
*       This File is the implementation of the functions
*		Defined in Request.hpp
*
**/

# include "./Request.hpp"
#  include <regex>

/************************ MEMBER ATTRIBUTES ************************/
webserv::Request::Request()
{
	this->_headers_done = false;
	this->_has_error = false;
	this->_header_special_char = "!#$%&'*+-.^_`|~";
}

/************************ MEMBER FUNCTIONS ************************/
void webserv::Request::parse(std::string &str)
{
	while (!this->_headers_done)
	{
		this->_header.parse(str);
		if (this->_header.is_done())
		{
			this->_headers_done = true;
			this->_body = this->_header.get_body();
		}
	}
	this->_body += str;
}

void webserv::Request::clear()
{
	this->_header.clear();
	this->_body.clear();
	this->_headers_done = false;
	this->_has_error = false;
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