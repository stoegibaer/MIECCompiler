#pragma once

#include "CodeGen.h"
#include <sstream>


namespace UType {
    constexpr auto OP_LUI   = 0b0110111;
    constexpr auto OP_AUIPC = 0b0010111;
}

namespace JType {
    constexpr auto OP_JAL       = 0b1101111;
}

namespace RType {
    constexpr auto OP_TYPE_REGISTER = 0b0110011;

    // RV32I
    constexpr auto FUNC3_ADD    = 0b000;
    constexpr auto FUNC7_ADD    = 0b0000000;
    constexpr auto FUNC3_SUB    = 0b000;
    constexpr auto FUNC7_SUB    = 0b0100000;
    constexpr auto FUNC3_SLL    = 0b001;
    constexpr auto FUNC7_SLL    = 0b0000000;
    constexpr auto FUNC3_SLT    = 0b010;
    constexpr auto FUNC7_SLT    = 0b0000000;
    constexpr auto FUNC3_SLTU   = 0b011;
    constexpr auto FUNC7_SLTU   = 0b0000000;
    constexpr auto FUNC3_XOR    = 0b100;
    constexpr auto FUNC7_XOR    = 0b0000000;
    constexpr auto FUNC3_SRL    = 0b101;
    constexpr auto FUNC7_SRL    = 0b0000000;
    constexpr auto FUNC3_SRA    = 0b101;
    constexpr auto FUNC7_SRA    = 0b0100000;
    constexpr auto FUNC3_OR     = 0b110;
    constexpr auto FUNC7_OR     = 0b0000000;
    constexpr auto FUNC3_AND    = 0b111;
    constexpr auto FUNC7_AND    = 0b0000000;

    // RV32M
    constexpr auto FUNC3_MUL    = 0b000;
    constexpr auto FUNC7_MUL    = 0b0000001;
    constexpr auto FUNC3_MULH   = 0b001;
    constexpr auto FUNC7_MULH   = 0b0000001;
    constexpr auto FUNC3_MULHSU = 0b010;
    constexpr auto FUNC7_MULHSU = 0b0000001;
    constexpr auto FUNC3_MULHU  = 0b011;
    constexpr auto FUNC7_MULHU  = 0b0000001;
    constexpr auto FUNC3_DIV    = 0b100;
    constexpr auto FUNC7_DIV    = 0b0000001;
    constexpr auto FUNC3_DIVU   = 0b101;
    constexpr auto FUNC7_DIVU   = 0b0000001;
    constexpr auto FUNC3_REM    = 0b110;
    constexpr auto FUNC7_REM    = 0b0000001;
    constexpr auto FUNC3_REMU   = 0b111;
    constexpr auto FUNC7_REMU   = 0b0000001;
}

namespace IType {
    constexpr auto OP_TYPE_LOAD = 0b0000011;
    constexpr auto FUNC3_LB = 0b000;
    constexpr auto FUNC3_LH = 0b001;
    constexpr auto FUNC3_LW = 0b010;
    constexpr auto FUNC3_LBU = 0b100;
    constexpr auto FUNC3_LHU = 0b101;

    constexpr auto OP_TYPE_IMMEDIATE = 0b0010011;
    constexpr auto FUNC3_ADDI = 0b000;
    constexpr auto FUNC3_SLTI = 0b010;
    constexpr auto FUNC3_SLTIU = 0b011;
    constexpr auto FUNC3_XORI = 0b100;
    constexpr auto FUNC3_ORI = 0b110;
    constexpr auto FUNC3_ANDI = 0b111;
    constexpr auto FUNC3_SLLI = 0b001;
    constexpr auto FUNC6_SLLI = 0b000000;
    constexpr auto FUNC3_SRLI = 0b101;
    constexpr auto FUNC6_SRLI = 0b000000;
    constexpr auto FUNC3_SRAI = 0b101;
    constexpr auto FUNC6_SRAI = 0b010000;

    constexpr auto OP_JALR = 0b1100111;
    constexpr auto FUNC3_JALR = 0b000;

    // currently not needed and implemented, but exist in RV32I
    constexpr auto OP_TYPE_FENCE = 0b0001111;   // for I/O
    constexpr auto OP_TYPE_E = 0b1110011;       // for exceptions
    constexpr auto OP_TYPE_CSR = 0b1110011;     // for controls and status register
}

namespace SType {
    constexpr auto OP_TYPE_STORE = 0b0100011;
    constexpr auto FUNC3_SB = 0b000;
    constexpr auto FUNC3_SH = 0b001;
    constexpr auto FUNC3_SW = 0b010;
}

