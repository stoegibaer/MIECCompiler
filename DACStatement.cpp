#include "DACStatement.h"
#include <sstream>
#include <stdexcept>

namespace MIEC {

    // ==================== Operand Implementation ====================

    // Default constructor - None
    Operand::Operand()
        : mKind(OperandKind::eNone), mData(0) {
    }

    // Variable constructor
    Operand::Operand(Symbol* symbol)
        : mKind(OperandKind::eVariable), mData(symbol) {
    }

    // Private constructor for other types
    Operand::Operand(OperandKind kind, int value)
        : mKind(kind), mData(value) {
    }

    Operand::Operand(OperandKind kind, Symbol* symbol)
        : mKind(kind), mData(symbol) {
    }

    // Factory methods
    Operand Operand::MakeTemp(int index) {
        return Operand(OperandKind::eTemp, index);
    }

    Operand Operand::MakeConstant(int value) {
        return Operand(OperandKind::eConstant, value);
    }

    Operand Operand::MakeLabel(int index) {
        return Operand(OperandKind::eLabel, index);
    }

    // Getters
    Symbol* Operand::GetSymbol() const {
        if (mKind != OperandKind::eVariable) {
            throw std::runtime_error("Operand is not a variable");
        }
        return std::get<Symbol*>(mData);
    }

    int Operand::GetTempIndex() const {
        if (mKind != OperandKind::eTemp) {
            throw std::runtime_error("Operand is not a temporary");
        }
        return std::get<int>(mData);
    }

    int Operand::GetConstantValue() const {
        if (mKind != OperandKind::eConstant) {
            throw std::runtime_error("Operand is not a constant");
        }
        return std::get<int>(mData);
    }

    int Operand::GetLabelIndex() const {
        if (mKind != OperandKind::eLabel) {
            throw std::runtime_error("Operand is not a label");
        }
        return std::get<int>(mData);
    }

    // String representation
    std::string Operand::ToString() const {
        std::ostringstream oss;
        switch (mKind) {
        case OperandKind::eVariable:
            oss << GetSymbol()->GetName();
            break;
        case OperandKind::eTemp:
            oss << "t" << GetTempIndex();
            break;
        case OperandKind::eConstant:
            oss << GetConstantValue();
            break;
        case OperandKind::eLabel:
            oss << "L" << GetLabelIndex();
            break;
        case OperandKind::eNone:
            oss << "";
            break;
        }
        return oss.str();
    }

    // ==================== DACStatement Implementation ====================

    DACStatement::DACStatement(OpKind op, const Operand& arg1, const Operand& arg2)
        : mOperation(op), mFirstOperand(arg1), mSecondOperand(arg2),
        mLabel(-1), mHasLabel(false) {
    }

    DACStatement::DACStatement(OpKind op, const Operand& arg1)
        : mOperation(op), mFirstOperand(arg1), mSecondOperand(),
        mLabel(-1), mHasLabel(false) {
    }

    DACStatement::DACStatement(OpKind op)
        : mOperation(op), mFirstOperand(), mSecondOperand(),
        mLabel(-1), mHasLabel(false) {
    }

    std::string DACStatement::OpKindToString() const {
        switch (mOperation) {
        case OpKind::eAdd: return "+";
        case OpKind::eSubtract: return "-";
        case OpKind::eMultiply: return "*";
        case OpKind::eDivide: return "/";
        case OpKind::eIsEqual: return "==";
        case OpKind::eIsLessEqual: return "<=";
        case OpKind::eIsGreaterEqual: return ">=";
        case OpKind::eIsNotEqual: return "!=";
        case OpKind::eIsLess: return "<";
        case OpKind::eIsGreater: return ">";
        case OpKind::eAssign: return "=";
        case OpKind::eJump: return "GOTO";
        case OpKind::eIfJump: return "IF";
        case OpKind::eIfFalseJump: return "ifFalse";
        case OpKind::ePrint: return "print";
        case OpKind::eExit: return "EXIT";
        default: return "UNKNOWN";
        }
    }

    std::string DACStatement::ToString() const {
        std::ostringstream oss;

        // Add label if present
        if (mHasLabel) {
            oss << "L" << mLabel << ": ";
        }

        // Format based on operation type
        switch (mOperation) {
        case OpKind::eAdd:
        case OpKind::eSubtract:
        case OpKind::eMultiply:
        case OpKind::eDivide:
            // Binary arithmetic: result implicit (temp)
            oss << mFirstOperand.ToString() << " "
                << OpKindToString() << " "
                << mSecondOperand.ToString();
            break;

        case OpKind::eAssign:
            // Assignment: target = source
            oss << mFirstOperand.ToString() << " = "
                << mSecondOperand.ToString();
            break;

        case OpKind::eIsEqual:
        case OpKind::eIsLessEqual:
        case OpKind::eIsGreaterEqual:
        case OpKind::eIsNotEqual:
        case OpKind::eIsLess:
        case OpKind::eIsGreater:
            // Comparison: left op right
            oss << mFirstOperand.ToString() << " "
                << OpKindToString() << " "
                << mSecondOperand.ToString();
            break;

        case OpKind::eIfFalseJump:
            // Conditional jump: ifFalse condition GOTO label
            oss << "ifFalse " << mFirstOperand.ToString()
                << " GOTO " << mSecondOperand.ToString();
            break;

        case OpKind::eJump:
            // Unconditional jump: GOTO label
            oss << "GOTO " << mFirstOperand.ToString();
            break;

        case OpKind::ePrint:
            // Print statement
            oss << "print " << mFirstOperand.ToString();
            break;

        case OpKind::eExit:
            // Exit
            oss << "EXIT";
            break;

        default:
            oss << "UNKNOWN OPERATION";
            break;
        }

        return oss.str();
    }

} // namespace MIEC
