#include "stdafx.h"

#include <cstdint>
#include <array>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <cassert>

#include <GameDefinitions/BaseTypes.h>
#include "OsirisProxy.h"

namespace osidbg
{
	EoCAllocFunc EoCAlloc{ nullptr };
	EoCFreeFunc EoCFree{ nullptr };

	void * GameAllocRaw(std::size_t size)
	{
#if defined(OSI_EOCAPP)
		return EoCAlloc(nullptr, size);
#else
		return EoCAlloc(nullptr, size, "", 1, "");
#endif
	}

	void GameFree(void * ptr)
	{
		EoCFree(nullptr, ptr);
	}


	void STDWString::Set(std::string const & s)
	{
		auto wcs = FromUTF8(s);
		Set(wcs);
	}


	void STDWString::Set(std::wstring const & s)
	{
		if (Capacity > 7) {
			GameFree(BufPtr);
		}

		Size = s.size();
		Capacity = s.size();

		if (Size > 7) {
			// FIXME - memory leak!
			BufPtr = GameAlloc<wchar_t>(Capacity + 1);
			wcscpy_s(BufPtr, Capacity + 1, s.c_str());
		} else {
			wcscpy_s(Buf, 8, s.c_str());
		}
	}


	void STDString::Set(std::string const & s)
	{
		if (Capacity > 15) {
			GameFree(BufPtr);
		}

		Size = s.size();
		Capacity = s.size();

		if (Size > 15) {
			// FIXME - memory leak!
			BufPtr = GameAlloc<char>(Capacity + 1);
			strcpy_s(BufPtr, Capacity + 1, s.c_str());
		} else {
			strcpy_s(Buf, 16, s.c_str());
		}
	}


	FileReaderPin::~FileReaderPin()
	{
		if (reader_ != nullptr) {
			gOsirisProxy->GetLibraryManager().DestroyFileReader(reader_);
		}
	}

	std::string FileReaderPin::ToString() const
	{
		if (!IsLoaded()) {
			OsiErrorS("File not loaded!");
			return "";
		}

		std::string contents;
		contents.resize(Size());
		memcpy(contents.data(), Buf(), Size());
		return contents;
	}


	CRPGStats_Modifier * ModifierList::GetAttributeInfo(const char * name, int * attributeIndex) const
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

	bool CRPGStatsManager::ObjectExists(FixedString statsId, FixedString type)
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

	std::optional<int> CRPGStatsManager::EnumLabelToIndex(const char * enumName, const char * enumLabel)
	{
		auto fs = ToFixedString(enumName);
		if (!fs) {
			OsiError("No enum named '" << enumName << "' exists");
			return {};
		}

		auto rpgEnum = modifierValueList.Find(enumName);
		if (rpgEnum == nullptr) {
			OsiError("No enum named '" << enumName << "' exists");
			return {};
		}

		auto index = rpgEnum->Values.Find(enumLabel);
		if (index == nullptr) {
			return {};
		} else {
			return *index;
		}
	}

	ModifierList * CRPGStatsManager::GetTypeInfo(CRPGStats_Object * object)
	{
		return modifierList.Find(object->ModifierListIndex);
	}

	RPGEnumeration * CRPGStatsManager::GetAttributeInfo(CRPGStats_Object * object, const char * attributeName, int & attributeIndex)
	{
		auto objModifiers = modifierList.Find(object->ModifierListIndex);
		assert(objModifiers != nullptr);

		auto modifierInfo = objModifiers->GetAttributeInfo(attributeName, &attributeIndex);
		if (modifierInfo == nullptr) {
			return nullptr;
		}

		auto typeInfo = modifierValueList.Find(modifierInfo->RPGEnumerationIndex);
		return typeInfo;
	}

	std::optional<FixedString> CRPGStatsManager::GetAttributeFixedString(CRPGStats_Object * object, const char * attributeName)
	{
		int attributeIndex;
		auto typeInfo = GetAttributeInfo(object, attributeName, attributeIndex);
		if (typeInfo == nullptr) {
			return {};
		}

		auto index = object->IndexedProperties[attributeIndex];
		if (strcmp(typeInfo->Name.Str, "FixedString") == 0) {
			return ModifierFSSet.Set.Buf[index];
		}
		else if (typeInfo->Values.ItemCount > 0) {
			auto enumLabel = typeInfo->Values.FindByValue(index);
			if (enumLabel) {
				return *enumLabel;
			}
			else {
				return {};
			}
		}
		else {
			return {};
		}
	}

	std::optional<int> CRPGStatsManager::GetAttributeInt(CRPGStats_Object * object, const char * attributeName)
	{
		int attributeIndex;
		auto typeInfo = GetAttributeInfo(object, attributeName, attributeIndex);
		if (typeInfo == nullptr) {
			return {};
		}

		auto index = object->IndexedProperties[attributeIndex];
		if (strcmp(typeInfo->Name.Str, "ConstantInt") == 0
			|| typeInfo->Values.ItemCount > 0) {
			return index;
		}
		else {
			return {};
		}
	}

	bool CRPGStatsManager::SetAttributeString(CRPGStats_Object * object, const char * attributeName, const char * value)
	{
		int attributeIndex;
		auto typeInfo = GetAttributeInfo(object, attributeName, attributeIndex);
		if (typeInfo == nullptr) {
			OsiError("Couldn't fetch type info for " << object->Name << "." << attributeName);
			return false;
		}

		if (strcmp(typeInfo->Name.Str, "FixedString") == 0) {
			auto fs = MakeFixedString(value);
			if (fs) {
				auto index = object->IndexedProperties[attributeIndex];
				ModifierFSSet.Set.Buf[index] = fs;
			} else {
				OsiError("Couldn't set " << object->Name << "." << attributeName << ": Value (\"" << value << "\") is not a valid FixedString");
			}
		} else if (typeInfo->Values.ItemCount > 0) {
			auto enumIndex = typeInfo->Values.Find(value);
			if (enumIndex != nullptr) {
				object->IndexedProperties[attributeIndex] = *enumIndex;
			} else {
				OsiError("Couldn't set " << object->Name << "." << attributeName << ": Value (\"" << value << "\") is not a valid enum label");
				return false;
			}
		} else {
			OsiError("Couldn't set " << object->Name << "." << attributeName << ": Inappropriate type");
			return false;
		}

		return true;
	}

