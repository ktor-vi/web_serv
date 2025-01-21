#include "../includes/webserv.hpp"

void handle_sigint(int sig) {
	(void)sig;
    exit(0);
}

void handle_eof(int sig) {
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
    	std::cout << "Method 1: [" << web_serv.getAllowedMethods(0, "/")[0]
    	        << "]\n";
    	std::cout << "Method 2: [" << web_serv.getAllowedMethods(0, "/")[1]
    	         << "]\n";
    	std::cout << "Method 3: [" << web_serv.getAllowedMethods(0, "/")[2]
     	         << "]\n";
		ft_webserver(web_serv);
  	} 
  	else
    	std::cerr << "Erreur lors du parsing du fichier de configuration." << std::endl;
  return 0;
}
