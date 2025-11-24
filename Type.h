#ifndef TYPE_H
#define TYPE_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <stdexcept>

#include "Symbol.h"

namespace MIEC {

	//=========================================================

	enum TypeKind {
		eInt,
		eFloat,
		eChar,
	};

	//=========================================================

	class Type
	{
		virtual int GetSize() = 0;
		Symbol GetTypeSymbol();

	private:
		TypeSymbol* mTypeSymbol;
	};

	////=========================================================

	//class BaseType : public Type
	//{
	//public:
	//	int GetSize() override;
	//	TypeKind GetKind();

	//private:
	//	TypeKind mType;
	//};

	////=========================================================

	//class StringType : public Type
	//{
	//public: 
	//	StringType(std::string const& value);

	//	int GetSize() override;

	//	std::string GetStringVal();

	//private:
	//	int mLength;
	//	std::string mValue;
	//};

	////=========================================================

	//class StructType : public Type {
	//public:	
	//	int GetSize() override;

	//private:
	//	int mVal;
	//};

	//=========================================================

} // namespace MIEC

#endif

