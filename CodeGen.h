#pragma once

#include "Object.h"
#include <ostream>


typedef int8_t BYTE;

// interface for all code generators, i.e. CodeGenProl16 or CodeGenRiscV

template<typename W>
class CodeGen : public Object
{
public:
    using WORD = W;
    // data transfer

    /// <summary>
    /// Load Immediate. Loads a constant with length WORD into the given register.
    /// </summary>
    /// <param name="aReg">Target Register</param>
    /// <param name="aData">WORD long constant to load into register</param>
    /// <returns>Returns the position of this statement in the code generator's code buffer (machinecode address) </returns>
    virtual WORD LoadI(BYTE aReg, WORD aData) = 0;

    /// <summary>
    /// Loads the value from the address held by register B into register A. 
    /// The optional data can be used as an additional target address offset. 
    /// </summary>
    /// <param name="aRegA">Target register</param>
    /// <param name="aRegB">Source register holding the memory address to load from</param>
    /// <param name="aData">Additional target address offset</param>
    virtual void Load(BYTE aRegA, BYTE aRegB, WORD aData) = 0;

    /// <summary>
    /// Stores the data in register A to the address in the memory held by register B. 
    /// An additional address offset to the address in register B can be given via aData.
    /// </summary>
    /// <param name="aRegA">Holding the data to store</param>
    /// <param name="aRegB">Holding the memory address to store to</param>
    /// <param name="aData">Additional target address offset</param>
    virtual void Store(BYTE aRegA, BYTE aRegB, WORD aData) = 0;

    /// <summary>
    /// Moves/Copies the value from register B into register A.
    /// </summary>
    /// <param name="aRegA">Target register</param>
    /// <param name="aRegB">Source register</param>
    virtual void Move(BYTE aRegA, BYTE aRegB) = 0;

    // arithmetic operations

    /// <summary>
    /// Sums up the values from register A and register B and saves the result in register Res.
    /// </summary>
    /// <param name="aRegA">Holding value 1</param>
    /// <param name="aRegB">Holding value 2</param>
    /// <param name="aRegRes">Target register, holding the sum of the values saved in the source registers</param>
    virtual void Add(BYTE aRegA, BYTE aRegB, BYTE aRegRes) = 0;

    /// <summary>
    /// Subtracts the value from register B from register A and saves the result in register Res.
    /// </summary>
    /// <param name="aRegA">Holding value 1</param>
    /// <param name="aRegB">Holding value 2</param>
    /// <param name="aRegRes">Target register, holding the result of the subtraction.</param>
    virtual void Sub(BYTE aRegA, BYTE aRegB, BYTE aRegRes) = 0;

    /// <summary>
    /// Increases the value in register A by 1.
    /// </summary>
    /// <param name="aRegA">Holding the value that is to be increased by 1</param>
    virtual void Inc(BYTE aRegA) = 0;

    /// <summary>
    /// Decreases the value in register A by 1.
    /// </summary>
    /// <param name="aRegA">Holding the value that is to be reduced by 1</param>
    virtual void Dec(BYTE aRegA) = 0;

    /// <summary>
    /// Multiplication, multiplies register A with register B and saves the result 
    /// into the given result register.
    /// </summary>
    /// <param name="aRegA"> Multiplicand </param>
    /// <param name="aRegB"> Multiplier </param>
    /// <param name="aRegRes"> Result register</param>
    /// <param name="aRegTmp"> Temporary register for calculation </param>
    virtual void Mul(BYTE aRegA, BYTE aRegB, BYTE aRegRes, BYTE aRegTmp) = 0;

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
    virtual void Div(BYTE aRegA, BYTE aRegB, BYTE aRegRes, BYTE aRegRemainder, BYTE aRegTmp, BYTE aRegBits) = 0;

    // logical operations

    /// <summary>
    /// Calculates the AND operation with the values in register A and register B. 
    /// The result is saved into the register Res.
    /// </summary>
    /// <param name="aRegA">Holding value 1</param>
    /// <param name="aRegB">Holding value 2</param>
    /// <param name="aRegRes">Target register</param>
    virtual void And(BYTE aRegA, BYTE aRegB, BYTE aRegRes) = 0;

