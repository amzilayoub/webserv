# Single Thread HTTP Server
## Overview
This project implements a single-threaded HTTP server similar to nginx. The server uses multiplexing to handle multiple connections efficiently and leverages various system calls and functions to manage processes, sockets, and file descriptors.

## Features
- Single-threaded architecture: Utilizes a single thread to manage multiple connections concurrently.
- Multiplexing: Employs kqueue mechanisms for efficient I/O handling.
- Comprehensive system call usage: Integrates a wide range of system calls and functions for robust server functionality.

## System Calls and Functions Used
The project makes use of the following system calls and functions:

- Process and file descriptor management: execve, dup, dup2, pipe, fork, fcntl, close, read, write, waitpid, kill, signal
- Error handling: strerror, gai_strerror, errno
- Socket programming: socketpair, socket, accept, listen, send, recv, bind, connect, getaddrinfo, freeaddrinfo, setsockopt, getsockname, getprotobyname
- Network byte order conversion: htons, htonl, ntohs, ntohl
- Multiplexing mechanisms: select, poll, epoll (epoll_create, epoll_ctl, epoll_wait), kqueue (kqueue, kevent)
- File system operations: chdir, access, stat, opendir, readdir, closedir

## Getting Started
### Prerequisites
Ensure you have a C/C++ compiler installed along with the necessary development libraries for networking and system programming.
### Installation
1- Clone the repository:
```sh
https://github.com/amzilayoub/webserv.git
```
2- Navigate to the project directory:
```
cd webserv
```
3- Compile the source code:
```
make
```
3- Create a cgi folder inside the /tmp folder to use PHP, PYthon... CGI
```
mkdir /tmp/cgi
```
4- (ONLY FOR WORDPRESS) In wordpress, in the 'wp-config.php', add the following line 'define('CONCATENATE_SCRIPTS', false)' in order to solve the style issue.

### Usage
Run the compiled server:
```
webserv config_file
```
By default, the server will start listening on port 8080. You can configure the port and other settings by modifying the configuration file server.conf.

## Configuration
The server configuration can be customized via the server.conf file located in the src/configFiles/ directory. to understand the config file parameters, please refer to NGINX config file documentation, as this http server handle everything as NGINX.

## Contributing
Contributions are welcome! Please follow these steps to contribute:

1- Fork the repository.
2- Create a new branch (git checkout -b feature/your-feature).
3- Make your changes.
4- Commit your changes (git commit -m 'Add some feature').
5- push to the branch (git push origin feature/your-feature).
6- Open a pull request.
## License
This project is licensed under the MIT License. See the LICENSE file for details.

Acknowledgments
Inspired by the architecture and efficiency of nginx.
Utilizes various system calls and functions for a comprehensive implementation of a high-performance HTTP server.
