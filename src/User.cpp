#include "Command.hpp"

User::User(IRC_Server& srv, bool auth) : Command(srv, auth) {}

User::~User() {}

// stntax: USER <username> <hostname> <servername> <realname>

void    User::execute(Client& client, std::vector<std::string> args)
{
    if (client.getPASS().empty())
    {
        client.reply(ERR_NOTREGISTERED(client.getNICK()));
        return ;
    }
    if (client.isRegistered())
    {
        client.reply(ERR_ALREADYREGISTERED(client.getNICK()));
        return;
    }

    if (args.size() < 4 || args[1] != "0" || args[2] != "*")
    {
        client.reply(ERR_NEEDMOREPARAMS(client.getNICK(), "USER"));
        return;
    }

    client.setUSER(args[0], args[3]);
    client.checkForRegistered();
}
