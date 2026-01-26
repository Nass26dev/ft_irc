#include "../includes/CmdParser.hpp"
#include <sstream>

Command::Command() : cmd("") {}

Command::~Command() {}
Command Parser::parse_string(std::string data)
{
    Command cmd;

    size_t last = data.find_last_not_of("\r\n");
    if (last != std::string::npos)
        data = data.substr(0, last + 1);
    else if (data == "\n" || data == "\r\n")
        return cmd;

    std::string trailing;
    size_t colon_pos = data.find(" :");
    if (colon_pos != std::string::npos) {
        trailing = data.substr(colon_pos + 2); 
        data = data.substr(0, colon_pos);
    }

    std::stringstream ss(data);
    std::string word;
    
    if (ss >> word)
        cmd.cmd = word;

    while (ss >> word)
        cmd.args.push_back(word);

    if (colon_pos != std::string::npos)
        cmd.args.push_back(trailing);

    return cmd;
}
