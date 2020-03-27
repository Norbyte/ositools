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

	struct ModuleInfo : public ProtectedGameObject<ModuleInfo>
	{
		FixedString ModuleUUID;
		STDWString Name;
		FixedString StartLevel;
		FixedString MenuLevel;
		FixedString LobbyLevel;
		FixedString CharacterCreationLevel;
		FixedString PhotoBoothLevel;
		uint32_t Version;
		uint32_t PublishVersion;
		STDString Hash;
		STDWString Directory;
		uint64_t ScriptDataList[4];
		STDWString Author;
		STDWString Description;
		ObjectSet<STDWString> ObjSet_STDWString;
		uint8_t field_118;
		uint8_t _Pad1[7];
		FixedString FS6;
		ObjectSet<FixedString> TargetModes;
		FixedString ModuleType;
	};

	struct Module : public ProtectedGameObject<Module>
	{
		void * VMT;
		ModuleInfo Info;
		TranslatedString Unknown1;
		TranslatedString Unknown2;
		ObjectSet<Module> LoadOrderedModules;
		ObjectSet<Module> ContainedModules;
		ObjectSet<Module> DependentModules;
		ObjectSet<Module> AddonModules;
		bool HasValidHash;
		bool UsesLsfFormat;
		bool FinishedLoading;
		uint8_t _Pad1[5];
		uint64_t DataHashList[4];
		bool BFSReset_M;
		uint8_t _Pad2[7];
	};

	struct ModManager : public ProtectedGameObject<ModManager>
	{
		void * VMT;
		Module BaseModule;
		uint8_t Flag;
		uint8_t _Pad1[7];
		ObjectSet<Module> Modules;
		ModuleSettings Settings;

		Module const * FindModByNameGuid(char const * nameGuid) const;
	};
#pragma pack(pop)
}
