#include "CodeGenerator.h"
#include <iostream>
#include <stdexcept>

namespace MIEC {

    CodeGenerator::CodeGenerator(DACGenerator& dacGen)
        : mDacGen(&dacGen), mCodeGen(nullptr), mRegAdmin(nullptr) {

        // Create code generator with disassembly enabled
        mCodeGen = new CodeGenRISCV(true, false);  // disasm=true, printOnTerminal=false

        // Create register administrator
        mRegAdmin = new RegisterAdmin(mCodeGen, BASE_POINTER_REG);
    }

    CodeGenerator::~CodeGenerator() {
        delete mCodeGen;
        delete mRegAdmin;
    }

    void CodeGenerator::GenerateCode(std::ostream& iexFile, std::ostream& disFile) {
        // Initialize label address vector (find maximum label index first)
        int maxLabel = 0;
        for (const auto& stmt : mDacGen->GetStatements()) {
            if (stmt.HasLabel() && stmt.GetLabel() > maxLabel) {
                maxLabel = stmt.GetLabel();
            }

            // Also check operands for labels
            if (stmt.GetFirstOperand().GetKind() == OperandKind::eLabel) {
                int labelIdx = stmt.GetFirstOperand().GetLabelIndex();
                if (labelIdx > maxLabel) {
                    maxLabel = labelIdx;
                }
            }
            if (stmt.GetSecondOperand().GetKind() == OperandKind::eLabel) {
                int labelIdx = stmt.GetSecondOperand().GetLabelIndex();
                if (labelIdx > maxLabel) {
                    maxLabel = labelIdx;
                }
            }
        }

        // Resize label address vector (initialize with -1 = unresolved)
        mLabelAddresses.resize(maxLabel + 1, -1);

        // Step 1: Initialize program (setup base pointer, initialize variables)
        InitializeProgram();

        // Step 2: Generate code for each DAC statement
        const auto& statements = mDacGen->GetStatements();

        for (size_t i = 0; i < statements.size(); ++i) {
            const DACStatement& stmt = statements[i];

            // Mark label if this statement has one
            if (stmt.HasLabel()) {
                int labelIdx = stmt.GetLabel();
                WORD currentPos = mCodeGen->GetCodePosition();
                mLabelAddresses[labelIdx] = currentPos;
            }

            // Generate code based on operation type
            OpKind op = stmt.GetOperation();

            switch (op) {
            case OpKind::eAdd:
                OperationAdd(stmt, i);
                break;

            case OpKind::eSubtract:
                OperationSubtract(stmt, i);
                break;

            case OpKind::eMultiply:
                OperationMultiply(stmt, i);
                break;

            case OpKind::eDivide:
                OperationDivide(stmt, i);
                break;

            case OpKind::eAssign:
                OperationAssign(stmt);
                break;

            case OpKind::eJump:
                OperationJump(stmt);
                break;

            case OpKind::eIfFalseJump:
                // Conditional jump: need to look back at the comparison statement
                if (i > 0) {
                    const DACStatement& compareStmt = statements[i - 1];
                    OperationConditionalJump(compareStmt, stmt, i - 1);
                }
                break;

            case OpKind::ePrint:
                OperationPrint(stmt);
                break;

            case OpKind::eIsEqual:
            case OpKind::eIsLessEqual:
            case OpKind::eIsGreaterEqual:
            case OpKind::eIsNotEqual:
            case OpKind::eIsLess:
            case OpKind::eIsGreater:
                // Comparison operations create temporaries
                OperationComparison(stmt, i);
                break;

            case OpKind::eExit:
                // Exit program
                mCodeGen->Sleep();
                break;

            default:
                throw std::runtime_error("Unknown operation kind in DAC");
            }
        }

        // Step 3: Resolve all unresolved jumps
        ResolveUnresolvedJumps();

        // Step 4: Write output files
        mCodeGen->WriteExecutable(iexFile);
        mCodeGen->WriteDisassembled(disFile);
    }

