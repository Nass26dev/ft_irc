#include <iostream>
#include "error.hpp"
#include "server.hpp"

bool launchServer(const std::string &port, const std::string &password) {
    
    try {
        Server server(port, password);
        server.run();
    }
    catch (...) {

    }
    return false;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "Error: Usage : ./ircserv <port> <password>\n";
        return 1;
    }

    std::string port(argv[1]);
    std::string password(argv[2]);
    
    try {
        Error::checkArgsError(port, password);
    }
    catch (Error::PortError& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    catch (Error::PasswordError& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    launchServer(port, password);
    
    return 0;
}
