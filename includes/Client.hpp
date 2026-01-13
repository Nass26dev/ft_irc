#pragma once
#include <iostream>
#include <vector>

class Client
{
    private:
        int _fd;
        std::vector<std::string> _msg;
        std::string _nickname;
        std::string _username;
        std::string _buffer;
        
    public:
        Client(int fd);
        int getFd() const;
        void appendToBuffer(const char* data, size_t len);
        bool hasLine();
        std::string extractLine();
        ~Client();
};


