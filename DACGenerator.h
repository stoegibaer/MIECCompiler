#ifndef DACGENERATOR_H
#define DACGENERATOR_H

#include <iostream>
#include <vector>
#include <stream>

#include "DACStatement.h"

namespace MIEC {

	class DACGenerator
	{
	public:
		DACGenerator();
		virtual ~DACGenerator() = default;
		void AddStatement(TOpKind const& operation, Operand const& firstOperand, Operand const& secondOperand);
		void AddStatement(TOpKind const& operation, Operand const& firstOperand);
		void Print(std::ostream & out);

	private:
		std::vector<DACStatement> mStatements;
	};


}



#endif DACGENERATOR_H