#include "IRC_Server.hpp"


int main(int c, char **v)
{
    if (c != 3)
    {
        std::cout<< "Too many arguments" << std::endl;
        return (1);
    }

    if (!validArgv(v[1], v[2]))
    {
        return (1);
    }

    try
    {
        IRC_Server s(v[1], v[2]);
        s.start();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

