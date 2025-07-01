#include "../includes/HttpResponse.hpp"

HttpResponse::HttpResponse() {}

HttpResponse::~HttpResponse() {}

void HttpResponse::setStatusCode(const int status){
	_statusCode = status;
}

void HttpResponse::setHttpVersion(const std::string& version){
	_httpVersion = version;
}

void HttpResponse::setText(const std::string& text){
	_text = text;
}

void HttpResponse::addHeader(const std::string& key, const std::string& value) {
	_headers[key] = value; //we might need to check if the key already exists
}

void HttpResponse::setBody(const std::vector<char>& body){
	_body = body;
	_bodyLen = body.size();
}

std::string& HttpResponse::getHttpVersion() {
	return _httpVersion;
}

int HttpResponse::getStatusCode() {
	return _statusCode;
}

std::string HttpResponse::responseToString(){
	std::string responseString = _httpVersion;

	responseString.push_back(' ');
	responseString += std::to_string(_statusCode);
	responseString.push_back(' ');
	responseString += _text;
	responseString += "\r\n";
	for (const auto& headerPair : _headers){
		responseString += headerPair.first;
		responseString += ": ";
		responseString += headerPair.second;
		responseString += "\r\n";
	}
	responseString += "\r\n";
	responseString.insert(responseString.end(), _body.begin(), _body.end());
	// std::cout << "our response: " << responseString << std::endl;
	// std::vector<char> responseBuffer(responseString.begin(), responseString.end());
	return responseString;
}