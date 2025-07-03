#include "../includes/HttpRequest.hpp"

HttpRequest::HttpRequest(){}

HttpRequest::~HttpRequest(){}

void HttpRequest::setMethod(const std::string& method) {
	_method = method;
}

void HttpRequest::setUri(const std::string& uri){
	_uri = uri;
}

void HttpRequest::setHttpVersion(const std::string& httpVersion){
	_httpVersion = httpVersion;
}

void HttpRequest::addHeader(const std::string& key, const std::string& value){ 
	// we might need to check if it already exists
	_headers[key] = value;
}

void HttpRequest::setError(int errorCode){
	isError = errorCode;
}

void HttpRequest::parseRequestLine(const std::string& line){
	std::string method, uri, version;
	size_t firstSpace = line.find(" ");

	method = line.substr(0, firstSpace);
	size_t secondSpace = line.find(" ", firstSpace + 1);
	uri = line.substr(firstSpace + 1, secondSpace - firstSpace);
	version = line.substr(secondSpace + 1, line.npos - secondSpace);
	setMethod(method);
	setUri(uri);
	setHttpVersion(version);
}

void HttpRequest::parseHostLine(const std::string& line){
	size_t hostLen = line.find(":");
	std::string hostName = line.substr(0, hostLen);
	size_t headerLen = line.find("\r\n");
	std::string header = line.substr(hostLen + 2, headerLen - hostLen);
	addHeader(hostName, header);
}

void HttpRequest::parser(const std::vector<char>& requestBuffer){ //handler that controls the parsing
	std::string rawRequest(requestBuffer.begin(), requestBuffer.end()); //we turn the buffer into a string from a vector
	size_t requestLineLen = rawRequest.find("\r\n"); //finds the end of the first line 
	std::string requestLine = rawRequest.substr(0, requestLineLen);
	HttpRequest request;
	parseRequestLine(requestLine); //this is always the first line
	size_t currentPosition = requestLineLen + 2;
	size_t headerEnd = rawRequest.find("\r\n\r\n");
	size_t endPosition = rawRequest.find("\r\n", currentPosition);
	while (currentPosition < headerEnd){ //splits line by line and adds the headers to a hashmap
		std::string currentHeaderLine = rawRequest.substr(currentPosition, endPosition - currentPosition);
		parseHostLine(currentHeaderLine);
		currentPosition = endPosition + 2;
		endPosition = rawRequest.find("\r\n", currentPosition);
	}
	std::cout << rawRequest << std::endl;
}

int HttpRequest::checkMethod(){
	if (_method != "GET" && _method != "POST" && _method != "DELETE")
		return -1;
	return 0;
}

// int HttpRequest::contentLengthCheck(Client& client){
// 	const ServerBlock* serverBlock = client.getServerBlock();

// 	//should happen in parsing.
// }

// void HttpRequest::checkRequest(Client& client){
// 	if (checkMethod() == -1){
// 		setError(501);
// 	}
// 	//we should check if _associatedBlock == nullptr
// 	if (_method == "POST")
// }