#ifndef DACGENERATOR_H
#define DACGENERATOR_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include "DACStatement.h"
#include "Symbol.h"

namespace MIEC {

    class DACGenerator {
    public:
        DACGenerator();
        virtual ~DACGenerator() = default;

        // ==================== Temporary and Label Management ====================

        // Create a new temporary variable (t1, t2, t3, ...)
        Operand CreateTemp();

        // Create a new label (returns label index)
        int CreateLabel();

        // Mark the current position with a label
        void MarkLabel(int labelIndex);

        // Get current statement index (for jump targets)
        int GetCurrentAddress() const;

        // ==================== Statement Generation ====================

        // Binary arithmetic operations (returns temporary with result)
        // Example: t1 = a + b
        Operand AddBinaryOp(OpKind op, const Operand& left, const Operand& right);

        // Assignment: target := source
        // Example: a = t1
        void AddAssignment(const Operand& target, const Operand& source);

        // Unconditional jump
        // Example: GOTO L1
        void AddJump(int labelIndex);

        // Conditional jump with comparison
        // Example: ifFalse a <= b GOTO L2
        void AddConditionalJump(OpKind compareOp, const Operand& left,
            const Operand& right, int labelIndex);

        // Print statement
        // Example: print a
        void AddPrint(const Operand& operand);

        // Exit statement
        void AddExit();

        // ==================== Utility ====================

        // Print all DAC statements (for debugging)
        void Print(std::ostream& out = std::cout) const;

        // Get all statements
        const std::vector<DACStatement>& GetStatements() const { return mStatements; }

        // Reset generator (clear all statements)
        void Reset();

    private:
        // Storage for all DAC statements
        std::vector<DACStatement> mStatements;

        // Counters for temporaries and labels
        int mTempCounter;
        int mLabelCounter;

        // Map from label index to statement address
        std::unordered_map<int, int> mLabelAddresses;

        // Helper: Add a statement to the list
        void AddStatement(const DACStatement& statement);
    };

} // namespace MIEC

#endif // DACGENERATOR_H
