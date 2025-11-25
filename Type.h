#ifndef TYPE_H
#define TYPE_H
#include <iostream>
#include <string>

namespace MIEC {

    class TypeSymbol;

    enum TypeKind {
        eInt,
        eFloat,
        eChar,
        eStruct,
        eArray
    };

    class Type {
    public:
        virtual ~Type() = default;
        virtual int GetSize() = 0;
        virtual TypeKind GetKind() const = 0;

        TypeSymbol* GetTypeSymbol();
        void SetTypeSymbol(TypeSymbol* typeSymbol);

        // Typenkompatibilit�t pr�fen
        bool IsCompatible(Type* other);

    protected:
        TypeSymbol* mTypeSymbol;
    };

    class BaseType : public Type {
    public:
        BaseType(TypeKind kind);
        int GetSize() override;
        TypeKind GetKind() const override;

    private:
        TypeKind mType;
    };

} // namespace MIEC

#endif