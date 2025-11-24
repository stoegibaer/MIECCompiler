#include "Type.h"
#include <sstream>
#include <algorithm>

namespace MIEC {

	Symbol Type::GetTypeSymbol()
	{
		return Symbol();
	}

	int BaseType::GetSize()
	{
		return 0;
	}

	TypeKind BaseType::GetKind()
	{
		return TypeKind();
	}

} // namespace MIEC
