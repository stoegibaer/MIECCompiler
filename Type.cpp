#include "Type.h"
#include <sstream>
#include <algorithm>

namespace MIEC {

	TypeSymbol* Type::GetTypeSymbol()
	{
		return mTypeSymbol;
	}

	BaseType::BaseType(TypeKind kind) : mType(kind)
	{
	}

	int BaseType::GetSize()
	{
		switch (mType) {
			case eInt:
				return sizeof(int);      // typischerweise 4 Bytes
			case eFloat:
				return sizeof(float);    // typischerweise 4 Bytes
			case eChar:
				return sizeof(char);     // 1 Byte
			default:
				return 0;
		}
	}

	TypeKind BaseType::GetKind() const
	{
		return mType;
	}

	int StringType::GetSize()
	{
		return mLength * sizeof(char);
	}

	std::string StringType::GetStringVal()
	{
		return mValue;
	}

	StructType::StructType(int size) : mVal(size)
	{
	}

	int StructType::GetSize()
	{
		return mVal;
	}

} // namespace MIEC
