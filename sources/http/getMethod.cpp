#include "../../includes/webserv.hpp"

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
  std::cout << this->_url << std::endl;
  if (this->_url[this->_url.length() - 1] == '/')
  {
    this->_rootDir = webServData.getRootPath(this->_port, this->_rootUrl);
    if(!webServData.getIndexPath(this->_port, this->_rootUrl).empty())
      this->_filePath = webServData.getIndexPath(this->_port, this->_rootUrl);
    else
      this->_filePath = this->_rootDir + "/" + this->_url.substr(this->_url.find_last_of("/") + 1);
  }
  else 
{

    if(!webServData.getRootPath(this->_port, this->_url).empty())
    {
      if(!webServData.getIndexPath(this->_port, this->_url).empty())
      {
        this->_rootDir = webServData.getRootPath(this->_port, this->_url);
        this->_filePath = webServData.getIndexPath(this->_port, this->_url);
      }
      else
    {
        this->_rootDir = webServData.getRootPath(this->_port, this->_url);
        this->_filePath = this->_rootDir + "/" + this->_url.substr(this->_url.find_last_of("/") + 1);

      }
    }
    else 
  {

      this->_rootDir = webServData.getRootPath(this->_port, this->_rootUrl);
      this->_filePath = this->_rootDir + "/" + this->_url.substr(this->_url.find_last_of("/") + 1);
    }
  }
  std::cout << "["<< this->_filePath << "]" << std::endl;
}
static std::string createRedirectResponse(int statusCode, const std::string& newLocation) {
  std::ostringstream response;
  response << "HTTP/1.1 " << statusCode << " Moved Permanently\r\n";
  response << "Location: " << newLocation << "\r\n";
  response << "Content-Length: 0\r\n";
  response << "Connection: close\r\n";	
  response << "\r\n";
  return response.str();
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

  ///std::cout << ">>> HTTP RESPONSE <<< " << std::endl << header.c_str() << std::endl;
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
  if (isMethodAllowed(webServData.getAllowedMethods(this->getServerPort(this->_buffer), this->_rootUrl), "GET") == false)
  {
    this->_response = createGetResponseHeader(errorPageToBody(403, webServData).size(), "text/html", "403 Forbidden")  + errorPageToBody(403, webServData);
    return;
  }
  if (!webServData.getRedirect(this->_port, this->_rootUrl).second.empty())
  {
    this->_response = createRedirectResponse(webServData.getRedirect(this->_port, this->_rootUrl).first, webServData.getRedirect(this->_port, this->_rootUrl).second);
    return;
  }
  if(webServData.getAutoindex(this->_port, this->_rootUrl) && this->_url[this->_url.length() - 1] == '/')
  {
    buildAutoIndexResponse(this->_rootDir);
    return;
  }
  if (access(this->_filePath.c_str(), F_OK) != 0)
  {
    this->_response = createGetResponseHeader(errorPageToBody(404, webServData).size(), "text/html", "404 Not Found") + errorPageToBody(404, webServData);
    return;
  }
  if (access(this->_filePath.c_str(), R_OK) == -1)
  {
    this->_response = createGetResponseHeader(errorPageToBody(403, webServData).size(), "text/html", "403 Forbidden")  + errorPageToBody(403, webServData);
    return;
  }
  std::cout << "["<< this->_filePath << "]" << std::endl;
  std::ifstream file(this->_filePath.c_str(), std::ios::binary);
  std::ostringstream content;
  content << file.rdbuf();
  file.close();
  write(1, "ok\n", 3);
  this->_response = createGetResponseHeader(content.str().size(), findContentType(this->_filePath), "200 OK") + content.str();

  // Activer EPOLLOUT pour écrire dans handle_write_event()
  struct epoll_event event;
  event.events = EPOLLOUT | EPOLLET;
  event.data.fd = this->_clientFd;
  if (epoll_ctl(this->_epollFd, EPOLL_CTL_MOD, this->_clientFd, &event) == -1) {
    perror("EPOLL_CTL MOD ERROR");
    return;
  }
}
