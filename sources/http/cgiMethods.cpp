#include "../../includes/webserv.hpp"

void HandleRequests::initCgiInfos(WebServer &webServData)
{
	this->_rootDir = webServData.getRootPath(this->_port, this->_rootUrl);
	
	size_t start = this->_buffer.find("POST ") + 5;
	size_t end = this->_buffer.find(" HTTP/");
	if (start == std::string::npos || end == std::string::npos)
	{
		std::cerr << "Error: CGI url not found in the request" << std::endl;
		return;
	}
	std::string requestUrl = this->_buffer.substr(start, end - start);
	size_t lastSlash = requestUrl.find_last_of('/');
	if (lastSlash == std::string::npos)
	{
		std::cerr << "Error: invalid CGI path" << std::endl;
		return;
	}
	this->_fileName = requestUrl.substr(lastSlash + 1);
	this->_filePath = this->_rootDir + this->_fileName;

	std::cout << "-------------------------------" << std::endl;
	std::cout << "ROOT DIR = " << this->_rootDir << std::endl;
	std::cout << "FILE_NAME = " << this->_fileName << std::endl;
	std::cout << "FILE_PATH = " << this->_filePath << std::endl;
	std::cout << "-------------------------------" << std::endl;
}

static std::vector<char *>	buildEnv(std::string rootDir, std::string fileName, std::string filePath)
{
	std::vector<std::string>	envVars;
	std::vector<char *>			envPtrs;
 	std::vector<char *>			mergedEnv;
	extern char					**environ;

	envVars.push_back("SCRIPT_FILENAME=" + filePath);
	envVars.push_back("PATH_INFO=" + fileName);
	envVars.push_back("DOCUMENT_ROOT=" + rootDir);

	for (size_t i = 0; i < envVars.size(); ++i)
	{
		std::string &var = envVars[i];
		envPtrs.push_back(const_cast<char *>(var.c_str()));
	}
	envPtrs.push_back(NULL);
	int	envSize = 0;
	while (environ[envSize] != NULL)
		envSize++;
	for (int i = 0; i < envSize; ++i)
		mergedEnv.push_back(environ[i]);
	for (size_t i = 0; i < envPtrs.size(); ++i)
		mergedEnv.push_back(envPtrs[i]);
	mergedEnv.push_back(NULL);
	return (mergedEnv);
}

std::string	HandleRequests::cgiExecution(void)
{
	std::string	result;
	int			pipefd[2];
	int 		status;
	int			timeout = 10;
	int			ret = 0;

	if (pipe(pipefd) == -1)
	{
		perror("pipe");
		return ("Error: pipe CGI");
	}
	pid_t pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return ("Error: fork CGI");
	}
	if (pid == 0)
	{
		char *args[] = {const_cast<char *>(this->_filePath.c_str()), NULL};
		std::vector<char *> env = buildEnv(this->_rootDir, this->_fileName, this->_filePath);

		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);
		execve(this->_filePath.c_str(), args, &env[0]);
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

			if (ret > 0)
				break;
			mySleep(10000);
		}
		if (WIFEXITED(status))
		{
			while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0)
			{
				buffer[bytesRead] = '\0';
				result += buffer;
			}
		}
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


