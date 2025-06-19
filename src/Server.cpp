#include "../includes/Constants.hpp"
#include "../includes/Parser.hpp"
#include "../includes/server.hpp"
// #include "../includes/ServerBlock.hpp"

Server::Server() {}

int Server::setNonBlocking(int fd) {
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

int Server::setupListenerSocket(int port) {
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

Server::~Server() {
	
}

int Server::run() {
	if (setupListenerSocket(8080) == -1) {
		std::cerr << "Error in server run" << std::endl;
		return 1;
	}

	//TODO here we have to make an a while (true) loop to process events
	return 0;
}

void Server::loadConfig(std::vector<std::vector<std::string>>& serverBlocks)
{

	for(size_t i = 0; i < serverBlocks.size(); i++)
	{
		std::vector<std::string> tokens = serverBlocks[i];
		this->_serverBlocks.push_back(parseServerBlock(serverBlocks[i]));
	}
}
void Server::printServerBlocks()
{
	for(size_t i = 0; i < _serverBlocks.size(); i++)
	{
		std::cout << "Server: " << i << std::endl;
		std::cout << "Server name: " << _serverBlocks[i].getServerName() << std::endl;
		std::cout << "Port number: " << _serverBlocks[i].getPort() << std::endl;
	}
}
