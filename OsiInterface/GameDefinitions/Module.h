#pragma once

#include "BaseTypes.h"

namespace dse
{

#pragma pack(push, 1)
	struct ModuleShortDesc : public ProtectedGameObject<ModuleShortDesc>
	{
		FixedString ModuleUUID;
		STDWString Name;
		uint32_t Version;
		uint8_t _Pad[4];
		STDString MD5;
		STDWString Folder;
	};

	struct ModuleSettings : public ProtectedGameObject<ModuleSettings>
	{
		void *VMT;
		ObjectSet<ModuleShortDesc> Mods;
		ObjectSet<FixedString> ModOrder;
	};

	struct ModuleInfo
	{
		FixedString ModuleUUID;
		STDWString Name;
		FixedString StartLevel;
		FixedString MenuLevel;
		FixedString LobbyLevel;
		FixedString CharacterCreationLevel;
		FixedString PhotoBoothLevel;
		uint32_t Version{ 0 };
		uint32_t PublishVersion{ 0 };
		STDString Hash;
		STDWString Directory;
		uint64_t ScriptDataList[4]{ 0 };
		STDWString Author;
		STDWString Description;
		ObjectSet<STDWString, GameMemoryAllocator, true> Tags;
		uint8_t NumPlayers{ 4 };
		uint8_t _Pad1[7]{ 0 };
		FixedString GMTemplate;
		ObjectSet<FixedString, GameMemoryAllocator, true> TargetModes;
		FixedString ModuleType;
		TranslatedString DisplayName;
		TranslatedString DisplayDescription;
	};

	struct Module
	{
		void* VMT{ nullptr };
		ModuleInfo Info;
		ObjectSet<Module, GameMemoryAllocator, true> LoadOrderedModules;
		ObjectSet<Module, GameMemoryAllocator, true> ContainedModules;
		ObjectSet<Module, GameMemoryAllocator, true> DependentModules;
		ObjectSet<Module, GameMemoryAllocator, true> AddonModules;
		bool HasValidHash{ true };
		bool UsesLsfFormat{ false };
		bool FinishedLoading{ false };
		uint8_t _Pad1[5]{ 0 };
		uint64_t DataHashList[4]{ 0 };
		bool BFSReset_M{ false };
		uint8_t _Pad2[7]{ 0 };
	};

	struct ModManager : public ProtectedGameObject<ModManager>
	{
		typedef void (*CollectAvailableMods)(ObjectSet<Module, GameMemoryAllocator, true>& mods);

		void * VMT;
		Module BaseModule;
		uint8_t Flag;
		uint8_t _Pad1[7];
		ObjectSet<Module, GameMemoryAllocator, true> AvailableMods;
		ModuleSettings Settings;

		Module const * FindModByNameGuid(char const * nameGuid) const;
	};
#pragma pack(pop)
}
