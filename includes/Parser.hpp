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


class LocationBlock;
class ServerBlock;

enum Type {
    BLOCK,
    DIRECTIVE,
    DIRECTIVE2,
    UNKNOWN
};

std::vector<std::string> trimBeginEnd (std::ifstream& file);
std::ifstream openConfigFile(const char* path);
void parsing(const char* path);
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

