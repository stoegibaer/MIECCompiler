#include "DACStatement.h"

namespace MIEC {

	DACStatement::DACStatement(Operand const& firstOp, Operand const& secondOp, TOpKind const op) :
		mFirstOperand(firstOp), mSecondOperand(secondOp), mOpperation(op)
	{
	}

	Operand DACStatement::GetFirst()
	{
		return mFirstOperand;
	}

	Operand DACStatement::GetSecond()
	{
		return mSecondOperand;
	}

	TOpKind DACStatement::GetOpType()
	{
		return mOpperation;
	}
}