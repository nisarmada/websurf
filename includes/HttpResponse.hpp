#pragma once

#include <sstream>
#include "server.hpp"
#include "HttpRequest.hpp"
#include "Client.hpp"
#include <fstream>
#include <ctime>
#include <cstdio>
#include "Cgi.hpp"
#include <sys/stat.h>
#include <dirent.h>  
#include <sys/stat.h>  
#include <sstream>  

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
		void executeResponse(HttpRequest& request, Client& client);
		void executeGet(HttpRequest& request, Client& client);
		void executePost(HttpRequest& request, Client& client);
		void executeDelete(HttpRequest& request, Client& client);
		const std::string& getRoot() const;
		void createBodyVector(Client& client, HttpRequest& request);
		void populateHeaders(HttpRequest& request);
		void populateErrorHeaders();
		void findContentType();
		std::string setErrorText();
		std::string createCompleteResponse();
		static void handleResponse(Client& client, WebServer& server);
		void executeCgi();
		bool checkAllowedMethods(Client& client, std::string check);
		void handleError(Client& client, HttpRequest& request);
		bool handleAutoindex(HttpRequest& request, Client& client);
		bool handleDirectoryRedirect(std::string& uri, std::string& fullPath);
		bool setBodyFromFile(Client& client, HttpRequest& request);
		void setBodyFromDirectoryList(Client& client, HttpRequest& request);
		void sendRedirect(const std::string& url);
		void populateFullPath(HttpRequest& request, Client& client);
		std::string buildFullUrl(HttpRequest& request);




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

