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
    //check if the getpath already exists for the location, throw an exception then.
    const std::string& path = location.getPath();
    if(_locations.find(path) != _locations.end()) //if founds returns iterator at right position if not returns end for indication not found.
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



