#include "DebuggerData.h"

void DebuggerData::clear()
{
    modules.clear();
}

void DebuggerData::addModule(const std::string &module)
{
    modules.push_back(module);
}

void DebuggerData::addSymbol(const SymbolRecord & symbol)
{
    symbols.push_back(symbol);
}
