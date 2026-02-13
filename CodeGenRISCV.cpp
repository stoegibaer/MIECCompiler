#include <string>
#include <ostream>
#include <cassert>
#include <cstring>
#include "CodeGenRISCV.h"


namespace MIEC
{
    const int CodeGenRISCV::cRegisterCount = 31; // 31 + hardwired zero register x0


    // ---------------------------- //
    //           CTOR/DTOR          //
    // ---------------------------- //

    CodeGenRISCV::CodeGenRISCV(bool disasm, bool printDisasmOnTerminal) : cGrowSize(1024)
    {
        mBufLength = cGrowSize;
        mpBuffer = new BYTE[mBufLength];
        mBufPos = 0;

        mDisassemble = disasm;
        mPrintDisasmOnTerminal = printDisasmOnTerminal;
    }

    CodeGenRISCV::~CodeGenRISCV()
    {
        delete[]mpBuffer;
    }


    // ---------------------------- //
    //      LOAD AND STORES         //
    // ---------------------------- //

    CodeGen<int32_t>::WORD CodeGenRISCV::LoadI(BYTE aReg, WORD aData)
    {
        // x[rd] = immediate
        // 1) immediate[31:12] rd 0110111           // first load upper immediate (top 20 bits of constant) - asm: lui rd, immediate
        // 2) immediate[11:0] rs1 000 rd 0010011    // then add last 12 bit of constant - asm: addi rd, rs1, immediate -> x[rd] = x[rs1] + immediate

        AppendCommand_UType(UType::OP_LUI, aReg, aData);
        WORD codePosData = mBufPos;
        AppendCommand_IType(IType::OP_TYPE_IMMEDIATE, aReg, aReg, IType::FUNC3_ADDI, aData);
        return codePosData;
    }

    void CodeGenRISCV::Load(BYTE aRegA, BYTE aRegB, WORD aData)
    {
        AppendCommand_IType(IType::OP_TYPE_LOAD, aRegA, aRegB, IType::FUNC3_LW, aData);
    }

    void CodeGenRISCV::LoadB(BYTE aRegA, BYTE aRegB, WORD aData)
    {
        AppendCommand_IType(IType::OP_TYPE_LOAD, aRegA, aRegB, IType::FUNC3_LB, aData);
    }

    void CodeGenRISCV::LoadBU(BYTE aRegA, BYTE aRegB, WORD aData)
    {
        AppendCommand_IType(IType::OP_TYPE_LOAD, aRegA, aRegB, IType::FUNC3_LBU, aData);
    }

    void CodeGenRISCV::LoadH(BYTE aRegA, BYTE aRegB, WORD aData)
    {
        AppendCommand_IType(IType::OP_TYPE_LOAD, aRegA, aRegB, IType::FUNC3_LH, aData);
    }

    void CodeGenRISCV::LoadHU(BYTE aRegA, BYTE aRegB, WORD aData)
    {
        AppendCommand_IType(IType::OP_TYPE_LOAD, aRegA, aRegB, IType::FUNC3_LHU, aData);
    }

    void CodeGenRISCV::Store(BYTE aRegA, BYTE aRegB, WORD aData)
    {
        // Interface says RegA=value, RegB=address
        // in RISCV, rs2 needs to hold value, rs1 the address - change the order of registers
        AppendCommand_SType(SType::OP_TYPE_STORE, aRegB, aRegA, SType::FUNC3_SW, aData);
    }

    void CodeGenRISCV::StoreB(BYTE aRegA, BYTE aRegB, WORD aData)
    {
        AppendCommand_SType(SType::OP_TYPE_STORE, aRegB, aRegA, SType::FUNC3_SB, aData);
    }

    void CodeGenRISCV::StoreH(BYTE aRegA, BYTE aRegB, WORD aData)
    {
        AppendCommand_SType(SType::OP_TYPE_STORE, aRegB, aRegA, SType::FUNC3_SH, aData);
    }


    // ---------------------------- //
    //    ARITHMETIC OPERATIONS     //
    // ---------------------------- //


    void CodeGenRISCV::Move(BYTE aRegA, BYTE aRegB)
    {
        // mv rd, rs1 (x[rd] = x[rs1]): expands to addi rd, rs1, 0
        AppendCommand_IType(IType::OP_TYPE_IMMEDIATE, aRegA, aRegB, IType::FUNC3_ADDI, 0);
    }

    void CodeGenRISCV::Add(BYTE aRegA, BYTE aRegB, BYTE aRegRes)
    {
        AppendCommand_RType(RType::OP_TYPE_REGISTER, aRegRes, aRegA, aRegB, RType::FUNC3_ADD, RType::FUNC7_ADD);
    }

    void CodeGenRISCV::Sub(BYTE aRegA, BYTE aRegB, BYTE aRegRes)
    {
        AppendCommand_RType(RType::OP_TYPE_REGISTER, aRegRes, aRegA, aRegB, RType::FUNC3_SUB, RType::FUNC7_SUB);
    }

    void CodeGenRISCV::Inc(BYTE aRegA)
    {
        AppendCommand_IType(IType::OP_TYPE_IMMEDIATE, aRegA, aRegA, IType::FUNC3_ADDI, 1);
    }

