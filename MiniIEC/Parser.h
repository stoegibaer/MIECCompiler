

#if !defined(MIEC_COCO_PARSER_H__)
#define MIEC_COCO_PARSER_H__

#include <iostream>
#include <fstream>
#include <codecvt>
#include <locale>
#include "SymbolTable.h"
#include "DACGenerator.h"
#include "CodeGenerator.h"


#include "Scanner.h"

namespace MIEC {


class Errors {
public:
	int count;			// number of errors detected

	Errors();
	void SynErr(int line, int col, int n);
	void Error(int line, int col, const wchar_t *s);
	void Warning(int line, int col, const wchar_t *s);
	void Warning(const wchar_t *s);
	void Exception(const wchar_t *s);

}; // Errors

class Parser {
private:
	enum {
		_EOF=0,
		_ident=1,
		_number=2
	};
	int maxT;

	Token *dummyToken;
	int errDist;
	int minErrDist;

	void SynErr(int n);
	void Get();
	void Expect(int n);
	bool StartOf(int s);
	void ExpectWeak(int n, int follow);
	bool WeakSeparator(int n, int syFol, int repFol);

public:
	Scanner *scanner;
	Errors  *errors;

	Token *t;			// last recognized token
	Token *la;			// lookahead token

SymbolTable mSymTab;
    DACGenerator mDACGen;


void SemError(const wchar_t* msg) {
    errors->Error(la->line, la->col, msg);
}



	Parser(Scanner *scanner);
	~Parser();
	void SemErr(const wchar_t* msg);

	void MIEC();
	void VarDecl();
	void Statements();
	void VarDeclList();
	void Stat();
	void Assignment();
	void PrintStatement();
	void WhileStatement();
	void IfStatement();
	void Expr(Type * &type, Operand &result);
	void Condition(Type * &leftType, Type*& rightType, Operand &leftResult, Operand &rightResult, OpKind &compareOp);
	void Term(Type*& type, Operand &result);
	void AddOp(OpKind &op);
	void Factor(Type*& type, Operand &result);
	void MulOp(OpKind &op);
	void Relop(OpKind &op);

	void Parse();

}; // end Parser

} // namespace


#endif

