#include "../includes/Parser.hpp"
#include "../includes/ServerBlock.hpp"
#include "../includes/server.hpp"

ServerBlock parseServerBlock (std::vector<std::string> serverBlock)
{
    ServerBlock parsedBlock;
    for(size_t i = 0; i < serverBlock.size(); i ++)
    {
        if(serverBlock[i] == "listen")
            parsedBlock.setPort(parseListen(serverBlock, i)); //set port
        else if (i + 1 < serverBlock.size() && serverBlock[i] == "server_name")
            parsedBlock.setServerName(serverBlock[i + 1]); //make a function where you can do semicolon checks here. 
        else if(serverBlock[i] == "client_max_body_size")
            parsedBlock.setClientBodySize(parseMaxBodySize(serverBlock, i));

    }
    return parsedBlock;
}

size_t parseMaxBodySize(std::vector<std::string> tokens, size_t i)
{
    if(i + 1 >= tokens.size())
    {
        std::cerr << "Error missing value for client_max_body_size" << std::endl;
        exit(1);
    }
    if(!stringIsDigit(tokens[i + 1]))
    {
        std::cerr << "Error invalid client_max_body_size: " << tokens[i + 1] << std::endl;
        exit(1);
    }
    unsigned long long overflowCheck = std::stoull(tokens[i + 1]);
   
    //for my future: catch the exeption instead of this, stoull throws exception by overflow.
    if(overflowCheck > std::numeric_limits<size_t>::max())
    {
        std::cerr << "Error invalid client_max_body_size: " << tokens[i + 1] << std::endl;
        exit(1);
    }
    
    std::cout << "value: " << std::numeric_limits<size_t>::max() << std::endl;

    size_t maxBodySize = static_cast<size_t>(std::stoull(tokens[i + 1]));
   
    return maxBodySize;

}

int parseListen(std::vector<std::string> tokens, size_t i) //do a check if i + 1 is smaller then tokens.size !!!!!!!!!!!!!
{
    if(!stringIsDigit(tokens[i + 1]))
    {
        std::cerr << "Error invalid port: " << tokens[i + 1] << std::endl;
        exit(1);
    }
    int port = std::stoi(tokens[i + 1]);
    if (port < 1 || port > 65535) //allowed ports on a system
    {
        std::cerr << "Error invalid port range: " << tokens[i + 1] << std::endl;
        exit(1);
    }
    return port;
}

bool stringIsDigit(std::string& str)
{
    for(size_t i = 0; i < str.size(); i++)
    {
        if(!std::isdigit(static_cast<unsigned char>(str[i]))) //cast for if a char is minus like special characters so it doesnt crash.
            return false;
    }
    return true;
}