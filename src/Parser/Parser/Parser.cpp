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

/************************ CONSTRUCOTRS ************************/
webserv::Parser::Parser(webserv::CharacterReader &cr)
{
	this->_lexer = new webserv::Lexer(cr);
	this->_object_level = 0;
	this->_special_char = "{}:;";
	// this->_token_analyser["server"] = &Parser::_server_token;
	this->_token_analyser["listen"] = &Parser::_listen_token;

}

webserv::Parser::~Parser()
{
	delete this->_lexer;
}



/************************ MEMBER FUNCTIONS ************************/
bool	webserv::Parser::parse(webserv::Store &store)
{
	std::string	token = "";
	std::string	expected_token;

	while (!this->_lexer->isEOFToken())
	{
		token = this->_lexer->nextToken();
		this->_dispatch(token, store);
	}
	return (true);
}

std::string	webserv::Parser::_dispatch(std::string const &str, webserv::Store &store)
{
	map_functions::const_iterator it = this->_token_analyser.find(str);
	if (it == this->_token_analyser.end())
		return "unexpected Token";
	else
	{
		token_analyser f = it->second;
		std::cout << "FUNC : " << str << " RET : " << (this->*f)(str, store) << std::endl;
	}
	return (std::string());
}

bool	webserv::Parser::_exec_regex(std::string const &target, std::string const &regex_expr)
{
	std::regex	rgx(regex_expr);
	std::smatch	match;

	if (std::regex_search(target.begin(), target.end(), match, rgx))
		return (true);
	return (false);
}

/************************ MEMBER FUNCTIONS (WORK ON TOKENS) ************************/
bool webserv::Parser::_server_token(std::string const &str, webserv::Store &store)
{
	std::string token;

	token = this->_lexer->nextToken();
	if (token == "{")
	{
		token = this->_lexer->peek();
		std::string::iterator it = this->_special_char.begin();
		for (; it != this->_special_char.end(); it++)
		{
			if (std::string((*it), 1) == token)
				return (false);
		}
		return (true);
	}
	return (false);
}

bool webserv::Parser::_listen_token(std::string const &str, webserv::Store &store)
{
	std::string	token;
	bool		isValid;

	token = this->_lexer->nextToken();
	isValid = this->_exec_regex(token, __REGEX_ALPHA_NUM__) || this->_exec_regex(token, __REGEX_IP__);
	if (isValid)
	{
		token = this->_lexer->nextToken();
		if (token == ":")
		{
			token = this->_lexer->nextToken();
			isValid = this->_exec_regex(token, __REGEX_INTEGER__);
			if (isValid)
				return (true);
		}
		else if (token == ";")
		{
			return (true);
		}
		else
			return (false);
	}
	// std::cout << "LISTEN : " << token << " - REGEX : " << isValid << std::endl;
	return (false);
}