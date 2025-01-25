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
        if(linestr.size() < 3)
        {
            continue;
        }
        char type = linestr[0];

        std::vector<Token> tokens = tokenize(linestr.substr(2));

        switch (type)
        {
        case 'M':
            parseModule(tokens, data);
            break;

        case 'F':
            parseFunction(tokens, data);
            break;

        case 'S':
            parseSymbol(tokens, data);
            break;

        case 'T':
            parseStructure(tokens, data);
            break;

        case 'L':
            parseLinker(tokens, data);
            break;
        
        default:
            printf("Unknown type: %c\n", type);
            break;
        }
    }

    return ptrdata;
}

void cdbParser::parseModule(std::vector<Token>& tokens, DebuggerData &data)
{
    if (tokens.size() < 1)
    {
        return;
    }
    data.addModule(tokens[0].value);
}

void cdbParser::parseFunction(std::vector<Token>& tokens, DebuggerData &data)
{
}

void cdbParser::parseSymbol(std::vector<Token>& tokens, DebuggerData &data)
{
    SymbolRecord symbol;
    if (tokens.size() < 12)
    {
        return;
    }
    size_t i = 0;
    symbol.scope.type = (Scope::Type)tokens[i++].value[0];
    if (symbol.scope.type != Scope::Type::GLOBAL)
    {
        symbol.scope.name = tokens[i - 1].value.substr(1);
    }
    symbol.name = tokens[i++].value;
    symbol.level = std::stoi(tokens[i++].value);
    symbol.block = std::stoi(tokens[i++].value);
    symbol.typeChain = parseTypeChain(tokens, i);
    symbol.addressSpace = (AddressSpace)tokens[i++].value[0];
    symbol.onStack = (tokens[i++].value[0] != '0');
    symbol.stack_offs = std::stoi(tokens[i++].value);
    if (tokens[i].type != Token::Type::LineEnd)
    {
        while (tokens[i].type != Token::Type::LineEnd)
        {
            if (tokens[i].type == Token::Type::LeftBracket || tokens[i].type == Token::Type::RightBracket)
            {
                i++;
                continue;
            }
            symbol.registers.push_back(getReg(tokens[i++]));
        }
    }
    data.addSymbol(symbol);
}

void cdbParser::parseStructure(std::vector<Token>& tokens, DebuggerData &data)
{
}

void cdbParser::parseLinker(std::vector<Token>& tokens, DebuggerData &data)
{
}

TypeChainRecord cdbParser::parseTypeChain(std::vector<Token>& tokens, size_t& i)
{
    TypeChainRecord typeChain;
    if (tokens.size() < 2)
    {
        return typeChain;
    }
    if (tokens[i].type == Token::Type::LeftBracket)
    {
        i++;
    }
    if (tokens[i].type == Token::Type::LeftBracket)
    {
        i++;
    }
    typeChain.size = std::stoi(tokens[i++].value);
    if (tokens[i].type == Token::Type::RightBracket)
    {
        i++;
    }
    size_t DCLTypeNum;
    while (1)
    {
        Token& token = tokens[i];
        if (token.value.size() < 2)
        {
            typeChain.sign = (token.value == "S");
            i++;
            break;
        }
        else
        {
            TypeChainRecord::Type type;
            type.DCLtype = getDCLType(token);
            if (type.DCLtype == TypeChainRecord::Type::DCLType::STRUCT)
            {
                type.name = token.value.substr(2);
            }
            else if (type.DCLtype == TypeChainRecord::Type::DCLType::ARRAY)
            {
                type.num.n = strtoull(token.value.substr(2).c_str(), nullptr, 10);
            }
            else if (type.DCLtype == TypeChainRecord::Type::DCLType::BITFIELD)
            {
                type.num.bitField.offset = strtoull(token.value.substr(2).c_str(), nullptr, 10);
                type.num.bitField.size = strtoull(token.value.substr(token.value.find_first_of('$') + 1).c_str(), nullptr, 10);
            }
            typeChain.types.push_back(type);
        }
        i++;
    }
    while (tokens[i].type == Token::Type::RightBracket)
    {
        i++;
    }
    return typeChain;
}

