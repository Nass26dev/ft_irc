#include "../includes/Client.hpp"



Client::Client(int fd) : _fd(fd) , _isRegistered(false) , _isAuthenticated(false)
{ 
    _nickname = "";
    _username = "";
    std::cout << _fd << std::endl;
}

Client::~Client()
{
}

int Client::getFd() const
{
    return _fd;
}

void Client::appendToBuffer(const char *data,size_t len)
{
    _buffer.append(data, len);
}

void Client::setNickname(std::string newNickname)
{
    _nickname = newNickname;
}

void Client::setUsername(std::string newUsername)
{
    _username = newUsername;
}
bool Client::hasLine() const 
{

    //a voir avec hexchat car nc sans -c envoie que \n mais cest possible que hexchat envoie \r\n
    return _buffer.find('\n') != std::string::npos;
}

std::string Client::extractLine() 
{
     //a voir avec hexchat car nc sans -c envoie que \n mais cest possible que hexchat envoie \r\n
    size_t pos = _buffer.find('\n');
    std::string line = _buffer.substr(0, pos);

    if (!line.empty() && line[line.size() - 1] == '\r')
        line.erase(line.size() - 1);

    _buffer.erase(0, pos + 1);
    return line;
}

std::string Client::getNickname()
{
    return _nickname;
}
std::string Client::getUsername()
{
    return _username;
}

bool Client::getIsAuthenticated()
{
    return _isAuthenticated;
}
bool Client::getIsRegistered()
{
    return _isRegistered;
}
void Client::setIsAuthenticated()
{
    if(_isAuthenticated == true)
        _isAuthenticated = false;
    else
        _isAuthenticated = true;
}

void Client::setIsRegistered()
{
    if(_isRegistered == true)
        _isRegistered = false;
    else
       _isRegistered = true;
}
