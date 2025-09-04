#include "../includes/Client.hpp"
#include <string_view>

Client::Client() : _bytesSent(0) {}

Client::Client(int fd) : _clientFd(fd), _bytesSent(0), _associatedServerBlock(nullptr) {}

Client::~Client() {}

int Client::getFd() const {
	return _clientFd;
}

void Client::appendData(const char* data, size_t len){
	_requestBuffer.insert(_requestBuffer.end(), data, data + len);
}

void Client::clearRequestBuffer(){
	_requestBuffer.clear();
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

void Client::connectClientToServerBlock(std::vector<ServerBlock>& serverBlocks){
	struct sockaddr_in serverAddr;
	socklen_t serverAddrLen = sizeof(serverAddr);

	if (getsockname(_clientFd, reinterpret_cast<sockaddr*>(&serverAddr), &serverAddrLen) == -1){
		perror("error in association between client and server block");
		close(_clientFd);
		return ;
	}
	int connectedPort = ntohs(serverAddr.sin_port); 
	for (auto& iterator : serverBlocks){
		if (iterator.getPort() == connectedPort){
			_associatedServerBlock = &iterator;
			return ;
		}
	}

}

const ServerBlock* Client::getServerBlock(){ 
	return _associatedServerBlock;
}

// void Client::handleResponse(){
// 	HttpRequest parsedRequest; //change name ?
// 	parsedRequest.parser(*this);
// 	HttpResponse testResponse;
// 	std::cout << "-----------------" << std::endl;
// 	testResponse.executeResponse(parsedRequest);

// 	this->setResponse(testResponse.createCompleteResponse());
// 	// std::cout << testResponse.createCompleteResponse() << std::endl;
// }
