#pragma once

#include <sstream>
#include "server.hpp"
#include "Client.hpp"
#include <fstream>
#include <ctime>
#include <cstdio>
#include <unistd.h>
#include <sys/wait.h>

class Cgi {
	private:
		HttpRequest&	_request;
		const std::string			_cgiPath;
		const std::string			_cgiPass;
		int					_requestPipe[2];
		int					_responsePipe[2];
		pid_t				_pid;
		std::string			_requestMethod;
		std::string			_queryString;
		std::string			_contentLength;
		std::string			_contentType;
		std::string			_scriptFileName;
		std::string			_pathInfo;
		std::string			_serverProtocol;
		std::string			_serverPort;
		std::string			_gatewayInterface;
	public:
		Cgi(HttpRequest& request);
		Cgi(HttpRequest& request, const std::string cgiPath, const std::string cgiPass,\
			 const std::string serverPort);
		~Cgi();
		void executeCgi();
		void childProcess();
		void parentProcess();
		void populateEnvironmentVariables();
		std::string findQueryString(const std::string& uri);
		std::string findPathInfo(const std::string& uri);
		void executeExecve();
		std::vector<char*> createEnvironmentVariableVector();
		void closePipes(std::string mode);
		void giveBodyToChild();
		void readCgiResponse(std::string& response);
		void parseResponse(std::string& rawResponse);
		void putHeaderInMap(std::unordered_map<std::string, std::string>& headers, std::string& headerString);
};