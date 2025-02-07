#include "../../includes/webserv.hpp"

class WebServer;

static int whichMethod(std::string str)
{
	if (str.find("GET") != std::string::npos)
		return (1);
	if (str.find("POST") != std::string::npos)
		return (2);
	if (str.find("DELETE") != std::string::npos)
		return (3);
	return (0);
}

 int HandleRequests::getServerPort(std::string req) /*par rapport a la requete actuelle*/
{
	int start = req.find("Host:");
	int line_start = start + 5;
	while (req[line_start] == ' ')
	{
		line_start++;
	}
	std::string::size_type line_end = req.find("\r\n", line_start);
	if (line_end == std::string::npos)
	{
		line_end = req.find("\n", line_start);
	}
	std::string hostline = req.substr(line_start, line_end - line_start);
	int port = std::atoi(hostline.substr(hostline.find_first_of(":") + 1).c_str());
	return port;
}

void 	HandleRequests::setBuffer(std::string buffer)
{
	this->_buffer = buffer;
}

void 	HandleRequests::setBytesRead(int bytes_read)
{
	this->_bytesRead = bytes_read;
}


void HandleRequests::initURLs()
{

	int method = whichMethod(this->_buffer);
	if (method > 0)
	{
		switch (method)
		{
		case 1:
			this->_url = this->_buffer.substr(4, this->_buffer.find(' ', 4) - 4);
			this->_rootUrl = this->_url.substr(0, this->_url.find_last_of("/") + 1);
			break;
		case 2:

			this->_url = this->_buffer.substr(5, this->_buffer.find(' ', 5) - 5);
			this->_rootUrl = this->_url.substr(0, this->_url.find_last_of("/") + 1);
			break;
		default:
			this->_url = this->_buffer.substr(7, this->_buffer.find(' ', 7) - 7);
			this->_rootUrl = this->_url.substr(0, this->_url.find_last_of("/") + 1);
			break;
		}
	}
}

std::string HandleRequests::getResponse() const
{
	return (this->_response);
}

bool	HandleRequests::isMethodAllowed(const std::vector<std::string> methods, const std::string asked)
{
	int	size = methods.size();

	for (int i = 0; i < size; ++i)
	{
		if (methods[i] == asked)
			return (true);
	}
	return (false);
}

HandleRequests::HandleRequests(std::string request ,WebServer &webServData, int epoll_fd, int client_fd) : _clientFd(client_fd), _epollFd(epoll_fd),  _webServData(webServData), _buffer(request)
{
	try
	{
		this->_port = this->getServerPort(this->_buffer);
		initURLs();
		if (webServData.getCGIStatus(this->_port, this->_rootUrl))
			cgiMethods(webServData);	
		int method = whichMethod(this->_buffer);
		if (method > 0)
		{
			std::cout << "<<< HTTP REQUEST RECEIVED >>>" << std::endl
					  << this->_buffer << std::endl;
			switch (method)
			{
			case 1:
				std::cout << "[GET method asked]" << std::endl;
				getMethod(webServData);
				break;
			case 2:
				std::cout << "[POST method asked]" << std::endl;
				postMethod(webServData);
				break;
			case 3:
				std::cout << "[DELETE method asked]" << std::endl;
				deleteMethod(webServData);
				break;
			default:
				break;
			}
		}
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

HandleRequests::~HandleRequests()
{
	// Ajoutez ici tout code nécessaire à la libération des ressources.
}
