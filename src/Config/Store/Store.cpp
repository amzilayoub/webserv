/***********************************************************************
* FILENAME :        Store.cpp
*
* DESCRIPTION :
*       This File is the implementation of the functions
*		Defined in STore.hpp
*
**/



# include "./Store.hpp"
# include <iostream>

/************************ CONSTRUCOTRS ************************/
webserv::Store::Store()
{
	this->clear();
}

webserv::Store::Store(webserv::Store const &rhs)
{
	this->clear();
	(*this) = rhs;
}

webserv::Store::~Store()
{

}

/************************ MEMBER FUNCTIONS************************/
void	webserv::Store::attach_location(webserv::Store const &rhs)
{
	this->root = (rhs.root.empty() ? this->root : rhs.root);
	this->allow_methods = (rhs.allow_methods.empty() ? this->allow_methods : rhs.allow_methods);
	this->upload_path = (rhs.upload_path.empty() ? this->upload_path : rhs.upload_path);
	this->index = (rhs.index.empty() ? this->index : rhs.index);
	this->error_page = (rhs.error_page.empty() ? this->error_page : rhs.error_page);
	this->autoindex = (rhs.is_autoindex_set ? rhs.autoindex : this->autoindex);
	this->client_max_body_size = (rhs.client_max_body_size == -1 ? this->client_max_body_size : rhs.client_max_body_size);
	this->redirection = (rhs.redirection.empty() ? this->redirection : rhs.redirection);
}

void	webserv::Store::clear()
{
	this->host = "0.0.0.0";
	this->port = 80;
	this->root.clear();
	this->server_name.clear();
	this->allow_methods.clear();
	this->upload_path.clear();
	this->index.clear();
	this->error_page.clear();
	this->autoindex = false;
	this->client_max_body_size = -1;
	this->redirection.clear();
	this->location.clear();
	this->location_object.clear();
	this->is_autoindex_set = false;
}

void	webserv::Store::print() const
{
	std::cout << "host: " << this->host << std::endl;
	std::cout << "port: " << this->port << std::endl;
	std::cout << "root: " << this->root << std::endl;
	std::cout << "server_name: " << this->server_name << std::endl;
	
	std::cout << "allow_methods: " << std::endl;
	std::list<std::string>::const_iterator allow_methods_it = this->allow_methods.begin();
	for (; allow_methods_it != this->allow_methods.end(); ++allow_methods_it)
		std::cout << "-- " << (*allow_methods_it) << std::endl;
	
	std::cout << "upload_path: " << this->upload_path << std::endl;

	std::cout << "index: " << std::endl;
	std::list<std::string>::const_iterator index_it = this->index.begin();
	for (; index_it != this->index.end(); ++index_it)
	std::cout << "-- " << (*index_it) << std::endl;

	std::cout << "error_page: " << std::endl;
	std::map<int, std::string>::const_iterator error_page_it = this->error_page.begin();
	for (; error_page_it != this->error_page.end(); ++error_page_it)
		std::cout << "-- " << error_page_it->first << " -> " << error_page_it->second << std::endl;
	
	std::cout << "autoindex: " << this->autoindex << std::endl;
	std::cout << "client_max_body_size: " << this->client_max_body_size << std::endl;
	
	std::cout << "redirection: " << std::endl;
	std::map<std::string, std::string>::const_iterator redirection_it = this->redirection.begin();
	for (; redirection_it != this->redirection.end(); ++redirection_it)
		std::cout << "-- " << redirection_it->first << " -> " << redirection_it->second << std::endl;
	
	std::cout << "location: " << this->location << std::endl;
	std::cout << "---- LIST OBJECTS ----" << std::endl;
	std::list<Store>::const_iterator location_it = this->location_object.begin();
	for (; location_it != this->location_object.end(); location_it++)
		(*location_it).print();
}

/************************ OPERATOR OVERLOAD ************************/
webserv::Store &webserv::Store::operator=(webserv::Store const &rhs)
{
	this->port = rhs.port;
	this->host = rhs.host;
	this->root = rhs.root;
	this->server_name = rhs.server_name;
	this->allow_methods = rhs.allow_methods;
	this->upload_path = rhs.upload_path;
	this->index = rhs.index;
	this->error_page = rhs.error_page;
	this->autoindex = rhs.autoindex;
	this->client_max_body_size = rhs.client_max_body_size;
	this->redirection = rhs.redirection;
	this->location = rhs.location;
	this->location_object = rhs.location_object;
	this->is_autoindex_set = rhs.is_autoindex_set;

	return (*this);
}