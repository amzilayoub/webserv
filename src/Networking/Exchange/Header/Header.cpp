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

webserv::Header::Header(void) : _is_done(false), _raw_header_len(0)
{
}

void	webserv::Header::parse(std::string &str, int len)
{
	std::string			headers;
	size_t				pos;

	this->_raw_string += std::string(str, 0, len);

	pos = this->_raw_string.find("\r\n\r\n");
	if (pos == std::string::npos || this->_is_done)
		return ;

	headers = this->_raw_string.substr(0, pos);
	this->_raw_header_len = headers.length();
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
	size_t 								index;
	int									i;

	words = webserv::split(str, "\r\n");
	line = webserv::split(words->front(), " ");
	it = line->begin();
	this->method = webserv::str_to_lower((*it));
	this->path = *(++it);
	this->protocol_version = *(++it);

	index = this->path.find("?");
	if (index != std::string::npos)
		this->query_string = this->path.substr(index + 1);
	this->_remove_hash_id();

	delete line;

	it = words->begin();
	/*
	** Skipping the first line that was parsed previously
	** example of that line: GET / HTTP/1.1
	*/
	++it;

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
		this->_headers[webserv::str_to_lower(line->front())] = value.substr(1);
		++it;
		delete line;
	}
	this->_is_done = true;
	delete words;
}

std::string	webserv::Header::serialize()
{
	std::map<std::string, std::string>::iterator	it;
	std::string										raw;

	it = this->_headers.begin();
	for (; it != this->_headers.end(); it++)
		raw += it->first + ": " + it->second + "\r\n";
	raw += "\r\n";

	return (raw);
}

void	webserv::Header::clear()
{
	this->method.clear();
	this->path.clear();
	this->_headers.clear();
	this->_body.clear();
	this->query_string.clear();
	this->_is_done = false;
}

void	webserv::Header::_remove_hash_id()
{
	size_t hash_index;
	size_t query_string_index;

	hash_index = this->path.find("#");
	if (hash_index != std::string::npos)
	{
		query_string_index = this->path.find("?");
		if (query_string_index == std::string::npos)
			this->path.erase(hash_index);
		else if (query_string_index >= hash_index)
			this->path.erase(hash_index);
		else
			this->path.erase(query_string_index);
	}
}

void	webserv::Header::print()
{
	std::map<std::string, std::string>::iterator it = this->_headers.begin();

	for (; it != this->_headers.end(); it++)
		std::cout << it->first << " => " << it->second << std::endl;
}

bool	webserv::Header::is_done() const
{
	return (this->_is_done);
}

std::string &webserv::Header::get_body()
{
	return (this->_body);
}

std::map<std::string, std::string>	&webserv::Header::get_headers()
{
	return (this->_headers);
}
size_t								&webserv::Header::get_raw_header_len() const
{
	return (const_cast<size_t&>(this->_raw_header_len));
}