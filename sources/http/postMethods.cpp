#include "../../includes/webserv.hpp"

void	postMethods(char *buffer)
{
	if (!strncmp("POST /shutdown", buffer, 12))
		throw (std::out_of_range("ask to close server"));
}