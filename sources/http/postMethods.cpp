#include "../../includes/webserv.hpp"

void	HandleRequests::postMethods(WebServer &webServerData)
{
	(void) webServerData;
	if (!strncmp("POST /shutdown", this->buffer.c_str(), 12))
		throw (std::out_of_range("ask to close server"));
}