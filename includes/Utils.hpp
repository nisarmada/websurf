#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <algorithm>
#include <map>
#include "Client.hpp"
#include <sys/stat.h>

const std::string findLongestMatch(const std::string& uri, const std::map<std::string, LocationBlock>& locations);
bool isDirectory(std::string file);
bool isCgi(const std::string& cgi);
bool cgiPathIsValid(const std::string& path);