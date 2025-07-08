#pragma once

#include <sstream>
#include "server.hpp"
#include "HttpRequest.hpp"

class HttpResponse {
	public:
		HttpResponse();
		~HttpResponse();
		void setHttpVersion(const std::string& version);
		void setStatusCode(const int status);
		void setText(const std::string& text);
		void addHeader(const std::string& key, const std::string& value);
		void setBody(const std::vector<char>& body);
		std::string responseToString();
		std::string& getHttpVersion();
		int getStatusCode();
		std::string& getText();
		std::vector<char>& getBody();
		void executeResponse(HttpRequest& request);
		void executeGet(HttpRequest& request);
		const std::string& getRoot() const;
		void createBodyVector();
		void populateHeaders(HttpRequest& request);
		void populateErrorHeaders();
		void findContentType();
		std::string setErrorText();
		std::string createCompleteResponse();
	private:
		const std::string _root;
		std::string _httpVersion;
		int _statusCode;
		std::string _text;
		std::unordered_map<std::string, std::string> _headers;
		std::vector<char> _body;
		size_t _bodyLen;
		std::string _path;
};

