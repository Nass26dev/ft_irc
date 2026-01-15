#include "../includes/Client.hpp"
#include "../includes/Server.hpp"
#include <cctype>
#include <stdlib.h>
#include <csignal>

bool server_running = true;


void signalHandler(int signum) 
{
    (void)signum; 
    std::cout << "\nSignal reçu, arrêt du serveur..." << std::endl;
    server_running = false;
}
int checker_entry(std::string port, std::string password)
{
    for (size_t i = 0; i < password.length(); i++)
    {
        if (std::isspace(static_cast<unsigned char>(password[i])))
        {
            std::cout << "Password must not contain whitespace" << std::endl;
            return 1;
        }
    }
    for (size_t i = 0; i < port.length(); i++)
    {
        if (!isdigit(port[i]))
        {
            std::cout << "Port must be numeric" << std::endl;
            return 1;
        }
    }

    long port_int = atol(port.c_str());

    if (port_int < 1024 || port_int > 65535)
    {
        std::cout << "Port must be between 1024 and 65535" << std::endl;
        return 1;
    }

    return 0;
}



int main(int argc,char **argv)
{
    std::signal(SIGINT, signalHandler);
    if (argc != 3)
    {
        std::cout << "Usage: ./ircserver <port> <password>" << std::endl;
        return 1;
    }

    if(checker_entry(argv[1],argv[2]) == 1)
    {
        std::cout << "CHECK ENTRY" << std::endl;
        return 1;
    }
    
   Server a(atoi(argv[1]),argv[2]);
    
   a.init();
}