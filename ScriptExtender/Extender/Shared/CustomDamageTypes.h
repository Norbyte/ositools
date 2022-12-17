#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/Stats.h>

BEGIN_SE()

struct CustomDamageTypeDescriptor
{
	uint32_t DamageTypeId;
	FixedString EnumLabel;
	STDString Name;
	FixedString NameHandle;
	STDString NameExtended;
	FixedString NameExtendedHandle;
	STDString Description;
	FixedString DescriptionHandle;
	STDString AmountAndTypeText;
	FixedString AmountAndTypeTextHandle;
	int32_t ColorCode{ -1 };
};

class CustomDamageTypeHelpers
{
public:
	CustomDamageTypeDescriptor* AssignType(FixedString const& name);
	CustomDamageTypeDescriptor* RegisterNewType(FixedString const& name);
	void Clear();

	DamageType GetDamageType(stats::GetDamageTypeProc* wrapped, char const* damageType);
	char const* GetDamageTypeString(stats::GetDamageTypeStringProc* wrapped, DamageType damageType);
	TranslatedString* DamageTypeToTranslateString(stats::DamageTypeToTranslateStringProc* wrapped, 
		TranslatedString* result, DamageType damageType);
	TranslatedString* DamageDescriptionToTranslateString(stats::DamageDescriptionToTranslateStringProc* wrapped, 
		TranslatedString* result, DamageType damageType);
	TranslatedString* DamageTypeToTranslateStringExtended(stats::DamageTypeToTranslateStringExtendedProc* wrapped, 
		TranslatedString* result, DamageType damageType);
	uint64_t GetColorCodeDmg(stats::GetColorCodeDmgProc* wrapped, DamageType damageType);
	void GetColorCodeAndTypeDmg(stats::ColorCodeAndTypeDmgProc* wrapped, eoc::Text* text, DamageType* pDamageType, 
		unsigned int amount, bool reflected);

private:
	std::unordered_map<uint32_t, CustomDamageTypeDescriptor> types_;
	std::unordered_map<FixedString, CustomDamageTypeDescriptor*> typeNames_;
	uint32_t nextDamageTypeId_{ (uint32_t)DamageType::Sentinel + 1 };
};

END_NS()
