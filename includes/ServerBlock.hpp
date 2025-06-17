/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ServerBlock.hpp                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: snijhuis <snijhuis@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2025/06/17 13:50:47 by snijhuis      #+#    #+#                 */
/*   Updated: 2025/06/17 14:09:03 by snijhuis      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Parser.hpp"

class ServerBlock
{
    private:
        int listen;
        std::string server_name;
        // std::vector<std::string> locations; //make a class for locations.
    public:
        ServerBlock();
        ~ServerBlock();
        int getPort();
        std::string getServerName();
        // std::vector<std::string> getLocation(std::string identifier);
};