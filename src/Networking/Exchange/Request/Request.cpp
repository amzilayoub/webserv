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
# include <sstream>
# include "../../../Utils/Utils.hpp"
# include "../../../Logger/Logger.hpp"

/************************ MEMBER ATTRIBUTES ************************/
webserv::Request::Request()
{
	this->_headers_done = false;
	this->_has_error = false;
	this->_header_special_char = "!#$%&'*+-.^_`|~";
	this->content_length = 0;
	this->_request_length = 0;
	this->_is_chunked = false;
	this->_chunk_left_to_read = -1;
	this->_body.clear();
	this->_transfer_encoding.clear();
}

/************************ MEMBER FUNCTIONS ************************/
bool webserv::Request::parse(std::string &str, int len)
{
	this->_request_length += len;
	if (!this->_headers_done)
	{
		this->_header.parse(str, len);
		if (this->_header.is_done())
		{
			this->_headers_done = true;
			// std::cout << "get_body" << this->_body << std::endl;
			std::map<std::string, std::string>::iterator it = this->_header.get_headers().find("content-length");
			if (it != this->_header.get_headers().end())
			{
				this->_body = this->_header.get_body();
				this->content_length = std::stoi(it->second);
				return (true);
			}
			it = this->_header.get_headers().find("transfer-encoding");
			if (it != this->_header.get_headers().end() && it->second == "chunked")
			{
				this->_transfer_encoding = "chunked";
				this->_is_chunked = true;
				this->handle_chunked_request(this->_header.get_body(), this->_header.get_body().length());
			}
			return true;
		}
		else
			return (true);
	}
	if (this->_is_chunked)
		return (this->handle_chunked_request(str, len));
	else
		this->_body += std::string(str, 0, len);
	return (true);
}

bool webserv::Request::handle_chunked_request(std::string &str, int len)
{
	if (str.rfind("0\r\n\r\n") == std::string::npos)
	{
		this->_body += std::string(str, 0, len);
		return (true);
	}
	this->_body += std::string(str, 0, len);
	int			last_index = 0;

	while (1)
	{
		size_t		index;
		int			to_read;
		std::string	key;
		std::string tmp;

		index = this->_body.find("\r\n", last_index);
		to_read = this->to_int(this->_body.substr(last_index, index));
		if (to_read == -1)
			return (false);
		tmp = this->_body.substr(last_index, index);
		this->content_length += to_read;
		if (to_read == 0)
		{
			this->_is_chunked = false;
			this->_body.erase(last_index);
			return (true);
		}
		this->_body.erase(last_index, (index - last_index) + 2);
		last_index += to_read;
		this->_body.erase(last_index, 2);
	}





	// std::string body;
	// size_t		index;

	// if (this->_chunk_left_to_read == -1)
	// {
	// 	index = str.find("\r\n");
	// 	if (index != std::string::npos)
	// 	{
	// 		this->_chunk_left_to_read = this->to_int(str.substr(0, index));
	// 		this->content_length += this->_chunk_left_to_read;
	// 		if (!this->_chunk_left_to_read)
	// 		{
	// 			std::cout << "CHUNCKED IS DONE" << std::endl;
	// 			this->_is_chunked = false;
	// 			return (true);
	// 		}
	// 		str.erase(0, index + 2);
	// 		len -= (index + 2);
	// 	}
	// 	else
	// 		return (false);
	// }
	// if (this->_chunk_left_to_read >= len)
	// {
	// 	// this->_body.append(str.substr(0, len), len);
	// 	this->_body += std::string(str.substr(0, len), 0, len);
	// 	this->_chunk_left_to_read -= len;
	// }
	// else if (this->_chunk_left_to_read < len)
	// {
	// 	this->_body += std::string(str.substr(0, this->_chunk_left_to_read), 0, this->_chunk_left_to_read);
	// 	// this->_body.append(str.substr(0, this->_chunk_left_to_read), this->_chunk_left_to_read);
	// 	/*
	// 	** -2 or +2 for \r\n
	// 	*/
	// 	len -= this->_chunk_left_to_read - 2;
	// 	str.erase(0, this->_chunk_left_to_read + 2);
	// 	this->_chunk_left_to_read = -1;
	// 	return (this->handle_chunked_request(str, len));
	// }
	// return (true);
}

void webserv::Request::clear()
{
	this->_header.clear();
	this->_body.clear();
	this->_headers_done = false;
	this->_has_error = false;
	this->_is_chunked = false;
	this->_request_length = 0;
	this->content_length = 0;
	this->html_path.clear();
	this->_chunk_left_to_read = -1;
	this->_transfer_encoding.clear();
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

int	webserv::Request::to_int(std::string str)
{
	try
	{
		int					val;   
		std::stringstream	ss;

		ss << std::hex << str;
		ss >> val;

		return (val);
	}
	catch(const std::exception& e)
	{
	}
	webserv::Logger::error("to_int functions");
	return (-1);
}

bool	webserv::Request::is_done(void)
{
	// std::cout << "BODY LENGTH = " << this->_request_length << std::endl;
	// std::cout << "content_length = " << this->content_length << std::endl;
	// std::cout << "this->get_header_obj().method = " << this->_header.method << std::endl;
	/*
	**the -4 is for \r\n\r\n
	*/
	if (!this->_header.is_done())
		return (false);

	size_t body_length = this->_request_length - this->get_header_obj().get_raw_header_len() - 4;
	
	if (this->_transfer_encoding == "chunked")
		return (!this->_is_chunked);	
	return ((body_length == this->content_length || this->content_length == 0));
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