namespace BType {
    constexpr auto OP_TYPE_BRANCH = 0b1100011;
    constexpr auto FUNC3_BEQ  = 0b000;
    constexpr auto FUNC3_BNEQ = 0b001;
    constexpr auto FUNC3_BLT  = 0b100;
    constexpr auto FUNC3_BGE  = 0b101;
    constexpr auto FUNC3_BLTU = 0b110;
    constexpr auto FUNC3_BGEU = 0b111;
}

namespace PType {
    constexpr auto OP_TYPE_PRINT = 0b1111111;
    constexpr auto FUNC3_INT     = 0b0000000;
    constexpr auto FUNC3_STRING  = 0b0000001;

    constexpr auto OP_TYPE_SLEEP = 0b1111110;
}


namespace MIEC
{
    class CodeGenRISCV : public CodeGen<int32_t>{
    public:
        /// <summary>
        /// Constructor of the RISC-V code generator. Takes 2 flags as input parameter for directly disassembling the generated machine code.
        /// </summary>
        /// <param name="disasm">Flag for if the machine code shall be disassembled and written into an internal stringstream, that can get 
        /// loaded into any other output stream (i.e. a file stream) via the WriteDisassembled method </param>
        /// <param name="printDisasmOnTerminal">Flag for if the disassembled machine code shall be printed on the code gens execution terminal</param>
        CodeGenRISCV(bool disasm, bool printDisasmOnTerminal);

        /// <summary>
        /// Destructor of the RISC-V code generator
        /// </summary>
        ~CodeGenRISCV();

        // load and store

        /// <summary>
        /// Load Immediate. Load a WORD sized constant into the destination register
        /// </summary>
        /// <param name="aReg">Target register</param>
        /// <param name="aData">Constant</param>
        /// <returns>Returns the position of this statement in the code generator's code buffer (machinecode address)</returns>
        virtual WORD LoadI(BYTE aReg, WORD aData) override;
       
        /// <summary>
        /// Loads the value from the address held by register B into register A.
        /// The optional data can be used as an additional target address offset. 
        /// </summary>
        /// <param name="aRegA">Target register</param>
        /// <param name="aRegB">Source register holding the memory address to load from</param>
        /// <param name="aData">Additional target address offset</param>
        virtual void Load(BYTE aRegA, BYTE aRegB, WORD aData) override;

        /// <summary>
        /// Loads a byte from the data in register B into register A.
        /// --- BYTEWISE LOAD NOT IMPLEMENTED IN RISC-V VM as of 2022! -> interpreted as "normal" load word!
        /// </summary>
        /// <param name="aRegA">Target register</param>
        /// <param name="aRegB">Source register holding the value to load</param>
        /// <param name="aData">Additional target address offset</param>
        void LoadB(BYTE aRegA, BYTE aRegB, WORD aData);

        /// <summary>
        /// Loads an unsigned byte from the data in register B into register A.
        /// --- BYTEWISE LOAD NOT IMPLEMENTED IN RISC-V VM as of 2022! -> interpreted as "normal" load word!
        /// </summary>
        /// <param name="aRegA">Target register</param>
        /// <param name="aRegB">Source register holding the value to load</param>
        /// <param name="aData">Additional target address offset</param>
        void LoadBU(BYTE aRegA, BYTE aRegB, WORD aData);

        /// <summary>
        /// Loads a halfword from the data in register B into register A.
        /// --- HALFWORD LOAD NOT IMPLEMENTED IN RISC-V VM as of 2022!  -> interpreted as "normal" load word!
        /// </summary>
        /// <param name="aRegA">Target register</param>
        /// <param name="aRegB">Source register holding the value to load</param>
        /// <param name="aData">Additional target address offset</param>
        void LoadH(BYTE aRegA, BYTE aRegB, WORD aData);

        /// <summary>
        /// Loads a unsigned halfword from the data in register B into register A.
        /// --- HALFWORD LOAD NOT IMPLEMENTED IN RISC-V VM as of 2022!  -> interpreted as "normal" load word!
        /// </summary>
        /// <param name="aRegA">Target register</param>
        /// <param name="aRegB">Source register holding the value to load</param>
        /// <param name="aData">Additional target address offset</param>
        void LoadHU(BYTE aRegA, BYTE aRegB, WORD aData);

