#include "client.hpp"

Client::Client(int socket) : _socket(socket) {}

Client::~Client() {
    close(_socket);
}
