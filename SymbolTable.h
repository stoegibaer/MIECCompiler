#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <stdexcept>
#include <vector>
#include "Symbol.h"
#include "Type.h"

namespace MIEC {

	class SymbolTable
	{
    public:
        static SymbolTable& GetInstance();

        bool AddVar(const std::string& name, int offset);
        Symbol* Find(const std::string& name);
        void Clear();

    private:
        SymbolTable() = default;
        SymbolTable(const SymbolTable&) = delete;
        SymbolTable& operator=(const SymbolTable&) = delete;
        std::unordered_map<std::string, std::unique_ptr<Symbol>> vars;
	};
} // namespace MIEC
