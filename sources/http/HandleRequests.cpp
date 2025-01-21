#include "../../includes/webserv.hpp"

static int	whichMethod(char *buffer)
{
	std::string	str = buffer;

	if (str.find("GET") != std::string::npos)
		return (1);
	if (str.find("POST") != std::string::npos)
		return (2);
	return (0);
}

HandleRequests::HandleRequests(int clientFd)
{
	this->_clientFd = clientFd;
	try
	{
		memset(_buffer, 0, sizeof(_buffer));
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
					getMethods();
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
