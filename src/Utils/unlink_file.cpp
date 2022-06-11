/***********************************************************************
* FILENAME :        unlink_file.cpp
*
* DESCRIPTION :
*       remove file
*
**/

# include "./Utils.hpp"
# include <string>
# include <stdio.h>
# include <iostream>
# include <ftw.h>

int unlink_file(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
	(void)sb;
	(void)typeflag;

	if (ftwbuf->level > 0)
	{
		if (remove(fpath))
			return (1);
		return (0);
	}
	return (0);
}