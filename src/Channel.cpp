#include "../includes/Channel.hpp"
#include "../includes/Macro.hpp"
#include <climits>

void Channel::setChannelFd(int fdChannel)
{
    _fdChannel = fdChannel;
}

void Channel::setNameChannel(std::string nameChannel)
{
    _nameChannel = nameChannel;
}

int Channel::setTopic(std::string topic,Client *client)
{
    if(_topicRestriction == false)
    {
        _topic = topic;
        return 0;
    }
    else if(isOperator(client))
    {
        _topic = topic;
        return 0;
    }
    return 1;
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
: _fdChannel(-1),             
  _nameChannel(nameChannel),
  _passwordBool(false),     
  _inviteOnly(false),
  _topicRestriction(false),  
  _userLimit(INT_MAX),      
  _usersOnline(0)             
{
}

Channel::~Channel()
{
}

int Channel::getUserOnline()
{
    return (int)_clients.size();
}
void Channel::setUserOnline(bool flag)
{
    if(flag)
        _usersOnline++;
    else
        _usersOnline--;
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
                std::cerr << "Failed to send message to FD: " << currentFd << std::endl;
                continue; 
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
bool Channel::isClientInChannel(Client *client)
{
    for (size_t i = 0; i < _clients.size(); i++)
    {
        if (_clients[i] == client)
            return true;
    }
    return false;
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
int Channel::getUserLimit()
{
    return _userLimit;
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
     for (std::vector<Client*>::iterator it = _inviteList.begin(); it != _inviteList.end(); ++it) 
    {
        if ((*it) == client) 
        {
            _inviteList.erase(it);
            break;
        }
    }
}
bool Channel::getTopicRestriction()
{
    return _topicRestriction;
}
void Channel::removeOperator(Client *client)
{
    for (std::vector<Client*>::iterator it = _operators.begin(); it != _operators.end(); ++it) 
    {
        if ((*it) == client) 
        {
            _operators.erase(it);
            break;
        }
    }
}
void Channel::addClient(Client *client, Channel *channel) 
{
    if (_userLimit != INT_MAX && _clients.size() >= (size_t)_userLimit)
    {
        std::string err = ":server " + std::string(ERR_CHANNELISFULL) + " " + client->getNickname() + " " + channel->getNameChannel() + " :Cannot join channel (+l)\r\n";
        send(client->getFd(), err.c_str(), err.length(), 0);
        return;
    }

    if (_inviteOnly == true)
    {
        bool isInvited = false;
        for (size_t i = 0; i < _inviteList.size(); i++) 
        {
            if (_inviteList[i]->getFd() == client->getFd())
            {
                isInvited = true;
                break;
            }
        }
        if (!isInvited)
        {
            std::string err = ":server " + std::string(ERR_INVITEONLYCHAN) + " " +  client->getNickname() + " " + channel->getNameChannel() + " :Cannot join channel (+i)\r\n";
            send(client->getFd(), err.c_str(), err.length(), 0);
            return; 
        }
    }
    _clients.push_back(client);
    std::string msg = ":" + client->getNickname() + "!" + client->getUsername() + "@localhost JOIN " + channel->getNameChannel() + "\r\n";
    channel->broadcastMessage(msg, -1);
}

void Channel::setUserLimit(int limit)
{
    _userLimit = limit;
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