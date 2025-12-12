#include "server.hpp"

Server::Server(const std::string &port, const std::string &password) : _port(port), _password(password) {
    this->createListenSocket();
    this->switchToNonBlocking();
    this->bindSocket();
    this->listenSocket();
    this->addToPoll();
}

Server::~Server() {
    close(this->_listenSocketFd);
}

void Server::createListenSocket() {
    this->_listenSocketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->_listenSocketFd < 0)
        throw  Error::SocketCreation();
}

void Server::switchToNonBlocking() {
    fcntl(this->_listenSocketFd, F_SETFL, O_NONBLOCK);
}

void Server::bindSocket() {
    sockaddr_in serv_addr;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(atoi(_port.c_str()));

    if (bind(this->_listenSocketFd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        throw Error::SocketBind();
}

void Server::listenSocket() {
    if (listen(this->_listenSocketFd, 10) < 0)
        throw Error::SocketListen();
}

void Server::addToPoll() {
    pollfd pfd;
    pfd.fd = this->_listenSocketFd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    _pollf_fds.push_back(pfd);
}

void Server::run() {

}
