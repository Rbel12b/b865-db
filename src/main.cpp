#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include "Parser/Parser.h"
#include "CLI.h"

Parser parser;
CLI cli;
DebuggerData *data = nullptr;

void printModules()
{
    if (data != nullptr)
    {
        std::cout << "Modules:\n";
        for (const auto &str : data->modules)
        {
            std::cout << "    " << str << "\n";
        }
    }
}

void handleHelpOption(const std::vector<std::string> &args)
{
    if (std::find(args.begin(), args.end(), "-h") != args.end() ||
        std::find(args.begin(), args.end(), "--help") != args.end() ||
        std::find(args.begin(), args.end(), "-help") != args.end())
    {
        cli.printUsage();
        exit(0);
    }
}

int main(int argc, char *argv[])
{
    cli.addCommand("modules", true, [](const std::vector<std::string> &args)
                   { printModules(); }, "Print modules from the debug file");
    cli.addCommand("print", true, [](const std::vector<std::string> &args)
                   { 
        if (args.size() > 1) { std::cout << args[1] << std::endl; } }, "<string> Print the string");

    // Convert command-line arguments to a vector of strings
    std::vector<std::string> args(argv + 1, argv + argc);

    bool help = false;
    bool inFile = false;
    std::string filename;

    handleHelpOption(args);

    for (const auto &arg : args)
    {
        if (arg[0] != '-')
        {
            inFile = true;
            filename = arg;
            break;
        }
    }

    if (inFile)
    {
        data = parser.parse(filename);
        if (data == nullptr)
        {
            std::cerr << "Error parsing file: " << filename << std::endl;
            return 1;
        }
    }
    else
    {
        std::cout << "No input file specified.\n";
        cli.printUsage();
        return help == true ? 0 : 1;
    }

    cli.start(help);

    if (data != nullptr)
    {
        delete data;
    }

    return 0;
}