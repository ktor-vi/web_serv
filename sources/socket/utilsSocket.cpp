/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utilsSocket.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jepatern <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 13:55:26 by jepatern          #+#    #+#             */
/*   Updated: 2025/01/15 13:55:35 by jepatern         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../includes/webserv.hpp"

void make_socket_nonblocking(int sockfd)
{
    if (fcntl(sockfd, F_SETFL, O_NONBLOCK) == -1) {
        perror("fcntl error");
        close(sockfd);  // Fermez la socket en cas d'erreur
        exit(EXIT_FAILURE);  // Arrêtez l'exécution ou gérez autrement
    }
}


bool	ft_isit_fdsocket(WebServer &data, int socket_fd)
{
	int i = -1;
	int index_max = data.getNumberOfServers();

	while (++i != index_max)
		if (data.getSocketFd(i) == socket_fd)
			return (1);
	return (0);
}