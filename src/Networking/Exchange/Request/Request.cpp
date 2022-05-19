/***********************************************************************
* FILENAME :        Request.cpp
*
* DESCRIPTION :
*       This File is the implementation of the functions
*		Defined in Request.hpp
*
**/

# include "./Request.hpp"

/************************ MEMBER ATTRIBUTES ************************/
webserv::Request::Request()
{
	this->_headers_done = false;
}

/************************ MEMBER FUNCTIONS ************************/
void webserv::Request::parse(std::string &str)
{
	while (!this->_headers_done)
	{
		this->_header.parse(str);
		if (this->_header.is_done())
		{
			this->_headers_done = true;
			this->_body = this->_header.get_body();
		}
	}
	this->_body += str;
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