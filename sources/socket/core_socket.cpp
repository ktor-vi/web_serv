#include "../../includes/webserv.hpp"

/*
Résumé des fonctions finales

    Sockets : socket, bind, listen, accept.
    Événements : epoll_create, epoll_ctl, epoll_wait
    Communication : send, recv.
    DNS : getaddrinfo, freeaddrinfo.
    Processus : fork, signal, kill.
    Fichiers : close, fcntl.
*/

char *ft_call_cgi(char *ans)
{
	int			fds[2];
    const char	*python_path = "/usr/bin/python3";
    const char	*script_path = "CGI.py";
	const char	*python = "python3";
    char *const	argv[] = { (char *)python, (char *)script_path, NULL }; // Format for execve

	pipe(fds);
    if (fork() == 0)
	{
		close(fds[0]);
		dup2(fds[1], STDOUT_FILENO);
		if (execve(python_path, argv, 0) == -1)
		{
        	perror("Erreur lors de l'exécution de execve");
        	exit(EXIT_FAILURE);
		}
    }
	close(fds[1]);
	wait(&fds[1]);
	read(fds[0], ans, 3);
	close(fds[0]);
    return (ans);
}

void	ft_setup_socket(int *server_fd, int port)
{
	struct sockaddr_in	addr_serv;
	int					opt = 1;

	*server_fd = socket(AF_INET, SOCK_STREAM, 0);	// socket creation as ipv4
    if (setsockopt(*server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
        perror("SETSOCKOPT ERROR\n");
		exit(1);
	}
	memset(&addr_serv, 0, sizeof(addr_serv));		// switch any bytes to 0
	addr_serv.sin_family = AF_INET;					// server will use ipv4
	addr_serv.sin_port = htons(port);				
	inet_pton(AF_INET, "127.0.0.1", &addr_serv.sin_addr); // server will listen 127.0.0.1:8080
	if (bind(*server_fd, (const struct sockaddr *)&addr_serv, sizeof(addr_serv)) == -1) // link socket the addr:port
		perror("BIND ERROR\n"), exit(1);
	if (listen(*server_fd, 5) < 0) 														//lock socket in listen mode
		perror("LISTEN ERROR\n"), exit(1);
	printf("SERVER ON 127.0.0.1:%d IS ONLINE\n", port);
}
#define MAX_EVENTS 10

void ft_webserver(void) 
{
    struct sockaddr_in	addr_client;
    socklen_t			addr_len;
    int					server_fd, server_fd2, client_fd, epoll_fd, fds, i;
	int					request_status;
    struct epoll_event	t_event, t_events[MAX_EVENTS];

	i = -1;
    ft_setup_socket(&server_fd, 8080);
	ft_setup_socket(&server_fd2, 8081);
    addr_len = sizeof(addr_client);
    epoll_fd = epoll_create1(0); //Init poll
    if (epoll_fd == -1){
        perror("EPOLL_CREATE ERROR");}
	t_event.data.fd = server_fd;
    t_event.events = EPOLLIN; // Nous écoutons les événements "prêt pour lecture"
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &t_event) == -1)
		perror("EPOLL_CTL ERROR");
	t_event.data.fd = server_fd2;
    t_event.events = EPOLLIN; // Nous écoutons les événements "prêt pour lecture"
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd2, &t_event) == -1)
        perror("EPOLL_CTL ERROR");
    while (1)
	{
		i = -1;
		fds = epoll_wait(epoll_fd, t_events, MAX_EVENTS, -1); // Attend des requetes et les place dans le tab EVENTS
		if (fds == -1)
            perror("EPOLL_WAIT ERROR");
		while (++i != fds)
		{
        	if (t_events[i].data.fd == server_fd || t_events[i].data.fd == server_fd2) // Si c'est un evenement du socket serveur, c'est une nouvelle connexion
			{
				if (t_events[i].data.fd == server_fd)
					client_fd = accept(server_fd, (struct sockaddr *)&addr_client, &addr_len); // que je dois accepter
				if (t_events[i].data.fd == server_fd2)
					client_fd = accept(server_fd2, (struct sockaddr *)&addr_client, &addr_len); // que je dois accepter
				if (client_fd < 0)
				{
            		perror("ACCEPT ERROR");
					continue;
				}
				// Ajout de la socket client à epoll
                t_event.data.fd = client_fd;
                t_event.events = EPOLLIN; // Événements de lecture
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &t_event) == -1)
				{
                    perror("EPOLL_CTL ADD CLIENT ERROR");
                    close(client_fd);
					continue;
				}
			}
			else
			{
				client_fd = t_events[i].data.fd;
        		request_status = handle_request(client_fd);
        		if (request_status == -1)
				{
        		    close(server_fd);
					close(server_fd2);
        	    	close(client_fd);
        	    	exit(1);
        		}
        		if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL) == -1)
            		perror("EPOLL_CTL DEL ERROR");
				close(client_fd);
   			}
		}
	}
}