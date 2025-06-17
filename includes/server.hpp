#pragma once

#include <fcntl.h>

class Server {
	public:
		Server();
		~Server();
		int run();
	private:
		int _listenSocket;
		// int _epollFd;

		int setNonBlocking(int fd);
		int setupListenerSocket(int port);
};