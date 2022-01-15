#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/EntitySystem.h>
#include <GameDefinitions/GameObjects/Module.h>
#include <glm/gtc/quaternion.hpp>

namespace dse
{
	struct GlobalSwitches
	{
		struct SomeOption
		{
			float A, B, C, D;
		};

		struct SoundSetting
		{
			float Value;
			float Value2;
			float Value3;
			FixedString ConfigKey;
			const char* PreviewKeyName;
			const char* RTPCKeyName;
			bool field_28;
			bool field_29;
			TranslatedString Name;
			void* UpdateProc;
		};

		void* VMT;
		int32_t DebugViewType;
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
		int MouseSensitivity;
		int MouseScrollSensitivity;
		int ControllerSensitivity;
		int ControllerStickDeadZone;
		int ControllerStickPressDeadZone;
		int ControllerTriggerDeadZone;
		bool ServerMonitor;
		bool StoryLog;
		bool Story;
		bool StoryEvents;
		bool DisableStoryPatching;
		bool ForceStoryPatching;
		bool StatsArgPassed;
		bool field_63;
		bool EnableSoundErrorLogging;
		STDString ChatLanguage;
		bool ShowLocalizationMarkers;
		bool field_89;
		uint16_t ForcePort;
		bool EnablePortmapping;
		int GameVisibilityOnline;
		int GameVisibilityLAN;
		int GameVisibilityDirect;
		bool DisableLocalMessagePassing;
		bool EnableSteamP2P;
		STDString DirectConnectAddress;
		int field_C0;
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
		bool field_E8;
		bool field_E9;
		bool field_EA;
		bool field_EB;
		bool field_EC;
		bool field_ED;
		bool field_EE;
		bool field_EF;
		std::array<SoundSetting, 12> SoundSettings;
		int SoundQuality;
		bool MuteSoundWhenNotFocused;
		int SoundPartyLosingThreshold;
		int SoundPartyWinningThreshold;
		float GameCameraRotation;
		float CameraSpeedMultiplier;
		bool GameCameraRotationLocked;
		bool GameCameraEnableCloseUpDialog;
		bool GameCameraEnableDynamicCombatCamera;
		bool GameCameraShakeEnabled;
		bool GameCamAvoidScenery;
		bool GameCameraControllerMode;
		bool field_B8E;
		bool field_B8F;
		uint32_t NrOfQuickSaves;
		uint32_t NrOfAutoSaves;
		uint32_t NrOfReloadSaves;
		uint32_t MaxNrOfQuickSaves;
		uint32_t MaxNrOfAutoSaves;
		uint32_t MaxNrOfReloadSaves;
		bool CanAutoSave;
		bool ForceSplitscreen;
		__int16 field_BAA;
		int MaxAmountDialogsInLog;
		float field_BB0;
		float ControllerCharacterWalkThreshold;
		float ControllerCharacterRunThreshold;
		float ControllerMoveSweepCone;
		float AIBoundsSizeMultiplier;
		float WeaponRangeMultiplier;
		float RotateRampSpeed;
		float MaxRotateSpeed;
		bool MoveDirectionCount;
		bool field_BC9;
		bool field_BCA;
		bool field_BCB;
		bool field_BCC;
		bool field_BCD;
		bool field_BCE;
		bool field_BCF;
		bool FadeCharacters;
		bool SomeAiLogFlag;
		bool field_BD2;
		bool field_BD3;
		bool AutoIdentifyItems;
		bool EnableVoiceLogging;
		bool MouseLock;
		bool DisableEdgePanning;
		bool DualDialogsEnabled;
		bool EscClosesAllUI;
		bool ShowSubtitles;
		bool EnableGameOver;
		bool UseRadialContextMenu;
		bool LogSaveLoadErrors;
		bool EnableAiThinking;
		bool EnableBlending;
		float OverheadZoomModifier;
		bool OverheadZoomEnabled;
		bool AddGenericKeyWords;
		bool AddStoryKeyWords;
		bool CacheDialogs;
		bool AllowXPGain;
		bool ShowSubtitles2;
		uint8_t Difficulty;
		uint8_t GameMode;
		bool field_BF4;
		bool field_BF5;
		bool field_BF6;
		bool field_BF7;
		FixedString ItemColorOverride;
		int PeaceCharacterHighlightMode;
		int CombatCharacterHighlightMode;
		int TacticalCharacterHighlightMode;
		int ArenaCharacterHighlightMode;
		int TutorialBoxMode;
		bool PeaceCharacterHighlightFlag;
		bool CombatCaracterHighlightFlag;
		bool TacticalCharacterHighlightFlag;
		bool ArenaCharacterHighlightFlag;
		bool ShowBuildVersion;
		bool SomeConditionalStoryLogField;
		bool field_C1A;
		bool field_C1B;
		int NextServerMode;
		int ServerMode;
		uint32_t ServerFrameCap;
		std::array<float, 61> Floats1;
		std::array<float, 61> Floats2;
		std::array<float, 32> Floats3;
		std::array<SomeOption, 50> Options2;
		int GameMasterBind;
		bool AutoRemoveHotbarSkills;
		uint8_t AutoFillHotbarCategories;
		bool AllowMovementFreePointer;
		int field_11B8;
		int field_11BC;
		int field_11C0;
		int field_11C4;
		int field_11C8;
	};

	struct InputValue
	{
		int field_0;
		int field_4;
		bool field_8;
	};

	struct InputEvent
	{
		int EventId;
		int InputPlayerIndex;
		int16_t InputDeviceId;
		InputValue OldValue;
		InputValue NewValue;
		InputType Type;
		bool WasPreferred;
	};


