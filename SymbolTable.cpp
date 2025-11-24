#include "SymbolTable.h"
#include <sstream>
#include <algorithm>

namespace MIEC {

    

    SymbolTable& SymbolTable::GetInstance()
    {
        static SymbolTable instance;
        return instance;
    }

    bool SymbolTable::AddVar(const std::string& name, int offset)
    {
        if (vars.find(name) != vars.end()) return false;
        vars[name] = std::make_unique<VarSymbol>(name, offset);
        return true;
    }

    Symbol* SymbolTable::Find(const std::string& name)
    {
        auto it = vars.find(name);
        return it != vars.end() ? it->second.get() : nullptr;
    }
    void SymbolTable::Clear()
    {
        vars.clear();
    }

} // namespace MIEC
