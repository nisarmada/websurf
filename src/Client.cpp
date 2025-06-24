#include "Client.hpp"

Client::Client() {}

Client::Client(int fd) : _clientFd(fd) {}

Client::~Client() {}

int Client::getFd() const {
	return _clientFd;
}

void Client::appendData(const char* data, size_t len){
	_requestBuffer.insert(_requestBuffer.end(), data, data + len);
}

bool Client::headerIsComplete() const {
	std::string_view bufferCheck(_requestBuffer.data(), _requestBuffer.size());
	return (bufferCheck.find("\r\n\r\f") != std::string_view::npos);
}