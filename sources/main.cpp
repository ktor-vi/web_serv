#include "../includes/webserv.hpp"

void handle_sigint(int sig) {
	(void)sig;
    exit(0);
}

int main()
{
  	Config config;

    if (signal(SIGINT, handle_sigint) == SIG_ERR)
	{
        perror("Erreur lors de l'enregistrement du signal SIGINT");
        exit(1);
    }
	if (config.parseFile("config.conf"))
	{
    	// config.printConfig();
    	WebServer web_serv(config);

    	// web_serv.printServer();
    	web_serv.verifyServer();
		ft_webserver(web_serv);
  	} 
  	else
    	std::cerr << "Erreur lors du parsing du fichier de configuration." << std::endl;
  return 0;
}
