#include "stdafx.h"

#include <GameDefinitions/BaseTypes.h>
#include <GameDefinitions/Symbols.h>
#include <GameDefinitions/Enumerations.h>
#include "OsirisProxy.h"
#include <PropertyMaps.h>

namespace dse
{
	CRPGStats_Modifier * ModifierList::GetAttributeInfo(FixedString const& name, int * attributeIndex) const
	{
		auto index = Attributes.FindIndex(name);
		if (index == -1) {
			return nullptr;
		}
		else {
			*attributeIndex = index;
			return Attributes.Find(index);
		}
	}

	bool CRPGStatsManager::ObjectExists(FixedString const& statsId, FixedString const& type)
	{
		auto object = objects.Find(statsId);
		if (object == nullptr) {
			return false;
		}

		auto typeInfo = GetTypeInfo(object);
		if (typeInfo == nullptr) {
			return false;
		}

		return typeInfo->Name == type;
	}

	std::optional<int> CRPGStatsManager::EnumLabelToIndex(FixedString const& enumName, char const* enumLabel)
	{
		auto rpgEnum = modifierValueList.Find(enumName);
		if (rpgEnum == nullptr) {
			OsiError("No enum named '" << enumName << "' exists");
			return {};
		}

		auto index = rpgEnum->Values.Find(ToFixedString(enumLabel));
		if (index == nullptr) {
			return {};
		} else {
			return *index;
		}
	}

	FixedString CRPGStatsManager::EnumIndexToLabel(FixedString const& enumName, int index)
	{
		auto rpgEnum = modifierValueList.Find(enumName);
		if (rpgEnum == nullptr) {
			OsiError("No enum named '" << enumName << "' exists");
			return FixedString{};
		}

		auto value = rpgEnum->Values.FindByValue(index);
		if (value) {
			return *value;
		}
		else {
			return FixedString{};
		}
	}

	CRPGStats_Modifier * CRPGStatsManager::GetModifierInfo(FixedString const& modifierListName, FixedString const& modifierName)
	{
		auto modifiers = modifierList.Find(modifierListName);
		if (modifiers != nullptr) {
			return modifiers->Attributes.Find(modifierName);
		} else {
			return nullptr;
		}
	}

	ModifierList * CRPGStatsManager::GetTypeInfo(CRPGStats_Object * object)
	{
		return modifierList.Find(object->ModifierListIndex);
	}

	RPGEnumeration * CRPGStatsManager::GetAttributeInfo(CRPGStats_Object * object, FixedString const& attributeName, int & attributeIndex)
	{
		auto objModifiers = modifierList.Find(object->ModifierListIndex);
		if (objModifiers == nullptr) {
			return nullptr;
		}

		auto modifierInfo = objModifiers->GetAttributeInfo(attributeName, &attributeIndex);
		if (modifierInfo == nullptr) {
			return nullptr;
		}

		auto typeInfo = modifierValueList.Find(modifierInfo->RPGEnumerationIndex);
		return typeInfo;
	}

	std::optional<char const *> CRPGStatsManager::GetAttributeString(CRPGStats_Object * object, FixedString const& attributeName)
	{
		int attributeIndex;
		auto typeInfo = GetAttributeInfo(object, attributeName, attributeIndex);
		if (typeInfo == nullptr) {
			return {};
		}

		auto index = object->IndexedProperties[attributeIndex];
		if (typeInfo->Name == GFS.strFixedString) {
			return ModifierFSSet[index].Str;
		} else if (typeInfo->Name == GFS.strAttributeFlags) {
			if (index != -1) {
				auto attrFlags = (uint64_t)AttributeFlags[index];
				STDString flagsStr;
				for (auto i = 0; i < 64; i++) {
					if (attrFlags & (1ull << i)) {
						auto label = EnumInfo<StatAttributeFlags>::Find((StatAttributeFlags)(1ull << i));
						if (label) {
							if (!flagsStr.empty()) {
								flagsStr += ';';
							}

							flagsStr += label.Str;
						}
					}
				}

				return gTempStrings.Make(flagsStr);
			} else {
				return "";
			}
		} else if (typeInfo->Values.ItemCount > 0) {
			auto enumLabel = typeInfo->Values.FindByValue(index);
			if (enumLabel) {
				return enumLabel->Str;
			}
			else {
				return {};
			}
		}
		else {
			return {};
		}
	}