    void CodeGenRISCV::Dec(BYTE aRegA)
    {
        AppendCommand_IType(IType::OP_TYPE_IMMEDIATE, aRegA, aRegA, IType::FUNC3_ADDI, -1);
    }

    void CodeGenRISCV::Mul(BYTE aRegA, BYTE aRegB, BYTE aRegRes, BYTE aRegTmp)
    {
        // mul saves the lower 32 bit of result to rD, for a 64-mul: execute 1x mul for lower, 1x mulh for higher 32 bit
        AppendCommand_RType(RType::OP_TYPE_REGISTER, aRegRes, aRegA, aRegB, RType::FUNC3_MUL, RType::FUNC7_MUL);
    }

    void CodeGenRISCV::MulH(BYTE aRegA, BYTE aRegB, BYTE aRegRes, BYTE aRegTmp)
    {
        AppendCommand_RType(RType::OP_TYPE_REGISTER, aRegRes, aRegA, aRegB, RType::FUNC3_MULH, RType::FUNC7_MULH);
    }

    void CodeGenRISCV::MulHSU(BYTE aRegA, BYTE aRegB, BYTE aRegRes, BYTE aRegTmp)
    {
        AppendCommand_RType(RType::OP_TYPE_REGISTER, aRegRes, aRegA, aRegB, RType::FUNC3_MULHSU, RType::FUNC7_MULHSU);
    }

    void CodeGenRISCV::MulHU(BYTE aRegA, BYTE aRegB, BYTE aRegRes, BYTE aRegTmp)
    {
        AppendCommand_RType(RType::OP_TYPE_REGISTER, aRegRes, aRegA, aRegB, RType::FUNC3_MULHU, RType::FUNC7_MULHU);
    }

    void CodeGenRISCV::Div(BYTE aRegA, BYTE aRegB, BYTE aRegRes, BYTE aRegRemainder, BYTE aRegTmp, BYTE aRegBits)
    {
        // div rd, rs1, rs2: x[rd] = x[rs1] / x[rs2] = aRegA = regTmp (== regA) / regB
        AppendCommand_RType(RType::OP_TYPE_REGISTER, aRegRes, aRegA, aRegB, RType::FUNC3_DIV, RType::FUNC7_DIV);
    }

    void CodeGenRISCV::DivU(BYTE aRegDest, BYTE aRegA, BYTE aRegB)
    {
        AppendCommand_RType(RType::OP_TYPE_REGISTER, aRegDest, aRegA, aRegB, RType::FUNC3_DIVU, RType::FUNC7_DIVU);
    }

    void CodeGenRISCV::Rem(BYTE aRegRemainder, BYTE aRegA, BYTE aRegB)
    {
        AppendCommand_RType(RType::OP_TYPE_REGISTER, aRegRemainder, aRegA, aRegB, RType::FUNC3_REM, RType::FUNC7_REM);
    }

    void CodeGenRISCV::RemU(BYTE aRegRemainder, BYTE aRegA, BYTE aRegB)
    {
        AppendCommand_RType(RType::OP_TYPE_REGISTER, aRegRemainder, aRegA, aRegB, RType::FUNC3_REMU, RType::FUNC7_REMU);
    }

    void CodeGenRISCV::SetLessThan(BYTE aRegDest, BYTE aRegA, BYTE aRegB)
    {
        AppendCommand_RType(RType::OP_TYPE_REGISTER, aRegDest, aRegA, aRegB, RType::FUNC3_SLT, RType::FUNC7_SLT);
    }

    void CodeGenRISCV::SetLessThanU(BYTE aRegDest, BYTE aRegA, BYTE aRegB)
    {
        AppendCommand_RType(RType::OP_TYPE_REGISTER, aRegDest, aRegA, aRegB, RType::FUNC3_SLTU, RType::FUNC7_SLTU);
    }

    void CodeGenRISCV::SetLessThanI(BYTE aRegDest, BYTE aRegA, WORD aData)
    {
        AppendCommand_IType(IType::OP_TYPE_IMMEDIATE, aRegDest, aRegA, IType::FUNC3_SLTI, aData);
    }

    void CodeGenRISCV::SetLessThanIU(BYTE aRegDest, BYTE aRegA, WORD aData)
    {
        AppendCommand_IType(IType::OP_TYPE_IMMEDIATE, aRegDest, aRegA, IType::FUNC3_SLTIU, aData);
    }


    // ---------------------------- //
    //      LOGICAL OPERATIONS      //
    // ---------------------------- //

    void CodeGenRISCV::And(BYTE aRegA, BYTE aRegB, BYTE aRegRes)
    {
        AppendCommand_RType(RType::OP_TYPE_REGISTER, aRegRes, aRegA, aRegB, RType::FUNC3_AND, RType::FUNC7_AND);
    }

    void CodeGenRISCV::AndI(BYTE aRegA, BYTE aRegDest, WORD aData)
    {
        AppendCommand_IType(IType::OP_TYPE_IMMEDIATE, aRegDest, aRegA, IType::FUNC3_ANDI, aData);
    }

    void CodeGenRISCV::Or(BYTE aRegA, BYTE aRegB, BYTE aRegRes)
    {
        AppendCommand_RType(RType::OP_TYPE_REGISTER, aRegRes, aRegA, aRegB, RType::FUNC3_OR, RType::FUNC7_OR);
    }

