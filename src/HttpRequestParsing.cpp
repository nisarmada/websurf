#include "../includes/HttpRequestParsing.hpp"

HttpRequestParser::HttpRequestParser() {}

HttpRequestParser::~HttpRequestParser() {}

void HttpRequestParser::parseRequestLine(const std::string& line, HttpRequest& request) {
	std::string method, uri, version;
	size_t firstSpace = line.find(" ");

	method = line.substr(0, firstSpace);
	size_t secondSpace = line.find(" ", firstSpace + 1);
	uri = line.substr(firstSpace + 1, secondSpace - firstSpace - 1);
	version = line.substr(secondSpace + 1, line.npos - secondSpace);
	request.setMethod(method);
	request.setUri(uri);
	request.setHttpVersion(version);
}

void HttpRequestParser::parseHostLine(const std::string& line, HttpRequest& request){
	size_t hostLen = line.find(":");
	std::string hostName = line.substr(0, hostLen);
	size_t headerLen = line.find("\r\n");
	std::string header = line.substr(hostLen + 2, headerLen - hostLen);
	request.addHeader(hostName, header);
}

HttpRequest HttpRequestParser::parser(const std::vector<char>& requestBuffer){ //handler that controls the parsing
	std::string rawRequest(requestBuffer.begin(), requestBuffer.end()); //we turn the buffer into a string from a vector
	size_t requestLineLen = rawRequest.find("\r\n"); //finds the end of the first line 
	std::string requestLine = rawRequest.substr(0, requestLineLen);
	HttpRequest request;
	HttpRequestParser::parseRequestLine(requestLine, request); //this is always the first line
	size_t currentPosition = requestLineLen + 2;
	size_t headerEnd = rawRequest.find("\r\n\r\n");
	size_t endPosition = rawRequest.find("\r\n", currentPosition);
	while (currentPosition < headerEnd){ //splits line by line and adds the headers to a hashmap
		std::string currentHeaderLine = rawRequest.substr(currentPosition, endPosition - currentPosition);
		parseHostLine(currentHeaderLine, request);
		currentPosition = endPosition + 2;
		endPosition = rawRequest.find("\r\n", currentPosition);
	}
	return request;
}