#pragma once

#include <sys/socket.h>  // For socket(), connect(), bind(), listen(), accept()
#include <netinet/in.h>  // For sockaddr_in, htons(), htonl()
#include <arpa/inet.h>   // For inet_addr(), inet_ntoa()
#include <unistd.h>      // For close()
#include <cstring>       // For memset()
#include <iostream>      // For std::cout, std::cerr>
#include <fstream>
#include <vector>
#include <map>
#include <stack>
#include <limits>
#include <set>

class WebServer;
class LocationBlock;
class ServerBlock;

enum Type {
    BLOCK,
    DIRECTIVE, //needs i + 2 for ;
    DIRECTIVE2,//needs i + 3 for ;
    IGNORE,//dont need to check immediatly for semicolon
    UNKNOWN
};

std::vector<std::string> trimBeginEnd (std::ifstream& file);
std::ifstream openConfigFile(const char* path);
WebServer parsing(const char* path);
std::vector<std::string> tokenizeLine(std::string line);
bool isSpecialChar(char c);
bool checkSyntax(std::vector<std::string> tokens);
std::vector<std::string> getAllTokens(const std::vector<std::string>& cleanedLines);
void checkBrackets(std::vector<std::string> tokens);
void checkSemicolons(std::vector<std::string> tokens);
Type getType(const std::string& token);
void wrongPlaceSemicolon(std::vector<std::string> tokens, size_t i);
void mustHaveSemicolon(std::vector<std::string> tokens, size_t i);
void checkBracketStructure(std::vector<std::string> tokens);
void checkValidBracketOpening(std::vector<std::string> tokens);
std::vector<std::vector<std::string>> getServerBlockTokens(std::vector<std::string> tokens);

//utils file:
LocationBlock parseLocationBlock(std::vector<std::string> tokens, size_t i);
int parseListen(std::vector<std::string> tokens, size_t i);
ServerBlock parseServerBlock (std::vector<std::string> serverBlock);
bool stringIsDigit(std::string& str);
size_t parseMaxBodySize(std::vector<std::string> tokens, size_t i);
void expectSemicolon (const std::vector<std::string>& tokens, size_t index);
void parseMethods(std::vector<std::string> tokens, size_t i, LocationBlock& location);
void parseRedirectUrl(std::vector<std::string> tokens, size_t i, LocationBlock& location);
void parseUploadPath(std::vector<std::string> tokens, size_t i, LocationBlock& location);
void parseCgiPass(std::vector<std::string> tokens, size_t i, LocationBlock& location);
void parseAutoindex(std::vector<std::string> tokens, size_t i, LocationBlock& location);
void parseRoot(std::vector<std::string> tokens, size_t i, LocationBlock& location);
void parseIndex(std::vector<std::string> tokens, size_t i, LocationBlock& location);