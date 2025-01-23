#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include "cdbParser.h"

void cdbParser::init(std::filesystem::path filename)
{
    cdbFilename = filename;
}

DebuggerData *cdbParser::parse()
{
    std::ifstream file(cdbFilename.string());
    if (!file.is_open())
    {
        return nullptr;
    }
    DebuggerData *ptrdata = new DebuggerData();
    DebuggerData& data = *ptrdata;
    
    std::ostringstream buffer;
    buffer << file.rdbuf();
    std::istringstream iss(buffer.str());
    std::string linestr;
    while (std::getline(iss, linestr))
    {
        std::istringstream line(linestr);
        char type = line.get();
        line.get();
        if(line.bad())
        {
            continue;
        }
        switch (type)
        {
        case 'M':
            parseModule(line, data);
            break;

        case 'F':
            parseFunction(line, data);
            break;

        case 'S':
            parseSymbol(line, data);
            break;

        case 'T':
            parseStructure(line, data);
            break;

        case 'L':
            parseLinker(line, data);
            break;
        
        default:
            printf("Unknown type: %c\n", type);
            break;
        }
    }

    return ptrdata;
}

void cdbParser::parseModule(std::istringstream &line, DebuggerData &data)
{
    std::string moduleName;
    std::getline(line, moduleName);
    data.addModule(moduleName);
}

void cdbParser::parseFunction(std::istringstream &line, DebuggerData &data)
{
}

void cdbParser::parseSymbol(std::istringstream &line, DebuggerData &data)
{
    SymbolRecord symbol;
    symbol.scope.type = (Scope::Type)line.get();
    std::string str;
    if (symbol.scope.type != Scope::Type::GLOBAL)
    {
        std::getline(line, str, '$');
        symbol.scope.name = str;
    }
    if (line.peek() == '$')
    {
        line.ignore();
    }
    std::getline(line, str, '$');
    symbol.name = str;
    if (line.peek() == '$')
    {
        line.ignore();
    }
    std::getline(line, str, '$');
    symbol.level = std::stoi(str.substr(0,str.find_first_of('_')));
    if (line.peek() == '$')
    {
        line.ignore();
    }
    std::getline(line, str, '(');
    symbol.block = std::stoi(str);

    symbol.typeChain = parseTypeChain(line);
    if (line.peek() == ',')
    {
        line.ignore();
    }
    symbol.addressSpace = (AddressSpace)line.get();
    line.ignore();
    symbol.onStack = (line.get() != '0');
    line.ignore();
    std::getline(line, str, ',');
    symbol.stack_offs = std::stoi(str);
    if (line.peek() == ',')
    {
        line.ignore();
        if (line.get() == '[')
        {
            do
            {
                if (line.peek() == ',')
                {
                    line.ignore();
                }
                std::string reg;
                std::getline(line, reg, ',');
                if (reg == "a")
                {
                    symbol.registers.push_back(REG::A_IDX);
                }
                else if (reg == "x")
                {
                    symbol.registers.push_back(REG::X_IDX);
                }
                else if (reg == "y")
                {
                    symbol.registers.push_back(REG::Y_IDX);
                }
                else if (reg == "sp")
                {
                    symbol.registers.push_back(REG::SP_IDX);
                }
                else if (reg == "r0")
                {
                    symbol.registers.push_back(REG::R0_IDX);
                }
                else if (reg == "r1")
                {
                    symbol.registers.push_back(REG::R1_IDX);
                }
                else if (reg == "r2")
                {
                    symbol.registers.push_back(REG::R2_IDX);
                }
                else if (reg == "r3")
                {
                    symbol.registers.push_back(REG::R3_IDX);
                }
            } while (line.peek() == ',');
        }
    }
    data.addSymbol(symbol);
}

void cdbParser::parseStructure(std::istringstream &line, DebuggerData &data)
{
}

void cdbParser::parseLinker(std::istringstream &line, DebuggerData &data)
{
}

