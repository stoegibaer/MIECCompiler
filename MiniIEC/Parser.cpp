

#include <wchar.h>
#include "Parser.h"
#include "Scanner.h"


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
		if (la->kind == 9 /* "BEGIN_VAR" */) {
			VarDecl();
		}
		Expect(4 /* "BEGIN" */);
		Statements();
		Expect(5 /* "END" */);
		delete floatType; delete charType; delete intType; 
}

void Parser::VarDecl() {
		std::string name; Symbol * type; 
		Expect(9 /* "BEGIN_VAR" */);
		VarDeclList();
		Expect(10 /* "END_VAR" */);
}

void Parser::Statements() {
		while (StartOf(1)) {
			Stat();
		}
}

void Parser::Type(Type*& type) {
		if (la->kind == 6 /* "Integer" */) {
			Get();
			type = intType; 
		} else if (la->kind == 7 /* "Float" */) {
			Get();
			type = floatType; 
		} else if (la->kind == 8 /* "Char" */) {
			Get();
			type = charType; 
		} else SynErr(33);
}

void Parser::Ident(std::string &name) {
		Expect(_ident);
		char * nameChars = coco_string_create_char(t->val); 
		name = nameChars;
		coco_string_delete(nameChars); 
}

void Parser::VarDeclList() {
		Ident(name);
		Expect(11 /* ":" */);
		Type(type);
		Expect(12 /* ";" */);
		if (!symTab.AddVar(name, type, 0)) errors->Exception(L"Variable mit Name schon deklariert"); 
		while (la->kind == _ident) {
			Ident(name);
			Expect(11 /* ":" */);
			Type(type);
			Expect(12 /* ";" */);
			if (!symTab.AddVar(name, type, 0)) errors->Exception(L"Variable mit Name schon deklariert"); 
		}
}

void Parser::Stat() {
		if (la->kind == _ident) {
			Assignment();
		} else if (la->kind == 14 /* "print" */) {
			PrintStatement();
		} else if (la->kind == 17 /* "WHILE" */) {
			WhileStatement();
		} else if (la->kind == 19 /* "IF" */) {
			IfStatement();
		} else SynErr(34);
}

void Parser::Assignment() {
		std::string name; 
		Ident(name);
		Symbol* sym = symTab.Find(name);
		if(!sym) errors->Exception(L"Variable nicht deklariert");
		// optional: Typprüfung! if (sym->GetType() != ExprType) ...
		
		Expect(13 /* ":=" */);
		Expr();
		Expect(12 /* ";" */);
}

void Parser::PrintStatement() {
		Expect(14 /* "print" */);
		Expect(15 /* "(" */);
		Expr();
		Expect(16 /* ")" */);
		Expect(12 /* ";" */);
}

void Parser::WhileStatement() {
		Expect(17 /* "WHILE" */);
		Condition();
		Expect(18 /* "DO" */);
		Statements();
		Expect(5 /* "END" */);
}

void Parser::IfStatement() {
		Expect(19 /* "IF" */);
		Condition();
		Expect(20 /* "THEN" */);
		Statements();
		if (la->kind == 21 /* "ELSE" */) {
			Get();
			Statements();
		}
		Expect(5 /* "END" */);
}

void Parser::Expr() {
		Term();
		while (la->kind == 28 /* "+" */ || la->kind == 29 /* "-" */) {
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
		while (la->kind == 30 /* "*" */ || la->kind == 31 /* "/" */) {
			MulOp();
			Factor();
		}
}

void Parser::AddOp() {
		if (la->kind == 28 /* "+" */) {
			Get();
		} else if (la->kind == 29 /* "-" */) {
			Get();
		} else SynErr(35);
}

void Parser::Factor() {
		if (la->kind == _ident) {
			Get();
		} else if (la->kind == _number) {
			Get();
		} else if (la->kind == 15 /* "(" */) {
			Get();
			Expr();
			Expect(16 /* ")" */);
		} else SynErr(36);
}

void Parser::MulOp() {
		if (la->kind == 30 /* "*" */) {
			Get();
		} else if (la->kind == 31 /* "/" */) {
			Get();
		} else SynErr(37);
}

void Parser::Relop() {
		switch (la->kind) {
		case 22 /* "=" */: {
			Get();
			break;
		}
		case 23 /* "<=" */: {
			Get();
			break;
		}
		case 24 /* ">=" */: {
			Get();
			break;
		}
		case 25 /* "!=" */: {
			Get();
			break;
		}
		case 26 /* "<" */: {
			Get();
			break;
		}
		case 27 /* ">" */: {
			Get();
			break;
		}
		default: SynErr(38); break;
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

Parser::Parser(Scanner *scanner) {
	maxT = 32;

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

	static bool set[2][34] = {
		{T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x},
		{x,T,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,T,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x}
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
			case 6: s = coco_string_create(L"\"Integer\" expected"); break;
			case 7: s = coco_string_create(L"\"Float\" expected"); break;
			case 8: s = coco_string_create(L"\"Char\" expected"); break;
			case 9: s = coco_string_create(L"\"BEGIN_VAR\" expected"); break;
			case 10: s = coco_string_create(L"\"END_VAR\" expected"); break;
			case 11: s = coco_string_create(L"\":\" expected"); break;
			case 12: s = coco_string_create(L"\";\" expected"); break;
			case 13: s = coco_string_create(L"\":=\" expected"); break;
			case 14: s = coco_string_create(L"\"print\" expected"); break;
			case 15: s = coco_string_create(L"\"(\" expected"); break;
			case 16: s = coco_string_create(L"\")\" expected"); break;
			case 17: s = coco_string_create(L"\"WHILE\" expected"); break;
			case 18: s = coco_string_create(L"\"DO\" expected"); break;
			case 19: s = coco_string_create(L"\"IF\" expected"); break;
			case 20: s = coco_string_create(L"\"THEN\" expected"); break;
			case 21: s = coco_string_create(L"\"ELSE\" expected"); break;
			case 22: s = coco_string_create(L"\"=\" expected"); break;
			case 23: s = coco_string_create(L"\"<=\" expected"); break;
			case 24: s = coco_string_create(L"\">=\" expected"); break;
			case 25: s = coco_string_create(L"\"!=\" expected"); break;
			case 26: s = coco_string_create(L"\"<\" expected"); break;
			case 27: s = coco_string_create(L"\">\" expected"); break;
			case 28: s = coco_string_create(L"\"+\" expected"); break;
			case 29: s = coco_string_create(L"\"-\" expected"); break;
			case 30: s = coco_string_create(L"\"*\" expected"); break;
			case 31: s = coco_string_create(L"\"/\" expected"); break;
			case 32: s = coco_string_create(L"??? expected"); break;
			case 33: s = coco_string_create(L"invalid Type"); break;
			case 34: s = coco_string_create(L"invalid Stat"); break;
			case 35: s = coco_string_create(L"invalid AddOp"); break;
			case 36: s = coco_string_create(L"invalid Factor"); break;
			case 37: s = coco_string_create(L"invalid MulOp"); break;
			case 38: s = coco_string_create(L"invalid Relop"); break;

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

