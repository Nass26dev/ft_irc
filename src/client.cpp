#include "../includes/Client.hpp"



Client::Client(int fd) : _fd(fd)
{
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

void Client::set_nickname(std::string new_nickname)
{
    _nickname = new_nickname;
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
