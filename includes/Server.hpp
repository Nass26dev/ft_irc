#pragma once

#include <vector>
#include <iostream>
#include <poll.h>

class Client;
//class Topic;

class Server
{
    private:
        int _server_fd;
        std::vector<pollfd> _fds;
        std::vector<Client> _clients;
        //std::vector<Topic> _topics;
        int _port;
        std::string _password;
    public:
        Server(int port,std::string password);
        ~Server();
        void init();
        void add_poll_and_client(int client_fd);
        void listening();
        void disconnect_client(int client_fd);
        void handle_command(Client *client,std::string line);
        void handle_nick(Client *client,std::vector<std::string> args);
        Client *get_client_by_fd(int fd);
        
};


