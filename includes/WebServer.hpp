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
  const std::vector<Server> &getServers() const;
};
