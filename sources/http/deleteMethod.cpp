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
	std::cout << std::endl;
	std::cout << std::endl;
	this->_filePath = this->_rootDir + this->_fileName;
	std::cout << "COUCOU 1 !! [" << this->_fileName << "]" << std::endl;
	std::cout << "COUCOU 2 !! [" << this->_filePath << "]" << std::endl;
	std::cout << "COUCOU 3 !! [" << this->_rootDir << "]" << std::endl;
}

void HandleRequests::deleteMethod(WebServer &webServData)
{
	
	initDeleteInfos(webServData);
	std::cout << this->_buffer << std::endl;
	return ;
}
