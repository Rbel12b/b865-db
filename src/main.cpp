#include <iostream>
#include "Parser/Parser.h"
#include "CLI.h"

Parser parser;
CLI cli;
DebuggerData *data = nullptr;

void printModules()
{
    if (data != nullptr)
    {
        printf("Modules:\n");
        for (auto &str : data->modules)
        {
            printf("    %s\n", str.c_str());
        }
    }
}

int main(int argc, char *argv[])
{
    cli.addCommand("modules", true, [](const std::vector<std::string> &args)
                   { printModules(); }, "print Modules from the debug file");
    cli.addCommand("print", true, [](const std::vector<std::string> &args)
                   { if (args.size() > 1) { printf("%s\n", args[1].c_str()); } }, "<string> Print the string");
    // Convert command-line arguments to a vector of strings
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i)
    {
        args.emplace_back(argv[i]);
    }

    bool help = false;
    bool inFile = false;
    std::string filename = "";

    auto it = std::find(args.begin(), args.end(), std::string("--help"));
    if (it != args.end())
    {
        help = true;
    }

    for (const auto &arg : args)
    {
        if (arg != "--help")
        {
            inFile = true;
            filename = arg;
            break;
        }
    }

    if (inFile)
    {
        data = parser.parse(filename);
    }
    else
    {
        printf("No input file specified.\n");
        if (!help)
        {
            return 1;
        }
        else
        {
            cli.printUsage();
            return 0;
        }
    }
    cli.start(help);

    if (data != nullptr)
    {
        delete data;
    }
    return 0;
}