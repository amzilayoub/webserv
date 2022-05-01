/***********************************************************************
* FILENAME :        CharacterReader.cpp
*
* DESCRIPTION :
*       This file will contains all the implementation fo the
*		functiosn defined in the header file
*
**/

# include "./CharacterReader.hpp"
# include <exception>
# include <iostream>

/************************ CONSTRUCOTRS ************************/
webserv::CharacterReader::CharacterReader(std::string const &str) : _filename(str)
{
	std::string buf;

	this->_ifs.open(str);
	if (!this->_ifs.is_open())
		throw FailedToOpenFileException();
	
	/*
	** Read the whole config file into the _raw_input attribute
	*/
	while (std::getline(this->_ifs, buf))
		this->_raw_input += buf + '\n';
}

webserv::CharacterReader::~CharacterReader()
{
	this->_ifs.close();
}

/************************ MEMBER FUNCTIONS ************************/
char webserv::CharacterReader::peek() const
{
	return (this->_raw_input[0]);
}

char webserv::CharacterReader::peek(size_t index) const
{
	if (index >= this->_raw_input.length())
		throw OutOfBound();
	return (this->_raw_input[index]);
}

char webserv::CharacterReader::next()
{
	char c = this->_raw_input[0];

	this->_raw_input = this->_raw_input.substr(1);
	return (c);
}

bool webserv::CharacterReader::isEOF() const
{
	return (!(this->_raw_input.length() == 0));
}

/************************ MEMBER CLASS FUNCTIONS ************************/
const char *webserv::CharacterReader::FailedToOpenFileException::what() const throw()
{
	return ("Failed to open file !");
}

const char *webserv::CharacterReader::OutOfBound::what() const throw()
{
	return ("Index out of range !");
}