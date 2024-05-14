#include "Command.hpp"

PrivMsg::PrivMsg(IRC_Server& srv) : Command(srv) {}

PrivMsg::~PrivMsg() {}

// syntax: PRIVMSG <msgtarget> :<message>

void    PrivMsg::execute(Client& client, std::vector<std::string> args)
{
    if (!client.isRegistered())
    {
        client.reply(ERR_NOTREGISTERED(client.getNICK()));
        return;
    }
    if (args.empty())
    {
        client.reply(ERR_NEEDMOREPARAMS(client.getNICK(), "PRIVMSG"));
        return ;
    }

    std::string message = client.getMSG();
    if (message.empty())
    {
        client.reply(ERR_NOTEXTTOSEND(client.getNICK()));
        return ;
    }

    for (size_t i = 0; i < args.size(); i++)
    {
        std::string target = args[i];
        if (target[i] == '#' || target[i] == '&')
        {
            Channel* channel = _srv.getChannel(target);
            if (!channel)
            {
                client.reply(ERR_NOSUCHNICK(client.getNICK(), target));
                continue; ;
            }
            if (!channel->isInChannel(client))
            {
                client.reply(ERR_CANNOTSENDTOCHAN(client.getNICK(), target));
                continue; ;
            }
            
            channel->sendMsg(client, message, "PRIVMSG");
        }
        else
        {
            Client* recv_client = _srv.getClient(target);
            if (!recv_client)
            {
                client.sendMsg(ERR_NOSUCHNICK(client.getNICK(), target));
                continue; ;
            }

            recv_client->sendMsg(RPL_MSG(client.getPrefix(), "PRIVMSG", target, message));

        }
    }
}