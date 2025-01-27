#include "../../includes/webserv.hpp"

void	postMethods(std::string buffer)
{
	if (!strncmp("POST /shutdown", buffer.c_str(), 12))
		throw (std::out_of_range("ask to close server"));
}