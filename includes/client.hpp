#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <unistd.h>

class Client {
    private:
        int _socket;
    public:
        Client(int socket);
        ~Client();
};

#endif