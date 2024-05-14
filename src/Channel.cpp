#include "Channel.hpp"
#include "Numeric_and_error_replies.hpp"
#include <algorithm>
#include "EventManager.hpp"

// void Channel::print() const {
//     std::map<int, Client *>::const_iterator it = _clients.cbegin();
//     std::cout << "-----------------------------------------------------\n";
//     std::cout << _name << std::endl; 
//     std::cout << "size = " << this->_clients.size() << std::endl;
//     while (it != _clients.end())
//     {
//         std::string name = it->second->getNICK();
//         std::cout << name << ", isAdmin = " << this->isAdmin(*it->second) << ", isOperator = " << this->isOperator(*it->second) << std::endl;
//         it++;
//     }

//     std::cout << "\n-----------------------------------------------------\n";
// };



Channel::Channel(const std::string& name, const std::string& pass, Client *client) : _name(name), _pass(pass)
{
    if (client)
        this->joinClient(*client);
    this->_limit = 0;
    this->_inviteOnly = false;
    this->_topicMode = true;
}

Channel::~Channel()
{

}

void Channel::sendMsg(Client &client, const std::string &msg, const std::string& cmd)
{
    std::map<int, Client *>::iterator it = this->_clients.begin();

    for(; it != this->_clients.end(); ++it)
    {
        if (&client != it->second)
        {
            // it->second->sendMsg(RPL_MSG(client.getPrefix(), cmd, _name, msg));
            it->second->sendMsg(RPL_MSG(client.getPrefix(), cmd, _name, msg));
        }
    }    
}

void Channel::nameReply(Client &client)
{   
    if (_topic.empty())            
        client.sendMsg(RPL_NOTOPIC(_name + static_cast<char>(1)));
    else            
        client.sendMsg(RPL_TOPIC(_name + static_cast<char>(1), _topic));

    // sending channal's users list to new user
    std::string nickList;
    std::map<int, Client *>::iterator it = this->_clients.begin();
    for(; it != this->_clients.end(); ++it)
    {
        std::string prefix = this->isAdmin(*it->second) == true ? "@" : "+";

        nickList += prefix + it->second->getNICK() + "  ";
    }

    client.sendMsg(RPL_NAMREPLY(client.getNICK(), _name + static_cast<char>(1), nickList));
    client.sendMsg(RPL_ENDOFNAMES(client.getNICK(), _name + static_cast<char>(1)));
}

std::string Channel::getTopic(void) const
{
    return _topic;
}

void Channel::setTopic(const std::string& topic)
{
    _topic = topic;
}


void Channel::joinClient(Client &client)//_clients[i]->sending(RPL_JOIN(C->getPrefix(), _channelName));
{
    if (this->_clients.find(client._fd) != this->_clients.end())
    {
        return ;
    }
    if (this->_clients.empty())
    {
        this->_admin = &client;
    }
    this->_clients[client._fd] = &client;
    this->_primary.push_back(&client);
    client.joinToChannel(*this);

    std::map<int, Client *>::iterator it = this->_clients.begin();
    for(; it != this->_clients.end(); ++it)
    {
        it->second->sendMsg(RPL_JOIN(client.getPrefix(), _name));
    }
    this->nameReply(client);
}

void Channel::deleteClient(Client &client)
{
    std::map<int, Client *>::iterator it_m = this->_clients.find(client.getFd());
    if (it_m == this->_clients.end())
    {
        return ;
    }

    this->_clients.erase(client._fd);
    std::list<Client *>::iterator itPrimary = std::find(this->_primary.begin(), this->_primary.end(), &client);
    if (itPrimary != this->_primary.end())
    {
        this->_primary.erase(itPrimary);
    }
    std::list<Client *>::iterator itOperator = std::find(this->_listOperator.begin(), this->_listOperator.end(), &client);
    if (itOperator != this->_listOperator.end())
    {
        this->_listOperator.erase(itOperator);
    }

    if (&client == this->_admin)
    {
        if (this->_listOperator.empty() == false)
        {
            this->_admin = this->_listOperator.front();
            this->_listOperator.pop_front();
        }
        else if(this->_primary.empty() == false)
        {
            this->_admin = this->_primary.front();
            this->_primary.pop_front();
        }
    }
}

bool Channel::changeClientMode(Client& client, /* TypeClient */ int type)
{
    if (type == Operator)
    {
        if (this->_operators.insert(&client).second)
        {
            this->_listOperator.push_back(&client);
            return true;
        } 
    }
    else if (type == Primary)
    {
        if (this->_operators.erase(&client))
        {
           this->_listOperator.erase(std::find(this->_listOperator.begin(), this->_listOperator.end(), &client));
           return true;
        } 
    }
    return false;
}

void Channel::sending(Client* C, const std::string& msg)
{
    std::map<int, Client *>::const_iterator it = this->_clients.cbegin();

    while (it != this->_clients.cend())
    {
        if (C->_fd != it->first)
        {
            EventManager::addWriteFd(it->first);
            it->second->appendResponse(msg);
        }
        it++;
    }
}

Client* Channel::getClientNick(const std::string& nickname)
{
    std::map<int, Client *>::iterator it = this->_clients.begin();
    for (; it != this->_clients.end(); ++it)
    {
        if (it->second->getNICK() == nickname)
        {
            return (it->second);
        }
    }
    return (NULL);
}

void Channel::setChannelLimit(int limit)
{
    this->_limit = limit;
}

int Channel::getChannelLimit(void)
{
    return _limit;
}

bool Channel::emptyChannel(void)
{
    return (this->_clients.empty());
}

bool Channel::channelIsFull(void)
{
    if (_limit == 0)
        return false;

    return _limit <= static_cast<size_t>(_clients.size());
}

size_t Channel::get_limit() const
{
    return (this->_limit);
}

size_t Channel::get_size() const 
{ 
    return this->_clients.size();
}

std::string Channel::get_pass() const
{
    return (this->_pass);
}

bool Channel::isInviteOnly(void)
{
    return _inviteOnly;
}

std::string Channel::getName(void) const
{
    return (this->_name);
}

bool Channel::isOperator(Client& client) const
{
    std::set<Client *>::iterator it = this->_operators.find(&client);
    if (it != this->_operators.end())
        return (true);
    return (false);

}

void Channel::setInviteOnly(bool mode)
{
    this->_inviteOnly = mode;
}

void Channel::setTopicMode(bool mode)
{
    this->_topicMode = mode;
}

bool Channel::isTopicModeOn() const
{
    return (this->_topicMode);
}

void Channel::set_pass(const std::string& pass)
{
    _pass = pass;
}

void Channel::broadcast(const std::string& message)
{
    std::map<int, Client *>::iterator it = this->_clients.begin();
    for (; it != this->_clients.end(); it++)
    {
        it->second->reply(message);
    }
}

void Channel::broadcast(const std::string& message, Client* exclude)
{
    std::map<int, Client *>::iterator it = this->_clients.begin();
    for (; it != this->_clients.end(); it++)
    {
        if (it->second == exclude)
        {
            ++it;
            continue;
        }
        it->second->reply(message);
    }
}

bool Channel::isAdmin(const Client& client) const
{
    return (&client == this->_admin);
}

bool Channel::emptyClients(void)
{
    return _clients.empty();
}

bool Channel::isInChannel(Client& client)
{
    if (this->_clients.find(client.getFd()) == this->_clients.end())
        return false;
    return true;
}

const Client &Channel::getAdmin() const
{
    return (*_admin);
};

bool Channel::isEmpty() const
{
    return (_clients.empty());
};