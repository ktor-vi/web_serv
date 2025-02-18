#include "../../includes/webserv.hpp"
#include "../../class/HandleRequests.hpp"
static std::vector<std::string> getDirectoryListing(const std::string &directoryPath) {
    std::vector<std::string> files;
    DIR *dir = opendir(directoryPath.c_str());
    if (!dir) {
        std::cerr << "Error: Can't open directory" << std::endl;
        return files;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        std::string filename(entry->d_name);
        if (filename != "." && filename != "..") {
            files.push_back(filename);
        }
    }
    closedir(dir);
    return files;
}

static std::string generateAutoIndexPage(const std::string &directoryPath, const std::vector<std::string> &files) {
    std::ostringstream html;
    html << "<html><head><title>Index of " << directoryPath << "</title></head><body>";
    html << "<h1>Index of " << directoryPath << "</h1><ul>";

    for (size_t i = 0; i < files.size(); ++i) {
        html << "<li><a href=\"" << files[i] << "\">" << files[i] << "</a></li>";
    }

    html << "</ul></body></html>";
    return html.str();
}

void HandleRequests::buildAutoIndexResponse(const std::string &path) {
        std::vector<std::string> files = getDirectoryListing(path);
        std::string html = generateAutoIndexPage(path, files);

        std::ostringstream response;
        response << "HTTP/1.1 200 OK\r\n";
        response << "Content-Type: text/html\r\n";
        response << "Content-Length: " << html.size() << "\r\n";
        response << "\r\n";
        response << html;

        this->_response =  response.str();
}
