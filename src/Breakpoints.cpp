#include "Breakpoints.h"
#include <filesystem>
#include <algorithm>

int Breakpoint::setPos(std::string &file, size_t& _line, DebuggerData *data)
{
    std::filesystem::path path(file);
    this->file = path.string();

    asmFile = ((path.extension() != ".c") && (path.extension() != ".h"));

    if (asmFile && this->file.find_last_of('.') != std::string::npos)
    {
        this->file.erase(this->file.find_last_of('.'));
    }

    LinkerRecord* previousLineNode = nullptr;
    LinkerRecord* nextLineNode = nullptr;

    for (auto &LineNode : data->globalScope.linkerRecords)
    {
        if (((asmFile && (LineNode.type == LinkerRecord::Type::ASM_LINE)) ||
            (!asmFile && (LineNode.type == LinkerRecord::Type::C_LINE))) &&
            (file == LineNode.name))
        {
            if (LineNode.line < _line)
            {
                previousLineNode = &LineNode;
            }
            else if (nextLineNode == nullptr && LineNode.line >= _line)
            {
                nextLineNode = &LineNode;
                break;
            }
            else if (previousLineNode == nullptr)
            {
                previousLineNode = &LineNode;
            }
        }
    }

    if (nextLineNode != nullptr)
    {
        addr = nextLineNode->addr;
        level = nextLineNode->level;
        block = nextLineNode->block;
        this->line = nextLineNode->line;
    }
    else if (previousLineNode != nullptr)
    {
        addr = previousLineNode->addr;
        level = previousLineNode->level;
        block = previousLineNode->block;
        this->line = previousLineNode->line;
    }
    else
    {
        addr = 0;
        level = 0;
        block = 0;
        this->line = 0;
        printf("No source file named %s\n", file.c_str());
        return 1;
    }
    _line = this->line;
    return 0;
}

uint16_t Breakpoint::getAddr()
{
    return addr;
}

void BreakpointList::addBreakpoint(std::string &file, size_t &line, DebuggerData *data)
{
    Breakpoint bp;
    bp.id = id + 1;
    id++;
    if (bp.setPos(file, line, data))
    {
        id--;
        return;
    }

    std::vector<Breakpoint> matches;
    std::copy_if(breakpoints.begin(), breakpoints.end(), std::back_inserter(matches),
                 [&bp](const Breakpoint& obj) { return obj.addr == bp.addr; });
    if (matches.size())
    {
        printf("Note: breakpoint");
        if (matches.size() == 1)
        {
            printf(" %ld", matches[0].id);
        }
        else if (matches.size() == 2)
        {
            printf("s %ld and %ld", matches[0].id, matches[1].id);
        }
        else
        {
            printf("s ");
            for (size_t i = 0; i < matches.size() - 2; i++)
            {
                printf("%ld, ", matches[i].id);
            }
            printf("%ld and %ld", matches[matches.size() - 2].id, matches[matches.size() - 1].id);
        }
        printf(" also set at pc 0x%02x.\n", bp.addr);
    }
    printf("Breakpoint %ld at 0x%04x: file %s, line %ld.\n", bp.id, bp.addr, file.c_str(), line);
    breakpoints.push_back(bp);
    updateAddresses();
}

void BreakpointList::addBreakpoint(const std::vector<std::string> &args, DebuggerData *data)
{
    if (data == nullptr)
    {
        return;
    }
    std::string file;
    size_t line;
    int i = 2;
    if (args.size() >= 2)
    {
        file = args[1];
        if (file.find_first_of(':') != std::string::npos)
        {
            if (file.find_first_of(':') < file.size() - 1)
            {
                std::string linestr = file.substr(file.find_first_of(':') + 1);
                if (std::isdigit(linestr[0]))
                {
                    line = std::stoi(linestr);
                }
            }
            file = file.substr(0, file.find_first_of(':'));
        }
        else
        {
            line = 0;
        }
    }

    if (args.size() > 2)
    {
        if (args[i] == ":")
        {
            i++;
        }
        if (std::isdigit(args[i][0]))
        {
            line = std::stoi(args[i]);
        }
        else
        {
            line = 0;
        }
    }
    addBreakpoint(file, line, data);
}

void BreakpointList::delBreakpoint(size_t id)
{
    auto it = std::find_if(breakpoints.begin(), breakpoints.end(), [id](const Breakpoint& bp) { return bp.id == id; });
    if (it != breakpoints.end())
    {
        breakpoints.erase(it);
    }
    else
    {
        printf("Error: Breakpoint id %ld not found\n", id);
    }
}

void BreakpointList::delBreakpoint(const std::vector<std::string> &args)
{
    if (args.size() < 2)
    {
        printf("No breakpoint specified\n");
        return;
    }
    else
    {
        for (size_t i = 1; i < args.size(); i++)
        {
            if (std::isdigit(args[i][0]))
            {
                size_t id = std::stoi(args[i]);
                delBreakpoint(id);
            }
            else
            {
                printf("Error: Invalid breakpoint id: %s\n", args[1].c_str());
                break;
            }
        }
    }
    updateAddresses();
}

void BreakpointList::updateAddresses()
{
    addresses.clear();
    for (auto &bp : breakpoints)
    {
        addresses.push_back(bp.addr);
    }
}
