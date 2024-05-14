#pragma once

#include "Client.hpp"
#include "IRC_Server.hpp"
#include "Numeric_and_error_replies.hpp"
#include <vector>
#include <map>

class Client;
class Command;
class IRC_Server;

class Command
{
    protected:

        IRC_Server& _srv;
        bool    _auth;

        Command();
        Command(const Command& src);

    public:

        explicit Command(IRC_Server& srv, bool auth = true);
        virtual ~Command();

        bool auth_required() const;

        virtual void execute(Client& client, std::vector<std::string> args) = 0;
};

class PrivMsg : public Command
{
    public:

        PrivMsg(IRC_Server& srv);
        ~PrivMsg();

        void    execute(Client& client, std::vector<std::string> args);
};



class Part : public Command
{
    public:

        Part(IRC_Server& srv);
        ~Part();

        void    execute(Client& client, std::vector<std::string> args);
};

class Quit : public Command
{
    public:

        Quit(IRC_Server& srv, bool auth);
        ~Quit();

        void    execute(Client& client, std::vector<std::string> args);
};

class Join : public Command
{
    public:

        Join(IRC_Server& srv);
        ~Join();

        void    execute(Client& client, std::vector<std::string> args);
};

class User : public Command
{
    public:

        User(IRC_Server& srv, bool auth);
        ~User();

        void    execute(Client& client, std::vector<std::string> args);
};

class Nick : public Command
{
    public:

        Nick(IRC_Server& srv, bool auth);
        ~Nick();

        void    execute(Client& client, std::vector<std::string> args);
};

class Pass : public Command
{
    public:

        Pass(IRC_Server& srv, bool auth);
        ~Pass();

        void    execute(Client& client, std::vector<std::string> args);
};

class Kick : public Command
{
    public:

        Kick(IRC_Server& srv);
        ~Kick();

        void    execute(Client& client, std::vector<std::string> args);
};


class Invite : public Command
{
    public:

        Invite(IRC_Server& srv);
        ~Invite();

        void    execute(Client& client, std::vector<std::string> args);
};


class Ping : public Command
{
    public:

        Ping(IRC_Server& srv);
        ~Ping();

        void    execute(Client& client, std::vector<std::string> args);
};

class Pong : public Command
{
    public:

        Pong(IRC_Server& srv);
        ~Pong();

        void    execute(Client& client, std::vector<std::string> args);
};

class Mode : public Command
{
    public:

        Mode(IRC_Server& srv);
        ~Mode();

        void    execute(Client& client, std::vector<std::string> args);
};

class Topic : public Command
{
    public:

        Topic(IRC_Server& srv);
        ~Topic();

        void    execute(Client& client, std::vector<std::string> args);
};

class Cap : public Command
{
    public:

        Cap(IRC_Server& srv);
        ~Cap();

        void    execute(Client& client, std::vector<std::string> args);
};

class Notice : public Command
{
    public:

        Notice(IRC_Server& srv);
        ~Notice();

        void    execute(Client& client, std::vector<std::string> args);
};


class Who : public Command
{
    public:

        Who(IRC_Server& srv);
        ~Who();

        void    execute(Client& client, std::vector<std::string> args);
};