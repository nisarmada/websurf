#include "../includes/Constants.hpp"
#include "../includes/Parser.hpp"
#include "../includes/server.hpp"
// #include "../includes/ServerBlock.hpp"
#include "../includes/Client.hpp"

WebServer::WebServer() : _events(MAX_EVENTS){}

int WebServer::setNonBlocking(int fd) {
	int flags = fcntl(fd, F_GETFL, 0); //get this away NIKOS Illigal bussiness detected

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

	setNonBlocking(listeningSocket); //READ ABOUT THIS WHAT IT EXACTLY IS AND DOES.
	//delete this block later
	int optval = 1; // Set to 1 to enable SO_REUSEADDR
    if (setsockopt(listeningSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
        perror("setsockopt SO_REUSEADDR failed");
        // For simplicity, you can exit here as per your other error handling,
        // or choose to just log and continue if this error isn't considered fatal for your project.
        exit(EXIT_FAILURE);
    }
	// end of block to delete
	sockaddr_in serverAddress;
	std::memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(port);

	if (bind(listeningSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) != 0) { //CHECKOUT WHAT THIS DOES
		perror ("bind failed");
		return -1;
	}

	if (listen(listeningSocket, BACKLOG) < 0) { //CHECKOUT WHAT THIS DOES
		perror("listen failed");
		close (listeningSocket);
		return -2;
	}

	std::cout << "We are listening people !" << std::endl;
	return listeningSocket;
}

WebServer::~WebServer() {}

void WebServer::initializeServer() {
	_epollFd = epoll_create1(0); // this fd monitors other fds for any pending activity
	if (_epollFd == -1) {
		throw std::runtime_error("epoll create failed");
	}
	std::set<int> uniquePorts;
	for (auto& iteratingBlock : _serverBlocks){ //STAN LOOK AT THIS HOW IT WORKS
		uniquePorts.insert(iteratingBlock.getPort());
	}
	for (int port : uniquePorts){
		int currentListenFd = setupListenerSocket(port);
		if (currentListenFd == -1){
			throw std::runtime_error("setupListenerSocket failed for port " + std::to_string(port));
		}
		_listenSockets.push_back(currentListenFd);
		//CHECKOUT WHAT THIS DOES
		struct epoll_event event;
		event.events = EPOLLIN;
		event.data.fd = currentListenFd;
		//CHECK UNTIL HERE
		if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, currentListenFd, &event) == -1) { //epoll_ctl modifies something about the epoll
			std::cerr << "initialize server epoll ctl" << std::endl;
		} //this tells _epollFd to add _listenSocket to its monitored fds
	}
}

void WebServer::cleanupFd(int clientFd){
	epoll_ctl(EPOLL_CTL_DEL, _epollFd, clientFd, NULL);
	_clients.erase(clientFd);
	close(clientFd);
}

void WebServer::handleRequest(const HttpRequest& request){
	(void)request;
}

void WebServer::clientIsReadyToWriteTo(int clientFd){
	struct epoll_event event;
	event.events = EPOLLIN | EPOLLOUT; //we might want to remove EPOLLEt
	event.data.fd = clientFd;
	if (epoll_ctl(_epollFd, EPOLL_CTL_MOD, clientFd, &event) == -1){
		perror("epoll_ctl MOD EPOLLOUT failed"); // we may need to call cleanup crew here
	}
}

void WebServer::clientRead(int clientFd){
	Client& clientToRead = _clients.at(clientFd); //.at throws an error when the client fd doesnt exist in the hashmap that we made.
	char readBuffer[BUFFER_SIZE];
	ssize_t bytesRead = recv(clientFd, readBuffer, BUFFER_SIZE, 0); //recv returns how many bytes it read each itteration
	if (bytesRead == 0){ // client terminated the connection
		std::cout << "Cleanup crew called" << std::endl;
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
		clientToRead.appendData(readBuffer, bytesRead);
		std::string buffer(clientToRead.getRequestBuffer().begin(), clientToRead.getRequestBuffer().end());
		if (clientToRead.headerIsComplete())
		{
			HttpResponse::handleResponse(clientToRead);
			clientIsReadyToWriteTo(clientFd);
		}
	}
}



