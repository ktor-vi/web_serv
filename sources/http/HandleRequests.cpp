#include "../../includes/webserv.hpp"

class WebServer;

static int	whichMethod(char *buffer)
{
	std::string	str = buffer;

	if (str.find("GET") != std::string::npos)
		return (1);
	if (str.find("POST") != std::string::npos)
		return (2);
	return (0);
}

#define CHUNK_SIZE 4096

std::string	HandleRequests::createBuffer(int clientFd)
{
	char		buffer[CHUNK_SIZE];
	size_t		bytesRead;
	std::string	totalData;

	while (true)
	{
		memset(buffer, 0, sizeof(buffer));
		bytesRead = recv(clientFd, buffer, sizeof(buffer) - 1, 0); // read equivalent
		if (bytesRead > 0)
		{

		}

	}
	return (totalData);
}

HandleRequests::HandleRequests(int clientFd, WebServer &webServData) : _clientFd(clientFd), _webServData(webServData)
{
	try
	{
		memset(this->_buffer, 0, sizeof(this->_buffer));
		this->buffer = createBuffer(clientFd);
		this->_bytes = recv(clientFd, this->_buffer, 2048, 0); // read equivalent
		if (this->_bytes <= 0)
			throw (std::out_of_range("recv"));

		this->_buffer[this->_bytes] = '\0'; // Null-terminate the received data
		this->_request = this->_buffer;
		int	method = whichMethod(this->_buffer);
		if (method > 0)
		{
			std::cout << "<<< HTTP REQUEST RECEIVED >>>" << std::endl << this->_buffer << std::endl;
			switch(method)
			{
				case 1:
					std::cout << "[GET method asked]" << std::endl;
					getMethods(webServData);
					break ;
				case 2:
					std::cout << "[POST method asked]" << std::endl;
					postMethods(this->_buffer);
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

HandleRequests::~HandleRequests() {
    // Ajoutez ici tout code nécessaire à la libération des ressources.
}
