#include "../includes/webserv.hpp"

void checkDirectory(const std::string &path) {
    DIR *dir = opendir(path.c_str());
    if (!dir) {
        throw std::runtime_error("Path invalid: " + path);
    }
    closedir(dir);
}

void mySleep(unsigned int microseconds)
{
    struct timespec ts;
    ts.tv_sec = microseconds / 1000000;
    ts.tv_nsec = (microseconds % 1000000) * 1000;
    nanosleep(&ts, NULL);
}
