#ifndef MY_HEADER_HPP
#define MY_HEADER_HPP

#include <arpa/inet.h> // htons, htonl, ntohs, ntohl
#include <cstdlib>     // pour std::atoi
#include <dirent.h>    // opendir, readdir, closedir
#include <errno.h>     // errno
#include <fcntl.h>     // open, fcntl
#include <fstream>
#include <iostream>
#include <map>
#include <netdb.h>  // getaddrinfo, freeaddrinfo, gai_strerror, getprotobyname
#include <poll.h>   // poll
#include <signal.h> // signal, kill
#include <stack>
#include <string.h> // strerror
#include <string>
#include <sys/event.h>  // kqueue, kevent
#include <sys/select.h> // select
#include <sys/socket.h> // socket, accept, bind, listen, connect, send, recv, socketpair, setsockopt, getsockname
#include <sys/types.h> // socket, accept, bind, listen, connect, waitpid, kill, opendir, readdir, closedir
#include <unistd.h> // fork, execve, pipe, chdir, dup, dup2, close, read, write, access, stat
#include <vector>
struct LocationBlock {
  std::string path;
  std::map<std::string, std::string> directives;
};

struct ServerBlock {
  int listen;
  std::string server_name;
  std::vector<LocationBlock> locations;
  std::map<std::string, std::string> other_directives;
};

struct Location {
  std::string path;
  std::string index;
  std::string index_path;
  std::string root;
  bool autoindex;
  std::string dir_default_path;
  bool cgi;
  std::string cgi_path;
  std::pair<bool, std::string> cgi_depends;
  std::vector<std::string> allowed_methods;
};

struct Server {
  int listen;
  int body_size;
  std::string server_name;
  std::string root;
  std::map<int, std::string> error_pages;
  std::vector<Location> locations;
};

#endif // MY_HEADER_HPP
