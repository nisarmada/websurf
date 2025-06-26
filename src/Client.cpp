#include "../includes/Client.hpp"

Client::Client() : _bytesSent(0) {}

Client::Client(int fd) : _clientFd(fd), _bytesSent(0) {}

Client::~Client() {}

int Client::getFd() const {
	return _clientFd;
}

void Client::appendData(const char* data, size_t len){
	_requestBuffer.insert(_requestBuffer.end(), data, data + len);
}

bool Client::headerIsComplete() const {
	std::string_view bufferCheck(_requestBuffer.data(), _requestBuffer.size());
	return (bufferCheck.find("\r\n\r\n") != std::string_view::npos);
}

void Client::setResponse(const std::string& response){
	_responseBuffer.assign(response.begin(), response.end());
	_bytesSent = 0;
}

bool Client::hasResponseToSend() {
	return _responseBuffer.size() - _bytesSent > 0;
}

const std::vector<char>& Client::getResponseBuffer(){
	return _responseBuffer;
}

const std::vector<char>& Client::getRequestBuffer(){
	return _requestBuffer;
}

void Client::addBytesSent(ssize_t amount){
	_bytesSent += amount;
}

ssize_t Client::getBytesSent() {
	return _bytesSent;
}