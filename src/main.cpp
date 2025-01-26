#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include "Emulator/Emulator.h"
#include "Parser/Parser.h"
#include "CLI.h"
#include "Breakpoints.h"

Parser parser;
CLI cli;
DebuggerData *data = nullptr;
BreakpointList breakpoints;
Emulator emulator;

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

void dealloc()
{
    if (data != nullptr)
    {
        delete data;
        data = nullptr;
    }
}

int main(int argc, char *argv[])
{
    emulator.init();
    emulator.setBreakpoints(breakpoints.addresses);

    cli.addCommand("quit", "", true, [](const std::vector<std::string> &args)
               { dealloc(); cli.quit(args); }, "Quit the program");
    cli.addCommand("modules", "", true, [](const std::vector<std::string> &args)
                   { printModules(); }, "Print modules from the debug file");
    cli.addCommand("print", "<string>", true, [](const std::vector<std::string> &args)
                   { 
        if (args.size() > 1) { std::cout << args[1] << std::endl; } }, "Print the string");
    cli.addCommand("break", "<position>", true, [](const std::vector<std::string> &args)
                   { breakpoints.addBreakpoint(args, data); },
        "Add a breakpoint at the specified location (file:line or line [in the current file])");
    cli.addCommand("delete", "<id>", true, [](const std::vector<std::string> &args)
                   { breakpoints.delBreakpoint(args); },
        "Delete the breakpoint(s) with the specified id(s)");
    cli.addCommand("run", "<file>", true, [](const std::vector<std::string> &args)
                   {
                        if (args.size() > 1)
                        {
                            emulator.load(args[1]);
                        }
                        emulator.start();
                    }, "Start the emulator with the specified file");
    cli.addCommand("stop", "", false, [](const std::vector<std::string> &args)
                   { emulator.stop(); }, "Stop the emulator");
    cli.addCommand("continue", "", true, [](const std::vector<std::string> &args)
                   { emulator.start(); emulator.continue_exec(); }, "continue the execution of the program");

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

    std::cout << "b865-debugger (type 'quit' or 'q' to exit, 'help' for usage)\n";

    if (help)
    {
        cli.printUsage();
        return 0;
    }

    while(1)
    {
        if (cli.run())
        {
            break;
        }
        if (emulator.paused() && emulator.clockRunning())
        {
            size_t id = breakpoints.breakpoints[emulator.m_cpu.breakpointNum].id;
            printf("Program hit breakpoint %d at address 0x%04x\n", id, emulator.m_cpu.getStatus().PC.addr);
            emulator.stop();
        }
    }

    emulator.terminate();

    dealloc();

    return 0;
}