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
	bool					replace(std::string &str, std::string const target, std::string const to_replace);

}

int	unlink_file(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf);

# endif