#include "../includes/CmdParser.hpp"


Command::Command() : cmd("") {}

Command::~Command() {}

Command Parser::parse_string(std::string data)
{
    Command cmd;

    size_t space_pos = data.find(" ");
    if(space_pos == (size_t)-1)
        space_pos = data.find("\n");

    cmd.cmd = data.substr(0,space_pos);
    data.erase(0,space_pos + 1);
    cmd.args.push_back(data);
    return cmd;
}



