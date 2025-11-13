#include "SymbolTable.h"
#include <sstream>
#include <algorithm>

namespace MIEC {

    SymbolTable::SymbolTable() {
        // create primitive integer type (size 4 bytes)
        integerType = std::make_shared<Type>(L"Integer", 4);
    }

    void SymbolTable::addVariable(const std::wstring& name, TypePtr type) {
        if (table.find(name) != table.end()) {
            throw std::runtime_error("Doppeldeklaration: Variable '" + std::string(name.begin(), name.end()) + "'");
        }
        auto vs = std::make_shared<VarSymbol>(name, type);
        table[name] = vs;
    }

    SymbolPtr SymbolTable::addConstantLiteral(const std::wstring& literal, long long value, TypePtr type) {
        // use literal as key, but prefix to avoid clashes with user names
        std::wstring key = L"@const_" + literal;
        auto it = table.find(key);
        if (it != table.end()) return it->second;
        auto cs = std::make_shared<ConstSymbol>(key, type, value);
        table[key] = cs;
        return cs;
    }

    SymbolPtr SymbolTable::lookup(const std::wstring& name) const {
        auto it = table.find(name);
        if (it != table.end()) return it->second;
        return nullptr;
    }

    bool SymbolTable::exists(const std::wstring& name) const {
        return lookup(name) != nullptr;
    }

    void SymbolTable::computeOffsets(int startOffset) {
        int offset = startOffset;
        // stable order: iterate through table and assign offsets to VarSymbols
        // To be deterministic we collect var symbols separately
        std::vector<std::shared_ptr<VarSymbol>> vars;
        for (auto& p : table) {
            auto v = std::dynamic_pointer_cast<VarSymbol>(p.second);
            if (v) vars.push_back(v);
        }
        // sort by name for deterministic layout (optional)
        std::sort(vars.begin(), vars.end(), [](const std::shared_ptr<VarSymbol>& a, const std::shared_ptr<VarSymbol>& b) {
            return a->name < b->name;
            });
        for (auto& v : vars) {
            v->offset = offset;
            offset += (v->type ? v->type->size : 0);
        }
    }

    std::vector<SymbolPtr> SymbolTable::allSymbols() const {
        std::vector<SymbolPtr> res;
        res.reserve(table.size());
        for (auto& p : table) res.push_back(p.second);
        return res;
    }

} // namespace MIEC
