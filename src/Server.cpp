#include "../includes/Constants.hpp"
#include "../includes/Parser.hpp"
#include "../includes/server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Cgi.hpp"

pid_t cgi_pid_to_kill = -1;

void cleanupZombieChildren(int signum){
	(void)signum;
	while (waitpid(-1, NULL, WNOHANG) > 0){}
}

void sigalrm_handler(int signum){
	(void)signum;
	std::cerr << "Timeout occurred! Killing the child process " << cgi_pid_to_kill << std::endl;
	if (cgi_pid_to_kill != -1){
		kill(cgi_pid_to_kill, SIGKILL);
	}
}

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
	int listeningSocket;
	listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listeningSocket == -1) {
		perror ("socket creation failed");
		exit(EXIT_FAILURE);
	}

	setNonBlocking(listeningSocket); 
	int optval = 1; // Set to 1 to enable SO_REUSEADDR
    if (setsockopt(listeningSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
        perror("setsockopt SO_REUSEADDR failed");
        exit(EXIT_FAILURE);
    }
	sockaddr_in serverAddress;
	std::memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(port);

	if (bind(listeningSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) != 0) {
		perror ("bind failed");
		return -1;
	}

	if (listen(listeningSocket, BACKLOG) < 0) { 
		perror("listen failed");
		close (listeningSocket);
		return -2;
	}

	return listeningSocket;
}

WebServer::~WebServer() {}

void WebServer::initializeServer() {
	signal(SIGCHLD, cleanupZombieChildren);
	signal(SIGALRM, sigalrm_handler);
	_epollFd = epoll_create1(0); // this fd monitors other fds for any pending activity
	if (_epollFd == -1) {
		throw std::runtime_error("epoll create failed");
	}
	std::set<int> uniquePorts;
	for (auto& iteratingBlock : _serverBlocks){ 
		uniquePorts.insert(iteratingBlock.getPort());
	}
	for (int port : uniquePorts){
		int currentListenFd = setupListenerSocket(port);
		if (currentListenFd == -1){
			throw std::runtime_error("setupListenerSocket failed for port " + std::to_string(port));
		}
		_listenSockets.push_back(currentListenFd);
		struct epoll_event event;
		event.events = EPOLLIN;
		event.data.fd = currentListenFd;
		if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, currentListenFd, &event) == -1) { //epoll_ctl modifies something about the epoll
			std::cerr << "initialize server epoll ctl" << std::endl;
		} //this tells _epollFd to add _listenSocket to its monitored fds
	}
}

void WebServer::cleanupFd(int clientFd){
	struct epoll_event event{};
	event.data.fd = clientFd;
	epoll_ctl(EPOLL_CTL_DEL, _epollFd, clientFd, &event);
	_clients.erase(clientFd);
	close(clientFd);
}

void WebServer::handleRequest(const HttpRequest& request){
	(void)request;
}

void WebServer::clientIsReadyToWriteTo(int clientFd){
	struct epoll_event event;
	event.events = EPOLLIN | EPOLLOUT;
	event.data.fd = clientFd;
	if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, clientFd, &event) == -1){
		perror("epoll_ctl MOD EPOLLOUT failed");
	}
}

void WebServer::monitorCgiFd(int cgiReadFd, int clientFd, Cgi* cgiInstance){
	setNonBlocking(cgiReadFd);

	struct epoll_event event;
	event.events = EPOLLIN;
	event.data.fd = cgiReadFd;
	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, cgiReadFd, &event) == -1){
		delete cgiInstance;
		return ;
	}

	_cgiFdsToClientFds[cgiReadFd] = clientFd;
	_activeCgis[clientFd] = cgiInstance;
}


void WebServer::clientRead(int clientFd){
	Client& clientToRead = _clients.at(clientFd); //.at throws an error when the client fd doesnt exist in the hashmap that we made.
	char readBuffer[BUFFER_SIZE];
	ssize_t bytesRead = recv(clientFd, readBuffer, BUFFER_SIZE, 0); //recv returns how many bytes it read each itteration
	if (bytesRead == 0){ // client terminated the connection
		cleanupFd(clientFd);
	}
	else if (bytesRead < 0){
		if (errno == EAGAIN || errno == EWOULDBLOCK){
			return;
		}
		else{
			perror("recv failed");
			cleanupFd(clientFd);
		}
	}
	else{
		HttpRequest* currentRequest = nullptr;
		clientToRead.appendData(readBuffer, bytesRead);
		if (_activeRequests.count(clientFd)){
			currentRequest = _activeRequests.at(clientFd);
		}
		else{
			currentRequest = new HttpRequest();
			_activeRequests[clientFd] = currentRequest;
		}
		if (clientToRead.headerIsComplete())
		{
			currentRequest->parser(clientToRead);
			bool isComplete = false;
			if ((currentRequest->getMethod() == "POST" && currentRequest->isBodyComplete()) || currentRequest->getMethod() != "POST"){
				isComplete = true;
			}
			if (isComplete){
				HttpResponse::handleResponse(clientToRead, *this, *currentRequest);
				clientIsReadyToWriteTo(clientFd);

				delete currentRequest;
				_activeRequests.erase(clientFd);
			}
		}
	}
}

