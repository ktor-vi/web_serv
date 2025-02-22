#include "../../class/WebServer.hpp"

WebServer::WebServer(Config &conf)
{
	std::vector<ServerBlock> server_blocks = conf.getServers();
	std::vector<ServerBlock>::iterator it;
	std::vector<ServerBlock>::iterator ite = server_blocks.end();
  
	for (it = server_blocks.begin(); it != ite; it++)
	{
		Server server;
		server.server_name = it->server_name;
		server.listen = it->listen;
    std::pair<int, std::string>err400;
    err400.first = 400;
    err400.second = "sources/html/error_pages/400.html";
		server.error_pages.insert(err400);
    std::pair<int, std::string>err403;
    err403.first = 403;
    err403.second = "sources/html/error_pages/403.html";
		server.error_pages.insert(err403);
    std::pair<int, std::string>err404;
    err404.first = 404;
    err404.second = "sources/html/error_pages/404.html";
		server.error_pages.insert(err404);
    std::pair<int, std::string>err409;
    err409.first = 409;
    err409.second = "sources/html/error_pages/409.html";
		server.error_pages.insert(err409);
    std::pair<int, std::string>err413;
    err413.first = 413;
    err413.second = "sources/html/error_pages/413.html";
		server.error_pages.insert(err413);
    std::pair<int, std::string>err500;
    err500.first = 500;
    err500.second = "sources/html/error_pages/500.html";
		server.error_pages.insert(err500);
    std::map<std::string, std::string>::iterator jt;
		std::map<std::string, std::string>::iterator jte =
			it->other_directives.end();
		for (jt = it->other_directives.begin(); jt != jte; jt++)
		{
			if (jt->second.empty())
				throw std::runtime_error("Error: empty directive.");
			if (jt->first == "body_size")
				server.body_size = std::atoi(jt->second.c_str());
			else if (jt->first == "root")
			{
			server.root = jt->second;
			}
			else if (jt->first == "error_page")
			{
			std::pair<int, std::string> err;
			err.first = std::atoi(
				jt->second.substr(0, jt->second.find_first_of(" ")).c_str());
			err.second = jt->second.substr(jt->second.find_first_of(" ") + 1);
			if (err.second[0] == '/')
				err.second = err.second.substr(1);
      		server.error_pages.erase(err.first);
			server.error_pages.insert(err);
			}
			else
			  throw std::runtime_error("Error: unknown server directive.");
		}

		std::vector<LocationBlock> location_blocks = it->locations;
		std::vector<LocationBlock>::iterator kt;
		std::vector<LocationBlock>::iterator kte = location_blocks.end();
		for (kt = location_blocks.begin(); kt != kte; kt++)
		{
			Location location;
			location.cgi = false;
			location.autoindex = false;
			location.path = kt->path;
			std::map<std::string, std::string>::iterator lt;
			std::map<std::string, std::string>::iterator lte = kt->directives.end();
			for (lt = kt->directives.begin(); lt != lte; lt++)
			{
				if (lt->first == "root")
				{
					location.root = lt->second;
				}
				else if (lt->first == "index")
				{
					location.index = lt->second;
				}
				else if(lt->first == "return")
				{
					location.redirect.first = atoi(lt->second.substr(0, lt->second.find_first_of(" ")).c_str());
					location.redirect.second = lt->second.substr(lt->second.find_first_of(" "));

				}
				else if (lt->first == "dir_default")
				{
					location.dir_default_path = lt->second;
				}
				else if (lt->first == "cgi_path")
				{
					location.cgi_path = lt->second;
				}
				else if (lt->first == "autoindex")
				{
					if (lt->second != "on" && lt->second != "off")
					  throw std::runtime_error("Error: autoindex can only be on or off");
					
					if(lt->second == "on")
						location.autoindex = true;
					else 
						location.autoindex = false;
				}
				else if (lt->first == "cgi")
				{
					if (lt->second != "on" && lt->second != "off")
					  throw std::runtime_error("Error: cgi can only be on or off");
					location.cgi = lt->second == "on" ? true : false;
				}
				else if (lt->first == "cgi_depends")
				{
					location.cgi_depends.first =
						(!lt->second.empty() || lt->second == "off") ? true : false;
					location.cgi_depends.second = lt->second;
				}
				else if (lt->first == "allowed_methods")
				{
          if(lt->second.empty())
            location.allowed_methods = std::vector<std::string>();
					std::istringstream iss(lt->second);
					std::string method;
					while (iss >> method)
					{
						if (method == "GET" || method == "POST" || method == "DELETE")
						{
							location.allowed_methods.push_back(method);
						}
						else
						{
							throw std::runtime_error(
								"Error: unknown or unsupported HTTP method '" + method + "'");
						}
					}
				}
				else
				{
					throw std::runtime_error("Error: unknown location directive.");
				}
			}
			if (server.root.empty() && location.root.empty())
				throw std::runtime_error("Error: no root directives.");
			if (server.root.empty())
			{
				if (location.root[0] == '/')
					location.root = location.root.substr(1);
				if (!location.index.empty())
				{
					if (location.index[0] == '/')
					location.index_path = location.root + location.index;
					else
					location.index_path = location.root + "/" + location.index;
				}
				if (!location.dir_default_path.empty())
				{
					if (location.dir_default_path[0] == '/')
					location.dir_default_path =
						location.root + location.dir_default_path;
					else
					location.dir_default_path =
						location.root + "/" + location.dir_default_path;
				}
				if (!location.cgi_path.empty())
				{
					if (location.cgi_path[0] == '/')
					location.cgi_path = location.root + location.cgi_path;
					else
					location.cgi_path = location.root + "/" + location.cgi_path;
				}
				}
				else if (location.root.empty())
				{
				if (!location.index_path.empty())
				{
					if (location.index[0] == '/')
					location.index_path = server.root + location.index;
					else
					location.index_path = server.root + "/" + location.index;
				}
				if (!location.dir_default_path.empty())
				{
					if (location.dir_default_path[0] == '/')
						location.dir_default_path = server.root + location.dir_default_path;
					else
						location.dir_default_path =
						server.root + "/" + location.dir_default_path;
				}
				if (!location.cgi_path.empty())
				{
					if (location.cgi_path[0] == '/')
					location.cgi_path = server.root + location.cgi_path;
					else
					location.cgi_path = server.root + "/" + location.cgi_path;
				}
			}
			server.locations.push_back(location);
		}
		Location errors;
		errors.allowed_methods.push_back("GET");
		errors.path = "/_errors/";
		errors.root = "sources/html/error_pages/";
		server.locations.push_back(errors);
	servers.push_back(server);
	}
}

