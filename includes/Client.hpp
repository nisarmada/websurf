#pragma once

// #include "server.hpp"
#include "Parser.hpp"
#include <algorithm>
#include <string_view>
#include "ServerBlock.hpp"

class Client {
	public:
		Client();
		Client(int fd);
		~Client();
		int getFd() const;
		void appendData(const char* data, size_t len);
		bool headerIsComplete() const;
		void setResponse(const std::string& response);
		bool hasResponseToSend();
		const std::vector<char>& getResponseBuffer();
		const std::vector<char>& getRequestBuffer();
		void addBytesSent(ssize_t amountOfBytes);
		ssize_t getBytesSent();
		void connectClientToServerBlock(int fd, std::vector<ServerBlock>& serverBlocks);
	private:
		int _clientFd;
		std::vector<char> _requestBuffer;
		std::vector<char> _responseBuffer;
		ssize_t _bytesSent;
		ServerBlock* _associatedServerBlock;
};