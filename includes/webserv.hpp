#ifndef MY_HEADER_HPP
#define MY_HEADER_HPP

#include <unistd.h>       // fork, execve, pipe, chdir, dup, dup2, close, read, write, access, stat
#include <fcntl.h>        // open, fcntl
#include <sys/types.h>    // socket, accept, bind, listen, connect, waitpid, kill, opendir, readdir, closedir
#include <sys/socket.h>   // socket, accept, bind, listen, connect, send, recv, socketpair, setsockopt, getsockname
#include <netdb.h>        // getaddrinfo, freeaddrinfo, gai_strerror, getprotobyname
#include <arpa/inet.h>    // htons, htonl, ntohs, ntohl
#include <signal.h>       // signal, kill
#include <sys/select.h>   // select
#include <poll.h>         // poll
#include <sys/epoll.h>    // epoll_create, epoll_ctl, epoll_wait
#include <sys/event.h>    // kqueue, kevent
#include <errno.h>        // errno
#include <string.h>       // strerror
#include <dirent.h>       // opendir, readdir, closedir

#endif // MY_HEADER_HPP