    // ==================== Initialization ====================

    void CodeGenerator::InitializeProgram() {
        // Load base pointer with data segment address
        mCodeGen->LoadI(BASE_POINTER_REG, DATA_SEGMENT_BASE);

        // Initialize all variables to 0
        // Walk through symbol table and initialize each variable
        // Note: In the provided grammar, variables are added to the symbol table during parsing
        // We need to initialize them here by storing 0 at their offsets

        // Get a temporary register for the value 0
        RegNr zeroReg = mRegAdmin->GetFreeRegister();
        mCodeGen->LoadI(zeroReg, 0);

        // We need to know all variables and their offsets
        // Since we don't have direct access to the symbol table from here,
        // we'll rely on the fact that variables are used in the DAC code
        // For a more robust solution, we should pass the symbol table to the code generator

        // For now, we'll initialize memory by walking through the DAC and finding all variables
        std::set<std::string> initializedVars;

        for (const auto& stmt : mDacGen->GetStatements()) {
            // Check first operand
            if (stmt.GetFirstOperand().GetKind() == OperandKind::eVariable) {
                Symbol* sym = stmt.GetFirstOperand().GetSymbol();
                VarSymbol* var = dynamic_cast<VarSymbol*>(sym);
                if (var && initializedVars.find(var->GetName()) == initializedVars.end()) {
                    // Initialize this variable
                    int offset = var->GetOffset();
                    mCodeGen->Store(zeroReg, BASE_POINTER_REG, offset);
                    initializedVars.insert(var->GetName());
                }
            }

            // Check second operand
            if (stmt.GetSecondOperand().GetKind() == OperandKind::eVariable) {
                Symbol* sym = stmt.GetSecondOperand().GetSymbol();
                VarSymbol* var = dynamic_cast<VarSymbol*>(sym);
                if (var && initializedVars.find(var->GetName()) == initializedVars.end()) {
                    // Initialize this variable
                    int offset = var->GetOffset();
                    mCodeGen->Store(zeroReg, BASE_POINTER_REG, offset);
                    initializedVars.insert(var->GetName());
                }
            }
        }

        // Free the temporary register
        mRegAdmin->FreeRegister(zeroReg);
    }

    // ==================== Arithmetic Operations ====================

    void CodeGenerator::OperationAdd(const DACStatement& stmt, int stmtIndex) {
        // Get operands into registers
        RegNr leftReg = mRegAdmin->GetRegister(stmt.GetFirstOperand());
        RegNr rightReg = mRegAdmin->GetRegister(stmt.GetSecondOperand());

        // Get a register for the result
        RegNr resultReg = mRegAdmin->GetFreeRegister();

        // Generate: add resultReg, leftReg, rightReg
        mCodeGen->Add(leftReg, rightReg, resultReg);

        // Free input registers if they were temporaries
        if (stmt.GetFirstOperand().GetKind() == OperandKind::eTemp) {
            mRegAdmin->FreeRegister(leftReg);
        }
        if (stmt.GetSecondOperand().GetKind() == OperandKind::eTemp) {
            mRegAdmin->FreeRegister(rightReg);
        }

        // Assign result to the correct temporary
        // Calculate which temporary this statement creates
        int tempIndex = CalculateTempIndexForStatement(stmtIndex);
        Operand resultOp = Operand::MakeTemp(tempIndex);
        mRegAdmin->AssignRegister(resultReg, resultOp);
    }

