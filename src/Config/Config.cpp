/***********************************************************************
* FILENAME :        Config.cpp
*
* DESCRIPTION :
*       This File is the implementation of the functions
*		Defined in Store.hpp
*
**/

# include "./Config.hpp"
# include "../Utils/Utils.hpp"
# include "../Logger/Logger.hpp"

webserv::Config::Config(void)
{
	this->get_mime_types_list();
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
	
	// std::list<webserv::Store>::iterator it = this->config.begin();
	// for (; it != this->config.end(); it++)
	// {
	// 	(*it).print();
	// }
}

void	webserv::Config::get_mime_types_list()
{
	std::ifstream	fs;
	std::string		line;

	fs.open("./src/ConfigFiles/MIMETypes.txt");
	if (!fs.is_open())
		webserv::Logger::error("Cannot open MIMETYPES.txt file");
	while (std::getline(fs, line))
	{
		std::list<std::string> *words = webserv::split(line, " ");

		this->mime_types[words->back()] = words->front();
		delete words;
	}
	this->mime_types["default_type"] = "text/plain";
	this->mime_types["default_extension"] = ".txt";
}

std::string	webserv::Config::get_file_extension(std::string &content_type)
{
	std::map<std::string, std::string>::iterator it = this->mime_types.find(content_type);

	if (it == this->mime_types.end())
		return (".txt");
	return (it->second);
}