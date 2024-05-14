#include "Command.hpp"

Ping::Ping(IRC_Server& srv) : Command(srv) {}

Ping::~Ping() {}

// syntax: PING <server1> [<server2>]

void    Ping::execute(Client& client, std::vector<std::string> args)
{
    if (args.empty())
    {
        client.reply(ERR_NEEDMOREPARAMS(client.getNICK(), "PING"));
        return;
    }
    
    client.sendMsg(RPL_PING(client.getPrefix(), args[0]));
}
