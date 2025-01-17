/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   core_socket.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jepatern <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 13:55:39 by jepatern          #+#    #+#             */
/*   Updated: 2025/01/15 13:55:44 by jepatern         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
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

	usleep(100000);
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

void	ft_setup_socket(int *server_fd, int port, int epoll_fd, struct epoll_event *ptr)
{
	struct sockaddr_in	addr_serv;

	*server_fd = socket(AF_INET, SOCK_STREAM, 0);	// socket creation as ipv4
	if (*server_fd == -1)
		perror("Error creation socket\n"), exit(1);
	make_socket_nonblocking(*server_fd);
	memset(&addr_serv, 0, sizeof(addr_serv));		// switch any bytes to 0
	addr_serv.sin_family = AF_INET;					// server will use ipv4
	addr_serv.sin_port = htons(port);				
	inet_pton(AF_INET, "127.0.0.1", &addr_serv.sin_addr); // server will listen 127.0.0.1:8080
	if (bind(*server_fd, (const struct sockaddr *)&addr_serv, sizeof(addr_serv)) == -1) // link socket the addr:port
		perror("BIND ERROR\n"), exit(1);
	if (listen(*server_fd, 5) < 0) 														//lock socket in listen mode
		perror("LISTEN ERROR\n"), exit(1);
	ptr->events = EPOLLIN;
	ptr->data.fd = *server_fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, *server_fd, ptr) == -1)
		perror("EPOLL_CTL ERROR"), exit(1);
	printf("SERVER ON 127.0.0.1:%d IS ONLINE\n", port);
}

void ft_webserver(void) 
{
	struct sockaddr_in	addr_client;
	socklen_t			addr_len;
	int					server_fd, server_fd2, client_fd, epoll_fd, fds, i;
	int					request_status;
	struct epoll_event	t_event, t_events[MAX_EVENTS];

	i = -1;
	addr_len = sizeof(addr_client);
	epoll_fd = epoll_create1(0); //Init poll
	if (epoll_fd == -1){
		perror("EPOLL_CREATE ERROR");}
	ft_setup_socket(&server_fd, 8080, epoll_fd, &t_event);
	ft_setup_socket(&server_fd2, 8081, epoll_fd, &t_event);
	while (1)
	{
		i = -1;
		fds = epoll_wait(epoll_fd, t_events, MAX_EVENTS, -1); // Attend des requetes et les place dans le tab t_events
		if (fds == -1)
			perror("EPOLL_WAIT ERROR");
		while (++i != fds)
		{
			make_socket_nonblocking(t_events[i].data.fd);
			if (t_events[i].data.fd == server_fd || t_events[i].data.fd == server_fd2) // Si c'est un evenement du socket serveur, c'est une nouvelle connexion
			{
				client_fd = accept(t_events[i].data.fd, (struct sockaddr *)&addr_client, &addr_len); // que je dois accepter
				if (client_fd < 0)
				{
					perror("ACCEPT ERROR");
					continue;
				}
				t_event.data.fd = client_fd;
				if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &t_event) == -1) // Rajoud du fd client dans la pool
				{
					perror("EPOLL_CTL ADD CLIENT ERROR");
					close(client_fd);
					continue;
				}
			}
			else // sinon c'est une connexion deja accordee, a lire et repondre
			{
				client_fd = t_events[i].data.fd;
				request_status = handle_request(client_fd);
				if (request_status == -1)
				{
					close(server_fd);
					close(server_fd2);
					close(client_fd);
					perror("Server closed properly"), exit(0);
				}
				if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL) == -1)
				{
					perror("EPOLL_CTL DEL ERROR");
					continue;
				}
				close(client_fd);
   			}
		}
	}
}
