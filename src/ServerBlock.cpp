/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ServerBlock.cpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: snijhuis <snijhuis@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2025/06/17 13:51:03 by snijhuis      #+#    #+#                 */
/*   Updated: 2025/06/17 14:09:15 by snijhuis      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ServerBlock.hpp"



ServerBlock::ServerBlock() : listen(-1), server_name("") //locations() calls default constructor of vector.
{
}

ServerBlock::~ServerBlock()
{
}

int ServerBlock::getPort()
{
    return this->listen;
}

std::string ServerBlock::getServerName()
{
    return this->server_name;
}
