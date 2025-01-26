#pragma once
#ifndef _B865_CLI_H_
#define _B865_CLI_H_

#include <string>
#include <unordered_map>
#include <vector>
#include <functional>

class CLI
{
public:
    CLI();

    void start(bool help);
    void addCommand(const std::string &name, bool allowShortAlias,
                         const std::function<void(const std::vector<std::string> &)> &handler,
                         const std::string &usage);
    void printUsage();

private:
    struct CommandInfo
    {
        std::function<void(const std::vector<std::string> &)> handler;
        bool allowShortAlias;
        std::string usage;
    };

    std::unordered_map<std::string, CommandInfo> commands;
    std::unordered_map<char, std::string> shortAliases;
    bool running = true;

    void handleEscape(const std::string &input, size_t& i, std::string &currentArg);
    void parseArguments(const std::string &input, std::vector<std::string> &args);
    void handleQuotes(char c, bool &insideQuotes, char &quoteChar);
    void processFullCommand(const std::vector<std::string> &args);
    void processCommand(const std::string &input);
    void quitCommand(const std::vector<std::string> &args);
};

#endif