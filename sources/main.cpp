#include "../includes/webserv.hpp"

void handle_sigint(int sig) {
	(void)sig;
	exit(0);
}

int main(int argc, char **argv)
{
	Config config;

	if (argc < 2)
	{
		perror ("Bad arguments");
		return (1);
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
		catch(const std::exception& e)
		{
			std::cerr << "Error: " << e.what() << '\n';
		}
	} 
	else
		std::cerr << "Erreur lors du parsing du fichier de configuration." << std::endl;
	return (0);
}
