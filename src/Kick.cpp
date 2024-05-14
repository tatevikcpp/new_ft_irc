#include "Command.hpp"

Kick::Kick(IRC_Server& srv) : Command(srv) {}

Kick::~Kick() {}

// syntax: KICK <channel> <client> :[<message>]

void    Kick::execute(Client& client, std::vector<std::string> args)
{
    if (!client.isRegistered())
    {
        client.reply(ERR_NOTREGISTERED(client.getNICK()));
        return ;
    }

    if (args.size() < 2)
    {
        client.reply(ERR_NEEDMOREPARAMS(client.getNICK(), "KICK"));
        return ;
    }

    std::vector<std::string>::iterator it = std::find(args.begin(), args.end(), "");
    std::vector<std::string> channelNames(args.begin(), it);

    std::vector<std::string> chanelUsers;
    if (it != args.end())
        chanelUsers.insert(chanelUsers.begin(), it + 1, args.end()) /* = std::vector<std::string>(it + 1, args.end()) */;

    std::string comment = client.getMSG();
    
    std::string channelName = channelNames[0];

    Channel* channel = _srv.getChannel(channelName);
    if (!channel)
    {
        client.reply(ERR_NOSUCHCHANNEL(client.getNICK(), channelName + static_cast<char>(1)));
        return ;
    }

    if ((!channel->isOperator(client) && !channel->isAdmin(client)))
    {
        client.reply(ERR_CHANOPRIVSNEEDED(client.getNICK(), channelName + static_cast<char>(1)));
        return ;
    }

    for( size_t i = 0; i < chanelUsers.size(); ++i)
    {
        std::string nickname = chanelUsers[i];


        Client* clientKick = channel->getClientNick(nickname);
        if (!clientKick)
        {
            client.reply(ERR_USERNOTINCHANNEL(client.getNICK(), nickname, channelName + static_cast<char>(1)));
            return ;
        }

        if (!channel->isInChannel(*clientKick))
        {
            client.reply(ERR_NOTONCHANNEL(client.getNICK(), channelName + static_cast<char>(1)));
            return ;
        }

        if (!(channel->isAdmin(client)) && channel->isAdmin(*clientKick))
        {
            client.reply(ERR_CHANOPRIVSNEEDED(client.getNICK(), channelName + static_cast<char>(1)));
            return ;
        }
        
        if (comment.empty())
            comment = "No reason specified.";

        channel->deleteClient(*clientKick);
        clientKick->leaveChannel(channel->getName());
        if (channel->isEmpty()) {
            
        }
        clientKick->sendMsg(RPL_KICK(channel->getAdmin().getPrefix(), channel->getName(), client.getNICK(), comment));
        if (channel->isEmpty())
        {
            _srv.delChannel(channel);
        }
    }
}
