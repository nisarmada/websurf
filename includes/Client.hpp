#pragma once

// #include "server.hpp"
#include "Parser.hpp"

class Client {
	public:
		Client();
		// Client(int fd);
		~Client();

	private:
		// int _clientFd;
		std::vector<char> requestBuffer;
};