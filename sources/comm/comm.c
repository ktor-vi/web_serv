#include "../../includes/webserv.hpp"

/*
Résumé des fonctions finales

    Sockets : socket, bind, listen, accept.
    Événements : epoll_create, epoll_ctl, epoll_wait (ou poll).
    Communication : send, recv.
    DNS : getaddrinfo, freeaddrinfo.
    Processus : fork, signal, kill.
    Fichiers : close, fcntl.
*/

typedef struct	s_server_block
{
	char *listening_ip_address;
	char *listening_port;
	


}				t_server_block;

void ft_bomb_as_been_defused(char *msg)
{
	perror(msg);
	exit(errno);
}

void ft_comm(void) // execute socket
{
	struct sockaddr_in	addr_serv;
	struct sockaddr_in	addr_client;
    socklen_t			addr_len = sizeof(addr_client);
    int					server_fd;
	int					client_fd;
	int					count;
    char				buffer[1024] = {0};
	
	server_fd = socket(AF_INET, SOCK_STREAM, 0); // creation socket as ipv4

	memset(&addr_serv, 0, sizeof(addr_serv));
	addr_serv.sin_family = AF_INET; // server will use ipv4
	addr_serv.sin_port = htons(8080);
	inet_pton(AF_INET, "127.0.0.1", &addr_serv.sin_addr); // server will listen 127.0.0.1:8080

	if (bind(server_fd, (struct sockaddr *)&addr_serv, sizeof(addr_serv)) == -1) // link socket the addr:port
		ft_bomb_as_been_defused("BIND ERROR\n");

	if (listen(server_fd, 5) < 0) //lock socket in listen mode
		ft_bomb_as_been_defused("LISTEN ERROR\n");

	printf("SERVER IS ONLINE\n");
    while (1)
	{
        client_fd = accept(server_fd, (struct sockaddr *)&addr_client, &addr_len); // Wait a request

        if (client_fd < 0)
            ft_bomb_as_been_defused("ACCEPT ERROR\n");

		recv(client_fd, &buffer, 1024, 0); //Read and put in buffer the received message

		send(client_fd, "CONNECTION++++\n", 15, 0);
		send(client_fd, buffer, strlen(buffer), 0); //use what has beed received as an answer 
		send(client_fd, "----CONNECTION\n", 15, 0);

		write(1, buffer, strlen(buffer)); // print all received message in terminal
		close(client_fd);

		memset(&buffer, 0, 1024);
	}
	close(server_fd);
}

int main (void)
{
	ft_comm();

	return (0);
}