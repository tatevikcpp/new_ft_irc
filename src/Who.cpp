#include "Command.hpp"

Who::Who(IRC_Server& srv) : Command(srv) {}

Who::~Who() {}

void    Who::execute(Client& client, std::vector<std::string> args)
{
    (void)args;
    (void)client;
}