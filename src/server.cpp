#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/CmdParser.hpp"
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>
#include <iostream>
#include <cstdlib>

Server::Server(int port,std::string  password) : _port(port) ,_password(password) {}

Server::~Server() {}

Client *Server::get_client_by_fd(int fd)
{
    for(size_t i = 0; i < _clients.size();i++)
    {
        if(_clients[i].getFd() == fd)
            return &_clients[i];
    }
    return NULL;
}
void Server::init()
{
    _server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_server_fd < 0)
    {
        //perror("socket");
        exit(1);
    }
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;             
    addr.sin_addr.s_addr = htonl(INADDR_ANY); 
    addr.sin_port = htons(_port);

    if (bind(_server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        //perror("bind");
        close(_server_fd);
        exit(1);
    }

    if (listen(_server_fd, SOMAXCONN) < 0)
    {
        //perror("listen");
        close(_server_fd);
        exit(1);
    }
    std::cout << "Server initialized and listening on port 6667..." << std::endl;
    pollfd p;
    p.fd = _server_fd;
    p.events = POLLIN;
    p.revents = 0;

    _fds.push_back(p);
    listening();
}

void Server::add_poll_and_client(int client_fd)
{
    for (size_t i = 0; i < _fds.size(); i++)
    {
        if (_fds[i].fd == client_fd)
            return;
    }
    struct pollfd p;

    p.fd = client_fd;
    p.events = POLLIN | POLLOUT;
    p.revents = 0;

    _fds.push_back(p);

    _clients.push_back(Client(client_fd));

}

void Server::disconnect_client(int i)
{
    std::cout << _fds[i].fd << "disconected" << std::endl;
    close(_fds[i].fd);
    _fds.erase(_fds.begin() + i);
}

void Server::handle_nick(Client *client,std::vector<std::string> args)
{
    if(args.empty())
        return ;
    
    std::string new_nick = args[0];
    client->set_nickname(new_nick);
    std::cout << "le client "<<client->getFd() << "sapelle mtn " << new_nick<< std::endl; 

}
void Server::handle_command(Client *client,std::string line)
{
    (void)client;
    Command cmd  = Parser::parse_string(line);
    if(cmd.cmd == "NICK")
    {
        //std::cout << "insinde NICK = " << cmd.args[0] << std::endl;
        Server::handle_nick(client,cmd.args);
    }
}

void Server::listening()
{
    while (true)
    {
        poll(_fds.data(), _fds.size(), -1);

        for (size_t i = 0; i < _fds.size(); i++)
        {
            if (_fds[i].revents & POLLIN)
            {
                if (_fds[i].fd == _server_fd)
                {
                    int client_fd = accept(_server_fd, NULL, NULL);
                    add_poll_and_client(client_fd);
                }
                else
                {
                    char tmp_buffer[512];
                    memset(tmp_buffer, 0, sizeof(tmp_buffer));
                    int bytes = recv(_fds[i].fd,tmp_buffer, 511, 0);
                    if (bytes > 0)
                    {
                       Client *client = get_client_by_fd(_fds[i].fd);
                        client->appendToBuffer(tmp_buffer,bytes);
                        tmp_buffer[bytes] = '\0';

                        while(client->hasLine())
                        {
                            std::string line = client->extractLine();
                            handle_command(client, line);
                            //std::cout << line << std::endl;
                        }
                       // send(_fds[i].fd, _buffer,bytes, 0);
                    }
                    else
                    {
                            disconnect_client(i);
                            i--;
                    }
                }
            }
        }
    }

}