/***********************************************************************
* FILENAME :        Parser.cpp
*
* DESCRIPTION :
*       This File is the implementation of the functions
*		Defined in the header file
*
**/

# include "./Parser.hpp"
# include <iostream>
# include "../../Utils/Utils.hpp"

/************************ CONSTRUCOTRS ************************/
webserv::Parser::Parser(webserv::CharacterReader &cr)
{
	this->_lexer = new webserv::Lexer(cr);
	this->_object_level = 0;
	// this->_special_char = "{}:;$";
	this->_token_analyser["server"] = &Parser::_server_token;
	this->_token_analyser["listen"] = &Parser::_listen_token;
	this->_token_analyser["server_name"] = &Parser::_server_name_token;
	this->_token_analyser["root"] = &Parser::_root_token;
	this->_token_analyser["allow_methods"] = &Parser::_allow_methods_token;
	this->_token_analyser["upload_path"] = &Parser::_upload_path_token;
	this->_token_analyser["index"] = &Parser::_index_token;
	this->_token_analyser["error_page"] = &Parser::_error_page_token;
	this->_token_analyser["autoindex"] = &Parser::_autoindex_token;
	this->_token_analyser["client_max_body_size"] = &Parser::_client_max_body_size_token;
	this->_token_analyser["redirection"] = &Parser::_redirection_token;
	this->_token_analyser["location"] = &Parser::_location_token;
	this->_token_analyser["cgi"] = &Parser::_cgi_token;
	this->_token_analyser[__RIGHT_BRACKETS__] = &Parser::_right_bracket_token;

}

webserv::Parser::~Parser()
{
	delete this->_lexer;
}

/************************ MEMBER CLASS ************************/
webserv::Parser::UnexpectedTokenException::UnexpectedTokenException(std::string str) : std::exception()
{
	this->_error_message = str.c_str();
}

const char *webserv::Parser::UnexpectedTokenException::what() const throw()
{
	return (this->_error_message);
}

/************************ MEMBER FUNCTIONS ************************/
bool	webserv::Parser::parse(webserv::Store &store)
{
	std::string	token = "";
	std::string	expected_token;

	while (!this->_lexer->isEOFToken())
	{
		token = this->_lexer->nextToken();
		if (token == __SEMI_COLON__)
			continue;
		if (!this->_dispatch(token, store))
			throw UnexpectedTokenException("Unexpected Token near to " + token);
		if (this->_object_level == 0)
			return (true);
	}
	if (this->_object_level != 0)
		throw UnexpectedTokenException("end of object is not set");
	return (false);
}

bool	webserv::Parser::_dispatch(std::string const &str, webserv::Store &store)
{
	map_functions::const_iterator it = this->_token_analyser.find(str);
	if (it == this->_token_analyser.end())
		return (false);
	else
	{
		token_analyser f = it->second;
		if (!(this->*f)(store))
			return (false);
	}
	return (true);
}

bool	webserv::Parser::_exec_regex(std::string const &target, std::string const &regex_expr)
{
	std::regex	rgx(regex_expr);
	std::smatch	match;

	if (std::regex_search(target.begin(), target.end(), match, rgx))
		return (true);
	return (false);
}

bool	webserv::Parser::_is_semicolon()
{
	return (this->_lexer->nextToken() == __SEMI_COLON__);
}

/************************ MEMBER FUNCTIONS (WORK ON TOKENS) ************************/
bool webserv::Parser::_server_token(webserv::Store &store)
{
	std::string token;
	int			isValid;

	token = this->_lexer->nextToken();
	if (token == __LEFT_BRACKETS__)
	{
		++this->_object_level;
		token = this->_lexer->peek();
		isValid = this->_exec_regex(token, __REGEX_ALPHA_NUM__);
		if (isValid)
			return (true);
	}
	return (false);
}

bool webserv::Parser::_listen_token(webserv::Store &store)
{
	std::string	token;
	bool		isValid;

	token = this->_lexer->nextToken();
	isValid = this->_exec_regex(token, __REGEX_HOSTNAME__) || this->_exec_regex(token, __REGEX_IP__);
	if (isValid)
	{
		store.host = token;
		token = this->_lexer->nextToken();
		if (token == ":")
		{
			token = this->_lexer->nextToken();
			isValid = this->_exec_regex(token, __REGEX_INTEGER__);
			if (isValid)
				store.port = std::stoi(token);
		}
		else
			store.port = 80;
		return (this->_is_semicolon());
	}
	return (false);
}

