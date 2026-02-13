#ifndef REGISTERADMIN_H
#define REGISTERADMIN_H

#include "Object.h"
#include "DACStatement.h"
#include "CodeGenRISCV.h"
#include <map>
#include <set>
#include <memory>

namespace MIEC {

    // Type alias for register numbers (BYTE is defined in CodeGen.h)
    using RegNr = BYTE;

    /**
     * @brief Register Administrator - manages register allocation for RISC-V code generation
     *
     * Register allocation strategy:
     * - Register 0: Hard-wired zero (cannot be used)
     * - Register 1: Base pointer for data segment (reserved)
     * - Registers 2-31: Available for temporaries and variables
     */
    class RegisterAdmin : public Object {
    public:
        /**
         * @brief Constructor
         * @param codeGen Pointer to the code generator (for loading variables/constants)
         * @param basePointerReg Register number used as base pointer
         */
        explicit RegisterAdmin(CodeGenRISCV* codeGen, BYTE basePointerReg);

        /**
         * @brief Destructor
         */
        virtual ~RegisterAdmin() = default;

        /**
         * @brief Get a free register
         * @return Register number of a free register
         * @throws std::runtime_error if no free register available
         */
        RegNr GetFreeRegister();

        /**
         * @brief Get the register holding the given operand, or load it into a register
         * @param op Operand to get/load
         * @return Register number holding the operand's value
         */
        RegNr GetRegister(const Operand& op);

        /**
         * @brief Free a register (mark it as available)
         * @param regNr Register number to free
         */
        void FreeRegister(RegNr regNr);

        /**
         * @brief Assign an operand to a register (remember which operand is in which register)
         * @param regNr Register number
         * @param op Operand to assign to this register
         */
        void AssignRegister(RegNr regNr, const Operand& op);

        /**
         * @brief Check if a register is free
         * @param regNr Register number
         * @return true if register is free, false otherwise
         */
        bool IsRegisterFree(RegNr regNr) const;

        /**
         * @brief Free all registers (reset state)
         */
        void FreeAllRegisters();

        /**
         * @brief Get total number of available registers (excluding reserved ones)
         * @return Number of usable registers
         */
        int GetAvailableRegisterCount() const;

    private:
        /**
         * @brief Load a variable into a register
         * @param varSymbol Variable symbol to load
         * @param targetReg Register to load into
         */
        void LoadVariable(Symbol* varSymbol, RegNr targetReg);

        /**
         * @brief Load a constant into a register
         * @param value Constant value to load
         * @param targetReg Register to load into
         */
        void LoadConstant(int value, RegNr targetReg);

        /**
         * @brief Load a temporary into a register (temporaries are stored in registers)
         * @param tempIndex Temporary index
         * @param targetReg Register to load into
         * @return true if temporary was found in a register, false otherwise
         */
        bool LoadTemporary(int tempIndex, RegNr targetReg);

        // Member variables
        CodeGenRISCV* mCodeGen;           // Code generator for emitting load instructions
        BYTE mBasePointerReg;              // Base pointer register number

        std::set<RegNr> mFreeRegisters;    // Set of currently free registers

        // Maps operands to registers
        // Key: string representation of operand (e.g., "x", "t1", "42")
        // Value: register number holding this operand
        std::map<std::string, RegNr> mOperandToRegister;

        // Reverse map: register to operand
        std::map<RegNr, std::string> mRegisterToOperand;

        // Register range
        static constexpr RegNr MIN_USABLE_REG = 2;   // First usable register (0=zero, 1=BP)
        static constexpr RegNr MAX_USABLE_REG = 31;  // Last usable register
    };

} // namespace MIEC

#endif // REGISTERADMIN_H
