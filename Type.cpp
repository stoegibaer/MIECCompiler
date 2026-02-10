#include "Type.h"

namespace MIEC {

    TypeSymbol* Type::GetTypeSymbol() {
        return mTypeSymbol;
    }

    void Type::SetTypeSymbol(TypeSymbol* typeSymbol) {
        mTypeSymbol = typeSymbol;
    }

    bool Type::IsCompatible(Type* other) {
        if (other == nullptr) return false;
        return this->GetKind() == other->GetKind();
    }

    // BaseType
    BaseType::BaseType(TypeKind kind) : mType(kind) {
        mTypeSymbol = nullptr;
    }

    int BaseType::GetSize() {
        switch (mType) {
        case eInt:
            return sizeof(int);      // typischerweise 4 Bytes
        case eFloat:
            return sizeof(float);    // typischerweise 4 Bytes
        case eChar:
            return sizeof(char);     // 1 Byte
        default:
            return 0;
        }
    }

    TypeKind BaseType::GetKind() const {
        return mType;
    }

} // namespace MIEC