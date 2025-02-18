#include "../../includes/webserv.hpp"

void HandleRequests::initPostInfos(WebServer &webServData)
{
	this->_rootDir = webServData.getRootPath(this->_port, this->_rootUrl);
	this->_fileName = this->_buffer.substr(this->_buffer.find("filename=\"") + 10, this->_buffer.find("\"", this->_buffer.find("filename=\"") + 10) - this->_buffer.find("filename=\"") - 10);
	this->_filePath = this->_rootDir + this->_fileName;
}

static std::string createPostResponseHeader(size_t contentLength, const std::string &contentType, const std::string &statusCode)
{
  std::ostringstream headerStream;

	headerStream << "HTTP/1.1 " << statusCode << " \r\n";
	headerStream << "Content-Type: " << contentType << "\r\n";
	headerStream << "Content-Length: " << contentLength << "\r\n";
	headerStream << "Connection: keep-alive\r\n";
	headerStream << "\r\n";
	std::string header = headerStream.str();

std::cout << std::endl << ">>> HTTP POST RESPONSE <<< " << std::endl << header.c_str() << "////////////////" << std::endl;
	return (header.c_str());
}

void	HandleRequests::createPostResponse(std::string code)
{
	std::ifstream file(this->_filePath.c_str(), std::ios::binary);
	std::ostringstream content;
	content << file.rdbuf();
	file.close();
	this->_response = createPostResponseHeader(content.str().size(), findContentType(this->_filePath), code) + content.str();
}

void HandleRequests::handleMultipartData(void)
{
	size_t boundary_start = this->_buffer.find("boundary=") + 9;
	size_t boundary_end = this->_buffer.find("\r\n", boundary_start);
	std::string boundary = this->_buffer.substr(boundary_start, boundary_end - boundary_start);

	size_t header_end = this->_buffer.find("\r\n\r\n");
	std::string body = this->_buffer.substr(header_end + 4);  // Extraire le corps après les en-têtes

	std::string word;
	std::string file_data;

	size_t pos = 0;
	while ((pos = body.find(boundary, pos)) != std::string::npos)
	{
		size_t part_start = pos + boundary.length();
		size_t part_end = body.find(boundary, part_start);
		std::string part = body.substr(part_start, part_end - part_start);
		if (part.find("Content-Disposition: form-data; name=\"word\"") != std::string::npos)
		{
			size_t word_start = part.find("\r\n\r\n") + 4;
			word = part.substr(word_start);  // Extraire la valeur du champ "word"
			word = word.substr(0, word.find("\r\n"));  // Nettoyer les fins de ligne et espaces
			std::cout << "Word received: " << word << std::endl;
			setenv("WORD", word.c_str(), 1);
		}
		if (part.find("Content-Disposition: form-data; name=\"file\"") != std::string::npos)
		{
			size_t file_start = part.find("\r\n\r\n") + 4;
			file_data = part.substr(file_start);  // Extraire le contenu du fichier si besoin
			std::cout << "Data from file received" << std::endl;
		}

		pos = part_end;
	}
}

void HandleRequests::postMethod(WebServer &webServData)
{

	initPostInfos(webServData);

	if (isMethodAllowed(webServData.getAllowedMethods(this->getServerPort(this->_buffer), this->_rootUrl), "POST") == false)
	{
		createPostResponse("403 Forbidden");
		return;
	}

	if (this->_rootUrl.find("/cgi-bin/") == 0)
	{
		handleMultipartData();
		std::string cgiOutput = cgiMethod(webServData);
		this->_response = createPostResponseHeader(cgiOutput.size(), "text/plain", "200 OK") + cgiOutput;
		return;
	}
	int fileFd = open(this->_filePath.c_str(), O_WRONLY | O_CREAT | O_NONBLOCK | O_TRUNC, 0644);
	if (fileFd < 0) {
		perror("Error opening file");
		return;
	}

	webServData.setPostFileFds(this->_clientFd, fileFd);

	// Extraire le corps de la requête HTTP
	size_t header_end = this->_buffer.find("\r\n\r\n");
	if (header_end != std::string::npos)
	{
		size_t body_start = header_end + 4;  
		size_t file_data_start = this->_buffer.find("\r\n\r\n", body_start);
		if (file_data_start != std::string::npos)
			file_data_start += 4;	 
		std::string body = this->_buffer.substr(file_data_start);
		webServData.setPostBody(this->_clientFd, body);
	}

	// Activer EPOLLOUT pour écrire dans handle_write_event()
	struct epoll_event event;
	event.events = EPOLLOUT | EPOLLET;
	event.data.fd = this->_clientFd;
	if (epoll_ctl(this->_epollFd, EPOLL_CTL_MOD, this->_clientFd, &event) == -1)
	{
		perror("EPOLL_CTL MOD ERROR");
		return;
	}
	createPostResponse("200 OK");
}


