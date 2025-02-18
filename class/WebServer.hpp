#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include "../includes/webserv.hpp"

class Config;

class WebServer
	{
		private:
			std::string					conf;
			std::vector<Server>			servers;
			std::map<int, std::string>	serverNames;
			std::map<int, std::string>	responseBuffer; // Associe un client_fd à sa réponse
			std::map<int, std::string>	requestBuffer; // Associe un client_fd à sa réponse
			std::map<int, std::string>	postBody; // Associe un client_fd à sa postBody
			std::map<int, int>			postFileFds;

		public:
			WebServer(Config &conf);
			~WebServer();

			void							verifyServer() const;
			void							printServer() const;

			void							setSocketFd(int index, int socket_fd);
			int								getSocketFd(int index);
			int								getPort(int index);
			int								getNumberOfServers();

			int								getBodySize(int port);
			std::string						getServerName(int port);
			std::string						getErrorPagePath(int port, int error_code);
			std::string						getIndexPath(int port, std::string location_path);
			std::string						getRootPath(int port, std::string location_path);
			std::string						getDirDefaultPath(int port, std::string location_path);
			std::pair<bool, std::string>	getCGIDepends(int port, std::string location_path);
			std::string						getCGIPath(int port, std::string location_path);
			bool							getCGIStatus(int port, std::string location_path);
			std::vector<std::string>		getAllowedMethods(int port, std::string location_path);
			std::pair<int, std::string> 	getRedirect(int port, std::string location_path);
			bool 							getAutoindex(int port, std::string location_path);

			const std::vector<Server>		&getServers() const;

			std::string						getResponseBuffer(int fd);
			void							setResponseBuffer(int fd, std::string buffer);
			bool							responseBufferAbsent(int client_fd);
			void							eraseResponseBuffer(int client_fd);

			void							setPostBody(int fd, std::string buffer);
			std::string 					getPostBody(int fd);
			void							setPostFileFds(int fd, int file_fd);
			int								getPostFileFds(int fd);
			bool							postFileFdAbsent(int client_fd);
			void							removePostFileFds(int client_fd);

			std::string						&getRequestBuffer(int client_fd);
	};

#endif
