#include <iostream>
#include "error.hpp"
#include "server.hpp"

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "Error: Usage : ./ircserv <port> <password>\n";
        return 1;
    }
    try {
        std::string port(argv[1]);
        std::string password(argv[2]);

        Error::checkArgsError(port, password);

        Server server(port, password);

        server.run();
    }
    catch (Error::PortError& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    catch (Error::PasswordError& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
