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
	this->get_url_encoding();
}

void	webserv::Config::parse(std::string const &path)
{
	webserv::CharacterReader	cr(path);
	webserv::Store				store;
	webserv::Parser				parser(cr);


	while(parser.parse(store))
	{
		if (!store.check())
			exit(1);
		this->config.push_back(store);
		store.clear();
	}
}

void	webserv::Config::get_url_encoding()
{
	std::ifstream	fs;
	std::string		line;

	fs.open("./src/ConfigFiles/UrlEncoding.txt");
	if (!fs.is_open())
		webserv::Logger::error("Cannot open UrlEncoding.txt file");
	while (std::getline(fs, line))
	{
		std::list<std::string> *words = webserv::split(line, " ");

		this->url_encoding[words->back()] = std::stoi(words->front());
		delete words;
	}
	fs.close();
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
	fs.close();
}

std::string	webserv::Config::get_file_extension(std::string &content_type)
{
	std::map<std::string, std::string>::iterator it = this->mime_types.find(content_type);

	if (it == this->mime_types.end())
		return (".txt");
	return (it->second);
}