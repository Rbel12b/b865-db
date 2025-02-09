#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <filesystem>
#include "Emulator/Emulator.h"
#include "Parser/Parser.h"
#include "CLI.h"
#include "Breakpoints.h"

Parser parser;
CLI cli;
DebuggerData *data = nullptr;
BreakpointList breakpoints;
Emulator emulator;
std::string binaryFile;
std::string symbolFile;

bool MImode = false;

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

void handleArgs(const std::vector<std::string> &args)
{
    if (std::find(args.begin(), args.end(), "-h") != args.end() ||
        std::find(args.begin(), args.end(), "--help") != args.end() ||
        std::find(args.begin(), args.end(), "-help") != args.end())
    {
        cli.printUsage();
        deallocExit(0);
    }
    if (std::find(args.begin(), args.end(), "--interpreter=mi") != args.end())
    {
        MImode = true;
    }
    if (std::find(args.begin(), args.end(), "--args") != args.end())
    {
        size_t argsIndex = std::find(args.begin(), args.end(), "--args") - args.begin();
        if (argsIndex + 1 < args.size())
        {
            binaryFile = args[argsIndex + 1];
        }
        if (argsIndex + 2 < args.size())
        {
            symbolFile = args[argsIndex + 2];
        }
    }
}

int main(int argc, char *argv[])
{
    cli.usage_str = 
    "Usage: b865-db <options> --args <binary file> <symbol file>\n"
    "options:\n"
    "    --interpreter=mi   - set MI mode\n"
    "    -h -help --help    - display help and quit\n";
    emulator.init();
    emulator.pause();
    emulator.setBreakpoints(breakpoints.addresses);

    // Convert command-line arguments to a vector of strings
    std::vector<std::string> args(argv + 1, argv + argc);

    cli.addCommand("quit", "", true, [](const std::vector<std::string> &args)
                   { cli.quit(); (void)args;}, "Quit the program");
    cli.addCommand("modules", "", true, [](const std::vector<std::string> &args)
                   { printModules(); (void)args; }, "Print modules from the debug file");
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
                        BreakpointList::execPath = std::filesystem::path(args[1]).parent_path().string();
                        emulator.start();
                        emulator.continue_exec(); }, "Start the emulator with the specified file");
    cli.addCommand("stop", "", false, [](const std::vector<std::string> &args)
                   {
                        emulator.stop();
                        printf("Program paused at address 0x%04x\n", emulator.m_cpu.getStatus().PC.addr);
                        if (data != nullptr)
                        {
                            auto line = data->getLine(emulator.m_cpu.getStatus().PC.addr);
                            printf("    at %s:%ld\n", line.filename.c_str(), line.line);
                        } (void)args;}, "Stop the emulator");
    cli.addCommand("continue", "", true, [](const std::vector<std::string> &args)
                   { emulator.continue_exec(); emulator.start(); (void)args; }, "continue the execution of the program");
    handleArgs(args);

    BreakpointList::print = true;
    BreakpointList::execPath = std::filesystem::path(binaryFile).parent_path().string();


    data = parser.parse(symbolFile);
    if (data == nullptr)
    {
        std::cerr << "Error parsing file: " << symbolFile << std::endl;
        std::cerr << "Second argument must be a valid debug file." << std::endl;
        deallocExit(1);
        return 1;
    }

    std::cout << "b865-debugger (type 'quit' or 'q' to exit, 'help' for usage)\n";

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
                printf("Program hit breakpoint %ld at address 0x%04x\n", id, emulator.m_cpu.getStatus().PC.addr);
                emulator.stop();
            }
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
    }

    emulator.terminate();

    deallocExit();

    return 0;
}
