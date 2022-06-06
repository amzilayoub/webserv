/***********************************************************************
* FILENAME :        Kqueue.cpp
*
* DESCRIPTION :
*       This File is the implementation of the functions
*		Defined in Kqueue.hpp
*
**/

# include "./Kqueue.hpp"
# include "../../OutputColors.hpp"
# include "../../Logger/Logger.hpp"

/************************ CONSTRUCTORS/DESTRUCTOR ************************/
webserv::Kqueue::Kqueue()
{
	this->_kq = kqueue();
	this->test_error(this->_kq, "Creating Kqueue :");
	this->_n_ev = 0;
}

webserv::Kqueue::~Kqueue()
{
	close(this->_kq);
}

/************************ MEMBER FUNCTIONS ************************/
void	webserv::Kqueue::set_event(int fd, int filter, int flags, void *udata)
{
	EV_SET(&this->_ev_set, fd, filter, flags, 0, 0, udata);
}

void	webserv::Kqueue::add_event(void)
{
	int ret;

	ret = kevent(this->_kq, &this->_ev_set, 1, NULL, 0, NULL);
	this->test_error(ret, "Kqueue/add_event functions");
}

int		webserv::Kqueue::get_event(void)
{
	this->_n_ev = kevent(this->_kq, NULL, 0, this->_ev_list, __EV_LIST_SIZE__, NULL);
	this->test_error(this->_n_ev, "Kqueue/get_event function:");
	return (this->_n_ev);
}

void	webserv::Kqueue::create_event(int fd, int filter, int flags, void *udata)
{
	this->set_event(fd, filter, flags, udata);
	this->add_event();
}

bool	webserv::Kqueue::isEOF(int index)
{
	if (this->_n_ev <= index)
		this->test_error(-1, "Kqueue/isEOF function:");
	return (this->_ev_list[index].flags & EV_EOF);
}

bool	webserv::Kqueue::is_read_available(int index)
{
	if (this->_n_ev <= index)
		this->test_error(-1, "Kqueue/is_read_available function:");
	return (this->_ev_list[index].filter == EVFILT_READ);
}

bool	webserv::Kqueue::is_write_available(int index)
{
	if (this->_n_ev <= index)
		this->test_error(-1, "Kqueue/is_write_available function:");
	return (this->_ev_list[index].filter == EVFILT_WRITE);
}

void	webserv::Kqueue::test_error(int fd, const std::string &str)
{
	if (fd < 0)
		webserv::Logger::error(str);
}

/************************ GETTERS/SETTERS ************************/
struct kevent	*webserv::Kqueue::get_event_list()
{
	return (this->_ev_list);
}

int	&webserv::Kqueue::get_kq()
{
	return (this->_kq);
}

int				webserv::Kqueue::get_fd(int index)
{
	if (this->_n_ev <= index)
		this->test_error(-1, "Kqueue/get_ev_list function:");
	return (this->_ev_list[index].ident);
}

void	webserv::Kqueue::set_kqueue(int fd)
{
	this->_kq = fd;
}