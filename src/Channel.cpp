#include "../includes/Channel.hpp"


void Channel::setChannelFd(int fdChannel)
{
    _fdChannel = fdChannel;
}

void Channel::setNameChannel(std::string nameChannel)
{
    _nameChannel = nameChannel;
}
void Channel::setTopic(std::string topic)
{
    _topic = topic;
}

std::string Channel::getTopic()
{
    return _topic;
}
std::string Channel::getNameChannel()
{
    return _nameChannel;
}

Channel::Channel(std::string nameChannel) 
: _nameChannel(nameChannel)
{
}

Channel::~Channel()
{
}

void Channel::broadcastMessage(std::string msg, int excludeFd) 
{
    for (size_t i = 0; i < _clients.size(); i++) 
    {
        int currentFd = _clients[i]->getFd();

        if (currentFd != excludeFd) 
        {
            if (send(currentFd, msg.c_str(), msg.length(), 0) == -1) 
            {
                std::cerr << "Error sending to FD: " << currentFd << std::endl;
            }
        }
    }
}

Client *Channel::findClientInChannel(std::string nameClient)
{
    for(size_t i = 0;i < _clients.size(); i++)
    {
        if(_clients[i]->getNickname() == nameClient)
            return _clients[i];
    }
    return NULL;
}

void Channel::removeClient(Client *client)
{
    for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) 
    {
        if ((*it) == client) 
        {
            _clients.erase(it);
            break;
        }
    }
    
    for (std::vector<Client*>::iterator it = _operators.begin(); it != _operators.end(); ++it) 
    {
        if ((*it) == client) 
        {
            _operators.erase(it);
            break;
        }
    }
}
void Channel::addClient(Client *client) 
{
    _clients.push_back(client);
}

void Channel::addOperator(Client *client) 
{
    _operators.push_back(client);
}

bool Channel::isOperator(Client *client) 
{
    for (size_t i = 0; i < _operators.size(); i++) 
    {
        if (_operators[i]->getFd() == client->getFd())
            return true;
    }
    return false;
}

void Channel::addToInviteList(Client *client)
{
    _inviteList.push_back(client);
}