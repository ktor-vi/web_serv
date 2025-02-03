#include "../../includes/webserv.hpp"

// int		HandleRequests::openIndex(WebServer &webServData)
// {
// 	std::string	index = webServData.getIndexPath(webServData.getPort(0) , "/");
// 	this->_fdPage = open(index.c_str(), O_RDONLY); // index
// 	return (this->_fdPage);
// }

// bool	HandleRequests::fileExists(const std::string &path)
// {
// 	struct stat buffer;

// 	if (stat(path.c_str(), &buffer) == 0)
// 		return (true);
// 	return (false);
// }

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

void	HandleRequests::initGetInfos(WebServer &webServData)
{
	this->_bodySize = webServData.getBodySize(this->_port);
	this->_rootDir = webServData.getRootPath(this->_port, this->_rootUrl);
	if (this->_url[this->_url.length() - 1] == '/')
		this->_filePath = this->_rootDir + "/index.html";
	else
		this->_filePath = this->_rootDir + "/" + this->_url.substr(this->_url.find_last_of("/") + 1);
}

static std::string createGetResponseHeader(size_t contentLength, const std::string &contentType, const std::string &statusCode)
{
	std::ostringstream headerStream;

	headerStream << "HTTP/1.1 " << statusCode << " \r\n";
	headerStream << "Content-Type: " << contentType << "\r\n";
	headerStream << "Content-Length: " << contentLength << "\r\n";
	headerStream << "Connection: keep-alive\r\n";
	headerStream << "\r\n";
	std::string header = headerStream.str();

	return (header.c_str)();
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

void HandleRequests::getMethod(WebServer &webServData)
{
	initGetInfos(webServData);

	if (access(this->_filePath.c_str(), R_OK) != 0)
	{
		this->_response = createGetResponseHeader(0, "text/html", "404 Not Found");
		return;
	}

	std::ifstream file(this->_filePath.c_str(), std::ios::binary);
	std::ostringstream content;
	content << file.rdbuf();
	file.close();

	this->_response = createGetResponseHeader(content.str().size(), findContentType(this->_filePath), "200 OK") + content.str();
}