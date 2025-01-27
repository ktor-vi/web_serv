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
	else
		type = "text/html"; // gestion des erreurs ?
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
			std::cout << "no existing root" << std::endl;
	}
	return (root);
}

std::string	HandleRequests::findContentType(void)
{
	std::string	contentType;

	if (endsWith(this->_url, ".jpeg") || endsWith(this->_url, ".jpg"))
		contentType += "images/jpeg";
	if (endsWith(this->_url, ".png"))
		contentType += "images/png";
	return (contentType);
}

// std::string	getFileSize(void)
// {
// 	 std::ifstream	file(this->_filePath, std::ios::binary | std::ios::ate);
// }

int HandleRequests::sendHttpResponseHeader(void)
{
	std::string header = "HTTP/1.1 200 OK\r\n";
	header += "Content-Type: " + findContentType() + "\r\n";
	header += "Content-Length: " +  this->_request + "\r\n";
	header += "Connection: bite keep-alive\r\n";
	header += "\r\n";
	send(this->_clientFd, header.c_str(), header.size(), 0);
	return header.length();	
}

void	HandleRequests::getMethods(WebServer &webServData)
{
	// if 127.0.0.1::8080, send facebook html, if 8081 twitter html
	// this->_fdPage = open("public/html/index.html", O_RDONLY); // index
	this->_fdPage = openIndex(webServData);
	if (this->_fdPage < 0)
	{
		perror("File open error");
		close(this->_clientFd);
	}
	this->_url = this->_request.substr(4, this->_request.find(' ', 4) - 4);
	this->_folderType = findFolder(this->_url);
	this->_rootDir = findRoot(this->_folderType);
	this->_filePath = this->_rootDir + this->_url;
	std::cout << "File path: " << this->_filePath << std::endl;
	if (fileExists(this->_filePath))
	{
		std::cout << "LE FICHIER EXISTE" << std::endl;
	}
	else
		std::cout << "LE FICHIER EXISTE PAS :(" << std::endl;
	char bufferPage[this->_bodySize];
	int headerLen = sendHttpResponseHeader();
	this->_bytes = read(this->_fdPage, bufferPage, sizeof(bufferPage - headerLen) - 1);
	if (this->_bytes > 0)
		send(this->_clientFd, bufferPage, this->_bytes - headerLen, 0);
	while(read(this->_fdPage, bufferPage, sizeof(bufferPage) - 1) > 0)
		send(this->_clientFd, bufferPage, this->_bytes, 0)	;

	close(this->_fdPage);
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