    void CodeGenRISCV::OrI(BYTE aRegA, BYTE aRegDest, WORD aData)
    {
        AppendCommand_IType(IType::OP_TYPE_IMMEDIATE, aRegDest, aRegA, IType::FUNC3_ORI, aData);
    }

    void CodeGenRISCV::XOr(BYTE aRegA, BYTE aRegB, BYTE aRegRes)
    {
        AppendCommand_RType(RType::OP_TYPE_REGISTER, aRegRes, aRegA, aRegB, RType::FUNC3_XOR, RType::FUNC7_XOR);
    }

    void CodeGenRISCV::XOrI(BYTE aRegA, BYTE aRegDest, WORD aData)
    {
        AppendCommand_IType(IType::OP_TYPE_IMMEDIATE, aRegDest, aRegA, IType::FUNC3_XORI, aData);
    }

    void CodeGenRISCV::Not(BYTE aRegA, BYTE aRegRes)
    {
        // expands to xori rd, rs1, -1
        AppendCommand_IType(IType::OP_TYPE_IMMEDIATE, aRegRes, aRegA, IType::FUNC3_XORI, -1);
    }

    void CodeGenRISCV::ShL(BYTE aRegA, BYTE aRegB, BYTE aRegRes)
    {
        AppendCommand_RType(RType::OP_TYPE_REGISTER, aRegRes, aRegA, aRegB, RType::FUNC3_SLL, RType::FUNC7_SLL);
    }

    void CodeGenRISCV::ShR(BYTE aRegA, BYTE aRegB, BYTE aRegRes)
    {
        AppendCommand_RType(RType::OP_TYPE_REGISTER, aRegRes, aRegA, aRegB, RType::FUNC3_SRL, RType::FUNC7_SRL);
    }

    void CodeGenRISCV::ShRA(BYTE aRegA, BYTE aRegB, BYTE aRegRes)
    {
        AppendCommand_RType(RType::OP_TYPE_REGISTER, aRegRes, aRegA, aRegB, RType::FUNC3_SRA, RType::FUNC7_SRA);
    }

    void CodeGenRISCV::ShLLI(BYTE aRegA, BYTE aRegDest, WORD aData)
    {
        BYTE shamt = aData & 0x3F;
        WORD imm = IType::FUNC6_SRLI << 6 | shamt;
        AppendCommand_IType(IType::OP_TYPE_IMMEDIATE, aRegDest, aRegA, IType::FUNC3_SLLI, imm);
    }

    void CodeGenRISCV::ShRLI(BYTE aRegA, BYTE aRegDest, WORD aData)
    {
        // since IType does not have f6 except for shifts, else only the 12 bit immediate:
        // create the 12 bit immediate from shift amount in aData (6 bits) + known f6
        // shamt == least significant 6 bits of aData
        // move f6 to bits 12:6 in immediate
        BYTE shamt = aData & 0x3F;
        WORD imm = IType::FUNC6_SRLI << 6 | shamt;
        AppendCommand_IType(IType::OP_TYPE_IMMEDIATE, aRegDest, aRegA, IType::FUNC3_SRLI, imm);
    }

    void CodeGenRISCV::ShRAI(BYTE aRegA, BYTE aRegDest, WORD aData)
    {
        // same as ShRLI
        BYTE shamt = aData & 0x3F; // shamt taken from least sig bits of aData
        WORD imm = IType::FUNC6_SRAI << 6 | shamt;  
        AppendCommand_IType(IType::OP_TYPE_IMMEDIATE, aRegDest, aRegA, IType::FUNC3_SRAI, imm);
    }

    // ---------------------------- //
    //    ARITHMETIC OPERATIONS     //
    // ---------------------------- //


    CodeGen<int32_t>::WORD CodeGenRISCV::Jump(BYTE aRegA, BYTE aRegTmp, WORD addr)
    {
        WORD jumpAddress = mBufPos;
        AppendCommand_JType(JType::OP_JAL, aRegA, addr);
        return jumpAddress;
    }


    CodeGen<int32_t>::WORD CodeGenRISCV::JumpEQ(BYTE aRegA, BYTE aRegB, BYTE aRegTmp, WORD addr)
    {
        WORD jumpAddress = mBufPos;
        AppendCommand_BType(BType::OP_TYPE_BRANCH, aRegA, aRegB, BType::FUNC3_BEQ, addr);  
        return jumpAddress;
    }

    CodeGen<int32_t>::WORD CodeGenRISCV::JumpNEQ(BYTE aRegA, BYTE aRegB, BYTE aRegTmp, WORD addr)
    {
        WORD jumpAddress = mBufPos;
        AppendCommand_BType(BType::OP_TYPE_BRANCH, aRegA, aRegB, BType::FUNC3_BNEQ, addr);
        return jumpAddress;
    }

    CodeGen<int32_t>::WORD CodeGenRISCV::JumpL(BYTE aRegA, BYTE aRegB, BYTE aRegTmp, WORD addr)
    {
        WORD jumpAddress = mBufPos;
        AppendCommand_BType(BType::OP_TYPE_BRANCH, aRegA, aRegB, BType::FUNC3_BLT, addr); 
        return jumpAddress;
    }

