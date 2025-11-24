#ifndef SYMBOL_H
#define SYMBOL_H
#include <string>
#include "Type.h"

namespace MIEC {


class Symbol {
private:
    std::string mName;
    std::unique_ptr<Type> mType;

public:
    Symbol(const std::string& name);

    virtual ~Symbol() = default;

    std::string GetName() const;

    virtual Type GetType() const;
};

class ConstSymbol : public Symbol {
private:
    int mValue;
public:
    int GetValue();
};

class VarSymbol : public Symbol {
private:
    int mOffset;
public:
    int GetOffset();
};

class TypeSymbol : public Symbol {

};



}
#endif
