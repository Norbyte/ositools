#include "stdafx.h"

#include <GameDefinitions/BaseTypes.h>
#include <GameDefinitions/Symbols.h>
#include <GameDefinitions/Enumerations.h>
#include "OsirisProxy.h"
#include <PropertyMaps.h>

namespace dse
{
	StaticSymbols & GetStaticSymbols()
	{
		static auto sSymbols = new StaticSymbols();
		return *sSymbols;
	}

	void* MSVCMemoryAllocator::Alloc(std::size_t size)
	{
		return GetStaticSymbols().CrtAlloc(size);
	}

	void * GameAllocRaw(std::size_t size)
	{
#if defined(OSI_EOCAPP)
		return GetStaticSymbols().EoCAlloc(nullptr, size);
#else
		return GetStaticSymbols().EoCAlloc(nullptr, size, "", 1, "");
#endif
	}

	void GameFree(void * ptr)
	{
		GetStaticSymbols().EoCFree(nullptr, ptr);
	}

	unsigned int Primes[290] = {
		2, 3, 5, 7, 0x0B, 0x0D, 0x11, 0x13, 0x17, 0x1D, 0x1F, 0x25,
		0x29, 0x2B, 0x2F, 0x35, 0x3B, 0x3D, 0x43, 0x47, 0x49, 0x4F, 0x53,
		0x59, 0x61, 0x65, 0x71, 0x7F, 0x83, 0x95, 0x97, 0x0A3, 0x0AD,
		0x0B5, 0x0BF, 0x0D3, 0x0DF, 0x0E9, 0x0F1, 0x0FB, 0x107, 0x10F,
		0x119, 0x125, 0x133, 0x137, 0x14B, 0x15B, 0x161, 0x16F, 0x175,
		0x17F, 0x18D, 0x191, 0x1A3, 0x1A5, 0x1AF, 0x1BB, 0x1C9, 0x1CD,
		0x1DF, 0x1E7, 0x1EB, 0x1F7, 0x209, 0x21D, 0x22D, 0x233, 0x23B,
		0x24B, 0x251, 0x259, 0x265, 0x277, 0x281, 0x28D, 0x295, 0x2A1,
		0x2AB, 0x2B3, 0x2BD, 0x2CF, 0x2D7, 0x2DD, 0x2E7, 0x2EF, 0x2F9,
		0x305, 0x313, 0x31D, 0x329, 0x32B, 0x335, 0x347, 0x355, 0x35F,
		0x36D, 0x371, 0x38B, 0x38F, 0x3A1, 0x3A9, 0x3AD, 0x3B9, 0x3C7,
		0x3CB, 0x3D7, 0x3DF, 0x3F1, 0x41B, 0x44F, 0x47F, 0x4B1, 0x4EB,
		0x515, 0x5AB, 0x641, 0x679, 0x6AD, 0x6D9, 0x709, 0x79F, 0x7D3,
		0x805, 0x89B, 0x8CB, 0x905, 0x92F, 0x9C7, 0x9F7, 0x0A31, 0x0A61,
		0x0A93, 0x0AC1, 0x0AF1, 0x0B23, 0x0B57, 0x0B89, 0x0BB9, 0x0CB3,
		0x0CE5, 0x0D1F, 0x0D4F, 0x0D81, 0x0E17, 0x0E4B, 0x0E75, 0x0F0B,
		0x0F43, 0x0FD3, 0x1069, 0x109D, 0x1139, 0x1163, 0x119B, 0x122B,
		0x125F, 0x128F, 0x12C1, 0x1357, 0x138B, 0x13BB, 0x13ED, 0x1421,
		0x1459, 0x14E7, 0x151F, 0x15B5, 0x1645, 0x16DB, 0x170F, 0x1741,
		0x1777, 0x17A5, 0x17D5, 0x1807, 0x183B, 0x1871, 0x189D, 0x18D1,
		0x19CF, 0x19FD, 0x1A2D, 0x1AC9, 0x1AFB, 0x1B2F, 0x1B59, 0x1B91,
		0x1BBF, 0x1BEF, 0x1C27, 0x1C55, 0x1C8B, 0x1CB7, 0x1D53, 0x1D87,
		0x1DB3, 0x1E49, 0x1EDD, 0x1F0F, 0x1F49, 0x1F75, 0x1FA5, 0x20A1,
		0x2135, 0x2231, 0x2263, 0x22F7, 0x2329, 0x2363, 0x238F, 0x23BF,
		0x23F3, 0x2429, 0x24BB, 0x254F, 0x2581, 0x267B, 0x26AD, 0x2717,
		0x2905, 0x2AFB, 0x2CEF, 0x2EE7, 0x30D7, 0x32C9, 0x34C9, 0x36B9,
		0x38A7, 0x3AA5, 0x3C97, 0x3E81, 0x4087, 0x4273, 0x4465, 0x465D,
		0x4847, 0x4A39, 0x4C2D, 0x4E2B, 0x501B, 0x5209, 0x53FF, 0x55F3,
		0x57E5, 0x59DB, 0x5BD5, 0x5DC1, 0x5FBD, 0x61B5, 0x63B3, 0x6593,
		0x6785, 0x6983, 0x6B75, 0x6D61, 0x6F61, 0x7151, 0x733D, 0x753B,
		0x772D, 0x7925, 0x7B17, 0x7D03, 0x7EF7, 0x80F5, 0x82DF, 0x84E3,
		0x86C5, 0x88CF, 0x8AB3, 0x8CA7, 0x8EAB, 0x908B, 0x927D, 0x947B,
		0x9665, 0x986B, 0x9A4F, 0x9C49, 0x9E3B, 0x0A033, 0x0A223, 0x0A41D,
		0x0A60D, 0x0A7FB, 0x0A9FD, 0x0ABF1, 0x0ADD5, 0x0AFCF
	};

