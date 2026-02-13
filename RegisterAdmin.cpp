#include "RegisterAdmin.h"
#include <stdexcept>
#include <sstream>

namespace MIEC {

    RegisterAdmin::RegisterAdmin(CodeGenRISCV* codeGen, BYTE basePointerReg)
        : mCodeGen(codeGen), mBasePointerReg(basePointerReg) {

        // Initialize all usable registers as free
        for (RegNr reg = MIN_USABLE_REG; reg <= MAX_USABLE_REG; ++reg) {
            mFreeRegisters.insert(reg);
        }
    }

    RegNr RegisterAdmin::GetFreeRegister() {
        if (mFreeRegisters.empty()) {
            throw std::runtime_error("No free registers available! Spilling not implemented.");
        }

        // Get the first free register
        RegNr reg = *mFreeRegisters.begin();
        mFreeRegisters.erase(mFreeRegisters.begin());

        return reg;
    }

    RegNr RegisterAdmin::GetRegister(const Operand& op) {
        std::string opKey = op.ToString();

        // Check if operand is already in a register
        auto it = mOperandToRegister.find(opKey);
        if (it != mOperandToRegister.end()) {
            return it->second;
        }

        // Operand not in register yet - get a free register and load the operand
        RegNr reg = GetFreeRegister();

        // Load operand based on its kind
        switch (op.GetKind()) {
        case OperandKind::eVariable:
            LoadVariable(op.GetSymbol(), reg);
            break;

        case OperandKind::eConstant:
            LoadConstant(op.GetConstantValue(), reg);
            break;

        case OperandKind::eTemp:
            // Temporaries should already be in registers from previous operations
            // If not, this is an error in code generation logic
            if (!LoadTemporary(op.GetTempIndex(), reg)) {
                std::ostringstream oss;
                oss << "Temporary t" << op.GetTempIndex() << " not found in any register!";
                throw std::runtime_error(oss.str());
            }
            break;

        case OperandKind::eLabel:
            // Labels are not loaded into registers
            throw std::runtime_error("Cannot load a label into a register");

        case OperandKind::eNone:
            throw std::runtime_error("Cannot load None operand into a register");
        }

        // Remember which operand is in which register
        mOperandToRegister[opKey] = reg;
        mRegisterToOperand[reg] = opKey;

        return reg;
    }

    void RegisterAdmin::FreeRegister(RegNr regNr) {
        // Remove mappings
        auto it = mRegisterToOperand.find(regNr);
        if (it != mRegisterToOperand.end()) {
            std::string opKey = it->second;
            mOperandToRegister.erase(opKey);
            mRegisterToOperand.erase(it);
        }

        // Mark register as free
        mFreeRegisters.insert(regNr);
    }

    void RegisterAdmin::AssignRegister(RegNr regNr, const Operand& op) {
        std::string opKey = op.ToString();

        // Remove old mapping if register was used before
        auto it = mRegisterToOperand.find(regNr);
        if (it != mRegisterToOperand.end()) {
            mOperandToRegister.erase(it->second);
        }

        // Create new mapping
        mOperandToRegister[opKey] = regNr;
        mRegisterToOperand[regNr] = opKey;

        // Remove register from free set
        mFreeRegisters.erase(regNr);
    }

    bool RegisterAdmin::IsRegisterFree(RegNr regNr) const {
        return mFreeRegisters.find(regNr) != mFreeRegisters.end();
    }

    void RegisterAdmin::FreeAllRegisters() {
        mFreeRegisters.clear();
        mOperandToRegister.clear();
        mRegisterToOperand.clear();

        // Re-initialize all usable registers as free
        for (RegNr reg = MIN_USABLE_REG; reg <= MAX_USABLE_REG; ++reg) {
            mFreeRegisters.insert(reg);
        }
    }

    int RegisterAdmin::GetAvailableRegisterCount() const {
        return MAX_USABLE_REG - MIN_USABLE_REG + 1;
    }

    // ==================== Private Helper Methods ====================

    void RegisterAdmin::LoadVariable(Symbol* varSymbol, RegNr targetReg) {
        // Variables are stored in memory at [BP + offset]
        VarSymbol* var = dynamic_cast<VarSymbol*>(varSymbol);
        if (var == nullptr) {
            throw std::runtime_error("Symbol is not a variable!");
        }

        int offset = var->GetOffset();

        // Generate: lw targetReg, offset(BP)
        mCodeGen->Load(targetReg, mBasePointerReg, offset);
    }

    void RegisterAdmin::LoadConstant(int value, RegNr targetReg) {
        // Generate: li targetReg, value (LoadI = Load Immediate)
        mCodeGen->LoadI(targetReg, value);
    }

    bool RegisterAdmin::LoadTemporary(int tempIndex, RegNr targetReg) {
        // Build temporary key
        std::ostringstream oss;
        oss << "t" << tempIndex;
        std::string tempKey = oss.str();

        // Check if temporary is in a register
        auto it = mOperandToRegister.find(tempKey);
        if (it != mOperandToRegister.end()) {
            // Found it - copy from that register to target register
            RegNr sourceReg = it->second;
            if (sourceReg != targetReg) {
                mCodeGen->Move(targetReg, sourceReg);
            }
            return true;
        }

        // Temporary not found
        return false;
    }

} // namespace MIEC
