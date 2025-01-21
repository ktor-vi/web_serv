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
	if (fcntl(sockfd, F_SETFL, O_NONBLOCK) == -1) // Le socket fd devient non-bloquant
		perror("fcntl error");
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