#include "Command.hpp"

Notice::Notice(IRC_Server& srv) : Command(srv) {}

Notice::~Notice() {}

void    Notice::execute(Client& client, std::vector<std::string> args)
{
    (void)args;
    (void)client;
}