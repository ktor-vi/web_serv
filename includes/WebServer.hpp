#include "Config.hpp"
#include "webserv.hpp"
#include <sstream>
class WebServer {

private:
  std::vector<Server> servers;

public:
  WebServer(Config &conf);
  ~WebServer();
  void verifyServer() const;
  void printServer() const;
  void setSocketFd(int index, int socket_fd);
  int getSocketFd(int index);
  int getPort(int index);
  int getNumberOfServers();
  const std::vector<Server> &getServers() const;
};
