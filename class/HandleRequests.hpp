#ifndef HANDLEREQUESTS_HPP
#define HANDLEREQUESTS_HPP
#include "../includes/webserv.hpp"

class WebServer;

class HandleRequests
{
	public:
		HandleRequests(std::string request, WebServer &webServData, int epoll_fd, int client_fd);
		HandleRequests(const HandleRequests &copy);
		HandleRequests	&operator=(const HandleRequests &rhs);
		~HandleRequests(void);
		
		// HandleRequests.cpp
		std::string	createBuffer(int clientFd);
		void 		initURLs();
		// getMethod.cpp
		bool		fileExists(const std::string &path);
		int			openIndex(WebServer &webServData);
		std::string findFolder(std::string url);
		void 		sendFile(std::string filePath, std::string url, int bodySize, int clientFd, std::string statusCode);		
		void		initGetInfos(WebServer &webServData);
		void		initPostInfos(WebServer &webServData);
		void		getMethod(WebServer &webServData);
		void		postMethod(WebServer &webServData);
		void 		cgiMethods(WebServer &webServData);
		void		setBuffer(std::string buffer);
		void		setBytesRead(int bytes_read);
		std::string getResponse() const;

	private:
		int					_clientFd;
		int 				_epollFd;
		WebServer 			&_webServData;
		std::string			_buffer;
		int					_fdPage;
		std::string			_filePath;
		std::string			_fileName;
		int 				_bodySize;
		int 				_bytesRead;
		std::string			_rootDir;
		std::string			_url;
		std::string 		_rootUrl;
		int					 _port;
		std::string			_header;
		std::string			_response;

};

#endif
// std::ostream	&operator<<(std::ostream &stream, const HandleRequests &obj);
