#pragma once
#ifndef _CDB_PARSER_H_
#define _CDB_PARSER_H_

#include <string>
#include <vector>
#include <filesystem>
#include "DebuggerData.h"
#include <sstream>

class Token
{
public:
    enum class Type
    {
        Unknown,
        Text,
        LeftBracket,
        RightBracket,
        LineEnd,
    };
    Type type;
    std::string value;
};

class cdbParser
{
private:
    std::filesystem::path cdbFilename;
    
public:
    void init(std::filesystem::path filename);
    DebuggerData* parse();

private:
    // record parsers
    void parseModule(std::vector<Token>& tokens, size_t& i, DebuggerData& data);
    FunctionRecord parseFunction(std::vector<Token>& tokens, size_t& i);
    SymbolRecord parseSymbol(std::vector<Token>& tokens, size_t& i);
    TypeRecord parseType(std::vector<Token>& tokens, size_t& i);
    LinkerRecord parseLinker(std::vector<Token>& tokens, size_t& i);

    // parser helpers
    TypeChainRecord parseTypeChain(std::vector<Token>& tokens, size_t& i);
    void parseScopeNameLevelBlock(std::vector<Token>& tokens, size_t& i, ScopeNameLevelBlock& data);

    TypeChainRecord::Type::DCLType getDCLType(Token token);
    REG getReg(Token token);

    std::vector<Token> tokenize(const std::string& line);
};
#endif