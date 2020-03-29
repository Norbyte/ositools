#pragma once

#include "BaseTypes.h"
#include "Enumerations.h"
#include "EntitySystem.h"
#include "Module.h"

namespace dse
{

#pragma pack(push, 1)
	struct GlobalSwitches
	{
		void * VMT;
		uint32_t ShowDebugView;
		bool Cleave_M;
		bool ShowCloths;
		bool field_E;
		bool ShowDrawStats;
		bool ShowDebugLines;
		bool field_11;
		bool ShowFPS;
		bool ShowPhysics;
		bool field_14;
		bool ShowRagdollInfo;
		bool field_16;
		bool ShowRaycasting;
		bool ShowPhysXBoxes;
		bool field_19;
		bool YieldOnLostFocus;
		bool field_1B;
		bool EnableAngularCulling;
		bool field_1D;
		bool field_1E;
		bool field_1F;
		bool field_20;
		bool field_21;
		bool LoadScenery;
		bool LoadTextShaders;
		float TexelDensityMin;
		float TexelDensityIdeal;
		float TexelDensityMax;
		float TexelDensityExtreme;
		float UIScaling;
		float NodeWaitTimeMultiplier;
		bool EnableGenome;
		bool field_3D;
		bool UpdateOffstageOverlayMaterials_M;
		bool UpdateInvisibilityOverlayMaterials_M;
		bool FileLoadingLog;
		bool FileSavingLog;
		uint16_t _Pad1;
		uint32_t MouseSensitivity;
		uint32_t MouseScrollSensitivity;
		uint32_t ControllerSensitivity;
		uint32_t ControllerStickDeadZone;
		uint32_t ControllerStickPressDeadZone;
		uint32_t ControllerTriggerDeadZone;
		bool ServerMonitor;
		bool StoryLog;
		bool Story;
		bool StoryEvents;
		bool DisableStoryPatching;
		bool ForceStoryPatching;
		bool StatsArgPassed;
		bool field_63;
		bool EnableSoundErrorLogging;
		bool _Pad2[3];
		STDString ChatLanguage;
		bool ShowLocalizationMarkers;
		bool field_89;
		uint16_t ForcePort;
		bool EnablePortmapping;
		char _Pad3[3];
		uint32_t GameVisibilityOnline;
		uint32_t GameVisibilityLAN;
		uint32_t GameVisibilityDirect;
		bool DisableLocalMessagePassing;
		bool EnableSteamP2P;
		uint16_t _Pad4;
		STDString DirectConnectAddress;
		uint32_t field_C0;
		bool UpdateScene;
		bool UpdatePhysXScene;
		bool DoUnlearnCheck;
		bool VisualizeTextures;
		bool field_C8;
		bool field_C9;
		bool EnableModuleHashing;
		bool SomePhysXRagdollFlag;
		bool field_CC;
		bool field_CD;
		bool field_CE;
		bool field_CF;
		bool UseLevelCache;
		bool LoadAllEffectPools;
		bool ScriptLog;
		bool ShroudEnabled;
		bool GodMode;
		bool Fading;
		bool CheckRequirements;
		bool UseEndTurnFallback;
		bool ShowCharacterCreation;
		bool ShorOriginIntroInCC;
		bool DisableArmorSavingThrows;
		bool AlwaysShowSplitterInTrade;
		bool ResetTutorialsOnNewGame;
		bool field_DD;
		bool field_DE;
		bool field_DF;
		uint32_t ControllerLayout;
		bool RotateMinimap;
		bool ShowOverheadDamage;
		bool ShowOverheadText;
		bool ShowOverheadDialog;
		// Rest of structure not properly mapped yet
	};


	namespace eoc
	{
		struct NetworkFixedStrings : public ProtectedGameObject<NetworkFixedStrings>
		{
			uint16_t Initialized;
			uint8_t _Pad[6];
			ObjectSet<FixedString> FixedStrSet;
			FixedStringMapBase<int> FixedStrToNetIndexMap;
			uint8_t _Pad2[4];
			uint64_t Unkn1;
			uint64_t Unkn2;
		};

		struct VoiceTextMetaData
		{
			float Length;
			bool IsRecorded;
			uint8_t _Pad[3];
			int Priority;
			uint8_t _Pad1[4];
			Path Source;
			int CodecID;
			uint8_t _Pad2[4];
		};

		struct SpeakerManager
		{
			void * VMT;
			FixedStringRefMap<FixedString, FixedStringRefMap<FixedString, VoiceTextMetaData>> * SpeakerMetaDataHashMap;
		};
	}

	namespace esv
	{

