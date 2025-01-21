#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP
#include "webserv.hpp"
class Config; 
class WebServer {

private:
  std::vector<Server> servers;

public:
  WebServer(Config &conf);
  ~WebServer();
  
  void							verifyServer() const;
  void							printServer() const;
  void							setSocketFd(int index, int socket_fd);
  int							getSocketFd(int index);
  int							getPort(int index);
  int							getNumberOfServers();
  std::string 					getServerName(int index);
  std::string 					getErrorPagePath(int index, int error_code);
  std::string 					getIndexPath(int index, std::string location_path);
  std::vector<std::string> 		getAllowedMethods(int index,std::string location_path);
  bool 							getCGIStatus(int index, std::string location_path);
  std::string 					getCGIPath(int index, std::string location_path);
  std::string 					getDirDefaultPath(int index, std::string location_path);
  std::pair<bool, std::string> 	getCGIDepends(int index, std::string location_path);
  const std::vector<Server>		&getServers() const;
};

#endif