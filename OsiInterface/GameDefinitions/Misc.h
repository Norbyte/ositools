#pragma once

#include "BaseTypes.h"
#include "Enumerations.h"
#include "EntitySystem.h"
#include "Module.h"
#include <glm/gtc/quaternion.hpp>

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


	struct TranslatedStringRepository
	{
		typedef TranslatedStringRepository* (*GetInstance)();
		typedef STDWString * (*Get)(TranslatedStringRepository* self, RuntimeStringHandle* handle, uint64_t * length, char gender1, char gender2, bool unknown);
	};


	struct TranslatedStringKeyManager
	{
		typedef TranslatedStringKeyManager* (*GetInstance)();
		typedef TranslatedString * (*GetTranlatedStringFromKey)(TranslatedStringKeyManager* self, TranslatedString& string, FixedString const& handle, bool unknown);
	};


	namespace eoc
	{
		struct PathMover
		{
			glm::quat PathRotateOrig;
			glm::quat PathRotateTarget;
			glm::vec3 StartingPosition;
			glm::vec3 DestinationPos;
			float PathInterpolateValue;
			float PathSpeedSet;
			float PathSpeed;
			float PathAcceleration;
			uint8_t PathType;
			uint8_t _Pad1[3];
			int PathRotationType;
			float PathRadius;
			float PathShift;
			float PathMinArcDist;
			float PathMaxArcDist;
			uint64_t PathRepeat;
			uint8_t PathRandom;
			uint8_t _Pad2[7];
		};


		struct NetworkFixedStrings : public ProtectedGameObject<NetworkFixedStrings>
		{
			uint16_t Initialized;
			uint8_t _Pad[6];
			ObjectSet<FixedString, GameMemoryAllocator, true> FixedStrSet;
			Map<FixedString, int> FixedStrToNetIndexMap;
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
			RefMap<FixedString, RefMap<FixedString, VoiceTextMetaData>> * SpeakerMetaDataHashMap;
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
		glm::vec3 StartPosition{ .0f };
		glm::vec3 EndPosition{ .0f };
		uint8_t Random{ 0 };
		uint8_t _Pad[7];
		DamagePairList * DamageList;
		int32_t CasterLevel{ -1 };
		uint32_t _Pad2;
		ShootProjectileHelperHitObject * HitObject;
		bool IsTrap{ false };
		bool UnknownFlag1{ false };
		uint8_t _Pad3[6];
		FixedString CleanseStatuses;
		float StatusClearChance{ 0.0f };
		bool IsFromItem{ false };
		bool IsStealthed{ false };
		bool IgnoreObjects{ false };
	};

	struct SummonHelperResults
	{
		ObjectHandle SummonHandle;
		uint32_t Unknown{ 0 };
	};

	struct SummonHelperSummonArgs
	{
		ObjectHandle OwnerCharacterHandle;
		FixedString GameObjectTemplateFS;
		FixedString Level;
		glm::vec3 Position{ .0f };
		int32_t SummonLevel{ -1 };
#if defined(OSI_EOCAPP)
		int32_t SummoningAbilityLevel;
#endif
		float Lifetime{ 0.0f };
		bool IsTotem{ false };
		bool MapToAiGrid{ true };
	};

	typedef void * (*ProjectileHelpers_ShootProjectile)(void * ShootProjectileHelper);
	typedef void(*SummonHelpers__Summon)(SummonHelperResults * Results, SummonHelperSummonArgs * Args);
	typedef void(*GameStateEventManager__ExecuteGameStateChangedEvent)(void * self, GameState fromState, GameState toState);
	typedef void(*GameStateThreaded__GameStateWorker__DoWork)(void * self);

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

		inline FileReaderPin(FileReaderPin && other) noexcept
		{
			reader_ = other.reader_;
			if (&other != this) {
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

		STDString ToString() const;

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
