#pragma once

#include "Parser.hpp"
#include "ServerBlock.hpp"
#include "Client.hpp"
#include "HttpRequestParsing.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <fcntl.h>
#include <exception>
#include <stdexcept>
#include <sys/epoll.h>
#include <unordered_map>

class WebServer {
	public:
		WebServer();
		~WebServer();
		int run();
		void initializeServer();
		void acceptClientConnection();
		void startListening(int num_events);
		void clientRead(int clientFd);
		void cleanupFd(int clientFd);
		int setNonBlocking(int fd);
		int setupListenerSocket(int port);
		void loadConfig(std::vector<std::vector<std::string>>& serverBlocks);
		void printServerBlocks();
		void clientWrite(int clientFd);
	private:
		int _listenSocket;
		int _epollFd;
		std::vector<epoll_event> _events;
		std::unordered_map<int, Client> _clients;
		std::vector<ServerBlock> _serverBlocks;


};