        /// <summary>
        /// Stores the data in register A to the address in the memory held by register B. 
        /// An additional address offset to the address in register B can be given via aData.
        /// </summary>
        /// <param name="aRegA">Holding the data to store</param>
        /// <param name="aRegB">Holding the memory address to store to</param>
        /// <param name="aData">Additional target address offset</param>
        virtual void Store(BYTE aRegA, BYTE aRegB, WORD aData) override;

        /// <summary>
        /// Stores a byte of the data in register A to the address in the memory held by register B. 
        /// An additional address offset to the address in register B can be given via aData.
        /// --- BYTEWISE STORE NOT IMPLEMENTED IN RISC-V VM as of 2022!  -> interpreted as "normal" store word!
        /// </summary>
        /// <param name="aRegA">Holding the data to store</param>
        /// <param name="aRegB">Holding the memory address to store to</param>
        /// <param name="aData">Additional target address offset</param>
        void StoreB(BYTE aRegA, BYTE aRegB, WORD aData);

        /// <summary>
        /// Stores a halfword of the data in register A to the address in the memory held by register B. 
        /// An additional address offset to the address in register B can be given via aData.
        /// --- HALFWORD STORE NOT IMPLEMENTED IN RISC-V VM as of 2022!  -> interpreted as "normal" store word!
        /// </summary>
        /// <param name="aRegA">Holding the data to store</param>
        /// <param name="aRegB">Holding the memory address to store to</param>
        /// <param name="aData">Additional target address offset</param>
        void StoreH(BYTE aRegA, BYTE aRegB, WORD aData);

        /// <summary>
        /// Moves/Copies the value from register B into register A.
        /// </summary>
        /// <param name="aRegA">Target register</param>
        /// <param name="aRegB">Source register</param>
        virtual void Move(BYTE aRegA, BYTE aRegB) override;

        // arithmetic operations

        /// <summary>
        /// Sums up the values from register A and register B and saves the result in register Res.
        /// </summary>
        /// <param name="aRegA">Holding value 1</param>
        /// <param name="aRegB">Holding value 2</param>
        /// <param name="aRegRes">Target register, holding the sum of the values saved in the source registers</param>
        virtual void Add(BYTE aRegA, BYTE aRegB, BYTE aRegRes) override;

        /// <summary>
        /// Subtracts the value from register B from register A and saves the result in register Res.
        /// </summary>
        /// <param name="aRegA">Holding value 1</param>
        /// <param name="aRegB">Holding value 2</param>
        /// <param name="aRegRes">Target register, holding the result of the subtraction.</param>
        virtual void Sub(BYTE aRegA, BYTE aRegB, BYTE aRegRes) override;

        /// <summary>
        /// Increases the value in register A by 1.
        /// </summary>
        /// <param name="aRegA">Holding the value that is to be increased by 1</param>
        virtual void Inc(BYTE aRegA) override;

        /// <summary>
        /// Decreases the value in register A by 1.
        /// </summary>
        /// <param name="aRegA">Holding the value that is to be reduced by 1</param>
        virtual void Dec(BYTE aRegA) override;

        /// <summary>
        /// Multiplication, multiplies register A with register B and saves the result 
        /// into the given result register.
        /// </summary>
        /// <param name="aRegA"> Multiplicand </param>
        /// <param name="aRegB"> Multiplier </param>
        /// <param name="aRegRes"> Result register</param>
        /// <param name="aRegTmp"> Temporary register for calculation </param>
        virtual void Mul(BYTE aRegA, BYTE aRegB, BYTE aRegRes, BYTE aRegTmp) override;

        /// <summary>
        /// Multiplication, multiplies register A with register B and saves the result 
        /// into the given result register.
        /// </summary>
        /// <param name="aRegA"> Multiplicand </param>
        /// <param name="aRegB"> Multiplier </param>
        /// <param name="aRegRes"> Result register</param>
        /// <param name="aRegTmp"> Temporary register for calculation </param>
        void MulH(BYTE aRegA, BYTE aRegB, BYTE aRegRes, BYTE aRegTmp);

        /// <summary>
        /// Multiplication, multiplies register A with register B and saves the result 
        /// into the given result register. Takes both values as halfword, while the 
        /// value A is seen as signed and value B as unsigned.
        /// </summary>
        /// <param name="aRegA"> Multiplicand, signed </param>
        /// <param name="aRegB"> Multiplier, unsigned </param>
        /// <param name="aRegRes"> Result register</param>
        /// <param name="aRegTmp"> Temporary register for calculation </param>
        void MulHSU(BYTE aRegA, BYTE aRegB, BYTE aRegRes, BYTE aRegTmp);