    void CodeGenerator::OperationSubtract(const DACStatement& stmt, int stmtIndex) {
        // Get operands into registers
        RegNr leftReg = mRegAdmin->GetRegister(stmt.GetFirstOperand());
        RegNr rightReg = mRegAdmin->GetRegister(stmt.GetSecondOperand());

        // Get a register for the result
        RegNr resultReg = mRegAdmin->GetFreeRegister();

        // Generate: sub resultReg, leftReg, rightReg
        mCodeGen->Sub(leftReg, rightReg, resultReg);

        // Free input registers if they were temporaries
        if (stmt.GetFirstOperand().GetKind() == OperandKind::eTemp) {
            mRegAdmin->FreeRegister(leftReg);
        }
        if (stmt.GetSecondOperand().GetKind() == OperandKind::eTemp) {
            mRegAdmin->FreeRegister(rightReg);
        }

        // Assign result to the correct temporary
        int tempIndex = CalculateTempIndexForStatement(stmtIndex);
        Operand resultOp = Operand::MakeTemp(tempIndex);
        mRegAdmin->AssignRegister(resultReg, resultOp);
    }

    void CodeGenerator::OperationMultiply(const DACStatement& stmt, int stmtIndex) {
        // Get operands into registers
        RegNr leftReg = mRegAdmin->GetRegister(stmt.GetFirstOperand());
        RegNr rightReg = mRegAdmin->GetRegister(stmt.GetSecondOperand());

        // Get registers for result and temporary
        RegNr resultReg = mRegAdmin->GetFreeRegister();
        RegNr tmpReg = mRegAdmin->GetFreeRegister();

        // Generate: mul resultReg, leftReg, rightReg, tmpReg
        mCodeGen->Mul(leftReg, rightReg, resultReg, tmpReg);

        // Free temporary register
        mRegAdmin->FreeRegister(tmpReg);

        // Free input registers if they were temporaries
        if (stmt.GetFirstOperand().GetKind() == OperandKind::eTemp) {
            mRegAdmin->FreeRegister(leftReg);
        }
        if (stmt.GetSecondOperand().GetKind() == OperandKind::eTemp) {
            mRegAdmin->FreeRegister(rightReg);
        }

        // Assign result to the correct temporary
        int tempIndex = CalculateTempIndexForStatement(stmtIndex);
        Operand resultOp = Operand::MakeTemp(tempIndex);
        mRegAdmin->AssignRegister(resultReg, resultOp);
    }

    void CodeGenerator::OperationDivide(const DACStatement& stmt, int stmtIndex) {
        // Get operands into registers
        RegNr leftReg = mRegAdmin->GetRegister(stmt.GetFirstOperand());
        RegNr rightReg = mRegAdmin->GetRegister(stmt.GetSecondOperand());

        // Get registers for result and helper registers
        RegNr resultReg = mRegAdmin->GetFreeRegister();
        RegNr remainderReg = mRegAdmin->GetFreeRegister();
        RegNr tmpReg = mRegAdmin->GetFreeRegister();
        RegNr bitsReg = mRegAdmin->GetFreeRegister();

        // Generate: div resultReg, leftReg, rightReg, remainderReg, tmpReg, bitsReg
        mCodeGen->Div(leftReg, rightReg, resultReg, remainderReg, tmpReg, bitsReg);

        // Free helper registers
        mRegAdmin->FreeRegister(remainderReg);
        mRegAdmin->FreeRegister(tmpReg);
        mRegAdmin->FreeRegister(bitsReg);

        // Free input registers if they were temporaries
        if (stmt.GetFirstOperand().GetKind() == OperandKind::eTemp) {
            mRegAdmin->FreeRegister(leftReg);
        }
        if (stmt.GetSecondOperand().GetKind() == OperandKind::eTemp) {
            mRegAdmin->FreeRegister(rightReg);
        }

        // Assign result to the correct temporary
        int tempIndex = CalculateTempIndexForStatement(stmtIndex);
        Operand resultOp = Operand::MakeTemp(tempIndex);
        mRegAdmin->AssignRegister(resultReg, resultOp);
    }

    // ==================== Comparison Operations ====================

    void CodeGenerator::OperationComparison(const DACStatement& stmt, int stmtIndex) {
        // Comparisons in our DAC create a temporary that holds the comparison result
        // However, in RISC-V, we don't generate comparison code here
        // Instead, we use the comparison directly in the conditional jump

        // For now, we'll just track that this creates a temporary
        // The actual comparison happens in OperationConditionalJump

        // Note: We could implement this by using SLT (set less than) instructions,
        // but it's more efficient to use branch instructions directly
    }

