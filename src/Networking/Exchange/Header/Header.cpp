/***********************************************************************
* FILENAME :        Header.cpp
*
* DESCRIPTION :
*       This File is the implementation of the functions
*		Defined in Header.hpp
*
**/

# include "./Header.hpp"
# include <sstream>
# include <iostream>
# include <cstring>
# include <list>
# include "../../../Utils/Utils.hpp"

webserv::Header::Header(void) : _is_done(false)
{
}

void	webserv::Header::parse(std::string &str)
{
	std::string			headers;
	size_t				pos;

	this->_raw_string += str;

	// std::cout << this->_raw_string << std::endl;
	pos = this->_raw_string.find("\r\n\r\n");
	if (pos == std::string::npos || this->_is_done)
		return ;

	headers = this->_raw_string.substr(0, pos);
	this->_body = this->_raw_string.substr(pos + 4);
	this->_get_headers(headers);
	this->_raw_string.clear();
}

void	webserv::Header::_get_headers(std::string &str)
{
	std::string							key;
	std::string							value;
	std::list<std::string>				*line;
	std::list<std::string>				*words;
	std::list<std::string>::iterator	it;
	std::list<std::string>::iterator	tmp_it;
	int i;

	words = webserv::split(str, "\n");
	line = webserv::split(words->front(), " ");
	it = line->begin();
	this->_method = (*it);
	this->_path = *(++it);
	this->_protocol_version = *(++it);
	
	delete line;

	it = words->begin();
	/*
	** Skipping the first line that was parsed previously
	** example of that line: GET / HTTP/1.1
	*/
	++it;

	std::cout << "this->_method: " << this->_method << std::endl;
	std::cout << "this->_path: " << this->_path << std::endl;
	std::cout << "this->_protocol_version: " << this->_protocol_version << std::endl;

	while (it != words->end())
	{
		std::string value;

		line = webserv::split((*it), ":");
		tmp_it = line->begin();

		/*
		** Skipping the key and concatenate the values
		*/
		++tmp_it;
		for (; tmp_it != line->end(); tmp_it++)
			value += (*tmp_it) + ":";
		value.pop_back();
		this->_headers[line->front()] = value.substr(1);
		++it;
		delete line;
	}
	this->_is_done = true;
	std::map<std::string, std::string>::iterator it1 = this->_headers.begin();
	for (; it1 != this->_headers.end(); it1++)
		std::cout << "key = " << it1->first << " ---- value = " << it1->second << std::endl;
	delete words;
}

bool	webserv::Header::is_done() const
{
	return (this->_is_done);
}

std::string &webserv::Header::get_method() const
{
	return (const_cast<std::string&>(this->_method));
}

std::string &webserv::Header::get_path() const
{
	return (const_cast<std::string&>(this->_path));
}

std::string &webserv::Header::get_protocol_version() const
{
	return (const_cast<std::string&>(this->_protocol_version));
}

std::string &webserv::Header::get_body()
{
	return (this->_body);
}