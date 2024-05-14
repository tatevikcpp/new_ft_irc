#include "IRC_Server.hpp"
#include "utils.hpp"
#include <fcntl.h>
#include "Command.hpp"
#include "EventManager.hpp"

IRC_Server::IRC_Server(const char *port, const char *password)
{
    this->_port = std::atoi(port);
    this->_password = password;
    _s_addr.sin_family = AF_INET;
    _s_addr.sin_port = htons(_port);
    _s_addr.sin_addr.s_addr = INADDR_ANY;

    _commands["PASS"] = new Pass(*this, false);
    _commands["NICK"] = new Nick(*this, false);
    _commands["USER"] = new User(*this, false);
    _commands["QUIT"] = new Quit(*this, false);

    _commands["PING"] = new Ping(*this);
    _commands["PONG"] = new Pong(*this);
    _commands["JOIN"] = new Join(*this);
    _commands["KICK"] = new Kick(*this);
    _commands["MODE"] = new Mode(*this);
    _commands["INVITE"] = new Invite(*this);

	_commands["PRIVMSG"] = new PrivMsg(*this);
    _commands["TOPIC"] = new Topic(*this);
    _commands["CAP"] = new Cap(*this);
    _commands["NOTICE"] = new Notice(*this);
    _commands["WHO"] = new Who(*this);
}

IRC_Server::~IRC_Server()
{

}

std::string IRC_Server::getPASS(void)
{
    return (this->_password);
}


Client* IRC_Server::getClient(const std::string& nick)
{
    std::map<int, Client *>::iterator it_b = _clients.begin();
    std::map<int, Client *>::iterator it_e = _clients.end();

    while (it_b != it_e)
    {
        if (!nick.compare(it_b->second->getNICK()))
            return it_b->second;

        it_b++;
    }

    return (NULL);
}


Channel * IRC_Server::getChannel(const std::string& name)
{
    std::map<std::string, Channel *>::iterator it = this->_channels.find(name);
    if (it != this->_channels.end())
    {
        return(it->second);
    }
    return (NULL);
}

bool IRC_Server::checkNickname(const std::string& nick)
{
    std::map<std::string, int>::iterator it = this->_nickname.find(nick);
    if (it == _nickname.end())
    {
        return false;
    }
    return true;
}

void IRC_Server::changeNickname(Client &client, const std::string& newNick)
{
    if (this->_clients.find(client._fd) != this->_clients.end())
    {
        this->_clients[client._fd]->setNICK(newNick);
    }
}

void IRC_Server::addChannel(Channel &channel)
{
    if (this->_channels.insert(std::make_pair(channel._name, &channel)).second == false)
    {
        std::cout << "alredy exist\n";
    }
}

void IRC_Server::delChannel(Channel *channel)
{
    if (channel)
    {
        this->_channels.erase(channel->_name);
        delete channel;
    }
}

void IRC_Server::addClientToDelete(Client *client)
{
    this->_clientToDelete.push_back(client);
}

void IRC_Server::addClientToChannel(const std::string& name, Client &client)
{
    std::map<std::string, Channel *>::iterator  it = this->_channels.find(name);
    if (it != this->_channels.end())
    {
        it->second->joinClient(client);
    }
}

Channel* IRC_Server::createChannel(const std::string& name, const std::string& pass, Client &client)
{
    Channel *new_channel = new Channel(name, pass, &client);
    this->addChannel(*new_channel);
    return (new_channel);
}

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void IRC_Server::checkForCloseCannel(void)
{
    std::vector<std::string> closedFds; 
    std::map<std::string, Channel *>::iterator it = _channels.begin();

    for ( ; it != _channels.end(); ++it)
    {
        if ((it->second)->emptyClients())
        {
            closedFds.push_back(it->first);
        }
    }

    for (size_t i = 0; i < closedFds.size(); ++i)
    {
        delete _channels[closedFds[i]];
        _channels.erase(closedFds[i]);
    }
}

void IRC_Server::deleteClient(Client *client)
{
    this->_clients.erase(client->getFd());
    delete client;
    this->_clients.erase(100);
}

