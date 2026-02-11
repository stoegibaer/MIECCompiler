#ifndef SYMBOL_H
#define SYMBOL_H
#include <string>
#include "Type.h"

namespace MIEC {

    class Symbol {
    private:
        std::string mName;
        Type* mType;

    public:
        Symbol(const std::string& name);
        virtual ~Symbol() = default;

        std::string GetName() const;
        Type* GetType() const;
        void SetType(Type* type);
    };

    class ConstSymbol : public Symbol {
    private:
        int mValue;

    public:
        ConstSymbol(const std::string& name, int value, Type* type);
        int GetValue() const;
    };

    class VarSymbol : public Symbol {
    private:
        int mOffset;

    public:
        VarSymbol(const std::string& name, Type* type, int offset);
        int GetOffset() const;
    };

    class TypeSymbol : public Symbol {
    public:
        TypeSymbol(const std::string& name, Type* type);
    };

} // namespace MIEC

#endif