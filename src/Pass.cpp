#include "Command.hpp"

Pass::Pass(IRC_Server& srv, bool auth) : Command(srv, auth) {}

Pass::~Pass() {}

// syntax: PASS <password>

void    Pass::execute(Client& client, std::vector<std::string> args)
{
    if (args.empty())
    {
        client.reply(ERR_NEEDMOREPARAMS(client.getNICK(), "PASS"));
        return;
    }

    if (client.isRegistered())
    {
        client.reply(ERR_ALREADYREGISTERED(client.getNICK()));
        return;
    }

    if (_srv.getPASS() != args[0].substr(args[0][0] == ':' ? 1 : 0))
    {
        client.reply(ERR_PASSWDMISMATCH(client.getNICK()));
        return;
    }

    client.setPASS(args[0]);
}
