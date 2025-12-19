#include "server.hpp"
#include "error.hpp"

Server::Server(const std::string &port, const std::string &password) : _port(port), _password(password) {
    /* AF_INET = IPv4 protocol && SOCK_STREAM = TCP socket */
    this->_serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    /* INADDR_ANY = accept connections on any IP */
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(atoi(port.c_str()));
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(this->_serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
        throw Error::SocketBind();
    if (listen(this->_serverSocket, 5) < 0)
        throw Error::SocketListen();
}

Server::~Server() {
    close(this->_serverSocket);
}

void Server::run() {
    
}

