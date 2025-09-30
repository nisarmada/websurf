#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <algorithm>
#include <map>
#include "Client.hpp"
#include <sys/stat.h>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

const std::string findLongestMatch(const std::string& uri, const std::map<std::string, LocationBlock>& locations);
bool isDirectory(std::string file);
bool isCgi(HttpRequest& request, Client& client);
bool cgiPathIsValid(HttpResponse& response, HttpRequest& request, Client& client);