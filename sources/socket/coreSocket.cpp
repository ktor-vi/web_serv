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

int handle_read_event(int client_fd, int epoll_fd, WebServer &data)
{
	char buffer[4096];
	size_t bytesRead;
	std::string request;

	make_socket_nonblocking(client_fd);
	while (true)
	{
		memset(buffer, 0, CHUNK_SIZE);
		bytesRead = recv(client_fd, buffer, CHUNK_SIZE - 1, 0); // read equivalent
		if (bytesRead > 0)
		{
			request += std::string(buffer, bytesRead);
			// std::cout << "Chunk received (" << bytesRead << " bytes): " << std::string(buffer, bytesRead) << "\n";
			if (bytesRead < CHUNK_SIZE - 1)
				break;
		}
		else if (bytesRead == 0)
		{
			std::cout << "Client deco ??" << std::endl;
			break;
		}
		else if (bytesRead <= 0)
		{
			throw(std::out_of_range("recv"));
		}
		// std::cout << "Total data received:\n" << totalData << "\n";
	}
	HandleRequests requestHandler(request, data, epoll_fd, client_fd);
	data.setResponseBuffer(client_fd, requestHandler.getResponse());

	// Passer en mode écriture (EPOLLOUT)
	struct epoll_event t_event;
	t_event.events = EPOLLOUT | EPOLLET;
	t_event.data.fd = client_fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client_fd, &t_event) == -1)
	{
		perror("EPOLL_CTL MOD ERROR");
		return -1;
	}

	// On a traité la requête, donc on arrête ici
	return 0;
}

int handle_write_event(int client_fd, int epoll_fd, WebServer &data)
{

	make_socket_nonblocking(client_fd);
	if (!data.postFileFdAbsent(client_fd))
	{
		int fileFd = data.getPostFileFds(client_fd);
		std::string body = data.getPostBody(client_fd);
		static size_t offset = 0; // Pour suivre l'écriture

		ssize_t bytes_written = write(fileFd, body.c_str() + offset, body.size() - offset);

		if (bytes_written < 0)
		{
			perror("Write error");
			close(fileFd);
			data.removePostFileFds(client_fd);
			return -1;
		}

		offset += bytes_written;

		if (offset >= body.size())
		{
			close(fileFd);
			data.removePostFileFds(client_fd);
			offset = 0;
		}
	}

	if (data.responseBufferAbsent(client_fd))
		return -1;

	const std::string &response = data.getResponseBuffer(client_fd);
	ssize_t bytes_sent = send(client_fd, response.c_str(), response.size(), 0);

	if (bytes_sent < 0)
	{
		perror("SEND ERROR");
		return -1;
	}

	data.eraseResponseBuffer(client_fd);
	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
	close(client_fd);
	return 0;
}

void ft_webserver(WebServer &data)
{
	struct sockaddr_in addr_client;
	socklen_t addr_len = sizeof(addr_client);
	int client_fd, epoll_fd, fds, i;
	struct epoll_event t_event, t_events[MAX_EVENTS];

	epoll_fd = epoll_create1(0); // Init epoll
	if (epoll_fd == -1)
		perror("EPOLL_CREATE ERROR");

	ft_setup_all_socket(data, epoll_fd, &t_event); // Ajouter les sockets serveurs à epoll

	while (1)
	{
		fds = epoll_wait(epoll_fd, t_events, MAX_EVENTS, -1); // Attente d'événements
		if (fds == -1)
			perror("EPOLL_WAIT ERROR");

		for (i = 0; i < fds; i++)
		{
			int fd = t_events[i].data.fd;

			if (ft_isit_fdsocket(data, fd)) // Nouvelle connexion
			{
				client_fd = accept(fd, (struct sockaddr *)&addr_client, &addr_len);
				if (client_fd < 0)
				{
					perror("ACCEPT ERROR");
					continue;
				}
				make_socket_nonblocking(client_fd);
				t_event.events = EPOLLIN | EPOLLET; // Prêt à lire
				t_event.data.fd = client_fd;
				if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &t_event) == -1)
				{
					perror("EPOLL_CTL ADD CLIENT ERROR");
					close(client_fd);
					continue;
				}
			}
			else if (t_events[i].events & EPOLLIN) // Données à lire
			{
				if (handle_read_event(fd, epoll_fd, data) == -1) // Fonction pour lire les données
				{
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
					close(fd);
				}
			}
			else if (t_events[i].events & EPOLLOUT) // Prêt à écrire
			{
				if (handle_write_event(fd, epoll_fd, data) == -1) // Fonction pour envoyer la réponse
				{
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
					close(fd);
				}
			}
		}
	}
}
