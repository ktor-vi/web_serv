#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include "../includes/webserv.hpp"

class Config; 
class WebServer {

private:
  std::vector<Server> servers;

public:
  WebServer(Config &conf);
  ~WebServer();
  
  void						             	verifyServer() const;
  void						            	printServer() const;
  void							            setSocketFd(int index, int socket_fd);
  int							              getSocketFd(int index);
  int						              	getPort(int index);
  int             							getNumberOfServers();
  int                  					getBodySize(int port);
  std::string 				        	getServerName(int port);
  std::string 		        			getErrorPagePath(int port, int error_code);
  std::string 			        		getIndexPath(int port, std::string location_path);
  std::string 					        getRootPath(int port, std::string location_path) ;
  std::vector<std::string>  		getAllowedMethods(int port,std::string location_path);
  bool 							            getCGIStatus(int port, std::string location_path);
  std::string 		        			getCGIPath(int port, std::string location_path);
  std::string 			         		getDirDefaultPath(int port, std::string location_path);
  std::pair<bool, std::string> 	getCGIDepends(int port, std::string location_path);
  const std::vector<Server> 		&getServers() const;
};

#endif
