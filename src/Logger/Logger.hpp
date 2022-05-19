/***********************************************************************
* FILENAME :        Logger.hpp
*
* DESCRIPTION :
*       Logger class contains some functions to write log
*
**/

# ifndef __LOGGER_H__
#  define __LOGGER_H__

# include <string>

namespace webserv
{

class Logger
{
	/************************ MEMBER FUNCTIONS ************************/
	public:
		static void info(std::string const &msg);
		static void warning(std::string const &msg);
		static void debug(std::string const &msg);
		static void error(std::string const &msg);
	
	private:
		static void _get_time();
		static void _set_color(char const *color);
		static void _write_message(char const *color, std::string const &msg);
};

};

# endif