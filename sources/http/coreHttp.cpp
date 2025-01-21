#include "../../includes/webserv.hpp"

void send_http_response_header(int client_fd, const char *content_type, ssize_t content_length)
{
	char	response_header[1024];

	snprintf(response_header, sizeof(response_header),
			 "HTTP/1.1 200 OK\r\n"
			 "Content-Type: %s\r\n"
			 "Content-Length: %zd\r\n\r\n",
			 content_type, content_length);
	send(client_fd, response_header, strlen(response_header), 0);
}

int handle_request(int client_fd)
{
	ssize_t 	bytes;
	char		str[1024];
	char		buffer_page[8192];
	char		buffer[2048] = {0};
	int			fd_page;

	bytes = recv(client_fd, buffer, 2048, 0);
	if (bytes <= 0)
		return 0; // No data or error, return 0
	buffer[bytes] = '\0'; // Null-terminate the received data
	printf("|||||||||||>>>> HTTP REQUEST RECEIVED ->\n%s==== ", buffer);

	if (!strncmp("GET / HTTP/1.1", buffer, 14) && printf("Answer /void\n"))
	{
		// if 127.0.0.1::8080, send facebook html, if 8081 twitter html
		fd_page = open("var/www/html/index.html", O_RDONLY);
		if (fd_page < 0)
			fd_page = open("var/www/errors/error404.html", O_RDONLY);
		bytes = read(fd_page, buffer_page, sizeof(buffer_page) - 1);
		if (bytes > 0)
		{
			send_http_response_header(client_fd, "text/html", bytes);
			send(client_fd, buffer_page, bytes, 0);
		}
		 close(fd_page);
	}
	else if (!strncmp("GET /random", buffer, 10) && printf("Answer /random\n"))
	{
		send_http_response_header(client_fd, "text/plain", 3);
		send(client_fd, ft_call_cgi(str), 3, 0);
		return 1; // Successfully handled
	}
	else if (!strncmp("POST /shutdown", buffer, 12) && printf("Answer /shutdown\n"))
		return -1; // Signal to shut down the server
	return 1; // Request was not handled, continue with normal page request
}
