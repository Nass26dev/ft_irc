#include "error.hpp"

void Error::checkPortError(const std::string &port) {
    if (port.empty())
        throw Error::PortIsEmpty();
    if (port.find_first_not_of("0123456789") != std::string::npos)
        throw Error::PortIsNotDigit();
}

void Error::checkPasswordError(const std::string &password) {
    if (password.empty())
        throw Error::PasswordIsEmpty();
}

void Error::checkArgsError(const std::string &port, const std::string &password) {
    checkPortError(port);
    checkPasswordError(password);
}

const char *Error::PortIsEmpty::what() const throw() {return ("Error: Port is empty");}
const char *Error::PortIsNotDigit::what() const throw() {return ("Error: Port contains non-digit characters");}
const char *Error::PasswordIsEmpty::what() const throw() {return "Error: Password is empty";}
const char *Error::SocketBind::what() const throw() {return "Error: Impossible to bind the socket";}
const char *Error::SocketListen::what() const throw() {return "Error: Impossible to listen the socket";}