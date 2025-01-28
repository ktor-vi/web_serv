#include "../../includes/webserv.hpp"

int		HandleRequests::openIndex(WebServer &webServData)
{
	std::string	index = webServData.getIndexPath(webServData.getPort(0) , "/");
	this->_fdPage = open(index.c_str(), O_RDONLY); // index
	return (this->_fdPage);
}


// int sendHttpResponseHeader(int client_fd, const char *content_type, ssize_t content_length)
// {
//     char	response_header[1024];

//     snprintf(response_header, sizeof(response_header),
//              "HTTP/1.1 200 OK\r\n"
//              "Content-Type: %s\r\n"
//              "Content-Length: %zd\r\n\r\n",
//              content_type, content_length);
//     send(client_fd, response_header, strlen(response_header), 0);
// }

bool fileExists(const std::string& path)
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
	return (contentType);
}

// std::string	getFileSize(void)
// {
// 	 std::ifstream	file(this->_filePath, std::ios::binary | std::ios::ate);
// }

int sendHttpResponseHeader(int clientFd, size_t contentLength, const std::string &contentType, const std::string &statusCode)
{
    std::ostringstream headerStream;
    headerStream << "HTTP/1.1 " << statusCode << " \r\n";
    headerStream << "Content-Type: " << contentType << "\r\n";
    headerStream << "Content-Length: " << contentLength << "\r\n";
    headerStream << "Connection: keep-alive\r\n";
    headerStream << "\r\n";
    std::string header = headerStream.str();

    // Send the header
    send(clientFd, header.c_str(), header.size(), 0);
    return header.size();
}

static void sendFile(std::string filePath, std::string url, int bodySize, int clientFd, std::string statusCode)
{int fd = open(filePath.c_str(), O_RDONLY);
    if (fd < 0)
    {
        perror("File does not exist");
        return;
    }
    size_t fileSize = 0;
    char sizeBuffer[bodySize];
    ssize_t bytesRead;
    while ((bytesRead = read(fd, sizeBuffer, sizeof(sizeBuffer))) > 0)
    {
        fileSize += bytesRead;
    }
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
    char buffer[bodySize]; // Chunk size
    sendHttpResponseHeader(clientFd, fileSize, findContentType(url), statusCode);
    while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0)
    {
        send(clientFd, buffer, bytesRead, 0);
    }
    close(fd);}

void	HandleRequests::initInfos(WebServer &webServData)
{
	this->_fdPage = openIndex(webServData);
	if (this->_fdPage < 0)
	{
		perror("File open error");
		close(this->_clientFd);
		return;
	}
	this->_url = this->_request.substr(4, this->_request.find(' ', 4) - 4);
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

// void	HandleRequests::getMethods()
// {
// 	// if 127.0.0.1::8080, send facebook html, if 8081 twitter html
// 	this->_fdPage = open("public/html/index.html", O_RDONLY); // index
// 	if (this->_fdPage < 0)
// 	{
// 		perror("File open error");
// 		close(this->_clientFd);
// 	}

// 	this->_url = this->_request.substr(4, this->_request.find(' ', 4) - 4);
// 	std::cout << "ICI URL : " << this->_url << std::endl;

// 	this->_rootDir = "/home/rdendonc/Documents/WebServ/memes";
// 	this->_filePath = this->_rootDir + this->_url;
// 	std::cout << "ICI PATH : " << this->_filePath << std::endl;
// 	if (fileExists(this->_filePath))
// 	{
// 		std::cout << this->_filePath << std::endl;
// 		std::cout << "LE FICHIER EXISTE" << std::endl;
// 		std::string contentType = findContentType();
// 	}
// 	else
// 		std::cout << "LE FICHIER EXISTE PAS :(" << std::endl;
// 	this->_bytes = read(this->_fdPage, this->_bufferPage, sizeof(this->_bufferPage) - 1);
// 	if (this->_bytes > 0)
// 	{
// 		sendHttpResponseHeader(this->_clientFd, "text/html", this->_bytes);
// 		send(this->_clientFd, this->_bufferPage, this->_bytes, 0);
// 	}
// 	close(this->_fdPage);

// }