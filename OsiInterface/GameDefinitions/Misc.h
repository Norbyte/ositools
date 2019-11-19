#pragma once

#include "BaseTypes.h"
#include "Enumerations.h"
#include "EntitySystem.h"
#include "Module.h"

namespace osidbg
{

#pragma pack(push, 1)
	namespace eoc
	{
		struct NetworkFixedStrings
		{
			uint16_t Initialized;
			uint8_t _Pad[6];
			ObjectSet<FixedString> FixedStrSet;
			FixedStringMapBase<int> FixedStrToNetIndexMap;
			uint8_t _Pad2[4];
			uint64_t Unkn1;
			uint64_t Unkn2;
		};
	}

	namespace esv
	{

	struct ShootProjectileHelper
	{
		FixedString SkillId;
		ObjectHandle SourceCharacter;
		ObjectHandle Target;
		ObjectHandle Target2;
		glm::vec3 StartPosition;
		glm::vec3 EndPosition;
		uint8_t Random;
		uint8_t _Pad[7];
		void * DamageList;
		int32_t CasterLevel;
		uint32_t _Pad2;
		void * CollisionInfo;
		bool IsTrap;
		bool UnknownFlag1;
		uint8_t _Pad3[6];
		FixedString FS2;
		float StatusClearChance;
		bool HasCaster;
		bool IsStealthed;
		bool UnknownFlag2;
	};

	struct SummonHelperResults
	{
		ObjectHandle SummonHandle;
		uint32_t Unknown;
	};

	struct SummonHelperSummonArgs
	{
		ObjectHandle OwnerCharacterHandle;
		FixedString GameObjectTemplateFS;
		FixedString Level;
		glm::vec3 Position;
		uint32_t SummonLevel;
		float Lifetime;
		bool IsTotem;
		bool MapToAiGrid;
	};

	typedef void(*ProjectileHelpers_ShootProjectile)(void * ShootProjectileHelper, float Unknown);
	typedef void(*SummonHelpers__Summon)(SummonHelperResults * Results, SummonHelperSummonArgs * Args);

	}

	namespace ecl {

	struct GameStateMachine
	{
		uint8_t Unkn[16];
		GameState State;
	};

	struct EoCClient
	{
		void * VMT;
		void * GameEventManagerVMT;
		uint64_t field_10;
		void * NetEventManagerVMT;
		uint64_t field_20;
		void * VMT2;
		void * VMT3;
		uint64_t field_38;
		GameStateMachine ** GameStateMachine;
		uint64_t SomeNetObject;
		uint64_t field_50;
		uint64_t field_58;
		uint64_t field_60;
		FixedString FS1;
		FixedString LevelName;
		FixedString SomeGUID;
		FixedString FS_CurrentSaveGameGUID;
		bool IsLoading;
		bool IsLoading2;
		uint8_t _Pad1[6];
		void * PrimitiveSetVMT;
		PrimitiveSet<int> PrimitiveSetUnkn;
		uint64_t field_A8;
		uint16_t field_B0;
		uint8_t _Pad2[6];
		uint64_t field_B8;
		uint64_t field_C0;
		char field_C8;
		uint8_t _Pad3[7];
		uint64_t ScratchStr[4];
		ScratchBuffer ScratchBuf;
		uint8_t _Pad4[4];
		ModManager * ModManager;
		uint64_t field_118;
		STDWString WStr_CurrentHost_M;
		uint64_t SomeObject[16];
		int field_1C0;
		uint8_t _Pad5[4];
		uint64_t field_1C8[3];
		EntityWorld * EntityWorld;
		void * EntityManager;
	};

	typedef void (* EoCClient__HandleError)(void * self, STDWString *a2, bool a3, STDWString *a4);
	typedef void (* GameStateLoadModule__Do)(void * self);

	}

	struct FileReader
	{
		bool IsLoaded;
		uint8_t _Pad0[7];
		void * ScratchBufPtr;
		void * MemBuffer;
		uint64_t FileSize;
		int FileHandle;
		int FileHandle2;
		uint64_t ScratchBuffer;
		uint64_t G;
		int H;
		uint8_t _Pad1[4];
		uint64_t I;
		int FileType;
		uint8_t _Pad2[4];
		void * FileObject;
		uint64_t _Fill[16];
	};


	typedef void (* ls__FileReader__FileReader)(FileReader * self, Path * path, unsigned int type);
	typedef StringView * (* ls__Path__GetPrefixForRoot)(StringView * path, unsigned int rootType);
#pragma pack(pop)
}
