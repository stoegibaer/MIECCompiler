#include "Type.h"
#include <sstream>
#include <algorithm>

namespace MIEC {

	Symbol Type::GetTypeSymbol()
	{
		return mTypeSymbol;
	}

	//=========================================================

	//int BaseType::GetSize()
	//{
	//	int size = 0;

	//	switch (mType)
	//	{
	//	case MIEC::eInt:
	//		size = sizeof(int);
	//		break;

	//	case MIEC::eFloat:
	//		size = sizeof(float);
	//		break;
	//	case MIEC::eChar:
	//		size = sizeof(char);
	//		break;
	//	}

	//	return size;
	//}

	//TypeKind BaseType::GetKind()
	//{
	//	return mType;
	//}

	////=========================================================

	//int StringType::GetSize()
	//{
	//	mLength = mValue.size();
	//	return mLength;
	//}

	//std::string StringType::GetStringVal()
	//{
	//	return mValue;
	//}

	//=========================================================


} // namespace MIEC
