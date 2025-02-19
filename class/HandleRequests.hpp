#ifndef HANDLEREQUESTS_HPP
#define HANDLEREQUESTS_HPP
#include "../includes/webserv.hpp"

class WebServer;

class HandleRequests
{
	public:
		HandleRequests(std::string request, WebServer &webServData, int epoll_fd, int client_fd);
		HandleRequests(const HandleRequests &copy);
		HandleRequests &operator=(const HandleRequests &rhs);
		~HandleRequests(void);
		
		// cgiMethods.cpp
		void		initCgiInfos(WebServer &webServData);
		std::string	cgiExecution(void);
		void		handleMultipartData(void);
		std::string	cgiMethod(WebServer &webServData);
		
		// deleteMethod.cpp
		void		initDeleteInfos(WebServer &webServData);
		std::string	createDeleteResponseHeader(const std::string &statusCode);
		bool		isItLocked(std::string path);
		void		deleteMethod(WebServer &webServData);

		// getMethod.cpp
		std::string findFolder(std::string url);
		void		initGetInfos(WebServer &webServData);
		std::string	createGetResponseHeader(size_t contentLength, const std::string &contentType, const std::string &statusCode);
		void		getMethod(WebServer &webServData);

		// postMethod.cpp
		void		getUploadsLocation(WebServer &webServData);
		std::string	createPostResponseHeader(size_t contentLength, const std::string &contentType, const std::string &statusCode);
		void		createPostResponse(std::string code);
		void		postMethod(WebServer &webServData);

		// HandleRequests.cpp
		int			getKeepAlive(void);
		void		initPostInfos(WebServer &webServData);
		int			getServerPort(std::string req);
		void		setBuffer(std::string buffer);
		void		setBytesRead(int bytes_read);
		void 		initURLs();
		std::string	getResponse() const;
		bool		isMethodAllowed(const std::vector<std::string> methods, const std::string asked);
		std::string errorPageToBody(int error_code, WebServer &data);

    //autoIndexPage.cpp
    void    buildAutoIndexResponse(const std::string &path);

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
		std::string			_uploadsLocations;
		int					_keepAlive;

};

#endif
// std::ostream	&operator<<(std::ostream &stream, const HandleRequests &obj);
