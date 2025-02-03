#include "../../includes/webserv.hpp"

void HandleRequests::initDeleteInfos(WebServer &webServData)
{
	this->_rootDir = webServData.getRootPath(this->_port, this->_rootUrl);
	size_t methodPos = this->_buffer.find("DELETE ") + 7;
	size_t urlEnd = this->_buffer.find(" HTTP", methodPos);
	this->_fileName = this->_buffer.substr(methodPos, urlEnd - methodPos);
	size_t slashPos = this->_fileName.rfind('/');
	if (slashPos != std::string::npos)
		this->_fileName = this->_fileName.substr(slashPos + 1);
	this->_filePath = this->_rootDir + this->_fileName;
	std::cout << "COUCOU 1 !! [" << this->_fileName << "]" << std::endl;
	std::cout << "COUCOU 2 !! [" << this->_filePath << "]" << std::endl;
	std::cout << "COUCOU 3 !! [" << this->_rootDir << "]" << std::endl;
}

static std::string createDeleteResponseHeader(const std::string &statusCode)
{
    std::ostringstream headerStream;
    headerStream << "HTTP/1.1 " << statusCode << " \r\n";
    headerStream << "Content-Type: text/plain\r\n";
    headerStream << "Content-Length: 0\r\n";
    headerStream << "Connection: keep-alive\r\n";
    headerStream << "\r\n";
    return headerStream.str();
}

void HandleRequests::deleteMethod(WebServer &webServData)
{
	initDeleteInfos(webServData);

	if (access(this->_filePath.c_str(), F_OK) == -1) {
		this->_response = createDeleteResponseHeader("404 Not Found");
		return;
	}

	if (remove(this->_filePath.c_str()) == 0)
		this->_response = createDeleteResponseHeader("200 OK");
	else
		this->_response = createDeleteResponseHeader("403 Forbidden");

	struct epoll_event event;
	event.events = EPOLLOUT | EPOLLET;
	event.data.fd = this->_clientFd;
	if (epoll_ctl(this->_epollFd, EPOLL_CTL_MOD, this->_clientFd, &event) == -1)
	{
		perror("EPOLL_CTL MOD ERROR");
		return;
	}
}
