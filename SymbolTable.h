#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <string>
#include <unordered_map>
#include <memory>
#include "Symbol.h"
#include "Type.h"

namespace MIEC {

    class SymbolTable {
    public:
        SymbolTable();
        ~SymbolTable();

        // Initialisierung der vordefinierten Typen
        void Init();

        // Symbole hinzufügen
        bool AddVar(const std::string& name, Type* type, int offset);
        bool AddConst(const std::string& name, int value, Type* type);
        bool AddType(const std::string& name, Type* type);

        // Symbol suchen
        Symbol* Find(const std::string& name);

        // Offset-Verwaltung
        int GetCurrentOffset() const;
        void IncreaseOffset(int size);
        void ResetOffset();

        // Vordefinierte Typen abrufen
        Type* GetIntType();

        // Tabelle leeren
        void Clear();

    private:
        std::unordered_map<std::string, std::unique_ptr<Symbol>> mSymbols;
        int mCurrentOffset;

        // Vordefinierte Typen
        Type* mIntType;
    };

} // namespace MIEC

#endif