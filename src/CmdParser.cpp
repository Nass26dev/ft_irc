#include "../includes/CmdParser.hpp"


Command::Command() : cmd("") {}

Command::~Command() {}

Command Parser::parse_string(std::string data)
{
    Command cmd;

    size_t space_pos = data.find(" ");

    cmd.cmd = data.substr(0,space_pos);
    
    cmd.args.push_back(data);
    return cmd;
}



