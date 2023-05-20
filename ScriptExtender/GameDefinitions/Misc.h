#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/EntitySystem.h>
#include <GameDefinitions/CommonTypes.h>
#include <GameDefinitions/GameObjects/Module.h>
#include <glm/gtc/quaternion.hpp>

namespace dse
{
	struct GlobalCameraSwitches
	{
		float MoveSpeed;
		float LookAtDistanceThreshold;
		float DistanceAdjustmentSpeedMultiplier;
		float LookAtSpeedPower;
		float DistanceAdjustmentSpeedMultiplier2;
		float LookAtSpeedPower2;
		float MinCameraDistance;
		float MaxCameraDistance;
		float DefaultCameraDistance;
		float MinCameraDistanceController;
		float MaxCameraDistanceController;
		float DefaultCameraDistanceController;
		float MinCameraDistanceWithTarget;
		float MaxCameraDistanceWithTarget;
		float DefaultCameraDistanceWithTarget;
		float TargetHeightOffset;
		float TargetHeightOffsetController;
		float field_44;
		float field_48;
		float FOV;
		float FOVController;
		float CameraAvoidanceMultiplier;
		float CameraAvoidanceMultiplierWithTarget;
		float CamAvoidanceOffset;
		float CamAvoidanceOffsetWithTarget;
		float CamAvoidancePower;
		float CamAvoidancePowerWithTarget;
		float DistanceAdjustmentThreshold;
		float DistanceAdjustmentSpeedPower;
		float BaseCameraDistance;
		float BaseTargetCameraDistance;
		float ViewportFrameCursorThreshold1;
		float ViewportFrameCursorThreshold2;
		float DefaultCameraDistanceOverhead;
		float MinCameraDistanceOverhead;
		float MaxCameraDistanceOverhead;
		float FOVOverhead;
		glm::vec3 DefaultPosition;
		glm::vec3 DefaultPosition2;
		glm::vec3 DefaultCombatPosition;
		glm::vec3 DefaultCombatPosition2;
		glm::vec3 DefaultOverheadPosition;
		glm::vec3 DefaultOverheadPosition2;
		glm::vec3 DefaultControllerPosition;
		glm::vec3 DefaultControllerPosition2;
	};

	struct GlobalSwitches
	{
		struct ControlSwitch
		{
			float field_0;
			float field_4;
			float field_8;
			float field_C;
			float field_10;
			std::array<float, 4> PickingBoundLimits;
			std::array<float, 4> PickingAngleLimits;
			std::array<float, 4> PickingBoundDistances;
			float PickingMaxSmallBound;
			float PickingDistanceBias;
			float PickingAngleBias;
			float PickingCharacterBoundBias;
			float PickingHeightCutoff2;
			float PickingTorchBias;
			float PickingHeightCutoff;
			float ItemReachMaxDistance;
			float JoystickDistanceMultiplier;
			float JoystickDistanceThreshold1;
			float JoystickDistanceThreshold2;
			float JoystickAngleThreshold;
			int field_74;
			float HeightMultiplier;
			float field_7C;
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
		uint32_t ControllerLayout;
		bool RotateMinimap;
		bool ShowOverheadDamage;
		bool ShowOverheadText;
		bool ShowOverheadDialog;
		bool ShowFeedback;
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
		uint32_t NrOfQuickSaves;
		uint32_t NrOfAutoSaves;
		uint32_t NrOfReloadSaves;
		uint32_t MaxNrOfQuickSaves;
		uint32_t MaxNrOfAutoSaves;
		uint32_t MaxNrOfReloadSaves;
		bool CanAutoSave;
		bool ForceSplitscreen;
		int MaxAmountDialogsInLog;
		float field_BB0;
		float ControllerCharacterWalkThreshold;
		float ControllerCharacterRunThreshold;
		float ControllerMoveSweepCone;
		float AIBoundsSizeMultiplier;
		float WeaponRangeMultiplier;
		float RotateRampSpeed;
		float MaxRotateSpeed;
		uint64_t MoveDirectionCount;
		bool FadeCharacters;
		bool EnableAiLogging;
		bool AiLevelScaleScores;
		bool AiUsePositionScores;
		bool PeaceMode;
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
		bool ShowSpeechBubble;
		uint8_t Difficulty;
		uint8_t GameMode;
		int field_BF4;
		FixedString DebugItemColorOverride;
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
		GlobalCameraSwitches CameraSwitchesMode0;
		GlobalCameraSwitches CameraSwitchesMode1;
		ControlSwitch ControlSwitches;
		std::array<glm::vec4, 50> OverlayColors;
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

