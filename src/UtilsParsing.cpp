#include "../includes/Parser.hpp"
#include "../includes/ServerBlock.hpp"
#include "../includes/server.hpp"
#include "../includes/LocationBlock.hpp"

ServerBlock parseServerBlock (std::vector<std::string> serverBlock)
{
    ServerBlock parsedBlock;
    for(size_t i = 0; i < serverBlock.size(); i ++)
    {
        if(serverBlock[i] == "listen")
        {
            expectSemicolon(serverBlock, i + 2);
            parsedBlock.setPort(parseListen(serverBlock, i)); //set port
        }
        else if (i + 1 < serverBlock.size() && serverBlock[i] == "server_name")
        {
            expectSemicolon(serverBlock, i + 2);
            parsedBlock.setServerName(serverBlock[i + 1]); //make a function where you can do semicolon checks here.
        } 
        else if(serverBlock[i] == "client_max_body_size")
        {
            expectSemicolon(serverBlock, i + 2);
            parsedBlock.setClientBodySize(parseMaxBodySize(serverBlock, i));
        }
        else if(serverBlock[i] == "location")
        {
            LocationBlock newLocation = parseLocationBlock(serverBlock, i);
            parsedBlock.addLocation(newLocation);
        }

    }
    return parsedBlock;
}

//add here later the syntax check if everything is correct as an input.
//check if something is found i do i +=2 and t;hen else i++ instead of this. 
LocationBlock parseLocationBlock(std::vector<std::string> tokens, size_t i)
{
    LocationBlock location;

    location.setPath(tokens[i + 1]);
    i += 3; //skip location, path and {
    while(tokens[i] != "}")
    {
        if(i + 1 >= tokens.size())
            break;
        if(tokens[i] == "root")
        {
            expectSemicolon(tokens, i + 2);
            location.setRoot(tokens[i + 1]);
        }
        else if (tokens[i] == "index")
        {
            expectSemicolon(tokens, i + 2);
            location.setIndex(tokens[i + 1]);
        }
        i++;
    }
    return location;
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

void expectSemicolon (const std::vector<std::string>& tokens, size_t index)
{
    if (index >= tokens.size() || tokens[index] != ";")
        throw std::runtime_error("Expected ';' after all directive");
    if (index + 1 < tokens.size() && tokens[index + 1] == ";")
        throw std::runtime_error("Unexpected extra ';' after directive");
}