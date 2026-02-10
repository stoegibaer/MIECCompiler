#ifndef SYMBOL_H
#define SYMBOL_H
#include <string>

namespace MIEC {

    class Type;

    class Symbol {
    protected:
        std::string mName;
        Type* mType;

    public:
        Symbol(const std::string& name, Type* type);
        virtual ~Symbol() = default;
        std::string GetName() const;
        Type* GetType() const;
    };

    class ConstSymbol : public Symbol {
    private:
        int mValue;
    public:
        ConstSymbol(const std::string& name, Type* type, int value);
        int GetValue() const;
    };

    class VarSymbol : public Symbol {
    private:
        int mOffset;
    public:
        VarSymbol(const std::string& name, Type* type, int offset);
        int GetOffset() const;
    };

    //=========================================================
    class TypeSymbol : public Symbol {
    public:
        TypeSymbol(const std::string& typeName, Type* type);
        Type* GetType() const;
    private:
        std::string mTypeName;
        Type* mType;
    };

}
#endif