REG cdbParser::getReg(Token token)
{ 
    std::string reg = token.value;
    if (reg == "a")
    {
        return REG::A_IDX;
    }
    else if (reg == "x")
    {
        return REG::X_IDX;
    }
    else if (reg == "y")
    {
        return REG::Y_IDX;
    }
    else if (reg == "sp")
    {
        return REG::SP_IDX;
    }
    else if (reg == "r0")
    {
        return REG::R0_IDX;
    }
    else if (reg == "r1")
    {
        return REG::R1_IDX;
    }
    else if (reg == "r2")
    {
        return REG::R2_IDX;
    }
    else if (reg == "r3")
    {
        return REG::R3_IDX;
    }
    return REG::R0_IDX;
}

TypeChainRecord::Type::DCLType cdbParser::getDCLType(Token token)
{
    char c1 = token.value[0];
    char c2 = token.value[1];
    if (c1 == 'D')
    {
        if (c2 == 'A')
        {
            return TypeChainRecord::Type::DCLType::ARRAY;
        }
        else if (c2 == 'F')
        {
            return TypeChainRecord::Type::DCLType::FUNCTION;
        }
        else if (c2 == 'G')
        {
            return TypeChainRecord::Type::DCLType::GEN_POINTER;
        }
        else if (c2 == 'C')
        {
            return TypeChainRecord::Type::DCLType::CODE_POINTER;
        }
        else if (c2 == 'X')
        {
            return TypeChainRecord::Type::DCLType::EXT_RAM_POINTER;
        }
        else if (c2 == 'D')
        {
            return TypeChainRecord::Type::DCLType::INT_RAM_POINTER;
        }
        else if (c2 == 'P')
        {
            return TypeChainRecord::Type::DCLType::PAGED_POINTER;
        }
        else if (c2 == 'I')
        {
            return TypeChainRecord::Type::DCLType::UPPER128_POINTER;
        }
    }
    else
    {
        if (c2 == 'L')
        {
            return TypeChainRecord::Type::DCLType::LONG;
        }
        else if (c2 == 'I')
        {
            return TypeChainRecord::Type::DCLType::INT;
        }
        else if (c2 == 'C')
        {
            return TypeChainRecord::Type::DCLType::CHAR;
        }
        else if (c2 == 'S')
        {
            return TypeChainRecord::Type::DCLType::SHORT;
        }
        else if (c2 == 'V')
        {
            return TypeChainRecord::Type::DCLType::VOID;
        }
        else if (c2 == 'F')
        {
            return TypeChainRecord::Type::DCLType::FLOAT;
        }
        else if (c2 == 'T')
        {
            return TypeChainRecord::Type::DCLType::STRUCT;
        }
        else if (c2 == 'X')
        {
            return TypeChainRecord::Type::DCLType::SBIT;
        }
        else if (c2 == 'B')
        {
            return TypeChainRecord::Type::DCLType::BITFIELD;
        }
    }
    return TypeChainRecord::Type::DCLType::UNKNOWN;
}

std::vector<Token> cdbParser::tokenize(const std::string &line)
{
    std::vector<Token> tokens;
    

    for (size_t i = 0; i < line.size(); i++)
    {
        char c = line[i];

        if (c == '(' || c == '['|| c == '{')
        {
            tokens.push_back(Token{Token::Type::LeftBracket});
        }
        else if (c == ')' || c == ']' || c == '}' )
        {
            tokens.push_back(Token{Token::Type::RightBracket});
        }
        else if (c == ':', c == ',', c == '$')
        {
            continue;
        }
        else
        {
            size_t start = i;
            while (i < line.size() && line[i] != '(' && line[i] != '[' && line[i] != '{' &&
                line[i] != ')' && line[i] != ']' && line[i] != '}' && line[i] != ':' &&
                line[i] != ',' && line[i] != '$')
            {
                i++;
            }
            if (i > start)
            {
                tokens.push_back(Token{Token::Type::Text, line.substr(start, i - start)});
                i--;
            }
        }
    }
    tokens.push_back(Token{Token::Type::LineEnd});
    return tokens;
}