        /// <summary>
        /// Multiplication, multiplies register A with register B and saves the result 
        /// into the given result register. Takes both values as unsigned halfwords.
        /// </summary>
        /// <param name="aRegA"> Multiplicand, signed </param>
        /// <param name="aRegB"> Multiplier, unsigned </param>
        /// <param name="aRegRes"> Result register</param>
        /// <param name="aRegTmp"> Temporary register for calculation </param>
        void MulHU(BYTE aRegA, BYTE aRegB, BYTE aRegRes, BYTE aRegTmp);

        /// <summary>
        /// Divides the value in register A by register B and saves the result in register Res. The other parameters are 
        /// optional and may be needed for this calculation depending on the code generator's implementation.
        /// </summary>
        /// <param name="aRegA"> Holding the dividend</param>
        /// <param name="aRegB"> Holding the divisor</param>
        /// <param name="aRegRes"> Holding the quotient (result) of the division</param>
        /// <param name="aRegRemainder"> Remainder of division</param>
        /// <param name="aRegTmp"> Temporary register </param>
        /// <param name="aRegBits"> Register for number of bits (in case of calculation via loop)</param>
        virtual void Div(BYTE aRegA, BYTE aRegB, BYTE aRegRes, BYTE aRegRemainder, BYTE aRegTmp, BYTE aRegBits) override;

        /// <summary>
        /// Divides the value in register A by register B and saves the result in register Res. Both dividend and divisor
        /// are taken as unsigned.
        /// </summary>
        /// <param name="aRegDest"> Holding the quotient (result) of the division</param>
        /// <param name="aRegA"> Holding the dividend, unsigned</param>
        /// <param name="aRegB"> Holding the divisor, unsigned</param>
        void DivU(BYTE aRegDest, BYTE aRegA, BYTE aRegB);

        /// <summary>
        /// Calculates a normal division with the values from register A and register B, but saves 
        /// the remainder and not the quotient into the result register.
        /// </summary>
        /// <param name="aRegRemainder">Target register, holding the reminder of the division</param>
        /// <param name="aRegA">Holding the dividend</param>
        /// <param name="aRegB">Holding the divisor</param>
        void Rem(BYTE aRegRemainder, BYTE aRegA, BYTE aRegB);

        /// <summary>
        /// Calculates a unsigned division with the values from register A and register B, but saves 
        /// the remainder and not the quotient into the result register.
        /// </summary>
        /// <param name="aRegRemainder">Target register, holding the reminder of the division</param>
        /// <param name="aRegA">Holding the dividend, unsigned</param>
        /// <param name="aRegB">Holding the divisor, unsigned</param>
        void RemU(BYTE aRegRemainder, BYTE aRegA, BYTE aRegB);

        /// <summary>
        /// Writes a "1" into the target register if value in register A is less than value in register B; else writes "0". 
        /// </summary>
        /// <param name="aRegDest">Target register to be set if the condition is true</param>
        /// <param name="aRegA">Holding value A</param>
        /// <param name="aRegB">Holding value B</param>
        void SetLessThan(BYTE aRegDest, BYTE aRegA, BYTE aRegB);

        /// <summary>
        /// Writes a "1" into the target register if value in register A is less than value in register B; else writes "0". 
        /// Takes both values as unsigned.
        /// </summary>
        /// <param name="aRegDest">Target register to be set if the condition is true</param>
        /// <param name="aRegA">Holding value A, unsigned</param>
        /// <param name="aRegB">Holding value B, unsigned</param>
        void SetLessThanU(BYTE aRegDest, BYTE aRegA, BYTE aRegB);

        /// <summary>
        /// Writes a "1" into the target register if value in register A is less than the constant; else writes "0". 
        /// </summary>
        /// <param name="aRegDest">Target register to be set if the condition is true</param>
        /// <param name="aRegA">Holding value A</param>
        /// <param name="aRegB">WORD sized constant</param>
        void SetLessThanI(BYTE aRegDest, BYTE aRegA, WORD aData);

        /// <summary>
        /// Writes a "1" into the target register if value in register A is less than the constant; else writes "0". 
        /// Takes both values as unsigned.
        /// </summary>
        /// <param name="aRegDest">Target register to be set if the condition is true</param>
        /// <param name="aRegA">Holding value A, unsigned</param>
        /// <param name="aRegB">WORD sized constant, unsigned</param>
        void SetLessThanIU(BYTE aRegDest, BYTE aRegA, WORD aData);