	bool CRPGStatsManager::SetAttributeInt(CRPGStats_Object * object, const char * attributeName, int32_t value)
	{
		int attributeIndex;
		auto typeInfo = GetAttributeInfo(object, attributeName, attributeIndex);
		if (typeInfo == nullptr) {
			OsiError("Couldn't fetch type info for " << object->Name << "." << attributeName);
			return false;
		}

		if (strcmp(typeInfo->Name.Str, "ConstantInt") == 0) {
			object->IndexedProperties[attributeIndex] = value;
		} else if (typeInfo->Values.ItemCount > 0) {
			if (value > 0 && value < (int)typeInfo->Values.ItemCount) {
				object->IndexedProperties[attributeIndex] = value;
			} else {
				OsiError("Couldn't set " << object->Name << "." << attributeName << ": Enum index (\"" << value << "\") out of range");
				return false;
			}
		} else {
			OsiError("Couldn't set " << object->Name << "." << attributeName << ": Inappropriate type");
			return false;
		}

		return true;
	}

	uint32_t murmur3_32(const uint8_t* key, size_t len, uint32_t seed)
	{
		uint32_t h = seed;
		if (len > 3) {
			size_t i = len >> 2;
			do {
				uint32_t k;
				memcpy(&k, key, sizeof(uint32_t));
				key += sizeof(uint32_t);
				k *= 0xcc9e2d51;
				k = (k << 15) | (k >> 17);
				k *= 0x1b873593;
				h ^= k;
				h = (h << 13) | (h >> 19);
				h = h * 5 + 0xe6546b64;
			} while (--i);
		}
		if (len & 3) {
			size_t i = len & 3;
			uint32_t k = 0;
			do {
				k <<= 8;
				k |= key[i - 1];
			} while (--i);
			k *= 0xcc9e2d51;
			k = (k << 15) | (k >> 17);
			k *= 0x1b873593;
			h ^= k;
		}
		h ^= len;
		h ^= h >> 16;
		h *= 0x85ebca6b;
		h ^= h >> 13;
		h *= 0xc2b2ae35;
		h ^= h >> 16;
		return h;
	}

	uint32_t GlobalStringTable::Entry::Count() const
	{
		if (Next) {
			return StringPtrItems + Next->Count();
		}
		else {
			return StringPtrItems;
		}
	}

	char const * GlobalStringTable::Entry::Get(uint32_t i) const
	{
		if (i < StringPtrItems) {
			return StringPtrs[i];
		}
		else {
			return Next->Get(i - StringPtrItems);
		}
	}

	const char * GlobalStringTable::Find(char const * s, uint64_t length) const
	{
		auto const & entry = HashTable[Hash(s, length)];
		auto numItems = entry.Count();
		for (uint32_t i = 0; i < numItems; i++) {
			const char * str = entry.Get(i);
			// FIXME: length not available in buffer?
			//unsigned strLength = *(uint16_t const *)(s - 4);
			if (/*strLength == length && strncmp(s, str, length) == 0*/
				str != nullptr && strcmp(s, str) == 0)
			{
				return str;
			}
		}

		return nullptr;
	}

	uint32_t GlobalStringTable::Hash(char const * s, uint64_t length)
	{
		if (UseMurmur) {
			// D:OS2 DE
			return murmur3_32((const uint8_t *)s, length, 0) % 0xFFF1;
		}
		else {
			// D:OS2 classic
			uint32_t hash = 0;
			for (auto i = 0; i < length; i++) {
				hash = *s++ + 33 * hash;
			}

			return hash % 0xFFF1;
		}
	}

	Module const * ModManager::FindModByNameGuid(char const * nameGuid) const
	{
		auto modUuidFS = NameGuidToFixedString(nameGuid);
		if (!modUuidFS) {
			return nullptr;
		}

		auto & mods = GetModManager()->BaseModule.LoadOrderedModules.Set;

		for (uint32_t i = 0; i < mods.Size; i++) {
			auto const mod = mods.Buf + i;

			if (mod->Info.ModuleUUID == modUuidFS) {
				return mod;
			}
		}

		return nullptr;
	}

	void * EntityWorld::FindComponentByHandle(ObjectHandle componentHandle, ComponentType type)
	{
		if ((uint32_t)type >= Components.Size) {
			OsiError("Component type index " << (uint32_t)type << " too large!");
			return nullptr;
		}

		auto componentMgr = Components.Buf[(uint32_t)type].component;
		if (componentMgr == nullptr) {
			OsiError("Component type " << (uint32_t)type << " not bound!");
			return nullptr;
		}

		return componentMgr->FindComponentByHandle(&componentHandle);
	}

	void * EntityWorld::GetComponentByEntityHandle(ObjectHandle entityHandle, ComponentType type)
	{
		if (entityHandle.GetType() != 0) {
			OsiError("Entity handle has invalid type " << entityHandle.GetType());
			return nullptr;
		}

		auto index = entityHandle.GetIndex();
		if (index >= EntityEntries.Size) {
			OsiError("Entity index " << index << " too large!");
			return nullptr;
		}

		auto salt = entityHandle.GetSalt();
		if (salt != EntitySalts.Buf[index]) {
			OsiError("Salt mismatch on index " << index << "; " << salt << " != " << EntitySalts.Buf[index]);
			return nullptr;
		}

		auto entity = EntityEntries.Buf[index];
		if ((uint32_t)type >= entity->Layout.Entries.Size) {
			OsiError("Entity " << index << " has no component slot for " << (uint32_t)type);
			return nullptr;
		}

		auto const & layoutEntry = entity->Layout.Entries.Buf[(uint32_t)type];
		if (!layoutEntry.Handle.IsValid()) {
			OsiError("Entity " << index << " has no component bound to slot " << (uint32_t)type);
			return nullptr;
		}

		ObjectHandle componentHandle{ layoutEntry.Handle.Handle };
		auto componentMgr = Components.Buf[(uint32_t)type].component;
		return componentMgr->FindComponentByHandle(&componentHandle);
	}


