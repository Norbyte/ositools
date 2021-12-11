#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>

BEGIN_SE()

template <class T>
struct VariablesList
{
	Vector<T> Elements;
	Vector<uint16_t> Remaps;
};

struct VariableHandle
{
	ComponentHandle ObjectHandle;
	int Remap;
};

struct VariableManagerLocalData
{
	VariablesList<int32_t> IntVariables;
	VariablesList<int64_t> Int64Variables;
	VariablesList<float> FloatVariables;
	VariablesList<STDString> StringVariables;
	VariablesList<FixedString> FixedStringVariables;
	VariablesList<glm::vec3> Vector3Variables;
	RefMap<int, VariableHandle>* Key2Table;
};

struct VariableManager
{
	void* VMT;
	VariableManagerLocalData* LocalData;
	int field_10;
};

END_SE()
