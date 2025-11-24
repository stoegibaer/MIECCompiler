

#include <wchar.h>
#include <iostream>
#include "Parser.h"
#include "Scanner.h"
#include "SymbolTable.h"


namespace MIEC {


void Parser::SynErr(int n) {
	if (errDist >= minErrDist) errors->SynErr(la->line, la->col, n);
	errDist = 0;
}

void Parser::SemErr(const wchar_t* msg) {
	if (errDist >= minErrDist) errors->Error(t->line, t->col, msg);
	errDist = 0;
}

void Parser::Get() {
	for (;;) {
		t = la;
		la = scanner->Scan();
		if (la->kind <= maxT) { ++errDist; break; }

		if (dummyToken != t) {
			dummyToken->kind = t->kind;
			dummyToken->pos = t->pos;
			dummyToken->col = t->col;
			dummyToken->line = t->line;
			dummyToken->next = NULL;
			coco_string_delete(dummyToken->val);
			dummyToken->val = coco_string_create(t->val);
			t = dummyToken;
		}
		la = t;
	}
}

void Parser::Expect(int n) {
	if (la->kind==n) Get(); else { SynErr(n); }
}

void Parser::ExpectWeak(int n, int follow) {
	if (la->kind == n) Get();
	else {
		SynErr(n);
		while (!StartOf(follow)) Get();
	}
}

bool Parser::WeakSeparator(int n, int syFol, int repFol) {
	if (la->kind == n) {Get(); return true;}
	else if (StartOf(repFol)) {return false;}
	else {
		SynErr(n);
		while (!(StartOf(syFol) || StartOf(repFol) || StartOf(0))) {
			Get();
		}
		return StartOf(syFol);
	}
}

void Parser::MIEC() {
		Expect(3 /* "PROGRAM" */);
		Expect(_ident);
		if (la->kind == 6 /* "BEGIN_VAR" */) {
			VarDecl();
		}
		Expect(4 /* "BEGIN" */);
		Statements();
		Expect(5 /* "END" */);
}

void Parser::VarDecl() {
		Expect(6 /* "BEGIN_VAR" */);
		VarDeclList();
		Expect(7 /* "END_VAR" */);
}

void Parser::Statements() {
		while (StartOf(1)) {
			Stat();
		}
}

void Parser::VarDeclList() {
		Expect(_ident);
		std::string varName = coco_string_create_char(t->val);
		Expect(8 /* ":" */);
		Expect(9 /* "Integer" */);
		Expect(10 /* ";" */);
		if (!mSymTab.AddVar(varName, mCurrOffset))
		{
			SemErr(L"Doppeldeklaration von Variable");
		}
		else
		{
			mCurrOffset += 4; // Integer: 4 Bytes
		}
		while (la->kind == _ident) {
			Get();
			std::string varName = coco_string_create_char(t->val);
			Expect(8 /* ":" */);
			Expect(9 /* "Integer" */);
			Expect(10 /* ";" */);
			if (!mSymTab.AddVar(varName, mCurrOffset))
			{
				SemErr(L"Doppeldeklaration von Variable");
			}
			else
			{
				mCurrOffset += 4; // Integer: 4 Bytes
			}
		}
}

void Parser::Stat() {
		if (la->kind == _ident) {
			Assignment();
		} else if (la->kind == 12 /* "print" */) {
			PrintStatement();
		} else if (la->kind == 15 /* "WHILE" */) {
			WhileStatement();
		} else if (la->kind == 17 /* "IF" */) {
			IfStatement();
		} else SynErr(31);
}

void Parser::Assignment() {
		Expect(_ident);
		std::string varName = coco_string_create_char(t->val);
		Expect(11 /* ":=" */);
		Expr();
		if (!mSymTab.Find(varName))
		{
			SemErr(L"Variable nicht deklariert!");
		}
		Expect(10 /* ";" */);
}

void Parser::PrintStatement() {
		Expect(12 /* "print" */);
		Expect(13 /* "(" */);
		Expr();
		Expect(14 /* ")" */);
		Expect(10 /* ";" */);
}

void Parser::WhileStatement() {
		Expect(15 /* "WHILE" */);
		Condition();
		Expect(16 /* "DO" */);
		Statements();
		Expect(5 /* "END" */);
}

void Parser::IfStatement() {
		Expect(17 /* "IF" */);
		Condition();
		Expect(18 /* "THEN" */);
		Statements();
		if (la->kind == 19 /* "ELSE" */) {
			Get();
			Statements();
		}
		Expect(5 /* "END" */);
}

void Parser::Expr() {
		Term();
		while (la->kind == 26 /* "+" */ || la->kind == 27 /* "-" */) {
			AddOp();
			Term();
		}
}

void Parser::Condition() {
		Expr();
		Relop();
		Expr();
}

void Parser::Term() {
		Factor();
		while (la->kind == 28 /* "*" */ || la->kind == 29 /* "/" */) {
			MulOp();
			Factor();
		}
}

void Parser::AddOp() {
		if (la->kind == 26 /* "+" */) {
			Get();
		} else if (la->kind == 27 /* "-" */) {
			Get();
		} else SynErr(32);
}

void Parser::Factor() {
		if (la->kind == _ident) {
			Get();
			std::string varName = coco_string_create_char(t->val);
			if (!mSymTab.Find(varName))
			{
				SemErr(L"Variable nicht deklariert!");
			}
		} else if (la->kind == _number) {
			Get();
		} else if (la->kind == 13 /* "(" */) {
			Get();
			Expr();
			Expect(14 /* ")" */);
		} else SynErr(33);
}

void Parser::MulOp() {
		if (la->kind == 28 /* "*" */) {
			Get();
		} else if (la->kind == 29 /* "/" */) {
			Get();
		} else SynErr(34);
}

void Parser::Relop() {
		switch (la->kind) {
		case 20 /* "=" */: {
			Get();
			break;
		}
		case 21 /* "<=" */: {
			Get();
			break;
		}
		case 22 /* ">=" */: {
			Get();
			break;
		}
		case 23 /* "!=" */: {
			Get();
			break;
		}
		case 24 /* "<" */: {
			Get();
			break;
		}
		case 25 /* ">" */: {
			Get();
			break;
		}
		default: SynErr(35); break;
		}
}




// If the user declared a method Init and a mehtod Destroy they should
// be called in the contructur and the destructor respctively.
//
// The following templates are used to recognize if the user declared
// the methods Init and Destroy.

template<typename T>
struct ParserInitExistsRecognizer {
	template<typename U, void (U::*)() = &U::Init>
	struct ExistsIfInitIsDefinedMarker{};

