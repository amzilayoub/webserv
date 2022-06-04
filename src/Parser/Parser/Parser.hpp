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
#  define __REGEX__STRING__ "[a-zA-Z]+"
#  define __REGEX_INTEGER__ "[0-9]+"
#  define __REGEX_IP__ "(\\d{1,3}(\\.\\d{1,3}){3})"
#  define __REGEX_URI__ "^/|(/[\\w-]+)+$"

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
typedef bool (Parser::*token_analyser)(webserv::Store &store);
typedef std::map<std::string, token_analyser> map_functions;

class Parser
{
	/************************ MEMBER ATTRIBUTES ************************/
	private:
		Lexer								*_lexer;
		map_functions						_token_analyser;
		int									_object_level;

	/************************ CONSTRUCOTRS ************************/
	public:
		Parser(CharacterReader &cr);
		~Parser(void);
	
	/************************ MEMBER FUNCTIONS ************************/
	public:
		bool	parse(Store &store);
	
	private:
		bool			_dispatch(std::string const &str, webserv::Store &store);
		bool			_exec_regex(std::string const &target, std::string const &regex_expr);
		bool			_is_semicolon();

	/************************ MEMBER FUNCTIONS (WORK ON TOKENS) ************************/
	private:
		bool _server_token(webserv::Store &store);
		bool _listen_token(webserv::Store &store);
		bool _root_token(webserv::Store &store);
		bool _server_name_token(webserv::Store &store);
		bool _allow_methods_token(webserv::Store &store);
		bool _upload_path_token(webserv::Store &store);
		bool _index_token(webserv::Store &store);
		bool _error_page_token(webserv::Store &store);
		bool _autoindex_token(webserv::Store &store);
		bool _client_max_body_size_token(webserv::Store &store);
		bool _redirection_token(webserv::Store &store);
		bool _location_token(webserv::Store &store);
		bool _right_bracket_token(webserv::Store &store);

	/************************ MEMBER CLASS ************************/
	public:
		class UnexpectedTokenException : public std::exception
		{
			private:
				/**
				 * Member attribue: the error message
				 */
				char const *_error_message;

				/**
				 * Constructor
				 */
			public:
				UnexpectedTokenException(std::string str);

				/**
				 * @return const char* 
				 */
				const char *what() const throw();
		};
};

}
# endif