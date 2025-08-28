#include "../includes/Cgi.hpp"

Cgi::Cgi(HttpRequest& request) : _request(request) {
	_requestPipe[0] = -1;
	_requestPipe[1] = -1;
	_responsePipe[0] = -1;
	_responsePipe[1] = -1;
}

Cgi::Cgi(HttpRequest& request, const std::string cgiPath, const std::string cgiPass,\
	 const std::string serverPort) : _request(request), \
_cgiPath(cgiPath), _cgiPass(cgiPass), _gatewayInterface("CGI/1.1"), _serverPort(serverPort) {
	_requestPipe[0] = -1;
	_requestPipe[1] = -1;
	_responsePipe[0] = -1;
	_responsePipe[1] = -1;
}

Cgi::~Cgi() {}

void Cgi::populateEnvironmentVariables(){
	_requestMethod = _request.getMethod();
	if (_requestMethod == "GET"){
		_contentLength = "0";
	} else {
		_contentLength = _request.getHeader("Content-Length");
	}
	_contentType = _request.getHeader("Content-Type");
	_scriptFileName = _cgiPath;
	
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

