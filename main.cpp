# include "./src/Parser/CharacterReader/CharacterReader.hpp"
# include <map>
# include <iostream>

int main()
{
	webserv::CharacterReader cr("./src/Config/default.conf");

	while (cr.isEOF())
	{
		std::cout << cr.next();
	}
}