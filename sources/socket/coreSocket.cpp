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

#define MAX_EVENTS 30

void ft_webserver(WebServer &data)
{
	struct sockaddr_in		addr_client;
	socklen_t				addr_len = sizeof(addr_client);
	int						client_fd, epoll_fd, fds, i, fd;
	struct epoll_event		t_event, t_events[MAX_EVENTS];

	epoll_fd = epoll_create1(0); // Init epoll
	if (epoll_fd == -1)
		perror("EPOLL_CREATE ERROR"), exit(1);
	ft_setup_all_socket(data, epoll_fd, &t_event);
	while (1)
	{
		fds = epoll_wait(epoll_fd, t_events, MAX_EVENTS, -1); // Attente d'événements
		if (fds == -1)
			perror("EPOLL_WAIT ERROR");
		else
		{
			i = -1;
			while (++i < fds)
			{
				fd = t_events[i].data.fd;
				if (ft_isit_fdsocket(data, fd)) // Nouvelle connexion
				{
					client_fd = accept(fd, (struct sockaddr *)&addr_client, &addr_len);
					if (client_fd < 0)
					{
						perror("ACCEPT ERROR");
						break;
					}
					make_socket_nonblocking(client_fd);
					t_event.events = EPOLLIN | EPOLLET; // Prêt à lire
					t_event.data.fd = client_fd;
					if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &t_event) == -1)
					{
						perror("EPOLL_CTL ADD CLIENT ERROR");
						close(client_fd);
						break;
					}
				}
				else if (t_events[i].events & EPOLLIN)
				{
					if (handle_read_event(fd, epoll_fd, data) == -1) // Fonction pour lire les données
    					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL), close(fd);
				}
				else if (t_events[i].events & EPOLLOUT)
				{
					if (handle_write_event(fd, epoll_fd, data) == -1) // Fonction pour envoyer la réponse
						epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL), close(fd);
				}
			}
		}
	}
}

bool is_request_complete(const char *request, size_t request_len)
{
    const char *end_of_headers = strstr(request, "\r\n\r\n");
	uint content_len;
	uint body_len;

    if (!end_of_headers)
        return false;
    const char *content_len_header = strstr(request, "Content-Length: ");
    if (content_len_header)
	{
        content_len = atoi(content_len_header + 16);
        body_len = request_len - (end_of_headers - request + 4);
		if (body_len < content_len)
			return false;
    }
    return true;
}

bool should_close_connection(const std::string &request)
{
    size_t 			pos = 0;
    std::string 	line;
    bool 			found_connection_header = false;

    while ((pos = request.find("\r\n", pos)) != std::string::npos)
	{
        line = request.substr(pos, request.find("\r\n", pos) - pos);

        if (line.find("Connection:") == 0) {
            found_connection_header = true;
            std::transform(line.begin(), line.end(), line.begin(), ::tolower);  // Comparaison insensible à la casse
            if (line.find("connection: close") != std::string::npos) {
                return true;
            }
            if (line.find("connection: keep-alive") == std::string::npos) {
                return true;
            }
            break; 
        }
        pos = request.find("\r\n", pos) + 2;
    }
    if (!found_connection_header)
        return true;
    return false;
}

int handle_read_event(int client_fd, int epoll_fd, WebServer &data)
{
    static std::map<int, std::string> 	save;
    char 								buffer[4096];
    int 								bytesRead;
    struct epoll_event 					t_event;
    std::string 						request;

    make_socket_nonblocking(client_fd);

    while ((bytesRead = recv(client_fd, buffer, sizeof(buffer) - 1, 0)) > 0)
	{
        request.append(buffer, bytesRead);
        if (bytesRead < (int)sizeof(buffer) - 1)
            break;
    }
    if (bytesRead < 0)
        return -1;
    save[client_fd] += request;
    if (!is_request_complete(save[client_fd].c_str(), save[client_fd].size()))
        return 0;
    HandleRequests requestHandler(save[client_fd], data, epoll_fd, client_fd);
    data.setResponseBuffer(client_fd, requestHandler.getResponse());

    save.erase(client_fd);

    t_event.events = EPOLLOUT | EPOLLET;
    t_event.data.fd = client_fd;

	if (!should_close_connection(save[client_fd]))
		return (-1);
    return epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client_fd, &t_event) == -1 ? -1 : 0;
}

int handle_write_event(int client_fd, int epoll_fd, WebServer &data)
{
    int					fileFd;
    std::string			body;
    static size_t		offset = 0; // Pour suivre l'écriture
    ssize_t				bytes_written, bytes_sent;
    const std::string &response = data.getResponseBuffer(client_fd);

    make_socket_nonblocking(client_fd);

    if (!data.postFileFdAbsent(client_fd)) {
        fileFd = data.getPostFileFds(client_fd);
        body = data.getPostBody(client_fd);
        
        bytes_written = write(fileFd, body.c_str() + offset, body.size() - offset);
        if (bytes_written < 0) {
            perror("Write error");
            close(fileFd);
            data.removePostFileFds(client_fd);
            return -1;
        }
        offset += bytes_written;
        if (offset >= body.size()) {
            close(fileFd);
            data.removePostFileFds(client_fd);
            offset = 0;
        }
    }

    if (data.responseBufferAbsent(client_fd))
        return -1;

    bytes_sent = send(client_fd, response.c_str(), response.size(), 0);
    if (bytes_sent < 0) {
        perror("SEND ERROR");
        return -1;
    }

    data.eraseResponseBuffer(client_fd);
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
    close(client_fd);
    return 0;
}