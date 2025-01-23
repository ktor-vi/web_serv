/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coreSocket.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jepatern <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 13:55:39 by jepatern          #+#    #+#             */
/*   Updated: 2025/01/15 13:55:44 by jepatern         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../includes/webserv.hpp"

#define MAX_EVENTS 10

/*Résumé des fonctions finales
	Sockets : socket, bind, listen, accept.
	Événements : epoll_create, epoll_ctl, epoll_wait
	Communication : send, recv.
	DNS : getaddrinfo, freeaddrinfo.
	Processus : fork, signal, kill.
	Fichiers : close, fcntl.
*/

void	ft_webserver(WebServer &data)
{
	struct sockaddr_in	addr_client;
	socklen_t			addr_len = sizeof(addr_client);
	int					client_fd, epoll_fd, fds, i;
	struct epoll_event	t_event, t_events[MAX_EVENTS];

	i = -1;
	epoll_fd = epoll_create1(0); //Init poll
	if (epoll_fd == -1){
		perror("EPOLL_CREATE ERROR");}
	ft_setup_all_socket(data, epoll_fd, &t_event);
	while (1)
	{
		i = -1;
		fds = epoll_wait(epoll_fd, t_events, MAX_EVENTS, -1); // Attend des requetes et les place dans le tab t_events
		if (fds == -1)
			perror("EPOLL_WAIT ERROR");
		while (++i != fds)
		{
			make_socket_nonblocking(t_events[i].data.fd);
			if (ft_isit_fdsocket(data, t_events[i].data.fd)) // Si c'est un evenement du socket serveur, c'est une nouvelle connexion
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
				HandleRequests request(client_fd, data); // replacer par ex ; "ft_handle_request(client_fd, data)" data est l'obj webserv
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
