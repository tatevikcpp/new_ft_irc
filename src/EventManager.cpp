#include "EventManager.hpp"
#include <iostream>
fd_set          EventManager::_readMaster;
fd_set          EventManager::_writeMaster;

void EventManager::start() {
    FD_ZERO(&_readMaster);
    FD_ZERO(&_writeMaster);
}

void EventManager::addWriteFd(int fd)
{
    FD_SET(fd, &_writeMaster);
};

void EventManager::addReadFd(int fd)
{
    FD_SET(fd, &_readMaster);
};

void EventManager::delWriteFd(int fd)
{
    FD_CLR(fd, &_writeMaster);
};

void EventManager::delReadFd(int fd)
{
    FD_CLR(fd, &_readMaster);
};

fd_set *EventManager::getWriteFdSet()
{
    return (&_writeMaster);
};

fd_set *EventManager::getReadFdSet()
{
    return (&_readMaster);
};