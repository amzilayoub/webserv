# include "./src/Parser/CharacterReader/CharacterReader.hpp"
# include "./src/Parser/Lexer/Lexer.hpp"
# include <map>
# include <iostream>

int main()
{
	webserv::CharacterReader cr("./src/Config/default.conf");
	webserv::Lexer lx(cr);

	while (!lx.isEOFToken())
	{
		std::cout << lx.nextToken() << std::endl;
	}
}