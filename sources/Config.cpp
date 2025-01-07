#include "../includes/Config.hpp"
Config::Config() : lineNumber(0) {}
Config::~Config() {}
bool Config::parseFile(const std::string &fileName) {
  std::ifstream configFile(fileName.c_str());
  if (!configFile) {
    std::cerr << "Erreur : Impossible d'ouvrir le fichier." << std::endl;
    return false;
  }

  std::stack<std::string> blockStack;
  ServerBlock currentServer;
  LocationBlock currentLocation;
  bool inLocationBlock = false;

  std::string line;
  while (std::getline(configFile, line)) {
    ++lineNumber;
    line = trim(line);

    if (line.empty() || line[0] == '#')
      continue;

    try {
      if (line.find("{") != std::string::npos) {
        // Début d'un bloc
        if (line.find("server") == 0) {
          blockStack.push("server");
          currentServer = ServerBlock();
        } else if (line.find("location") == 0) {
          blockStack.push("location");
          inLocationBlock = true;

          size_t pos = line.find(" ");
          if (pos == std::string::npos) {
            throw std::runtime_error(
                "Chemin de location manquant ou syntaxe invalide.");
          }

          currentLocation.path = trim(line.substr(pos, line.find("{") - pos));
          if (currentLocation.path.empty()) {
            throw std::runtime_error("Chemin de location manquant.");
          }
        } else {
          throw std::runtime_error("Type de bloc inconnu.");
        }
      } else if (line.find("}") != std::string::npos) {
        // Fin d'un bloc
        if (blockStack.empty()) {
          throw std::runtime_error(
              "Bloc fermant sans bloc ouvrant correspondant.");
        }

        std::string blockType = blockStack.top();
        blockStack.pop();

        if (blockType == "location") {
          currentServer.locations.push_back(currentLocation);
          currentLocation = LocationBlock();
          inLocationBlock = false;
        } else if (blockType == "server") {
          servers.push_back(currentServer);
          currentServer = ServerBlock();
        }
      } else {
        // Directive clé-valeur
        size_t pos = line.find(";");
        if (pos == std::string::npos) {
          throw std::runtime_error("Directive mal formée (manque ';').");
        }

        std::string directive = line.substr(0, pos);
        size_t spacePos = directive.find(" ");
        if (spacePos == std::string::npos) {
          throw std::runtime_error(
              "Directive mal formée (clé ou valeur manquante).");
        }

        std::string key = trim(directive.substr(0, spacePos));
        std::string value = trim(directive.substr(spacePos + 1));

        if (inLocationBlock) {
          currentLocation.directives[key] = value;
        } else {
          if (key == "listen") {
            currentServer.listen = std::atoi(value.c_str());
          } else if (key == "server_name") {
            currentServer.server_name = value;
          } else {
            currentServer.other_directives[key] = value;
          }
        }
      }
    } catch (const std::runtime_error &e) {
      std::cerr << "Erreur à la ligne " << lineNumber << ": " << e.what()
                << "\n";
      return false;
    }
  }

  // Vérification : blocs non fermés
  if (!blockStack.empty()) {
    std::cerr << "Erreur : Bloc non fermé à la fin du fichier.\n";
    return false;
  }

  return true;
}

void Config::printConfig() const {
  for (size_t i = 0; i < servers.size(); ++i) {
    std::cout << "Server " << i + 1 << ":\n";
    std::cout << "  Listen: " << servers[i].listen << "\n";
    std::cout << "  Server Name: " << servers[i].server_name << "\n";

    for (size_t j = 0; j < servers[i].locations.size(); ++j) {
      std::cout << "  Location " << j + 1
                << " Path: " << servers[i].locations[j].path << "\n";
      for (std::map<std::string, std::string>::const_iterator it =
               servers[i].locations[j].directives.begin();
           it != servers[i].locations[j].directives.end(); ++it) {
        std::cout << "    " << it->first << ": " << it->second << "\n";
      }
    }

    for (std::map<std::string, std::string>::const_iterator it =
             servers[i].other_directives.begin();
         it != servers[i].other_directives.end(); ++it) {
      std::cout << "  " << it->first << ": " << it->second << "\n";
    }
  }
}
const std::vector<ServerBlock> &Config::getServers() const { return servers; }

std::string Config::trim(const std::string &str) const {
  size_t start = str.find_first_not_of(" \t");
  size_t end = str.find_last_not_of(" \t");
  return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}
