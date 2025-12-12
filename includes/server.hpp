#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include "error.hpp"
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <map>

class Server {
    private:
        int _listenSocketFd;
        std::string _port;
        std::string _password;
        std::vector<pollfd> _pollf_fds;
    
        void createListenSocket();
        void switchToNonBlocking();
        void bindSocket();
        void listenSocket();
        void addToPoll();
    public:
        Server(const std::string &port, const std::string &password);
        ~Server();

        void run();
};

#endif
