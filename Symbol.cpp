#include "Symbol.h"

namespace MIEC {

    Symbol::Symbol(const std::string& name) : mName(name), mType(nullptr) {
    }

    std::string Symbol::GetName() const {
        return mName;
    }

    Type* Symbol::GetType() const {
        return mType;
    }

    void Symbol::SetType(Type* type) {
        mType = type;
    }

    // ConstSymbol
    ConstSymbol::ConstSymbol(const std::string& name, int value, Type* type)
        : Symbol(name), mValue(value) {
        SetType(type);
    }

    int ConstSymbol::GetValue() const {
        return mValue;
    }

    // VarSymbol
    VarSymbol::VarSymbol(const std::string& name, Type* type, int offset)
        : Symbol(name), mOffset(offset) {
        SetType(type);
    }

    int VarSymbol::GetOffset() const {
        return mOffset;
    }

    // TypeSymbol
    TypeSymbol::TypeSymbol(const std::string& name, Type* type)
        : Symbol(name) {
        SetType(type);
    }

} // namespace MIEC