    /// <summary>
    /// Calculates the OR operation with the values in register A and register B. 
    /// The result is saved into the register Res.
    /// </summary>
    /// <param name="aRegA">Holding value 1</param>
    /// <param name="aRegB">Holding value 2</param>
    /// <param name="aRegRes">Target register</param>
    virtual void Or(BYTE aRegA, BYTE aRegB, BYTE aRegRes) = 0;

    /// <summary>
    /// Calculates the XOR operation with the values in register A and register B. 
    /// The result is saved into the register Res.
    /// </summary>
    /// <param name="aRegA">Holding value 1</param>
    /// <param name="aRegB">Holding value 2</param>
    /// <param name="aRegRes">Target register</param>
    virtual void XOr(BYTE aRegA, BYTE aRegB, BYTE aRegRes) = 0;

    /// <summary>
    /// Inverts the value in register A. Implementation depending on the code generator (normally expands to XORI operation).
    /// The result is saved into the register Res.
    /// </summary>
    /// <param name="aRegA">Holding value to invert</param>
    /// <param name="aRegRes">Target register</param>
    virtual void Not(BYTE aRegA, BYTE aRegRes) = 0;

    /// <summary>
    /// Shift left. The value in register A gets shifted to the left by the number of bits
    /// defined with the value in register B.
    /// The shifted value is saved in register Res.
    /// </summary>
    /// <param name="aRegA">Holding value to shift</param>
    /// <param name="aRegB">Holding the shift amount</param>
    /// <param name="aRegRes">Target register holding the shifted value</param>
    virtual void ShL(BYTE aRegA, BYTE aRegB, BYTE aRegRes) = 0;

    /// <summary>
    /// Shift right. The value in register A gets shifted to the right by the number of bits
    /// defined with the value in register B.
    /// The shifted value is saved in register Res.
    /// </summary>
    /// <param name="aRegA">Holding value to shift</param>
    /// <param name="aRegB">Holding the shift amount</param>
    /// <param name="aRegRes">Target register holding the shifted value</param>
    virtual void ShR(BYTE aRegA, BYTE aRegB, BYTE aRegRes) = 0;

    // program branch

    /// <summary>
    /// Jumps to the given address. Depending on the code generator, the register A is used 
    /// to executed an jump-and-link instruction (saves the current program counter into 
    /// register A) and the temporary register Tmp might also be needed. 
    /// Returns the code position of this instruction in the code generator's 
    /// instruction memory (as in the LoadI instruction)
    /// </summary>
    /// <param name="aRegA">Register for Jump-and-Link (gets an updated program counter)</param>
    /// <param name="aRegTmp">Temporary register, optional</param>
    /// <param name="addr">Address of instruction to jump to</param>
    /// <returns>Returns the position of this statement in the code generator's machinecode buffer</returns>
    virtual WORD Jump(BYTE aRegA, BYTE aRegTmp, WORD addr) = 0;

    // all of the following jump methods compare regA with regB according to the method name
    // if the jump was done, the code position of the jump in the instruction memory is returned (as in the LoadI instruction)
    // aRegA and aRegB for comparison, regTmp for if needed and addr address to jump to

    /// <summary>
    /// Executes a jump to the given address only if the values in register A and B are equal.
    /// </summary>
    /// <param name="aRegA">Holding comparison value 1</param>
    /// <param name="aRegB">Holding comparison value 2</param>
    /// <param name="aRegTmp">Optional temporary register</param>
    /// <param name="addr">Address of instruction to jump to</param>
    /// <returns>Returns the code position of this instruction</returns>
    virtual WORD JumpEQ(BYTE aRegA, BYTE aRegB, BYTE aRegTmp, WORD addr) = 0;

    /// <summary>
    /// Executes a jump to the given address only if the values in register A and B are not equal.
    /// </summary>
    /// <param name="aRegA">Holding comparison value 1</param>
    /// <param name="aRegB">Holding comparison value 2</param>
    /// <param name="aRegTmp">Optional temporary register</param>
    /// <param name="addr">Address of instruction to jump to</param>
    /// <returns>Returns the code position of this instruction</returns>
    virtual WORD JumpNEQ(BYTE aRegA, BYTE aRegB, BYTE aRegTmp, WORD addr) = 0;

