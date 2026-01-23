
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
        std::string _passwordChannel;
        bool _passwordBool;
        bool _inviteOnly;
        bool _topicRestriction;
        std::vector<Client*> _clients;
        std::vector<Client*> _operators;
        std::vector<Client*> _inviteList;
    public:

        Channel(std::string nameChannel);
        ~Channel();

        void broadcastMessage(std::string msg,int excludeFd);
        
        void setNameChannel(std::string nameChannel);
        void setChannelFd(int fdChannel);
        void setTopic(std::string topic,Client *client);

        void setInviteOnly(bool active);
        void setTopicRestriction(bool active);

        void setPasswordChannel(std::string passwordChannel);
        bool getInviteOnly();
        void addToInviteList(Client *client);
        void addClient(Client *client,Channel *channel);
        void addOperator(Client *client);
        bool isOperator(Client *client);
        bool getPasswordBool();
        bool checkPassword(std::string password);
        Client *findClientInChannel(std::string nameClient);
        std::string getNameChannel();
        void removeClient(Client *client);
        std::string getTopic();
        
};

#endif