    // ==================== Assignment ====================

    void CodeGenerator::OperationAssign(const DACStatement& stmt) {
        // target = source
        const Operand& target = stmt.GetFirstOperand();
        const Operand& source = stmt.GetSecondOperand();

        // Get source value into a register
        RegNr sourceReg = mRegAdmin->GetRegister(source);

        // Store to target
        if (target.GetKind() == OperandKind::eVariable) {
            // Store to variable in memory
            VarSymbol* var = dynamic_cast<VarSymbol*>(target.GetSymbol());
            if (var == nullptr) {
                throw std::runtime_error("Assignment target is not a variable!");
            }

            int offset = var->GetOffset();

            // Generate: sw sourceReg, offset(BP)
            mCodeGen->Store(sourceReg, BASE_POINTER_REG, offset);
        }
        else {
            throw std::runtime_error("Assignment target must be a variable!");
        }

        // Free source register if it was a temporary
        if (source.GetKind() == OperandKind::eTemp) {
            mRegAdmin->FreeRegister(sourceReg);
        }
    }

    // ==================== Jumps ====================

    void CodeGenerator::OperationJump(const DACStatement& stmt) {
        // Unconditional jump: GOTO label
        int labelIndex = stmt.GetFirstOperand().GetLabelIndex();

        // Get a temporary register for the jump
        RegNr tmpReg = mRegAdmin->GetFreeRegister();
        mCodeGen->LoadI(tmpReg, 0);  // Initialize register to avoid VM warning

        // Check if label is already resolved
        WORD labelAddr = GetLabelAddress(labelIndex);

        if (labelAddr != -1) {
            // Label already resolved - generate direct jump
            mCodeGen->Jump(0, tmpReg, labelAddr);
        }
        else {
            // Label not yet resolved - generate placeholder and remember position
            WORD jumpPos = mCodeGen->Jump(0, tmpReg, 0);  // placeholder address = 0
            mUnresolvedJumps.push_back(std::make_pair(jumpPos, labelIndex));
        }

        mRegAdmin->FreeRegister(tmpReg);
    }

