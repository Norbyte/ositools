#pragma once

#include <GameDefinitions/Base/Base.h>

namespace dse
{
	struct ModuleShortDesc
	{
		FixedString ModuleUUID;
		STDWString Name;
		Version Version;
		STDString MD5;
		STDWString Folder;
	};

	struct ModuleSettings : public ProtectedGameObject<ModuleSettings>
	{
		using ValidateProc = int(ModuleSettings& self, ModuleSettings& host, ObjectSet<ModuleShortDesc>& mismatches);

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
		Version ModVersion{ 0 };
		Version PublishVersion{ 0 };
		STDString Hash;
		STDWString Directory;
		uint64_t ScriptDataList[4]{ 0 };
		STDWString Author;
		STDWString Description;
		ObjectSet<STDWString, GameMemoryAllocator, true> Tags;
		uint8_t NumPlayers{ 4 };
		FixedString GMTemplate;
		ObjectSet<FixedString, GameMemoryAllocator, true> TargetModes;
		FixedString ModuleType;
		TranslatedString DisplayName;
		TranslatedString DisplayDescription;
	};

	struct Module
	{
		using HashProc = bool (Module* self);

		void* VMT{ nullptr };
		ModuleInfo Info;
		ObjectSet<Module, GameMemoryAllocator, true> LoadOrderedModules;
		ObjectSet<Module, GameMemoryAllocator, true> ContainedModules;
		ObjectSet<Module, GameMemoryAllocator, true> DependentModules;
		ObjectSet<Module, GameMemoryAllocator, true> AddonModules;
		bool HasValidHash{ true };
		bool UsesLsfFormat{ false };
		bool FinishedLoading{ false };
		uint64_t DataHashList[4]{ 0 };
		bool BFSReset{ false };
	};

	struct ModManager : public ProtectedGameObject<ModManager>
	{
		typedef void (*CollectAvailableMods)(ObjectSet<Module, GameMemoryAllocator, true>& mods);

		void * VMT;
		Module BaseModule;
		uint8_t Flag;
		ObjectSet<Module, GameMemoryAllocator, true> AvailableMods;
		ModuleSettings Settings;

		Module const * FindModByNameGuid(char const * nameGuid) const;
	};
}
