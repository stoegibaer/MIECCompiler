#ifndef DACSTATEMENT_H
#define DACSTATEMENT_H

#include "Symbol.h"
#include <string>
#include <variant>

namespace MIEC {

    // Operator types for three-address code
    enum class OpKind {
        eAdd,
        eSubtract,
        eMultiply,
        eDivide,
        eIsEqual,
        eIsLessEqual,
        eIsGreaterEqual,
        eIsNotEqual,
        eIsLess,
        eIsGreater,
        eAssign,
        eJump,
        eIfJump,
        eIfFalseJump,
        ePrint,
        eExit
    };

    // Operand types
    enum class OperandKind {
        eVariable,      // Variable from symbol table
        eTemp,          // Temporary variable (t1, t2, ...)
        eConstant,      // Integer constant
        eLabel,         // Label (L1, L2, ...)
        eNone           // No operand (for unary operations)
    };

    // Operand class representing different types of operands
    class Operand {
    public:
        // Constructors for different operand types
        Operand();                                  // None
        explicit Operand(Symbol* symbol);           // Variable
        static Operand MakeTemp(int index);         // Temporary
        static Operand MakeConstant(int value);     // Constant
        static Operand MakeLabel(int index);        // Label

        // Getters
        OperandKind GetKind() const { return mKind; }
        Symbol* GetSymbol() const;
        int GetTempIndex() const;
        int GetConstantValue() const;
        int GetLabelIndex() const;

        // Check if operand is valid (not None)
        bool IsValid() const { return mKind != OperandKind::eNone; }

        // String representation for printing
        std::string ToString() const;

    private:
        OperandKind mKind;
        std::variant<Symbol*, int> mData;

        // Private constructor for factory methods
        Operand(OperandKind kind, int value);
        Operand(OperandKind kind, Symbol* symbol);
    };

    // Single DAC statement (triple representation)
    class DACStatement {
    public:
        // Constructors
        DACStatement(OpKind op, const Operand& arg1, const Operand& arg2);
        DACStatement(OpKind op, const Operand& arg1);  // For unary operations
        explicit DACStatement(OpKind op);              // For operations without operands (EXIT)

        // Getters
        OpKind GetOperation() const { return mOperation; }
        const Operand& GetFirstOperand() const { return mFirstOperand; }
        const Operand& GetSecondOperand() const { return mSecondOperand; }

        // Label management for this statement
        void SetLabel(int label) { mLabel = label; mHasLabel = true; }
        bool HasLabel() const { return mHasLabel; }
        int GetLabel() const { return mLabel; }

        // String representation for printing
        std::string ToString() const;

        // Helper for converting OpKind to string
        std::string OpKindToString() const;

    private:
        OpKind mOperation;
        Operand mFirstOperand;
        Operand mSecondOperand;
        int mLabel;
        bool mHasLabel;
    };

} // namespace MIEC

#endif // DACSTATEMENT_H