void	WebServer::verifyServer() const
{
	std::vector<Server>::const_iterator it;
	std::vector<Server>::const_iterator ite = servers.end();

	for (it = servers.begin(); it != ite; it++)
	{
		if (it->listen < 1024 || it->listen > 65535)
			throw std::runtime_error("Bad Port");
		if (it->body_size < 1024 || it->body_size > 2000000)
			throw std::runtime_error("Bad Body size (1024B-2MB)");		
		std::map<int, std::string>::const_iterator jt;
		std::map<int, std::string>::const_iterator jte = it->error_pages.end();
		for (jt = it->error_pages.begin(); jt != jte; jt++)
		{
			if (jt->first < 100 || jt->first >= 600)
			throw std::runtime_error("Bad Error Code");
			if (access(jt->second.c_str(), R_OK) != 0)
			throw std::runtime_error("Bad Error Page Path");
		}
		std::vector<Location> locations = it->locations;
		std::vector<Location>::iterator kt;
		std::vector<Location>::iterator kte = locations.end();
		for (kt = locations.begin(); kt != kte; kt++)
		{
			if (!kt->root.empty() && access(kt->root.c_str(), F_OK) == -1)
				throw std::runtime_error("Bad Root Path");
			if (!kt->index_path.empty() && access(kt->index_path.c_str(), R_OK) != 0)
				throw std::runtime_error("Bad Index Page Path");
			if (!kt->dir_default_path.empty() &&
				access(kt->dir_default_path.c_str(), R_OK) != 0)
			throw std::runtime_error("Bad Directory Page Path");
			if (!kt->cgi_path.empty() && access(kt->cgi_path.c_str(), R_OK) != 0)
				throw std::runtime_error("Bad CGI Script Path");
			if((kt->redirect.first != 301 && kt->redirect.first != 302 )&& !kt->redirect.second.empty())
				throw std::runtime_error("Bad Error Code for redirect");
		}
	}
}

