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

void ft_bomb_as_been_defused(char *msg)
{
	perror(msg);
	exit(errno);
}

char *ft_call_cgi(char *ans)
{
	int fds[2];
    const char	*python_path = "/usr/bin/python3";
    const char	*script_path = "CGI.py";
    char *const argv[] = { "python3", (char *)script_path, NULL }; // Format for execve

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

void	ft_setup_socket(int *server_fd)
{
	struct sockaddr_in	addr_serv;
	int opt = 1;

	*server_fd = socket(AF_INET, SOCK_STREAM, 0);	// socket creation as ipv4
    if (setsockopt(*server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) // FORBIDDEN FUNCTION !!!
        ft_bomb_as_been_defused("SETSOCKOPT ERROR\n");
	memset(&addr_serv, 0, sizeof(addr_serv));		// switch any bytes to 0
	addr_serv.sin_family = AF_INET;					// server will use ipv4
	addr_serv.sin_port = htons(8080);				
	inet_pton(AF_INET, "127.0.0.1", &addr_serv.sin_addr); // server will listen 127.0.0.1:8080

	if (bind(*server_fd, (const struct sockaddr *)&addr_serv, sizeof(addr_serv)) == -1) // link socket the addr:port
		ft_bomb_as_been_defused("BIND ERROR\n");
	if (listen(*server_fd, 5) < 0) 														//lock socket in listen mode
		ft_bomb_as_been_defused("LISTEN ERROR\n");
	printf("SERVER IS ONLINE\n");
}

void send_http_response_header(int client_fd, const char *content_type, size_t content_length) {
    char response_header[256];

    snprintf(response_header, sizeof(response_header),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: %s\r\n"
             "Content-Length: %zu\r\n\r\n",
             content_type, content_length);

    send(client_fd, response_header, strlen(response_header), 0);
}

void ft_webserver(void) {
    struct sockaddr_in 	addr_client;
    socklen_t 			addr_len;
    int 				server_fd, client_fd, fd_page;
    ssize_t 			bytes_received, bytes_read;
    char 				buffer[1024], buffer_page[8192], str[1024];

    addr_len = sizeof(addr_client);
    ft_setup_socket(&server_fd);
    while (1)
	{
        client_fd = accept(server_fd, (struct sockaddr *)&addr_client, &addr_len); // Wait for a request
        if (client_fd < 0) {
            perror("ACCEPT ERROR");
            continue;
        }
        bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            close(client_fd);
            continue;
        }
        buffer[bytes_received] = '\0';
        printf(">>>>%s<<<<\n\n", buffer);
        if (!strncmp("GET /random", (const char *)buffer, 10)) {
            send_http_response_header(client_fd, "text/plain", 3);
            send(client_fd, ft_call_cgi(str), 3, 0);
        } else if (!strncmp("POST /shutdown", (const char *)buffer, 12)) {
            close(server_fd);
            close(client_fd);
            exit(1);
        } else {
            fd_page = open("html/index.html", O_RDONLY);
            if (fd_page < 0) {
                perror("File open error");
                close(client_fd);
                continue;
            }
            bytes_read = read(fd_page, buffer_page, sizeof(buffer_page) - 1);
            if (bytes_read > 0) {
                send_http_response_header(client_fd, "text/html", bytes_read);
                send(client_fd, buffer_page, bytes_read, 0);
            }
            close(fd_page);
        }
        close(client_fd);
    }
}
