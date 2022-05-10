/***********************************************************************
* FILENAME :        Kqueue.hpp
*
* DESCRIPTION :
*       This file will contains a class Kqueue,
*		that has some the attributes/functions that
*		we will manipulate the kqueue instance
*
**/

# ifndef __KQUEUE__HPP__
#  define __KQUEUE__HPP__

#  include <sys/event.h>
#  include <iostream>
#  include <cstdio>
#  include <unistd.h>

#  define __EV_LIST_SIZE__ 100

namespace webserv
{

class Kqueue
{
	/************************ MEMBER ATTRIBUTES ************************/
	private:
		int				_kq;

		/*
		** Creating event list,
		** ev_set is the list of action to monitor
		** ev_list for the received list that an action happend
		*/
		struct kevent	_ev_list[__EV_LIST_SIZE__];
		struct kevent	_ev_set;
		int				_filter;
		int				_flags;

		/*
		** The return value of kqueue, means the number of fd that an event happend on
		*/
		int				_n_ev;

	/************************ CONSTRUCTORS/DESTRUCTOR ************************/
	public:
		Kqueue(void);
		~Kqueue();

	/************************ MEMBER FUNCTIONS ************************/
	public:
		/**
		 * Set the kernel event set/list and associate it with the file descriptor
		 * @param fd file descriptor 
		 * @returns void
		 */
		void	set_event(int fd);
		
		/**
		 * add the events to the kqueue
		 * @param void
		 * @returns void
		 */
		void	add_event(void);
		
		/**
		 * get an event list/ in other way, get the list of file
		 * descriptor that the event happens on
		 * @param void
		 * @returns void
		 */
		int		get_event(void);


		/**
		 * get if we finish working on the fd, in other way, the client disconnected
		 * @param index: as we receive a list of events, we need to specify which index(the index of the fd)
		 * @returns bool, true is we reached EOF, otherwise false
		 */
		bool	isEOF(int index);
		
		/**
		 * get if the fd is available for read event
		 * @param index: as we receive a list of events, we need to specify which index(the index of the fd)
		 * @returns bool, true is available, otherwise false
		 */
		bool	is_read_available(int index);

		void	test_error(int fd, const std::string &str);
	
	/************************ GETTERS/SETTERS ************************/
	public:
		struct kevent	*get_event_list();
		int				get_fd(int index);
};

}

# endif