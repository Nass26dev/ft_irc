#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/CmdParser.hpp"
#include "../includes/Macro.hpp"
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <climits>

Server::Server(int port,std::string  password) : _server_name("IRC") ,_port(port),_password(password)  {}

Server::~Server() {}

void Server::stop() 
{
    for (size_t i = 0; i < _fds.size(); i++) 
        close(_fds[i].fd);
    _fds.clear();
  
    for (size_t i = 0; i < _clients.size(); i++)
        delete _clients[i]; 
    _clients.clear(); 

    for (size_t i = 0; i < _channels.size(); i++)
        delete _channels[i]; 
    _channels.clear();
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
        exit(EXIT_FAILURE);
    if (bind(_server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        close(_server_fd);
        exit(EXIT_FAILURE);
    }
    if (listen(_server_fd, SOMAXCONN) < 0)
    {
        close(_server_fd);
        exit(EXIT_FAILURE);
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

    std::cout << client->getIsRegistered() << std::endl;
    _clients.push_back(client);

}

void Server::disconnectClient(int i) 
{ 
    int fd = _fds[i].fd; close(fd); 
    _fds.erase(_fds.begin() + i);
    for(std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) 
    { 
        if ((*it)->getFd() == fd) 
        {
            for(size_t j = 0; j < _channels.size(); j++)
            {
                _channels[j]->removeClient(*it);
            }
            delete *it; 
            _clients.erase(it); 
            break; 
        } 
    }
    return; 
}

void Server::handleNick(Client *client,std::vector<std::string> args)
{
    if(args.empty())
        return ;
    std::string oldNick = client->getNickname();
    std::string new_nick = args[0];
    client->setNickname(new_nick);

    std::string rpl = ":" + oldNick + "!" + client->getUsername() + "@" + "localhost" + " NICK " + new_nick + "\r\n";
    send(client->getFd(), rpl.c_str(), rpl.length(), 0);
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
        return true;
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
Channel *Server::findChannel(std::string nameChannel)
{
    Channel* channel = NULL;

    for (size_t i = 0; i < _channels.size(); i++) 
    {
        if (_channels[i]->getNameChannel() == nameChannel) 
        {
            channel = _channels[i];
            break;
        }
    }
    return channel;
}
Client *Server::findClient(std::string nameClient)
{
    for(size_t i = 0;i < _clients.size(); i++)
    {
        if(_clients[i]->getNickname() == nameClient)
            return _clients[i];
    }
    return NULL;
}

void Server::handleCommand(Client *client,std::string line)
{
    std::cout << "Before parsing = " << line << std::endl;
    Command cmd  = Parser::parse_string(line);

    if (cmd.cmd == "bot" && client->getStepFlag() == 3)
    {
        if (cmd.args.size() < 1 || cmd.args[0][0] != '#')
        {
            std::string err = ":server NOTICE " + client->getNickname() + " :Usage: BOT #channel\r\n";
            send(client->getFd(), err.c_str(), err.length(), 0);
            return;
        }
        
        std::string channelName = cmd.args[0];
        Channel *channel = findChannel(channelName);
        
        if (!channel)
        {
            std::string err = ":server NOTICE " + client->getNickname() + " :Channel " + channelName + " does not exist\r\n";
            send(client->getFd(), err.c_str(), err.length(), 0);
            return;
        }        
        if (!channel->findClientInChannel(client->getNickname()))
        {
            std::string err = ":server NOTICE " + client->getNickname() + " :You are not in channel " + channelName + "\r\n";
            send(client->getFd(), err.c_str(), err.length(), 0);
            return;
        }
        
        srand(time(NULL));
        bool action = rand() % 2;
        
        if (action == true)
        {
            channel->addOperator(client);
            std::string opMsg = ":BOT MODE " + channel->getNameChannel() + " +o " + client->getNickname() + "\r\n";
            channel->broadcastMessage(opMsg, -1);
            std::string notice = ":BOT NOTICE " + client->getNickname() + " :You won! You are now an operator on " + channelName + "\r\n";
            send(client->getFd(), notice.c_str(), notice.length(), 0);
        }
        else
        {
            std::string kickMsg = ":BOT KICK " + channel->getNameChannel() + " " + client->getNickname() + " :You lost the game!\r\n";
            channel->broadcastMessage(kickMsg, -1);
            channel->removeClient(client);
            std::string notice = ":BOT NOTICE " + client->getNickname() + " :You lost! You have been kicked from " + channelName + "\r\n";
            send(client->getFd(), notice.c_str(), notice.length(), 0);
        }
    }
    else if(cmd.cmd == "NICK" && (client->getStepFlag() == 1 || client->getStepFlag() == 3))
    {
        if(client->getIsAuthenticated() == false)
            return;
        Server::handleNick(client,cmd.args);
        if(client->getStepFlag() == 1)
            client->setStepFlag();
    }
    else if(cmd.cmd == "PASS" && client->getStepFlag() == 0)
    {
        if(cmd.args.size() == 0)
        {
            return;
        }
       if(Server::handlePassword(cmd.args) == false)
       {
           std::cerr << "Wrong Password" <<std::endl; 
           return;
       }
        client->setIsAuthenticated();
        client->setStepFlag();
    }
    else if(cmd.cmd == "USER" && client->getStepFlag() == 2)
    {
        Server::handleUsername(client,cmd.args);
        client->setStepFlag();
    }
    else if (cmd.cmd == "PRIVMSG" && client->getStepFlag() == 3)
        Server::privateMessage(client,cmd.args);
    else if (cmd.cmd == "JOIN" && client->getStepFlag() == 3)
    {
        if(cmd.args[0][0] != '#')
        {
            std::string err = ":server " + std::string(ERR_NOSUCHCHANNEL) + client->getNickname() + " " + cmd.args[0] + " :No such channel\r\n";
            send(client->getFd(), err.c_str(), err.length(), 0);
            return;
        }
        Channel *channel = findChannel(cmd.args[0]);
        std::string providedPass = (cmd.args.size() > 1) ? cmd.args[1] : "";
        if (channel == NULL) 
        {
            channel = new Channel(cmd.args[0]);
            _channels.push_back(channel);
            channel->addOperator(client);
        }
        if (channel->getPasswordBool())
        {
            if (!channel->checkPassword(providedPass))
            {
                std::string err = ":server " + std::string(ERR_BADCHANNELKEY)+ " " + client->getNickname() + " " + channel->getNameChannel() + " :Cannot join channel (+k)\r\n";
                send(client->getFd(), err.c_str(), err.length(), 0);
                return;
            }
        }
        channel->addClient(client,channel);
    }
    else if (cmd.cmd == "KICK" && client->getStepFlag() == 3)
    {
        if (cmd.args.size() < 2 )
        {
            std::string err461 = ":" + _server_name + " " +  ERR_NEEDMOREPARAMS+ " " + client->getNickname() + " KICK :Not enough parameters\r\n";
            send(client->getFd(), err461.c_str(), err461.length(), 0);
            return;
        }
        std::string channelName = cmd.args[0];
        std::string targetNick = cmd.args[1];
        if(cmd.args[1][0] == '#')
        {
            if (cmd.args.size() < 3)
            {
                std::string err461 = ":" + _server_name + " " + ERR_NEEDMOREPARAMS + " " + client->getNickname() + " KICK :Not enough parameters\r\n";
                send(client->getFd(), err461.c_str(), err461.length(), 0);
                return;
            }
            channelName = cmd.args[1];
            targetNick = cmd.args[2];
        }
        if(cmd.args.size() >= 3 && cmd.args[2][0] == '#')
        {
            if (cmd.args.size() < 4)
            {
                std::string err461 = ":" + _server_name + " " + ERR_NEEDMOREPARAMS + " " + client->getNickname() + " KICK :Not enough parameters\r\n";
                send(client->getFd(), err461.c_str(), err461.length(), 0);
                return;
            }
            channelName = cmd.args[2];
            targetNick = cmd.args[3];
        }
        Channel *channel = findChannel(channelName);
        if (!channel)
        {
            std::string err403 = ":" + _server_name + " " + ERR_NOSUCHCHANNEL+ " " + client->getNickname() + " " + channelName + " :No such channel\r\n";
            send(client->getFd(), err403.c_str(), err403.length(), 0);
            return;
        }
        if (!channel->isClientInChannel(client))
        {
            std::string err442 = ":" + _server_name + " " + ERR_NOTONCHANNEL + " " + client->getNickname() + " " + channelName + " :You're not on that channel\r\n";
            send(client->getFd(), err442.c_str(), err442.length(), 0);
            return;
        }
        if (!channel->isOperator(client))
        {
            std::string err482 = ":" + _server_name + " " + std::string(ERR_CHANOPRIVSNEEDED) + " " + client->getNickname() + " " + channelName + " :You're not channel operator\r\n";
            send(client->getFd(), err482.c_str(), err482.length(), 0);
            std::string err = ": NOTICE " + channel->getNameChannel() + " :You're not channel operator\r\n";
            send(client->getFd(), err.c_str(), err.length(), 0);
            return;
        }
        Client *target = channel->findClientInChannel(targetNick);
        if (!target) 
        {
            std::string err441 = ":" + _server_name + " " + std::string(ERR_USERNOTINCHANNEL) + " " + client->getNickname() + " " + targetNick + " " + channel->getNameChannel() + " :They aren't on that channel\r\n";
            send(client->getFd(), err441.c_str(), err441.length(), 0);
            std::string notice = ":" + _server_name + " NOTICE " + channel->getNameChannel() + " :Target " + targetNick + " is not on this channel\r\n";
            send(client->getFd(), notice.c_str(), notice.length(), 0);
            return;
        }
        std::string reason;
        if (cmd.args[1][0] == '#')
            reason = (cmd.args.size() > 3) ? cmd.args[3] : "Kicked by operator";
        else if (cmd.args.size() >= 3 && cmd.args[2][0] == '#')
            reason = (cmd.args.size() > 4) ? cmd.args[4] : "Kicked by operator";
        else
            reason = (cmd.args.size() > 2) ? cmd.args[2] : "Kicked by operator";
        std::string kickMsg = ":" + client->getNickname() + " KICK " + channel->getNameChannel() + " " + targetNick + " :" + reason + "\r\n";
        channel->broadcastMessage(kickMsg, -1);
        channel->removeClient(target);
    }
    else if (cmd.cmd == "INVITE" && client->getStepFlag() == 3)
    {
        if (cmd.args.size() < 2) return; 
        std::string targetNick = cmd.args[0]; 
        std::string channelName = cmd.args[1]; 
        Channel *channel = findChannel(channelName);
        
        if (!channel || !channel->isOperator(client))
            return;
        Client *target = findClient(targetNick); 
        if (target == NULL) 
            return;
        std::string inviteMsg = ":" + client->getNickname() + " INVITE " + targetNick + " :" + channel->getNameChannel() + "\r\n";
        send(target->getFd(), inviteMsg.c_str(), inviteMsg.length(), 0);

        std::string rpl = ":server " + std::string(RPL_INVITING) + " " + client->getNickname() + " " + targetNick + " " + channel->getNameChannel() + "\r\n";
        send(client->getFd(), rpl.c_str(), rpl.length(), 0);

        channel->addToInviteList(target);
    }
    else if(cmd.cmd == "TOPIC" && client->getStepFlag() == 3)
    {
        std::string target = cmd.args[0];
        Channel *channel = NULL;

        if (target[0] == '#' && cmd.args.size() == 1) 
        {
            channel = findChannel(target);
            if(!channel)
                return;
            if (channel->getTopic().empty()) 
            {
                std::string msg = ":server " + std::string(RPL_NOTOPIC) + client->getNickname() + " " + channel->getNameChannel() + " :No topic is set\r\n";
                send(client->getFd(), msg.c_str(), msg.length(), 0);
            } 
            else if(channel->getTopicRestriction())
            {
                std::string msg = ":server " + std::string(RPL_TOPIC) + client->getNickname() + " " + channel->getNameChannel() + " :" + channel->getTopic() + "\r\n";
                send(client->getFd(), msg.c_str(), msg.length(), 0);
            }
        }
        else if(target[0] == '#' && cmd.args.size() == 2)
        {
            channel = findChannel(target);
            if(!channel)
                return;
            if(channel->setTopic(cmd.args[1],client))
                return;
            std::string msg = ":" + client->getNickname() + " TOPIC " + channel->getNameChannel() + " :" + channel->getTopic() + "\r\n";
            channel->broadcastMessage(msg, -1);
        }
    }
    else if (cmd.cmd == "MODE" && client->getStepFlag() == 3)
    {
        if (cmd.args.size() < 2) return;

        Channel *channel = findChannel(cmd.args[0]);
        if (!channel || !channel->isOperator(client)) return;

        std::string modes = cmd.args[1]; 
        bool active = (modes[0] == '+');
        bool desactive = (modes[0] == '-');
        char flag = modes[1];

        if (flag == 'i') 
        {
            channel->setInviteOnly(active);
            std::string sign = active ? "+" : "-";
            std::string msg = ":" + client->getNickname() + "!" + client->getUsername() + "@" + "localhost" 
                + " MODE " + channel->getNameChannel() + " " + sign + "i\r\n";
            channel->broadcastMessage(msg, -1);
            return;
        }
        else if(flag == 'o')
        {
            if(cmd.args.size() != 3)
            {
                std::string msg = ": NOTICE " + channel->getNameChannel() + " :Mode +o:Not good format /mode +o <User> invalid.\r\n";
                send(client->getFd(), msg.c_str(), msg.length(), 0);
                return;
            }
            Client *target  = findClient(cmd.args[2]);
            if(!target)
            {
                std::string msg = ": NOTICE " + channel->getNameChannel() + " :Mode +o:Dont find client invalid.\r\n";
                send(client->getFd(), msg.c_str(), msg.length(), 0);
                return;
            }
            if(channel->isOperator(client) && active)
            {
                if(!target)
                    return;
                channel->addOperator(target);
                std::string msg = ":" + client->getNickname() + "!" + client->getNickname() + "@" + "localhost" 
                + " MODE " + channel->getNameChannel() + " +o " + target->getNickname() + "\r\n";
                channel->broadcastMessage(msg,-1);
            }
            else if(channel->isOperator(client) && channel->isOperator(target) && !active)
            {
                channel->removeOperator(target);
                std::string msg = ":" + client->getNickname() + "!" + client->getNickname() + "@" + "localhost" 
                + " MODE " + channel->getNameChannel() + " -o " + target->getNickname() + "\r\n";
                channel->broadcastMessage(msg,-1);
            }
        }
        else if(flag == 'l')
        {
            bool isValid = true;
            if(cmd.args.size() != 3 && active)
            {
                isValid = false;
                std::string msg = ": NOTICE " + channel->getNameChannel() + " :Mode +l: Number is missing or invalid.\r\n";
                send(client->getFd(), msg.c_str(), msg.length(), 0);
                return;
            }
            else if(channel->isOperator(client) && desactive)
            {
                channel->setUserLimit(INT_MAX);
                std::string msg = ":" + client->getNickname() + " MODE " + channel->getNameChannel() + " -l\r\n";
                channel->broadcastMessage(msg,-1);
                return;
            }
            else
            {
                for(size_t i = 0; i < cmd.args[2].length(); i++) 
                {
                    if(!isdigit(cmd.args[2][i]))
                    {
                        isValid = false;
                        break;
                    }
                }
            }
            if(!isValid && !desactive)
            {
                std::string msg = ": NOTICE " + channel->getNameChannel() + " :Mode +l: Number is missing or invalid.\r\n";
                send(client->getFd(), msg.c_str(), msg.length(), 0);
                return;
            }
            if(channel->isOperator(client) && active)
            {
                channel->setUserLimit(atoi(cmd.args[2].c_str()));
                std::string msg = ":" + client->getNickname() + "!" + channel->getNameChannel() + " MODE " + channel->getNameChannel() + " +l " + cmd.args[2] + "\r\n";
                channel->broadcastMessage(msg, -1);
                return;
            }
            
        } 
        else if (flag == 't') 
        {
            if (active) {

                channel->setTopicRestriction(active);
                std::string msg = ":" + client->getNickname() + "!" + channel->getNameChannel() + " MODE " + channel->getNameChannel() + " +t\r\n";
                channel->broadcastMessage(msg, -1);
            }
            else
            {
                channel->setTopicRestriction(false);
                std::string msg = ":" + client->getNickname() + "!" + channel->getNameChannel() + " MODE " + channel->getNameChannel() + " -t\r\n";
                channel->broadcastMessage(msg, -1);
            }
            return;
        }
        else if (flag == 'k') 
        {
            if (active && cmd.args.size() == 3)
            {
                channel->setPasswordChannel(cmd.args[2]);
                std::string msg = ":" + client->getNickname() + " MODE " + channel->getNameChannel() + " " + modes;
                msg += " " + cmd.args[2] + "\r\n";
                channel->broadcastMessage(msg, -1);
            }
            else
            {
                channel->setPasswordChannel("");
                std::string msg = ":" + client->getNickname() + " MODE " + channel->getNameChannel() + " -k\r\n";
                channel->broadcastMessage(msg, -1);
                return;
            }      
        }
    }
    else
    {
        return;
    }
    if (!client->getIsRegistered() && client->getIsAuthenticated() 
        && !client->getNickname().empty() && !client->getUsername().empty()) 
    {
        client->setIsRegistered();
        std::string welcome = ":" + _server_name + " " + RPL_WELCOME + " " + client->getNickname() 
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
                    if(bytes == -1)
                        stop();
                    tmp_buffer[bytes] = '\0';
                    if (bytes > 0)
                    {
                       Client *client = getClientByFd(_fds[i].fd);
                        client->appendToBuffer(tmp_buffer,bytes);
                        
                        while(client->hasLine())
                        {
                            std::string line = client->extractLine();
                            handleCommand(client, line);
                        }
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
