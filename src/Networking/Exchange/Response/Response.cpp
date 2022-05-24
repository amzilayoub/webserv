/***********************************************************************
* FILENAME :        Response.cpp
*
* DESCRIPTION :
*       This File is the implementation of the functions
*		Defined in Response.hpp
*
**/

# include "./Response.hpp"
# include "../HttpStatusCode.hpp"
// # include <stdlib.h>
/************************ MEMBER ATTRIBUTES ************************/
webserv::Response::Response()
{
	this->_header.protocol_version = "HTTP/1.0";

	this->status_code_list[BAD_REQUEST] = "Bad Request";
	this->_status_code = -1;
}

/************************ MEMBER FUNCTIONS ************************/
void	webserv::Response::error(int status_code)
{
	this->_status_code = std::to_string(status_code);
	this->_status_string = this->status_code_list[status_code];
	this->get_headers()["Content-Length"] = std::to_string(this->_status_string.length());
	this->_body = this->_status_string;
}

std::string	&webserv::Response::serialize(void)
{
	this->_response = this->_header.protocol_version + " ";
	this->_response += this->_status_code + " ";
	this->_response += this->_status_string + "\r\n";

	this->_response += this->_header.serialize();
	this->_response += this->_body + "\r\n";

	return (this->_response);
}

/************************ GETTERS/SETTERS ************************/
std::map<std::string, std::string>	&webserv::Response::get_headers(void)
{
	return (this->_header.get_headers());
}

std::string &webserv::Response::get_body()
{
	return (this->_body);
}