void WebServer::clientWrite(int clientFd){
	Client& clientToWrite = _clients.at(clientFd);

	if (!clientToWrite.hasResponseToSend()){
		return; //we might want to remove EPOLLOUT or handle differently
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
		std::cout << "We sent " << bytesSentThisRound << " bytes to client" << std::endl;
	}
	if (!clientToWrite.hasResponseToSend()){ // maybe we shouldnt close the fd immediately
		std::cout << "Full response sent to client " << clientFd << std::endl;
		cleanupFd(clientFd);
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

void WebServer::startListening(int num_events){
	for (int i = 0; i < num_events; i++) {
		int currentFd = _events[i].data.fd;
		uint32_t eventFlags = _events[i].events;
		if (eventFlags & EPOLLIN) {
			if (fdIsListeningSocket(currentFd)) { //new connection
				acceptClientConnection(currentFd);
			}
			else { // if it's not a listen socket then it is a client
				clientRead(currentFd); //we may need to use try-catch here in case the fd doesn't exist in our client map --------existing connection
			}
		}
		else if (eventFlags & EPOLLOUT){
			clientWrite(currentFd);
		}
	}
}

void WebServer::createClientAndMonitorFd(int clientSocket){
	Client clientInstance(clientSocket);
		clientInstance.connectClientToServerBlock(_serverBlocks); // we should potentially add a check in case the name is not there
		std::cout << "client with fd " << clientInstance.getFd() << " is associated to serverblock "\
				<< clientInstance.getServerBlock()->getServerName() << std::endl; 
		_clients.insert(std::make_pair(clientSocket, clientInstance));
		struct epoll_event clientEvent;
		clientEvent.events = EPOLLIN | EPOLLOUT; //I removed EPOLLET not sure if that's correct
		clientEvent.data.fd = clientSocket;
		if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, clientSocket, &clientEvent) == -1){
			std::cerr << "Epoll ctllllll" << std::endl;
		}
		std::cout << "Client connected @!!! fuck yes" << std::endl;
}

void WebServer::acceptClientConnection(int listenerFd){
	sockaddr_in clientAdress;
	socklen_t clientAdressLen = sizeof(clientAdress);
	int clientSocketFd = accept(listenerFd, reinterpret_cast<sockaddr*>(&clientAdress), &clientAdressLen);
	if (clientSocketFd == -1) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) { //this is not allowed in the subject
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
			std::cerr << "Error in serverBlock number: " << i << ". " << e.what() << std::endl;
			exit(1);
		}
		
	}
}






//remove later, this is just for test purposes.
void WebServer::printServerBlocks()
{
	for (size_t i = 0; i < _serverBlocks.size(); ++i)
	{
		// std::cout << "=== Server Block " << i << " ===" << std::endl;
		// std::cout << "Server name:      " << _serverBlocks[i].getServerName() << std::endl;
		// std::cout << "Port number:      " << _serverBlocks[i].getPort() << std::endl;
		// std::cout << "Max body size:    " << _serverBlocks[i].getClientBodySize() << std::endl;

		// ✅ Check and print 404 error page if it exists
		if (_serverBlocks[i].hasErrorPage(404))
		{
			std::cout << "Error page for 404: " << _serverBlocks[i].getErrorPagePath(404) << std::endl;
		}
		if (_serverBlocks[i].hasErrorPage(500))
		{
			std::cout << "Error page for 500: " << _serverBlocks[i].getErrorPagePath(500) << std::endl;
		}

		// Print location blocks
		const std::map<std::string, LocationBlock>& locations = _serverBlocks[i].getLocations();
		if (locations.empty())
			std::cout << "No locations defined." << std::endl;
		else
		{
			std::cout << "Locations:" << std::endl;
			for (std::map<std::string, LocationBlock>::const_iterator it = locations.begin(); it != locations.end(); ++it)
			{
				std::cout << "  --Path--:       " << it->first << std::endl;
				std::cout << "    Root:         " << it->second.getRoot() << std::endl;
				std::cout << "    Index:        " << it->second.getIndex() << std::endl;

				const std::set<std::string>& methods = it->second.getMethods();
				if (!methods.empty())
				{
					std::cout << "    Methods:      ";
					for (std::set<std::string>::const_iterator mit = methods.begin(); mit != methods.end(); ++mit)
						std::cout << *mit << " ";
					std::cout << std::endl;
				}

				std::cout << "    Autoindex:    " << (it->second.getAutoindex() ? "on" : "off") << std::endl;

				if (!it->second.getRedirectUrl().empty())
					std::cout << "    Redirect:     " << it->second.getRedirectUrl() << std::endl;

				if (!it->second.getUploadPath().empty())
					std::cout << "    Upload path:  " << it->second.getUploadPath() << std::endl;
			}
		}

		std::cout << std::endl;
	}
}






