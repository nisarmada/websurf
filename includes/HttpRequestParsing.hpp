#pragma once

#include "HttpRequest.hpp"
#include <string>
#include <algorithm>
#include <iostream>

class HttpRequestParser {
	public:
		HttpRequestParser();
		~HttpRequestParser();
		static HttpRequest parser(const std::vector<char>& requestBuffer);
	
	private:
		static void parseRequestLine(const std::string& line, HttpRequest& request);
		static void parseHostLine(const std::string& line, HttpRequest& request);
};