#include "Command.hpp"

Nick::Nick(IRC_Server& srv, bool auth) : Command(srv, auth) {}

Nick::~Nick() {}

// syntax: NICK <nickname>

bool nickIsCorrect(std::string buffer)
{
    std::string notAllowed = " ,*?!@$:#.";
    std::size_t pos = buffer.find_first_of(notAllowed);
    if (pos != std::string::npos)
    {
        return false;
    }
    return true;
}

void    Nick::execute(Client& client, std::vector<std::string> args)
{
    if (client.getPASS().empty())
    {
        client.reply(ERR_NOTREGISTERED(client.getNICK()));
        return ;
    }
    if (args.empty() || args[0].empty()) 
    {
        client.reply(ERR_NONICKNAMEGIVEN(client.getNICK()));
        return;
    }

    std::string nickname = args[0];

    if (!nickIsCorrect(nickname))
    {
        client.reply(ERR_ERRONEUSNICKNAME(client.getNICK(), nickname));
        return ;
    }

    if (_srv.getClient(nickname))
    {
        client.reply(ERR_NICKNAMEINUSE(client.getNICK(), nickname));
        return;
    }

    client.setNICK(nickname);
    client.checkForRegistered();
}