#include "Symbol.h"

MIEC::Symbol::Symbol(const std::string& name) : mName(name)
{
}

std::string MIEC::Symbol::GetName() const
{
	return mName;
}

MIEC::Type* MIEC::Symbol::GetType() const
{
	return mType;
}


MIEC::VarSymbol::VarSymbol(const std::string& n, int off) : Symbol(n), mOffset(off)
{}

int MIEC::VarSymbol::GetOffset() const
{
	return mOffset;
}

MIEC::TypeSymbol::TypeSymbol(const std::string& name) : Symbol(name)
{
}

MIEC::ConstSymbol::ConstSymbol(const std::string& name, int value)
	: Symbol(name), mValue(value)
{
}

int MIEC::ConstSymbol::GetValue() const
{
	return mValue;
}
