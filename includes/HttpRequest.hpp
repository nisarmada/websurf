#pragma once

// #include "HttpRequest.hpp"
#include <string>
#include <unordered_map>
#include <vector>
#include "Client.hpp"
class ServerBlock;

class HttpRequest {
	public:
		HttpRequest();
		~HttpRequest();
		void setMethod(const std::string& method);
		void setUri(const std::string& uri);
		void setHttpVersion(const std::string& httpVersion);
		void addHeader(const std::string& key, const std::string& value);
		void addBody(const char* data, size_t len);
		void setError(int errorCode);
		std::string& getMethod() const;
		std::string& getUri() const;
		std::string& getHttpVersion() const ;
		std::string& getHeader(const std::string& key);
		void checkRequest(Client& client);
		int checkMethod();
		int contentLengthCheck(Client& client);
		void parseRequestLine(const std::string& line);
		void parser(const std::vector<char>& requestBuffer);
		void parseHostLine(const std::string& line);
	private:
		std::string _method;
		std::string _uri;
		std::string _httpVersion;
		std::unordered_map<std::string, std::string> _headers;
		std::vector<char> body;
		int isError = 0;
};