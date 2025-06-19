/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ServerBlock.hpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: snijhuis <snijhuis@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2025/06/17 13:50:47 by snijhuis      #+#    #+#                 */
/*   Updated: 2025/06/19 16:43:32 by snijhuis      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Parser.hpp"

class ServerBlock
{
    private:
        int port;
        std::string server_name;
        size_t client_max_body_size;
        // std::vector<std::string> locations; //make a class for locations.
    public:
        ServerBlock();
        ~ServerBlock();
        void setPort(int port);
        int getPort();
        void setServerName(std::string serverName);
        size_t getClientBodySize();
        void setClientBodySize(size_t maxSize);
        std::string getServerName();
        // std::vector<std::string> getLocation(std::string identifier);
};