        // logical operations

        /// <summary>
        /// Calculates the AND operation with the values in register A and register B. 
        /// The result is saved into the register Res.
        /// </summary>
        /// <param name="aRegA">Holding value 1</param>
        /// <param name="aRegB">Holding value 2</param>
        /// <param name="aRegRes">Target register</param>
        virtual void And(BYTE aRegA, BYTE aRegB, BYTE aRegRes) override;

        /// <summary>
        /// Calculates the AND operation with the value in register A and a WORD sized constant. 
        /// The result is saved into the target register.
        /// </summary>
        /// <param name="aRegA">Holding value 1</param>
        /// <param name="aRegDest">Target register</param>
        /// <param name="aData">WORD sized constant</param>
        void AndI(BYTE aRegA, BYTE aRegDest, WORD aData);

        /// <summary>
        /// Calculates the OR operation with the values in register A and register B. 
        /// The result is saved into the register Res.
        /// </summary>
        /// <param name="aRegA">Holding value 1</param>
        /// <param name="aRegB">Holding value 2</param>
        /// <param name="aRegRes">Target register</param>
        virtual void Or(BYTE aRegA, BYTE aRegB, BYTE aRegRes) override;

        /// <summary>
        /// Calculates the OR operation with the value in register A and a WORD sized constant. 
        /// The result is saved into the target register.
        /// </summary>
        /// <param name="aRegA">Holding value 1</param>
        /// <param name="aRegDest">Target register</param>
        /// <param name="aData">WORD sized constant</param>
        void OrI(BYTE aRegA, BYTE aRegDest, WORD aData);

        /// <summary>
        /// Calculates the XOR operation with the values in register A and register B. 
        /// The result is saved into the register Res.
        /// </summary>
        /// <param name="aRegA">Holding value 1</param>
        /// <param name="aRegB">Holding value 2</param>
        /// <param name="aRegRes">Target register</param>
        virtual void XOr(BYTE aRegA, BYTE aRegB, BYTE aRegRes) override;

        /// <summary>
        /// Calculates the XOR operation with the value in register A and a WORD sized constant. 
        /// The result is saved into the target register.
        /// </summary>
        /// <param name="aRegA">Holding value 1</param>
        /// <param name="aRegDest">Target register</param>
        /// <param name="aData">WORD sized constant</param>
        void XOrI(BYTE aRegA, BYTE aRegDest, WORD aData);

        /// <summary>
        /// Inverts the value in register A. Expands to an XORI operation with the value in register A and -1 as the immediate.
        /// The result is saved into the register Res.
        /// </summary>
        /// <param name="aRegA">Holding value to invert</param>
        /// <param name="aRegRes">Target register</param>
        virtual void Not(BYTE aRegA, BYTE aRegRes) override;

        /// <summary>
        /// Shift left. The value in register A gets shifted to the left by the number of bits
        /// defined with the value in register B.
        /// The shifted value is saved in register Res.
        /// </summary>
        /// <param name="aRegA">Holding value to shift</param>
        /// <param name="aRegB">Holding the shift amount</param>
        /// <param name="aRegRes">Target register holding the shifted value</param>
        virtual void ShL(BYTE aRegA, BYTE aRegB, BYTE aRegRes) override;

        /// <summary>
        /// Shift left logical immediate. Shifts the value in register A logical to the left by 
        /// the 6 least significant bits of aData (= shamt). Saves the result to the target register. 
        /// </summary>
        /// <param name="aRegA">Holding the value to shift</param>
        /// <param name="aRegDest">Holding the shifted value</param>
        /// <param name="aData">Holding the shift amount</param>
        void ShLLI(BYTE aRegA, BYTE aRegDest, WORD aData);

        /// <summary>
        /// Shift right. The value in register A gets shifted to the right by the number of bits
        /// defined with the value in register B.
        /// The shifted value is saved in register Res.
        /// </summary>
        /// <param name="aRegA">Holding value to shift</param>
        /// <param name="aRegB">Holding the shift amount</param>
        /// <param name="aRegRes">Target register holding the shifted value</param>
        virtual void ShR(BYTE aRegA, BYTE aRegB, BYTE aRegRes) override;

        /// <summary>
        /// Shift right arithmetic. Shifts the value in register A arithmetically to the right by 
        /// the value in register B. Saves the result to the target register. 
        /// </summary>
        /// <param name="aRegA">Holding the value to shift</param>
        /// <param name="aRegDest">Holding the shifted value</param>
        /// <param name="aData">Holding the shift amount</param>
        void ShRA(BYTE aRegA, BYTE aRegB, BYTE aRegRes);

