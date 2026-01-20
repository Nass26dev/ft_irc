
#ifndef CHANNEL_HPP
#define CHANNEL_HPP
#include <iostream>
#include <vector>
#include "Client.hpp"
#include <sys/socket.h>

class Client;

class Channel
{
    private:
        int _fdChannel;
        std::string _nameChannel;
        std::string _topic;
        std::vector<Client*> _clients;
        std::vector<Client*> _operators;
    public:

        Channel(std::string nameChannel);
        ~Channel();

        void broadcastMessage(std::string msg,int excludeFd);
        
        void setNameChannel(std::string nameChannel);
        void setChannelFd(int fdChannel);
        void setTopic(std::string topic);

        void addClient(Client *client);

        std::string getNameChannel();

        std::string getTopic();
};

#endif