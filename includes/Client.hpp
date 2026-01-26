#pragma once
#include <iostream>
#include <vector>

class Client
{
    private:
        int _fd;
        std::string _nickname;
        std::string _username;
        std::string _buffer;
        bool _isRegistered;
        bool _isAuthenticated;
        
        public:
        Client(int fd);
        void appendToBuffer(const char* data, size_t len);
        bool hasLine() const;
        std::string extractLine();
        
        void setNickname(std::string newNickname);
        void setUsername(std::string newUsername);
        
        void setIsRegistered();
        void setIsAuthenticated();
        
        int getFd() const;
        bool getIsRegistered();
        bool getIsAuthenticated();
        std::string getNickname();
        std::string getUsername();
        ~Client();
};


