#pragma once
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>



class EventManager 
{
    EventManager();
    ~EventManager();
    public:
        static void start();

        static void addWriteFd(int fd);
        static void delWriteFd(int fd);
        static void addReadFd(int fd);
        static void delReadFd(int fd);
        static fd_set *getWriteFdSet();
        static fd_set *getReadFdSet();
    private:
        static fd_set _readMaster;    // read file descriptor list
        static fd_set _writeMaster;    // read file descriptor list
};