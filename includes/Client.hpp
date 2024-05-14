#pragma once

#include <iostream>
#include <vector>
#include <list>
#include "IRC_Server.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <map>
#include "Channel.hpp"

#include "Numeric_and_error_replies.hpp"

class IRC_Server;
class Channel;

enum TypeClient
{
    Admin,
    Operator,
    Primary
};

class Client
{
    public:

        Client(int fd, const struct sockaddr& addr);
        ~Client();
        void leaveChannel(const std::string &name);
        Channel* createChannel(const std::string& name, const std::string& pass = "");
        bool isRegistered(void);
        void setNICK(const std::string& nick);
        std::string getNICK(void) const;
        void setPASS(const std::string& pass);
        std::string getPASS(void);
        void setUSER(const std::string& username, const std::string& realname);
        bool checkForRegistered(void);
        void splitbuffer(void);
        void sendLalala(void);
        void setArguments(void);
        std::vector<std::string> getArguments(void);
        std::string getName(void) const;
        std::string getPrefix(void) const;
        void reply(const std::string& reply);
        void sendMsg(const std::string& msg);
        void appendResponse(const std::string &str);
        std::string getCommand(void);
        Channel * getChannel(const std::string& name);
        void joinToChannel(Channel &channel);
        int getFd(void);
        void leavALLChannels(void);
        std::string _buffer;
        std::string getMSG(void) const;
        std::string getLalala(void) const;
        const std::string& getMSG(void);
        void sendMsgAllChannels(const std::string& name);


        bool _isColon;
        const int _fd;
        
    private:

        Client(Client&);
        Client& operator=(Client&);

        struct sockaddr _clientAddr;
        std::map<std::string, std::pair<Channel*, TypeClient> > _channels;

        std::string _pass;
        std::string _nick;
        std::string _username;
        std::string _hostname;
        std::string _realname;
        std::string _msg;

        std::string _lalala;

        std::string _command;
        std::vector<std::string> _arguments;
        std::vector<std::string> _vecBuffer;

        bool _registered;
};