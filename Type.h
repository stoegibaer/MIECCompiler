#ifndef TYPE_H
#define TYPE_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <stdexcept>

namespace MIEC {

	//=========================================================

	enum TypeKind {
		eInt,
		eFloat,
		eChar,
	};

	//=========================================================

	#pragma once
	class TypeSymbol;

	class Type
	{
		TypeSymbol* GetTypeSymbol() const;
		virtual int GetSize() = 0;

	private:
		TypeSymbol* mTypeSymbol;
	};

	////=========================================================

	class BaseType : public Type
	{
	public:
		int GetSize() override;
		TypeKind GetKind();

	private:
		TypeKind mType;
	};

	////=========================================================

	class StringType : public Type
	{
	public: 
		StringType(std::string const& value);

		int GetSize() override;

		std::string GetStringVal();

	private:
		int mLength;
		std::string mValue;
	};

	////=========================================================
	#pragma once
	class VarSymbol;

	class StructType : public Type {
	public:	
		struct Elem {
			std::string name;
			VarSymbol* VarSymbol;
			int offset;
		};

		int GetSize() override;
		std::string GetName();
		void AddField(const std::string& name, VarSymbol* var);
		std::vector<StructType::Elem> GetField();

	private:
		int mSize;
		std::string mName;
		std::vector<Elem> mElems;
	};

	//=========================================================

} // namespace MIEC

#endif

