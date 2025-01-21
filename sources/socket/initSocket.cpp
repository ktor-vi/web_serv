/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initSocket.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jepatern <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 13:55:39 by jepatern          #+#    #+#             */
/*   Updated: 2025/01/15 13:55:44 by jepatern         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../../includes/webserv.hpp"

void	ft_setup_socket(int *server_fd, int port, int epoll_fd, struct epoll_event *ptr)
{
	struct sockaddr_in	addr_serv;
	int opt = 1;

	*server_fd = socket(AF_INET, SOCK_STREAM, 0);	// socket creation as ipv4
	if (*server_fd == -1)
		perror("Error creation socket\n"), exit(1);
    if (setsockopt(*server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        close(*server_fd);
        exit(1);
    }
	make_socket_nonblocking(*server_fd);
	memset(&addr_serv, 0, sizeof(addr_serv));
	addr_serv.sin_family = AF_INET;					// server will use ipv4
	addr_serv.sin_port = htons(port);				
	inet_pton(AF_INET, "127.0.0.1", &addr_serv.sin_addr); // server will listen 127.0.0.1:808x
	if (bind(*server_fd, (const struct sockaddr *)&addr_serv, sizeof(addr_serv)) == -1) // link socket to the addr:port
		perror("BIND ERROR\n"), exit(1);
	if (listen(*server_fd, 5) < 0) 														//lock socket in listen mode
		perror("LISTEN ERROR\n"), exit(1);
	ptr->events = EPOLLIN;
	ptr->data.fd = *server_fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, *server_fd, ptr) == -1)
		perror("EPOLL_CTL ERROR"), exit(1);
	printf("SERVER ON 127.0.0.1:%d IS ONLINE\n", port);
}

void ft_setup_all_socket(WebServer &data, int epoll_fd, struct epoll_event *ptr)
{
	int socket_fd;
	int i = -1;
	int index_max = data.getNumberOfServers();

	while (++i != index_max)
	{
		printf("PORT server%d == %d\n", i, data.getPort(i));
		ft_setup_socket(&socket_fd, data.getPort(i), epoll_fd, ptr);
		data.setSocketFd(i, socket_fd);
	}
}