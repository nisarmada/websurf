#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <algorithm>
#include <map>
#include "Client.hpp"


const std::string findLongestMatch(const std::string& uri, const std::map<std::string, LocationBlock>& locations);
bool isDirectory(std::string file);
bool isCgi(const std::string& cgi);
bool cgiPathIsValid(const std::string& path);