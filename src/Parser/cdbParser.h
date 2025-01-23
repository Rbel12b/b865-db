#pragma once
#ifndef _CDB_PARSER_H_
#define _CDB_PARSER_H_

#include <string>
#include <vector>
#include <filesystem>
#include "DebuggerData.h"
#include <sstream>

class cdbParser
{
private:
    std::filesystem::path cdbFilename;
    
public:
    void init(std::filesystem::path filename);
    DebuggerData* parse();

private:
    // record parsers
    void parseModule(std::istringstream& line, DebuggerData& data);
    void parseFunction(std::istringstream& line, DebuggerData& data);
    void parseSymbol(std::istringstream& line, DebuggerData& data);
    void parseStructure(std::istringstream& line, DebuggerData& data);
    void parseLinker(std::istringstream& line, DebuggerData& data);

    // parser helpers
    TypeChainRecord parseTypeChain(std::istringstream& line);

    TypeChainRecord::Type::DLCType getDLCType(char c1, char c2);
};
#endif