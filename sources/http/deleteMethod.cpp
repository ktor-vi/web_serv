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

bool	isADirectory(std::string path)
{
	struct stat	info;
	if (stat(path.c_str(), &info) != 0) {
		return false;
	}
	return (info.st_mode & S_IFDIR) != 0;
}

bool	HandleRequests::isItLocked(std::string path)
{
	int fd = open(path.c_str(), O_RDONLY);
	if (fd < 0)
		return (false);
	bool locked = flock(fd, LOCK_EX | LOCK_NB) == -1;
	if (!locked)
		flock(fd, LOCK_UN);
	close (fd);
	return (locked);
}

void HandleRequests::deleteMethod(WebServer &webServData)
{
	initDeleteInfos(webServData);
	if (isMethodAllowed(webServData.getAllowedMethods(this->getServerPort(this->_buffer), this->_rootUrl), "DELETE") == false)
	{
		createPostResponse("403 Forbidden");
		return;
	}
	if (access(this->_filePath.c_str(), F_OK) == -1) {
		this->_response = createDeleteResponseHeader("404 Not Found");
		return;
	}
	if (open(this->_filePath.c_str(), O_RDONLY) == -1)
	{
		this->_response = createDeleteResponseHeader("403 Forbidden");
		return ;
	}
	bool	isDirectory = isADirectory(this->_filePath);
	bool	isLocked = isItLocked(this->_filePath);
	if (isLocked)
	{
		this->_response = createDeleteResponseHeader("409 Conflict");
		return ;
	}
	if (remove(this->_filePath.c_str()) == 0)
	{
		if (isDirectory == true)	
			this->_response = createDeleteResponseHeader("204 OK");
		else
			this->_response = createDeleteResponseHeader("200 OK");
	}
	else
		this->_response = createDeleteResponseHeader("500 Internal Server Error");
	struct epoll_event event;
	event.events = EPOLLOUT | EPOLLET;
	event.data.fd = this->_clientFd;
	if (epoll_ctl(this->_epollFd, EPOLL_CTL_MOD, this->_clientFd, &event) == -1)
	{
		perror("EPOLL_CTL MOD ERROR");
		return;
	}
}