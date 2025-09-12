
#include "../includes/Constants.hpp"
#include "../includes/Parser.hpp"
#include "../includes/server.hpp"

//curl http://localhost:8080/

int main (int argc, char** argv)
{
    if(argc != 2)
    {
        std::cerr << "Usage: ./webserv configfile" << std::endl;
        return 1;
    }
	
    WebServer webSurv;
	parsing(argv[1], webSurv);
	try {
		webSurv.run();
	}
	catch (std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}
    return 0;
}
