#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>

class Server {
    private:
        std::string _port;
        std::string _password;
    public:
        Server(const std::string &port, const std::string &password);
        ~Server();

        void run();
};

#endif
