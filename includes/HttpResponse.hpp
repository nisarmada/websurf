#pragma once

#include "server.hpp"

class HttpResponse {
	public:
		HttpResponse();
		~HttpResponse();
		void setHttpVersion(const std::string& version);
		void setStatusCode(const int status);
		void setText(const std::string& text);
		void addHeader(const std::string& key, const std::string& value);
		void setBody(const std::vector<char>& body);
		std::vector<char> responseToBuffer();
		std::string& getHttpVersion();
		int getStatusCode();
		std::string& getText();
		std::vector<char>& getBody();
	private:
		std::string _httpVersion;
		int _statusCode;
		std::string _text;
		std::unordered_map<std::string, std::string> _headers;
		std::vector<char> _body;
		size_t _bodyLen;
};