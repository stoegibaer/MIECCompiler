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
	class TypeSymbol;

	enum TypeKind {
		eInt,
		eFloat,
		eChar,
		eStruct,
		eArray
	};

	class Type
	{
		//virtual ~Type() = default;
		virtual int GetSize() = 0;
		TypeSymbol* GetTypeSymbol();

	protected:
		TypeSymbol* mTypeSymbol;
	};

	class BaseType : public Type
	{
	public:
		BaseType(TypeKind kind);
		int GetSize() override;
		TypeKind GetKind() const;

	private:
		TypeKind mType;
	};

	class StringType : public Type
	{
	public:
		StringType(int length);
		int GetSize() override;
		std::string GetStringVal();

	private:
		int mLength;
		std::string mValue;
	};

	class StructType : public Type {
	public:	
		StructType(int size);
		int GetSize();

	private:
		int mVal;
	};



} // namespace MIEC

#endif

