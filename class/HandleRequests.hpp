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
		int 		sendHttpResponseHeader(size_t contentLength, const std::string& contentType, const std::string &statusCode);
		void		initInfos(WebServer &webServData);
		void		getMethods(WebServer &webServData);

		// postMethod.cpp
		void	postMethods(WebServer &webServerData);

	private:
		int			_clientFd;
		WebServer 	&_webServData;
		char		_str[1024];
		char		_buffer[2048];
		std::string	buffer;
		size_t 		_bytes;
		int			_fdPage;
		std::string	_filePath;
		int 		_bodySize;
		std::string	_rootDir;
		std::string	_url;
		std::string _rootUrl;
		int			 _port;
		std::string	_request;
		std::string	_response;
		std::string	_folderType;

};

#endif
// std::ostream	&operator<<(std::ostream &stream, const HandleRequests &obj);
