#pragma once

#include <iostream>
#include <vector>

class Command
{
    public:
        Command();
        ~Command();      
        std::vector<std::string> args;
        std::string cmd;

};


class Parser
{
    public:
        static Command parse_string(std::string data);
    private:
        Parser();
};