	void PendingStatuses::Add(esv::Status * status)
	{
		PendingStatus pend { status, false };
		statuses_.insert(std::make_pair(status->StatusHandle, pend));
	}

	void PendingStatuses::Remove(esv::Status * status)
	{
		auto it = statuses_.find(status->StatusHandle);
		if (it != statuses_.end()) {
			statuses_.erase(it);
		} else {
			OsiError("Attempted to remove non-pending status " << std::hex << (int64_t)status->StatusHandle);
		}
	}

	PendingStatus * PendingStatuses::Find(ObjectHandle owner, ObjectHandle handle)
	{
		auto it = statuses_.find(handle);
		if (it != statuses_.end()) {
			auto & status = it->second;
			if (owner == status.Status->TargetHandle) {
				return &status;
			} else {
				OsiError("Attempted to retrieve pending status " << std::hex << (int64_t)status.Status->StatusHandle
					<< " on wrong character!");
				return nullptr;
			}
		} else {
			return nullptr;
		}
	}

	esv::Status * esv::StatusMachine::GetStatusByHandle(ObjectHandle handle) const
	{
		auto count = Statuses.Set.Size;
		for (uint32_t i = 0; i < count; i++) {
			auto status = Statuses.Set.Buf[i];
			if (status->StatusHandle == handle) {
				return status;
			}
		}

		return nullptr;
	}


	CharacterStatsGetters gCharacterStatsGetters;

#define DEFN_GETTER(type, name) decltype(CharacterStatsGetters::Wrapper##name) * decltype(CharacterStatsGetters::Wrapper##name)::gHook;
#include <GameDefinitions/CharacterGetters.inl>
#undef DEFN_GETTER

	decltype(CharacterStatsGetters::WrapperHitChance) * decltype(CharacterStatsGetters::WrapperHitChance)::gHook;