	unsigned int GetNearestLowerPrime(unsigned int num)
	{
		if (num < 2) return 2;
		if (num >= 45007) return 45007;

		unsigned start = 0;
		unsigned end = (unsigned)std::size(Primes);

		while (start < end) {
			auto mid = (start + end) >> 1;
			auto delta = (int64_t)num - Primes[mid];
			if (delta >= 0) {
				if (delta == 0) {
					return Primes[mid];
				}

				start = mid + 1;
			} else {
				end = (start + end) >> 1;
			}
		}

		return Primes[start];
	}

	void StaticSymbols::CanonicalizePath(STDString & path) const
	{
		if (path.find('\\') != std::string::npos) {
			WARN("Path contains backslashes: \"%s\"; canonical paths should only contain forward slashes.", path.c_str());
		}

		std::replace(path.begin(), path.end(), '\\', '/');
	}

#if defined(OSI_EOCAPP)
	STDString StaticSymbols::ToPath(StringView path, PathRootType root, bool canonicalize) const
	{
		if (PathRoots == nullptr) {
			ERR("LibraryManager::ToPath(): Path root API not available!");
			return "";
		}

		STDString canonicalPath(path);
		if (canonicalize) {
			CanonicalizePath(canonicalPath);
		}

		auto rootPath = PathRoots[(unsigned)root];

		STDString absolutePath(*rootPath);
		absolutePath += "/" + canonicalPath;
		return absolutePath;
	}

	FileReaderPin StaticSymbols::MakeFileReader(StringView path, PathRootType root, bool canonicalize) const
	{
		if (PathRoots == nullptr || FileReaderCtor == nullptr) {
			ERR("LibraryManager::MakeFileReader(): File reader API not available!");
			return FileReaderPin(nullptr);
		}

		auto absolutePath = ToPath(path, root, canonicalize);

		Path lsPath;
		lsPath.Name = absolutePath;

		auto reader = new FileReader();
		FileReaderCtor(reader, &lsPath, 2);
		return FileReaderPin(reader);
	}
#else
	STDString StaticSymbols::ToPath(StringView path, PathRootType root, bool canonicalize) const
	{
		if (GetPrefixForRoot == nullptr) {
			ERR("LibraryManager::ToPath(): Path root API not available!");
			return "";
		}

		STDString canonicalPath(path);
		if (canonicalize) {
			CanonicalizePath(canonicalPath);
		}

		StringView rootPath;
		GetPrefixForRoot(&rootPath, (unsigned)root);

		STDString absolutePath(rootPath);
		absolutePath += "/";
		absolutePath += canonicalPath;
		return absolutePath;
	}

	FileReaderPin StaticSymbols::MakeFileReader(StringView path, PathRootType root, bool canonicalize) const
	{
		if (GetPrefixForRoot == nullptr || FileReaderCtor == nullptr) {
			ERR("LibraryManager::MakeFileReader(): File reader API not available!");
			return FileReaderPin(nullptr);
		}

		Path lsPath;
		lsPath.Name = ToPath(path, root, canonicalize);

		auto reader = new FileReader();
		FileReaderCtor(reader, &lsPath, 2);
		return FileReaderPin(reader);
	}
#endif

	bool StaticSymbols::FileExists(StringView path, PathRootType root, bool canonicalize) const
	{
		// TODO - implement using proper FS file exists call
		auto reader = MakeFileReader(path, root, canonicalize);
		return reader.IsLoaded();
	}

