#include "../../includes/webserv.hpp"

int		HandleRequests::openIndex(WebServer &webServData)
{
	std::string	index = webServData.getIndexPath(webServData.getPort(0) , "/");
	this->_fdPage = open(index.c_str(), O_RDONLY); // index
	return (this->_fdPage);
}

bool	HandleRequests::fileExists(const std::string &path)
{
	struct stat buffer;

	if (stat(path.c_str(), &buffer) == 0)
		return (true);
	return (false);
}

bool	endsWith(const std::string &url, const std::string &suffix)
{
	if (suffix.size() > url.size())
		return (false);
	if (url.substr(url.size() - suffix.size()) == suffix)
		return (true);
	return (false);
}

std::string HandleRequests::findFolder(std::string url)
{
	std::string	type;

	if (endsWith(url, ".jpeg") || endsWith(url, ".jpg"))
		type = "img";
	else if (endsWith(url, ".png"))
		type = "img";
	else if(endsWith(url, ".html")) 
		type = "text/html"; // gestion des erreurs ?
	else if(endsWith(url, ".css")) 
		type = "text/css";
	return (type);
}

std::string	HandleRequests::findRoot(std::string contentType)
{
	int	i = 0;
	std::string	types[3] = {"img", "css", "other"};
	std::string	root;

	while (i < 4 && types[i] != contentType)
		i++;
	switch(i)
	{
		case 0:
			root = this->_webServData.getRootPath(8080, "/images/");
			break ;
		case 1:
			root = "/home/rdendonc/Documents/WebServ/public/css";
			break ;
		case 2:
			root = "/home/rdendonc/Documents/WebServ/public/html";
			break ;
		default:
			std::cout << " root" << std::endl;
	}
	return (root);
}

std::string	findContentType(std::string url) // comment ca se fait que les gifs passent ??
{
	std::string	contentType;

	if (endsWith(url, ".jpeg") || endsWith(url, ".jpg"))
		contentType = "images/jpeg";
	else if (endsWith(url, ".png"))
		contentType = "images/png";
	else if(endsWith(url, ".html")) 
		contentType = "text/html"; // gestion des erreurs ?
	else if(endsWith(url, ".css")) 
		contentType = "text/css";
	else if(endsWith(url, ".ico")) 
		contentType = "images/ico";
	return (contentType);
}

// std::string	getFileSize(void)
// {
// 	 std::ifstream	file(this->_filePath, std::ios::binary | std::ios::ate);
// }

static std::string sendGetResponseHeader(size_t contentLength, const std::string &contentType, const std::string &statusCode)
{
	std::ostringstream headerStream;

	headerStream << "HTTP/1.1 " << statusCode << " \r\n";
	headerStream << "Content-Type: " << contentType << "\r\n";
	headerStream << "Content-Length: " << contentLength << "\r\n";
	headerStream << "Connection: keep-alive\r\n";
	headerStream << "\r\n";
	std::string header = headerStream.str();

	return header.c_str();
}

void HandleRequests::sendFile(std::string filePath, std::string url, int bodySize, int clientFd, std::string statusCode)
{
	size_t	fileSize = 0;
	char	buffer[bodySize];
	char	sizeBuffer[bodySize];
	ssize_t	bytesRead;

	int fd = open(filePath.c_str(), O_RDONLY);
	if (fd < 0)
			throw(std::out_of_range("file doesn't exist"));
	while ((bytesRead = read(fd, sizeBuffer, sizeof(sizeBuffer))) > 0)
		fileSize += bytesRead;
	if (bytesRead < 0)
	{
		perror("Error reading file to determine size");
		close(fd);
		close(clientFd);
		return;
	}
	close(fd);
	fd = open(filePath.c_str(), O_RDONLY);
	if (fd < 0)
	{
		perror("File reopen error");
		close(clientFd);
		return;
	}
	std::string fullResponse = sendGetResponseHeader(fileSize, findContentType(url), statusCode);
	while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0)
		fullResponse.append(buffer, bytesRead);
	if (fullResponse.length() > _header.length())
		send(clientFd, fullResponse.c_str(), fullResponse.length(), 0);
	close(fd);
}

void	HandleRequests::initInfos(WebServer &webServData)
{
	this->_bodySize = webServData.getBodySize(this->_port);
	this->_url = this->_buffer.substr(4, this->_buffer.find(' ', 4) - 4);
	this->_rootUrl = this->_url.substr(0, this->_url.find_last_of("/") + 1);
	this->_rootDir = webServData.getRootPath(this->_port, this->_rootUrl);
	if (this->_url[this->_url.length() - 1] == '/')
		this->_filePath = this->_rootDir + "/index.html";
	else
		this->_filePath = this->_rootDir + "/" + this->_url.substr(this->_url.find_last_of("/") + 1);
}

void HandleRequests::getMethods(WebServer &webServData)
{
	initInfos(webServData);

	if (access(this->_filePath.c_str(), R_OK) != 0)
	{
		sendFile(webServData.getErrorPagePath(this->_port, 404), this->_url, this->_bodySize, this->_clientFd, "404 Not Found");	
		return;
	}
	std::vector<std::string> allowedMethods = webServData.getAllowedMethods(this->_port, this->_rootUrl);
	if (std::find(allowedMethods.begin(), allowedMethods.end(), "GET") == allowedMethods.end())
	{
		sendFile(webServData.getErrorPagePath(this->_port, 405), this->_url, this->_bodySize, this->_clientFd, "405 Method Not Allowed");
		return;
	}
	sendFile(this->_filePath, this->_url, this->_bodySize, this->_clientFd, "200 OK");
}
