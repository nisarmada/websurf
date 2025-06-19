#pragma once

#include "Parser.hpp"
#include "ServerBlock.hpp"
#include <fcntl.h>

class Server {
	public:
		Server();
		~Server();
		int run();
		void loadConfig(std::vector<std::vector<std::string>>& serverBlocks);
		void printServerBlocks();

		private:
		int _listenSocket;
		// int _epollFd;
		std::vector<ServerBlock> _serverBlocks;
		int setNonBlocking(int fd);
		int setupListenerSocket(int port);
};