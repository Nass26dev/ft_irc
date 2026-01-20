#pragma once

#include <vector>
#include <iostream>
#include <poll.h>

class Client;
//class Topic;
extern bool server_running;
class Server
{
    private:
        int _server_fd;
        std::string _server_name;
        std::vector<pollfd> _fds;
        std::vector<Client> _clients;
        //std::vector<Topic> _topics;
        int _port;
        std::string _password;
    public:
        Server(int port,std::string password);
        ~Server();
        void init();
        void addPollAndClient(int client_fd);
        void listening();
        void disconnectClient(int client_fd);
        void handleCommand(Client *client,std::string line);
        void handleNick(Client *client,std::vector<std::string> args);
        void handleUsername(Client *client,std::vector<std::string> args);
        bool handlePassword(std::vector<std::string> args);
        void privateMessage(Client *client, std::vector<std::string> args);
        int  getFdByClientName(std::string clientName);
        void stop();
        Client *getClientByFd(int fd);
        
};


