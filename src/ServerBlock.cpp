#include "../includes/ServerBlock.hpp"



ServerBlock::ServerBlock() : port(-1), server_name(""), client_max_body_size(0) //locations() calls default constructor of vector.
{

}

ServerBlock::~ServerBlock()
{
}

int ServerBlock::getPort() const
{
    return this->port;
}

const std::string ServerBlock::getServerName() const
{
    return this->server_name;
}
void ServerBlock::setPort(int port)
{
	if(this->port != -1)
	{
		std::cerr << "Error: multiple listening directives" << std::endl;
		exit(1);
	}
    this->port = port;
}


size_t ServerBlock::getBodySize() const {
	return client_max_body_size;
}

void ServerBlock::setServerName(std::string serverName)
{
    this->server_name = serverName;
}

size_t ServerBlock::getClientBodySize()
{
    return this->client_max_body_size;
}

void ServerBlock::setClientBodySize(size_t maxSize)
{
    this->client_max_body_size = maxSize;
}

void ServerBlock::addLocation(const LocationBlock& location)
{
    const std::string& path = location.getPath();
    if(_locations.find(path) != _locations.end()) //if found returns iterator at right position if not returns end for indication not found.
        throw std::runtime_error("Duplicate location path: " + path);
    _locations[location.getPath()] = location;
}



const std::map<std::string, LocationBlock>& ServerBlock::getLocations() const
{
    return _locations;
}

void ServerBlock::validateServerBlock() const
{
    if(port == -1)
        throw std::runtime_error("Missing 'listen' directive in server block");
    if(server_name.empty())
        throw std::runtime_error("Missing 'server_name' directive in server block");
    if(client_max_body_size == 0)
        throw std::runtime_error("Missing 'client_max_body_size' directive in server block");
    if(_locations.empty())
        throw std::runtime_error("Missing 'location block' in server block");
}

void ServerBlock::setErrorPage(const std::string& code_str, const std::string& path)
{
	int code = std::atoi(code_str.c_str());
     _errorPages[code] = path;
}

std::string ServerBlock::getErrorPagePath(int code) const
{
	return _errorPages.at(code);
}

bool ServerBlock::hasErrorPage(int code) const
{
	return _errorPages.find(code) != _errorPages.end();
}