        /// <summary>
        /// Shift right logical immediate. Shifts the value in register A logical to the right by 
        /// the 6 least significant bits of aData (= shamt). Saves the result to the target register. 
        /// </summary>
        /// <param name="aRegA">Holding the value to shift</param>
        /// <param name="aRegDest">Holding the shifted value</param>
        /// <param name="aData">Holding the shift amount</param>
        void ShRLI(BYTE aRegA, BYTE aRegDest, WORD aData);

        /// <summary>
        /// Shift right arithmetic immediate. Shifts the value in register A arithmetically to the right by 
        /// the 6 least significant bits of aData (= shamt). Saves the result to the target register. 
        /// </summary>
        /// <param name="aRegA">Holding the value to shift</param>
        /// <param name="aRegDest">Holding the shifted value</param>
        /// <param name="aData">Holding the shift amount</param>
        void ShRAI(BYTE aRegA, BYTE aRegDest, WORD aData);

        // program branch

        /// <summary>
        /// Jumps to the given address. The register A is used to execute an 
        /// jump-and-link instruction (saves the current program counter into 
        /// register A). 
        /// Returns the code position of this instruction in the code generator's 
        /// instruction memory (as in the LoadI instruction)  
        /// </summary>
        /// <param name="aRegA">Register for Jump-and-Link (gets an updated program counter)</param>
        /// <param name="aRegTmp">Temporary register, not used</param>
        /// <param name="addr">Address of instruction to jump to</param>
        /// <returns>Returns the position of this statement in the code generator's machinecode buffer</returns>
        virtual WORD Jump(BYTE aRegA, BYTE aRegTmp, WORD addr) override;

        /// <summary>
        /// Executes a jump to the given address only if the values in register A and B are equal.
        /// </summary>
        /// <param name="aRegA">Holding comparison value 1</param>
        /// <param name="aRegB">Holding comparison value 2</param>
        /// <param name="aRegTmp">Optional temporary register, not used</param>
        /// <param name="addr">Address of instruction to jump to</param>
        /// <returns>Returns the code position of this instruction</returns>
        virtual WORD JumpEQ(BYTE aRegA, BYTE aRegB, BYTE aRegTmp, WORD addr) override;

        /// <summary>
        /// Executes a jump to the given address only if the values in register A and B are not equal.
        /// </summary>
        /// <param name="aRegA">Holding comparison value 1</param>
        /// <param name="aRegB">Holding comparison value 2</param>
        /// <param name="aRegTmp">Optional temporary register, not used</param>
        /// <param name="addr">Address of instruction to jump to</param>
        /// <returns>Returns the code position of this instruction</returns>
        virtual WORD JumpNEQ(BYTE aRegA, BYTE aRegB, BYTE aRegTmp, WORD addr) override;

        /// <summary>
        /// Executes a jump to the given address only if the value in register A is less than the value in register B.
        /// </summary>
        /// <param name="aRegA">Holding comparison value 1</param>
        /// <param name="aRegB">Holding comparison value 2</param>
        /// <param name="aRegTmp">Optional temporary register, not used</param>
        /// <param name="addr">Address of instruction to jump to</param>
        /// <returns>Returns the code position of this instruction</returns>
        virtual WORD JumpL(BYTE aRegA, BYTE aRegB, BYTE aRegTmp, WORD addr) override;

        /// <summary>
        /// Executes a jump to the given address only if the value in register A is less or equal than the value in register B.
        /// </summary>
        /// <param name="aRegA">Holding comparison value 1</param>
        /// <param name="aRegB">Holding comparison value 2</param>
        /// <param name="aRegTmp">Optional temporary register, not used</param>
        /// <param name="addr">Address of instruction to jump to</param>
        /// <returns>Returns the code position of this instruction</returns>
        virtual WORD JumpLE(BYTE aRegA, BYTE aRegB, BYTE aRegTmp, WORD addr) override;

        /// <summary>
        /// Executes a jump to the given address only if the value in register A is greater than the value in register B.
        /// </summary>
        /// <param name="aRegA">Holding comparison value 1</param>
        /// <param name="aRegB">Holding comparison value 2</param>
        /// <param name="aRegTmp">Optional temporary register, not used</param>
        /// <param name="addr">Address of instruction to jump to</param>
        /// <returns>Returns the code position of this instruction</returns>
        virtual WORD JumpG(BYTE aRegA, BYTE aRegB, BYTE aRegTmp, WORD addr) override;