    CodeGen<int32_t>::WORD CodeGenRISCV::JumpLE(BYTE aRegA, BYTE aRegB, BYTE aRegTmp, WORD addr)
    {
        WORD jumpAddress = mBufPos;
        AppendCommand_BType(BType::OP_TYPE_BRANCH, aRegB, aRegA, BType::FUNC3_BGE, addr); 
        return jumpAddress;
    }

    CodeGen<int32_t>::WORD CodeGenRISCV::JumpG(BYTE aRegA, BYTE aRegB, BYTE aRegTmp, WORD addr)
    {
        WORD jumpAddress = mBufPos;
        AppendCommand_BType(BType::OP_TYPE_BRANCH, aRegB, aRegA, BType::FUNC3_BLT, addr); 
        return jumpAddress;
    }

    CodeGen<int32_t>::WORD CodeGenRISCV::JumpGE(BYTE aRegA, BYTE aRegB, BYTE aRegTmp, WORD addr)
    {
        WORD jumpAddress = mBufPos;
        AppendCommand_BType(BType::OP_TYPE_BRANCH, aRegA, aRegB, BType::FUNC3_BGE, addr); 
        return jumpAddress;
    }

    CodeGen<int32_t>::WORD CodeGenRISCV::JumpGEU(BYTE aRegA, BYTE aRegB, BYTE aRegTmp, WORD addr)
    {
        WORD jumpAddress = mBufPos;
        AppendCommand_BType(BType::OP_TYPE_BRANCH, aRegA, aRegB, BType::FUNC3_BGEU, addr);    
        return jumpAddress;
    }

    CodeGen<int32_t>::WORD CodeGenRISCV::JumpLTU(BYTE aRegA, BYTE aRegB, BYTE aRegTmp, WORD addr)
    {
        WORD jumpAddress = mBufPos;
        AppendCommand_BType(BType::OP_TYPE_BRANCH, aRegA, aRegB, BType::FUNC3_BLTU, addr);    
        return jumpAddress;
    }

    CodeGen<int32_t>::WORD CodeGenRISCV::JumpR(BYTE aRegA, BYTE aRegDest, WORD addr)
    {
        WORD jumpAddress = mBufPos;
        AppendCommand_IType(IType::OP_JALR, aRegDest, aRegA, IType::FUNC3_JALR, addr);
        return jumpAddress;
    }



    // ---------------------------- //
    //       PROGRAM CONTROL        //
    // ---------------------------- //
 
    void CodeGenRISCV::Nop()
    {
        // nop == addi x0, x0, 0
        AppendCommand_IType(IType::OP_TYPE_IMMEDIATE, 0, 0, IType::FUNC3_ADDI, 0);
    }

    void CodeGenRISCV::Sleep()
    {
        // user defined instruction
        AppendCommand_PType(PType::OP_TYPE_SLEEP, 0, 0);
    }


    // ---------------------------- //
    //       PRINT TO CONSOLE       //
    // ---------------------------- //

    void CodeGenRISCV::PrintInt(BYTE aRegA)
    {
        AppendCommand_PType(PType::OP_TYPE_PRINT, PType::FUNC3_INT, aRegA);
    }

    void CodeGenRISCV::PrintString(BYTE aRegA)
    {
        AppendCommand_PType(PType::OP_TYPE_PRINT, PType::FUNC3_STRING, aRegA);
    }



    // ---------------------------- //
    //     INSTRUCTION FORMATS      //
    // ---------------------------- //

    // R-Type
    void CodeGenRISCV::AppendCommand_RType(BYTE opcode, BYTE regDest, BYTE regSrc1, BYTE regSrc2, BYTE funct3, BYTE funct7)
    {
        if (regDest > cRegisterCount)
        {
            wprintf(L"Register overflow regDest: %d\n", regDest);
            return;
        }
        if (regSrc1 > cRegisterCount)
        {
            wprintf(L"Register overflow regSrc1: %d\n", regSrc1);
            return;
        }
        if (regSrc2 > cRegisterCount)
        {
            wprintf(L"Register overflow regSrc2: %d\n", regSrc2);
            return;
        }

        WORD f7 = funct7 << 25;
        WORD f3 = funct3 << 12;
        WORD r2 = regSrc2 << 20;
        WORD r1 = regSrc1 << 15;
        WORD rd = regDest << 7;

        WORD instruction = (f7 | r2 | r1 | f3 | rd | opcode);
        AppendWord(instruction);
    }

    // I-type
    void CodeGenRISCV::AppendCommand_IType(BYTE opcode, BYTE regDest, BYTE regSrc1, BYTE funct3, WORD imm) {
        if (regDest > cRegisterCount)
        {
            wprintf(L"Register overflow regDest: %d\n", (int)regDest);
            return;
        }
        if (regSrc1 > cRegisterCount)
        {
            wprintf(L"Register overflow regSrc1: %d\n", (int)regSrc1);
            return;
        }

        WORD imm_11to0 = (imm & 0xFFF);
        WORD f3 = funct3;
        WORD r1 = regSrc1;
        WORD rd = regDest;

        WORD instruction = (imm_11to0 << 20 | r1 << 15 | f3 << 12 | rd << 7 | opcode);
        AppendWord(instruction);
    }