	std::optional<int> CRPGStatsManager::GetAttributeInt(CRPGStats_Object * object, FixedString const& attributeName)
	{
		int attributeIndex;
		auto typeInfo = GetAttributeInfo(object, attributeName, attributeIndex);
		if (typeInfo == nullptr) {
			return {};
		}

		auto index = object->IndexedProperties[attributeIndex];
		if (typeInfo->Name == GFS.strConstantInt
			|| typeInfo->Values.ItemCount > 0) {
			return index;
		}
		else {
			return {};
		}
	}

	std::optional<int> CRPGStatsManager::GetAttributeIntScaled(CRPGStats_Object * object, FixedString const& attributeName, int level)
	{
		auto objModifiers = modifierList.Find(object->ModifierListIndex);
		if (objModifiers == nullptr) {
			return {};
		}

		int attributeIndex;
		auto modifierInfo = objModifiers->GetAttributeInfo(attributeName, &attributeIndex);
		if (modifierInfo == nullptr) {
			return {};
		}

		auto levelMap = LevelMaps.Find(modifierInfo->LevelMapIndex);
		auto value = object->IndexedProperties[attributeIndex];
		if (levelMap) {
			return (int32_t)levelMap->GetScaledValue(value, level);
		} else {
			return value;
		}
	}

	int CRPGStatsManager::GetOrCreateFixedString(const char * value)
	{
		auto fs = MakeFixedString(value);
		if (!fs) return -1;

		auto & strings = ModifierFSSet.Set;
		for (uint32_t i = 0; i < strings.Size; i++) {
			if (strings[i] == fs) {
				return i;
			}
		}

		strings.Add(fs);
		return strings.Size - 1;
	}

	std::optional<StatAttributeFlags> CRPGStatsManager::StringToAttributeFlags(const char * value)
	{
		StatAttributeFlags flags{ 0 };
		STDString token;
		std::istringstream tokenStream(value);
		while (std::getline(tokenStream, token, ';')) {
			auto label = EnumInfo<StatAttributeFlags>::Find(token.c_str());
			if (label) {
				flags |= *label;
			} else {
				OsiError("Invalid AttributeFlag: " << token);
			}
		}

		return flags;
	}

	void* CRPGStatsManager::BuildScriptCheckBlock(STDString const& source)
	{
		auto build = GetStaticSymbols().ScriptCheckBlock__Build;
		if (!build) {
			OsiError("ScriptCheckBlock::Build not available!");
			return nullptr;
		}

		return build(source, ConditionsManager.Variables, 0, (int)source.size());
	}

	void* CRPGStatsManager::BuildScriptCheckBlockFromProperties(STDString const& source)
	{
		STDString updated = source;
		for (size_t i = 0; i < updated.size(); i++) {
			if (updated[i] == ';') {
				updated[i] = '&';
			}
		}
		
		return BuildScriptCheckBlock(updated);
	}

	bool CRPGStatsManager::SetAttributeString(CRPGStats_Object * object, FixedString const& attributeName, const char * value)
	{
		int attributeIndex;
		auto typeInfo = GetAttributeInfo(object, attributeName, attributeIndex);
		if (typeInfo == nullptr) {
			OsiError("Couldn't fetch type info for " << object->Name << "." << attributeName);
			return false;
		}

		if (typeInfo->Name == GFS.strFixedString) {
			auto fs = GetOrCreateFixedString(value);
			if (fs != -1) {
				object->IndexedProperties[attributeIndex] = fs;
			} else {
				OsiError("Couldn't set " << object->Name << "." << attributeName << ": Unable to allocate pooled string");
			}
		} else if (typeInfo->Name == GFS.strAttributeFlags) {
			auto attrFlagsIndex = object->IndexedProperties[attributeIndex];
			if (attrFlagsIndex != -1) {
				auto & attrFlags = AttributeFlags[attrFlagsIndex];
				auto flags = StringToAttributeFlags(value);
				if (flags) {
					attrFlags = *flags;
				}
			} else {
				OsiError("Couldn't set " << object->Name << "." << attributeName << ": Stats entry has no AttributeFlags");
			}
		} else if (typeInfo->Values.ItemCount > 0) {
			auto enumIndex = typeInfo->Values.Find(ToFixedString(value));
			if (enumIndex != nullptr) {
				object->IndexedProperties[attributeIndex] = *enumIndex;
			} else {
				OsiError("Couldn't set " << object->Name << "." << attributeName << ": Value (\"" << value << "\") is not a valid enum label");
				return false;
			}
		} else {
			OsiError("Couldn't set " << object->Name << "." << attributeName << ": Inappropriate type: " << typeInfo->Name.Str);
			return false;
		}

		return true;
	}