        /// <summary>
        /// Executes a jump to the given address only if the value in register A is greater or equal than the value in register B.
        /// </summary>
        /// <param name="aRegA">Holding comparison value 1</param>
        /// <param name="aRegB">Holding comparison value 2</param>
        /// <param name="aRegTmp">Optional temporary register</param>
        /// <param name="addr">Address of instruction to jump to</param>
        /// <returns>Returns the code position of this instruction</returns>
        virtual WORD JumpGE(BYTE aRegA, BYTE aRegB, BYTE aRegTmp, WORD addr) override;

        /// <summary>
        /// Executes a jump to the given address only if the value in register A is greater or equal than the value in register B.
        /// Takes both values as unsigned.
        /// </summary>
        /// <param name="aRegA">Holding comparison value 1</param>
        /// <param name="aRegB">Holding comparison value 2</param>
        /// <param name="aRegTmp">Optional temporary register, not used</param>
        /// <param name="addr">Address of instruction to jump to</param>
        /// <returns>Returns the code position of this instruction</returns>
        WORD JumpGEU(BYTE aRegA, BYTE aRegB, BYTE aRegTmp, WORD addr);

        /// <summary>
        /// Executes a jump to the given address only if the value in register A is less than the value in register B.
        /// Takes both values as unsigned.
        /// </summary>
        /// <param name="aRegA">Holding comparison value 1</param>
        /// <param name="aRegB">Holding comparison value 2</param>
        /// <param name="aRegTmp">Optional temporary register, not used</param>
        /// <param name="addr">Address of instruction to jump to</param>
        /// <returns>Returns the code position of this instruction</returns>
        WORD JumpLTU(BYTE aRegA, BYTE aRegB, BYTE aRegTmp, WORD addr);

        /// <summary>
        /// Jump Register. Executes a direct jump to the address saved in register A and saves the current PC to the 
        /// destination register (Jump And Link Register - JALR).
        /// </summary>
        /// <param name="aRegA">Holding the target address to jump to</param>
        /// <param name="aRegDest">Holding the link of the jump</param>
        /// <param name="addr">Additional target address offset</param>
        /// <returns></returns>
        WORD JumpR(BYTE aRegA, BYTE aRegDest, WORD addr);
 
        // program control

        /// <summary>
        /// No operation. Extends to addi x0, x0, 0.
        /// </summary>
        virtual void Nop() override;

        /// <summary>
        /// This instruction is used for exiting the main program.
        /// Takes the user defined op code PType::OP_TYPE_SLEEP.
        /// </summary>
        virtual void Sleep() override;

        // output to console

        /// <summary>
        /// Instruction to print the given value as an integer to the executing machine.
        /// </summary>
        /// <param name="aRegA">Holding the value to print</param>
        virtual void PrintInt(BYTE aRegA) override;

        /// <summary>
        /// Instruction to print the given value as a string to the executing machine.
        /// </summary>
        /// <param name="aRegA">Holding the value to print</param>
        virtual void PrintString(BYTE aRegA) override;


        // helper
        
        /// <summary>
        /// Returns the current (= tail) position in the code generator's instruction buffer.
        /// </summary>
        /// <returns></returns>
        virtual WORD GetCodePosition() const override;

        /// <summary>
        /// Stores the address specified by aAddress at the code-position specified by aCodePos
        /// </summary>
        /// <param name="aCodePos">Target position in code generator's instruction buffer</param>
        /// <param name="aAddress">Address value to save</param>
        virtual void SetAddress(WORD aCodePos, WORD aAddress) override;

        /// <summary>
        /// Writes the whole instruction buffer of the code generator into the given output stream.
        /// </summary>
        /// <param name="os">Output stream (file) to write the instruction buffer into</param>
        virtual void WriteExecutable(std::ostream& os) override;

        /// <summary>
        /// Writes the whole buffer holding the disassembled machine code into the given output stream.
        /// </summary>
        /// <param name="os">Output stream (file) to write the disassembled machine code buffer into</param>
        void WriteDisassembled(std::ostream& os);

        /// <summary>
        /// Returns the number of registers of the code generator.
        /// </summary>
        /// <returns></returns>
        virtual int GetRegCnt() const override;

        static const int cRegisterCount;

    private:
        // instruction formats