    // S-type
    void CodeGenRISCV::AppendCommand_SType(BYTE opcode, BYTE regSrc1, BYTE regSrc2, BYTE funct3, WORD imm) {
        if (regSrc1 > cRegisterCount)
        {
            wprintf(L"Register overflow regSrc1: %d\n", (int)regSrc1);
            return;
        }
        if (regSrc2 > cRegisterCount)
        {
            wprintf(L"Register overflow regSrc2: %d\n", (int)regSrc2);
            return;
        }

        WORD imm_11to5 = (imm & 0xFE0) << 25;
        WORD imm_4to0 = (imm & 0x1F) << 7;
        WORD reg2 = regSrc2 << 20;
        WORD reg1 = regSrc1 << 15;
        WORD f3 = funct3 << 12;

        WORD instruction = (imm_11to5 | reg2 | reg1 | f3 | imm_4to0 | opcode);
        AppendWord(instruction);
    }


    // B-type
    void CodeGenRISCV::AppendCommand_BType(BYTE opcode, BYTE regSrc1, BYTE regSrc2, BYTE funct3, WORD imm) {
        if (regSrc1 > cRegisterCount)
        {
            wprintf(L"Register overflow regSrc1: %d\n", (int)regSrc1);
            return;
        }
        if (regSrc2 > cRegisterCount)
        {
            wprintf(L"Register overflow regSrc2: %d\n", (int)regSrc2);
            return;
        } 
        
        WORD imm_11 = (imm & 0x800) << 20; //<< 31;
        WORD imm_10 = (imm & 0x400) >> 3; // << 7;
        WORD imm_9to4 = (imm & 0x3F0) << 21; // << 25;
        WORD imm_3to0 = (imm & 0xF) << 8;
        WORD f3 = funct3 << 12;
        WORD r2 = regSrc2 << 20;
        WORD r1 = regSrc1 << 15;

        WORD instruction = (imm_11 | imm_9to4 | r2 | r1 | f3 | imm_3to0 | imm_10 | opcode);
        AppendWord(instruction);
    }

    /// <summary>
    /// Creates an 32-Bit instruction of the U-type format.
    /// The created instruction is saved in the internal machine code buffer via AppendWord.
    /// </summary>
    /// <param name="opcode">OP-Code of a U-type instruction</param>
    /// <param name="regDest">Destination register</param>
    /// <param name="imm">32-Bit immediate; BUT only the top 20 Bit are used</param>
    void CodeGenRISCV::AppendCommand_UType(BYTE opcode, BYTE regDest, WORD imm) {
        if (regDest > cRegisterCount)
        {
            wprintf(L"Register overflow regDest: %d\n", (int)regDest);
            return;
        }

        WORD imm_31to12 = imm & 0xFFFFF000; // no shift needed, mask already picks upper 20 bits
        WORD rd = regDest << 7;

        WORD instruction = (imm_31to12 | rd | opcode);
        AppendWord(instruction);
    }

    // J-type
    void CodeGenRISCV::AppendCommand_JType(BYTE opcode, BYTE regDest, WORD imm) {
        if (regDest > cRegisterCount)
        {
            wprintf(L"Register overflow regDest: %d\n", (int)regDest);
            return;
        }

        WORD imm_19 = (imm & 0x80000) << 12;
        WORD imm_18to11 = (imm & 0x7F800) << 1;
        WORD imm_10 = (imm & 0x400) << 10;
        WORD imm_9to0 = (imm & 0x3FF) << 21;
        WORD rd = regDest << 7;

        WORD instruction = (imm_19 | imm_9to0 | imm_10 | imm_18to11 | rd | opcode);
        AppendWord(instruction);
    }

    /// <summary>
    /// P-Type
    /// Prints the value of the handed over register
    /// 000000000000sssssfff00000opcode  == [31:20] = 0; rs1 funct3 0 opcode
    /// </summary>
    void CodeGenRISCV::AppendCommand_PType(BYTE opcode, BYTE func3, BYTE regSrc) {
        if (regSrc > cRegisterCount)
        {
            wprintf(L"Register overflow regDest: %d\n", (int)regSrc);
            return;
        }
        WORD f3 = func3 << 12;
        WORD rs1 = regSrc << 15;

        WORD instruction = (rs1 | f3 | opcode);
        AppendWord(instruction);
    }



    // ---------------------------- //
    //            HELPER            //
    // ---------------------------- //

    void CodeGenRISCV::AppendWord(WORD aWord)
    {
        if (mBufPos + sizeof(WORD) > mBufLength)
        {
            BYTE* pNewBuffer = new BYTE[mBufLength + cGrowSize];
            memcpy(pNewBuffer, mpBuffer, mBufLength);
            delete[]mpBuffer;
            mpBuffer = pNewBuffer;
            mBufLength += cGrowSize;
        }
        *((WORD*)(mpBuffer + mBufPos)) = aWord;
        mBufPos += sizeof(WORD);

        if(mDisassemble) Disassemble(aWord);
    }


    BYTE MaskOpcode(int32_t instruction)
    {
        BYTE opcode = (instruction & 0x0000007F);
        return opcode;
    }