void WebServer::clientWrite(int clientFd){
	Client& clientToWrite = _clients.at(clientFd);

	if (!clientToWrite.hasResponseToSend()){
		return;
	}
	const std::vector<char>& responseBuffer = clientToWrite.getResponseBuffer();
	ssize_t bytesSent = clientToWrite.getBytesSent();
	ssize_t bytesRemaining = responseBuffer.size() - bytesSent;
	ssize_t bytesSentThisRound = send(clientFd, responseBuffer.data() + bytesSent, bytesRemaining, 0);
	if (bytesSentThisRound < 0){
		if (errno == EAGAIN || errno == EWOULDBLOCK){
			return;
		}
		else{
			perror("send failed");
			cleanupFd(clientFd);
			return ;
		}
	}
	else {
		clientToWrite.addBytesSent(bytesSentThisRound);
	}
	if (!clientToWrite.hasResponseToSend()){
		if (clientToWrite.getCloseConnection() == true)
			cleanupFd(clientFd);
		else
			clientToWrite.resetState();
	}
}

bool WebServer::fdIsListeningSocket(int fd){
	for (int socket : _listenSockets){
		if (fd == socket){
			return true;
		}
	}
	return false;
}

void WebServer::cgiWaitAndCleanup(int cgiFd, Cgi* cgi, int clientFd){
	alarm(0);
	cgi_pid_to_kill = -1;
	epoll_ctl(_epollFd, EPOLL_CTL_DEL, cgiFd, NULL);
	close (cgiFd);
	waitpid(cgi->getPid(), NULL, 0);
	_cgiFdsToClientFds.erase(cgiFd);
	_activeCgis.erase(clientFd);
	delete cgi;
}


void WebServer::cgiResponse(int cgiFd){
	int clientFd = _cgiFdsToClientFds.at(cgiFd);
	Cgi* cgi = _activeCgis.at(clientFd);
	HttpResponse httpResponse;
	cgi->parseResponse(cgi->getResponseString(), httpResponse);
	_clients.at(clientFd).setResponse(httpResponse.responseToString());

	clientIsReadyToWriteTo(clientFd);
	cgiWaitAndCleanup(cgiFd, cgi, clientFd);
}

void WebServer::readCgiData(int cgiFd){
	int clientFd = _cgiFdsToClientFds.at(cgiFd);
	Cgi* cgi = _activeCgis.at(clientFd);
	char buffer[BUFFER_SIZE];
	ssize_t bytesRead = read(cgiFd, buffer, BUFFER_SIZE);

	if (bytesRead > 0){
		cgi->getResponseString().append(buffer, bytesRead);
	}
	else {
		return ;
	}
}

void WebServer::startListening(int num_events){
	for (int i = 0; i < num_events; i++) {
		int currentFd = _events[i].data.fd;
		uint32_t eventFlags = _events[i].events;
		if (_cgiFdsToClientFds.count(currentFd)){
			if (eventFlags & (EPOLLHUP | EPOLLERR)){
				cgiResponse(currentFd);
			}
			else if (eventFlags & EPOLLIN){
				readCgiData(currentFd);
			}
		}
		else if (eventFlags & EPOLLIN) {
			if (fdIsListeningSocket(currentFd)) { //new connection
				acceptClientConnection(currentFd);
			}
			else { // if it's not a listen socket then it is a client
				clientRead(currentFd);
			}
		}
		else if (eventFlags & EPOLLOUT){
			clientWrite(currentFd);
		}
	}
}

void WebServer::createClientAndMonitorFd(int clientSocket){
	Client clientInstance(clientSocket);
		clientInstance.connectClientToServerBlock(_serverBlocks);
		_clients.insert(std::make_pair(clientSocket, clientInstance));
		struct epoll_event clientEvent;
		clientEvent.events = EPOLLIN | EPOLLOUT;
		clientEvent.data.fd = clientSocket;
		if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, clientSocket, &clientEvent) == -1){
			std::cerr << "Epoll ctl error" << std::endl;
		}
}

void WebServer::acceptClientConnection(int listenerFd){
	sockaddr_in clientAdress;
	socklen_t clientAdressLen = sizeof(clientAdress);
	int clientSocketFd = accept(listenerFd, reinterpret_cast<sockaddr*>(&clientAdress), &clientAdressLen);
	if (clientSocketFd == -1) {
			perror("accept failed");
			return;
	}
	else {
			if (setNonBlocking(clientSocketFd) == -1){
				throw std::runtime_error("set non blocking for client socket failed");
			}
			createClientAndMonitorFd(clientSocketFd);
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

void WebServer::loadConfig(std::vector<std::vector<std::string>>& serverBlocks)
{

	for(size_t i = 0; i < serverBlocks.size(); i++)
	{
		try
		{
			ServerBlock tempServerBlock = parseServerBlock(serverBlocks[i]);
			tempServerBlock.validateServerBlock();
			this->_serverBlocks.push_back(tempServerBlock);
		}
		catch(const std::exception& e)
		{
			std::cerr << "Error: " << e.what() << std::endl;
			exit(1);
		}
		
	}
}
