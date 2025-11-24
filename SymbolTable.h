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

        void Add(std::unique_ptr<Symbol> sym);
        std::unique_ptr<Symbol> Find(std::string str);
        void Delete(/*todo*/);

    private:
        SymbolTable() = default;
        SymbolTable(const SymbolTable&) = delete;
        SymbolTable& operator=(const SymbolTable&) = delete;
        std::vector<std::unique_ptr<Symbol>> symbols;
	};
} // namespace MIEC
