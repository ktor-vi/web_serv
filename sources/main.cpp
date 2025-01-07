#include "../includes/WebServer.hpp"
int main() {
  Config config;

  if (config.parseFile("config.conf")) {
    // config.printConfig();
    WebServer web_serv(config);
    web_serv.printServer();
  } else {
    std::cerr << "Erreur lors du parsing du fichier de configuration."
              << std::endl;
  }
  return 0;
}
