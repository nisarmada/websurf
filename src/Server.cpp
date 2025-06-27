#include "../includes/Constants.hpp"
#include "../includes/Parser.hpp"
#include "../includes/server.hpp"
// #include "../includes/ServerBlock.hpp"
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
	//delete this block later
	int optval = 1; // Set to 1 to enable SO_REUSEADDR
    if (setsockopt(_listenSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
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
	if (setupListenerSocket(_serverBlocks[0].getPort()) == -1) {
		throw std::runtime_error("setupListenerSocket failed");
	}
	struct epoll_event event;
	event.events = EPOLLIN;
	event.data.fd = _listenSocket;
	epoll_ctl(_epollFd, EPOLL_CTL_ADD, _listenSocket, &event); //this tells _epollFd to add _listenSocket to its monitored fds
}

void WebServer::cleanupFd(int clientFd){
	epoll_ctl(EPOLL_CTL_DEL, _epollFd, clientFd, NULL);
	_clients.erase(clientFd);
	close(clientFd);
}

void WebServer::handleRequest(const HttpRequest& request){
	(void)request;
}

void WebServer::clientRead(int clientFd){
	Client& clientToRead = _clients.at(clientFd);
	char readBuffer[BUFFER_SIZE];
	ssize_t bytesRead = recv(clientFd, readBuffer, BUFFER_SIZE, 0); 
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
		clientToRead.appendData(readBuffer, bytesRead);
		std::cout << "Bytes read from client " << clientFd << ": " << bytesRead << std::endl;
		if (clientToRead.headerIsComplete()){
			std::cout << "header is complete" << std::endl;
			HttpRequest parsedRequest = HttpRequestParser::parser(clientToRead.getRequestBuffer());
			// std::string httpResponseText = "HTTP/1.1 200 OK\r\nContent-Length: 5\r\n\r\nHello";
			HttpResponse testResponse;
			testResponse.setStatusCode(200);
			testResponse.setHttpVersion("Http/1.1");
			testResponse.setText("OK");
			testResponse.addHeader("Content-Type", "text/plain");
			testResponse.addHeader("Server", "MyAwesomeWebserv");
			std::string bodyContent = "This is a test response!";
			std::vector<char> bodyVector(bodyContent.begin(), bodyContent.end());
			testResponse.addHeader("Content-Length", std::to_string(bodyVector.size()));
			testResponse.setBody(bodyVector);
			testResponse.responseToBuffer();
			// clientToRead.setResponse(httpResponseText);
			clientWrite(clientFd);
		}
		//we might need to include the request inside the client object
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

void WebServer::startListening(int num_events){
	for (int i = 0; i < num_events; i++) {
		int currentFd = _events[i].data.fd;
		uint32_t eventFlags = _events[i].events;
		if (eventFlags & EPOLLIN) {
			if (currentFd == _listenSocket) {
				acceptClientConnection();
			}
			else { // if it's not a listen socket then it is a client
				clientRead(currentFd); //we may need to use try-catch here in case the fd doesn't exist in our client map
			}
		}
		else if (eventFlags & EPOLLOUT){
			clientWrite(currentFd);
		}
	}
}

void WebServer::acceptClientConnection(){
	sockaddr_in clientAdress;
	socklen_t clientAdressLen = sizeof(clientAdress);
	int clientSocketFd = accept(_listenSocket, reinterpret_cast<sockaddr*>(&clientAdress), &clientAdressLen);
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
			Client clientInstance(clientSocketFd);
			_clients.insert(std::make_pair(clientSocketFd, clientInstance));
			struct epoll_event clientEvent;
			clientEvent.events = EPOLLIN | EPOLLOUT | EPOLLET;
			clientEvent.data.fd = clientSocketFd;
			epoll_ctl(_epollFd, EPOLL_CTL_ADD, clientSocketFd, &clientEvent);
			// std::cout << "Client connected @!!! fuck yes" << std::endl;
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
		std::cout << "=== Server Block " << i << " ===" << std::endl;
		std::cout << "Server name:      " << _serverBlocks[i].getServerName() << std::endl;
		std::cout << "Port number:      " << _serverBlocks[i].getPort() << std::endl;
		std::cout << "Max body size:    " << _serverBlocks[i].getClientBodySize() << std::endl;

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





