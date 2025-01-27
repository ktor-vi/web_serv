#ifndef HANDLEREQUESTS_HPP
#define HANDLEREQUESTS_HPP
#include "../includes/webserv.hpp"

class WebServer;

class HandleRequests
{
	public:
		HandleRequests(int clientFd, WebServer &webServData);
		HandleRequests(const HandleRequests &copy);
		HandleRequests	&operator=(const HandleRequests &rhs);
		~HandleRequests(void);
		
		int			openIndex(WebServer &webServData);
		std::string findFolder(std::string url);
		std::string	findRoot(std::string contentType);
		std::string	findPath(std::string rootDir);
		int 		sendHttpResponseHeader(void);
		std::string	findContentType(void);
		void		getMethods(WebServer &webServData);
		std::string	createBuffer(int clientFd);

	private:
		int			_clientFd;
		WebServer 	&_webServData;
		char		_str[1024];
		char		_buffer[2048];
		std::string	buffer;
		size_t 		_bytes;
		int			_fdPage;
<<<<<<< HEAD
		char		_bufferPage[8192];
=======
		int			_clientFd;
>>>>>>> 8aad97e (before pull)
		
		std::string	_filePath;
		int 		_bodySize;
		std::string	_rootDir;
		std::string	_url;
		int			 _port;
		std::string	_request;
		std::string	_response;
		std::string	_folderType;

};

#endif
// std::ostream	&operator<<(std::ostream &stream, const HandleRequests &obj);