    /// <summary>
    /// Stores the absolute address specified by aAddress at the code-position specified by aCodePos by 
    /// manipulating/rewriting the instruction in the instruction buffer.
    /// The relative address needed for RISCV jumps is calculated from the CodePos and the absolute address
    /// </summary>
    /// <param name="aCodePos">Position of the jump instruction in the instruction buffer, which gets a new immediate to jump to</param>
    /// <param name="aAddress">New address that will be overwriting the old address in the instruction</param>
    void CodeGenRISCV::SetAddress(WORD aCodePos, WORD aAddress)
    {
        // 1) check if the jump would be in reach:
        if ((uint32_t)aAddress < 8192) {   //uint(32_t)aCodePos  // signed: [4096; -4095]; unsigned: 2*4096 = 8192

            // 2) get machinecode instruction on position aCodePos from the code buffer
            WORD inst = *((WORD*)(mpBuffer + aCodePos)); //mpBuffer[aCodePos];
            WORD opcode = MaskOpcode(inst);

            // 3) check which jump type the opcode has
            // 4) mask the offset as needed for each jump type (different bit patterns!)

            WORD replacedOffset = 0;
            WORD offsetBitmask = 0;

            // conditional jump, B-Type
            if (opcode == BType::OP_TYPE_BRANCH) {
                // do not cut off bit 0, else only jumping to even addresses
                WORD imm_11 = (aAddress & 0x800) << 20; //<< 31;
                WORD imm_10 = (aAddress & 0x400) >> 3; // << 7;
                WORD imm_9to4 = (aAddress & 0x3F0) << 21; // << 25;
                WORD imm_3to0 = (aAddress & 0xF) << 8;

                replacedOffset = (imm_11 | imm_9to4 | imm_3to0 | imm_10);
                offsetBitmask = 0xFE000F80;  // mask bits 31:25 and 11:7 
            }
            // unconditional jumps, J-Type
            else if (opcode == JType::OP_JAL) {
                WORD imm_19 = (aAddress & 0x80000) << 12;
                WORD imm_18to11 = (aAddress & 0x7F800) << 1;
                WORD imm_10 = (aAddress & 0x400) << 10;
                WORD imm_9to0 = (aAddress & 0x3FF) << 21;

                replacedOffset = imm_19 | imm_9to0 | imm_10 | imm_18to11;
                offsetBitmask = 0xFFFFF000;  // mask bits 31:12 //0x100000 | 0xFF000 | 0x800 | 0x7FE;
            }
            // unconditional jump and link, I-Type
            // address is read from register, here only set additional offset
            else if (opcode == IType::OP_JALR) {
                replacedOffset = (aAddress & 0xFFF) << 20;
                offsetBitmask = 0xFFF00000; // mask bits 31:20  //0xFFF;
            }
            else {
                wprintf(L"Opcode was not any kind of jump instruction");
            }

            // 5) delete all bits in the instruction where the offset-bitmask is 1
            //    -> because like this the whole instruction would be reset, invert the mask
            //       so that the instruction is kept on all other bits than the mask
            inst &= ~offsetBitmask;

            // 6) add the new offset to the instruction through OR
            inst |= replacedOffset;

            // 7) write the new instruction back into the buffer
            *((WORD*)(mpBuffer + aCodePos)) = inst;
        }
        else assert(false);
    }


    // gets the actual code-position
    CodeGen<int32_t>::WORD CodeGenRISCV::GetCodePosition() const
    {
        WORD pos = mBufPos / sizeof(WORD); 
        return pos;
    }


    void CodeGenRISCV::WriteExecutable(std::ostream& os)
    {
        os.write(reinterpret_cast<char const*>(mpBuffer), mBufPos);
    }

    void CodeGenRISCV::WriteDisassembled(std::ostream& os)
    {
        os << disasm.str();
    }

