/***********************************************************************
* FILENAME :        Parser.hpp
*
* DESCRIPTION :
*       This file will contains all the functions definition
*       That we will need to Parse our config file
*
*
* NOTE :
*		We will use Recursive descent implementation
*
**/

# ifndef __PARSER__HPP__
#  define __PARSER__HPP__

#  define __ANY__ ""
#  define __REGEX_ALPHA_NUM__ "^[a-zA-Z0-9_]*$"
#  define __REGEX_IP__ "(\\d{1,3}(\\.\\d{1,3}){3})"
#  define __REGEX_INTEGER__ "[0-9]+"

#  include <map>
#  include <regex>
#  include "../Parser.includes.hpp"

namespace webserv
{

/*
** decalring the Parser class as it will be needed by the pointer to function
** in the typedef, the Parser is not known yet, so the compiler will throw an error
*/
class Parser;

/************************ MEMBER ATTRIBUTES ************************/
typedef bool (Parser::*token_analyser)(std::string const &str, webserv::Store &store);
typedef std::map<std::string, token_analyser> map_functions;

class Parser
{
	/************************ MEMBER ATTRIBUTES ************************/
	private:
		Lexer								*_lexer;
		map_functions						_token_analyser;
		int									_object_level;
		std::string							_special_char;

	/************************ CONSTRUCOTRS ************************/
	public:
		Parser(CharacterReader &cr);
		~Parser(void);
	
	/************************ MEMBER FUNCTIONS ************************/
	public:
		bool	parse(Store &store);
	
	private:
		std::string _dispatch(std::string const &str, webserv::Store &store);
		bool		_exec_regex(std::string const &target, std::string const &regex_expr);

	/************************ MEMBER FUNCTIONS (WORK ON TOKENS) ************************/
	private:
		bool _server_token(std::string const &str, webserv::Store &store);
		bool _listen_token(std::string const &str, webserv::Store &store);

};

}
# endif