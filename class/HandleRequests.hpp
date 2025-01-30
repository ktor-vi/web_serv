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
		
		// HandleRequests.cpp
		std::string	createBuffer(int clientFd);
		
		// getMethod.cpp
		bool		fileExists(const std::string &path);
		// bool		endsWith(const std::string &url, const std::string &suffix);
		int			openIndex(WebServer &webServData);
		std::string findFolder(std::string url);
		std::string	findRoot(std::string contentType);
		// std::string	findContentType(std::string url);
		void 		sendFile(std::string filePath, std::string url, int bodySize, int clientFd, std::string statusCode);		void		initInfos(WebServer &webServData);
		void		getMethods(WebServer &webServData);
		void		postMethods(WebServer &webServData);

	private:
		int					_clientFd;
		WebServer 			&_webServData;
		std::string			buffer;
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
		std::vector<char>	_response;

};

#endif
// std::ostream	&operator<<(std::ostream &stream, const HandleRequests &obj);