	void StaticSymbols::DestroyFileReader(FileReader * reader)
	{
		if (FileReaderDtor != nullptr) {
			FileReaderDtor(reader);
		}
	}


	FileReaderPin::~FileReaderPin()
	{
		if (reader_ != nullptr) {
			GetStaticSymbols().DestroyFileReader(reader_);
		}
	}

	STDString FileReaderPin::ToString() const
	{
		if (!IsLoaded()) {
			OsiErrorS("File not loaded!");
			return "";
		}

		STDString contents;
		contents.resize(Size());
		memcpy(contents.data(), Buf(), Size());
		return contents;
	}


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

	void TextBuffer::Replace(WStringView replacement)
	{
		if (Buf) {
			GameFree(Buf);
		}

		Buf = GameAllocArray<wchar_t>(replacement.size() + 1);
		memcpy(Buf, replacement.data(), sizeof(wchar_t) * (replacement.size() + 1));
		Capacity = replacement.size() + 1;
		Length = replacement.size();
	}

	void eoc::Text::ReplaceParam(int index, WStringView replacement)
	{
		if (index < 1 || index > std::size(Params)) {
			OsiWarnS("Param index out of bounds");
			return;
		}

		auto & param = Params[index - 1];
		if (param.PlaceholderOffset == -1 || param.PlaceholderSize == -1) {
			return;
		}

		STDWString s;
		s.resize((std::size_t)Buf->Length);
		memcpy(s.data(), Buf->Buf, sizeof(wchar_t) * Buf->Length);

		STDWString newS = s.substr(0, param.PlaceholderOffset);
		newS += replacement;
		newS += s.substr(param.PlaceholderOffset + param.PlaceholderSize);
		Buf->Replace(newS);

		for (auto i = 0; i < 8; i++) {
			if (i != index - 1
				&& Params[i].PlaceholderSize != -1
				&& Params[i].PlaceholderOffset > param.PlaceholderOffset) {
				Params[i].PlaceholderOffset = Params[i].PlaceholderOffset - param.PlaceholderSize + (uint32_t)replacement.size();
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
		return murmur3_32((const uint8_t *)s, length, 0) % 0xFFF1;
	}

	Module const * ModManager::FindModByNameGuid(char const * nameGuid) const
	{
		auto modUuidFS = NameGuidToFixedString(nameGuid);
		if (!modUuidFS) {
			return nullptr;
		}

		for (auto const& mod : BaseModule.LoadOrderedModules) {
			if (mod.Info.ModuleUUID == modUuidFS) {
				return &mod;
			}
		}

		return nullptr;
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

	esv::Status * esv::StatusMachine::GetStatus(ObjectHandle handle) const
	{
		for (auto status : Statuses) {
			if (status->StatusHandle == handle) {
				return status;
			}
		}

		return nullptr;
	}

	esv::Status* esv::StatusMachine::GetStatus(NetId netId) const
	{
		return FindByNetId(netId);
	}

	esv::Status* esv::StatusMachine::GetStatus(FixedString const& statusId) const
	{
		for (auto status : Statuses) {
			if (status->StatusId == statusId) {
				return status;
			}
		}

		return nullptr;
	}

	ecl::Status* ecl::StatusMachine::GetStatus(StatusType type) const
	{
		for (auto status : Statuses) {
			if (status->GetStatusId() == type) {
				return status;
			}
		}

		return nullptr;
	}

	ecl::Status* ecl::StatusMachine::GetStatus(FixedString const& statusId) const
	{
		for (auto status : Statuses) {
			if (status->StatusId == statusId) {
				return status;
			}
		}

		return nullptr;
	}

	ecl::Status* ecl::StatusMachine::GetStatus(NetId netId) const
	{
		return FindByNetId(netId);
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

	esv::Status * esv::Character::GetStatus(ObjectHandle statusHandle, bool returnPending) const
	{
		if (StatusMachine == nullptr) {
			return nullptr;
		}

		auto status = StatusMachine->GetStatus(statusHandle);
		if (status != nullptr) {
			return status;
		}

		if (returnPending) {
			ObjectHandle ownerHandle;
			this->GetObjectHandle(ownerHandle);

			auto pendingStatus = ExtensionState::Get().PendingStatuses.Find(ownerHandle, statusHandle);
			if (pendingStatus != nullptr) {
				return pendingStatus->Status;
			}
		}

		return nullptr;
	}

	esv::Status* esv::Character::GetStatus(NetId netId) const
	{
		if (StatusMachine == nullptr) {
			return nullptr;
		}

		// Pending statuses have no NetID, so we can't check them here
		return StatusMachine->GetStatus(netId);
	}

	esv::Status * esv::Item::GetStatus(ObjectHandle statusHandle, bool returnPending) const
	{
		if (StatusMachine == nullptr) {
			return nullptr;
		}

		auto status = StatusMachine->GetStatus(statusHandle);
		if (status != nullptr) {
			return status;
		}

		if (returnPending) {
			ObjectHandle ownerHandle;
			this->GetObjectHandle(ownerHandle);

			auto pendingStatus = ExtensionState::Get().PendingStatuses.Find(ownerHandle, statusHandle);
			if (pendingStatus != nullptr) {
				return pendingStatus->Status;
			}
		}

		return nullptr;
	}

	esv::Status* esv::Item::GetStatus(NetId netId) const
	{
		if (StatusMachine == nullptr) {
			return nullptr;
		}

		return StatusMachine->GetStatus(netId);
	}


	ecl::Status* ecl::Character::GetStatus(NetId netId) const
	{
		if (StatusMachine == nullptr) {
			return nullptr;
		}

		return StatusMachine->GetStatus(netId);
	}

	void HitDamageInfo::ClearDamage()
	{
		TotalDamage = 0;
		DamageList.Clear();
	}

	void HitDamageInfo::ClearDamage(dse::DamageType damageType)
	{
		for (auto const& dmg : DamageList) {
			if (dmg.DamageType == damageType) {
				TotalDamage -= dmg.Amount;
			}
		}

		DamageList.ClearDamage(damageType);
	}

	void HitDamageInfo::AddDamage(dse::DamageType damageType, int32_t amount)
	{
		TotalDamage += amount;
		DamageList.AddDamage(damageType, amount);
	}


	void DamagePairList::ClearDamage(dse::DamageType damageType)
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

	char const * TempStrings::Make(STDString const & str)
	{
		auto s = _strdup(str.c_str());
		pool_.push_back(s);
		return s;
	}

	char const * TempStrings::Make(std::string const & str)
	{
		auto s = _strdup(str.c_str());
		pool_.push_back(s);
		return s;
	}

	TempStrings gTempStrings;



	void UIObject::OnFunctionCalled(const char * a1, unsigned int a2, InvokeDataValue * a3)
	{
		return GetStaticSymbols().EoCUI__vftable->OnFunctionCalled(this, a1, a2, a3);
	}

	void UIObject::OnCustomDrawCallback(void * a1)
	{
		return GetStaticSymbols().EoCUI__vftable->OnCustomDrawCallback(this, a1);
	}

	void UIObject::Destroy(bool a1)
	{
		return GetStaticSymbols().EoCUI__vftable->Destroy(this, a1);
	}

	void UIObject::SetHandle(ObjectHandle * a1)
	{
		return GetStaticSymbols().EoCUI__vftable->SetHandle(this, a1);
	}

	ObjectHandle * UIObject::GetHandle(ObjectHandle * a1)
	{
		return GetStaticSymbols().EoCUI__vftable->GetHandle(this, a1);
	}

	void UIObject::RequestDelete()
	{
		return GetStaticSymbols().EoCUI__vftable->RequestDelete(this);
	}

	void UIObject::SetOwnerPlayerId(uint64_t a1)
	{
		return GetStaticSymbols().EoCUI__vftable->SetOwnerPlayerId(this, a1);
	}

	void UIObject::SetPos(int * a1)
	{
		return GetStaticSymbols().EoCUI__vftable->SetPos(this, a1);
	}

	void UIObject::KeepWithin(int a1, int a2)
	{
		return GetStaticSymbols().EoCUI__vftable->KeepWithin(this, a1, a2);
	}

	void UIObject::Show()
	{
		return GetStaticSymbols().EoCUI__vftable->Show(this);
	}

	void UIObject::Hide()
	{
		return GetStaticSymbols().EoCUI__vftable->Hide(this);
	}

	const char * UIObject::GetDebugName()
	{
		return GetStaticSymbols().EoCUI__vftable->GetDebugName(this);
	}

	bool UIObject::IsControllerUI()
	{
		return GetStaticSymbols().EoCUI__vftable->IsControllerUI(this);
	}

	void UIObject::Init()
	{
		return GetStaticSymbols().EoCUI__vftable->Init(this);
	}

	void UIObject::InitAPI()
	{
		return GetStaticSymbols().EoCUI__vftable->InitAPI(this);
	}

	void UIObject::Update(float a1)
	{
		return GetStaticSymbols().EoCUI__vftable->Update(this, a1);
	}

	void UIObject::PostUpdate(float a1)
	{
		return GetStaticSymbols().EoCUI__vftable->PostUpdate(this, a1);
	}

	void UIObject::Render(void * a1, void * a2)
	{
		return GetStaticSymbols().EoCUI__vftable->Render(this, a1, a2);
	}

	void UIObject::RegisterInvokeNames()
	{
		return GetStaticSymbols().EoCUI__vftable->RegisterInvokeNames(this);
	}

	void UIObject::Resize()
	{
		return GetStaticSymbols().EoCUI__vftable->Resize(this);
	}

	void * UIObject::OnInputEvent(void * a1, void * a2)
	{
		return GetStaticSymbols().EoCUI__vftable->OnInputEvent(this, a1, a2);
	}

	uint8_t * UIObject::SendEventToFlash(uint8_t * a2, void *a3, unsigned int a4)
	{
		return GetStaticSymbols().EoCUI__vftable->SendEventToFlash(this, a2, a3, a4);
	}

	void * UIObject::OnInputEventText(void * a1, void * a2)
	{
		return GetStaticSymbols().EoCUI__vftable->OnInputEventText(this, a1, a2);
	}

	uint16_t * UIObject::OnUnlinkedInput(uint16_t * a1, uint32_t a2, uint16_t a3)
	{
		return GetStaticSymbols().EoCUI__vftable->OnUnlinkedInput(this, a1, a2, a3);
	}

	void UIObject::SetModifierStates(bool a1, bool a2, bool a3, bool a4)
	{
		return GetStaticSymbols().EoCUI__vftable->SetModifierStates(this, a1, a2, a3, a4);
	}

	bool UIObject::OnAPIPreResetDevice(void * a1)
	{
		return GetStaticSymbols().EoCUI__vftable->OnAPIPreResetDevice(this, a1);
	}

	bool UIObject::OnAPIPostResetDevice(void * a1)
	{
		return GetStaticSymbols().EoCUI__vftable->OnAPIPostResetDevice(this, a1);
	}

	void UIObject::OnControllerModeChanged()
	{
		return GetStaticSymbols().EoCUI__vftable->OnControllerModeChanged(this);
	}

	void UIObject::OnPlayerDisconnect(int a1)
	{
		return GetStaticSymbols().EoCUI__vftable->OnPlayerDisconnect(this, a1);
	}

	void UIObject::ReleaseRenderData()
	{
		return GetStaticSymbols().EoCUI__vftable->ReleaseRenderData(this);
	}

	void UIObject::PrepareRenderData()
	{
		return GetStaticSymbols().EoCUI__vftable->PrepareRenderData(this);
	}

	void UIObject::DoPrepareRenderData()
	{
		return GetStaticSymbols().EoCUI__vftable->DoPrepareRenderData(this);
	}

	void UIObject::Activate()
	{
		return GetStaticSymbols().EoCUI__vftable->Activate(this);
	}

	void UIObject::Deactivate()
	{
		return GetStaticSymbols().EoCUI__vftable->Deactivate(this);
	}

	void UIObject::LoseFocus()
	{
		return GetStaticSymbols().EoCUI__vftable->LoseFocus(this);
	}

	int64_t UIObject::GetBitmapHeight()
	{
		return GetStaticSymbols().EoCUI__vftable->GetBitmapHeight(this);
	}

	int64_t UIObject::GetBitmapWidth()
	{
		return GetStaticSymbols().EoCUI__vftable->GetBitmapWidth(this);
	}

	void * UIObject::GetCharacter()
	{
		return GetStaticSymbols().EoCUI__vftable->GetCharacter(this);
	}

	bool UIObject::SetPlayerHandle(ObjectHandle * handle)
	{
		return GetStaticSymbols().EoCUI__vftable->SetPlayerHandle(this, handle);
	}

	ObjectHandle * UIObject::GetPlayerHandle(ObjectHandle * handle)
	{
		return GetStaticSymbols().EoCUI__vftable->GetPlayerHandle(this, handle);
	}

	bool UIObject::Unknown1()
	{
		return GetStaticSymbols().EoCUI__vftable->Unknown1(this);
	}

	void UIObject::Unknown2()
	{
		return GetStaticSymbols().EoCUI__vftable->Unknown2(this);
	}

	void * UIObject::Unknown3()
	{
		return GetStaticSymbols().EoCUI__vftable->Unknown3(this);
	}

	void UIObject::Unknown4(void * a1)
	{
		return GetStaticSymbols().EoCUI__vftable->Unknown4(this, a1);
	}

	namespace ecl
	{
		EoCUI::EoCUI(dse::Path * path)
		{
			GetStaticSymbols().EoCUI__ctor(this, path);
		}
	}
}