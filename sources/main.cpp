#include "../includes/WebServer.hpp"
int main() {
  Config config;

  if (config.parseFile("config.conf")) {
    // config.printConfig();
    WebServer web_serv(config);
    // web_serv.printServer();
    web_serv.verifyServer();
    std::cout << "Method 1: [" << web_serv.getAllowedMethods(0, "/")[0]
              << "]\n";
    std::cout << "Method 2: [" << web_serv.getAllowedMethods(0, "/")[1]
              << "]\n";
    std::cout << "Method 3: [" << web_serv.getAllowedMethods(0, "/")[2]
              << "]\n";
  } else {
    std::cerr << "Erreur lors du parsing du fichier de configuration."
              << std::endl;
  }
  return 0;
}
