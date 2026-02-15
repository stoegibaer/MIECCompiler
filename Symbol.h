#ifndef SYMBOL_H
#define SYMBOL_H
#include <string>
#include "Type.h"

namespace MIEC {
    /**
     * @class Symbol
     * @brief Base class representing a symbol in the symbol table.
     *
     * @details A symbol represents any named entity in the program (variables,
     * constants, types). This is the base class for specific symbol types.
     */
    class Symbol {
    private:
        /// @brief The name of the symbol.
        std::string mName;

        /// @brief Pointer to the type information for this symbol.
        Type* mType;

    public:
        /**
         * @brief Constructs a symbol with the given name.
         *
         * @param name The name of the symbol.
         */
        Symbol(const std::string& name);

        /**
         * @brief Virtual destructor.
         */
        virtual ~Symbol() = default;

        /**
         * @brief Retrieves the name of this symbol.
         *
         * @return std::string The symbol's name.
         */
        std::string GetName() const;

        /**
         * @brief Retrieves the type information for this symbol.
         *
         * @return Type* Pointer to the type object, or nullptr if not set.
         */
        Type* GetType() const;

        /**
         * @brief Sets the type information for this symbol.
         *
         * @param type Pointer to the type object.
         */
        void SetType(Type* type);
    };

    /**
     * @class ConstSymbol
     * @brief Represents a constant symbol in the symbol table.
     *
     * @details A constant symbol is an immutable named value with a fixed integer value.
     */
    class ConstSymbol : public Symbol {
    private:
        /// @brief The constant integer value.
        int mValue;

    public:
        /**
         * @brief Constructs a constant symbol.
         *
         * @param name The name of the constant.
         * @param value The integer value of the constant.
         * @param type Pointer to the type object for this constant.
         */
        ConstSymbol(const std::string& name, int value, Type* type);

        /**
         * @brief Retrieves the constant value.
         *
         * @return int The integer value of this constant.
         */
        int GetValue() const;
    };

    /**
     * @class VarSymbol
     * @brief Represents a variable symbol in the symbol table.
     *
     * @details A variable symbol represents a named storage location with an
     * associated type and memory offset.
     */
    class VarSymbol : public Symbol {
    private:
        /// @brief The offset in memory where this variable is stored.
        int mOffset;

    public:
        /**
         * @brief Constructs a variable symbol.
         *
         * @param name The name of the variable.
         * @param type Pointer to the type object for this variable.
         * @param offset The memory offset of the variable.
         */
        VarSymbol(const std::string& name, Type* type, int offset);

        /**
         * @brief Retrieves the memory offset of this variable.
         *
         * @return int The offset value for this variable.
         */
        int GetOffset() const;
    };

    /**
     * @class TypeSymbol
     * @brief Represents a type symbol in the symbol table.
     *
     * @details A type symbol represents a user-defined type (class or structure)
     * that can be referenced by name in the program.
     */
    class TypeSymbol : public Symbol {
    public:
        /**
         * @brief Constructs a type symbol.
         *
         * @param name The name of the type.
         * @param type Pointer to the type object.
         */
        TypeSymbol(const std::string& name, Type* type);
    };
} // namespace MIEC

#endif // SYMBOL_H