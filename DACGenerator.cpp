#include "DACGenerator.h"
#include <iomanip>

namespace MIEC {

    DACGenerator::DACGenerator()
        : mTempCounter(1), mLabelCounter(1) {
    }

    // ==================== Temporary and Label Management ====================

    Operand DACGenerator::CreateTemp() {
        return Operand::MakeTemp(mTempCounter++);
    }

    int DACGenerator::CreateLabel() {
        return mLabelCounter++;
    }

    void DACGenerator::MarkLabel(int labelIndex) {
        // Mark the next statement with this label
        int address = static_cast<int>(mStatements.size());
        mLabelAddresses[labelIndex] = address;

        // If there's already a statement at this position, add the label to it
        // Otherwise, the label will be added when the next statement is created
    }

    int DACGenerator::GetCurrentAddress() const {
        return static_cast<int>(mStatements.size());
    }

    // ==================== Statement Generation ====================

    Operand DACGenerator::AddBinaryOp(OpKind op, const Operand& left, const Operand& right) {
        // Create temporary for result
        Operand result = CreateTemp();

        // Add the operation statement
        // The result is implicit in the triple (position in array)
        DACStatement stmt(op, left, right);
        AddStatement(stmt);

        return result;
    }

    void DACGenerator::AddAssignment(const Operand& target, const Operand& source) {
        DACStatement stmt(OpKind::eAssign, target, source);
        AddStatement(stmt);
    }

    void DACGenerator::AddJump(int labelIndex) {
        Operand labelOp = Operand::MakeLabel(labelIndex);
        DACStatement stmt(OpKind::eJump, labelOp);
        AddStatement(stmt);
    }

    void DACGenerator::AddConditionalJump(OpKind compareOp, const Operand& left,
        const Operand& right, int labelIndex) {
        // First, create a temporary for the comparison result
        Operand condTemp = AddBinaryOp(compareOp, left, right);

        // Then create the conditional jump
        Operand labelOp = Operand::MakeLabel(labelIndex);
        DACStatement stmt(OpKind::eIfFalseJump, condTemp, labelOp);
        AddStatement(stmt);
    }

    void DACGenerator::AddPrint(const Operand& operand) {
        DACStatement stmt(OpKind::ePrint, operand);
        AddStatement(stmt);
    }

    void DACGenerator::AddExit() {
        DACStatement stmt(OpKind::eExit);
        AddStatement(stmt);
    }

    // ==================== Utility ====================

    void DACGenerator::AddStatement(const DACStatement& statement) {
        DACStatement stmt = statement;

        // Check if this address should have a label
        int address = static_cast<int>(mStatements.size());
        for (const auto& pair : mLabelAddresses) {
            if (pair.second == address) {
                stmt.SetLabel(pair.first);
                break;
            }
        }

        mStatements.push_back(stmt);
    }

    void DACGenerator::Print(std::ostream& out) const {
        out << "=== Three-Address Code (DAC) ===" << std::endl;
        out << std::endl;

        for (size_t i = 0; i < mStatements.size(); ++i) {
            const DACStatement& stmt = mStatements[i];

            // Add line number for reference
            out << std::setw(3) << i << ": ";

            // Handle implicit result assignment for binary operations
            OpKind op = stmt.GetOperation();
            bool isBinaryArithmetic = (op == OpKind::eAdd || op == OpKind::eSubtract ||
                op == OpKind::eMultiply || op == OpKind::eDivide);

            bool isComparison = (op == OpKind::eIsEqual || op == OpKind::eIsLessEqual ||
                op == OpKind::eIsGreaterEqual || op == OpKind::eIsNotEqual ||
                op == OpKind::eIsLess || op == OpKind::eIsGreater);

            // Print label if present
            if (stmt.HasLabel()) {
                out << "L" << stmt.GetLabel() << ": ";
            }
            else {
                out << "    ";
            }

            // For binary operations, show the implicit result
            if (isBinaryArithmetic || isComparison) {
                // Find the temp that would be created for this operation
                // We need to track which statement creates which temp
                // For now, we'll calculate based on statement index
                int tempIndex = 0;
                for (size_t j = 0; j <= i; ++j) {
                    OpKind prevOp = mStatements[j].GetOperation();
                    if (prevOp == OpKind::eAdd || prevOp == OpKind::eSubtract ||
                        prevOp == OpKind::eMultiply || prevOp == OpKind::eDivide ||
                        prevOp == OpKind::eIsEqual || prevOp == OpKind::eIsLessEqual ||
                        prevOp == OpKind::eIsGreaterEqual || prevOp == OpKind::eIsNotEqual ||
                        prevOp == OpKind::eIsLess || prevOp == OpKind::eIsGreater) {
                        tempIndex++;
                    }
                }

                if (isBinaryArithmetic) {
                    out << "t" << tempIndex << " = "
                        << stmt.GetFirstOperand().ToString() << " "
                        << stmt.OpKindToString() << " "
                        << stmt.GetSecondOperand().ToString();
                }
                else {
                    out << "t" << tempIndex << " = "
                        << stmt.GetFirstOperand().ToString() << " "
                        << stmt.OpKindToString() << " "
                        << stmt.GetSecondOperand().ToString();
                }
            }
            else {
                // For other operations, use the standard ToString
                out << stmt.ToString();
            }

            out << std::endl;
        }

        out << std::endl;
    }

    void DACGenerator::Reset() {
        mStatements.clear();
        mLabelAddresses.clear();
        mTempCounter = 1;
        mLabelCounter = 1;
    }

} // namespace MIEC
