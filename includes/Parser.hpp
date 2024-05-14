/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tkhechoy <tkhechoy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/30 13:52:51 by shovsepy          #+#    #+#             */
/*   Updated: 2024/03/27 14:40:18 by tkhechoy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_PARSER_HPP
#define IRC_PARSER_HPP

#include <iostream>
#include <sstream>
#include <string>

#include <vector>
#include <map>

class Parser;

#include "Command.hpp"

class Parser
{
    public:

        Parser(IRC_Server& srv);
        ~Parser();

        void invoke(Client* client, const std::string& message);
    private:

        IRC_Server*                             _srv;
        std::map<std::string, Command *>    _commands;

        std::string trim(const std::string& str);

};

#endif
