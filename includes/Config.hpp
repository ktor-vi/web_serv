#include "webserv.hpp"
#include <sstream>
class Config {

private:
  std::vector<ServerBlock> servers;
  int lineNumber;
  std::string trim(const std::string &str) const;

public:
  Config();
  ~Config();
  bool verifyConfig() const;
  bool parseFile(const std::string &fileName);
  void printConfig() const;
  const std::vector<ServerBlock> &getServers() const;
};
