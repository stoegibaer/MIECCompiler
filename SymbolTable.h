#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <stdexcept>

namespace MIEC {

    // forward
    struct Type;
    struct Symbol;

    using TypePtr = std::shared_ptr<Type>;
    using SymbolPtr = std::shared_ptr<Symbol>;

    //
    // Type
    //
    struct Type {
        std::wstring name;
        int size; // bytes

        explicit Type(const std::wstring& n = L"Unknown", int s = 0) : name(n), size(s) {}
    };

    //
    // Symbol base
    //
    struct Symbol {
        std::wstring name;
        TypePtr type;
        bool isConstant = false;

        Symbol(const std::wstring& n, TypePtr t) : name(n), type(t) {}
        virtual ~Symbol() = default;
    };

    //
    // Variable symbol (stores offset)
    //
    struct VarSymbol : Symbol {
        int offset = 0; // byte offset in data segment
        VarSymbol(const std::wstring& n, TypePtr t) : Symbol(n, t) {}
    };

    //
    // Constant symbol
    //
    struct ConstSymbol : Symbol {
        long long intValue = 0;
        bool isInteger = true;
        ConstSymbol(const std::wstring& n, TypePtr t, long long v)
            : Symbol(n, t), intValue(v) {
            isConstant = true;
        }
    };

    //
    // SymbolTable
    //
    class SymbolTable {
    public:
        SymbolTable();
        ~SymbolTable() = default;

        // add variable, throws on redeclaration
        void addVariable(const std::wstring& name, TypePtr type);

        // add constant (literal), id will be generated or keyed by literal string
        SymbolPtr addConstantLiteral(const std::wstring& literal, long long value, TypePtr type);

        // lookup symbol (returns nullptr if not found)
        SymbolPtr lookup(const std::wstring& name) const;

        // check if a name exists
        bool exists(const std::wstring& name) const;

        // compute offsets for all variables (simple linear allocation)
        // startOffset default 0, incremented by type->size
        void computeOffsets(int startOffset = 0);

        // helper: pre-defined type accessors
        TypePtr getIntegerType() const { return integerType; }

        // for debugging / listing
        std::vector<SymbolPtr> allSymbols() const;

    private:
        std::unordered_map<std::wstring, SymbolPtr> table;
        TypePtr integerType;
    };
} // namespace MIEC