	void CharacterStatsGetters::WrapAll()
	{
		if (!ExtensionState::Get().HasFeatureFlag("FormulaOverrides")) return;
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

	void CharacterStatsGetters::ResetExtension()
	{
#define DEFN_GETTER(type, name) Wrapper##name.ClearHook();
#include <GameDefinitions/CharacterGetters.inl>
#undef DEFN_GETTER
		WrapperHitChance.ClearHook();
	}

	std::optional<int32_t> CharacterStatsGetters::GetStat(CDivinityStats_Character * character, 
		char const * name, bool original, bool baseStats)
	{
		auto statType = EnumInfo<StatGetterType>::Find(name);
		if (!statType) {
			return {};
		}

		switch (*statType) {
#define DEFN_GETTER(type, n) case StatGetterType::n: \
	return CharacterStatGetter<n##Tag>(Get##n, Wrapper##n, character, original, baseStats);

#include <GameDefinitions/CharacterGetters.inl>
#undef DEFN_GETTER

		default:
			OsiError("Unknown character stat: '" << name << "'");
			return {};
		}
	}


	std::optional<int32_t> CDivinityStats_Character::GetHitChance(CDivinityStats_Character * target)
	{
		if (gCharacterStatsGetters.GetHitChance == nullptr) {
			return {};
		}

		return gCharacterStatsGetters.GetHitChance(this, target);
	}


	std::optional<int32_t> CDivinityStats_Character::GetStat(char const * name, bool baseStats)
	{
		return gCharacterStatsGetters.GetStat(this, name, false, baseStats);
	}


	bool CDivinityStats_Character::HasTalent(TalentType talent, bool excludeBoosts)
	{
		if (gCharacterStatsGetters.GetTalent) {
			return gCharacterStatsGetters.GetTalent(this, talent, excludeBoosts);
		} else {
			return false;
		}
	}


	int32_t CDivinityStats_Character::GetAbility(AbilityType ability, bool excludeBoosts)
	{
		if (gCharacterStatsGetters.GetAbility) {
			return gCharacterStatsGetters.GetAbility(this, ability, excludeBoosts, false);
		} else {
			return 0;
		}
	}


	esv::Status * esv::Character::GetStatusByHandle(ObjectHandle statusHandle, bool returnPending) const
	{
		if (StatusMachine == nullptr) {
			return nullptr;
		}

		auto status = StatusMachine->GetStatusByHandle(statusHandle);
		if (status != nullptr) {
			return status;
		}

		if (returnPending) {
			ObjectHandle ownerHandle;
			this->GetObjectHandle(&ownerHandle);

			auto pendingStatus = ExtensionState::Get().PendingStatuses.Find(ownerHandle, statusHandle);
			if (pendingStatus != nullptr) {
				return pendingStatus->Status;
			}
		}

		return nullptr;
	}

	esv::Status * esv::Item::GetStatusByHandle(ObjectHandle statusHandle, bool returnPending) const
	{
		if (StatusMachine == nullptr) {
			return nullptr;
		}

		auto status = StatusMachine->GetStatusByHandle(statusHandle);
		if (status != nullptr) {
			return status;
		}

		if (returnPending) {
			ObjectHandle ownerHandle;
			this->GetObjectHandle(&ownerHandle);

			auto pendingStatus = ExtensionState::Get().PendingStatuses.Find(ownerHandle, statusHandle);
			if (pendingStatus != nullptr) {
				return pendingStatus->Status;
			}
		}

		return nullptr;
	}


	void DamagePairList::AddDamage(DamageType DamageType, int32_t Amount)
	{
		bool added{ false };
		for (uint32_t i = 0; i < Size; i++) {
			if (Buf[i].DamageType == DamageType) {
				auto newAmount = Buf[i].Amount + Amount;
				if (newAmount == 0) {
					Remove(i);
				} else {
					Buf[i].Amount += newAmount;
				}

				added = true;
				break;
			}
		}

		if (!added && Amount != 0) {
			TDamagePair dmg;
			dmg.DamageType = DamageType;
			dmg.Amount = Amount;
			if (!SafeAdd(dmg)) {
				OsiErrorS("DamageList capacity exceeded!");
			}
		}
	}



	bool CharacterDynamicStat::RemoveTalent(TalentType talent, bool remove)
	{
		if (remove) {
			return RemovedTalents.Set((uint32_t)talent);
		} else {
			return RemovedTalents.Clear((uint32_t)talent);
		}
	}

	bool CharacterDynamicStat::IsTalentRemoved(TalentType talent)
	{
		return RemovedTalents.IsSet((uint32_t)talent);
	}


	EnumInfo<ComponentType>::Label const EnumInfo<ComponentType>::Values[] = {
		{ ComponentType::AnimationBlueprint, "AnimationBlueprint" },
		{ ComponentType::Spline, "Spline" },
		{ ComponentType::Combat, "Combat" },
		{ ComponentType::CustomStats, "CustomStats" },
		{ ComponentType::GameMaster, "GameMaster" },
		{ ComponentType::GMJournalNode, "GMJournalNode" },
		{ ComponentType::Container, "Container" },
		{ ComponentType::ContainerElement, "ContainerElement" },
		{ ComponentType::Net, "Net" },
		{ ComponentType::OverviewMap, "OverviewMap" },
		{ ComponentType::Note, "Note" },
		{ ComponentType::Vignette, "Vignette" },
		{ ComponentType::CustomStatDefinition, "CustomStatDefinition" },
		{ ComponentType::Item, "Item" },
		{ ComponentType::Character, "Character" },
		{ ComponentType::Egg, "Egg" },
		{ ComponentType::Projectile, "Projectile" },
	};


	EnumInfo<PlayerUpgradeAttribute>::Label const EnumInfo<PlayerUpgradeAttribute>::Values[] = {
		{ PlayerUpgradeAttribute::Strength, "Strength" },
		{ PlayerUpgradeAttribute::Finesse, "Finesse" },
		{ PlayerUpgradeAttribute::Intelligence, "Intelligence" },
		{ PlayerUpgradeAttribute::Constitution, "Constitution" },
		{ PlayerUpgradeAttribute::Memory, "Memory" },
		{ PlayerUpgradeAttribute::Wits, "Wits" },
	};


	EnumInfo<StatGetterType>::Label const EnumInfo<StatGetterType>::Values[] = {
		{ StatGetterType::MaxMp, "MaxMp" },
		{ StatGetterType::APStart, "APStart" },
		{ StatGetterType::APRecovery, "APRecovery" },
		{ StatGetterType::APMaximum, "APMaximum" },
		{ StatGetterType::Strength, "Strength" },
		{ StatGetterType::Finesse, "Finesse" },
		{ StatGetterType::Intelligence, "Intelligence" },
		{ StatGetterType::Vitality, "Vitality" },
		{ StatGetterType::Memory, "Memory" },
		{ StatGetterType::Wits, "Wits" },
		{ StatGetterType::Accuracy, "Accuracy" },
		{ StatGetterType::Dodge, "Dodge" },
		{ StatGetterType::CriticalChance, "CriticalChance" },
		{ StatGetterType::FireResistance, "FireResistance" },
		{ StatGetterType::EarthResistance, "EarthResistance" },
		{ StatGetterType::WaterResistance, "WaterResistance" },
		{ StatGetterType::AirResistance, "AirResistance" },
		{ StatGetterType::PoisonResistance, "PoisonResistance" },
		{ StatGetterType::ShadowResistance, "ShadowResistance" },
		{ StatGetterType::CustomResistance, "CustomResistance" },
		{ StatGetterType::LifeSteal, "LifeSteal" },
		{ StatGetterType::Sight, "Sight" },
		{ StatGetterType::Hearing, "Hearing" },
		{ StatGetterType::Movement, "Movement" },
		{ StatGetterType::Initiative, "Initiative" },
		{ StatGetterType::Unknown, "Unknown" },
		{ StatGetterType::ChanceToHitBoost, "ChanceToHitBoost" }
	};


	EnumInfo<AbilityType>::Label const EnumInfo<AbilityType>::Values[] = {
		{ AbilityType::WarriorLore, "WarriorLore" },
		{ AbilityType::RangerLore, "RangerLore" },
		{ AbilityType::RogueLore, "RogueLore" },
		{ AbilityType::SingleHanded, "SingleHanded" },
		{ AbilityType::TwoHanded, "TwoHanded" },
		{ AbilityType::PainReflection, "PainReflection" },
		{ AbilityType::Ranged, "Ranged" },
		{ AbilityType::Shield, "Shield" },
		{ AbilityType::Reflexes, "Reflexes" },
		{ AbilityType::PhysicalArmorMastery, "PhysicalArmorMastery" },
		{ AbilityType::MagicArmorMastery, "MagicArmorMastery" },
		{ AbilityType::VitalityMastery, "VitalityMastery" },
		{ AbilityType::Sourcery, "Sourcery" },
		{ AbilityType::FireSpecialist, "FireSpecialist" },
		{ AbilityType::WaterSpecialist, "WaterSpecialist" },
		{ AbilityType::AirSpecialist, "AirSpecialist" },
		{ AbilityType::EarthSpecialist, "EarthSpecialist" },
		{ AbilityType::Necromancy, "Necromancy" },
		{ AbilityType::Summoning, "Summoning" },
		{ AbilityType::Polymorph, "Polymorph" },
		{ AbilityType::Telekinesis, "Telekinesis" },
		{ AbilityType::Repair, "Repair" },
		{ AbilityType::Sneaking, "Sneaking" },
		{ AbilityType::Pickpocket, "Pickpocket" },
		{ AbilityType::Thievery, "Thievery" },
		{ AbilityType::Loremaster, "Loremaster" },
		{ AbilityType::Crafting, "Crafting" },
		{ AbilityType::Barter, "Barter" },
		{ AbilityType::Charm, "Charm" },
		{ AbilityType::Intimidate, "Intimidate" },
		{ AbilityType::Reason, "Reason" },
		{ AbilityType::Persuasion, "Persuasion" },
		{ AbilityType::Leadership, "Leadership" },
		{ AbilityType::Luck, "Luck" },
		{ AbilityType::DualWielding, "DualWielding" },
		{ AbilityType::Wand, "Wand" },
		{ AbilityType::Perseverance, "Perseverance" },
		{ AbilityType::Runecrafting, "Runecrafting" },
		{ AbilityType::Brewmaster, "Brewmaster" },
		{ AbilityType::Sulfurology, "Sulfurology" }
	};


	EnumInfo<CriticalRoll>::Label const EnumInfo<CriticalRoll>::Values[] = {
		{ CriticalRoll::Roll, "Roll" },
		{ CriticalRoll::Critical, "Critical" },
		{ CriticalRoll::NotCritical, "NotCritical" }
	};

	EnumInfo<HighGroundBonus>::Label const EnumInfo<HighGroundBonus>::Values[] = {
		{ HighGroundBonus::Unknown, "Unknown" },
		{ HighGroundBonus::HighGround, "HighGround" },
		{ HighGroundBonus::EvenGround, "EvenGround" },
		{ HighGroundBonus::LowGround, "LowGround" }
	};

	EnumInfo<StatusFlags0>::Label const EnumInfo<StatusFlags0>::Values[] = {
		{ StatusFlags0::SF0_KeepAlive, "KeepAlive" },
		{ StatusFlags0::SF0_IsOnSourceSurface, "IsOnSourceSurface" },
		{ StatusFlags0::SF0_IsFromItem, "IsFromItem" },
		{ StatusFlags0::SF0_Channeled, "Channeled" },
		{ StatusFlags0::SF0_IsLifeTimeSet, "IsLifeTimeSet" },
		{ StatusFlags0::SF0_InitiateCombat, "InitiateCombat" },
		{ StatusFlags0::SF0_Influence, "Influence" }
	};

	EnumInfo<StatusFlags1>::Label const EnumInfo<StatusFlags1>::Values[] = {
		{ StatusFlags1::SF1_BringIntoCombat, "BringIntoCombat" },
		{ StatusFlags1::SF1_IsHostileAct, "IsHostileAct" },
		{ StatusFlags1::SF1_IsInvulnerable, "IsInvulnerable" },
		{ StatusFlags1::SF1_IsResistingDeath, "IsResistingDeath" }
	};

	EnumInfo<StatusFlags2>::Label const EnumInfo<StatusFlags2>::Values[] = {
		{ StatusFlags2::SF2_ForceStatus, "ForceStatus" },
		{ StatusFlags2::SF2_ForceFailStatus, "ForceFailStatus" },
		{ StatusFlags2::SF2_RequestDelete, "RequestDelete" },
		{ StatusFlags2::SF2_RequestDeleteAtTurnEnd, "RequestDeleteAtTurnEnd" },
		{ StatusFlags2::SF2_Started, "Started" }
	};

	EnumInfo<CauseType>::Label const EnumInfo<CauseType>::Values[] = {
		{ CauseType::None, "None" },
		{ CauseType::SurfaceMove, "SurfaceMove" },
		{ CauseType::SurfaceCreate, "SurfaceCreate" },
		{ CauseType::SurfaceStatus, "SurfaceStatus" },
		{ CauseType::StatusEnter, "StatusEnter" },
		{ CauseType::StatusTick, "StatusTick" },
		{ CauseType::Attack, "Attack" },
		{ CauseType::Offhand, "Offhand" },
		{ CauseType::GM, "GM" }
	};

	EnumInfo<DeathType>::Label const EnumInfo<DeathType>::Values[] = {
		{ DeathType::None, "None" },
		{ DeathType::Physical, "Physical" },
		{ DeathType::Piercing, "Piercing" },
		{ DeathType::Arrow, "Arrow" },
		{ DeathType::DoT, "DoT" },
		{ DeathType::Incinerate, "Incinerate" },
		{ DeathType::Acid, "Acid" },
		{ DeathType::Electrocution, "Electrocution" },
		{ DeathType::FrozenShatter, "FrozenShatter" },
		{ DeathType::PetrifiedShatter, "PetrifiedShatter" },
		{ DeathType::Explode, "Explode" },
		{ DeathType::Surrender, "Surrender" },
		{ DeathType::Hang, "Hang" },
		{ DeathType::KnockedDown, "KnockedDown" },
		{ DeathType::Lifetime, "Lifetime" },
		{ DeathType::Sulfur, "Sulfur" },
		{ DeathType::Sentinel, "Sentinel" }
	};

	EnumInfo<DamageType>::Label const EnumInfo<DamageType>::Values[] = {
		{ DamageType::None, "None" },
		{ DamageType::Physical, "Physical" },
		{ DamageType::Piercing, "Piercing" },
		{ DamageType::Corrosive, "Corrosive" },
		{ DamageType::Magic, "Magic" },
		{ DamageType::Chaos, "Chaos" },
		{ DamageType::Fire, "Fire" },
		{ DamageType::Air, "Air" },
		{ DamageType::Water, "Water" },
		{ DamageType::Earth, "Earth" },
		{ DamageType::Poison, "Poison" },
		{ DamageType::Shadow, "Shadow" },
		{ DamageType::Sulfuric, "Sulfuric" },
		{ DamageType::Sentinel, "Sentinel" }
	};

	EnumInfo<HitType>::Label const EnumInfo<HitType>::Values[] = {
		{ HitType::Melee, "Melee" },
		{ HitType::Magic, "Magic" },
		{ HitType::Ranged, "Ranged" },
		{ HitType::WeaponDamage, "WeaponDamage" },
		{ HitType::Surface, "Surface" },
		{ HitType::DoT, "DoT" },
		{ HitType::Reflected, "Reflected" }
	};

	EnumInfo<ItemSlot>::Label const EnumInfo<ItemSlot>::Values[] = {
		{ ItemSlot::Helmet, "Helmet" },
		{ ItemSlot::Breast, "Breast" },
		{ ItemSlot::Leggings, "Leggings" },
		{ ItemSlot::Weapon, "Weapon" },
		{ ItemSlot::Shield, "Shield" },
		{ ItemSlot::Ring, "Ring" },
		{ ItemSlot::Belt, "Belt" },
		{ ItemSlot::Boots, "Boots" },
		{ ItemSlot::Gloves, "Gloves" },
		{ ItemSlot::Amulet, "Amulet" },
		{ ItemSlot::Ring2, "Ring2" },
		{ ItemSlot::Wings, "Wings" },
		{ ItemSlot::Horns, "Horns" },
		{ ItemSlot::Overhead, "Overhead" }
	};

	EnumInfo<HitFlag>::Label const EnumInfo<HitFlag>::Values[] = {
		{ HitFlag::HF_Hit, "Hit" },
		{ HitFlag::HF_Blocked, "Blocked" },
		{ HitFlag::HF_Dodged, "Dodged" },
		{ HitFlag::HF_Missed, "Missed" },
		{ HitFlag::HF_CriticalHit, "CriticalHit" },
		{ HitFlag::HF_AlwaysBackstab, "AlwaysBackstab" },
		{ HitFlag::HF_FromSetHP, "FromSetHP" },
		{ HitFlag::HF_DontCreateBloodSurface, "DontCreateBloodSurface" },
		{ HitFlag::HF_Reflection, "Reflection" },
		{ HitFlag::HF_NoDamageOnOwner, "NoDamageOnOwner" },
		{ HitFlag::HF_FromShacklesOfPain, "FromShacklesOfPain" },
		{ HitFlag::HF_DamagedMagicArmor, "DamagedMagicArmor" },
		{ HitFlag::HF_DamagedPhysicalArmor, "DamagedPhysicalArmor" },
		{ HitFlag::HF_DamagedVitality, "DamagedVitality" },
		{ HitFlag::HF_PropagatedFromOwner, "PropagatedFromOwner" },
		{ HitFlag::HF_Surface, "Surface" },
		{ HitFlag::HF_DoT, "DoT" },
		{ HitFlag::HF_ProcWindWalker, "ProcWindWalker" },
		// Custom flags
		{ HitFlag::HF_NoEvents, "NoEvents" }
	};

	EnumInfo<StatusHealType>::Label const EnumInfo<StatusHealType>::Values[] = {
		{ StatusHealType::SHT_None, "None" },
		{ StatusHealType::SHT_Vitality, "Vitality" },
		{ StatusHealType::SHT_PhysicalArmor, "PhysicalArmor" },
		{ StatusHealType::SHT_MagicArmor, "MagicArmor" },
		{ StatusHealType::SHT_AllArmor, "AllArmor" },
		{ StatusHealType::SHT_All, "All" },
		{ StatusHealType::SHT_Source, "Source" }
	};

	EnumInfo<StatAttributeFlags>::Label const EnumInfo<StatAttributeFlags>::Values[] = { 
		{ StatAttributeFlags::AF_FreezeImmunity, "FreezeImmunity" },
		{ StatAttributeFlags::AF_BurnImmunity, "BurnImmunity" },
		{ StatAttributeFlags::AF_StunImmunity, "StunImmunity" },
		{ StatAttributeFlags::AF_PoisonImmunity, "PoisonImmunity" },
		{ StatAttributeFlags::AF_CharmImmunity, "CharmImmunity" },
		{ StatAttributeFlags::AF_FearImmunity, "FearImmunity" },
		{ StatAttributeFlags::AF_KnockdownImmunity, "KnockdownImmunity" },
		{ StatAttributeFlags::AF_MuteImmunity, "MuteImmunity" },
		{ StatAttributeFlags::AF_ChilledImmunity, "ChilledImmunity" },
		{ StatAttributeFlags::AF_WarmImmunity, "WarmImmunity" },
		{ StatAttributeFlags::AF_WetImmunity, "WetImmunity" },
		{ StatAttributeFlags::AF_BleedingImmunity, "BleedingImmunity" },
		{ StatAttributeFlags::AF_CrippledImmunity, "CrippledImmunity" },
		{ StatAttributeFlags::AF_BlindImmunity, "BlindImmunity" },
		{ StatAttributeFlags::AF_CursedImmunity, "CursedImmunity" },
		{ StatAttributeFlags::AF_WeakImmunity, "WeakImmunity" },
		{ StatAttributeFlags::AF_SlowedImmunity, "SlowedImmunity" },
		{ StatAttributeFlags::AF_DiseasedImmunity, "DiseasedImmunity" },
		{ StatAttributeFlags::AF_InfectiousDiseasedImmunity, "InfectiousDiseasedImmunity" },
		{ StatAttributeFlags::AF_PetrifiedImmunity, "PetrifiedImmunity" },
		{ StatAttributeFlags::AF_DrunkImmunity, "DrunkImmunity" },
		{ StatAttributeFlags::AF_SlippingImmunity, "SlippingImmunity" },
		{ StatAttributeFlags::AF_FreezeContact, "FreezeContact" },
		{ StatAttributeFlags::AF_BurnContact, "BurnContact" },
		{ StatAttributeFlags::AF_StunContact, "StunContact" },
		{ StatAttributeFlags::AF_PoisonContact, "PoisonContact" },
		{ StatAttributeFlags::AF_ChillContact, "ChillContact" },
		{ StatAttributeFlags::AF_Torch, "Torch" },
		{ StatAttributeFlags::AF_Arrow, "Arrow" },
		{ StatAttributeFlags::AF_Unbreakable, "Unbreakable" },
		{ StatAttributeFlags::AF_Unrepairable, "Unrepairable" },
		{ StatAttributeFlags::AF_Unstorable, "Unstorable" },
		{ StatAttributeFlags::AF_Grounded, "Grounded" },
		{ StatAttributeFlags::AF_HastedImmunity, "HastedImmunity" },
		{ StatAttributeFlags::AF_TauntedImmunity, "TauntedImmunity" },
		{ StatAttributeFlags::AF_SleepingImmunity, "SleepingImmunity" },
		{ StatAttributeFlags::AF_AcidImmunity, "AcidImmunity" },
		{ StatAttributeFlags::AF_SuffocatingImmunity, "SuffocatingImmunity" },
		{ StatAttributeFlags::AF_RegeneratingImmunity, "RegeneratingImmunity" },
		{ StatAttributeFlags::AF_DisarmedImmunity, "DisarmedImmunity" },
		{ StatAttributeFlags::AF_DecayingImmunity, "DecayingImmunity" },
		{ StatAttributeFlags::AF_ClairvoyantImmunity, "ClairvoyantImmunity" },
		{ StatAttributeFlags::AF_EnragedImmunity, "EnragedImmunity" },
		{ StatAttributeFlags::AF_BlessedImmunity, "BlessedImmunity" },
		{ StatAttributeFlags::AF_ProtectFromSummon, "ProtectFromSummon" },
		{ StatAttributeFlags::AF_Floating, "Floating" },
		{ StatAttributeFlags::AF_DeflectProjectiles, "DeflectProjectiles" },
		{ StatAttributeFlags::AF_IgnoreClouds, "IgnoreClouds" },
		{ StatAttributeFlags::AF_MadnessImmunity, "MadnessImmunity" },
		{ StatAttributeFlags::AF_ChickenImmunity, "ChickenImmunity" },
		{ StatAttributeFlags::AF_IgnoreCursedOil, "IgnoreCursedOil" },
		{ StatAttributeFlags::AF_ShockedImmunity, "ShockedImmunity" },
		{ StatAttributeFlags::AF_WebImmunity, "WebImmunity" },
		{ StatAttributeFlags::AF_LootableWhenEquipped, "LootableWhenEquipped" },
		{ StatAttributeFlags::AF_PickpocketableWhenEquipped, "PickpocketableWhenEquipped" },
		{ StatAttributeFlags::AF_LoseDurabilityOnCharacterHit, "LoseDurabilityOnCharacterHit" },
		{ StatAttributeFlags::AF_EntangledContact, "EntangledContact" },
		{ StatAttributeFlags::AF_ShacklesOfPainImmunity, "ShacklesOfPainImmunity" },
		{ StatAttributeFlags::AF_MagicalSulfur, "MagicalSulfur" },
		{ StatAttributeFlags::AF_ThrownImmunity, "ThrownImmunity" },
		{ StatAttributeFlags::AF_InvisibilityImmunity, "InvisibilityImmunity" }
	};

	EnumInfo<TalentType>::Label const EnumInfo<TalentType>::Values[] = {
		{ TalentType::TALENT_None, "None" },
		{ TalentType::TALENT_ItemMovement, "ItemMovement" },
		{ TalentType::TALENT_ItemCreation, "ItemCreation" },
		{ TalentType::TALENT_Flanking, "Flanking" },
		{ TalentType::TALENT_AttackOfOpportunity, "AttackOfOpportunity" },
		{ TalentType::TALENT_Backstab, "Backstab" },
		{ TalentType::TALENT_Trade, "Trade" },
		{ TalentType::TALENT_Lockpick, "Lockpick" },
		{ TalentType::TALENT_ChanceToHitRanged, "ChanceToHitRanged" },
		{ TalentType::TALENT_ChanceToHitMelee, "ChanceToHitMelee" },
		{ TalentType::TALENT_Damage, "Damage" },
		{ TalentType::TALENT_ActionPoints, "ActionPoints" },
		{ TalentType::TALENT_ActionPoints2, "ActionPoints2" },
		{ TalentType::TALENT_Criticals, "Criticals" },
		{ TalentType::TALENT_IncreasedArmor, "IncreasedArmor" },
		{ TalentType::TALENT_Sight, "Sight" },
		{ TalentType::TALENT_ResistFear, "ResistFear" },
		{ TalentType::TALENT_ResistKnockdown, "ResistKnockdown" },
		{ TalentType::TALENT_ResistStun, "ResistStun" },
		{ TalentType::TALENT_ResistPoison, "ResistPoison" },
		{ TalentType::TALENT_ResistSilence, "ResistSilence" },
		{ TalentType::TALENT_ResistDead, "ResistDead" },
		{ TalentType::TALENT_Carry, "Carry" },
		{ TalentType::TALENT_Throwing, "Throwing" },
		{ TalentType::TALENT_Repair, "Repair" },
		{ TalentType::TALENT_ExpGain, "ExpGain" },
		{ TalentType::TALENT_ExtraStatPoints, "ExtraStatPoints" },
		{ TalentType::TALENT_ExtraSkillPoints, "ExtraSkillPoints" },
		{ TalentType::TALENT_Durability, "Durability" },
		{ TalentType::TALENT_Awareness, "Awareness" },
		{ TalentType::TALENT_Vitality, "Vitality" },
		{ TalentType::TALENT_FireSpells, "FireSpells" },
		{ TalentType::TALENT_WaterSpells, "WaterSpells" },
		{ TalentType::TALENT_AirSpells, "AirSpells" },
		{ TalentType::TALENT_EarthSpells, "EarthSpells" },
		{ TalentType::TALENT_Charm, "Charm" },
		{ TalentType::TALENT_Intimidate, "Intimidate" },
		{ TalentType::TALENT_Reason, "Reason" },
		{ TalentType::TALENT_Luck, "Luck" },
		{ TalentType::TALENT_Initiative, "Initiative" },
		{ TalentType::TALENT_InventoryAccess, "InventoryAccess" },
		{ TalentType::TALENT_AvoidDetection, "AvoidDetection" },
		{ TalentType::TALENT_AnimalEmpathy, "AnimalEmpathy" },
		{ TalentType::TALENT_Escapist, "Escapist" },
		{ TalentType::TALENT_StandYourGround, "StandYourGround" },
		{ TalentType::TALENT_SurpriseAttack, "SurpriseAttack" },
		{ TalentType::TALENT_LightStep, "LightStep" },
		{ TalentType::TALENT_ResurrectToFullHealth, "ResurrectToFullHealth" },
		{ TalentType::TALENT_Scientist, "Scientist" },
		{ TalentType::TALENT_Raistlin, "Raistlin" },
		{ TalentType::TALENT_MrKnowItAll, "MrKnowItAll" },
		{ TalentType::TALENT_WhatARush, "WhatARush" },
		{ TalentType::TALENT_FaroutDude, "FaroutDude" },
		{ TalentType::TALENT_Leech, "Leech" },
		{ TalentType::TALENT_ElementalAffinity, "ElementalAffinity" },
		{ TalentType::TALENT_FiveStarRestaurant, "FiveStarRestaurant" },
		{ TalentType::TALENT_Bully, "Bully" },
		{ TalentType::TALENT_ElementalRanger, "ElementalRanger" },
		{ TalentType::TALENT_LightningRod, "LightningRod" },
		{ TalentType::TALENT_Politician, "Politician" },
		{ TalentType::TALENT_WeatherProof, "WeatherProof" },
		{ TalentType::TALENT_LoneWolf, "LoneWolf" },
		{ TalentType::TALENT_Zombie, "Zombie" },
		{ TalentType::TALENT_Demon, "Demon" },
		{ TalentType::TALENT_IceKing, "IceKing" },
		{ TalentType::TALENT_Courageous, "Courageous" },
		{ TalentType::TALENT_GoldenMage, "GoldenMage" },
		{ TalentType::TALENT_WalkItOff, "WalkItOff" },
		{ TalentType::TALENT_FolkDancer, "FolkDancer" },
		{ TalentType::TALENT_SpillNoBlood, "SpillNoBlood" },
		{ TalentType::TALENT_Stench, "Stench" },
		{ TalentType::TALENT_Kickstarter, "Kickstarter" },
		{ TalentType::TALENT_WarriorLoreNaturalArmor, "WarriorLoreNaturalArmor" },
		{ TalentType::TALENT_WarriorLoreNaturalHealth, "WarriorLoreNaturalHealth" },
		{ TalentType::TALENT_WarriorLoreNaturalResistance, "WarriorLoreNaturalResistance" },
		{ TalentType::TALENT_RangerLoreArrowRecover, "RangerLoreArrowRecover" },
		{ TalentType::TALENT_RangerLoreEvasionBonus, "RangerLoreEvasionBonus" },
		{ TalentType::TALENT_RangerLoreRangedAPBonus, "RangerLoreRangedAPBonus" },
		{ TalentType::TALENT_RogueLoreDaggerAPBonus, "RogueLoreDaggerAPBonus" },
		{ TalentType::TALENT_RogueLoreDaggerBackStab, "RogueLoreDaggerBackStab" },
		{ TalentType::TALENT_RogueLoreMovementBonus, "RogueLoreMovementBonus" },
		{ TalentType::TALENT_RogueLoreHoldResistance, "RogueLoreHoldResistance" },
		{ TalentType::TALENT_NoAttackOfOpportunity, "NoAttackOfOpportunity" },
		{ TalentType::TALENT_WarriorLoreGrenadeRange, "WarriorLoreGrenadeRange" },
		{ TalentType::TALENT_RogueLoreGrenadePrecision, "RogueLoreGrenadePrecision" },
		{ TalentType::TALENT_WandCharge, "WandCharge" },
		{ TalentType::TALENT_DualWieldingDodging, "DualWieldingDodging" },
		{ TalentType::TALENT_Human_Inventive, "Human_Inventive" },
		{ TalentType::TALENT_Human_Civil, "Human_Civil" },
		{ TalentType::TALENT_Elf_Lore, "Elf_Lore" },
		{ TalentType::TALENT_Elf_CorpseEating, "Elf_CorpseEating" },
		{ TalentType::TALENT_Dwarf_Sturdy, "Dwarf_Sturdy" },
		{ TalentType::TALENT_Dwarf_Sneaking, "Dwarf_Sneaking" },
		{ TalentType::TALENT_Lizard_Resistance, "Lizard_Resistance" },
		{ TalentType::TALENT_Lizard_Persuasion, "Lizard_Persuasion" },
		{ TalentType::TALENT_Perfectionist, "Perfectionist" },
		{ TalentType::TALENT_Executioner, "Executioner" },
		{ TalentType::TALENT_ViolentMagic, "ViolentMagic" },
		{ TalentType::TALENT_QuickStep, "QuickStep" },
		{ TalentType::TALENT_Quest_SpidersKiss_Str, "Quest_SpidersKiss_Str" },
		{ TalentType::TALENT_Quest_SpidersKiss_Int, "Quest_SpidersKiss_Int" },
		{ TalentType::TALENT_Quest_SpidersKiss_Per, "Quest_SpidersKiss_Per" },
		{ TalentType::TALENT_Quest_SpidersKiss_Null, "Quest_SpidersKiss_Null" },
		{ TalentType::TALENT_Memory, "Memory" },
		{ TalentType::TALENT_Quest_TradeSecrets, "Quest_TradeSecrets" },
		{ TalentType::TALENT_Quest_GhostTree, "Quest_GhostTree" },
		{ TalentType::TALENT_BeastMaster, "BeastMaster" },
		{ TalentType::TALENT_LivingArmor, "LivingArmor" },
		{ TalentType::TALENT_Torturer, "Torturer" },
		{ TalentType::TALENT_Ambidextrous, "Ambidextrous" },
		{ TalentType::TALENT_Unstable, "Unstable" },
		{ TalentType::TALENT_ResurrectExtraHealth, "ResurrectExtraHealth" },
		{ TalentType::TALENT_NaturalConductor, "NaturalConductor" },
		{ TalentType::TALENT_Quest_Rooted, "Quest_Rooted" },
		{ TalentType::TALENT_PainDrinker, "PainDrinker" },
		{ TalentType::TALENT_DeathfogResistant, "DeathfogResistant" },
		{ TalentType::TALENT_Sourcerer, "Sourcerer" },
		{ TalentType::TALENT_Rager, "Rager" },
		{ TalentType::TALENT_Elementalist, "Elementalist" },
		{ TalentType::TALENT_Sadist, "Sadist" },
		{ TalentType::TALENT_Haymaker, "Haymaker" },
		{ TalentType::TALENT_Gladiator, "Gladiator" },
		{ TalentType::TALENT_Indomitable, "Indomitable" },
		{ TalentType::TALENT_WildMag, "WildMag" },
		{ TalentType::TALENT_Jitterbug, "Jitterbug" },
		{ TalentType::TALENT_Soulcatcher, "Soulcatcher" },
		{ TalentType::TALENT_MasterThief, "MasterThief" },
		{ TalentType::TALENT_GreedyVessel, "GreedyVessel" },
		{ TalentType::TALENT_MagicCycles, "MagicCycles" }
	};

}