#ifndef ERROR_HPP
#define ERROR_HPP

#include <iostream>
#include <string>

class Error {
    private:
        static void checkPortError(const std::string &port);
        static void checkPasswordError(const std::string &password);
    public:
        static void checkArgsError(const std::string &port, const std::string &password);
        
        class PortError : public std::exception {};
        class PasswordError : public std::exception {};
        class ServerError : public std::exception {};

        class PortIsEmpty : public PortError {
            const char *what() const throw();
        };
        class PortIsNotDigit : public PortError {
            const char *what() const throw();
        };
        class PasswordIsEmpty : public PasswordError {
            const char *what() const throw();
        };
        class SocketCreation : public ServerError {
            const char *what() const throw();
        };
        class SocketBind : public ServerError {
            const char *what() const throw();
        };
        class SocketListen : public ServerError {
            const char *what() const throw();
        };
};

#endif
