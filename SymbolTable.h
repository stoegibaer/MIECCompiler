#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include <string>
#include <unordered_map>
#include <memory>
#include "Symbol.h"
#include "Type.h"

namespace MIEC {
    /**
     * @class SymbolTable
     * @brief Manages all symbols in the compilation context.
     *
     * @details The symbol table stores and manages all named entities including
     * variables, constants, and types. It handles symbol lookup, offset management
     * for memory allocation, and initialization of predefined types.
     */
    class SymbolTable {
    public:
        /**
         * @brief Constructs a SymbolTable instance.
         */
        SymbolTable();

        /**
         * @brief Destructs the SymbolTable and releases all managed symbols.
         */
        ~SymbolTable();

        /**
         * @brief Initializes the symbol table with predefined types.
         *
         * @details This method must be called before using the symbol table
         * to set up built-in types.
         */
        void Init();

        /**
         * @brief Adds a variable symbol to the symbol table.
         *
         * @param name The name of the variable.
         * @param type Pointer to the type object for the variable.
         * @param offset The memory offset allocated for this variable.
         * @return bool True if the variable was successfully added, false if
         * a symbol with this name already exists.
         */
        bool AddVar(const std::string& name, Type* type, int offset);

        /**
         * @brief Adds a constant symbol to the symbol table.
         *
         * @param name The name of the constant.
         * @param value The integer value of the constant.
         * @param type Pointer to the type object for the constant.
         * @return bool True if the constant was successfully added, false if
         * a symbol with this name already exists.
         */
        bool AddConst(const std::string& name, int value, Type* type);

        /**
         * @brief Adds a type symbol to the symbol table.
         *
         * @param name The name of the type.
         * @param type Pointer to the type object.
         * @return bool True if the type was successfully added, false if
         * a symbol with this name already exists.
         */
        bool AddType(const std::string& name, Type* type);

        /**
         * @brief Looks up a symbol by name.
         *
         * @param name The name of the symbol to find.
         * @return Symbol* Pointer to the symbol if found, nullptr otherwise.
         */
        Symbol* Find(const std::string& name);

        /**
         * @brief Retrieves the current memory offset.
         *
         * @return int The current offset value for memory allocation.
         */
        int GetCurrentOffset() const;

        /**
         * @brief Increases the current memory offset.
         *
         * @param size The amount to increase the offset by.
         */
        void IncreaseOffset(int size);

        /**
         * @brief Resets the memory offset to zero.
         *
         * Used when entering a new scope or function context.
         */
        void ResetOffset();

        /**
         * @brief Retrieves the predefined integer type.
         *
         * @return Type* Pointer to the built-in integer type object.
         */
        Type* GetIntType();

        /**
         * @brief Clears all symbols from the symbol table.
         *
         * Removes all stored symbols and resets the offset to zero.
         */
        void Clear();

    private:
        /// @brief Storage map for all symbols indexed by name.
        std::unordered_map<std::string, std::unique_ptr<Symbol>> mSymbols;

        /// @brief Current memory offset for variable allocation.
        int mCurrentOffset;

        /// @brief Pointer to the predefined integer type.
        Type* mIntType;
    };
} // namespace MIEC

#endif // SYMBOLTABLE_H