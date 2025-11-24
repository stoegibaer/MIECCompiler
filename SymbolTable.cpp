#include "SymbolTable.h"
#include <sstream>
#include <algorithm>

namespace MIEC {

    

    SymbolTable& SymbolTable::GetInstance()
    {
        static SymbolTable instance;
        return instance;
    }

    void SymbolTable::Add(std::unique_ptr<Symbol> sym)
    {
    }

    std::unique_ptr<Symbol> SymbolTable::Find(std::string str)
    {
        return std::unique_ptr<Symbol>();
    }

} // namespace MIEC