    /// <summary>
    /// Executes a jump to the given address only if the value in register A is less than the value in register B.
    /// </summary>
    /// <param name="aRegA">Holding comparison value 1</param>
    /// <param name="aRegB">Holding comparison value 2</param>
    /// <param name="aRegTmp">Optional temporary register</param>
    /// <param name="addr">Address of instruction to jump to</param>
    /// <returns>Returns the code position of this instruction</returns>
    virtual WORD JumpL(BYTE aRegA, BYTE aRegB, BYTE aRegTmp, WORD addr) = 0;

    /// <summary>
    /// Executes a jump to the given address only if the value in register A is less or equal than the value in register B.
    /// </summary>
    /// <param name="aRegA">Holding comparison value 1</param>
    /// <param name="aRegB">Holding comparison value 2</param>
    /// <param name="aRegTmp">Optional temporary register</param>
    /// <param name="addr">Address of instruction to jump to</param>
    /// <returns>Returns the code position of this instruction</returns>
    virtual WORD JumpLE(BYTE aRegA, BYTE aRegB, BYTE aRegTmp, WORD addr) = 0;

    /// <summary>
    /// Executes a jump to the given address only if the value in register A is greater than the value in register B.
    /// </summary>
    /// <param name="aRegA">Holding comparison value 1</param>
    /// <param name="aRegB">Holding comparison value 2</param>
    /// <param name="aRegTmp">Optional temporary register</param>
    /// <param name="addr">Address of instruction to jump to</param>
    /// <returns>Returns the code position of this instruction</returns>
    virtual WORD JumpG(BYTE aRegA, BYTE aRegB, BYTE aRegTmp, WORD addr) = 0;

    /// <summary>
    /// Executes a jump to the given address only if the value in register A is greater or equal than the value in register B.
    /// </summary>
    /// <param name="aRegA">Holding comparison value 1</param>
    /// <param name="aRegB">Holding comparison value 2</param>
    /// <param name="aRegTmp">Optional temporary register</param>
    /// <param name="addr">Address of instruction to jump to</param>
    /// <returns>Returns the code position of this instruction</returns>
    virtual WORD JumpGE(BYTE aRegA, BYTE aRegB, BYTE aRegTmp, WORD addr) = 0;

    /// <summary>
    /// No operation. This instruction is used for if nothing shall be done.
    /// Implementation depending on the code generator.
    /// </summary>
    virtual void Nop() = 0;

    /// <summary>
    /// This instruction is used for exiting the main program.
    /// Implementation depending ón the code generator.
    /// </summary>
    virtual void Sleep() = 0;

    /// <summary>
    /// Returns the current (= tail) position in the code generator's instruction buffer.
    /// </summary>
    /// <returns></returns>
    virtual WORD GetCodePosition() const = 0;

    /// <summary>
    /// Stores the address specified by aAddress at the code-position specified by aCodePos
    /// </summary>
    /// <param name="aCodePos">Target position in code generator's instruction buffer</param>
    /// <param name="aAddress">Address value to save</param>
    virtual void SetAddress(WORD aCodePos, WORD aAddress) = 0;

    // output to console

    /// <summary>
    /// Instruction to print the given value as an integer to the executing machine.
    /// </summary>
    /// <param name="aRegA">Holding the value to print</param>
    virtual void PrintInt(BYTE aRegA) = 0;

    /// <summary>
    /// Instruction to print the given value as a string to the executing machine.
    /// </summary>
    /// <param name="aRegA">Holding the value to print</param>
    virtual void PrintString(BYTE aRegA) = 0;

    /// <summary>
    /// Writes the whole instruction buffer of the code generator into the given output stream.
    /// </summary>
    /// <param name="os">Output stream (file) to write the instruction buffer into</param>
    virtual void WriteExecutable(std::ostream& os) = 0;


    // helper

    /// <summary>
    /// Returns the number of registers of the code generator.
    /// </summary>
    /// <returns></returns>
    virtual int GetRegCnt() const = 0;

    BYTE* mpBuffer = nullptr;
    size_t mBufLength = 0;
    WORD mBufPos = 0;
};
