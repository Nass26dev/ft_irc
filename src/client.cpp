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