	bool CRPGStatsManager::SetAttributeInt(CRPGStats_Object * object, FixedString const& attributeName, int32_t value)
	{
		int attributeIndex;
		auto typeInfo = GetAttributeInfo(object, attributeName, attributeIndex);
		if (typeInfo == nullptr) {
			OsiError("Couldn't fetch type info for " << object->Name << "." << attributeName);
			return false;
		}

		if (typeInfo->Name == GFS.strConstantInt) {
			object->IndexedProperties[attributeIndex] = value;
		} else if (typeInfo->Values.ItemCount > 0) {
			if (value > 0 && value < (int)typeInfo->Values.ItemCount) {
				object->IndexedProperties[attributeIndex] = value;
			} else {
				OsiError("Couldn't set " << object->Name << "." << attributeName << ": Enum index (\"" << value << "\") out of range");
				return false;
			}
		} else {
			OsiError("Couldn't set " << object->Name << "." << attributeName << ": Inappropriate type: " << typeInfo->Name.Str);
			return false;
		}

		return true;
	}

	CRPGStats_Object * StatFindObject(char const * name)
	{
		auto stats = GetStaticSymbols().GetStats();
		if (stats == nullptr) {
			OsiError("CRPGStatsManager not available");
			return nullptr;
		}

		auto object = stats->objects.Find(name);
		if (object == nullptr) {
			OsiError("Stat object '" << name << "' does not exist");
			return nullptr;
		}

		return object;
	}

	CRPGStats_Object * StatFindObject(int index)
	{
		auto stats = GetStaticSymbols().GetStats();
		if (stats == nullptr) {
			OsiError("CRPGStatsManager not available");
			return nullptr;
		}

		auto object = stats->objects.Find(index);
		if (object == nullptr) {
			OsiError("Stat object #" << index << " does not exist");
			return nullptr;
		}

		return object;
	}

	CRPGStats_Object * SkillPrototype::GetStats() const
	{
		return StatFindObject(RPGStatsObjectIndex);
	}

	CRPGStats_Object * StatusPrototype::GetStats() const
	{
		return StatFindObject(RPGStatsObjectIndex);
	}

#define DEFN_GETTER(type, name) decltype(CharacterStatsGetters::Wrapper##name) * decltype(CharacterStatsGetters::Wrapper##name)::gHook;
#include <GameDefinitions/CharacterGetters.inl>
#undef DEFN_GETTER

	decltype(CharacterStatsGetters::WrapperHitChance) * decltype(CharacterStatsGetters::WrapperHitChance)::gHook;

	void CharacterStatsGetters::WrapAll()
	{
		if (Wrapped) return;

#define DEFN_GETTER(type, name) if (Get##name != nullptr) { \
	Wrapper##name.Wrap(Get##name); \
}

#include <GameDefinitions/CharacterGetters.inl>
#undef DEFN_GETTER

		if (GetHitChance != nullptr) {
			WrapperHitChance.Wrap(GetHitChance);
		}

