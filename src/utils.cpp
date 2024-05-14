#include "utils.hpp"
#include <iostream>
#include <sstream>

bool validArgv(std::string port, std::string password)
{
    if (password.empty() ) //|| password.length() < 1
    {
        std::cout << "Error: password must contain at least 8 symbols!" << std::endl;
        return false;
    }
    
    if (port.empty())
    {
        std::cout << "Error: empty port!" << std::endl;
        return false;
    }

    for (size_t i = 0; i < port.size(); ++i)
    {
        if (!std::isdigit(port[i]))
        {
            std::cout << "Error: port must contain only digits!" << std::endl;
            return false;
        }
    }

    int portNum = std::atoi(port.c_str());
    if (portNum <= 0 || portNum > 65535)
    {
        std::cout << "Error: port must be in range [1:65535]!" << std::endl;
        return false;
    }
    
    return true;
}

std::string my_to_string(size_t num)
{
    std::stringstream str;
    str << num;
    return (str.str());
};

std::vector<std::string>  split(const std::string &str, char c)
{
    std::stringstream test(str);
    std::string segment;
    std::vector<std::string> seglist;

    while(std::getline(test, segment, c))
    {
        if (segment.empty() == false)
        {
            seglist.push_back(segment);
        }
    }
    return (seglist);
}