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

void Channel::broadcastMessage(std::string msg, int excludeFd) {
    for (size_t i = 0; i < _clients.size(); i++) {
        int currentFd = _clients[i]->getFd();

        if (currentFd != excludeFd) {
            if (send(currentFd, msg.c_str(), msg.length(), 0) == -1) {
                std::cerr << "Error sending to FD: " << currentFd << std::endl;
            }
        }
    }
}

void Channel::addClient(Client *client) {
    _clients.push_back(client);
}