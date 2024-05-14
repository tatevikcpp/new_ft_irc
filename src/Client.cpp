#include "Client.hpp"
#include <netinet/in.h>
#include <sstream>
#include <utility>
#include "Numeric_and_error_replies.hpp"
#include "EventManager.hpp"

Client::Client(int fd, const struct sockaddr& addr) : _fd(fd)
{
    this->_clientAddr = addr;
    this->_registered = false;

    char hostname[NI_MAXHOST];
    getnameinfo((struct sockaddr*)&addr, sizeof(addr), hostname, NI_MAXHOST, NULL, 0, NI_NUMERICSERV);
    _hostname = hostname; 
}

Client::~Client()
{
    this->leavALLChannels();
    close(_fd);
    EventManager::delReadFd(_fd);
    EventManager::delWriteFd(_fd);
}

bool Client::isRegistered(void)
{
    return (this->_registered);
}

bool Client::checkForRegistered(void)
{
    if (!_pass.empty() && !_username.empty() && !_nick.empty() && !_registered)
    {
        this->_registered = true;
        reply(RPL_WELCOME(_nick));
    }
    else
        this->_registered = false;

    return (this->_registered);
}

void Client::sendMsgAllChannels(const std::string& msg)
{
    std::map<std::string, std::pair<Channel*, TypeClient> > :: iterator it = this->_channels.begin();
    for (; it != this->_channels.end(); ++it)
    {
        it->second.first->sending(this, msg);
    }
}

std::string Client::getLalala(void) const
{
    return (this->_lalala);
}

void Client::sendLalala(void)
{
    if (send(this->_fd, this->_lalala.c_str(), this->_lalala.length(), 0) < 0)
        std::cerr << "Error: can't send message to client." << std::endl;
    this->_lalala.clear();
}

int Client::getFd(void)
{
    return (this->_fd);
}

void Client::setPASS(const std::string& pass)
{
    this->_pass = pass;
}

std::string Client::getPASS(void)
{
    return (_pass);
};

void Client::setNICK(const std::string& nick)
{
    this->_nick = nick;
}

void Client::setUSER(const std::string& username, const std::string& realname)
{
    this->_username = username;
    this->_realname = realname;
}

//   std::map<std::string, std::pair<Channel*, TypeClient>>
Channel* Client::createChannel(const std::string& name, const std::string& pass)
{
    if (this->_channels.find(name) == this->_channels.end())
    {
        return (NULL);
    }
    Channel *tmp = new Channel(name, pass, this);
    this->_channels[name] = std::make_pair(tmp, Admin);
    return (tmp);
}

void Client::leaveChannel(const std::string &name)
{
    std::map<std::string, std::pair<Channel*, TypeClient> >::iterator it = this->_channels.find(name);

    if (it != this->_channels.end())
    {
        it->second.first->deleteClient(*this);
        this->_channels.erase(it);
    }
}


void Client::leavALLChannels()
{
    std::map<std::string, std::pair<Channel*, TypeClient> >::iterator it = this->_channels.begin();

    for (; it != this->_channels.end(); ++it)
    {
        it->second.first->deleteClient(*this);
    }
    this->_channels.clear();
}

std::string     trim(const std::string& str)
{
    std::string WHITESPACE = " \n\r\t\f\v";
    std::string result = "";

    size_t  start = str.find_first_not_of(WHITESPACE);
    if (start != std::string::npos)
        result = str.substr(start);

    size_t  end = result.find_last_not_of(WHITESPACE);
    if (end != std::string::npos)
        result = result.substr(0, end + 1);

    return result;
}

std::vector<std::string> split(std::string const &input)
{
    std::istringstream buffer(input);
    std::vector<std::string> ret;

    std::copy(std::istream_iterator<std::string>(buffer), 
              std::istream_iterator<std::string>(),
              std::back_inserter(ret));
    return ret;
}

std::string Client::getCommand(void)
{
    return (this->_command);
}

std::string Client::getMSG(void) const
{
    return (_msg);
}

const std::string& Client::getMSG(void)
{
    return (_msg);
}

void Client::setArguments(void)
{
    _arguments.clear();
    _command.clear();


    if (!this->_vecBuffer.empty() && !this->_vecBuffer[0].empty())
    {
        _command = _vecBuffer[0].substr(0, _vecBuffer[0].find(" "));
        _vecBuffer[0].erase(0, _vecBuffer[0].find(" "));
        size_t pos = _vecBuffer[0].find(":");

        if (pos != std::string::npos)
        {
            _msg = _vecBuffer[0].substr(pos + 1);
            _vecBuffer[0].erase(pos);
            _isColon = true;
        }
        else
        {
            _msg = "";
            _isColon = false;
        }
        
        std::vector<std::string> splitedVec = split(_vecBuffer[0], ' ');
        for (size_t i = 0; i < splitedVec.size(); ++i)
        {
            std::vector<std::string> tmp = split(splitedVec[i], ',');

            for (size_t i = 0; i < tmp.size(); ++i)
            {
                _arguments.push_back(tmp[i]);
            }
            if (i + 1 < splitedVec.size() && splitedVec[i].back() != ',' && splitedVec[i + 1].front() != ',' 
                && (_command == "JOIN" || _command == "PRIVMSG" || _command == "KICK" || _command == "TOPIC"))
            {
                _arguments.push_back("");
            }
            
        }
        
    }
}

std::vector<std::string> Client::getArguments(void)
{
    return (this->_arguments);
}


void Client::splitbuffer(void)
{
    std::string str = this->_buffer;
    std::string del = "\r\n";
    size_t start = 0;

    _vecBuffer.clear();
    if (str.find(del) == std::string::npos)
        del = '\n';
    
    size_t end = str.find(del);

    while (end != std::string::npos)
    {
        this->_vecBuffer.push_back(str.substr(start, end - start));
        start = end + del.length();
        end = str.find(del, start);
    }
}

void Client::reply(const std::string& reply)
{
    std::string buff = ":" + this->getPrefix() + " " + reply + "\r\n";

    EventManager::addWriteFd(this->_fd);
    this->appendResponse(buff);
}


void Client::sendMsg(const std::string& msg)
{
    std::string buff = msg + "\r\n";

    EventManager::addWriteFd(this->_fd);
    this->appendResponse(buff);
}

void Client::appendResponse(const std::string &str)
{
    _lalala.append(str);
};


std::string Client::getNICK(void) const
{
    return (this->_nick);
}

std::string Client::getPrefix(void) const
{
    std::string prefix = this->_nick;

    if (!this->_username.empty())
    {
        prefix += "!" + this->_username;
    }

    if (!this->_hostname.empty())
    {
        prefix += "@" + this->_hostname;
    }

    return (prefix); 
}

Channel * Client::getChannel(const std::string& name)
{
    std::map<std::string, std::pair<Channel*, TypeClient> >::iterator it = this->_channels.find(name);
    if (it == this->_channels.end())
    {
        return (NULL);
    }
    return (it->second.first);
}

void Client::joinToChannel(Channel &channel)
{

    std::map<std::string, std::pair<Channel*, TypeClient> >::iterator it = this->_channels.find(channel.getName());

    if (it == this->_channels.end())
    {
        this->_channels.insert(std::pair<std::string, std::pair<Channel*, TypeClient> >(channel.getName(), std::pair<Channel*, TypeClient>(&channel, Primary)));
    }
}