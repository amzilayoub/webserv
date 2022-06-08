/***********************************************************************
* FILENAME :        Utils.hpp
*
* DESCRIPTION :
*       here we're going to have some helper functions
*
**/



# ifndef __UTILS__HPP__
#  define __UTILS__HPP__

#  include <list>

namespace webserv
{
	std::list<std::string>	*split(std::string const &str, std::string const &delimeter);
	std::string				&str_to_lower(std::string &str);
	std::string				webserv::str_to_upper(std::string str);
	bool					replace(std::string &str, std::string const target, std::string const to_replace);
	bool					replace_last(std::string &str, std::string const target, std::string const to_replace);
	bool					ends_with(std::string const & value, std::string const & ending);
}

int	unlink_file(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf);

# endif