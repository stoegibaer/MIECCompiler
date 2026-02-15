#ifndef DACSTATEMENT_H
#define DACSTATEMENT_H
#include "Symbol.h"
#include <string>
#include <variant>

namespace MIEC {
    /**
     * @enum OpKind
     * @brief Enumerates all operator types for three-address code instructions.
     */
    enum class OpKind {
        eAdd,              ///< Addition operator (+)
        eSubtract,         ///< Subtraction operator (-)
        eMultiply,         ///< Multiplication operator (*)
        eDivide,           ///< Division operator (/)
        eIsEqual,          ///< Equality comparison operator (==)
        eIsLessEqual,      ///< Less than or equal comparison operator (<=)
        eIsGreaterEqual,   ///< Greater than or equal comparison operator (>=)
        eIsNotEqual,       ///< Not equal comparison operator (!=)
        eIsLess,           ///< Less than comparison operator (<)
        eIsGreater,        ///< Greater than comparison operator (>)
        eAssign,           ///< Assignment operator (:=)
        eJump,             ///< Unconditional jump (GOTO)
        eIfJump,           ///< Conditional jump if true
        eIfFalseJump,      ///< Conditional jump if false
        ePrint,            ///< Print statement
        eExit              ///< Program exit statement
    };

    /**
     * @enum OperandKind
     * @brief Enumerates all types of operands used in three-address code.
     */
    enum class OperandKind {
        eVariable,      ///< Variable from the symbol table
        eTemp,          ///< Temporary variable (t1, t2, ...)
        eConstant,      ///< Integer constant value
        eLabel,         ///< Label for jump targets (L1, L2, ...)
        eNone           ///< No operand (for unary operations or no-op)
    };

    /**
     * @class Operand
     * @brief Represents an operand in a three-address code statement.
     *
     * @details An operand can be a variable, temporary, constant, label, or none.
     * This class provides factory methods and accessors for different operand types.
     */
    class Operand {
    public:
        /**
         * @brief Constructs an empty operand of kind eNone.
         */
        Operand();

        /**
         * @brief Constructs a variable operand from a symbol.
         *
         * @param symbol Pointer to the symbol table entry for the variable.
         */
        explicit Operand(Symbol* symbol);

        /**
         * @brief Factory method to create a temporary variable operand.
         *
         * @param index The index of the temporary variable.
         * @return Operand A temporary operand with the given index.
         */
        static Operand MakeTemp(int index);

        /**
         * @brief Factory method to create a constant operand.
         *
         * @param value The constant integer value.
         * @return Operand A constant operand with the given value.
         */
        static Operand MakeConstant(int value);

        /**
         * @brief Factory method to create a label operand.
         *
         * @param index The index of the label.
         * @return Operand A label operand with the given index.
         */
        static Operand MakeLabel(int index);

        /**
         * @brief Returns the kind of this operand.
         *
         * @return OperandKind The type of this operand.
         */
        OperandKind GetKind() const { return mKind; }

        /**
         * @brief Retrieves the symbol pointer if this is a variable operand.
         *
         * @return Symbol* The symbol table entry, or nullptr if not a variable.
         */
        Symbol* GetSymbol() const;

        /**
         * @brief Retrieves the temporary index if this is a temporary operand.
         *
         * @return int The temporary variable index.
         */
        int GetTempIndex() const;

        /**
         * @brief Retrieves the constant value if this is a constant operand.
         *
         * @return int The integer constant value.
         */
        int GetConstantValue() const;

        /**
         * @brief Retrieves the label index if this is a label operand.
         *
         * @return int The label index.
         */
        int GetLabelIndex() const;

        /**
         * @brief Checks if this operand is valid.
         *
         * @return bool True if the operand kind is not eNone, false otherwise.
         */
        bool IsValid() const { return mKind != OperandKind::eNone; }

        /**
         * @brief Generates a string representation of this operand.
         *
         * @return std::string The operand as a string (e.g., "t1", "42", "x").
         */
        std::string ToString() const;

    private:
        /// @brief The kind of this operand.
        OperandKind mKind;

        /// @brief Storage for operand data (symbol pointer or integer value).
        std::variant<Symbol*, int> mData;

        /**
         * @brief Private constructor for integer-based operands.
         *
         * @param kind The operand kind.
         * @param value The integer value (temporary index, constant, or label index).
         */
        Operand(OperandKind kind, int value);

        /**
         * @brief Private constructor for symbol-based operands.
         *
         * @param kind The operand kind.
         * @param symbol Pointer to the symbol table entry.
         */
        Operand(OperandKind kind, Symbol* symbol);
    };

    /**
     * @class DACStatement
     * @brief Represents a single three-address code statement.
     *
     * @details A DAC statement consists of an operation and up to two operands,
     * following the triple representation (op, arg1, arg2).
     */
    class DACStatement {
    public:
        /**
         * @brief Constructs a binary operation statement.
         *
         * @param op The operation to perform.
         * @param arg1 The first operand.
         * @param arg2 The second operand.
         */
        DACStatement(OpKind op, const Operand& arg1, const Operand& arg2);

        /**
         * @brief Constructs a unary operation statement.
         *
         * @param op The operation to perform.
         * @param arg1 The operand.
         */
        DACStatement(OpKind op, const Operand& arg1);

        /**
         * @brief Constructs a nullary operation statement.
         *
         * @param op The operation to perform (e.g., eExit).
         */
        explicit DACStatement(OpKind op);

        /**
         * @brief Returns the operation type of this statement.
         *
         * @return OpKind The operation kind.
         */
        OpKind GetOperation() const { return mOperation; }

        /**
         * @brief Returns the first operand of this statement.
         *
         * @return const Operand& A reference to the first operand.
         */
        const Operand& GetFirstOperand() const { return mFirstOperand; }

        /**
         * @brief Returns the second operand of this statement.
         *
         * @return const Operand& A reference to the second operand.
         */
        const Operand& GetSecondOperand() const { return mSecondOperand; }

        /**
         * @brief Associates a label with this statement.
         *
         * @param label The label index to assign.
         */
        void SetLabel(int label) { mLabel = label; mHasLabel = true; }

        /**
         * @brief Checks if this statement has an associated label.
         *
         * @return bool True if a label has been assigned, false otherwise.
         */
        bool HasLabel() const { return mHasLabel; }

        /**
         * @brief Retrieves the associated label index.
         *
         * @return int The label index (only valid if HasLabel() is true).
         */
        int GetLabel() const { return mLabel; }

        /**
         * @brief Generates a string representation of this statement.
         *
         * @return std::string A human-readable representation of the statement.
         */
        std::string ToString() const;

        /**
         * @brief Converts an operation kind to its string representation.
         *
         * @return std::string The operation as a string (e.g., "+", "-", "GOTO").
         */
        std::string OpKindToString() const;

    private:
        /// @brief The operation type of this statement.
        OpKind mOperation;

        /// @brief The first operand.
        Operand mFirstOperand;

        /// @brief The second operand.
        Operand mSecondOperand;

        /// @brief The label index associated with this statement (if any).
        int mLabel;

        /// @brief Flag indicating whether this statement has an associated label.
        bool mHasLabel;
    };
} // namespace MIEC

#endif // DACSTATEMENT_H