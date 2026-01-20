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

Server::Server(int port,std::string  password) : _server_name("IRC") ,_port(port),_password(password)  {}

Server::~Server() {}

void Server::stop() 
{

    for (size_t i = 0; i < _fds.size(); i++) 
    {

        close(_fds[i].fd);
    }
    std::cout << "JE FERME LE SEVEUR !!!!!!!!!!" << std::endl;
    _fds.clear();

    std::cout << _fds.size() << std::endl;
}
Client *Server::getClientByFd(int fd)
{
    for(size_t i = 0; i < _clients.size();i++)
    {
        if(_clients[i]->getFd() == fd)
            return _clients[i];
    }
    return NULL;
}
void Server::init()
{
    _server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_server_fd < 0)
        exit(1);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;             
    addr.sin_addr.s_addr = htonl(INADDR_ANY); 
    addr.sin_port = htons(_port);

    int opt = 1;
    
    if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) 
    {
        exit(EXIT_FAILURE);
    }
    if (bind(_server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        close(_server_fd);
        exit(1);
    }

    if (listen(_server_fd, SOMAXCONN) < 0)
    {
        close(_server_fd);
        exit(1);
    }
    std::cout << "Server initialized and listening on port " << _port << "..." << std::endl;
    pollfd p;
    p.fd = _server_fd;
    p.events = POLLIN;
    p.revents = 0;

    _fds.push_back(p);
    listening();
}

void Server::addPollAndClient(int client_fd)
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
    
    Client *client = new Client(client_fd);

    _clients.push_back(client);

}

void Server::disconnectClient(int i)
{
    std::cout << _fds[i].fd << "disconected" << std::endl;
    close(_fds[i].fd);
    _fds.erase(_fds.begin() + i);
}

void Server::handleNick(Client *client,std::vector<std::string> args)
{
    if(args.empty())
        return ;
    
    std::string new_nick = args[0];
    client->setNickname(new_nick);
   // std::cout << "le client "<<client->getFd() << "sapelle mtn " << new_nick<< std::endl; 

}
void Server::handleUsername(Client *client,std::vector<std::string> args)
{
    if(args.empty())
        return ;
    
    std::string new_username = args[0];
    client->setUsername(new_username);
}
bool Server::handlePassword(std::vector<std::string> args)
{

    if(args[0] == _password)
    {
        std::cout << "Acces garented" << std::endl;
        return true;
    }
    else
        return false;
}

int Server::getFdByClientName(std::string clientName)
{
    for(size_t i = 0; i < _clients.size();i++)
    {
        if(_clients[i]->getNickname() == clientName)
            return _clients[i]->getFd();
    }
    std::cout << "on trouve pas le nom" << std::endl;
    return -1;
}

Channel *Server::getChannelByName(std::string nameChannel) 
{
    for (size_t i = 0; i < _channels.size(); i++) 
    {
        if (nameChannel == _channels[i]->getNameChannel()) 
            return _channels[i];
    }
    return NULL;
}

void Server::privateMessage(Client *client, std::vector<std::string> args)
{
    if (args.size() < 2) return;

    std::string target = args[0];
    std::string msg = args[1];


    if (target[0] == '#') 
    {
        Channel* channel = getChannelByName(target);
        if (channel == NULL)
            return;
        std::string fullMsg = ":" + client->getNickname() + " PRIVMSG " + target + " :" + msg + "\r\n";
        channel->broadcastMessage(fullMsg, client->getFd());
    } 
    else 
    {
        int fdTarget = getFdByClientName(target);
        if (fdTarget == -1) return;

        std::string fullMsg = ":" + client->getNickname() + " PRIVMSG " + target + " :" + msg + "\r\n";
        send(fdTarget, fullMsg.c_str(), fullMsg.length(), 0);
    }
}
void Server::handleCommand(Client *client,std::string line)
{
   std::cout << "Before parsing = " <<  line << std::endl;
    Command cmd  = Parser::parse_string(line);
    // if(!cmd.args[0].empty())
    // {
    //     std::cout << "After parseing = " << cmd.args[0] << std::endl;// "    " <<  cmd.args[1] << std::endl;
    // }
    if(cmd.cmd == "NICK")
    {
       // std::cout << "NICK = " << cmd.args[0] << std::endl;
        Server::handleNick(client,cmd.args);
    }
    else if(cmd.cmd == "PASS")
    {
        //std::cout << "PASS = " << cmd.args[0] << std::endl;
       if(Server::handlePassword(cmd.args) == false)
            throw std::runtime_error("Wrong Password");
        client->setIsAuthenticated();
    }
    else if(cmd.cmd == "USER")
    {
        //std::cout << "USER = " << cmd.args[0] << std::endl;
        Server::handleUsername(client,cmd.args);
    }
    else if (cmd.cmd == "PRIVMSG")
    {
        //std::cout << "PRIVATE MSG" << std::endl;
        Server::privateMessage(client,cmd.args);
    }
    else if (cmd.cmd == "JOIN")
    {
        Channel* channel = NULL;
        for (size_t i = 0; i < _channels.size(); i++) {
            if (_channels[i]->getNameChannel() == cmd.args[0]) {
                channel = _channels[i];
                break;
            }
        }
        if (channel == NULL) {
            channel = new Channel(cmd.args[0]);
            _channels.push_back(channel);

        }
        channel->addClient(client);
        std::string msg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + "localhost" + " JOIN " + channel->getNameChannel() + "\r\n";
        channel->broadcastMessage(msg, -1);
    }
    else
    {
        return;
    }

  //  std::cout << client->getIsRegistered()<< "     " << client->getIsAuthenticated() << std::endl;
    if (!client->getIsRegistered() && client->getIsAuthenticated() 
        && !client->getNickname().empty() && !client->getUsername().empty()) 
    {

        client->setIsRegistered();
        std::string welcome = ":" + _server_name + " 001 " + client->getNickname() 
                            + " :Welcome to the IRC Network " + client->getNickname() + "\r\n";
        
        send(client->getFd(), welcome.c_str(), welcome.length(), 0);
    }

}

void Server::listening()
{
    while (server_running)
    {
        poll(_fds.data(), _fds.size(), -1);

        for (size_t i = 0; i < _fds.size(); i++)
        {
            if (_fds[i].revents & POLLIN)
            {
                if (_fds[i].fd == _server_fd)
                {
                    int client_fd = accept(_server_fd, NULL, NULL);
                    addPollAndClient(client_fd);
                }
                else
                {
                    char tmp_buffer[512];
                    memset(tmp_buffer, 0, sizeof(tmp_buffer));
                    int bytes = recv(_fds[i].fd,tmp_buffer, 511, 0);
                    tmp_buffer[bytes] = '\0';
                    if (bytes > 0)
                    {
                       Client *client = getClientByFd(_fds[i].fd);
                        client->appendToBuffer(tmp_buffer,bytes);
                        
                        while(client->hasLine())
                        {
                            std::string line = client->extractLine();
                            try
                            {
                                handleCommand(client, line);
                            }
                            catch(std::exception &e)
                            {
                                disconnectClient(i);
                                i--;
                                std::cout << e.what() << std::endl;
                            }
                            //std::cout << line << std::endl;
                        }
                       // send(_fds[i].fd, _buffer,bytes, 0);
                    }
                    else
                    {
                            disconnectClient(i);
                            i--;
                    }
                }
            }
        }
    }
    stop();
}