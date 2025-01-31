#include "../../includes/webserv.hpp"

void HandleRequests::initPostInfos(WebServer &webServData)
{
	this->_rootDir = webServData.getRootPath(this->_port, this->_rootUrl);
	this->_fileName = this->_buffer.substr(this->_buffer.find("filename=\"") + 10, this->_buffer.find("\"", this->_buffer.find("filename=\"") + 10) - this->_buffer.find("filename=\"") - 10);
	this->_filePath = this->_rootDir + this->_fileName;
}
void HandleRequests::postMethods(WebServer &webServData)
{
	initPostInfos(webServData);
	int fileFd;

	if (!strncmp("POST /shutdown", this->_buffer.c_str(), 12))
		throw(std::out_of_range("ask to close server"));
	write(1, "ok\n", 3);

	if ((fileFd = open(this->_filePath.c_str(), O_WRONLY | O_CREAT | O_TRUNC)) < 0)
		throw std::runtime_error("Error: can't open file for write");
	size_t header_end = this->_buffer.find("\r\n\r\n");
	if (header_end != std::string::npos)
	{
    	size_t body_start = header_end + 4;  
    	size_t file_data_start = this->_buffer.find("\r\n\r\n", body_start);
    	if (file_data_start != std::string::npos)
        	file_data_start += 4; 
    	std::string body = this->_buffer.substr(file_data_start);
			write(fileFd, body.c_str(), body.length());
	}
	// sendResponse
	return ;
}