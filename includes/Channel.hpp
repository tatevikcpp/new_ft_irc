#pragma once
#include <iostream>
#include "Client.hpp"
#include <map>
#include <set>

class Client;

class Channel
{
    public:
        Channel(const std::string& name, const std::string& pass = "", Client *client = NULL);
        ~Channel();

        void deleteClient(Client &client);
        void joinClient(Client &client);
        bool isInChannel(Client& client);
        void sending(Client* C, const std::string& msg);
        bool channelIsFull(void);
        bool emptyChannel(void);
        size_t get_limit() const;
        size_t get_size()const;
        std::string get_pass() const;
        void set_pass(const std::string& pass);
        std::string getName(void) const;
        void broadcast(const std::string& message);
        void broadcast(const std::string& message, Client* exclude);
        void print() const;
        void setChannelLimit(int limit);
        int getChannelLimit(void);
        bool changeClientMode(Client& client, int type);
        void nameReply(Client &client);
        bool emptyClients(void);
        const Client &getAdmin() const;
        void setInviteOnly(bool mode);
        bool isInviteOnly(void);
        bool isOperator(Client& client) const;
        bool isEmpty() const;

        void setTopicMode(bool mode);
        bool isTopicModeOn() const;
        Client * getClientNick(const std::string& nickname);
        bool isAdmin(const Client& client) const;
        void sendMsg(Client &client, const std::string &msg, const std::string& cmd);
        std::string getTopic(void) const;
        void setTopic(const std::string& topic);

    private:
        std::map<int, Client *> _clients;
        std::set<Client *> _operators;
        std::list<Client *> _primary;
        Client *_admin;
        std::list<Client *> _listOperator;
        size_t _limit;
        bool _inviteOnly;
        bool _topicMode;
        
    public:
        std::string _name;
        std::string _pass;
        std::string _topic;
};