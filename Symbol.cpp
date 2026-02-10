#include "Type.h"
#include "Symbol.h"

MIEC::Symbol::Symbol(const std::string& name, Type* type) : mName(name), mType(type)
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

int MIEC::ConstSymbol::GetValue() const
{
	return mValue;
}

MIEC::VarSymbol::VarSymbol(const std::string& name, Type* type, int offset)
	: Symbol(name, type), mOffset(offset)
{
}

int MIEC::VarSymbol::GetOffset() const
{
	return mOffset;
}


MIEC::TypeSymbol::TypeSymbol(const std::string& name, Type* type) : Symbol(name, type)
{
}

MIEC::Type* MIEC::TypeSymbol::GetType() const {
	return mType;
}

MIEC::ConstSymbol::ConstSymbol(const std::string& name, Type* type, int value)
	: Symbol(name, type), mValue(value)
{
}

int MIEC::ConstSymbol::GetValue() const
{
	return 0;
}