void	WebServer::printServer() const
{
	int i = 0;
	std::vector<Server>::const_iterator it;
	std::vector<Server>::const_iterator ite = servers.end();
	for (it = servers.begin(); it != ite; it++)
	{

		std::cout << "Server : " << ++i << std::endl;
		std::cout << "Port : " << it->listen << std::endl;
		std::cout << "Server Name : " << it->server_name << std::endl;
		(it->root.empty()) ? std::cout << "No server root" << std::endl
							: std::cout << "Server root : "
										<< "[" << it->root << "]" << std::endl;
		std::cout << "Body Size : " << it->body_size << std::endl;
		std::map<int, std::string>::const_iterator jt;
		std::map<int, std::string>::const_iterator jte = it->error_pages.end();
		for (jt = it->error_pages.begin(); jt != jte; jt++)
		{
			std::cout << "[" << jt->first << "] : "
					<< "[" << jt->second << "]" << std::endl;
		}
		std::vector<Location> locations = it->locations;
		std::vector<Location>::iterator kt;
		std::vector<Location>::iterator kte = locations.end();
		int k = 0;
		std::cout << std::endl;
		for (kt = locations.begin(); kt != kte; kt++)
		{
			std::cout << "Location : " << ++k << ": [" << kt->path << "]"
					<< std::endl;
			(kt->root.empty()) ? std::cout << "No location root" << std::endl
							: std::cout << "Location root : "
										<< "[" << kt->root << "]" << std::endl;
			(kt->index_path.empty()) ? std::cout << "No index Path " << std::endl
									: std::cout << "Index Path : "
												<< "[" << kt->index_path << "]"
												<< std::endl;
			(kt->dir_default_path.empty())
				? std::cout << "No Dir Default Path : " << std::endl
				: std::cout << "Dir Default Path : "
							<< "[" << kt->dir_default_path << "]" << std::endl;
			(kt->redirect.second.empty())
				? std::cout << "No redirect : " << std::endl
				: std::cout << "Redirect code : "
							<< "[" << kt->redirect.first << "] " << "on " << kt->redirect.second << std::endl;
			(kt->autoindex) ? std::cout << "Autoindex on" << std::endl
							: std::cout << "Autoindex off" << std::endl;
			(kt->cgi) ? std::cout << "CGI on" << std::endl
					: std::cout << "CGI off" << std::endl;
			(kt->cgi_depends.first) ? std::cout << "CGI depends on"
												<< "[" << kt->cgi_depends.second
												<< "]" << std::endl

									: std::cout << "CGI depends off" << std::endl;
			std::vector<std::string>::iterator lt;
			std::vector<std::string>::iterator lte = kt->allowed_methods.end();
			std::cout << "Allowed Methods : ";
			for (lt = kt->allowed_methods.begin(); lt != lte; lt++)
			{
			std::cout << "[" << *lt << "] ";
			}
			std::cout << std::endl;
		}

		std::cout << std::endl;
	}
}

void	WebServer::setSocketFd(int index, int socket_fd)
{
	this->servers[index].socket_fd = socket_fd;
}

int	WebServer::getSocketFd(int index)
{
	return this->servers[index].socket_fd;
}

int	WebServer::getPort(int index)
{ 
	return this->servers[index].listen;
}

int	WebServer::getNumberOfServers()
{
	return this->servers.size();
}

std::string WebServer::getServerName(int port)
{
	int index = 0;
	std::vector<Server>::const_iterator it;
	std::vector<Server>::const_iterator ite = servers.end();
	for (it = servers.begin(); it != ite; it++)
		{
		if (port == it->listen)
			break;
		index++;
	}

	return this->servers[index].server_name;
}