	struct TranslatedArgumentString
	{
		void* VMT{ nullptr };
		ObjectSet<void*>* ArgumentObjectSet{ nullptr };
		bool HasArgumentSet{ false };
		void* ArgumentBuf{ nullptr };
		STDWString Unknown;
		uint32_t ArgumentBufSize{ 0 };
		TranslatedString TranslatedStr;
	};

	struct TranslatedStringRepository : public ProtectedGameObject<TranslatedStringRepository>
	{
		typedef TranslatedStringRepository* (*GetInstance)();
		typedef STDWString * (*Get)(TranslatedStringRepository* self, RuntimeStringHandle* handle, uint64_t * length, char gender1, char gender2, bool unknown);
		typedef void (*UnloadOverrides)(TranslatedStringRepository* self);

		void* VMT;
		int field_8;
		Map<FixedString, STDWString*>* TranslatedStrings[4];
		Map<FixedString, STDWString*>* TranslatedStringOverrides[4];
		Map<FixedString, TranslatedArgumentString*> TranslatedArgumentStrings;
		Map<FixedString, RuntimeStringHandle> StringHandles;
		CRITICAL_SECTION CriticalSection;
		ScratchBuffer ScratchBuf;
		bool DoDecoration;
		bool IsLoaded;
		STDWString NotFoundStr;
	};

	struct TranslatedStringKeyManager : public ProtectedGameObject<TranslatedStringKeyManager>
	{
		typedef TranslatedStringKeyManager* (*GetInstance)();
		typedef TranslatedString * (*GetTranlatedStringFromKey)(TranslatedStringKeyManager* self, TranslatedString& string, FixedString const& handle, bool unknown);

		void* VMT;
		ObjectSet<Path> Paths;
		Map<FixedString, TranslatedString> StringKeys;
		Map<FixedString, FixedString> StringSpeakers;
		bool Unknown;
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
			int PathRotationType;
			float PathRadius;
			float PathShift;
			float PathMinArcDist;
			float PathMaxArcDist;
			uint64_t PathRepeat;
			uint8_t PathRandom;
		};


		struct NetworkFixedStrings : public ProtectedGameObject<NetworkFixedStrings>
		{
			uint16_t Initialized;
			ObjectSet<FixedString, GameMemoryAllocator, true> FixedStrSet;
			Map<FixedString, int> FixedStrToNetIndexMap;
			uint64_t Unkn1;
			uint64_t Unkn2;
		};

		struct VoiceTextMetaData
		{
			float Length;
			bool IsRecorded;
			int Priority;
			Path Source;
			int CodecID;
		};

		struct SpeakerManager
		{
			void * VMT;
			RefMap<FixedString, RefMap<FixedString, VoiceTextMetaData>> * SpeakerMetaDataHashMap;
		};
		

		struct TextBuffer
		{
			wchar_t * Buf;
			uint64_t Capacity;
			uint64_t Length;
			uint64_t Unknown;

			void Replace(WStringView replacement);
		};

		struct Text
		{
			struct Param
			{
				int32_t PlaceholderOffset;
				int32_t Unknown[2];
				int32_t PlaceholderSize;
			};

			void * VMT;
			TextBuffer * Buf;
			uint32_t Unknown[2];
			Param Params[8];

			void ReplaceParam(int index, WStringView replacement);
		};
	}

	namespace esv
	{

	struct SummonHelperResults
	{
		ComponentHandle SummonHandle;
		uint32_t Unknown{ 0 };
	};

	struct SummonHelperSummonArgs
	{
		ComponentHandle OwnerCharacterHandle;
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

	typedef void(*SummonHelpers__Summon)(SummonHelperResults * Results, SummonHelperSummonArgs * Args);
	typedef void(*GameStateEventManager__ExecuteGameStateChangedEvent)(void * self, GameState fromState, GameState toState);
	typedef void(*GameStateThreaded__GameStateWorker__DoWork)(void * self);
	typedef void(*GameStateMachine__Update)(void* self, GameTime* time);

	}

	enum class FileType
	{
		Unknown,
		MemBuffer,
		MemoryMapped
	};

	struct FileReader : public Noncopyable<FileReader>
	{
		using CtorProc = FileReader * (FileReader* self, Path const& path, unsigned int type);
		using DtorProc = void(FileReader* self);

		bool IsLoaded{ false };
		void* DataPtr{ nullptr };
		void* ReadPtr{ nullptr };
		uint64_t FileSize{ 0 };
		uint64_t FileHandle{ 0 };
		uint64_t FileHandle2{ 0 };
		ScratchBuffer ScratchBuffer;
		FileType Type{ FileType::Unknown };
		void* FileObject{ nullptr };
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
				return reader_->DataPtr;
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

	class TempStrings
	{
	public:
		char const * Make(STDString const & str);
		char const * Make(std::string const & str);

	private:
		std::vector<char *> pool_;
	};

	extern TempStrings gTempStrings;

	struct App
	{
	public:
		struct VMT
		{
			using OnInputEventProc = void (App* self, uint16_t& retval, InputEvent const& inputEvent);

			void* Destroy;
			void* GetInputListenerPriority;
			void* GetInputListenerFilter;
			void* GetInputListenerName;
			void* IsModal;
			void* InTextInput;
			OnInputEventProc* OnInputEvent;
			void* OnInputModifierChangedEvent;
			void* OnInputEventText;
			void* OnUnlinkedInput;
		};

		VMT* __vftable;
		void* InputDeviceListenerVMT;
		void* WindowEventListenerVMT;
		void* API;
	};
}
