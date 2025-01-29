#include "../../includes/webserv.hpp"

void	HandleRequests::postMethods(WebServer &webServData)
{
	if (!strncmp("POST /shutdown", this->buffer.c_str(), 12))
		throw (std::out_of_range("ask to close server"));
	this->_url = this->_request.substr(4, this->_request.find(' ', 4) - 4);
	this->_rootUrl = this->_url.substr(0, this->_url.find_last_of("/") + 1);
    this->_rootDir = webServData.getRootPath(this->_port, this->_rootUrl);
}