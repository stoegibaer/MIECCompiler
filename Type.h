#ifndef SYMBOL_H
#define SYMBOL_H


#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <stdexcept>

#include "Symbol.h"

namespace MIEC {

	enum TypeKind {
		eInt,
		eFloat,
		eChar,
		eStruct,
		eArray
	};

	class Type
	{
		virtual int GetSize() = 0;
		Symbol GetTypeSymbol();

	private:
		TypeSymbol mTypeSymbol;
	};

	class BaseType : public Type
	{
	public:
		int GetSize();
		TypeKind GetKind();

	private:
		TypeKind mType;
	};


} // namespace MIEC

#endif

