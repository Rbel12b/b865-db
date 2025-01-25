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
    if (symbol.scope.type == Scope::Type::GLOBAL)
    {
        globalScope.symbols.push_back(symbol);
    }
    else if (symbol.scope.type == Scope::Type::FILE)
    {
        checkScopeExists(symbol.scope);
        if (symbol.scope.name.find('.') != std::string::npos)
        {
            funcScope[symbol.scope.name].symbols.push_back(symbol);
        }
        else
        {
            fileScope[symbol.scope.name].symbols.push_back(symbol);
        }
    }
}

void DebuggerData::addFunc(const FunctionRecord &func)
{
    if (func.scope.type == Scope::Type::GLOBAL)
    {
        globalScope.functions.push_back(func);
    }
    else if (func.scope.type == Scope::Type::FILE)
    {
        checkScopeExists(func.scope);
        if (func.scope.name.find('.') != std::string::npos)
        {
            funcScope[func.scope.name].functions.push_back(func);
        }
        else
        {
            fileScope[func.scope.name].functions.push_back(func);
        }
    }
}

void DebuggerData::checkScopeExists(Scope scope)
{
    if (scope.name.find('.') != std::string::npos)
    {
        if (funcScope.count(scope.name) == 0)
        {
            funcScope[scope.name] = ScopeData();
        }
    }
    else
    {
        if (fileScope.count(scope.name) == 0)
        {
            fileScope[scope.name] = ScopeData();
        }
    }
}
