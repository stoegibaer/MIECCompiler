#include "Type.h"
#include "Symbol.h" // -> avoid circular dependency
#include <sstream>
#include <algorithm>

namespace MIEC {
	Type::Type(TypeSymbol* sym) : mTypeSymbol(sym)
	{
	}
	TypeSymbol* Type::GetTypeSymbol() const
	{
		return mTypeSymbol;
	}

	//=========================================================

	BaseType::BaseType(TypeKind kind) : Type(/*what the heck*/), mType(kind)
	{
	}

	int BaseType::GetSize()
	{
		int size = 0;

		switch (mType)
		{
		case MIEC::eInt:
			size = sizeof(int);
			break;
		case MIEC::eFloat:
			size = sizeof(float);
			break;
		case MIEC::eChar:
			size = sizeof(char);
			break;
		}

		return size;
	}

	TypeKind BaseType::GetKind()
	{
		return mType;
	}

	////=========================================================

	StringType::StringType(std::string const& value)
	{
	}

	int StringType::GetSize()
	{
		mLength = mValue.size();
		return mLength;
	}

	std::string StringType::GetStringVal()
	{
		return mValue;
	}

	//=========================================================



	StructType::StructType(const std::string& name) : mName(name), mSize(0)
	{
	}

	int StructType::GetSize()
	{
		int totalSize = 0;
		for (auto& elem : mElems) {
			totalSize += elem.offset;
		}
		mSize = totalSize;
		return totalSize;
	}

	std::string StructType::GetName()
	{
		return mName;
	}

	void StructType::AddField(const std::string& name, VarSymbol* var)
	{
		mElems.push_back({ name, var, var->GetOffset() });
	}

	std::vector<StructType::Elem> StructType::GetField()
	{
		return mElems;
	}

	//=========================================================

	ArrayType::ArrayType(Type* elemType, int count) : mElemType(elemType), mCount(count)
	{
	}

	int ArrayType::GetCount() const
	{
		return mCount;
	}

	Type* ArrayType::GetElemType() const
	{
		return mElemType;
	}

	int ArrayType::GetSize()
	{
		return mElemType->GetSize() * mCount;
	}

} // namespace MIEC
