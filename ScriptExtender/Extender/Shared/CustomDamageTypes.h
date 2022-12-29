#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/Stats.h>
#include <Lua/Shared/LuaHelpers.h>

BEGIN_SE()

struct CustomDamageTypeCallbacks;

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

	CustomDamageTypeCallbacks* GetCallbacks();
};

class CustomDamageTypeHelpers
{
public:
	CustomDamageTypeDescriptor* AssignType(FixedString const& name);
	CustomDamageTypeDescriptor* RegisterNewType(FixedString const& name);
	void Clear();

	uint32_t MaxDamageTypeId() const
	{
		return nextDamageTypeId_;
	}

	DamageType GetDamageType(stats::GetDamageTypeProc* wrapped, char const* damageType);
	char const* GetDamageTypeString(stats::GetDamageTypeStringProc* wrapped, DamageType damageType);
	TranslatedString* DamageTypeToTranslateString(stats::DamageTypeToTranslateStringProc* wrapped, 
		TranslatedString* result, DamageType damageType);
	TranslatedString* DamageDescriptionToTranslateString(stats::DamageDescriptionToTranslateStringProc* wrapped, 
		TranslatedString* result, DamageType damageType);
	TranslatedString* DamageTypeToTranslateStringExtended(stats::DamageTypeToTranslateStringExtendedProc* wrapped, 
		TranslatedString* result, DamageType damageType);
	uint64_t GetColorCodeDmg(stats::GetColorCodeDmgProc* wrapped, DamageType damageType);
#if defined(OSI_EOCAPP)
	void GetColorCodeAndTypeDmg(stats::ColorCodeAndTypeDmgProc* wrapped, eoc::Text* text, unsigned int amount,
		bool reflected, DamageType damageType);
	bool ComputeScaledDamage(stats::Character::ComputeScaledDamageProc* wrapped, Character* self, Item* weapon, 
		DamageDescList* damages, bool keepCurrentDamages);
	int32_t GetResistance(stats::Character::GetResistanceProc* wrapped, Character* self,
		DamageType damageType, bool baseValues);
	float GetDamageBoostByType(stats::Character::GetDamageBoostByTypeProc* wrapped, Character* self,
		DamageType damageType);
#else
	void GetColorCodeAndTypeDmg(stats::ColorCodeAndTypeDmgProc* wrapped, eoc::Text* text, DamageType& damageType,
		unsigned int amount, bool reflected);
	bool ComputeScaledDamage(stats::Character::ComputeScaledDamageProc* wrapped, Character* self, Item* weapon, 
		DamageDescList* damages, bool includeBoosts, bool keepCurrentDamages);
	int32_t GetResistance(stats::Character::GetResistanceProc* wrapped, Character* self,
		DamageType damageType, bool baseValues, bool excludeBoosts);
	float GetDamageBoostByType(stats::Character::GetDamageBoostByTypeProc* wrapped, Character* self,
		DamageType damageType, bool excludeBoosts);
#endif
	bool ComputeItemDamage(stats::Item::ComputeDamageProc* wrapped, Item* self,
		DamageDescList* damages, bool keepCurrentDamages);

private:
	std::unordered_map<uint32_t, CustomDamageTypeDescriptor> types_;
	std::unordered_map<FixedString, CustomDamageTypeDescriptor*> typeNames_;
	uint32_t nextDamageTypeId_{ (uint32_t)DamageType::Sentinel + 1 };
};

struct CustomDamageTypeCallbacks
{
	uint32_t DamageTypeId;
	lua::Traits<lua::RegistryEntry, lua::CallableTrait> GetResistanceCallback;
	lua::Traits<lua::RegistryEntry, lua::CallableTrait> GetDamageBoostCallback;
	lua::Traits<lua::RegistryEntry, lua::CallableTrait> GetArmorDamageCallback;
	lua::Traits<lua::RegistryEntry, lua::CallableTrait> GetMagicArmorDamageCallback;
};

class CustomDamageTypeCallbackManager
{
public:
	CustomDamageTypeCallbacks* GetOrRegister(uint32_t damageTypeId);
	void Clear();

	std::optional<int32_t> GetResistance(Character* self, DamageType damageType, bool baseValues);
	std::optional<float> GetDamageBoostByType(Character* self, DamageType damageType);

private:
	std::unordered_map<uint32_t, CustomDamageTypeCallbacks> types_;
};

END_NS()
