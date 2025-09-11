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
        else if(serverBlock[i] == "error_page")
        {
            expectSemicolon(serverBlock, i + 3);
            parsedBlock.setErrorPage(serverBlock[i + 1], serverBlock[i + 2]);
        }
    }
    return parsedBlock;
}

//add here later the syntax check if everything is correct as an input. CHECK
//check if something is found i do i +=2 and t;hen else i++ instead of this. 
LocationBlock parseLocationBlock(std::vector<std::string> tokens, size_t& i)
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
            parseRoot(tokens, i, location);
        } 
        else if (tokens[i] == "index")
            parseIndex(tokens, i, location);
        else if (tokens[i] == "methods")
            parseMethods(tokens, i, location);
        else if (tokens[i] == "autoindex")
            parseAutoindex(tokens, i, location);
        else if (tokens[i] == "return")
            parseRedirectUrl(tokens, i, location);
        else if(tokens[i] == "upload_path")
            parseUploadPath(tokens, i, location);
        else if(tokens[i] == "cgi_pass")
            parseCgiPass(tokens, i, location);
        i++;
    }
    return location;
}

void parseIndex(std::vector<std::string> tokens, size_t i, LocationBlock& location)
{
    if(i + 1 >= tokens.size())
        throw std::runtime_error("Missing value after 'index' directive.");
    if(!location.getIndex().empty())
        throw std::runtime_error("Multiple definitions of 'index' directive.");
    
    location.setIndex(tokens[i + 1]);
}

void parseRoot(std::vector<std::string> tokens, size_t i, LocationBlock& location)
{
    if(i + 1 >= tokens.size())
        throw std::runtime_error("Missing value after 'root' directive.");
    if(!location.getRoot().empty())
    {
        throw std::runtime_error("Multile definitions of 'root' directive.");
    }
    std::string root = tokens[i + 1];
    if(root[0] != '.')
        root = "." + root;

    location.setRoot(root);
}

void parseCgiPass(std::vector<std::string> tokens, size_t i, LocationBlock& location)
{
    if(i + 1 >= tokens.size())
        throw std::runtime_error("Missing value after 'cgi_pass' directive.");
    if(!location.getCgiPass().empty())
        throw std::runtime_error("Multiple defenitions of 'cgi_pass'.");
    location.setCgiPass(tokens[i + 1]);
}

void parseUploadPath(std::vector<std::string> tokens, size_t i, LocationBlock& location)
{
    if(i + 1 >= tokens.size())
        throw std::runtime_error("Missing value after 'upload_path' directive.");
    if(!location.getUploadPath().empty()) 
        throw std::runtime_error("Multiple definitions of 'upload_path'.");
    
    location.setUploadPath(tokens[i + 1]);
}

void parseRedirectUrl(std::vector<std::string> tokens, size_t i, LocationBlock& location)
{
    if(i + 1 >= tokens.size())
        throw std::runtime_error("Missing value after 'return' directive.");
    if(!location.getRedirectUrl().empty())
        throw std::runtime_error("Multiple definitions of 'return' directive.");
    location.setRedirectUrl(tokens[i + 1]);
}

void parseAutoindex(std::vector<std::string> tokens, size_t i, LocationBlock& location)
{
    if(i + 1 >= tokens.size())
        throw std::runtime_error("Missing value after 'autoindex' directive.");
    else if(location.getAutoindexDouble()) 
        throw std::runtime_error("Multiple definitions of 'autoindex' directive.");
    else if(tokens[i + 1] == "on")
    {
        location.setAutoindexDouble(true);
        location.setAutoindex(true);
        return;
    }
    else if(tokens[i + 1] == "off")
    {
        location.setAutoindexDouble(true);
        location.setAutoindex(false);
        return;
    }
    else
        throw std::runtime_error("Invalid value after 'autoindex' directive.");

}

void parseMethods(std::vector<std::string> tokens, size_t i, LocationBlock& location)
{
    std::set<std::string> allowed = {"GET", "POST", "DELETE"};

    if(i + 1 >= tokens.size())
        throw std::runtime_error("Expected HTTP method after 'methods' directive.");
    i++; //skip the methods token itself.
    while(tokens[i] != ";")
    {
        if(allowed.find(tokens[i]) == allowed.end()) //if not found it returns end.
            throw std::runtime_error("Invalid HTTP method.");
        location.addMethod(tokens[i]);
        i ++;
    }
    if (i >= tokens.size() || tokens[i] != ";") //check that a semicolon comes after the directive
        throw std::runtime_error("Missing semicolon after 'methods' directive.");
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
   
    //for my future: catch the exeption instead of this, stoull throws exception by overflow. CHECK
    if(overflowCheck > std::numeric_limits<size_t>::max())
    {
        std::cerr << "Error invalid client_max_body_size: " << tokens[i + 1] << std::endl;
        exit(1);
    }
    
    size_t maxBodySize = static_cast<size_t>(std::stoull(tokens[i + 1]));
    return maxBodySize;
}

int parseListen(std::vector<std::string> tokens, size_t i) //CHECK if i + 1 is smaller then tokens.size !!!!!!!!!!!!!
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
        if(!std::isdigit(static_cast<unsigned char>(str[i]))) 
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