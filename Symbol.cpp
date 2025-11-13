#include "Symbol.h"

MIEC::Symbol::Symbol(const std::string& name) : mName(name)
{
}

std::string MIEC::Symbol::GetName() const
{
	return mName;
}

Type MIEC::Symbol::GetType() const
{
	return mType;
}

int MIEC::ConstSymbol::GetValue()
{
	return mValue;
}
