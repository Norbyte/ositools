#include "stdafx.h"

#include <GameDefinitions/BaseTypes.h>
#include <GameDefinitions/Symbols.h>
#include <GameDefinitions/Enumerations.h>
#include "OsirisProxy.h"
#include <PropertyMaps.h>

namespace osidbg
{
	StaticSymbols gStaticSymbols;

	void * GameAllocRaw(std::size_t size)
	{
#if defined(OSI_EOCAPP)
		return gStaticSymbols.EoCAlloc(nullptr, size);
#else
		return gStaticSymbols.EoCAlloc(nullptr, size, "", 1, "");
#endif
	}

	void GameFree(void * ptr)
	{
		gStaticSymbols.EoCFree(nullptr, ptr);
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

#if defined(OSI_EOCAPP)
	std::string StaticSymbols::ToPath(std::string const & path, PathRootType root) const
	{
		if (PathRoots == nullptr) {
			ERR("LibraryManager::ToPath(): Path root API not available!");
			return "";
		}

		auto rootPath = PathRoots[(unsigned)root];

		std::string absolutePath = rootPath->GetPtr();
		absolutePath += "/" + path;
		return absolutePath;
	}

	FileReaderPin StaticSymbols::MakeFileReader(std::string const & path, PathRootType root) const
	{
		if (PathRoots == nullptr || FileReaderCtor == nullptr) {
			ERR("LibraryManager::MakeFileReader(): File reader API not available!");
			return FileReaderPin(nullptr);
		}

		auto absolutePath = ToPath(path, root);

		Path lsPath;
		lsPath.Name.Set(absolutePath);

		auto reader = new FileReader();
		FileReaderCtor(reader, &lsPath, 2);
		return FileReaderPin(reader);
	}
#else
	std::string StaticSymbols::ToPath(std::string const & path, PathRootType root) const
	{
		if (GetPrefixForRoot == nullptr) {
			ERR("LibraryManager::ToPath(): Path root API not available!");
			return "";
		}

		StringView rootPath;
		GetPrefixForRoot(&rootPath, (unsigned)root);

		std::string absolutePath = rootPath.Buf;
		absolutePath += "/" + path;
		return absolutePath;
	}

	FileReaderPin StaticSymbols::MakeFileReader(std::string const & path, PathRootType root) const
	{
		if (GetPrefixForRoot == nullptr || FileReaderCtor == nullptr) {
			ERR("LibraryManager::MakeFileReader(): File reader API not available!");
			return FileReaderPin(nullptr);
		}

		auto absolutePath = ToPath(path, root);

		Path lsPath;
		lsPath.Name.Set(absolutePath);

		auto reader = new FileReader();
		FileReaderCtor(reader, &lsPath, 2);
		return FileReaderPin(reader);
	}
#endif

	void StaticSymbols::DestroyFileReader(FileReader * reader)
	{
		if (FileReaderDtor != nullptr) {
			FileReaderDtor(reader);
		}
	}


	FileReaderPin::~FileReaderPin()
	{
		if (reader_ != nullptr) {
			gStaticSymbols.DestroyFileReader(reader_);
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

	CRPGStats_Modifier * CRPGStatsManager::GetModifierInfo(const char * modifierListName, const char * modifierName)
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

	RPGEnumeration * CRPGStatsManager::GetAttributeInfo(CRPGStats_Object * object, const char * attributeName, int & attributeIndex)
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

	std::optional<char const *> CRPGStatsManager::GetAttributeString(CRPGStats_Object * object, const char * attributeName)
	{
		int attributeIndex;
		auto typeInfo = GetAttributeInfo(object, attributeName, attributeIndex);
		if (typeInfo == nullptr) {
			return {};
		}

		auto index = object->IndexedProperties[attributeIndex];
		if (strcmp(typeInfo->Name.Str, "FixedString") == 0) {
			return ModifierFSSet[index].Str;
		} else if (strcmp(typeInfo->Name.Str, "AttributeFlags") == 0) {
			if (index != -1) {
				auto attrFlags = AttributeFlags[index];
				std::string flagsStr;
				for (auto i = 0; i < 64; i++) {
					if (attrFlags & (1ull << i)) {
						auto label = EnumInfo<StatAttributeFlags>::Find((StatAttributeFlags)(1ull << i));
						if (label) {
							if (!flagsStr.empty()) {
								flagsStr += ';';
							}

							flagsStr += *label;
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

	std::optional<int> CRPGStatsManager::GetAttributeIntScaled(CRPGStats_Object * object, const char * attributeName, int level)
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

	std::optional<uint64_t> CRPGStatsManager::StringToAttributeFlags(const char * value)
	{
		uint64_t flags{ 0 };
		std::string token;
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

	bool CRPGStatsManager::SetAttributeString(CRPGStats_Object * object, const char * attributeName, const char * value)
	{
		int attributeIndex;
		auto typeInfo = GetAttributeInfo(object, attributeName, attributeIndex);
		if (typeInfo == nullptr) {
			OsiError("Couldn't fetch type info for " << object->Name << "." << attributeName);
			return false;
		}

		if (strcmp(typeInfo->Name.Str, "FixedString") == 0) {
			auto fs = GetOrCreateFixedString(value);
			if (fs != -1) {
				object->IndexedProperties[attributeIndex] = fs;
			} else {
				OsiError("Couldn't set " << object->Name << "." << attributeName << ": Unable to allocate pooled string");
			}
		} else if (strcmp(typeInfo->Name.Str, "AttributeFlags") == 0) {
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
			auto enumIndex = typeInfo->Values.Find(value);
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
			OsiError("Couldn't set " << object->Name << "." << attributeName << ": Inappropriate type: " << typeInfo->Name.Str);
			return false;
		}

		return true;
	}

	CRPGStats_Object * StatFindObject(char const * name)
	{
		auto stats = gStaticSymbols.GetStats();
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
		auto stats = gStaticSymbols.GetStats();
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

	void TextBuffer::Replace(std::wstring const & replacement)
	{
		if (Buf) {
			GameFree(Buf);
		}

		Buf = GameAlloc<wchar_t>(replacement.size() + 1);
		memcpy(Buf, replacement.c_str(), sizeof(wchar_t) * (replacement.size() + 1));
		Capacity = replacement.size() + 1;
		Length = replacement.size();
	}

	void eoc::Text::ReplaceParam(int index, std::wstring const & replacement)
	{
		if (index < 1 || index > std::size(Params)) {
			OsiWarnS("Param index out of bounds");
			return;
		}

		auto & param = Params[index - 1];
		if (param.PlaceholderOffset == -1 || param.PlaceholderSize == -1) {
			return;
		}

		std::wstring s;
		s.resize((std::size_t)Buf->Length);
		memcpy(s.data(), Buf->Buf, sizeof(wchar_t) * Buf->Length);

		std::wstring newS = s.substr(0, param.PlaceholderOffset);
		newS += replacement;
		newS += s.substr(param.PlaceholderOffset + param.PlaceholderSize);
		Buf->Replace(newS);

		for (auto i = 0; i < 8; i++) {
			if (i != index - 1
				&& Params[i].PlaceholderSize != -1
				&& Params[i].PlaceholderOffset > param.PlaceholderOffset) {
				Params[i].PlaceholderOffset -= param.PlaceholderSize;
			}
		}

		param.PlaceholderOffset = -1;
		param.PlaceholderSize = -1;
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

		auto componentMgr = Components[(uint32_t)type].component;
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
		if (salt != EntitySalts[index]) {
			OsiError("Salt mismatch on index " << index << "; " << salt << " != " << EntitySalts[index]);
			return nullptr;
		}

		auto entity = EntityEntries[index];
		if ((uint32_t)type >= entity->Layout.Entries.Size) {
			OsiError("Entity " << index << " has no component slot for " << (uint32_t)type);
			return nullptr;
		}

		auto const & layoutEntry = entity->Layout.Entries[(uint32_t)type];
		if (!layoutEntry.Handle.IsValid()) {
			OsiError("Entity " << index << " has no component bound to slot " << (uint32_t)type);
			return nullptr;
		}

		ObjectHandle componentHandle{ layoutEntry.Handle.Handle };
		auto componentMgr = Components[(uint32_t)type].component;
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
			auto status = Statuses[i];
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


	void HitDamageInfo::ClearDamage()
	{
		TotalDamage = 0;
		DamageList.Clear();
	}

	void HitDamageInfo::ClearDamage(osidbg::DamageType damageType)
	{
		for (auto i = 0; i < DamageList.Size; i++) {
			auto const & dmg = DamageList[i];
			if (dmg.DamageType == damageType) {
				TotalDamage -= dmg.Amount;
			}
		}

		DamageList.ClearDamage(damageType);
	}

	void HitDamageInfo::AddDamage(osidbg::DamageType damageType, int32_t amount)
	{
		TotalDamage += amount;
		DamageList.AddDamage(damageType, amount);
	}


	void DamagePairList::ClearDamage(osidbg::DamageType damageType)
	{
		for (uint32_t i = 0; i < Size; i++) {
			if (Buf[i].DamageType == damageType) {
				Remove(i);
				i--;
			}
		}
	}

	void DamagePairList::AddDamage(DamageType damageType, int32_t amount)
	{
		if (amount == 0) return;

		bool added{ false };
		for (uint32_t i = 0; i < Size; i++) {
			if (Buf[i].DamageType == damageType) {
				auto newAmount = Buf[i].Amount + amount;
				if (newAmount == 0) {
					Remove(i);
				} else {
					Buf[i].Amount = newAmount;
				}

				added = true;
				break;
			}
		}

		if (!added && amount != 0) {
			TDamagePair dmg;
			dmg.DamageType = damageType;
			dmg.Amount = amount;
			if (!SafeAdd(dmg)) {
				OsiErrorS("DamageList capacity exceeded!");
			}
		}
	}


	namespace esv
	{
		char const * const sActionStateNames[] = {
			"None",
			nullptr,
			"Attack",
			"Die",
			"Hit",
			"Idle",
			"Animation",
			"PrepareSkill",
			"UseSkill",
			"PickUp",
			"Drop",
			"MoveItem",
			"KnockedDown",
			"Summoning",
			"Polymorphing",
			"UseItem",
			"CombineItem",
			"TeleportFall",
			"Sheath",
			"Unsheath",
			"Identify",
			"Repair",
			"Lockpick",
			"DisarmTrap",
			"Incapacitated",
			"JumpFlight",
			nullptr
		};


		PropertyMapBase * ActionState::GetPropertyMap()
		{
			switch (GetType()) {
			case ActionStateType::ASPrepareSkill:
				return &gASPrepareSkillStatPropertyMap;

			default:
				return nullptr;
			}
		}

		char const * ActionState::GetTypeName()
		{
			auto type = GetType();
			if (type <= ActionStateType::ASJumpFlight) {
				return sActionStateNames[(unsigned)type];
			} else {
				return nullptr;
			}
		}
	}

	char const * TempStrings::Make(std::string const & str)
	{
		auto s = _strdup(str.c_str());
		pool_.push_back(s);
		return s;
	}

	TempStrings gTempStrings;
}