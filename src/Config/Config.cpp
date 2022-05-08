/***********************************************************************
* FILENAME :        Config.cpp
*
* DESCRIPTION :
*       This File is the implementation of the functions
*		Defined in Store.hpp
*
**/

# include "./Config.hpp"

webserv::Config::Config(void)
{
}

void	webserv::Config::parse(std::string const &path)
{
	webserv::CharacterReader	cr(path);
	webserv::Store				store;
	webserv::Parser				parser(cr);


	while(parser.parse(store))
	{
		this->config.push_back(store);
		store.clear();
	}
	
	std::list<webserv::Store>::iterator it = this->config.begin();
	for (; it != this->config.end(); it++)
	{
		(*it).print();
	}
}