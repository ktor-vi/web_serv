#include "../../includes/webserv.hpp"

void HandleRequests::postMethods(WebServer &webServData)
{
	if (!strncmp("POST /shutdown", this->buffer.c_str(), 12))
		throw(std::out_of_range("ask to close server"));
	write(1, "ok\n", 3);
	this->_url = this->buffer.substr(5, this->buffer.find(' ', 5) - 5);
	std::cout << "[ " << this->_url << " ]" << std::endl;
	this->_rootUrl = this->_url.substr(0, this->_url.find_last_of("/") + 1);
	std::cout << this->_rootUrl << std::endl;
	this->_rootDir = webServData.getRootPath(this->_port, this->_rootUrl);
	this->_fileName = this->buffer.substr(this->buffer.find("filename=\"") + 10, this->buffer.find("\"", this->buffer.find("filename=\"") + 10) - this->buffer.find("filename=\"") - 10);
	this->_filePath = this->_rootDir + this->_fileName;
	std::cout << this->_filePath << std::endl;
	int fileFd;
	if ((fileFd = open(this->_filePath.c_str(), O_WRONLY | O_CREAT | O_TRUNC)) < 0)
		throw std::runtime_error("Error opening file for write");
	size_t header_end = this->buffer.find("\r\n\r\n");
if (header_end != std::string::npos)
{
    size_t body_start = header_end + 4;  

    size_t file_data_start = this->buffer.find("\r\n\r\n", body_start);
    if (file_data_start != std::string::npos)
        file_data_start += 4; 

    std::string body = this->buffer.substr(file_data_start);
		write(fileFd, body.c_str(), body.length());
	}

	
}