#include "../includes/HttpResponse.hpp"

HttpResponse::HttpResponse() : _root("./www")
{} 

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

void HttpResponse::executeResponse(HttpRequest& request)
{
	if (request.getMethod() == "GET")
		executeGet(request);

	return;
}

void HttpResponse::executeGet(HttpRequest& request)
{
	std::string uri = request.getUri();
	std::cout << "first uri get: " << uri << std::endl;
	if(uri == "/")
		uri = "/index.html";
	std::cout << "uri: " << uri << std::endl;
	std::string fullPath = _root + uri;
	std::cout << fullPath << std::endl;
	createBodyVector(request, fullPath);
}

const std::string& HttpResponse::getRoot() const
{
	return _root;
}

void HttpResponse::createBodyVector(HttpRequest& request, std::string& path)
{
	std::cout << "in createBodyVector" << std::endl;
	std::ifstream body(path.c_str(), std::ios::binary); //std::ios::binary reads the file as it is raw bytes.
	std::string content;
	if(!body.is_open())
	{
		std::cout << "in file not found" << std::endl;

		content = "404 Not Found";
		setStatusCode(404);
		addHeader("Content-Type", "text/plain");
		setBody(std::vector<char>(content.begin(), content.end()));
		throw std::runtime_error("Change this later createbodyvector");
	}
	std::stringstream file;
	file << body.rdbuf();
	content = file.str();
	std::cout << " sssssssssss " << content << std::endl; 
	(void)request;

}

