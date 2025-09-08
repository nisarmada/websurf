#include "../includes/HttpResponse.hpp"
#include "../includes/Utils.hpp"


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
	std::cout << "method is ------->" << request.getMethod() << std::endl;
	if (request.getMethod() == "GET" && checkAllowedMethods(client, "GET"))
		executeGet(request, client);
	if (request.getMethod() == "POST" && checkAllowedMethods(client, "POST"))
		executePost(request, client);
	if (request.getMethod() == "DELETE" && checkAllowedMethods(client, "DELETE"))
		executeDelete(request, client);
	

	if(getStatusCode() == 200) //check how it goes with the getError is not checked now.
		populateHeaders(request);
	
	else if(_body.empty())
	{
		std::cout << "WE NEED TO KNOW THIS ARE WE HERE LETS SEE" << std::endl;
		// populateErrorHeaders();
		createBodyVector(client, request);
	}
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

//change this to the correct messages.  //check if the status code is correct or not. 
void HttpResponse::populateErrorHeaders()
{
	std::string errorMessage = setErrorText();
	std::stringstream html;

	html << "<html><head><title>" << _statusCode << " " << errorMessage << "</title></head>"
		<< "<body><h1>" << _statusCode << " " << errorMessage << "</h1></body></html>";

	std::string htmlStr = html.str();

	addHeader("Content-Type", "text/html");
	addHeader("Content-Length", std::to_string(htmlStr.length()));
	setBody(std::vector<char>(htmlStr.begin(), htmlStr.end()));
}


void HttpResponse::populateHeaders(HttpRequest& request)
{
	setHttpVersion(request.getHttpVersion());
	if (_statusCode == 200){
		setText("OK");
		findContentType();

	}
	else {
		setErrorText();
	}
	std::cout << request.getHttpVersion() << std::endl;
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
	std::cout << "HTTP VERSIONNNN-->" << _httpVersion << std::endl;
	std::string bodyString(_body.begin(), _body.end());
	response +=  " " + std::to_string(_statusCode) + " " + _text + "\r\n";
	for (auto& iterator : _headers){
		response += iterator.first + ": " + iterator.second + "\r\n"; 
	}
	response += "\r\n" + bodyString;
	// std::cout << "response is --------> " << response << std::endl;
	return response;
}


void HttpResponse::executeGet(HttpRequest& request, Client& client)
{
	std::string uri = request.getUri();
	std::string index = request.extractLocationVariable(client, "_index");
	std::string fullPath;

	std::cout << "we are in executeGet " << std::endl;

	if (isDirectory(uri)){
		std::string index = request.extractLocationVariable(client, "_index");
		if (index.empty()){
			setStatusCode(404);
			populateErrorHeaders(); //check if this needs to be here
			std::cerr << "index is not found " << std::endl;
			return ;
		}
		if (uri.back() == '/'){
			uri += index;
		} else {
			uri += "/" + index;
		}
	}
	if (!_root.empty() && _root.back() == '/' && !uri.empty() && uri.front() == '/')
		fullPath = _root + uri.substr(1); // avoid double slash
	else if (!_root.empty() && _root.back() != '/' && !uri.empty() && uri.front() != '/')
		fullPath = _root + "/" + uri;
	else
		fullPath = _root + uri;
	_path = fullPath;
	std::cout << "full path-----> " << fullPath << std::endl;
	std::ifstream testFile(fullPath.c_str()); //change it
	if (!testFile.is_open()) {
		setStatusCode(404);
		// populateErrorHeaders(); //check if this needs to be here
		std::cerr << "File not found: " << fullPath << std::endl;
		// return;
	}
	createBodyVector(client, request);
}

bool HttpResponse::checkAllowedMethods(Client& client, std::string check)
{
	std::set<std::string> methods = client.getServerBlock()->getLocations().find("/")->second.getMethods();
	if (methods.find(check) == methods.end())
	{
		std::cout << "123456789098654 INNNNNNNN IF NOT FOUND" << std::endl;
		setStatusCode(405);
		return false;
	}
		std::cout << "123456789098654 innnnnnnnnnnnnnnn FOUNDDDDD" << std::endl;
	return true;
}



