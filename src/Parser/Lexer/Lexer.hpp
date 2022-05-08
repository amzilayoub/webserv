/***********************************************************************
* FILENAME :        Lexer.hpp
*
* DESCRIPTION :
*       This file will contains all the functions definition
*       That we will need to create a list of tokens from
*		The config file
*
*
* NOTE :
*		We will Use BNS Algorithm to tokenise the string
*
**/

# ifndef __LEXER__HPP__
#  define __LEXER__HPP__

/************************ SOME CONSTANTS ************************/
#  define __SEMI_COLON__ ";"
#  define __COLON__ ":"
#  define __LEFT_BRACKETS__ "{"
#  define __RIGHT_BRACKETS__ "}"

#  include <string>
#  include <list>
#  include "../CharacterReader/CharacterReader.hpp"

namespace webserv
{

class Lexer
{
	/************************ MEMBER ATTRIBUTES ************************/
	private:
		CharacterReader 			&_charReader;
		std::string					_last_token;
		std::list<std::string>		_separatore;
	
	/************************ CONSTRUCOTRS ************************/
	public:
		Lexer(CharacterReader &rhs);
	
	/************************ MEMBER FUNCTIONS ************************/
	public:

		/**
		 * 
		 * Return the next Token, calling this method multiple times will return
		 * the same Token
		 * @param None
		 * 
		 * @returns the next Token
		 * 
		 */
		std::string peek() const;

		/**
		 * 
		 * Return the next Token and remove it from the list of tokens,
		 * calling this method multiple times will return
		 * a new token each time
		 * @param None
		 * 
		 * @returns the next Token
		 * 
		 */
		std::string nextToken();
		
		/**
		 * 
		 * collect tokens into a single string until reach semi colon ;
		 * @param None
		 * 
		 * @returns std::string (Long token)
		 * 
		 */
		std::string join();

		bool isEOFToken() const;
	

	private:
		/**
		 * 
		 * this function extract a token from the input string and
		 * store it in the attribute _last_token
		 * @param void
		 * 
		 * @returns void
		 * 
		 */
		void	_storeToken();
		bool	_isWhiteSPace(char const &c) const;
		bool	_isSeparatore(std::string const &str) const;
	
	/************************ MEMBER CLASS ************************/
};

}

# endif