#pragma once

#include "Client.hpp"
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
	private:
		int _listenSocket;
		int _epollFd;
		std::unordered_map<int, Client> _clients;

		int setNonBlocking(int fd);
		int setupListenerSocket(int port);
};