#pragma once

// #include "server.hpp"
#include "Parser.hpp"
#include <algorithm>
#include <string_view>

class Client {
	public:
		Client();
		Client(int fd);
		~Client();
		int getFd() const;
		void appendData(const char* data, size_t len);
		bool headerIsComplete() const;
	private:
		int _clientFd;
		std::vector<char> _requestBuffer;
};