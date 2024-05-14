#include "Command.hpp"

Cap::Cap(IRC_Server& srv) : Command(srv) {}

Cap::~Cap() {}

void    Cap::execute(Client& client, std::vector<std::string> args)
{
    (void)args;
    (void)client;
}