#include "../includes/Cgi.hpp"


Cgi::Cgi(HttpRequest& request) : _request(request) {
	_requestPipe[0] = -1;
	_requestPipe[1] = -1;
	_responsePipe[0] = -1;
	_responsePipe[1] = -1;
}

Cgi::Cgi(HttpRequest& request, const std::string cgiPath, const std::string cgiPass,\
	 const std::string serverPort) : _request(request), \
_cgiPath(cgiPath), _cgiPass(cgiPass), _serverPort(serverPort), _gatewayInterface("CGI/1.1") {
	_requestPipe[0] = -1;
	_requestPipe[1] = -1;
	_responsePipe[0] = -1;
	_responsePipe[1] = -1;
}

Cgi::~Cgi() {}

std::string Cgi::findQueryString(const std::string& uri){
	size_t questionmarkPosition = uri.find_first_of("?");
	if (questionmarkPosition == std::string::npos){
		return ("");
	}
	std::string queryString = uri.substr(questionmarkPosition + 1);
	return queryString;
}

std::string Cgi::findPathInfo(const std::string& uri){
	size_t scriptPos = uri.find(_cgiPath);
	if (scriptPos == std::string::npos){
		return "";
	}
	size_t queryPos = uri.find('?');
	size_t pathInfoStart = scriptPos + _cgiPath.length();
	if (queryPos != std::string::npos){
		return uri.substr(pathInfoStart, queryPos - pathInfoStart); //everything between the script name and the query string
	} else {
		return uri.substr(pathInfoStart); //everything after the script name
	}
}

void Cgi::populateEnvironmentVariables(){
	_requestMethod = _request.getMethod();
	if (_requestMethod == "GET"){
		_contentLength = "0";
	} else {
		_contentLength = _request.getHeader("Content-Length");
	}
	_contentType = _request.getHeader("Content-Type");
	_scriptFileName = _cgiPath;
	_queryString = findQueryString(_request.getUri());
	_pathInfo = findPathInfo(_request.getUri());
	_serverProtocol = _request.getHttpVersion();
}

std::vector<char*> Cgi::createEnvironmentVariableVector(){
	std::vector<std::string> envStr;
	envStr.push_back("REQUEST_METHOD=" + _requestMethod);
    envStr.push_back("CONTENT_LENGTH=" + _contentLength);
    envStr.push_back("CONTENT_TYPE=" + _contentType);
    envStr.push_back("SCRIPT_FILENAME=" + _scriptFileName);
    envStr.push_back("QUERY_STRING=" + _queryString);
    envStr.push_back("PATH_INFO=" + _pathInfo);
    envStr.push_back("GATEWAY_INTERFACE=" + _gatewayInterface);
    envStr.push_back("SERVER_PORT=" + _serverPort);
	std::vector<char*> envp;
	for (const auto& iterator : envStr){
		envp.push_back(strdup(iterator.c_str()));
	}
	envp.push_back(nullptr);
	return envp;
}

void Cgi::executeExecve(){
	char* argv[] = {
		const_cast<char*>(_cgiPass.c_str()),
		const_cast<char*>(_cgiPath.c_str()),
		NULL
	};
	std::vector<char*> envp = createEnvironmentVariableVector();
	if (execve(const_cast<char*>(_cgiPass.c_str()), argv, envp.data()) == -1){
		std::cerr << "Execve failed:(" << std::endl;
		for (const auto& iterator : envp){
			free(iterator);
		}
		exit(EXIT_FAILURE);
	}
}

void Cgi::childProcess(){
	closePipes("child");
	if (dup2(_requestPipe[0], STDIN_FILENO) == -1){
		std::cerr << "dup2 failed for input" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (dup2(_responsePipe[1], STDOUT_FILENO) == -1){
		std::cerr << "dup2 failed for output" << std::endl;
		exit(EXIT_FAILURE);
	}
	close(_requestPipe[0]);
	close(_responsePipe[1]);
	populateEnvironmentVariables();
	executeExecve();
}

void Cgi::closePipes(std::string mode){
	if (mode == "fail"){
		close(_requestPipe[0]);
		close(_requestPipe[1]);
		close(_responsePipe[0]);
		close(_responsePipe[1]);
	}
	else if (mode == "parent"){
		close(_requestPipe[0]);
		close(_responsePipe[1]);
	}
	else{
		close(_requestPipe[1]); //write end of request pipe
		close(_responsePipe[0]); //read end of response pipe
	}
}

void Cgi::giveBodyToChild(){
	const std::vector<char>& body = _request.getBody();
	
	if (!body.empty()){
		ssize_t bytesWritten = write(_requestPipe[1], body.data(), body.size());
		if (bytesWritten < 0){
			std::cerr << "Write to Cgi failed in parent process" << std::endl;
		}
	}
	close(_requestPipe[1]);

}

std::string& Cgi::getResponseString(){
	return _cgiResponse;
}

void Cgi::putHeaderInMap(std::unordered_map<std::string, std::string>& headers, std::string& headerString){
	size_t keyEnd = headerString.find(':');
	std::string key = headerString.substr(0, keyEnd);
	size_t startValuePos = headerString.find_first_not_of(' ', keyEnd + 1);
	std::string value = headerString.substr(startValuePos);

	headers[key] = value;
}


void Cgi::parseResponse(std::string& rawResponse, HttpResponse& response){
	size_t headersEnd = rawResponse.find("\r\n\r\n");

	if (headersEnd != std::string::npos){
		std::string headersPart = rawResponse.substr(0, headersEnd);
		std::string bodyPart = rawResponse.substr(headersEnd + 4);

		std::unordered_map<std::string, std::string> headers;
		size_t currentPosition = 0;
		size_t nextPosition;
		while ((nextPosition = headersPart.find("\r\n", currentPosition)) != std::string::npos){
			std::string headerLine = headersPart.substr(currentPosition, nextPosition - currentPosition);
			putHeaderInMap(headers, headerLine);
			currentPosition = nextPosition + 2;
		}
		if (currentPosition < headersPart.length()){
			std::string headerLine = headersPart.substr(currentPosition);
			putHeaderInMap(headers, headerLine);
		}
		for (const auto& pair : headers) {
			response.addHeader(pair.first, pair.second);
		}
		response.setBody(std::vector<char>(bodyPart.begin(), bodyPart.end()));
		response.setStatusCode(200);
		response.setHttpVersion("HTTP/1.1");
		response.setText("OK");
	}
	else{
		response.setStatusCode(500);
		response.populateErrorHeaders();
	}
}

int Cgi::executeCgi() {
	if (pipe(_requestPipe) == -1){
		std::cerr << "ERROR WITH REQUEST PIPE" << std::endl; //first stage errors
	}
	if (pipe(_responsePipe) == -1){
		std::cerr << "ERROR WITH RESPONSE PIPE" << std::endl;
		
	}
	_pid = fork();
	if (_pid == -1){
		closePipes("fail");
	}
	else if (_pid == 0){ //child process
		childProcess();
		exit(EXIT_FAILURE); //if it reaches here execve failed
	} else { //parent process
		closePipes("parent");
		cgi_pid_to_kill = _pid;
		alarm(3);
		giveBodyToChild();
		return _responsePipe[0]; //read-end of the pipe so it can be added to epoll
	}
	return -1;
}

pid_t Cgi::getPid(){
	return _pid;
}
