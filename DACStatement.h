#ifndef DACSTATEMENT_H
#define DACSTATEMENT_H


namespace MIEC {

	// TODO; delete when operand is implemented
	class Operand
	{
	public:
		Operand();
		~Operand();

	private:

	};

	Operand::Operand()
	{
	}

	Operand::~Operand()
	{
	}

	// Operand kind
	enum class TOpKind {
		eAdd, eSubtract, eMultiply, eDivide, eIsEqual, eIsLessEqual, eIsGreaterEqual,
		eIsNotEqual, eIsLess, eIsGreater, eAssign, eJump, eIfJump, eIfFalseJump, ePrint,
		eExit
	};
	

	// Store information about a single operation line
	class DACStatement
	{
	public:
		DACStatement(Operand const& firstOp, Operand const& secondOp, TOpKind const op);
		virtual ~DACStatement() = default;
		Operand GetFirst();
		Operand GetSecond();
		TOpKind GetOpType();

	private:
		Operand mFirstOperand;
		Operand mSecondOperand;
		TOpKind mOpperation;
	};
}




#endif // !DACSTATEMENT_H
