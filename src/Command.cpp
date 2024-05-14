#include "Command.hpp"

Command::Command(IRC_Server& srv, bool auth) :_srv(srv), _auth(auth) {}

Command::~Command() {}

bool    Command::auth_required() const 
{ 
    return _auth; 
}