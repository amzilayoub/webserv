/***********************************************************************
* FILENAME :        CharacterReader.hpp
*
* DESCRIPTION :
*       This file will contains a class with some functions
*		That will read from the input file (config file)
*
**/

# ifndef __CHARACTER_READER__HPP__
#  define __CHARACTER_READER__HPP__

#  include <string>
#  include <fstream>

namespace webserv
{

class CharacterReader
{
	/************************ MEMBER ATTRIBUTES ************************/
	private:
		std::string		const	_filename;
		std::string				_raw_input;
		std::ifstream			_ifs;
	
	/************************ CONSTRUCOTRS ************************/
	public:
		CharacterReader(std::string const &str);
		~CharacterReader(void);

	/************************ MEMBER FUNCTIONS ************************/
	public:
		/**
		 * 
		 * Return the next character of the next k-th character
		 * @param (index: optional) the index of the character to return,
		 * return the next if not specified
		 * 
		 * @returns the next character
		 * 
		 */
		char peek() const;
		char peek(size_t index) const;

		/**
		 * 
		 * Return the next character of the next k-th character and remove it
		 * from the _raw_input attributes
		 * @param None
		 * 
		 * @returns the next character
		 * 
		 */
		char next();

		bool isEOF() const;

	/************************ MEMBER CLASS (EXCEPTIONS) ************************/
	class FailedToOpenFileException : std::exception
	{
		const char *what() const throw();
	};

	class OutOfBound : std::exception
	{
		const char *what() const throw();
	};
};

}

# endif