/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Parser.hpp                                         :+:    :+:            */
/*                                                     +:+                    */
/*   By: snijhuis <snijhuis@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2025/06/09 14:53:27 by snijhuis      #+#    #+#                 */
/*   Updated: 2025/06/09 17:52:04 by snijhuis      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <sys/socket.h>  // For socket(), connect(), bind(), listen(), accept()
#include <netinet/in.h>  // For sockaddr_in, htons(), htonl()
#include <arpa/inet.h>   // For inet_addr(), inet_ntoa()
#include <unistd.h>      // For close()
#include <cstring>       // For memset()
#include <iostream>      // For std::cout, std::cerr>
#include <fstream>
#include <vector>

std::vector<std::string> trimBeginEnd (std::ifstream& file);
std::ifstream openConfigFile(const char* path);
void parsing(const char* path);
std::vector<std::string> tokenizeLine(std::string line);
bool isSpecialChar(char c);
bool checkSyntax(std::vector<std::vector<std::string>> tokens);
bool checkSemicolons(std::vector<std::vector<std::string>> tokens);
bool needSemicolon(std::vector<std::string> tokenLine);