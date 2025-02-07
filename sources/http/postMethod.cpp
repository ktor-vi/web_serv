#include "../../includes/webserv.hpp"

void HandleRequests::initPostInfos(WebServer &webServData)
{
	this->_rootDir = webServData.getRootPath(this->_port, this->_rootUrl);
	this->_fileName = this->_buffer.substr(this->_buffer.find("filename=\"") + 10, this->_buffer.find("\"", this->_buffer.find("filename=\"") + 10) - this->_buffer.find("filename=\"") - 10);
	this->_filePath = this->_rootDir + this->_fileName;
	std::cout << "COUCOU 1a !! [" << this->_fileName << "]" << std::endl;
	std::cout << "COUCOU 2b !! [" << this->_filePath << "]" << std::endl;
	std::cout << "COUCOU 3c !! [" << this->_rootDir << "]" << std::endl;
}

static std::string createPostResponseHeader(size_t contentLength, const std::string &contentType, const std::string &statusCode)
{
	std::ostringstream headerStream;

	headerStream << "HTTP/1.1 " << statusCode << " \r\n";
	headerStream << "Content-Type: " << contentType << "\r\n";
	headerStream << "Content-Length: " << contentLength << "\r\n";
	headerStream << "Connection: keep-alive\r\n";
	headerStream << "\r\n";
	std::string header = headerStream.str();

	return (header.c_str());
}

void	HandleRequests::createPostResponse(std::string code)
{
	std::ifstream file(this->_filePath.c_str(), std::ios::binary);
	std::ostringstream content;
	content << file.rdbuf();
	file.close();
	this->_response = createPostResponseHeader(content.str().size(), findContentType(this->_filePath), code) + content.str();
}

void HandleRequests::postMethod(WebServer &webServData)
{
	initPostInfos(webServData);

	if (isMethodAllowed(webServData.getAllowedMethods(this->getServerPort(this->_buffer), this->_rootUrl), "GET") == false)
	{
		createPostResponse("403 Forbidden");
		return;
	}	

	int fileFd = open(this->_filePath.c_str(), O_WRONLY | O_CREAT | O_NONBLOCK | O_TRUNC, 0644);
	if (fileFd < 0) {
		perror("Error opening file");
		return;
	}

	webServData.setPostFileFds(this->_clientFd, fileFd);

	// Extraire le corps de la requête HTTP
	size_t header_end = this->_buffer.find("\r\n\r\n");
	if (header_end != std::string::npos)
	{
		size_t body_start = header_end + 4;  
		size_t file_data_start = this->_buffer.find("\r\n\r\n", body_start);
		if (file_data_start != std::string::npos)
			file_data_start += 4;	 
		std::string body = this->_buffer.substr(file_data_start);
		webServData.setPostBody(this->_clientFd, body);
	}

	// Activer EPOLLOUT pour écrire dans handle_write_event()
	struct epoll_event event;
	event.events = EPOLLOUT | EPOLLET;
	event.data.fd = this->_clientFd;
	if (epoll_ctl(this->_epollFd, EPOLL_CTL_MOD, this->_clientFd, &event) == -1)
	{
		perror("EPOLL_CTL MOD ERROR");
		return;
	}
	createPostResponse("200 OK");
}


