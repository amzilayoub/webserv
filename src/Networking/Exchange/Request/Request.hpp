/***********************************************************************
* FILENAME :        Request.hpp
*
* DESCRIPTION :
*       Request class to handle the client request
*
**/


# ifndef __REQUEST__HPP__
#  define __REQUEST__HPP__

#  include <map>
#  include <string>
#  include "../Header/Header.hpp"
#  define __REGEX_ALPHA_NUM__ "^[a-zA-Z0-9_]*$"
#  include "../../../Config/Store/Store.hpp"

namespace webserv
{

class Request
{
	/************************ MEMBER TYPE ************************/
	public:
		typedef std::map<std::string, std::string>::iterator hr_iterator;

	/************************ MEMBER ATTRIBUTES ************************/
	public:
		webserv::Store	config;
		size_t			content_length;
		std::string		html_path;

	private:
		webserv::Header _header;
		std::string		_body;
		bool			_headers_done;
		bool			_has_error;
		std::string		_header_special_char;
		size_t			_request_length;

		std::string		_transfer_encoding;
		bool			_is_chunked;
		int				_chunk_left_to_read;
		
		
	
	/************************ CONSTRUCTOR/DESTRUCTIR ************************/
	public:
		Request();
	
	/************************ MEMBER FUNCTIONS ************************/
	public:
		bool parse(std::string &str, int len);
		void clear(void);
		void handle_location(void);
		bool handle_chunked_request(std::string &str, int len);
		bool is_done(void);
		int to_int(std::string str);

	/************************ GETTERS/SETTERS ************************/
	void								set_config(webserv::Store config);
	std::map<std::string, std::string>	&get_headers(void);
	std::string							&get_body();
	bool								header_is_done();
	webserv::Header						&get_header_obj(void);
	void								add_request_length(int size);
};

};


# endif