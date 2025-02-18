#include "../../class/Config.hpp"

Config::Config() : lineNumber(0) {}

Config::~Config() {}

bool Config::parseFile(const std::string &fileName)
{ 
  std::ifstream configFile(fileName.c_str());
  if (!configFile) {
    std::cerr << "Error: can't open file '" << fileName << "'." << std::endl;
    return false;
  }

  std::stack<std::string> blockStack;
  ServerBlock currentServer;
  LocationBlock currentLocation;
  bool inLocationBlock = false;
  bool hasValidBlocks = false;

  std::string line;
  while (std::getline(configFile, line)) {
    ++lineNumber;
    line = trim(line);

    if (line.empty() || line[0] == '#')
      continue;

    try {
      std::cout << "Parsing line " << lineNumber << ": " << line << std::endl;

      if (line.find("{") != std::string::npos) {
        hasValidBlocks = true;

        if (line.find("server") == 0) {
          blockStack.push("server");
          currentServer = ServerBlock();
        } else if (line.find("location") == 0) {
          blockStack.push("location");
          inLocationBlock = true;

          size_t pos = line.find(" ");
          size_t openBracePos = line.find("{");

          if (pos == std::string::npos || openBracePos == std::string::npos || openBracePos <= pos) {
            throw std::runtime_error("Invalid location block syntax.");
          }

          if (openBracePos - pos <= 1) {
            throw std::runtime_error("Location path is missing.");
          }

          std::cout << "Location path parsing: pos=" << pos << ", openBracePos=" << openBracePos << std::endl;

          currentLocation.path = trim(line.substr(pos, openBracePos - pos));
          if (currentLocation.path.empty()) {
            throw std::runtime_error("Path cannot be empty.");
          }
        } else {
          throw std::runtime_error("Unknown block type.");
        }
      } else if (line.find("}") != std::string::npos) {
        if (blockStack.empty()) {
          throw std::runtime_error("Closing block without an opening block.");
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
        size_t pos = line.find(";");
        if (pos == std::string::npos) {
          throw std::runtime_error("Bad directive (missing ';').");
        }

        std::string directive = line.substr(0, pos);
        size_t spacePos = directive.find_first_not_of(" ");
        size_t keyEnd = directive.find_first_of(" ", spacePos);

        if (keyEnd == std::string::npos || spacePos == std::string::npos) {
          throw std::runtime_error("Bad directive (missing key or value).");
        }

        std::string key = trim(directive.substr(0, keyEnd));
        std::string value = trim(directive.substr(keyEnd));

        if (key.empty() || value.empty()) {
          throw std::runtime_error("Bad directive (empty key or value).");
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

  if (!hasValidBlocks) {
    std::cerr << "Error: No valid configuration blocks found.\n";
    return false;
  }

  if (!blockStack.empty()) {
    std::cerr << "Error: Block not closed.\n";
    return false;
  }

  if (servers.empty()) {
    std::cerr << "Error: No valid configuration found in the file.\n";
    return false;
  }

  return true;
}

void Config::printConfig() const {
  if (servers.empty()) {
    std::cout << "No valid servers found in the configuration.\n";
    return;
  }

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
  if (str.empty())
    return "";

  size_t start = str.find_first_not_of(" \t");
  if (start == std::string::npos) // Si la ligne contient uniquement des espaces
    return "";

  size_t end = str.find_last_not_of(" \t");
  return str.substr(start, end - start + 1);
}