	struct ShootProjectileHelperHitObject
	{
		int Unknown{ 0 };
		uint8_t _Pad[4];
		ObjectHandle Target;
		glm::vec3 Position;
		uint8_t _Pad2[4];
	};

	struct ShootProjectileHelper
	{
		FixedString SkillId;
		ObjectHandle Caster;
		ObjectHandle Source;
		ObjectHandle Target;
		glm::vec3 StartPosition;
		glm::vec3 EndPosition;
		uint8_t Random;
		uint8_t _Pad[7];
		DamagePairList * DamageList;
		int32_t CasterLevel;
		uint32_t _Pad2;
		ShootProjectileHelperHitObject * HitObject;
		bool IsTrap;
		bool UnknownFlag1;
		uint8_t _Pad3[6];
		FixedString CleanseStatuses;
		float StatusClearChance;
		bool IsFromItem;
		bool IsStealthed;
		bool IgnoreObjects;
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
		int32_t SummonLevel;
#if defined(OSI_EOCAPP)
		int32_t SummoningAbilityLevel;
#endif
		float Lifetime;
		bool IsTotem;
		bool MapToAiGrid;
	};

	typedef void * (*ProjectileHelpers_ShootProjectile)(void * ShootProjectileHelper);
	typedef void(*SummonHelpers__Summon)(SummonHelperResults * Results, SummonHelperSummonArgs * Args);
	typedef void(*GameStateEventManager__ExecuteGameStateChangedEvent)(void * self, ServerGameState fromState, ServerGameState toState);
	typedef void(*GameStateThreaded__GameStateWorker__DoWork)(void * self);

	}

	namespace ecl {

	struct GameStateMachine : public ProtectedGameObject<GameStateMachine>
	{
		uint8_t Unknown;
		uint8_t _Pad1[7];
		void * CurrentState;
		ClientGameState State;
	};

	struct EoCClient : public ProtectedGameObject<EoCClient>
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
		net::Client * GameClient;
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
		void * Random;
		void * ItemCombinationManager;
		char field_C8;
		uint8_t _Pad3[7];
		uint64_t ScratchStr[4];
		ScratchBuffer ScratchBuf;
		uint8_t _Pad4[4];
		ModManager * ModManager;
		void * ChatManager;
		STDWString WStr_CurrentHost_M;
		uint64_t SomeObject[16];
		int field_1C0;
		uint8_t _Pad5[4];
		uint64_t field_1C8[3];
		EntityWorld * EntityWorld;
		void * EntityManager;
	};

	typedef void (* EoCClient__HandleError)(void * self, STDWString const * message, bool exitGame, STDWString const *a4);
	typedef void (*GameStateThreaded__GameStateWorker__DoWork)(void * self);
	typedef void (* GameStateEventManager__ExecuteGameStateChangedEvent)(void * self, ClientGameState fromState, ClientGameState toState);

	}

	struct FileReader : public Noncopyable<FileReader>
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

	class FileReaderPin
	{
	public:
		inline FileReaderPin(FileReader * reader)
			: reader_(reader)
		{}

		~FileReaderPin();

		FileReaderPin(FileReaderPin const &) = delete;
		FileReaderPin & operator =(FileReaderPin const &) = delete;
		FileReaderPin & operator =(FileReaderPin &&) = delete;

		inline FileReaderPin(FileReaderPin && other)
		{
			if (&other != this) {
				reader_ = other.reader_;
				other.reader_ = nullptr;
			}
		}

		bool IsLoaded() const
		{
			return reader_ != nullptr && reader_->IsLoaded;
		}

		void * Buf() const
		{
			if (IsLoaded()) {
				return reader_->ScratchBufPtr;
			} else {
				return nullptr;
			}
		}

		std::size_t Size() const
		{
			if (IsLoaded()) {
				return reader_->FileSize;
			} else {
				return 0;
			}
		}

		std::string ToString() const;

	private:
		FileReader * reader_;
	};


#if defined(OSI_EOCAPP)
	typedef char const * (*ls__FixedString__Create)(char const * str, int length);
#else
	typedef void(*ls__FixedString__Create)(FixedString * self, char const * str, int length);
#endif
	typedef FileReader * (* ls__FileReader__FileReader)(FileReader * self, Path * path, unsigned int type);
	typedef void (* ls__FileReader__Dtor)(FileReader * self);
	typedef StringView * (* ls__Path__GetPrefixForRoot)(StringView * path, unsigned int rootType);
#pragma pack(pop)

	class TempStrings
	{
	public:
		char const * Make(STDString const & str);
		char const * Make(std::string const & str);

	private:
		std::vector<char *> pool_;
	};

	extern TempStrings gTempStrings;
}
