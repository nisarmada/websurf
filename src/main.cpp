#include "../includes/Constants.hpp"
#include "../includes/Parser.hpp"
#include "../includes/server.hpp"
//TCP is Transmission control protocol
//check out epoll

int main (int argc, char** argv)
{
    if(argc != 2)
    {
        std::cerr << "Usage: ./webserv configfile" << std::endl;
        return 1;
    }
    parsing(argv[1]);
	
	Server webSurv;
	webSurv.run();
    return 0;
}






//removing newlines double spaces etc. cleanup before the next step.















// int main (void)
// {
//     int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
//     if(socket_fd == -1)
//     {
//         std::cerr << "Error: Socket() failed" << std::endl;
//         return 1;
//     }
    
//     sockaddr_in address{}; //creates struct initialised everything to 0
//     address.sin_family = AF_INET;
//     address.sin_addr.s_addr = INADDR_ANY; //listens to all network interfaces
//     address.sin_port = htons(PORT); //changes the way numbers are stored (computer and network store differnt.)
    
//     if(bind(socket_fd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == -1) //binds the address information to the socket otherwise it has no information and does nothing
//     {
//         std::cerr << "Error: bind() failed" << std::endl;
//         return 1;
//     }
//     if(listen(socket_fd, BACKLOG) == -1) //it won't send or rceive data directly it will wait for clients to connect
//     {
//         std::cerr << "Error: listen() failed" << std::endl;
//         return 1;
//     }
//     std::cout << "Server is listening on a port: " << PORT << std::endl;
    
//     sockaddr_in client_address{};
//     socklen_t client_len = sizeof(client_address); //socklen_t is unsigned int
    
//     int client_socket = accept(socket_fd, reinterpret_cast<sockaddr*>(&client_address), &client_len); //creates a socket for the client to connect to so the main socket can remain open.
    
//     char buffer[1024] = {0};
//     int bytes_received = read(client_socket, buffer, sizeof(buffer) - 1); //reads from connected client stores in buffer and returns how many bytes received

//     std::cout << "Received " << bytes_received << " bytes\n";

//     std::string response =
//     "HTTP/1.1 200 OK\r\n"
//     "Content-Type: text/html\r\n"
//     "Content-Length: 13\r\n"
//     "\r\n"
//     "Hello, world!"; //HTTP hard coded to send hello world.

//     send(client_socket, response.c_str(), response.size(), 0); //sending the response, send expects c style. 0 stands for default behaviour.

//     close(client_socket);
//     close(socket_fd);
//     return 0;
// }