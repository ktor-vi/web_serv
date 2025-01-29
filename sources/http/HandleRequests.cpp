#include "../../includes/webserv.hpp"

class WebServer;

static int	whichMethod(std::string str)
{
	if (str.find("GET") != std::string::npos)
		return (1);
	if (str.find("POST") != std::string::npos)
		return (2);
	return (0);
}

static int getPort(std::string req) /*par rapport a la requete actuelle*/
{
    int start = req.find("Host:");
    int line_start = start + 5;
    while (req[line_start] == ' ') { 
        line_start++;
    }
    std::string::size_type line_end = req.find("\r\n", line_start);
    if (line_end == std::string::npos) {
        line_end = req.find("\n", line_start); 
    }
    std::string hostline = req.substr(line_start, line_end - line_start);
	int port = std::atoi(hostline.substr(hostline.find_first_of(":") + 1).c_str());
	return port;
}

std::string	HandleRequests::createBuffer(int clientFd)
{
	size_t		bufferSize = CHUNK_SIZE;
	char		*buffer = new char[CHUNK_SIZE];
	size_t		bytesRead;
	std::string	totalData;
	int 		flag = -4;

	while (true)
	{
		memset(buffer, 0, bufferSize);
		bytesRead = recv(clientFd, buffer, bufferSize - 1, 0); // read equivalent
		if (bytesRead > 0)
		{
			totalData += std::string(buffer, bytesRead);
			// std::cout << "Chunk received (" << bytesRead << " bytes): " << std::string(buffer, bytesRead) << "\n";
			if(bytesRead < CHUNK_SIZE -1)
				break;
		}
		else if (bytesRead == 0)
		{
			std::cout << "Client deco ??" << std::endl;
			break ;
		}			
		else if (bytesRead <= 0)
		{
			delete[] buffer;
			throw (std::out_of_range("recv"));
		}
		// std::cout << "Total data received:\n" << totalData << "\n";
	}
	delete[] buffer;
	this->_bytesRead = bytesRead;
	return (totalData);
}

HandleRequests::HandleRequests(int clientFd, WebServer &webServData) : _clientFd(clientFd), _webServData(webServData)
{
	try
	{
		this->buffer = createBuffer(clientFd);
		this->_port = getPort(this->buffer);
		this->_request = this->buffer;
		int	method = whichMethod(this->buffer);
		if (method > 0)
		{
			std::cout << "<<< HTTP REQUEST RECEIVED >>>" << std::endl << this->buffer << std::endl;
			switch(method)
			{
				case 1:
					std::cout << "[GET method asked]" << std::endl;
					getMethods(webServData);
					break ;
				case 2:
					std::cout << "[POST method asked]" << std::endl;
					postMethods(webServData);
					break ;
				default:
					break ;
			}
		}
	}
	catch(const std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

// HandleRequests::HandleRequests(int clientFd, WebServer &webServData) : _clientFd(clientFd), _webServData(webServData)
// {
// 	try
// 	{
// 		memset(_buffer, 0, sizeof(_buffer));
// 		this->_bytes = recv(clientFd, this->_buffer, 2048, 0); // read equivalent
// 		if (this->_bytes <= 0)
// 			throw (std::out_of_range("recv"));

// 		this->_buffer[this->_bytes] = '\0'; // Null-terminate the received data
// 		this->_request = this->_buffer;
// 		this->_port = getPort(this->_buffer);
// 		this->_bodySize = 1024;
// 		int	method = whichMethod(this->_buffer);
// 		if (method > 0)
// 		{
// 			std::cout << "<<< HTTP REQUEST RECEIVED >>>" << std::endl << this->_buffer << std::endl;
// 			switch(method)
// 			{
// 				case 1:
// 					std::cout << "[GET method asked]" << std::endl;
// 					getMethods(webServData);
// 					break ;
// 				case 2:
// 					std::cout << "[POST method asked]" << std::endl;
// 					postMethods(this->_buffer);
// 					break ;
// 				default:
// 					break ;
// 			}
// 		}
// 	}
// 	catch(const std::exception &e)
// 	{
// 		std::cerr << "Error: " << e.what() << std::endl;
// 	}
// }

HandleRequests::~HandleRequests() {
    // Ajoutez ici tout code nécessaire à la libération des ressources.
}