void HttpResponse::executePost(HttpRequest& request, Client& client)
{
	std::cout << "we are in executePost " << std::endl;
	// std::string root = request.extractLocationVariable(client, "_root");
    std::string uploadPath = request.extractLocationVariable(client, "_uploadPath");
    if (uploadPath.empty()) {
        setStatusCode(500);
        return;
    }
    
    // Get original filename from custom header
    std::string originalFilename = request.getHeader("X-Filename");
    std::string fileName;
    
    if (!originalFilename.empty()) {
        // Use the original filename with its extension
        fileName = originalFilename;
    } else {
        // Fallback to timestamp if no original filename found
        fileName = "uploaded_file_" + std::to_string(time(NULL));
    }
    
    // std::string fullPath = _root + "/" + uploadPath + "/" + fileName;
    std::string fullPath = _root + "/" + uploadPath + "/" + fileName;

	std::cout << "full path in post is --------> " << fullPath << std::endl;
	std::cout << "root POST -->" << _root << std::endl;
    std::ofstream file(fullPath, std::ios::binary);
    if (!file.is_open()) {
        setStatusCode(500);
        return;
    }
    
    const std::vector<char>& body = request.getBody();
    file.write(body.data(), body.size());
    file.close();
    
    setStatusCode(200);
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

//check for path if I have to add / if not specified or should I just say file not exist. 
void HttpResponse::createBodyVector(Client& client, HttpRequest& request)
{
	if(request.getError() != 0)	//change this logic maybe here. 
		_statusCode = request.getError(); //maybe change this logic here. 

	if(_statusCode >= 400)
	{
		handleError(client, request);
		return;
	}
	std::ifstream file(_path.c_str(), std::ios::binary); //std::ios::binary reads the file as it is raw bytes.
	std::string content;
	if(!file.is_open())
	{
		_statusCode = 404;
		handleError(client, request);
		return ;
	}
	std::stringstream buf;
	buf << file.rdbuf();
	content = buf.str();
	setBody(std::vector<char>(content.begin(), content.end()));
	setStatusCode(200);
}



void HttpResponse::handleError(Client& client, HttpRequest& request)
{
	if(client.getServerBlock()->hasErrorPage(_statusCode))
	{
		_path =   client.getServerBlock()->getErrorPagePath(_statusCode);
		std::string extension = request.extractLocationVariable(client, "_root");
		std::cout << "WRITEEEE" << extension << std::endl;
		_path = extension + _path;
		std::cout << "THIS IS THE PATH AND VERY IMPORTANT TO KNOW: " << _path << std::endl;
		std::ifstream file(_path.c_str(), std::ios::binary);
		if(!file.is_open())
		{
			std::cout << "------------------------------------- CHECK HERE" << std::endl;
			// _statusCode = 404; //should I do this or not?
			populateErrorHeaders();
			return;
		}
		std::stringstream buffer;
		buffer << file.rdbuf();
		std::string content = buffer.str();
		setBody(std::vector<char>(content.begin(), content.end()));
		addHeader("Content-Type", "text/html");
		// addHeader("Content-Length", std::to_string(content.size()));
		populateHeaders(request);
		return;
	}
	populateErrorHeaders();
}

void HttpResponse::handleResponse(Client& client){
	HttpRequest request;
	HttpResponse response;
	request.parser(client);
	const std::string cgiPass = request.extractLocationVariable(client, "_cgiPass");
	const std::string cgiRoot = request.extractLocationVariable(client, "_root");
	const std::string fullPathCgi = cgiRoot + request.getUri();
	const std::string serverPort = std::to_string(client.getServerBlock()->getPort());
	// std::cout << "CGI FULLLLLL PATH------> " << fullPathCgi << std::endl;
	// std::cout << "CGI PATH------> " << cgiRoot << std::endl;
	// std::cout << "CGI PASS------> " << cgiPass << std::endl;
	if (isCgi(cgiPass)){
		std::cout << "yessss " << std::endl;
		Cgi cgi(request, fullPathCgi, cgiPass, serverPort);
		cgi.executeCgi(response);
	}
	else{
		response.executeResponse(request, client);
	}

	client.setResponse(response.createCompleteResponse());
	// std::cout << testResponse.createCompleteResponse() << std::endl;
}
