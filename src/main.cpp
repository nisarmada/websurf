#include "../includes/server.hpp"
#include "../includes/Constants.hpp"

int main() {
	int firstSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (!firstSocket) {
		perror("first socket creation failed");
	}
	sockaddr_in serverAddress;

	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(PORT);

	if (bind(firstSocket, (sockaddr*)(&serverAddress), sizeof(serverAddress)) != 0) {
		perror("error with bind");
		close(firstSocket);
	}
	listen(firstSocket, BACKLOG);
	sockaddr_in clientAddress;
	memset(&clientAddress, 0, sizeof(clientAddress));
	socklen_t clientAddressLength = sizeof(clientAddress);
	int clientSocket = accept(firstSocket, (sockaddr *)(&clientAddress), &clientAddressLength);
	if (!clientSocket) {
		perror("failed to accept client socket");
	}

	const char* message = "Hello Stan!";
	ssize_t bytesSent = send(clientSocket, message, strlen(message), 0);
	if (bytesSent == -1) {
		perror("failed ot send messgae");
	}else{
			std::cout << "Sent message: " << message << std::endl;
		}
		close(firstSocket);
		close(clientSocket);
}
