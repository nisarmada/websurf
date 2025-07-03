#pragma once

// #include "HttpRequest.hpp"
#include <string>
#include <unordered_map>
#include <vector>

class HttpRequest {
	public:
		HttpRequest();
		~HttpRequest();
		void setMethod(const std::string& method);
		void setUri(const std::string& uri);
		void setHttpVersion(const std::string& httpVersion);
		void addHeader(const std::string& key, const std::string& value);
		void addBody(const char* data, size_t len);
		const std::string& getMethod() const;
		const std::string& getUri() const;
		std::string& getHttpVersion() const ;
		std::string& getHeader(const std::string& key);
	private:
		std::string _method;
		std::string _uri;
		std::string _httpVersion;
		std::unordered_map<std::string, std::string> _headers;
		std::vector<char> body;
};