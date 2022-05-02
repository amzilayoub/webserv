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

bool	webserv::Lexer::_isWhiteSPace(char const &c) const
{
	return ((c >= 9 && c <= 13) || c == 32);
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
		if (this->_isWhiteSPace(c) || c == __COMMA__)
		{
			this->_last_token = token;
			return ;
		}
	}
	this->_last_token = "";
}