    void CodeGenerator::OperationConditionalJump(const DACStatement& compareStmt,
        const DACStatement& jumpStmt,
        int compareIndex) {
        // ifFalse condition GOTO label
        // This means: if (left op right) is FALSE, jump to label
        // In RISC-V: we need to negate the condition

        // Get comparison operands
        RegNr leftReg = mRegAdmin->GetRegister(compareStmt.GetFirstOperand());
        RegNr rightReg = mRegAdmin->GetRegister(compareStmt.GetSecondOperand());

        // Get label
        int labelIndex = jumpStmt.GetSecondOperand().GetLabelIndex();
        WORD labelAddr = GetLabelAddress(labelIndex);

        // Get a temporary register
        RegNr tmpReg = mRegAdmin->GetFreeRegister();
        mCodeGen->LoadI(tmpReg, 0);  // Initialize register to avoid VM warning

        WORD jumpPos = 0;

        // Generate conditional jump based on comparison type
        // NOTE: ifFalse means we jump when condition is FALSE
        // So we need to use the OPPOSITE comparison
        OpKind compareOp = compareStmt.GetOperation();

        switch (compareOp) {
        case OpKind::eIsEqual:  // if !(a == b) goto L  =>  if (a != b) goto L
            jumpPos = mCodeGen->JumpNEQ(leftReg, rightReg, tmpReg,
                labelAddr != -1 ? labelAddr : 0);
            break;

        case OpKind::eIsNotEqual:  // if !(a != b) goto L  =>  if (a == b) goto L
            jumpPos = mCodeGen->JumpEQ(leftReg, rightReg, tmpReg,
                labelAddr != -1 ? labelAddr : 0);
            break;

        case OpKind::eIsLess:  // if !(a < b) goto L  =>  if (a >= b) goto L
            jumpPos = mCodeGen->JumpGE(leftReg, rightReg, tmpReg,
                labelAddr != -1 ? labelAddr : 0);
            break;

        case OpKind::eIsGreater:  // if !(a > b) goto L  =>  if (a <= b) goto L
            // a <= b means: !(a > b) means: (b >= a)
            jumpPos = mCodeGen->JumpGE(rightReg, leftReg, tmpReg,
                labelAddr != -1 ? labelAddr : 0);
            break;

        case OpKind::eIsLessEqual:  // if !(a <= b) goto L  =>  if (a > b) goto L
            // a > b means: (b < a)
            jumpPos = mCodeGen->JumpL(rightReg, leftReg, tmpReg,
                labelAddr != -1 ? labelAddr : 0);
            break;

        case OpKind::eIsGreaterEqual:  // if !(a >= b) goto L  =>  if (a < b) goto L
            jumpPos = mCodeGen->JumpL(leftReg, rightReg, tmpReg,
                labelAddr != -1 ? labelAddr : 0);
            break;

        default:
            throw std::runtime_error("Invalid comparison operation for conditional jump");
        }

        // If label not resolved, remember for later
        if (labelAddr == -1) {
            mUnresolvedJumps.push_back(std::make_pair(jumpPos, labelIndex));
        }

        // Free registers
        if (compareStmt.GetFirstOperand().GetKind() == OperandKind::eTemp) {
            mRegAdmin->FreeRegister(leftReg);
        }
        if (compareStmt.GetSecondOperand().GetKind() == OperandKind::eTemp) {
            mRegAdmin->FreeRegister(rightReg);
        }
        mRegAdmin->FreeRegister(tmpReg);
    }

    // ==================== Print ====================

    void CodeGenerator::OperationPrint(const DACStatement& stmt) {
        // Get operand into register
        RegNr valueReg = mRegAdmin->GetRegister(stmt.GetFirstOperand());

        // Generate: print valueReg
        mCodeGen->PrintInt(valueReg);

        // Free register if it was a temporary
        if (stmt.GetFirstOperand().GetKind() == OperandKind::eTemp) {
            mRegAdmin->FreeRegister(valueReg);
        }
    }

    // ==================== Helper Methods ====================

    void CodeGenerator::ResolveUnresolvedJumps() {
        for (const auto& jump : mUnresolvedJumps) {
            WORD jumpPos = jump.first;
            int labelIndex = jump.second;

            WORD labelAddr = GetLabelAddress(labelIndex);
            if (labelAddr == -1) {
                std::ostringstream oss;
                oss << "Label L" << labelIndex << " was never defined!";
                throw std::runtime_error(oss.str());
            }

            // Patch the jump instruction with the correct address
            mCodeGen->SetAddress(jumpPos, labelAddr);
        }
    }

    int CodeGenerator::CalculateTempIndexForStatement(int stmtIndex) const {
        // Calculate which temporary is created by this statement
        // by counting all previous binary operations (arithmetic + comparison)
        int tempIndex = 0;
        for (int i = 0; i <= stmtIndex; ++i) {
            OpKind op = mDacGen->GetStatements()[i].GetOperation();
            if (op == OpKind::eAdd || op == OpKind::eSubtract ||
                op == OpKind::eMultiply || op == OpKind::eDivide ||
                op == OpKind::eIsEqual || op == OpKind::eIsLessEqual ||
                op == OpKind::eIsGreaterEqual || op == OpKind::eIsNotEqual ||
                op == OpKind::eIsLess || op == OpKind::eIsGreater) {
                tempIndex++;
            }
        }
        return tempIndex;
    }

    WORD CodeGenerator::GetLabelAddress(int labelIndex) const {
        if (labelIndex < 0 || labelIndex >= static_cast<int>(mLabelAddresses.size())) {
            return -1;
        }
        return mLabelAddresses[labelIndex];
    }

} // namespace MIEC
