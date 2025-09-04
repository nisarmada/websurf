#include "../includes/HttpRequest.hpp"
#include "../includes/Utils.hpp"
#include "../includes/Utils.hpp"


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

const std::string& HttpRequest::getUri() const{
	return _uri;
}

const std::string& HttpRequest::getMethod() const {
	return _method;
}
std::string& HttpRequest::getHeader(const std::string& key){
	static std::string emptyString = "";
	auto it = _headers.find(key);
	if (it != _headers.end()){
		return it->second;
	}
	return emptyString;
}

std::string HttpRequest::getHttpVersion() const {
	return _httpVersion;
}

void HttpRequest::setError(int errorCode){
	_isError = errorCode;
}

void HttpRequest::parseRequestLine(const std::string& line){
	std::string method, uri, version;
	size_t firstSpace = line.find(" ");

	method = line.substr(0, firstSpace);
	size_t secondSpace = line.find(" ", firstSpace + 1);
	uri = line.substr(firstSpace + 1, secondSpace - firstSpace - 1);
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

void HttpRequest::addBody(const char* data, size_t len){
	_body.insert(_body.end(), data, data + len);
}

void HttpRequest::parseBody(Client& client){
	const std::vector<char>& requestBuffer = client.getRequestBuffer();
	std::string rawRequest(requestBuffer.begin(), requestBuffer.end());
	size_t headerEnd = rawRequest.find("\r\n\r\n");
	size_t bodyStart = headerEnd + 4;
	if (bodyStart < rawRequest.size()){
		std::string bodyContent = rawRequest.substr(bodyStart);
		addBody(bodyContent.data(), bodyContent.length());
		_bodyFullyParsed = true;
	}else{
		_bodyFullyParsed = true;
	}
	// std::cout << "we are in parse bodyyyy" << std::endl;
}

int HttpRequest::checkChunked(){
	if (getHeader("Transfer-Encoding") != "" && getHeader("Content-Length") != ""){
		setError(400);
		throw std::runtime_error("Bad Request");
	}
	if (getHeader("Transfer-Encoding") != ""){
		_isChunked = 1;
		return 1;
	}
	return 0;
}


const std::string HttpRequest::parseExceptBody(Client& client){
	const std::vector<char>& requestBuffer = client.getRequestBuffer();
	std::string rawRequest(requestBuffer.begin(), requestBuffer.end()); //we turn the buffer into a string from a vector
	size_t requestLineLen = rawRequest.find("\r\n"); //finds the end of the first line 
	std::string requestLine = rawRequest.substr(0, requestLineLen);
	parseRequestLine(requestLine); //this is always the first line
	size_t currentPosition = requestLineLen + 2;
	size_t headerEnd = rawRequest.find("\r\n\r\n");
	size_t endPosition = rawRequest.find("\r\n", currentPosition);
	while (currentPosition < headerEnd){ //splits line by line and adds the headers to a hashmap
		std::string currentHeaderLine = rawRequest.substr(currentPosition, endPosition - currentPosition);
		parseHostLine(currentHeaderLine); //this means parseHeader
		currentPosition = endPosition + 2;
		endPosition = rawRequest.find("\r\n", currentPosition);
	}
	_bodyReadPosition = headerEnd + 4;
	_headersComplete = true;
	// std::cout << "----------------" << std::endl;
	// std::cout << rawRequest << std::endl;
	return rawRequest;
}

// void HttpRequest::parseBodyChunked(std::string& rawRequest, size_t headerEnd){
	
// }


void HttpRequest::parser(Client& client){ //handler that controls the parsing
	if (!client.headerIsComplete()) {
		std::cout << "Headers not complete yet. Waiting for more data..." << std::endl;
        return; // Return immediately, the client buffer needs more data
    }
	std::string firstHalfRequest = parseExceptBody(client);
	if (checkChunked() == 1){
		parseBodyChunked(client);
		std::cout << "afteeeeer" << std::endl;
	}else if (_method == "POST"){
		parseBody(client);
	}
	extractLocationVariable(client, "/");
	checkRequest(client);
	// std::cout << firstHalfRequest << std::endl;
}

int HttpRequest::checkMethod(Client& client){
	std::set<std::string> methods = extractMethods(client);
	if (methods.find(_method) == methods.end()){
		std::cout << "we are in check method" << std::endl;
		return -1;
	}
	if (_method != "GET" && _method != "POST" && _method != "DELETE")
		return -1;
	if (_httpVersion != "HTTP/1.1"){
		setError(400);
		// throw std::runtime_error("Bad request");
	}
	if (_uri.size() > MAX_URI_LENGTH){
		setError(414);
		// throw std::runtime_error("URI too long");
	}
	return 0;
}

void HttpRequest::contentLengthCheck(Client& client){
	const ServerBlock* serverBlock = client.getServerBlock();
	size_t maxSizeConfig = serverBlock->getBodySize();
	std::string& requestSizeString = getHeader("Content-Length");
	
	if (requestSizeString == ""){
		setError(411); // I think this is the correct error code
		throw std::runtime_error("Content Length is missing");
	}
	size_t requestSize = static_cast<size_t>(std::stoul(requestSizeString));
	if (requestSize > maxSizeConfig){
		setError(411);
		throw std::runtime_error("Payload too large");
	}
}

void HttpRequest::checkRequest(Client& client){
	if (checkMethod(client) == -1){ //we should change that to throw an exception instead of return -1
		setError(501);
		
		// throw std::runtime_error("Method not allowed");
		return;
	}
	//we should check if _associatedBlock == nullptr
	if ((_method == "POST") && !_isChunked){ //GET shouldnt be there, it's only for testing
		contentLengthCheck(client);
	}
}

const std::set<std::string> HttpRequest::extractMethods(Client& client){
	const ServerBlock* serverBlock = client.getServerBlock();
    const std::map<std::string, LocationBlock>& locations = serverBlock->getLocations();
    std::string longestMatch = findLongestMatch(_uri, locations);
	
	
	const LocationBlock& currentLocation = locations.at(longestMatch);
	std::set<std::string> methods = currentLocation.getMethods();
	return methods;
}

const std::string HttpRequest::extractLocationVariable(Client& client, std::string identifier){
    const ServerBlock* serverBlock = client.getServerBlock();
    const std::map<std::string, LocationBlock>& locations = serverBlock->getLocations();
   
	std::cout << "identifier is -----> " << identifier << std::endl;
	std::cout << "uri is -------> " << _uri << std::endl;
	std::string longestMatch = findLongestMatch(_uri, locations); //this finds the longet match so we don't throw an exception if the full path doesn't exist
	std::cout << "longest match is ---> " << longestMatch << std::endl;
	const LocationBlock& currentLocation = locations.at(longestMatch);
	std::cout << "index is  -----> " << currentLocation.getIndex() << std::endl;
    // std::cout << "uir: ---> " <<_uri << std::endl;
    if (identifier == "_path") {
        return currentLocation.getPath();
    } else if (identifier == "_root") {
        return currentLocation.getRoot();
    } else if (identifier == "_index") {
        return currentLocation.getIndex();
    } else if (identifier == "_redirectUrl") {
        return currentLocation.getRedirectUrl();
    } else if (identifier == "_uploadPath") {
        return currentLocation.getUploadPath();
    } else if (identifier == "_cgiPass") {
        return currentLocation.getCgiPass();
	}
	const std::string emptyString = "";
    return emptyString;
}

const std::vector<char>& HttpRequest::getBody() const {
    return _body;
}

int HttpRequest::getError()
{
	return _isError;
}


