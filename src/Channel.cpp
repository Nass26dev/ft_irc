#include "../includes/Channel.hpp"


void Channel::setChannelFd(int fdChannel)
{
    _fdChannel = fdChannel;
}

void Channel::setNameChannel(std::string nameChannel)
{
    _nameChannel = nameChannel;
}
void Channel::setTopic(std::string topic,Client *client)
{
    if(_topicRestriction == false)
        _topic = topic;
    else
    {
        if(isOperator(client))
            _topic = topic;
    }
    return;
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
void Channel::setTopicRestriction(bool active)
{
    if(active == true)
        _topicRestriction = true;
    else
        _topicRestriction = false;
}
void Channel::setInviteOnly(bool active)
{
    if(active == true)
        _inviteOnly = true;
    else
        _inviteOnly = false;
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

bool Channel::getPasswordBool() 
{
    return _passwordBool;
}

bool Channel::checkPassword(std::string password) 
{
 
    if (!_passwordBool) 
        return true;
    return (_passwordChannel == password);
}

void Channel::setPasswordChannel(std::string passwordChannel) 
{
    _passwordChannel = passwordChannel;
    _passwordBool = !passwordChannel.empty();
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
void Channel::addClient(Client *client,Channel *channel) 
{
    if(_inviteOnly == false)
    {
        _clients.push_back(client);
        std::string msg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + "localhost" + " JOIN " + channel->getNameChannel() + "\r\n";
        channel->broadcastMessage(msg, -1);
        return;
    }
    else
    {
        for (size_t i = 0; i < _inviteList.size(); i++) 
        {
            if (_inviteList[i]->getFd() == client->getFd())
            {

                _clients.push_back(client);
                 std::string msg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + "localhost" + " JOIN " + channel->getNameChannel() + "\r\n";
                channel->broadcastMessage(msg, -1);
            }
        }
    }
    return;
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