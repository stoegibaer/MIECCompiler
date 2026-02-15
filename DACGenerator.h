#ifndef DACGENERATOR_H
#define DACGENERATOR_H
#include <iostream>
#include <vector>
#include <unordered_map>
#include "DACStatement.h"
#include "Symbol.h"

namespace MIEC {
    /**
     * @class DACGenerator
     * @brief Generator for Three-Address Code (DAC) statements.
     *
     * @details This class manages the generation and storage of DAC statements,
     * including temporary variables, labels, and various instruction types.
     * It provides an interface for creating temporaries, managing control flow,
     * and generating intermediate code for a compiler.
     */
    class DACGenerator {
    public:
        /**
         * @brief Constructs a DACGenerator instance.
         *
         * Initializes all internal counters and maps to their default values.
         */
        DACGenerator();

        /**
         * @brief Virtual destructor.
         */
        virtual ~DACGenerator() = default;

        // ==================== Temporary and Label Management ====================

        /**
         * @brief Creates a new temporary variable.
         *
         * Generates a new temporary variable with a unique name (t1, t2, t3, ...).
         *
         * @return Operand The newly created temporary variable.
         */
        Operand CreateTemp();

        /**
         * @brief Creates a new label.
         *
         * Generates a unique label index that can be used as a jump target.
         *
         * @return int The index of the newly created label.
         */
        int CreateLabel();

        /**
         * @brief Marks the current position with a label.
         *
         * Associates the given label index with the current statement address
         * in the instruction stream.
         *
         * @param labelIndex The index of the label to mark.
         */
        void MarkLabel(int labelIndex);

        /**
         * @brief Retrieves the current statement address.
         *
         * Returns the index of the next statement to be added. Used to determine
         * jump targets and control flow addresses.
         *
         * @return int The current position in the statement vector.
         */
        int GetCurrentAddress() const;

        // ==================== Statement Generation ====================

        /**
         * @brief Generates a binary arithmetic operation.
         *
         * Creates a DAC statement for a binary operation and stores the result
         * in a newly created temporary variable.
         *
         * @param op The operation type (addition, subtraction, multiplication, etc.).
         * @param left The left operand.
         * @param right The right operand.
         * @return Operand The temporary variable containing the operation result.
         *
         * @example
         * @code
         * Operand t1 = gen.AddBinaryOp(OpKind::ADD, a, b);  // t1 = a + b
         * @endcode
         */
        Operand AddBinaryOp(OpKind op, const Operand& left, const Operand& right);

        /**
         * @brief Generates an assignment statement.
         *
         * Creates a DAC statement for a simple assignment operation.
         *
         * @param target The destination variable.
         * @param source The source operand.
         *
         * @example
         * @code
         * gen.AddAssignment(a, t1);  // a = t1
         * @endcode
         */
        void AddAssignment(const Operand& target, const Operand& source);

        /**
         * @brief Generates an unconditional jump statement.
         *
         * Creates a GOTO statement that jumps to the specified label unconditionally.
         *
         * @param labelIndex The index of the target label.
         *
         * @example
         * @code
         * int label = gen.CreateLabel();
         * gen.AddJump(label);  // GOTO label
         * @endcode
         */
        void AddJump(int labelIndex);

        /**
         * @brief Generates a conditional jump statement with comparison.
         *
         * Creates a conditional jump based on a comparison operation. The jump
         * is taken if the comparison evaluates to false.
         *
         * @param compareOp The comparison operator (less than, greater than, equal, etc.).
         * @param left The left operand of the comparison.
         * @param right The right operand of the comparison.
         * @param labelIndex The index of the target label if condition is false.
         *
         * @example
         * @code
         * int label = gen.CreateLabel();
         * gen.AddConditionalJump(OpKind::LEQ, a, b, label);  // ifFalse a <= b GOTO label
         * @endcode
         */
        void AddConditionalJump(OpKind compareOp, const Operand& left,
            const Operand& right, int labelIndex);

        /**
         * @brief Generates a print statement.
         *
         * Creates a DAC statement that outputs the value of the given operand.
         *
         * @param operand The operand to print.
         *
         * @example
         * @code
         * gen.AddPrint(a);  // print a
         * @endcode
         */
        void AddPrint(const Operand& operand);

        /**
         * @brief Generates an exit statement.
         *
         * Creates a statement that terminates program execution.
         */
        void AddExit();

        // ==================== Utility ====================

        /**
         * @brief Prints all generated DAC statements.
         *
         * Outputs all statements in a human-readable format for debugging purposes.
         *
         * @param out The output stream to write to (default: std::cout).
         */
        void Print(std::ostream& out = std::cout) const;

        /**
         * @brief Retrieves all generated statements.
         *
         * @return const std::vector<DACStatement>& A constant reference to the
         * vector containing all generated statements.
         */
        const std::vector<DACStatement>& GetStatements() const { return mStatements; }

        /**
         * @brief Resets the generator to its initial state.
         *
         * Clears all statements and resets all internal counters. After calling
         * this method, the generator is ready to generate new code.
         */
        void Reset();

    private:
        /// @brief Storage for all generated DAC statements.
        std::vector<DACStatement> mStatements;

        /// @brief Counter for generating unique temporary variable names.
        int mTempCounter;

        /// @brief Counter for generating unique label indices.
        int mLabelCounter;

        /// @brief Maps label indices to their corresponding statement addresses.
        std::unordered_map<int, int> mLabelAddresses;

        /**
         * @brief Helper function to add a statement to the list.
         *
         * Appends the given statement to the internal statement vector.
         *
         * @param statement The statement to add.
         *
         * @internal This is a private helper method used internally.
         */
        void AddStatement(const DACStatement& statement);
    };
} // namespace MIEC

#endif // DACGENERATOR_H