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
};

class ConstSymbol : public Symbol {
private:
    int mValue;
public:
    ConstSymbol(const std::string& name, int value);
    int GetValue() const;
};

class VarSymbol : public Symbol {
private:
    int mOffset;
public:
    VarSymbol(const std::string& n, int off);
    int GetOffset() const;
};

class TypeSymbol : public Symbol {
public:
    TypeSymbol(const std::string& name);
};



}
#endif
