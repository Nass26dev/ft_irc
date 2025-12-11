#include "server.hpp"

Server::Server(const std::string &port, const std::string &password) : _port(port), _password(password) {
    /* AF_INET = IPv4 && SOCK_STREAM = TCP */
    this->_listenSocketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->_listenSocketFd < 0)
        throw Error::SocketCreation();
    
    if (fcntl(this->_listenSocketFd, F_SETFL, O_NONBLOCK))
        throw Error::SocketFcntl();

    sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(atoi(_port.c_str()));

    if (bind(this->_listenSocketFd, (struct sockaddr *)&serv_addr, sizeof(serv_addr) < 0))
        throw Error::SocketBind();

    if (listen(this->_listenSocketFd, 10) < 0)
        throw Error::SocketListen();

    pollfd pfd;
    pfd.fd = this->_listenSocketFd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    this->_poll_fds.push_back(pfd);
}

Server::~Server() {
    close(_listenSocketFd);
}

void Server::run() {

}
