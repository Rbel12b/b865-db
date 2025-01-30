#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
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

void deallocExit(int code = 0)
{
    if (data != nullptr)
    {
        delete data;
        data = nullptr;
    }
    emulator.terminate();
    emulator.stop();
    exit(code);
}

int main(int argc, char *argv[])
{
    emulator.init();
    emulator.pause();
    emulator.setBreakpoints(breakpoints.addresses);

    cli.addCommand("quit", "", true, [](const std::vector<std::string> &args)
                   { deallocExit(); cli.quit(args); }, "Quit the program");
    cli.addCommand("modules", "", true, [](const std::vector<std::string> &args)
                   { printModules(); }, "Print modules from the debug file");
    cli.addCommand("print", "<string>", true, [](const std::vector<std::string> &args)
                   { if (args.size() > 1) { std::cout << args[1] << std::endl; } }, "Print the string");
    cli.addCommand("break", "<position>", true, [](const std::vector<std::string> &args)
                   { breakpoints.addBreakpoint(args, data); }, "Add a breakpoint at the specified location (file:line or line [in the current file])");
    cli.addCommand("delete", "<id>", true, [](const std::vector<std::string> &args)
                   { breakpoints.delBreakpoint(args); }, "Delete the breakpoint(s) with the specified id(s)");
    cli.addCommand("run", "<file>", true, [](const std::vector<std::string> &args)
                   {
                        if (args.size() > 1)
                        {
                            if (emulator.load(args[1]))
                            {
                                return;
                            }
                        }
                        emulator.start();
                        emulator.continue_exec(); }, "Start the emulator with the specified file");
    cli.addCommand("stop", "", false, [](const std::vector<std::string> &args)
                   {
                        emulator.stop();
                        printf("Program paused at address 0x%04x\n", emulator.m_cpu.getStatus().PC.addr);
                        if (data != nullptr)
                        {
                            auto line = data->getLine(emulator.m_cpu.getStatus().PC.addr);
                            printf("    at %s:%d\n", line.filename.c_str(), line.line);
                        } }, "Stop the emulator");
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
            deallocExit(1);
            return 1;
        }
    }
    else
    {
        std::cout << "No input file specified.\n";
        cli.printUsage();
        deallocExit(help == true ? 0 : 1);
        return 1;
    }

    std::cout << "b865-debugger (type 'quit' or 'q' to exit, 'help' for usage)\n";

    if (help)
    {
        cli.printUsage();
        deallocExit();
        return 0;
    }

    while (1)
    {
        if (!emulator.clockRunning() && cli.run())
        {
            break;
        }
        if (emulator.clockRunning())
        {
            if (emulator.pausedAtBreakpoint())
            {
                size_t id = breakpoints.breakpoints[emulator.m_cpu.breakpointNum].id;
                printf("Program hit breakpoint %d at address 0x%04x\n", id, emulator.m_cpu.getStatus().PC.addr);
                emulator.stop();
            }
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
    }

    emulator.terminate();

    deallocExit();

    return 0;
}
