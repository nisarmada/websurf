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
	std::cout << "INSIDE CGI URIII---> " << uri << std::endl;
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
		envp.push_back(const_cast<char*>(iterator.c_str()));
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
		exit(EXIT_FAILURE);
	}
}

void Cgi::childProcess(){
	close(_requestPipe[1]); //write end of request pipe
	close(_responsePipe[0]); //read end of response pipe
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

void Cgi::executeCgi() {
	if (pipe(_requestPipe) == -1){
		std::cerr << "ERROR WITH REQUEST PIPE" << std::endl; //first stage errors
	}
	if (pipe(_responsePipe) == -1){
		std::cerr << "ERROR WITH RESPONSE PIPE" << std::endl;
		close(_requestPipe[0]);
		close(_requestPipe[1]);
	}
	_pid = fork();
	if (_pid == 0){ //child process
		childProcess();
	} else { //parent process
		std::cout << "PARENT PROCESSSS" << std::endl;
	}

}