	struct GraphicSettings
	{
		void* VMT;
		bool Fullscreen;
		bool FakeFullscreenEnabled;
		int ScreenWidth;
		int ScreenHeight;
		int RefreshRateNumerator;
		int RefreshRateDenominator;
		int MonitorIndex;
		bool VSync;
		int VSyncDivider;
		bool TripleBuffering;
		bool ClothGPUAcceleration;
		float GammaCorrection;
		float HDRGamma;
		int TextureDetail;
		int TextureFiltering;
		bool TextureStreamingEnabled;
		int ModelDetail;
		int AnimationAllowedPixelError;
		int LightingDetail;
		bool EnableSpotLightsSMAA;
		bool ShadowsEnabled;
		bool PointLightShadowsEnabled;
		int ShadowQuality;
		float MaxDrawDistance;
		float MaxDrawDistance2;
		int AntiAliasing;
		bool BloomEnabled;
		bool DOFEnabled;
		bool GodRaysEnabled;
		bool LensFlareEnabled;
		bool MotionBlurEnabled;
		bool SSAOEnabled;
		bool EnableSSR;
		bool UseForwardRendering;
		bool EnableLightAssignmentStage;
		bool FrameCapEnabled;
		int16_t FrameCapFPS;
		bool RenderMT;
		float HDRPaperWhite;
		float HDRMaxNits;
		bool PostProcessingInjectTexture;
		bool ShowHDRCalibration;
	};


	struct Mouse
	{
		glm::vec2 Position;
		bool field_8;
	};


	struct CoreGlobals
	{
		Mouse* Mouse;
		GameTime GameTime;
		double TotalTime;
		int LastDeltaTime;
		int NumTicks;
		void* WorkerThreadScheduler;
		bool field_30;
		bool field_31;
	};


	struct Timeline
	{
		double Clock;
		float LocalClock;
		float NextSampleTime;
		float Duration;
		int NumSamples;
		int Loop;
		float Speed;
		int NumSamplesEvents;
		ObjectSet<void*>* Events;
		void* Listener;
		bool OwnsEventList;
		bool Running;
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

	struct PreciseClock
	{
		uint64_t PerformanceCounter;
		uint64_t TimeElapsed;
		bool Started;
	};

	struct GameClock : public PreciseClock
	{
		int FrameCap;
		uint64_t LastFrameTime;
		uint64_t PerformanceFrequency;
		uint64_t StartPerformanceCounter;
		uint64_t CurrentPerformanceCounter;
		uint64_t FrameStartPerformanceCounter;
	};

	struct InputEvent;
	struct RenderThread;

	struct IApp : public ProtectedGameObject<IApp>
	{
	public:
		struct VMT
		{
			using OnInputEventProc = void (IApp* self, uint16_t& retval, InputEvent const& inputEvent);

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

	struct BaseApp : public IApp
	{
		void* VMT;
		void* TaskBarPtr;
#if !defined(OSI_EOCAPP)
		bool TaskBarInitialized;
#endif
		STDString MediaPathTxt;
		void* DebugStringScratchBuffer;
		void* DebugStringScratchBufferCol2;
		__int64 field_68;
		void* CameraManager;
		int DefaultRenderFrameId;
		void* DefaultRenderView;
		void* FullScreenRenderView;
		void* field_90;
		void* field_98;
		Map<FixedString, void*> Map__HM;
		GameClock Clock;
		double LastTickTime;
		float LastDeltaTime;
		bool SomeTickFlag;
		__int64 field_110;
		uint8_t ScreenFade[144];
		RenderThread* RenderThread;
#if !defined(OSI_EOCAPP)
		void* Console;
		void* ConsoleServer;
		__int64 field_1C0;
#endif
		bool IsWindowOpen;
		bool CloseRequested;
		bool IsFocused;
		WORD FrameCap;
		uint64_t GameClockInitValue;
		uint64_t GameClockInitValue2;
		void* FILEptr;
		void* Config;
		void* Config2;
		void* GraphicSettingsConfig;
		__int64 lsImmediateCommandManager_CommandBuffer;
		__int64 ImmediateCommandManager_CommandBuffer;
		__int64 RelaxedCommandManager_CommandBuffer;
		__int64 field_210;
		__int64 EntityManager;
#if defined(OSI_EOCAPP)
		void* field_1F8;
#endif
		bool IsInScreenShotVideoMode;
#if !defined(OSI_EOCAPP)
		ScratchBuffer ScratchStr;
#endif
		CRITICAL_SECTION ConsoleCriticalSection;
		ObjectSet<void*> ConsoleCommandBuffers;
		void* EntityWorld;
		char field_298;
		__int64 field_2A0;
		__int64 field_2A8;
		__int64 field_2B0;
		__int64 field_2B8;

		rf::RendererCommandBuffer* GetRCB();
	};

	struct App : public BaseApp
	{
		void* EoC;
		void* ServerWorker;
		bool RequestStopServer;
		ObjectSet<UserId> OS_UserID2;
		ObjectSet<UserId> OS_UserID;
		ObjectSet<uint16_t> OS_Short;
	};

}
