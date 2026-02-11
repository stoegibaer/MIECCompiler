#include "SymbolTable.h"

namespace MIEC {

    SymbolTable::SymbolTable() : mCurrentOffset(0), mIntType(nullptr) {
        Init();
    }

    SymbolTable::~SymbolTable() {
        // Typen müssen manuell gelöscht werden, da sie nicht in unique_ptr gespeichert sind
        delete mIntType;
    }

    void SymbolTable::Init() {
        // Vordefinierten Integer-Typ anlegen
        mIntType = new BaseType(eInt);
        TypeSymbol* intTypeSymbol = new TypeSymbol("Integer", mIntType);
        mIntType->SetTypeSymbol(intTypeSymbol);

        // Type-Symbol in die Tabelle einfügen
        mSymbols["Integer"] = std::unique_ptr<Symbol>(intTypeSymbol);
    }

    bool SymbolTable::AddVar(const std::string& name, Type* type, int offset) {
        // Prüfen ob Name bereits existiert (Doppeldeklaration)
        if (mSymbols.find(name) != mSymbols.end()) {
            return false;
        }

        mSymbols[name] = std::make_unique<VarSymbol>(name, type, offset);
        return true;
    }

    bool SymbolTable::AddConst(const std::string& name, int value, Type* type) {
        // Prüfen ob Name bereits existiert
        if (mSymbols.find(name) != mSymbols.end()) {
            return false;
        }

        mSymbols[name] = std::make_unique<ConstSymbol>(name, value, type);
        return true;
    }

    bool SymbolTable::AddType(const std::string& name, Type* type) {
        // Prüfen ob Name bereits existiert
        if (mSymbols.find(name) != mSymbols.end()) {
            return false;
        }

        mSymbols[name] = std::make_unique<TypeSymbol>(name, type);
        return true;
    }

    Symbol* SymbolTable::Find(const std::string& name) {
        auto it = mSymbols.find(name);
        return it != mSymbols.end() ? it->second.get() : nullptr;
    }

    int SymbolTable::GetCurrentOffset() const {
        return mCurrentOffset;
    }

    void SymbolTable::IncreaseOffset(int size) {
        mCurrentOffset += size;
    }

    void SymbolTable::ResetOffset() {
        mCurrentOffset = 0;
    }

    Type* SymbolTable::GetIntType() {
        return mIntType;
    }

    void SymbolTable::Clear() {
        mSymbols.clear();
        mCurrentOffset = 0;
        Init(); // Vordefinierte Typen wieder hinzufügen
    }

} // namespace MIEC
