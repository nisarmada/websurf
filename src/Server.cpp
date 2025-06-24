#include "../includes/Constants.hpp"
#include "../includes/Parser.hpp"
#include "../includes/server.hpp"
#include "../includes/Client.hpp"

WebServer::WebServer() : _events(MAX_EVENTS){}

int WebServer::setNonBlocking(int fd) {
	int flags = fcntl(fd, F_GETFL, 0);

	if (flags == -1) {
		perror("fcntl GETFL failed");
		exit (EXIT_FAILURE);
	}

	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
		perror("fcntl SETFL failed:(");
		exit (EXIT_FAILURE);
	}
	return (flags);
}

int WebServer::setupListenerSocket(int port) {
	_listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_listenSocket == -1) {
		perror ("socket creation failed");
		exit(EXIT_FAILURE);
	}

	setNonBlocking(_listenSocket);
	sockaddr_in serverAddress;
	std::memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(port);

	if (bind(_listenSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) != 0) {
		perror ("bind failed");
		return -1;
	}

	if (listen(_listenSocket, BACKLOG) < 0) {
		perror("listen failed");
		close (_listenSocket);
		return -2;
	}

	std::cout << "We are listening people !" << std::endl;
	return 0;
}

WebServer::~WebServer() {}

void WebServer::initializeServer() {
	_epollFd = epoll_create1(0); // this fd monitors other fds for any pending activity
	if (_epollFd == -1) {
		throw std::runtime_error("epoll create failed");
	}
	if (setupListenerSocket(8080) == -1) {
		throw std::runtime_error("setupListenerSocket failed");
	}
	struct epoll_event event;
	event.events = EPOLLIN;
	event.data.fd = _listenSocket;
	epoll_ctl(_epollFd, EPOLL_CTL_ADD, _listenSocket, &event); //this tells _epollFd to add _listenSocket to its monitored fds
}

void WebServer::startListening(int num_events){
	for (int i = 0; i < num_events; i++) {
		int currentFd = _events[i].data.fd;
		uint32_t eventFlags = _events[i].events;
		if (eventFlags & EPOLLIN) {
			if (currentFd == _listenSocket) {
				acceptClientConnection();
			}
		}
	}
}

void WebServer::acceptClientConnection(){
	sockaddr_in clientAdress;
	socklen_t clientAdressLen = sizeof(clientAdress);
	int clientSocketFd = accept(_listenSocket, reinterpret_cast<sockaddr*>(&clientAdress), &clientAdressLen);
	if (clientSocketFd == -1) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			return;
		}
		else {
			perror("accept failed");
			return;
		}
	}
	else {
			if (setNonBlocking(clientSocketFd) == -1){
				throw std::runtime_error("set non blocking for client socket failed");
			}
			Client clientInstance;
			_clients.insert(std::make_pair(clientSocketFd, clientInstance));
			struct epoll_event clientEvent;
			clientEvent.events = EPOLLIN | EPOLLOUT | EPOLLET;
			clientEvent.data.fd = clientSocketFd;
			epoll_ctl(_epollFd, EPOLL_CTL_ADD, clientSocketFd, &clientEvent);
			std::cout << "Client connected @!!! fuck yes" << std::endl;
	}
}

int WebServer::run() {
	initializeServer();
	while (true) {
		int num_events = epoll_wait(_epollFd, _events.data(), MAX_EVENTS, -1);
		startListening(num_events);
	}
	return 0;
}