int	WebServer::getBodySize(int port)
{
	int index = 0;
	std::vector<Server>::const_iterator it;
	std::vector<Server>::const_iterator ite = servers.end();
	for (it = servers.begin(); it != ite; it++)
	{
		if (port == it->listen)
			break;
		index++;
	}

	return this->servers[index].body_size;
}
std::string	WebServer::getErrorPagePath(int port, int error_code)
{
	int index = 0;
	std::vector<Server>::const_iterator it;
	std::vector<Server>::const_iterator ite = servers.end();
	for (it = servers.begin(); it != ite; it++)
	{
		if (port == it->listen)
			break;
		index++;
	}
	std::map<int, std::string>::const_iterator jt;
	std::map<int, std::string>::const_iterator jte =
		this->servers[index].error_pages.end();
	for (jt = this->servers[index].error_pages.begin(); jt != jte; jt++)
	{
		if (jt->first == error_code)
			return jt->second;
	}
	return "";
}

std::string	WebServer::getIndexPath(int port, std::string location_path)
{
	int index = 0;
	std::vector<Server>::const_iterator it;
	std::vector<Server>::const_iterator ite = servers.end();
	for (it = servers.begin(); it != ite; it++)
	{
		if (port == it->listen)
			break;
		index++;
	}
	std::vector<Location> locations = this->servers[index].locations;
	std::vector<Location>::iterator kt;
	std::vector<Location>::iterator kte = locations.end();
	for (kt = locations.begin(); kt != kte; kt++)
	{
		if (kt->path == location_path)
			return kt->index_path;
	}
	return "";
}

std::string	WebServer::getRootPath(int port, std::string location_path)
{
	int index = 0;
	std::vector<Server>::const_iterator it;
	std::vector<Server>::const_iterator ite = servers.end();
	for (it = servers.begin(); it != ite; it++)
	{
		if (port == it->listen)
			break;
		index++;
	}
	std::vector<Location> locations = this->servers[index].locations;
	std::vector<Location>::iterator kt;
	std::vector<Location>::iterator kte = locations.end();
	for (kt = locations.begin(); kt != kte; kt++)
	{
		if (kt->path == location_path)
			return kt->root;
	}
	return "";
}

std::string	WebServer::getDirDefaultPath(int port, std::string location_path)
{
	int index = 0;
	std::vector<Server>::const_iterator it;
	std::vector<Server>::const_iterator ite = servers.end();
	for (it = servers.begin(); it != ite; it++)
	{
		if (port == it->listen)
			break;
	index++;
	}
	std::vector<Location> locations = this->servers[index].locations;
	std::vector<Location>::iterator kt;
	std::vector<Location>::iterator kte = locations.end();
	for (kt = locations.begin(); kt != kte; kt++)
	{
		if (kt->path == location_path)
			return kt->dir_default_path;
	}
	return "";
}

	std::pair<bool, std::string>
WebServer::getCGIDepends(int port, std::string location_path)
{
	int index = 0;
	std::vector<Server>::const_iterator it;
	std::vector<Server>::const_iterator ite = servers.end();
	for (it = servers.begin(); it != ite; it++)
	{
		if (port == it->listen)
			break;
		index++;
	}
	std::vector<Location> locations = this->servers[index].locations;
	std::vector<Location>::iterator kt;
	std::vector<Location>::iterator kte = locations.end();
	for (kt = locations.begin(); kt != kte; kt++)
	{
		if (kt->path == location_path)
			return kt->cgi_depends;
	}
	return std::pair<bool, std::string>();
}

std::string	WebServer::getCGIPath(int port, std::string location_path)
{
	int index = 0;
	std::vector<Server>::const_iterator it;
	std::vector<Server>::const_iterator ite = servers.end();
	for (it = servers.begin(); it != ite; it++)
	{
		if (port == it->listen)
			break;
		index++;
	}
	std::vector<Location> locations = this->servers[index].locations;
	std::vector<Location>::iterator kt;
	std::vector<Location>::iterator kte = locations.end();
	for (kt = locations.begin(); kt != kte; kt++)
	{
		if (kt->path == location_path)
			return kt->cgi_path;
	}
	return "";
}

