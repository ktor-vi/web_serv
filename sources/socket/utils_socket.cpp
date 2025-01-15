/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_socket.cpp                                   :+:      :+:    :+:   */
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

