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

bool Client::hasLine()
{
    for(size_t i = 0; i < _buffer.size() ; i++)
    {
        if(_buffer[i] == '\r' || _buffer[i] == '\n')
            return (0);
    }
    return 1;
}
std::string Client::extractLine()
{
    
}