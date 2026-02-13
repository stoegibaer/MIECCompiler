#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H

#include "Object.h"
#include "CodeGenRISCV.h"
#include "DACGenerator.h"
#include "RegisterAdmin.h"
#include <vector>
#include <list>
#include <utility>
#include <fstream>

namespace MIEC {

    // Type alias for WORD (matching CodeGenRISCV which uses int32_t)
    using WORD = int32_t;

    /**
     * @brief Code Generator that converts DAC (Three-Address Code) to RISC-V machine code
     */
    class CodeGenerator : public Object {
    public:
        /**
         * @brief Constructor
         * @param dacGen Reference to the DAC Generator containing the intermediate code
         */
        explicit CodeGenerator(DACGenerator& dacGen);

        /**
         * @brief Destructor
         */
        virtual ~CodeGenerator();

        /**
         * @brief Main method to generate machine code from DAC and write to files
         * @param iexFile Output stream for executable (.iex) file
         * @param disFile Output stream for disassembly (.dis) file
         */
        void GenerateCode(std::ostream& iexFile, std::ostream& disFile);

    private:
        // ==================== Code Generation Methods ====================

        /**
         * @brief Initialize the program (setup base pointer, initialize variables)
         */
        void InitializeProgram();

        /**
         * @brief Generate code for arithmetic addition
         */
        void OperationAdd(const DACStatement& stmt, int stmtIndex);

        /**
         * @brief Generate code for arithmetic subtraction
         */
        void OperationSubtract(const DACStatement& stmt, int stmtIndex);

        /**
         * @brief Generate code for arithmetic multiplication
         */
        void OperationMultiply(const DACStatement& stmt, int stmtIndex);

        /**
         * @brief Generate code for arithmetic division
         */
        void OperationDivide(const DACStatement& stmt, int stmtIndex);

        /**
         * @brief Generate code for assignment operation
         */
        void OperationAssign(const DACStatement& stmt);

        /**
         * @brief Generate code for unconditional jump
         */
        void OperationJump(const DACStatement& stmt);

        /**
         * @brief Generate code for conditional jump (comparison + ifFalse jump)
         * @param compareStmt The comparison statement
         * @param jumpStmt The ifFalse jump statement
         */
        void OperationConditionalJump(const DACStatement& compareStmt,
            const DACStatement& jumpStmt,
            int compareIndex);

        /**
         * @brief Generate code for print statement
         */
        void OperationPrint(const DACStatement& stmt);

        /**
         * @brief Generate code for comparison operations (==, <, >, <=, >=, !=)
         */
        void OperationComparison(const DACStatement& stmt, int stmtIndex);

        // ==================== Helper Methods ====================

        /**
         * @brief Resolve all unresolved jumps after code generation
         */
        void ResolveUnresolvedJumps();

        /**
         * @brief Get the code address for a label
         * @param labelIndex The label index
         * @return Code address, or -1 if not found
         */
        WORD GetLabelAddress(int labelIndex) const;

        // ==================== Member Variables ====================

        DACGenerator* mDacGen;                    // Reference to DAC generator
        CodeGenRISCV* mCodeGen;                   // RISC-V code generator
        RegisterAdmin* mRegAdmin;                 // Register administrator

        // For jump resolution
        std::list<std::pair<WORD, int>> mUnresolvedJumps;  // (code_position, label_index)
        std::vector<WORD> mLabelAddresses;                  // label_index -> code_address

        // Constants
        static constexpr WORD DATA_SEGMENT_BASE = 0x10000;  // Base address for data segment
        static constexpr BYTE BASE_POINTER_REG = 1;         // Register 1 = Base Pointer

        // Temporary counter for creating unique temporaries
        int mTempCounter;
    };

} // namespace MIEC

#endif // CODEGENERATOR_H