    int CodeGenRISCV::GetRegCnt() const
    {
        return cRegisterCount;
    }

    

    
    void CodeGenRISCV::Disassemble(WORD machinecode) {

        WORD opcode = (machinecode & 0x7F);
        std::string instruction = "";

        switch (opcode)
        {
        case RType::OP_TYPE_REGISTER: {
            WORD rd = (machinecode & 0xf80) >> 7;
            WORD f3 = (machinecode & 0x7000) >> 12;
            WORD rs1 = (machinecode & 0xf8000) >> 15;
            WORD rs2 = (machinecode & 0x1f00000) >> 20;
            WORD f7 = (machinecode & 0xfe000000) >> 25;

            if (f3 == RType::FUNC3_ADD && f7 == RType::FUNC7_ADD) instruction = "add"; 
            else if (f3 == RType::FUNC3_SUB && f7 == RType::FUNC7_SUB) instruction = "sub";
            else if (f3 == RType::FUNC3_SLL && f7 == RType::FUNC7_SLL) instruction = "sll"; 
            else if (f3 == RType::FUNC3_SLT && f7 == RType::FUNC7_SLT) instruction = "slt"; 
            else if (f3 == RType::FUNC3_SLTU && f7 == RType::FUNC7_SLTU) instruction = "sltu";
            else if (f3 == RType::FUNC3_XOR && f7 == RType::FUNC7_XOR) instruction = "xor";
            else if (f3 == RType::FUNC3_SRL && f7 == RType::FUNC7_SRL) instruction = "srl"; 
            else if (f3 == RType::FUNC3_SRA && f7 == RType::FUNC7_SRA) instruction = "sra";
            else if (f3 == RType::FUNC3_OR && f7 == RType::FUNC7_OR) instruction = "or";
            else if (f3 == RType::FUNC3_AND && f7 == RType::FUNC7_AND) instruction = "and"; 
            else if (f3 == RType::FUNC3_DIV && f7 == RType::FUNC7_DIV) instruction = "div"; 
            else if (f3 == RType::FUNC3_DIVU && f7 == RType::FUNC7_DIVU) instruction = "divu"; 
            else if (f3 == RType::FUNC3_REM && f7 == RType::FUNC7_REM) instruction = "rem"; 
            else if (f3 == RType::FUNC3_REMU && f7 == RType::FUNC7_REMU) instruction = "remu"; 
            else if (f3 == RType::FUNC3_MUL && f7 == RType::FUNC7_MUL) instruction = "mul"; 
            else if (f3 == RType::FUNC3_MULH && f7 == RType::FUNC7_MULH) instruction = "mulh";
            else if (f3 == RType::FUNC3_MULHU && f7 == RType::FUNC7_MULHU) instruction = "mulhu";
            else if (f3 == RType::FUNC3_MULHSU && f7 == RType::FUNC7_MULHSU) instruction = "mulhsu";
            else instruction = "unknown"; 

            if(mPrintDisasmOnTerminal) wprintf(L"%S r%d,r%d,r%d\n", instruction.c_str(), rd, rs1, rs2);
            disasm << instruction << " r" << rd << ",r" << rs1 << ",r" << rs2 << std::endl;
            break; 
        }

        case IType::OP_TYPE_IMMEDIATE: {
            WORD rd = (machinecode & 0xf80) >> 7;
            WORD f3 = (machinecode & 0x7000) >> 12;
            WORD rs1 = (machinecode & 0xf8000) >> 15;

            if (f3 == IType::FUNC3_SLLI || f3 == IType::FUNC3_SRLI || f3 == IType::FUNC3_SRAI) {
                WORD f6 = (machinecode & 0xfc000000) >> 26;
                WORD shamt = (machinecode & 0x3f00000) >> 20;

                if (f3 == IType::FUNC3_SLLI && f6 == IType::FUNC6_SLLI) instruction = "slli"; 
                else if (f3 == IType::FUNC3_SRLI && f6 == IType::FUNC6_SRLI) instruction = "srli";
                else if (f3 == IType::FUNC3_SRAI && f6 == IType::FUNC6_SRAI) instruction = "srai"; 

                if (mPrintDisasmOnTerminal) wprintf(L"%S %d,%d,%d\n", instruction.c_str(), rd, rs1, shamt);
                disasm << instruction << " " << rd << "," << rs1 << "," << shamt << std::endl;
            }
            else {
                WORD imm12 = (machinecode & 0xfff00000) >> 20;

                if (f3 == IType::FUNC3_ADDI) instruction = "addi"; 
                else if (f3 == IType::FUNC3_SLTI) instruction = "slti";
                else if (f3 == IType::FUNC3_SLTIU) instruction = "sltiu";
                else if (f3 == IType::FUNC3_XORI) instruction = "xori"; 
                else if (f3 == IType::FUNC3_ORI) instruction = "ori"; 
                else if (f3 == IType::FUNC3_ANDI) instruction = "andi";
                else instruction = "unknown"; 

                if (mPrintDisasmOnTerminal) wprintf(L"%S r%d,r%d,%d\n", instruction.c_str(), rd, rs1, imm12);
                disasm << instruction << " r" << rd << ",r" << rs1 << "," << imm12 << std::endl;
            }
            break;
        }

        case IType::OP_TYPE_LOAD: {
            WORD rd = (machinecode & 0xf80) >> 7;
            WORD f3 = (machinecode & 0x7000) >> 12;
            WORD rs1 = (machinecode & 0xf8000) >> 15;
            WORD imm12 = (machinecode & 0xfff00000) >> 20;
            
            if (f3 == IType::FUNC3_LB) instruction = "lb"; 
            else if (f3 == IType::FUNC3_LH) instruction = "lh"; 
            else if (f3 == IType::FUNC3_LW) instruction = "lw";
            else if (f3 == IType::FUNC3_LBU) instruction = "lbu"; 
            else if (f3 == IType::FUNC3_LHU) instruction = "lhu"; 
            else instruction = "unknown";

            if (mPrintDisasmOnTerminal) wprintf(L"%S r%d,[r%d]+%d\n", instruction.c_str(), rd, rs1, imm12);
            disasm << instruction << " r" << rd << ",[r" << rs1 << "]+" << imm12 << std::endl;
            break;
        }

        case IType::OP_JALR: {
            WORD rd = (machinecode & 0xf80) >> 7;
            WORD f3 = (machinecode & 0x7000) >> 12;
            WORD rs1 = (machinecode & 0xf8000) >> 15;
            WORD imm12 = (machinecode & 0xfff00000) >> 20;
            
            if (f3 == IType::FUNC3_JALR) instruction = "jalr";
            else instruction = "unknown";

            if (mPrintDisasmOnTerminal) wprintf(L"%S r%d,[r%d]+%d\n", instruction.c_str(), rd, rs1, imm12);
            disasm << instruction << " r" << rd << ",[r" << rs1 << "]+" << imm12 << std::endl;
            break;
        }

        case SType::OP_TYPE_STORE: {

            WORD f3 = (machinecode & 0x7000) >> 12;
            WORD rs1 = (machinecode & 0xf8000) >> 15;
            WORD rs2 = (machinecode & 0x1f00000) >> 20;

            WORD imm5 = (machinecode & 0xf80) >> 7;
            WORD imm7 = (machinecode & 0xfe000000) >> 25;
            WORD imm = (imm7 << 5) | imm5;

            if (f3 == SType::FUNC3_SB) instruction = "sb";
            else if (f3 == SType::FUNC3_SH) instruction = "sh"; 
            else if (f3 == SType::FUNC3_SW) instruction = "sw";
            else instruction = "unknown";

            if (mPrintDisasmOnTerminal) wprintf(L"%S r%d,[r%d]+%d\n", instruction.c_str(), rs2, rs1, imm);
            disasm << instruction << " r" << rs2 << ",[r" << rs1 << "]+" << imm << std::endl;
            break;
        }

        case BType::OP_TYPE_BRANCH: {

            // first get bits for registers and f3
            WORD f3 = (machinecode & 0x7000) >> 12;
            WORD rs1 = (machinecode & 0xf8000) >> 15;
            WORD rs2 = (machinecode & 0x1f00000) >> 20;

            // then recreate the immediate correctly by moving the bits to correct position
            WORD imm12 = (machinecode & (1 << 31)) >> 20;       // move from 31 to 12
            WORD imm11 = (machinecode & (1 << 7)) << 4;         // move from 7 to 11
            WORD imm105 = (machinecode & 0x7E000000) >> 20;     // move from 30:25 to 10:5
            WORD imm41 = (machinecode & 0xF00) >> 5;             // move from 11:8 to 4:1
            WORD imm = imm12 | imm11 | imm105 | imm41;          // assemble all bits to the immediate
            
            // check which instruction was done
            if (f3 == BType::FUNC3_BEQ) instruction = "beq"; 
            else if (f3 == BType::FUNC3_BNEQ) instruction = "bneq"; 
            else if (f3 == BType::FUNC3_BLT) instruction = "blt"; 
            else if (f3 == BType::FUNC3_BGE) instruction = "bge"; 
            else if (f3 == BType::FUNC3_BLTU) instruction = "bltu"; 
            else if (f3 == BType::FUNC3_BGEU) instruction = "bgeu";
            else instruction = "unknown";
            
            if (mPrintDisasmOnTerminal) wprintf(L"%S r%d,r%d,#%d\n", instruction.c_str(), rs1, rs2, imm);
            disasm << instruction << " r" << rs1 << ",r" << rs2 << ",#" << imm << std::endl;
            break;
        }

        case UType::OP_LUI: {
            WORD rd = (machinecode & 0xf80) >> 7;
            WORD imm20 = (machinecode & 0xfffff000) >> 12;
            instruction = "lui";

            if (mPrintDisasmOnTerminal) wprintf(L"%S r%d,%d\n", instruction.c_str(), rd, imm20);
            disasm << instruction << " r" << rd << "," << imm20 << std::endl;
            break;
        }

        case JType::OP_JAL: {
            WORD rd = (machinecode & 0xf80) >> 7;

            WORD imm20 = (machinecode & (1 << 31)) >> 31;
            WORD imm1912 = (machinecode & 0xFF000) >> 12;
            WORD imm11 = (machinecode & (1 << 20)) >> 20;
            WORD imm101 = (machinecode & 0x7FE00000) >> 21;
            WORD imm = (imm20 << 19) | (imm1912 << 12) | (imm11 << 11) | (imm101 << 1);

            instruction = "jal";
            if (mPrintDisasmOnTerminal) wprintf(L"%S r%d,#%d\n", instruction.c_str(), rd, imm);
            disasm << instruction << " r" << rd << ",#" << imm << std::endl;
            break;
        }
        case PType::OP_TYPE_PRINT: {
            WORD f3 = (machinecode & 0x7000) >> 12;
            WORD rs1 = (machinecode & 0xf8000) >> 15;

            if (f3 == PType::FUNC3_INT) instruction = "pint"; 
            else if (f3 == PType::FUNC3_STRING) instruction = "pstr";

            if (mPrintDisasmOnTerminal) wprintf(L"%S r%d\n", instruction.c_str(), rs1);
            disasm << instruction << " r" << rs1 << std::endl;
            break;
        }
        case PType::OP_TYPE_SLEEP: {
            if (mPrintDisasmOnTerminal) wprintf(L"sleep\n");
            disasm << "sleep" << std::endl;
            break;
        }
        default:
            if (mPrintDisasmOnTerminal) wprintf(L"unknown opcode");
            disasm << "unknown opcode" << std::endl;
            break;
        }
    }
}