#ifndef TYPE_H
#define TYPE_H
#include <iostream>
#include <string>

namespace MIEC {
    class TypeSymbol;

    /**
     * @enum TypeKind
     * @brief Enumerates all supported type kinds in the language.
     */
    enum TypeKind {
        eInt,      ///< Integer type
        eFloat,    ///< Floating-point type
        eChar,     ///< Character type
        eStruct,   ///< Structure/composite type
        eArray     ///< Array type
    };

    /**
     * @class Type
     * @brief Abstract base class representing a type in the type system.
     * 
     * @details This class defines the interface for all types in the language.
     * It provides methods to query type properties, manage associated type symbols,
     * and check type compatibility.
     */
    class Type {
    public:
        /**
         * @brief Virtual destructor.
         */
        virtual ~Type() = default;

        /**
         * @brief Retrieves the size of this type in bytes.
         * 
         * @return int The size of the type.
         */
        virtual int GetSize() = 0;

        /**
         * @brief Retrieves the kind of this type.
         * 
         * @return TypeKind The type kind enumeration value.
         */
        virtual TypeKind GetKind() const = 0;

        /**
         * @brief Retrieves the associated type symbol.
         * 
         * @return TypeSymbol* Pointer to the type symbol, or nullptr if not set.
         */
        TypeSymbol* GetTypeSymbol();

        /**
         * @brief Associates a type symbol with this type.
         * 
         * @param typeSymbol Pointer to the type symbol to associate.
         */
        void SetTypeSymbol(TypeSymbol* typeSymbol);

        /**
         * @brief Checks if this type is compatible with another type.
         * 
         * @param other Pointer to the other type to compare with.
         * @return bool True if the types are compatible, false otherwise.
         */
        bool IsCompatible(Type* other);

    protected:
        /// @brief Pointer to the associated type symbol.
        TypeSymbol* mTypeSymbol;
    };

    /**
     * @class BaseType
     * @brief Represents a built-in base type (int, float, char, etc.).
     * 
     * @details This class implements the Type interface for basic types that
     * are built into the language.
     */
    class BaseType : public Type {
    public:
        /**
         * @brief Constructs a base type with the specified kind.
         * 
         * @param kind The type kind to create.
         */
        BaseType(TypeKind kind);

        /**
         * @brief Retrieves the size of this base type in bytes.
         * 
         * @return int The size of the type.
         */
        int GetSize() override;

        /**
         * @brief Retrieves the kind of this base type.
         * 
         * @return TypeKind The type kind enumeration value.
         */
        TypeKind GetKind() const override;

    private:
        /// @brief The kind of this base type.
        TypeKind mType;
    };
} // namespace MIEC

#endif // TYPE_H