        /// <summary>
        /// Creates a register type (R-type) machine code from the given parameters. 
        /// The machine code is appended to the internal instruction buffer.
        /// </summary>
        /// <param name="opcode">Code of the operation</param>
        /// <param name="regDest">Target register</param>
        /// <param name="regSrc1">Source register 1</param>
        /// <param name="regSrc2">Source register 2</param>
        /// <param name="funct3">Additional 3-bit long opcode for further operation specification</param>
        /// <param name="funct7">Additional 7-bit long opcode for further operation specification</param>
        void AppendCommand_RType(BYTE opcode, BYTE regDest, BYTE regSrc1, BYTE regSrc2, BYTE funct3, BYTE funct7);  

        /// <summary>
        /// Creates an immediate and load type (I-type) machine code from the given parameters. 
        /// The machine code is appended to the internal instruction buffer.
        /// </summary>
        /// <param name="opcode">Code of the operation</param>
        /// <param name="regDest">Target register</param>
        /// <param name="regSrc1">Source register 1</param>
        /// <param name="funct3">Additional 3-bit long opcode for further operation specification</param>
        /// <param name="imm">Additional WORD sized constant (immediate)</param>
        void AppendCommand_IType(BYTE opcode, BYTE regDest, BYTE regSrc1, BYTE funct3, WORD imm);

        /// <summary>
        /// Creates an store type (S-type) machine code from the given parameters. 
        /// The machine code is appended to the internal instruction buffer.
        /// </summary>
        /// <param name="opcode">Code of the operation</param>
        /// <param name="regSrc1">Source register 1</param>
        /// <param name="regSrc2">Source register 2</param>
        /// <param name="funct3">Additional 3-bit long opcode for further operation specification</param>
        /// <param name="imm">Additional WORD sized constant (immediate)</param>
        void AppendCommand_SType(BYTE opcode, BYTE regSrc1, BYTE regSrc2, BYTE funct3, WORD imm);  

        /// <summary>
        /// Creates a branch type (B-type) machine code from the given parameters. 
        /// The machine code is appended to the internal instruction buffer.
        /// </summary>
        /// <param name="opcode">Code of the operation</param>
        /// <param name="regSrc1">Source register 1</param>
        /// <param name="regSrc2">Source register 2</param>
        /// <param name="funct3">Additional 3-bit long opcode for further operation specification</param>
        /// <param name="imm">Additional WORD sized constant (immediate)</param>
        void AppendCommand_BType(BYTE opcode, BYTE regSrc1, BYTE regSrc2, BYTE funct3, WORD imm); 

        /// <summary>
        /// Creates an upper immediate type (U-type) machine code from the given parameters. 
        /// The machine code is appended to the internal instruction buffer.
        /// </summary>
        /// <param name="opcode">Code of the operation</param>
        /// <param name="regDest">Target register</param>
        /// <param name="imm">Additional WORD sized constant (immediate)</param>
        void AppendCommand_UType(BYTE opcode, BYTE regDest, WORD imm);  

        /// <summary>
        /// Creates a jump type (J-type) machine code from the given parameters. 
        /// The machine code is appended to the internal instruction buffer.
        /// </summary>
        /// <param name="opcode">Code of the operation</param>
        /// <param name="regDest">Target register</param>
        /// <param name="imm">Additional WORD sized constant (immediate)</param>
        void AppendCommand_JType(BYTE opcode, BYTE regDest, WORD imm);

        /// <summary>
        /// Creates a private type (P-type) machine code from the given parameters. 
        /// The machine code is appended to the internal instruction buffer.
        /// </summary>
        /// <param name="opcode">Code of the operation</param>
        /// <param name="funct3">Additional 3-bit long opcode for further operation specification</param>
        /// <param name="regSrc">Source register</param>
        void AppendCommand_PType(BYTE opcode, BYTE func3, BYTE regSrc);

        // helper

        /// <summary>
        /// Appends a WORD sized machine code to the internal instruction buffer.
        /// </summary>
        /// <param name="aWord"></param>
        void AppendWord(WORD aWord);

        /// <summary>
        /// Disassembles the given machine code and saves it to the internal disassembled buffer.
        /// If the code gen was constructed with mPrintDisasmOnTerminal=true, the disassembled 
        /// code is additionally printed on the execution terminal after disassembling.
        /// </summary>
        /// <param name="machinecode"></param>
        void Disassemble(WORD machinecode);

        // member variables

        const unsigned int cGrowSize;

        bool mDisassemble;
        bool mPrintDisasmOnTerminal;
        std::stringstream disasm;
    };

} // namespace MIEC