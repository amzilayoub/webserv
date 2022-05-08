/***********************************************************************
* FILENAME :        Lexer.cpp
*
* DESCRIPTION :
*       This file will contains all the implementation fo the
*		functiosn defined in the header file
*
**/

# include "./Lexer.hpp"
# include <iostream>

/************************ CONSTRUCOTRS ************************/
webserv::Lexer::Lexer(CharacterReader &rhs) : _charReader(rhs)
{
	_separatore.push_front(__SEMI_COLON__);
	_separatore.push_front(__COLON__);
	_separatore.push_front(__RIGHT_BRACKETS__);
	_separatore.push_front(__LEFT_BRACKETS__);
	this->_storeToken();
}

/************************ MEMBER FUNCTIONS ************************/
std::string webserv::Lexer::peek() const
{
	return (this->_last_token);
}

std::string webserv::Lexer::nextToken()
{
	std::string token = this->_last_token;

	this->_storeToken();
	return (token);
}

std::string webserv::Lexer::join()
{
	std::string value;

	value = "";
	while (!this->isEOFToken())
	{
		if (this->peek() == __SEMI_COLON__)
			return (value);
		if (value != "")
			value += " " + this->nextToken();
		else
			value = this->nextToken();
	}
	return (value);
}

bool	webserv::Lexer::_isWhiteSPace(char const &c) const
{
	return ((c >= 9 && c <= 13) || c == 32);
}

bool webserv::Lexer::_isSeparatore(std::string const &str) const
{
	if (this->_isWhiteSPace(str[0]))
		return (true);
	std::list<std::string>::const_iterator it = this->_separatore.begin();

	for (; it != this->_separatore.end(); it++)
	{
		if ((*it) == str)
			return (true);
	}
	return (false);
}

bool	webserv::Lexer::isEOFToken() const
{
	return (this->_last_token == "");
}

void	webserv::Lexer::_storeToken()
{
	std::string	token = "";
	char		c;

	/*
	** Skipping the white spaces
	*/
	while (this->_charReader.isEOF())
	{
		c = this->_charReader.peek();
		if (this->_isWhiteSPace(c))
			this->_charReader.next();
		else
			break;
	}
	while (this->_charReader.isEOF())
	{
		token += this->_charReader.next();
		c = this->_charReader.peek();
		if (this->_isSeparatore(std::string(1, c)) || this->_isSeparatore(token))
		{
			this->_last_token = token;
			return ;
		}
	}
	this->_last_token = "";
}