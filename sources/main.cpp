#include "../includes/webserv.hpp"

void handle_sigint(int sig) {
	(void)sig;
	exit(0);
}

int main(int argc, char **argv)
{
	Config config;

	(void)argv;
	if (argc != 2)
	{
		std::cerr << "Error: Bad arguments" << std::endl;
		exit(1);
	}
	if (signal(SIGINT, handle_sigint) == SIG_ERR)
	{
		perror("Erreur lors de l'enregistrement du signal SIGINT");
		exit(1);
	}
	if (config.parseFile(argv[1]))
	{
		WebServer web_serv(config);
		try
		{
			web_serv.printServer();
			web_serv.verifyServer();
			ft_webserver(web_serv);
		}
		catch (const std::exception& e)
		{
			std::cerr << "Error: " << e.what() << '\n';
		}
	} 
	else
		return (1);
	return (0);
}
