
# include <iostream>
# include "./src/Webserv.includes.hpp"

int main()
{
	webserv::Config config;

	config.parse("./src/ConfigFiles/default.conf");
}