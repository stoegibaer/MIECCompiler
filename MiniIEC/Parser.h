

#if !defined(MIEC_COCO_PARSER_H__)
#define MIEC_COCO_PARSER_H__

#include "../Symbol.h"
#include "../Type.h"
#include "../SymbolTable.h"


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

SymbolTable& symTab = SymbolTable::GetInstance();
BaseType* floatType = new BaseType(TypeKind::eFloat);
BaseType* intType = new BaseType(TypeKind::eInt);
BaseType* charType = new BaseType(TypeKind::eChar);




	Parser(Scanner *scanner);
	~Parser();
	void SemErr(const wchar_t* msg);

	void MIEC();
	void VarDecl();
	void Statements();
	void Type(Type*& type);
	void Ident(std::string &name);
	void VarDeclList();
	void Stat();
	void Assignment();
	void PrintStatement();
	void WhileStatement();
	void IfStatement();
	void Expr();
	void Condition();
	void Term();
	void AddOp();
	void Factor();
	void MulOp();
	void Relop();

	void Parse();

}; // end Parser

} // namespace


#endif

