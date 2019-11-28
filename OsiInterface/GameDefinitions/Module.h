#pragma once

#include "BaseTypes.h"

namespace osidbg
{

#pragma pack(push, 1)
	struct ModuleShortDesc
	{
		FixedString FS1;
		STDWString WStr1;
		int field_28;
		int field_2C;
		STDString Str1;
		STDWString WStr2;
	};

	struct ModuleSettings
	{
		void *VMT;
		ObjectSet<ModuleShortDesc> ObjSet_ModuleShortDesc;
		ObjectSet<FixedString> ObjSet_FixedString;
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

	struct ModuleUnknownInner
	{
		void * VMT;
		void * Unknown;
		STDString Str1;
		STDWString Str2;
	};

	struct ModuleUnknown
	{
		void * VMT;
		ModuleUnknownInner Inner1;
		ModuleUnknownInner Inner2;
	};

	struct Module
	{
		void * VMT;
		ModuleInfo Info;
		ModuleUnknown Unknown1;
		ModuleUnknown Unknown2;
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

	struct ModManager
	{
		void * VMT;
		Module BaseModule;
		uint8_t Flag;
		uint8_t _Pad1[7];
		ObjectSet<Module> Modules;
		ModuleSettings Settings;

		Module const * FindModByNameGuid(std::string const & nameGuid) const;
	};
#pragma pack(pop)
}