	struct InitIsMissingType {
		char dummy1;
	};
	
	struct InitExistsType {
		char dummy1; char dummy2;
	};

	// exists always
	template<typename U>
	static InitIsMissingType is_here(...);

	// exist only if ExistsIfInitIsDefinedMarker is defined
	template<typename U>
	static InitExistsType is_here(ExistsIfInitIsDefinedMarker<U>*);

	enum { InitExists = (sizeof(is_here<T>(NULL)) == sizeof(InitExistsType)) };
};

template<typename T>
struct ParserDestroyExistsRecognizer {
	template<typename U, void (U::*)() = &U::Destroy>
	struct ExistsIfDestroyIsDefinedMarker{};

	struct DestroyIsMissingType {
		char dummy1;
	};
	
	struct DestroyExistsType {
		char dummy1; char dummy2;
	};

	// exists always
	template<typename U>
	static DestroyIsMissingType is_here(...);

	// exist only if ExistsIfDestroyIsDefinedMarker is defined
	template<typename U>
	static DestroyExistsType is_here(ExistsIfDestroyIsDefinedMarker<U>*);

	enum { DestroyExists = (sizeof(is_here<T>(NULL)) == sizeof(DestroyExistsType)) };
};

// The folloing templates are used to call the Init and Destroy methods if they exist.

// Generic case of the ParserInitCaller, gets used if the Init method is missing
template<typename T, bool = ParserInitExistsRecognizer<T>::InitExists>
struct ParserInitCaller {
	static void CallInit(T *t) {
		// nothing to do
	}
};

// True case of the ParserInitCaller, gets used if the Init method exists
template<typename T>
struct ParserInitCaller<T, true> {
	static void CallInit(T *t) {
		t->Init();
	}
};

// Generic case of the ParserDestroyCaller, gets used if the Destroy method is missing
template<typename T, bool = ParserDestroyExistsRecognizer<T>::DestroyExists>
struct ParserDestroyCaller {
	static void CallDestroy(T *t) {
		// nothing to do
	}
};

// True case of the ParserDestroyCaller, gets used if the Destroy method exists
template<typename T>
struct ParserDestroyCaller<T, true> {
	static void CallDestroy(T *t) {
		t->Destroy();
	}
};

void Parser::Parse() {
	t = NULL;
	la = dummyToken = new Token();
	la->val = coco_string_create(L"Dummy Token");
	Get();
	MIEC();
	Expect(0);
}

Parser::Parser(Scanner *scanner) : mSymTab(SymbolTable::GetInstance()) {
	maxT = 30;

	ParserInitCaller<Parser>::CallInit(this);
	dummyToken = NULL;
	t = la = NULL;
	minErrDist = 2;
	errDist = minErrDist;
	this->scanner = scanner;
	errors = new Errors();
}

bool Parser::StartOf(int s) {
	const bool T = true;
	const bool x = false;

	static bool set[2][32] = {
		{T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x},
		{x,T,x,x, x,x,x,x, x,x,x,x, T,x,x,T, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x}
	};



	return set[s][la->kind];
}

Parser::~Parser() {
	ParserDestroyCaller<Parser>::CallDestroy(this);
	delete errors;
	delete dummyToken;
}

Errors::Errors() {
	count = 0;
}

void Errors::SynErr(int line, int col, int n) {
	wchar_t* s;
	switch (n) {
			case 0: s = coco_string_create(L"EOF expected"); break;
			case 1: s = coco_string_create(L"ident expected"); break;
			case 2: s = coco_string_create(L"number expected"); break;
			case 3: s = coco_string_create(L"\"PROGRAM\" expected"); break;
			case 4: s = coco_string_create(L"\"BEGIN\" expected"); break;
			case 5: s = coco_string_create(L"\"END\" expected"); break;
			case 6: s = coco_string_create(L"\"BEGIN_VAR\" expected"); break;
			case 7: s = coco_string_create(L"\"END_VAR\" expected"); break;
			case 8: s = coco_string_create(L"\":\" expected"); break;
			case 9: s = coco_string_create(L"\"Integer\" expected"); break;
			case 10: s = coco_string_create(L"\";\" expected"); break;
			case 11: s = coco_string_create(L"\":=\" expected"); break;
			case 12: s = coco_string_create(L"\"print\" expected"); break;
			case 13: s = coco_string_create(L"\"(\" expected"); break;
			case 14: s = coco_string_create(L"\")\" expected"); break;
			case 15: s = coco_string_create(L"\"WHILE\" expected"); break;
			case 16: s = coco_string_create(L"\"DO\" expected"); break;
			case 17: s = coco_string_create(L"\"IF\" expected"); break;
			case 18: s = coco_string_create(L"\"THEN\" expected"); break;
			case 19: s = coco_string_create(L"\"ELSE\" expected"); break;
			case 20: s = coco_string_create(L"\"=\" expected"); break;
			case 21: s = coco_string_create(L"\"<=\" expected"); break;
			case 22: s = coco_string_create(L"\">=\" expected"); break;
			case 23: s = coco_string_create(L"\"!=\" expected"); break;
			case 24: s = coco_string_create(L"\"<\" expected"); break;
			case 25: s = coco_string_create(L"\">\" expected"); break;
			case 26: s = coco_string_create(L"\"+\" expected"); break;
			case 27: s = coco_string_create(L"\"-\" expected"); break;
			case 28: s = coco_string_create(L"\"*\" expected"); break;
			case 29: s = coco_string_create(L"\"/\" expected"); break;
			case 30: s = coco_string_create(L"??? expected"); break;
			case 31: s = coco_string_create(L"invalid Stat"); break;
			case 32: s = coco_string_create(L"invalid AddOp"); break;
			case 33: s = coco_string_create(L"invalid Factor"); break;
			case 34: s = coco_string_create(L"invalid MulOp"); break;
			case 35: s = coco_string_create(L"invalid Relop"); break;

		default:
		{
			wchar_t format[20];
			coco_swprintf(format, 20, L"error %d", n);
			s = coco_string_create(format);
		}
		break;
	}
	wprintf(L"-- line %d col %d: %ls\n", line, col, s);
	coco_string_delete(s);
	count++;
}

void Errors::Error(int line, int col, const wchar_t *s) {
	wprintf(L"-- line %d col %d: %ls\n", line, col, s);
	count++;
}

void Errors::Warning(int line, int col, const wchar_t *s) {
	wprintf(L"-- line %d col %d: %ls\n", line, col, s);
}

void Errors::Warning(const wchar_t *s) {
	wprintf(L"%ls\n", s);
}

void Errors::Exception(const wchar_t* s) {
	wprintf(L"%ls", s); 
	exit(1);
}

} // namespace

