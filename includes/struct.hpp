#ifndef STRUCT_HPP
#define STRUCT_HPP

#include "webserv.hpp"

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
	// path in navigator
	std::string path;
	// name of index file
	std::string index;
	// path of the index file
	std::string index_path;
	// root path applicable to location
	std::string root;
	// automatic listing of directory
	bool autoindex;
	// path for file used in directory listing
	std::string dir_default_path;
	// is cgi on or off
	bool cgi;
	// cgi file path
	std::string cgi_path;
	// active or unactive - path to cgi dependant folder
	std::pair<bool, std::string> cgi_depends;
	// vector of allowed http methods for location, in string format
	std::vector<std::string> allowed_methods;
};

struct Server {
	// port for server
	int listen;
	// requests body size
	int body_size;
	// socket fd
	int socket_fd;
	// server name
	std::string server_name;
	// root path for server to use as default if none is specicified in location
	std::string root;
	// map of : error-code - path to error page
	std::map<int, std::string> error_pages;
	// vector of locations
	std::vector<Location> locations;
};

#endif