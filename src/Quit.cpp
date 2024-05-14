#include "Command.hpp"

Quit::Quit(IRC_Server& srv, bool auth) : Command(srv, auth) {}

Quit::~Quit() {}

// syntax: QUIT [<message>]

void    Quit::execute(Client& client, std::vector<std::string> args)
{
    std::string reason;
    if (args.empty())
    {
        reason = client.getMSG().empty() ? "Leaving ..." :  client.getMSG();
    }

    client.sendMsgAllChannels(RPL_QUIT(client.getPrefix(), reason));
    // client.sendMsg(RPL_QUIT(client.getPrefix(), reason));
    client.leavALLChannels();
    _srv.checkForCloseCannel();
    // _srv.deleteClient(&client);
    _srv.addClientToDelete(&client);
}
