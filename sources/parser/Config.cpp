#include "../../includes/Config.hpp"
Config::Config() : lineNumber(0) {}
Config::~Config() {}
bool Config::parseFile(const std::string &fileName) {
  std::ifstream configFile(fileName.c_str());
  if (!configFile) {
    std::cerr << "Error: can't open file." << std::endl;
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
            throw std::runtime_error("Invalid/missing path or bad syntax.");
          }

          currentLocation.path = trim(line.substr(pos, line.find("{") - pos));
          if (currentLocation.path.empty()) {
            throw std::runtime_error("Path empty.");
          }
        } else {
          throw std::runtime_error("Unknown block type.");
        }
      } else if (line.find("}") != std::string::npos) {
        // Fin d'un bloc
        if (blockStack.empty()) {
          throw std::runtime_error("Closing block without opening block.");
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
          throw std::runtime_error("Bad directive (missing ';').");
        }

        std::string directive = line.substr(0, pos);
        size_t spacePos = directive.find_first_not_of(" ");
        size_t keyEnd = directive.find_first_of(" ", spacePos);
        std::string key = trim(directive.substr(0, keyEnd));
        std::string value = trim(directive.substr(keyEnd));

        if (spacePos == std::string::npos) {
          throw std::runtime_error("Bad directive (missing key or value).");
        }

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
      std::cerr << "Error on line " << lineNumber << ": " << e.what() << "\n";
      return false;
    }
  }

  // Vérification : blocs non fermés
  if (!blockStack.empty()) {
    std::cerr << "Error : Block not closed.\n";
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