int IRC_Server::start(void)
{
    fd_set read_fds;  // temp file descriptor list for select()
    fd_set write_fds;  // temp file descriptor list for select()
    struct sockaddr remoteaddr; // client address
    socklen_t addrlen;

    char buf[256];    // buffer for client data
    int nbytes;

    int yes = 1;        // for setsockopt() SO_REUSEADDR, below
    int rv;

    struct addrinfo hints, *ai, *p;
    FD_ZERO(&read_fds);

    // get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, my_to_string(this->_port).c_str(), &hints, &ai)) != 0)
    {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }
    
    for(p = ai; p != NULL; p = p->ai_next) 
    {
        this->_listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (this->_listener < 0) 
        {
            continue;
        }
        
        // lose the pesky "address already in use" error message
        setsockopt(this->_listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(this->_listener, p->ai_addr, p->ai_addrlen) < 0) 
        {
            perror("bind: ");
            close(this->_listener);
            continue;
        }

        break;
    }

    // if we got here, it means we didn't get bound
    if (p == NULL) 
    {
        fprintf(stderr, "selectserver: failed to bind\n");
        exit(2);
    }

    freeaddrinfo(ai); // all done with this

    // listen
    if (listen(this->_listener, 0) == -1)
    {
        perror("listen");
        exit(3);
    }

    // add the listener to the master set 
    EventManager::start();
    EventManager::addReadFd(this->_listener);

    // keep track of the biggest file descriptor
    this->_fdmax = this->_listener + 1; // so far, it's this one

    for(;;) 
    {
        read_fds = *EventManager::getReadFdSet(); // copy it
        write_fds = *EventManager::getWriteFdSet(); // copy it
        _select_fd = select(_fdmax, &read_fds, &write_fds, NULL, NULL);
        // std::cout << "_select_fd= " << _select_fd <<  std::endl;
        if (_select_fd == -1)
        {
            perror("select");
        }
        else if (_select_fd)
        {
            // run through the existing connections looking for data to read
            if (FD_ISSET(this->_listener, &read_fds)) 
            {
                addrlen = sizeof remoteaddr;
                _newfd = accept(this->_listener,
                    (struct sockaddr *)&remoteaddr,
                    &addrlen);

                if (_newfd == -1)
                {
                    perror("accept");
                }
                else if (_newfd)
                {
                    std::cout << "client connect" << std::endl;
                    if (fcntl(this->_listener, F_SETFL, O_NONBLOCK) == -1)
                    {
                        close(this->_listener);
                        std::cout << "Error: setting client fd to non-blocking mode failed!" << std::endl;
                    }

                    EventManager::addReadFd(_newfd);
                    if (_newfd >= _fdmax) 
                    {
                        _fdmax = _newfd + 1;
                    }
                    std::cout << "new user = " << _newfd << std::endl;
                    Client* tmp = new Client(_newfd, remoteaddr);
                    this->_clients[_newfd] = tmp;
                }
            }
            for(std::map<int, Client *>::iterator it = this->_clients.begin();
                   _select_fd != 0 && it != this->_clients.end() ; ++it)
            {
                if (FD_ISSET(it->first, &read_fds))
                {
                    _select_fd--;
                    if ((nbytes = recv(it->first, buf, sizeof buf, 0)) <= 0)
                    {
                        std::cout << "nbytes= " << nbytes << std::endl;
                        if (nbytes == 0)
                        {
                            std::map<int, Client *>::iterator clientIt = _clients.find(it->first);
                            if (clientIt != _clients.end())
                            {
                                this->_commands["QUIT"]->execute(*clientIt->second, clientIt->second->getArguments());
                            }
                            // connection closed
                            printf("selectserver: socket %d hung up\n", it->first);
                        } 
                        else
                        {
                            perror("recv");
                        }

                        // close(it->first);
                        // EventManager::delReadFd(it->first);
                        // EventManager::delWriteFd(it->first);
                        
                    } 
                    else
                    {
                        std::cout << "nbytes = " << nbytes << std::endl;
                        it->second->_buffer.append(buf, nbytes);

                        if (it->second->_buffer.find('\n') != std::string::npos)
                        {
                            std::cout << "_buffer = " << it->second->_buffer << std::endl;
                            it->second->splitbuffer();
                            it->second->setArguments();
                            if (it->second->getCommand().empty() == true)
                            {
                                it->second->_buffer.clear();
                                continue;
                            }
                            std::cout << "it->second->getCommand() = " << it->second->getCommand() << std::endl;
                            if (_commands.find(it->second->getCommand()) == _commands.end()) 
                            {
                                it->second->reply(ERR_UNKNOWNCOMMAND(it->second->getNICK(), it->second->getCommand()));
                                it->second->_buffer.clear();
                                continue;
                            }

                            it->second->_buffer.clear();
                            this->_commands[it->second->getCommand()]->execute(*it->second, it->second->getArguments());

                        }
                        // std::map<std::string, Channel *>::iterator itChannel = _channels.begin();
                        // while (itChannel != _channels.end()) 
                        // {
                        //     itChannel->second->print();
                        //     itChannel++;
                        // }
                    }
                }
                else if (FD_ISSET(it->first, &write_fds))
                {
                    it->second->sendLalala();
                    EventManager::delWriteFd(it->first);
                }

            }
            // TODO QCEL FUNKICIA MECH
            for (size_t i = 0; i < _clientToDelete.size(); ++i)
            {
                std::cout << " _clientToDelete[i]->getNICK()" << _clientToDelete[i]->getNICK() << std::endl;;
                deleteClient(_clientToDelete[i]);
            }
            _clientToDelete.clear();
        }
    }
    return 0;
}

void IRC_Server::print_channels()
{
    std::map<std::string, Channel *>::iterator it = this->_channels.begin();
    for( ; it != this->_channels.end(); ++it)
    {
        std::cout << "!!!: " << it->first << std::endl;
    }
}