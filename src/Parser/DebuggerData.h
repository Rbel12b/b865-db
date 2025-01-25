#pragma once
#ifndef _DEBUGGERDATA_H_
#define _DEBUGGERDATA_H_

#include <string>
#include <vector>
#include <cstdint>

enum class REG
{
    A_IDX,
    X_IDX,
    Y_IDX,
    SP_IDX,
    R0_IDX,
    R1_IDX,
    R2_IDX,
    R3_IDX,
};

class TypeChainRecord
{
public:
    int size = 0;
    class Type
    {
    public:
        enum class DCLType
        {
            ARRAY,
            FUNCTION,
            GEN_POINTER,
            CODE_POINTER,
            EXT_RAM_POINTER,
            INT_RAM_POINTER,
            PAGED_POINTER,
            UPPER128_POINTER,
            LONG,
            INT,
            CHAR,
            SHORT,
            VOID,
            FLOAT,
            STRUCT,
            SBIT,
            BITFIELD,
            UNKNOWN,
        };
        DCLType DCLtype = DCLType::UNKNOWN;
        union
        {
            int n;
            struct
            {
                char offset;
                char size;
            } bitField;
        } num;
        std::string name;
    };
    std::vector<Type> types;
    bool sign = false;
};

class Scope
{
public:
    enum class Type
    {
        GLOBAL = 'G',
        FUNCTION = 'F',
        FILE = 'L',
        UNKNOWN,
    };
    Type type = Type::UNKNOWN;
    std::string name;
};

enum class AddressSpace
{
    EXT_STACK = 'A',
    INT_STACK = 'B',
    CODE = 'C',
    CODE_STATIC = 'D',
    INT_RAM_LOW = 'E',
    EXT_RAM = 'F',
    INT_RAM = 'G',
    BIT_ADR = 'H',
    SFR_SPACE = 'I',
    SBIT_SPACE = 'J',
    REGISTER_SPACE = 'R',
    FUNCTION_UNDEF = 'Z',
    UNKNOWN,
};

class SymbolRecord
{
public:
    Scope scope;
    std::string name;
    uint64_t level = 0;
    uint64_t block = 0;
    TypeChainRecord typeChain;
    AddressSpace addressSpace = AddressSpace::UNKNOWN;
    bool onStack = false;
    int stack_offs = 0;
    std::vector<REG> registers;
};


class DebuggerData
{
public:
    std::vector<std::string> modules;
    std::vector<SymbolRecord> symbols;

public:
    void clear();
    void addModule(const std::string& module);
    void addSymbol(const SymbolRecord& symbol);
};

#endif