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
	responseString.insert(responseString.end(), _body.begin(), _body.end()); //_body is binary data.
	// std::cout << "our response: " << responseString << std::endl;
	// std::vector<char> responseBuffer(responseString.begin(), responseString.end());
	return responseString;
}

void HttpResponse::executeResponse(HttpRequest& request, Client& client)
{
	if (request.getMethod() == "GET")
		executeGet(request, client);
	if (request.getMethod() == "POST")
		executePost(request, client);
	if (request.getMethod() == "DELETE")
		executeDelete(request, client);
	
	if(getStatusCode() == 200)
		populateHeaders(request);
	else
		populateErrorHeaders();
	return;
}

std::string HttpResponse::setErrorText(){
	if (getStatusCode() == 404){
		setText("Not Found");
		return ("404 Not Found");
	}
	else if (getStatusCode() == 413){
		setText("Payload Too Large");
		return ("413 Payload Too Large");
	}
	else if (getStatusCode() == 405){
		setText("Method Not Allowed");
		return ("Method Not Allowed");
	}
	else if (getStatusCode() == 403){
		setText("Forbidden");
		return ("403 Forbidden");
	}
	else {
		setText("Internal Server Error");
		return ("500 Internal Server Error");
	}
}

void HttpResponse::populateErrorHeaders()
{
	std::string htmlError = "<html><head><title>" + setErrorText() + "</title></head>"
    "<body><h1>404 Not Found</h1></body></html>";
	addHeader("Content-Type", "text/html");
	addHeader("Content-Length", std::to_string(htmlError.length()));
	setBody(std::vector<char>(htmlError.begin(), htmlError.end()));
}


void HttpResponse::populateHeaders(HttpRequest& request)
{
	setHttpVersion(request.getHttpVersion());
	std::cout << request.getHttpVersion() << std::endl;
	setText("OK");
	findContentType();
	addHeader("Content-Length", std::to_string(_bodyLen));
}

void HttpResponse::findContentType()
{
	size_t index = _path.find_last_of('.');
	std::string extention = _path.substr(index + 1);
	if(extention == "css")
		addHeader("Content-Type", "text/css");
	else if(extention == "html")
		addHeader("Content-Type", "text/html");
	else if(extention == "js")
		addHeader("Content-Type", "application/javascript");
	else if(extention == "jpg" || extention == "jpeg")
		addHeader("Content-Type", "image/jpeg");
	else if(extention == "png")
		addHeader("Content-Type", "image/png");
	else if (extention == "gif")
		addHeader("Content-Type", "image/gif");
	else if (extention == "ico")
		addHeader("Content-Type", "image/x-icon");
	// else
	// 	throw std::runtime_error("findcontentype temp exception"); //remove
		
}
std::string HttpResponse::createCompleteResponse()
{
	std::string response = _httpVersion;
	std::string bodyString(_body.begin(), _body.end());
	response +=  " " + std::to_string(_statusCode) + " " + _text + "\r\n";
	for (auto& iterator : _headers){
		response += iterator.first + ": " + iterator.second + "\r\n"; 
	}
	response += "\r\n" + bodyString;
	return response;
}


void HttpResponse::executeGet(HttpRequest& request, Client& client)
{
	std::string uri = request.getUri();
	std::string index = request.extractLocationVariable(client, "_index");
	if (index.empty()){
		std::cerr << "index is not found " << std::endl;
		return ;
	}
	std::cout << "indexxxxxxxxxxxx " << index << std::endl;
	if(uri == "/"){
		uri = index;
	}
	std::string fullPath;
	if (!_root.empty() && _root.back() == '/' && !uri.empty() && uri.front() == '/')
		fullPath = _root + uri.substr(1); // avoid double slash
	else if (!_root.empty() && _root.back() != '/' && !uri.empty() && uri.front() != '/')
		fullPath = _root + "/" + uri;
	else
		fullPath = _root + uri;
	_path = fullPath;
	std::cout << fullPath << std::endl;
	createBodyVector();
}

void HttpResponse::executePost(HttpRequest& request, Client& client)
{
    std::string uploadPath = request.extractLocationVariable(client, "_uploadPath");
    if (uploadPath.empty()) {
        setStatusCode(405);
        return;
    }
    
    const std::vector<char>& body = request.getBody();
    std::string fileName = "uploaded_file_" + std::to_string(time(NULL));
    std::string fullPath = _root + "/" + uploadPath + "/" + fileName;
    
    std::ofstream file(fullPath, std::ios::binary);
    if (!file.is_open()) {
        setStatusCode(500);
        return;
    }
    
    file.write(body.data(), body.size());
    file.close();
    
    setStatusCode(201);
    setText("Created");
    addHeader("Content-Type", "text/plain");
    std::string responseBody = "File uploaded successfully: " + fileName;
    setBody(std::vector<char>(responseBody.begin(), responseBody.end()));
}


void HttpResponse::executeDelete(HttpRequest& request, Client& client)
{
    std::string uploadPath = request.extractLocationVariable(client, "_uploadPath");
    if (uploadPath.empty()) {
        setStatusCode(405);
        return;
    }
    
    std::string uri = request.getUri();
    size_t lastSlash = uri.find_last_of('/');
    if (lastSlash == std::string::npos || lastSlash == uri.length() - 1) {
        setStatusCode(400);
        return;
    }
    
    std::string fileName = uri.substr(lastSlash + 1);
    if (fileName.empty()) {
        setStatusCode(400);
        return;
    }
    
    std::string fullPath = _root + "/" + uploadPath + "/" + fileName;
    
    std::ifstream checkFile(fullPath);
    if (!checkFile.good()) {
        setStatusCode(404);
        return;
    }
    checkFile.close();
    
    if (std::remove(fullPath.c_str()) != 0) {
        setStatusCode(500);
        return;
    }
    
    setStatusCode(200);
    setText("OK");
    addHeader("Content-Type", "text/plain");
    std::string responseBody = "File deleted successfully: " + fileName;
    setBody(std::vector<char>(responseBody.begin(), responseBody.end()));
}



const std::string& HttpResponse::getRoot() const
{
	return _root;
}

void HttpResponse::createBodyVector()
{
	std::ifstream body(_path.c_str(), std::ios::binary); //std::ios::binary reads the file as it is raw bytes.
	std::string content;
	std::cout << "we are here ---------------" << std::endl;
	if(!body.is_open())
	{
		content = "404 Not Found";
		setStatusCode(404);
		addHeader("Content-Type", "text/plain");
		populateErrorHeaders();
		return ;
	}
	std::stringstream file;
	file << body.rdbuf();
	content = file.str();
	setBody(std::vector<char>(content.begin(), content.end()));
	setStatusCode(200);
}

void HttpResponse::handleResponse(Client& client){
	HttpRequest parsedRequest; //change name ?
	parsedRequest.parser(client);
	HttpResponse testResponse;
	std::cout << "-----------------" << std::endl;
	testResponse.executeResponse(parsedRequest, client);

	client.setResponse(testResponse.createCompleteResponse());
	// std::cout << testResponse.createCompleteResponse() << std::endl;
}