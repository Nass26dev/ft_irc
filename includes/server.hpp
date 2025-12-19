#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstdlib>
#include <poll.h>
#include <vector>

class Client;

class Server {
    private:
        int _serverSocket;
        std::string _port;
        std::string _password;
        std::vector<Client> _clients;

    public:
        Server(const std::string &port, const std::string &password);
        ~Server();

        void run();
};

#endif
