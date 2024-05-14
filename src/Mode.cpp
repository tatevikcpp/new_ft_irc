#include "Command.hpp"

Mode::Mode(IRC_Server& srv) : Command(srv) {}

Mode::~Mode() {}

// syntax:  MODE <nickname> <flags> (user)
//          MODE <channel> <flags> [<args>]

void    Mode::execute(Client& client, std::vector<std::string> args)
{
    // hanling errors
    if (!client.isRegistered())
    {
        client.reply(ERR_NOTREGISTERED(client.getNICK()));
        return ;
    }

    if (args.empty() || args.size() < 2)
    {
        client.reply(ERR_NEEDMOREPARAMS(client.getNICK(), "MODE"));
        return ;
    }
    
    std::string channelName = args[0];
    Channel* channel = _srv.getChannel(channelName);
    if (!channel)
    {
        client.reply(ERR_NOSUCHCHANNEL(client.getNICK(), channelName + static_cast<char>(1)));
        return ;
    }

    if (!channel->isInChannel(client))
    {
        client.reply(ERR_NOTONCHANNEL(client.getNICK(), channelName + static_cast<char>(1)));
        return ;
    }

    if (!channel->isOperator(client) && !channel->isAdmin(client))
    {
        client.reply(ERR_CHANOPRIVSNEEDED(client.getNICK(), channelName + static_cast<char>(1)));
        return ;
    }

    if (args.size() > 1)
    {
        std::string mode = args[1];

        // MODE +/- i
        if (mode == "i" || mode == "+i" || mode == "-i")
        {
            bool condition = (mode == "-i") ? false : true;
            mode = (mode == "i") ? "+i" : mode;

            channel->setInviteOnly(condition);
            client.sendMsg(RPL_MODE(client.getPrefix(), channelName, mode));
            client.reply(RPL_CHANNELMODEIS(channelName, channelName + static_cast<char>(1), mode));
        }
        else if (mode == "t" || mode == "+t" || mode == "-t")
        {
            bool condition = (mode == "-t") ? false : true;
            mode = (mode == "t") ? "+t" : mode;

            channel->setTopicMode(condition);
            client.sendMsg(RPL_MODE(client.getPrefix(), channelName, mode));
            client.reply(RPL_CHANNELMODEIS(channelName, channelName + static_cast<char>(1), mode));
        }
        else if (mode == "k" || mode == "+k" || mode == "-k")
        {
            std::string key;
            if (args.size() > 2)
                key = args[2];

            if (mode != "-k")
            {
                if (channel->get_pass() != "")
                {
                    client.reply(ERR_KEYSET(client.getNICK(), mode));
                    return;
                }

                channel->set_pass(key);
                client.sendMsg(RPL_MODE(client.getPrefix(), channelName, "+k " + key));
            }
            else
            {
                if (channel->get_pass() == key)
                {
                    channel->set_pass("");
                    client.sendMsg(RPL_MODE(client.getPrefix(), channelName, "-k "));
                }
                else
                {
                    client.reply(ERR_BADCHANNELKEY(client.getNICK(), mode, ":invalid key"));
                    return ;
                }
            }
            client.reply(RPL_CHANNELMODEIS(channelName, channelName + static_cast<char>(1), mode));
        }
        // MODE +/- o
        else if (mode == "o" || mode == "+o" || mode == "-o")
        {
            // bool condition = (mode == "-o") ? false : true;
            mode = (mode == "o") ? "+o" : mode;

            if (args.size() < 3)
            {
                client.reply(ERR_NEEDMOREPARAMS(client.getNICK(), "MODE"));
                return ;
            }
 
            std::string nickname = args[2];
            // nickname.erase(0, 1);
            Client* ptr = channel->getClientNick(nickname);
            if (!ptr)
            {
                client.reply(ERR_USERNOTINCHANNEL(client.getNICK(), nickname, channelName + static_cast<char>(1)));
                return ;
            }

            if (mode == "-o")
            {
                if (!channel->isAdmin(*ptr) && channel->changeClientMode(*ptr, Primary))
                {
                    (*ptr).sendMsg(RPL_MSG((*ptr).getPrefix(), "MODE", channelName, ":you are no longer a channel operator"));
                }
            }
            else
            {
                (*ptr).sendMsg(RPL_MSG((*ptr).getPrefix(), "MODE", channelName, ":you are now a channel operator"));
                channel->changeClientMode(*ptr, Operator);
            }
            client.reply(RPL_CHANNELMODEIS(channelName, channelName + static_cast<char>(1), mode));
        }
        //  MODE +/- l
        else if (mode == "l" || mode == "+l" || mode == "-l")
        {
            if (mode != "-l")
            {
                if (args.size() < 3)
                {
                    client.reply(ERR_NEEDMOREPARAMS(client.getNICK(), "MODE"));
                    return ;
                }

                int new_limit = std::atoi(args[2].c_str());
                if (new_limit < 1)
                {
                    client.reply(ERR_UNKNOWNMODE(client.getNICK(), mode, " :limit must be greater than 0"));
                    return;
                }
                if ((size_t)new_limit < channel->get_size())
                {
                    client.reply(ERR_UNKNOWNMODE(client.getNICK(), mode, " :number of clients is already greater than limit"));
                    return;
                }
                channel->setChannelLimit(new_limit);
            }
            else
            {
                channel->setChannelLimit(0); // unlimit
            }
            
            client.reply(RPL_CHANNELMODEIS(channelName, channelName + static_cast<char>(1), mode));
        }
        else
        {
            client.reply(ERR_UNKNOWNMODE(client.getNICK(), mode, " :is unknown mode char to me"));
            return;
        }
    }
}