#include "../includes/webserv.hpp"

void checkDirectory(const std::string &path) {
    DIR *dir = opendir(path.c_str());
    if (!dir) {
        throw std::runtime_error("Path invalid: " + path);
    }
    closedir(dir);
}