		Wrapped = true;
	}

	void CharacterStatsGetters::UnwrapAll()
	{
#define DEFN_GETTER(type, name) Wrapper##name.ClearHook();
#include <GameDefinitions/CharacterGetters.inl>
#undef DEFN_GETTER
		WrapperHitChance.ClearHook();
	}

	std::optional<int32_t> CharacterStatsGetters::GetStat(CDivinityStats_Character * character, 
		FixedString const& name, bool original, bool excludeBoosts)
	{
		auto statType = EnumInfo<StatGetterType>::Find(name);
		if (!statType) {
			return {};
		}

		switch (*statType) {
#define DEFN_GETTER(type, n) case StatGetterType::n: \
	return CharacterStatGetter<n##Tag>(Get##n, Wrapper##n, character, original, excludeBoosts);

#include <GameDefinitions/CharacterGetters.inl>
#undef DEFN_GETTER

		case StatGetterType::PhysicalResistance:
			return character->GetPhysicalResistance(excludeBoosts);

		case StatGetterType::PiercingResistance:
			return character->GetPiercingResistance(excludeBoosts);

		case StatGetterType::CorrosiveResistance:
			return character->GetCorrosiveResistance(excludeBoosts);

		case StatGetterType::MagicResistance:
			return character->GetMagicResistance(excludeBoosts);

		case StatGetterType::DamageBoost:
			return character->GetDamageBoost();

		default:
			OsiError("No stat fetcher defined for stat: '" << name << "'");
			return {};
		}
	}


	std::optional<int32_t> CDivinityStats_Character::GetHitChance(CDivinityStats_Character * target)
	{
		auto getter = GetStaticSymbols().CharStatsGetters.GetHitChance;
		if (getter == nullptr) {
			return {};
		}

		return getter(this, target);
	}


	std::optional<int32_t> CDivinityStats_Character::GetStat(FixedString const& name, bool excludeBoosts)
	{
		return GetStaticSymbols().CharStatsGetters.GetStat(this, name, false, excludeBoosts);
	}


	bool CDivinityStats_Character::HasTalent(TalentType talent, bool excludeBoosts)
	{
		auto getter = GetStaticSymbols().CharStatsGetters.GetTalent;
		if (getter) {
			return getter(this, talent, excludeBoosts);
		} else {
			return false;
		}
	}


	int32_t CDivinityStats_Character::GetAbility(AbilityType ability, bool excludeBoosts)
	{
		auto getter = GetStaticSymbols().CharStatsGetters.GetAbility;
		if (getter) {
			return getter(this, ability, excludeBoosts, false);
		} else {
			return 0;
		}
	}

	CDivinityStats_Item * CDivinityStats_Character::GetItemBySlot(ItemSlot slot, bool mustBeEquipped)
	{
		for (auto stat = ItemStats; stat != ItemStatsEnd; stat++) {
			if ((*stat)->ItemSlot == slot && (!mustBeEquipped || (*stat)->IsEquipped)) {
				auto index = (*stat)->ItemStatsHandle;
				if (index >= 0 
					&& index < DivStats->ItemList->Handles.Size()) {
					auto item = DivStats->ItemList->Handles.Start[index];
					if (item) {
						return item;
					}
				}
			}
		}

		return nullptr;
	}

	CDivinityStats_Item * CDivinityStats_Character::GetMainWeapon()
	{
		auto weapon = GetItemBySlot(ItemSlot::Weapon, true);
		if (weapon == nullptr || weapon->ItemType != EquipmentStatsType::Weapon) {
			weapon = GetItemBySlot(ItemSlot::Shield, true);
			if (weapon == nullptr || weapon->ItemType != EquipmentStatsType::Weapon) {
				weapon = DivStats->DefaultWeaponStats;
			}
		}

		return weapon;
	}

	CDivinityStats_Item * CDivinityStats_Character::GetOffHandWeapon()
	{
		auto mainWeapon = GetItemBySlot(ItemSlot::Weapon, true);
		if (mainWeapon == nullptr || mainWeapon->ItemType != EquipmentStatsType::Weapon) {
			return nullptr;
		}

		auto offHandWeapon = GetItemBySlot(ItemSlot::Shield, true);
		if (offHandWeapon == nullptr || offHandWeapon->ItemType != EquipmentStatsType::Weapon) {
			return nullptr;
		}

		return offHandWeapon;
	}

	bool CDivinityStats_Character::IsBoostActive(uint32_t conditionMask)
	{
		return conditionMask == 0
			|| (conditionMask & 1) && ActiveBoostConditions[1] > 0
			|| (conditionMask & 2) && ActiveBoostConditions[2] > 0
			|| (conditionMask & 4) && ActiveBoostConditions[3] > 0
			|| (conditionMask & 8) && ActiveBoostConditions[4] > 0
			|| (conditionMask & 0x10) && ActiveBoostConditions[5] > 0
			|| (conditionMask & 0x20) && ActiveBoostConditions[6] > 0
			|| (conditionMask & 0x40) && ActiveBoostConditions[7] > 0
			|| (conditionMask & 0x80) && ActiveBoostConditions[8] > 0
			|| (conditionMask & 0x100) && ActiveBoostConditions[9] > 0
			|| (conditionMask & 0x200) && ActiveBoostConditions[10] > 0
			|| (conditionMask & 0x300) && ActiveBoostConditions[11] > 0
			|| (conditionMask & 0x400) && ActiveBoostConditions[12] > 0
			|| (conditionMask & 0x500) && ActiveBoostConditions[13] > 0
			|| (conditionMask & 0x600) && ActiveBoostConditions[14] > 0
			|| (conditionMask & 0x700) && ActiveBoostConditions[15] > 0;
	}

	template <class IterFunc, class ItemFunc>
	int32_t ComputeCharacterStat(CDivinityStats_Character * character, int32_t initVal, IterFunc iter, ItemFunc iterItem, bool excludeBoosts)
	{
		int32_t value = initVal;

		auto lastStat = excludeBoosts ? (character->DynamicStats + 3) : character->DynamicStatsEnd;

		unsigned i = 0;
		for (auto statPtr = character->DynamicStats; statPtr != lastStat; statPtr++, i++) {
			auto & stat = *statPtr;
			if (stat->BoostConditionsMask == 0 || character->IsBoostActive(stat->BoostConditionsMask)) {
				value = iter(character, value, i, stat);
			}
		}

		if (!excludeBoosts) {
			auto & itemHandles = character->DivStats->ItemList->Handles;
			for (auto statPtr = character->ItemStats; statPtr != character->ItemStatsEnd; statPtr++) {
				auto & stat = **statPtr;
				if (stat.IsEquipped && stat.ItemStatsHandle >= 0 && stat.ItemStatsHandle < itemHandles.Size()) {
					auto item = itemHandles.Start[stat.ItemStatsHandle];
					if (item != nullptr) {
						value = iterItem(character, value, item);
					}
				}
			}
		}

		return value;
	}

	int32_t CDivinityStats_Character::GetPhysicalResistance(bool excludeBoosts)
	{
		auto physResist = std::min(DynamicStats[0]->PhysicalResistance, DynamicStats[0]->MaxResistance);
		auto addDynamic = [](CDivinityStats_Character * self, int32_t val, unsigned index, CharacterDynamicStat * stat) -> int32_t {
			return val + stat->PhysicalResistance;
		};
		auto addItem = [](CDivinityStats_Character * self, int32_t val, CDivinityStats_Item * item) -> int32_t {
			return val + item->GetPhysicalResistance();
		};

		return ComputeCharacterStat(this, physResist, addDynamic, addItem, excludeBoosts);
	}

	int32_t CDivinityStats_Character::GetPiercingResistance(bool excludeBoosts)
	{
		auto physResist = std::min(DynamicStats[0]->PiercingResistance, DynamicStats[0]->MaxResistance);
		auto addDynamic = [](CDivinityStats_Character * self, int32_t val, unsigned index, CharacterDynamicStat * stat) -> int32_t {
			return val + stat->PiercingResistance;
		};
		auto addItem = [](CDivinityStats_Character * self, int32_t val, CDivinityStats_Item * item) -> int32_t {
			return val + item->GetPiercingResistance();
		};

		return ComputeCharacterStat(this, physResist, addDynamic, addItem, excludeBoosts);
	}

	int32_t CDivinityStats_Character::GetCorrosiveResistance(bool excludeBoosts)
	{
		auto physResist = std::min(DynamicStats[0]->CorrosiveResistance, DynamicStats[0]->MaxResistance);
		auto addDynamic = [](CDivinityStats_Character * self, int32_t val, unsigned index, CharacterDynamicStat * stat) -> int32_t {
			return val + stat->CorrosiveResistance;
		};
		auto addItem = [](CDivinityStats_Character * self, int32_t val, CDivinityStats_Item * item) -> int32_t {
			return val + item->GetCorrosiveResistance();
		};

		return ComputeCharacterStat(this, physResist, addDynamic, addItem, excludeBoosts);
	}

	int32_t CDivinityStats_Character::GetMagicResistance(bool excludeBoosts)
	{
		auto physResist = std::min(DynamicStats[0]->MagicResistance, DynamicStats[0]->MaxResistance);
		auto addDynamic = [](CDivinityStats_Character * self, int32_t val, unsigned index, CharacterDynamicStat * stat) -> int32_t {
			return val + stat->MagicResistance;
		};
		auto addItem = [](CDivinityStats_Character * self, int32_t val, CDivinityStats_Item * item) -> int32_t {
			return val + item->GetMagicResistance();
		};

		return ComputeCharacterStat(this, physResist, addDynamic, addItem, excludeBoosts);
	}

	int32_t CDivinityStats_Character::GetDamageBoost()
	{
		int32_t damageBoost = 0;

		for (auto statPtr = DynamicStats; statPtr != DynamicStatsEnd; statPtr++) {
			auto & stat = **statPtr;
			if (stat.BoostConditionsMask == 0 || IsBoostActive(stat.BoostConditionsMask)) {
				damageBoost += stat.DamageBoost;
				if (Level > 1) {
					damageBoost += (int)round(stat.DamageBoostGrowthPerLevel * (Level - 1) * 0.1f);
				}
			}
		}

		return damageBoost;
	}

	int32_t CDivinityStats_Item::GetPhysicalResistance()
	{
		int32_t resistance = 0;
		for (auto stat = DynamicAttributes_Start; stat != DynamicAttributes_End; stat++) {
			resistance += (*stat)->PhysicalResistance;
		}

		return resistance;
	}

	int32_t CDivinityStats_Item::GetPiercingResistance()
	{
		int32_t resistance = 0;
		for (auto stat = DynamicAttributes_Start; stat != DynamicAttributes_End; stat++) {
			resistance += (*stat)->PiercingResistance;
		}

		return resistance;
	}

	int32_t CDivinityStats_Item::GetMagicResistance()
	{
		int32_t resistance = 0;
		for (auto stat = DynamicAttributes_Start; stat != DynamicAttributes_End; stat++) {
			resistance += (*stat)->MagicResistance;
		}

		return resistance;
	}

	int32_t CDivinityStats_Item::GetCorrosiveResistance()
	{
		int32_t resistance = 0;
		for (auto stat = DynamicAttributes_Start; stat != DynamicAttributes_End; stat++) {
			resistance += (*stat)->CorrosiveResistance;
		}

		return resistance;
	}


	bool CDivinityStats_Item::HasTalent(TalentType talent)
	{
		for (auto stat = DynamicAttributes_Start; stat != DynamicAttributes_End; stat++) {
			if ((*stat)->Talents.HasTalent(talent)) {
				return true;
			}
		}

		return false;
	}


	int32_t CDivinityStats_Item::GetAbility(AbilityType ability)
	{
		int32_t points = 0;
		for (auto stat = DynamicAttributes_Start; stat != DynamicAttributes_End; stat++) {
			points += (*stat)->AbilityModifiers[(unsigned)ability];
		}

		return points;
	}


	PropertyMapBase & CDivinityStats_Equipment_Attributes::GetPropertyMap() const
	{
		switch (StatsType) {
		case EquipmentStatsType::Weapon:
			return gEquipmentAttributesWeaponPropertyMap;

		case EquipmentStatsType::Armor:
			return gEquipmentAttributesArmorPropertyMap;

		case EquipmentStatsType::Shield:
			return gEquipmentAttributesShieldPropertyMap;

		default:
			OsiError("Unknown equipment stats type: " << (unsigned)StatsType);
			return gEquipmentAttributesPropertyMap;
		}
	}
}