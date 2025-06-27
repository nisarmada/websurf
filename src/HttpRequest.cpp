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