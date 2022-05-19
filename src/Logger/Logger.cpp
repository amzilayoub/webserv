/***********************************************************************
* FILENAME :        Logger.cpp
*
* DESCRIPTION :
*       Definition of the member functions
*
**/

# include "./Logger.hpp"
# include <iostream>
# include "../OutputColors.hpp"
# include <ctime>

void webserv::Logger::info(std::string const &msg)
{
	webserv::Logger::_write_message(GREEN, msg);
}
void webserv::Logger::warning(std::string const &msg)
{
	webserv::Logger::_write_message(YELLOW, msg);
}

void webserv::Logger::debug(std::string const &msg)
{
	webserv::Logger::_write_message(MAGENTA, msg);
}

void webserv::Logger::error(std::string const &msg)
{
	webserv::Logger::_write_message(RED, msg);
	perror("");
	exit(EXIT_FAILURE);

}

void webserv::Logger::_get_time()
{
	std::time_t t = std::time(0);
    std::tm* now = std::localtime(&t);

    std::cout << (now->tm_year + 1900) << '-' << (now->tm_mon + 1) << '-' <<  now->tm_mday
		<< " " << now->tm_hour << ":" << now->tm_min << ":" << now->tm_sec << " | ";
}

void webserv::Logger::_set_color(char const *color)
{
	std::cout << color;
}

void webserv::Logger::_write_message(char const *color, std::string const &msg)
{
	webserv::Logger::_set_color(color);
	webserv::Logger::_get_time();
	std::cout << msg << std::endl;
	webserv::Logger::_set_color(RESET);
}