bool	WebServer::getCGIStatus(int port, std::string location_path)
{
	int index = 0;
	std::vector<Server>::const_iterator it;
	std::vector<Server>::const_iterator ite = servers.end();
	for (it = servers.begin(); it != ite; it++)
		{
		if (port == it->listen)
			break;
		index++;
	}
	std::vector<Location> locations = this->servers[index].locations;
	std::vector<Location>::iterator kt;
	std::vector<Location>::iterator kte = locations.end();
	for (kt = locations.begin(); kt != kte; kt++)
	{
		if (kt->path == location_path)
			return kt->cgi;
	}
	return false;
}

std::vector<std::string>
WebServer::getAllowedMethods(int port, std::string location_path)
{
	int index = 0;
	std::vector<Server>::const_iterator it;
	std::vector<Server>::const_iterator ite = servers.end();
	for (it = servers.begin(); it != ite; it++)
	{
		if (port == it->listen)
			break;
		index++;
	}
	std::vector<Location> locations = this->servers[index].locations;
	std::vector<Location>::iterator kt;
	std::vector<Location>::iterator kte = locations.end();
	for (kt = locations.begin(); kt != kte; kt++)
	{
		if (kt->path == location_path)
			return kt->allowed_methods;
	}
	return std::vector<std::string>();
}

std::pair<int, std::string> WebServer::getRedirect(int port, std::string location_path)
{
	int index = 0;
	std::vector<Server>::const_iterator it;
	std::vector<Server>::const_iterator ite = servers.end();
	for (it = servers.begin(); it != ite; it++)
	{
		if (port == it->listen)
			break;
		index++;
	}
	std::vector<Location> locations = this->servers[index].locations;
	std::vector<Location>::iterator kt;
	std::vector<Location>::iterator kte = locations.end();
	for (kt = locations.begin(); kt != kte; kt++)
	{
		if (kt->path == location_path)
			return kt->redirect;
	}
	return std::pair<int, std::string>();
}

bool WebServer::getAutoindex(int port, std::string location_path)
{
	int index = 0;
	std::vector<Server>::const_iterator it;
	std::vector<Server>::const_iterator ite = servers.end();
	for (it = servers.begin(); it != ite; it++)
	{
		if (port == it->listen)
			break;
		index++;
	}
	std::vector<Location> locations = this->servers[index].locations;
	std::vector<Location>::iterator kt;
	std::vector<Location>::iterator kte = locations.end();
	for (kt = locations.begin(); kt != kte; kt++)
	{
		if (kt->path == location_path)
			return kt->autoindex;
	}
	return false;
}

const std::vector<Server> 	&WebServer::getServers() const 
{ 
	return servers; 
}

std::string	WebServer::getResponseBuffer(int fd)
{
	return this->responseBuffer[fd];
}

void	WebServer::setResponseBuffer(int fd, std::string buffer)
{
	this->responseBuffer[fd] = buffer;
}

bool	WebServer::responseBufferAbsent(int client_fd)
{
	if (this->responseBuffer.find(client_fd) == this->responseBuffer.end())
	 	return true;
	else
 		return false;
}

void	WebServer::eraseResponseBuffer(int client_fd)
{
	this->responseBuffer.erase(client_fd);
}

std::string	WebServer::getPostBody(int fd)
{
	return this->postBody[fd];
}
void	WebServer::setPostBody(int fd, std::string buffer)
{
	this->postBody[fd] = buffer;
}

void	WebServer::setPostFileFds(int fd, int file_fd)
{
	this->postFileFds[fd] = file_fd;
}

int	WebServer::getPostFileFds(int fd)
{
	return this->postFileFds[fd];
}

bool	WebServer::postFileFdAbsent(int client_fd)
{
	if (this->postFileFds.find(client_fd) == this->postFileFds.end())
		return true;
	else
		return false;
}

void	WebServer::removePostFileFds(int client_fd)
{
	this->postFileFds.erase(client_fd);
}

std::string&	WebServer::getRequestBuffer(int client_fd)
{
	// Vous pouvez utiliser un conteneur comme un std::map pour associer chaque client à son tampon de requête
	return this->requestBuffer[client_fd];
}

void	WebServer::setKeepAlive(int value)
{
	this->keepAlive = value;
	return ;
}

int	WebServer::getKeepAlive(void)
{
	return (this->keepAlive);
}

WebServer::~WebServer() {}
