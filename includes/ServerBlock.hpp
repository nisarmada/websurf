#pragma once
#include "Parser.hpp"
#include "LocationBlock.hpp"

class ServerBlock
{
    private:
        int port;
        std::string server_name;
        size_t client_max_body_size;
        std::map<std::string, LocationBlock> _locations;
    public:
        ServerBlock();
        ~ServerBlock();
        void setPort(int port);
        int getPort();
		size_t getBodySize() const;
        void setServerName(std::string serverName);
        size_t getClientBodySize();
        void setClientBodySize(size_t maxSize);
        const std::string getServerName() const;
        void addLocation(const LocationBlock& location);
        const std::map<std::string, LocationBlock>& getLocations() const;
        void validateServerBlock() const;
        void expectSemicolon (const std::vector<std::string>& tokens, size_t index);

};
