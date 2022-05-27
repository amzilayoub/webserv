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
	this->_header.protocol_version = "HTTP/1.1";

	this->status_code_list[BAD_REQUEST] = "Bad Request";
	this->status_code_list[METHOD_NOT_ALLOWED] = "Method Not Allowed";
	this->status_code_list[METHOD_NOT_IMPLEMENTED] = "Not Implemented";
	this->status_code_list[NOT_FOUND] = "Not Found";
	this->status_code_list[PAYLOAD_TOO_LARGE] = "Payload Too Large";
	this->status_code_list[UNSUPPORTED_MEDIA_TYPE] = "Unsupported Media Type";
	this->status_code_list[SEE_OTHER] = "See Other";
	this->status_code_list[INTERNAL_SERVER_ERROR] = "Internal Server Error";

	this->error_pages[BAD_REQUEST] = std::to_string(METHOD_NOT_ALLOWED) + std::string(" Bad Request");
	this->error_pages[METHOD_NOT_ALLOWED] = std::to_string(METHOD_NOT_ALLOWED) + std::string(" Method Not Allowed");
	this->error_pages[METHOD_NOT_IMPLEMENTED] = std::to_string(METHOD_NOT_IMPLEMENTED) + std::string(" Not Implemented");
	this->error_pages[NOT_FOUND] = std::to_string(NOT_FOUND) + std::string(" Not Found");
	this->error_pages[PAYLOAD_TOO_LARGE] = std::to_string(PAYLOAD_TOO_LARGE) + std::string(" Payload Too Large");
	this->error_pages[UNSUPPORTED_MEDIA_TYPE] = std::to_string(UNSUPPORTED_MEDIA_TYPE) + std::string(" Unsupported Media Type");
	this->error_pages[SEE_OTHER] = std::to_string(SEE_OTHER) + std::string(" See Other");
	this->error_pages[INTERNAL_SERVER_ERROR] = std::to_string(INTERNAL_SERVER_ERROR) + std::string(" Internal Server Error");
	this->_status_code = -1;
}

/************************ MEMBER FUNCTIONS ************************/
void	webserv::Response::error(int status_code)
{
	this->set_status(status_code);
	this->get_headers()["Content-Length"] = std::to_string(this->error_pages[status_code].length());
	this->_body = this->error_pages[status_code];
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

void webserv::Response::clear()
{
	this->_header.clear();
	this->_body.clear();
	this->_status_code.clear();
	this->_status_string.clear();
	this->_response.clear();
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

void	webserv::Response::set_header(std::string key, std::string value)
{
	this->_header.get_headers()[key] = value;
}

void	webserv::Response::set_status(int status_code)
{
	this->_status_code = std::to_string(status_code);
	this->_status_string = this->status_code_list[status_code];
}