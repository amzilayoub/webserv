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
# include <iostream>
# include <sstream>
# include "../../../Logger/Logger.hpp"

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
	this->status_code_list[MULTIPLE_CHOICES] = "Multiple Choices";
	this->status_code_list[OK] = "OK";
	this->status_code_list[FORBIDDEN] = "Forbidden";
	this->status_code_list[NO_CONTENT] = "No Content";
	this->status_code_list[MOVED_PERMANENTLY] = "Moved Permanently";

	this->error_pages[BAD_REQUEST] = std::to_string(METHOD_NOT_ALLOWED) + std::string(" Bad Request");
	this->error_pages[METHOD_NOT_ALLOWED] = std::to_string(METHOD_NOT_ALLOWED) + std::string(" Method Not Allowed");
	this->error_pages[METHOD_NOT_IMPLEMENTED] = std::to_string(METHOD_NOT_IMPLEMENTED) + std::string(" Not Implemented");
	this->error_pages[NOT_FOUND] = std::to_string(NOT_FOUND) + std::string(" Not Found");
	this->error_pages[PAYLOAD_TOO_LARGE] = std::to_string(PAYLOAD_TOO_LARGE) + std::string(" Payload Too Large");
	this->error_pages[UNSUPPORTED_MEDIA_TYPE] = std::to_string(UNSUPPORTED_MEDIA_TYPE) + std::string(" Unsupported Media Type");
	this->error_pages[SEE_OTHER] = std::to_string(SEE_OTHER) + std::string(" See Other");
	this->error_pages[INTERNAL_SERVER_ERROR] = std::to_string(INTERNAL_SERVER_ERROR) + std::string(" Internal Server Error");
	this->error_pages[MULTIPLE_CHOICES] = std::to_string(MULTIPLE_CHOICES) + std::string(" Multiple Choices");
	this->error_pages[OK] = std::to_string(OK) + std::string(" OK");
	this->error_pages[FORBIDDEN] = std::to_string(FORBIDDEN) + std::string(" Forbidden");
	this->error_pages[NO_CONTENT] = std::to_string(NO_CONTENT) + std::string(" No Content");
	this->error_pages[MOVED_PERMANENTLY] = std::to_string(MOVED_PERMANENTLY) + std::string(" Moved Permanently");

	this->_status_code = -1;
	this->_has_error = false;
	this->_start_chunked = false;
	this->_is_done = true;
	this->_one_shot = false;
}

/************************ MEMBER FUNCTIONS ************************/
void	webserv::Response::error(int status_code)
{
	std::string error_content;

	error_content = this->get_error_file_content(status_code);
	if (error_content != "")
		this->error_pages[status_code] = error_content;
	this->set_status(status_code);
	this->get_headers()["Content-Length"] = std::to_string(this->error_pages[status_code].length());
	this->_body = this->error_pages[status_code];
	this->_has_error = true;
}

std::string	webserv::Response::serialize(void)
{
	this->_response = this->_header.protocol_version + " ";
	this->_response += this->_status_code + " ";
	this->_response += this->_status_string + "\r\n";

	if (this->_has_error || this->_one_shot)
	{
		this->_response += this->_header.serialize();
		this->_response += this->_body;
	}
	else if (this->_start_chunked)
	{
		char buf[__CHUNK_TO_READ__ + 1];

		this->_response.clear();
		this->_file.read(buf ,__CHUNK_TO_READ__);

		buf[this->_file.gcount()] = 0;
		this->_response = this->to_hexa(this->_file.gcount()) + "\r\n";
		// this->_response += std::string(buf, 0, this->_file.gcount());
		this->_response.append(buf, this->_file.gcount()); // += std::string(buf, 0, this->_file.gcount());
		this->_response += std::string("\r\n", 0, 2);

		if (this->_file.gcount() == 0)
		{
			this->_is_done = true;
			this->_start_chunked = false;
		}
	}
	else
	{
		this->set_header("Content-Type", this->_content_type);
		this->set_header("Transfer-Encoding", "chunked");
		this->_response += this->_header.serialize();
		this->_start_chunked = true;
		this->_is_done = false;
	}
	// std::cout << this->_response << std::endl;
	return (this->_response);
}

void webserv::Response::clear()
{
	this->_header.clear();
	this->_body.clear();
	this->_status_code.clear();
	this->_status_string.clear();
	this->_response.clear();
	this->_has_error = false;
	if (this->_file.is_open())
		this->_file.close();
	this->_start_chunked = false;
	this->_is_done = true;
	this->_one_shot = false;
}

void		webserv::Response::set_file(std::string path, std::string content_type)
{
	this->_file.open(path.c_str(), std::ios::binary);

	if (!this->_file.is_open())
	{
		this->error(INTERNAL_SERVER_ERROR);
		return ;
	}
	this->_content_type = content_type;
}

std::string	webserv::Response::to_hexa(int number)
{
	std::stringstream stream;

	stream << std::hex << number;
	return (std::string(stream.str()));
}

bool	webserv::Response::isEOF(void)
{
	return (this->_file.peek() == EOF);
}

bool	webserv::Response::is_done(void)
{
	return (this->_is_done);
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

void		webserv::Response::set_body(std::string str)
{
	this->_body = str;
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

bool	webserv::Response::has_error(void)
{
	return (this->_has_error);
}

void		webserv::Response::set_one_shot(bool value)
{
	this->_one_shot = value;
}

void	webserv::Response::set_config(webserv::Store config)
{
	this->config = config;
}

void	webserv::Response::set_status_code(std::string status_code, std::string status_string)
{
	this->_status_code = status_code;
	this->_status_string = status_string;
}

std::string	webserv::Response::get_error_file_content(int status_code)
{
	std::map<int, std::string>::iterator	it;
	std::ifstream							ifs;
	std::string								content;

	it = this->config.error_page.find(status_code);
	if (it == this->config.error_page.end())
		return ("");
	ifs.open(it->second);
	if (ifs.is_open())
	{
		std::string line;
		
		while (std::getline(ifs, line))
			content += line;
		return (content);
	}
	return ("");
}