TypeChainRecord cdbParser::parseTypeChain(std::istringstream &line)
{
    TypeChainRecord typeChain;
    if (line.peek() == '(')
    {
        line.ignore();
    }
    if (line.get() == '{')
    {
        line >> typeChain.size;
        line.ignore();
    }
    bool last;
    int i = 0;
    std::string type;
    std::getline(line, type, ',');
    if (type[type.size() - 1] == ')')
    {
        last = true;
    }
    char c1 = type[0];
    char c2 = type[1];
    i += 2;
    typeChain.type.type = getDLCType(c1, c2);
    if (typeChain.type.type == TypeChainRecord::Type::DLCType::STRUCT)
    {
        typeChain.type.name = type.substr(i, type.find_first_of(":,") - i);
    }
    else if (typeChain.type.type == TypeChainRecord::Type::DLCType::ARRAY ||
        typeChain.type.type == TypeChainRecord::Type::DLCType::BITFIELD)
    {
        typeChain.type.n = strtoull(type.substr(i, type.find_first_of(":,") - i).c_str(), nullptr, 10);
    }
    if (last)
    {
        typeChain.sign = (type.substr(type.find_first_of(":,") + 1, 1) == "S");
    }
    else
    {
        if (line.peek() == ',')
        {
            line.ignore();
        }
        std::getline(line, type, ',');
        c1 = type[0];
        c2 = type[1];
        typeChain.type2.type = getDLCType(c1, c2);
        if (typeChain.type2.type == TypeChainRecord::Type::DLCType::STRUCT)
        {
            typeChain.type2.name = type.substr(i, type.find_first_of(":,") - i);
        }
        else if (typeChain.type2.type == TypeChainRecord::Type::DLCType::ARRAY ||
            typeChain.type2.type == TypeChainRecord::Type::DLCType::BITFIELD)
        {
            typeChain.type2.n = strtoull(type.substr(i, type.find_first_of(":,") - i).c_str(), nullptr, 10);
        }
        typeChain.sign = (type.substr(type.find_first_of(":,") + 1, 1) == "S");
    }
    if (line.peek() == ')')
    {
        line.ignore();
    }
    return typeChain;
}

TypeChainRecord::Type::DLCType cdbParser::getDLCType(char c1, char c2)
{
    if (c1 == 'D')
    {
        if (c2 == 'A')
        {
            return TypeChainRecord::Type::DLCType::ARRAY;
        }
        else if (c2 == 'F')
        {
            return TypeChainRecord::Type::DLCType::FUNCTION;
        }
        else if (c2 == 'G')
        {
            return TypeChainRecord::Type::DLCType::GEN_POINTER;
        }
        else if (c2 == 'C')
        {
            return TypeChainRecord::Type::DLCType::CODE_POINTER;
        }
        else if (c2 == 'X')
        {
            return TypeChainRecord::Type::DLCType::EXT_RAM_POINTER;
        }
        else if (c2 == 'D')
        {
            return TypeChainRecord::Type::DLCType::INT_RAM_POINTER;
        }
        else if (c2 == 'P')
        {
            return TypeChainRecord::Type::DLCType::PAGED_POINTER;
        }
        else if (c2 == 'I')
        {
            return TypeChainRecord::Type::DLCType::UPPER128_POINTER;
        }
    }
    else
    {
        if (c2 == 'L')
        {
            return TypeChainRecord::Type::DLCType::LONG;
        }
        else if (c2 == 'I')
        {
            return TypeChainRecord::Type::DLCType::INT;
        }
        else if (c2 == 'C')
        {
            return TypeChainRecord::Type::DLCType::CHAR;
        }
        else if (c2 == 'S')
        {
            return TypeChainRecord::Type::DLCType::SHORT;
        }
        else if (c2 == 'V')
        {
            return TypeChainRecord::Type::DLCType::VOID;
        }
        else if (c2 == 'F')
        {
            return TypeChainRecord::Type::DLCType::FLOAT;
        }
        else if (c2 == 'T')
        {
            return TypeChainRecord::Type::DLCType::STRUCT;
        }
        else if (c2 == 'X')
        {
            return TypeChainRecord::Type::DLCType::SBIT;
        }
        else if (c2 == 'B')
        {
            return TypeChainRecord::Type::DLCType::BITFIELD;
        }
    }
    return TypeChainRecord::Type::DLCType::UNKNOWN;
}