bool webserv::Parser::_root_token(webserv::Store &store)
{
	std::string	value;

	value = this->_lexer->join();
	if (this->_exec_regex(value, __REGEX_URI__))
	{
		store.root = value;
		return (this->_is_semicolon());
	}
	return (false);
}

bool webserv::Parser::_server_name_token(webserv::Store &store)
{
	std::string	value;

	value = this->_lexer->join();
	store.server_name = value;
	return (this->_is_semicolon());
}

bool webserv::Parser::_allow_methods_token(webserv::Store &store)
{
	std::string	value;

	while (!this->_lexer->isEOFToken())
	{
		value = this->_lexer->nextToken();
		if (value == __SEMI_COLON__)
			return (true);
		else if (this->_exec_regex(value, __REGEX__STRING__))
			store.allow_methods.push_back(webserv::str_to_lower(value));
		else
			return (false);
	}
	return (false);
}

bool webserv::Parser::_upload_path_token(webserv::Store &store)
{
	std::string	value;

	value = this->_lexer->join();
	if (this->_exec_regex(value, __REGEX_URI__))
	{
		store.upload_path = value;
		return (this->_is_semicolon());
	}
	return (false);
}

bool webserv::Parser::_index_token(webserv::Store &store)
{
	std::string	value;

	while (!this->_lexer->isEOFToken())
	{
		value = this->_lexer->nextToken();
		if (value == __SEMI_COLON__)
			return (true);
		store.index.push_back(value);
	}
	return (false);
}

bool webserv::Parser::_error_page_token(webserv::Store &store)
{
	std::string	error_number;
	std::string	error_page;

	error_number = this->_lexer->nextToken();
	if (this->_exec_regex(error_number, __REGEX_INTEGER__))
	{
		error_page = this->_lexer->join();
		store.error_page[std::stoi(error_number)] = error_page;
	}
	return (this->_is_semicolon());
}

bool webserv::Parser::_autoindex_token(webserv::Store &store)
{
	std::string	value;

	value = this->_lexer->nextToken();
	store.is_autoindex_set = true;
	if (this->_exec_regex(value, __REGEX__STRING__))
	{
		if (value == "on" || value == "off")
		{
			store.autoindex = (value == "on");
			return (this->_is_semicolon());
		}
	}
	return (false);
}

bool webserv::Parser::_client_max_body_size_token(webserv::Store &store)
{
	std::string	value;

	value = this->_lexer->nextToken();
	if (this->_exec_regex(value, __REGEX_INTEGER__))
	{
		store.client_max_body_size = std::stoi(value);
		return (this->_is_semicolon());
	}
	return (false);
}

bool webserv::Parser::_redirection_token(webserv::Store &store)
{
	std::string	key;
	std::string	value;

	key = this->_lexer->nextToken();
	if (key == __SEMI_COLON__)
		return (false);
	value = this->_lexer->nextToken();
	store.redirection[key] = value;
	return (true);
}

bool webserv::Parser::_cgi_token(webserv::Store &store)
{
	std::string	value;

	value = this->_lexer->nextToken();
	if (value.front() != '.')
		return (false);
	store.cgi.extension = value;
	value = this->_lexer->join();
	if (value == __SEMI_COLON__)
		return (false);
	store.cgi.path = value;
	return (true);
}

bool webserv::Parser::_location_token(webserv::Store &store)
{
	std::string		value;
	webserv::Store	obj;
	int				obj_level;

	obj_level = 0;
	value = this->_lexer->nextToken();
	if (this->_exec_regex(value, __REGEX_URI__))
	{
		obj.location = value;
		value = this->_lexer->nextToken();
		if (value == __LEFT_BRACKETS__)
		{
			obj_level++;
			while (obj_level > 0 && !this->_lexer->isEOFToken())
			{
				value = this->_lexer->nextToken();
				if (value == __LEFT_BRACKETS__)
				{
					obj_level++;
					continue;
				}
				else if (value == __RIGHT_BRACKETS__)
				{
					obj_level--;
					continue;
				}
				this->_dispatch(value, obj);
			}
			if (obj_level == 0)
			{
				store.location_object.push_back(obj);
				return (true);
			}
		}
	}
	return (false);
}

bool webserv::Parser::_right_bracket_token(webserv::Store &store)
{
	this->_object_level--;

	return (true);
}