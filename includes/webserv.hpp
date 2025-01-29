#ifndef MY_HEADER_HPP
#define MY_HEADER_HPP

#include <algorithm>
#include <arpa/inet.h> // htons, htonl, ntohs, ntohl
#include <cstdlib>	 // pour std::atoi
#include <dirent.h>	// opendir, readdir, closedir
#include <errno.h>	 // errno
#include <fcntl.h>	 // open, fcntl
#include <fstream>
#include <iostream>
#include <map>
#include <netdb.h>  // getaddrinfo, freeaddrinfo, gai_strerror, getprotobyname
#include <signal.h> // signal, kill
#include <stack>
#include <string.h> // strerror
#include <string>
#include <sys/socket.h> // socket, accept, bind, listen, connect, send, recv, socketpair, setsockopt, getsockname
#include <sys/types.h> // socket, accept, bind, listen, connect, waitpid, kill, opendir, readdir, closedir
#include <unistd.h> // fork, execve, pipe, chdir, dup, dup2, close, read, write, access, stat
#include <vector>
#include <sys/wait.h>
#include <sys/epoll.h>
#include <sstream>
#include <sys/stat.h>

#include "../class/HandleRequests.hpp"
#include "struct.hpp"
#include "../class/Config.hpp"
#include "../class/WebServer.hpp"

#define CHUNK_SIZE 4096

class WebServer;

// SOCKET PART
void	ft_webserver(WebServer &data);
bool	ft_isit_fdsocket(WebServer &data, int socket_fd);
void	ft_setup_socket(int *server_fd, int port, int epoll_fd, struct epoll_event *ptr);
void	ft_setup_all_socket(WebServer &data, int epoll_fd, struct epoll_event *ptr);
void	make_socket_nonblocking(int sockfd); // litteral

// CGI PART
char *ft_call_cgi(char *ans);

// HTTP PART
void		postMethods(std::string buffer);
void		getMethods(int clientFd, ssize_t bytes);
int			handle_request(int client_fd);
void		send_http_response_header(int client_fd, const char *content_type, ssize_t content_length);
int 		sendHttpResponseHeader(int clientFd, size_t contentLength, const std::string &contentType, const std::string &statusCode);
std::string	findContentType(std::string url);
#endif