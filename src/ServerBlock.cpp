/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ServerBlock.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: snijhuis <snijhuis@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2025/06/17 13:51:03 by snijhuis      #+#    #+#                 */
/*   Updated: 2025/06/19 16:44:34 by snijhuis      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ServerBlock.hpp"



ServerBlock::ServerBlock() : port(-1), server_name(""), client_max_body_size(-1) //locations() calls default constructor of vector.
{
    
}

ServerBlock::~ServerBlock()
{
}

int ServerBlock::getPort()
{
    return this->port;
}

std::string ServerBlock::getServerName()
{
    return this->server_name;
}
void ServerBlock::setPort(int port)
{
    this->port = port;
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

