#include "../../includes/webserv.hpp"

void HandleRequests::postMethods(WebServer &webServData)
{
	int fileFd;

	if (!strncmp("POST /shutdown", this->_buffer.c_str(), 12))
		throw(std::out_of_range("ask to close server"));
	write(1, "ok\n", 3);
	this->_url = this->_buffer.substr(5, this->_buffer.find(' ', 5) - 5);
	std::cout << "[ " << this->_url << " ]" << std::endl;
	this->_rootUrl = this->_url.substr(0, this->_url.find_last_of("/") + 1);
	std::cout << this->_rootUrl << std::endl;
	this->_rootDir = webServData.getRootPath(this->_port, this->_rootUrl);
	this->_fileName = this->_buffer.substr(this->_buffer.find("filename=\"") + 10, this->_buffer.find("\"", this->_buffer.find("filename=\"") + 10) - this->_buffer.find("filename=\"") - 10);
	this->_filePath = this->_rootDir + this->_fileName;
	std::cout << this->_filePath << std::endl;
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