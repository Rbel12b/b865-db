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
    else
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
    else
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

void DebuggerData::addType(const TypeRecord &type)
{
    if (type.scope.type == Scope::Type::GLOBAL)
    {
        globalScope.types.push_back(type);
    }
    else
    {
        checkScopeExists(type.scope);
        if (type.scope.name.find('.') != std::string::npos)
        {
            funcScope[type.scope.name].types.push_back(type);
        }
        else
        {
            fileScope[type.scope.name].types.push_back(type);
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
