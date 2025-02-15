#include "../../includes/webserv.hpp"

void HandleRequests::initCgiInfos(WebServer &webServData)
{
	this->_rootDir = webServData.getRootPath(this->_port, this->_rootUrl);
	
	// this->_fileName = this->_buffer.substr(this->_buffer.find("filename=\"") + 10, this->_buffer.find("\"", this->_buffer.find("filename=\"") + 10) - this->_buffer.find("filename=\"") - 10); // CHANGER 
	size_t start = this->_buffer.find("POST ") + 5;
	size_t end = this->_buffer.find(" HTTP/");
	if (start == std::string::npos || end == std::string::npos)
	{
		std::cerr << "Erreur : URL CGI introuvable dans la requête" << std::endl;
		return;
	}
	std::string requestUrl = this->_buffer.substr(start, end - start);
    size_t lastSlash = requestUrl.find_last_of('/');
    if (lastSlash == std::string::npos)
    {
        std::cerr << "Erreur : Chemin CGI invalide" << std::endl;
        return;
    }
    this->_fileName = requestUrl.substr(lastSlash + 1);
	// this->_fileName = this->_buffer.substr(start, end - start);
	this->_filePath = this->_rootDir + this->_fileName;

	std::cout << "ROOT DIR = " << this->_rootDir << std::endl;
	std::cout << "FILE_NAME = " << this->_fileName << std::endl;
	std::cout << "FILE_PATH = " << this->_filePath << std::endl;
}

std::string	HandleRequests::cgiExecution(void)
{
	int			pipefd[2];
	std::string	result;
	int 		status;
	int			timeout = 10;
	int			ret = 0;

	if (pipe(pipefd) == -1)
	{
		perror("pipe");
		return ("Error CGI");
	}
	pid_t pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return ("Error CGI");
	}
	if (pid == 0)
	{
		close(pipefd[0]);
		char *args[] = {const_cast<char *>(this->_filePath.c_str()), NULL};
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);
		extern char **environ;
		execve(this->_filePath.c_str(), args, environ);
		perror("execve problem");
		exit(1);
	} 
	else
	{
		close(pipefd[1]);
		make_socket_nonblocking(pipefd[0]);
		char buffer[1024];
		ssize_t bytesRead;
		// on laisse 10 tentative au CGI de se terminer (avec mySleep(10000) entre chaque check)
		while (timeout-- != 0)
		{
			ret = waitpid(pid, &status, WNOHANG);
			// ret > 0, child dead sans erreur
			if (ret > 0)
				break;
			mySleep(10000);
		}
		 // si il termine normalement, execute
		if (WIFEXITED(status))
		{
			while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0)
			{
				buffer[bytesRead] = '\0';
				result += buffer;
			}
		}
		// sinon error, KILL le pid dans un etat ??
		else
		{
			perror("CGI error");
			kill(pid, SIGKILL);
		}
		close(pipefd[0]);
	}
	return (result);
}

std::string HandleRequests::cgiMethod(WebServer &webServData)
{
	std::cout << "[ CGI HANDLING ]" << std::endl;
	initCgiInfos(webServData);
	std::string	result = this->cgiExecution();
	return (result);
}


