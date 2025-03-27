#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/EntitySystem.h>
#include <GameDefinitions/Stats.h>
#include <GameDefinitions/GameObjects/Status.h>
#include <GameDefinitions/Sound.h>
#include <GameDefinitions/GameObjects/RootTemplates.h>
#include <GameDefinitions/GameObjects/Ai.h>
#include <GameDefinitions/GameObjects/Plan.h>

BEGIN_NS(eoc)

struct PlayerUpgrade : public ProtectedGameObject<PlayerUpgrade>
{
	stats::Character * Stats;
	uint32_t AttributePoints;
	uint32_t CombatAbilityPoints;
	uint32_t CivilAbilityPoints;
	uint32_t TalentPoints;
	uint32_t Unknown;
	ObjectSet<int> Attributes;
	ObjectSet<int> Abilities;
	stats::TalentArray Talents;
	ObjectSet<uint16_t> Traits;
	bool IsCustom;
};

struct PlayerCustomData : public ProtectedGameObject<PlayerCustomData>
{
	void* VMT;
	bool Initialized; // If true, CustomIconImg will be used
	bool CustomLookEnabled;
	STDWString Name;
	TranslatedString NameTranslated;
	FixedString ClassType;
	uint32_t SkinColor;
	uint32_t HairColor;
	uint32_t ClothColor1;
	uint32_t ClothColor2;
	uint32_t ClothColor3;
	bool IsMale;
	FixedString Race;
	FixedString OriginName;
	FixedString Icon;
	FixedString MusicInstrument;
	FixedString OwnerProfileID;
	FixedString ReservedProfileID;
	FixedString AiPersonality;
	FixedString Speaker;
	ScratchBuffer CustomIconImg;
};

struct SkillBarItem : public ProtectedGameObject<SkillBarItem>
{
	SkillBarItemType Type;
	FixedString SkillOrStatId;
	ComponentHandle ItemHandle;
};

END_NS()


BEGIN_NS(esv)

struct Skill : public ProtectedGameObject<Skill>
{
	void * VMT;
	FixedString UnknownFS;
	NetId NetID;
	SkillAIParams AIParams;
	ObjectSet<ComponentHandle> CauseList;
	ComponentHandle UnknownHandle;
	uint32_t Unknown1;
	ComponentHandle OwnerHandle;
	FixedString SkillId;
	float ActiveCooldown;
	bool IsActivated;
	bool IsLearned;
	bool ZeroMemory;
	bool OncePerCombat;
	bool ShouldSyncCooldown;
	int32_t NumCharges;
	int32_t MaxCharges;
	uint64_t Unknown3;
};

struct SkillManager : public ProtectedGameObject<SkillManager>
{
	using CanMemorizeProc = bool (SkillManager* self, stats::SkillPrototype* skill, bool checkMemoryRequirement);

	SkillState * CurrentSkillState;
	ComponentHandle OwnerHandle;
	Map<FixedString, Skill *> Skills;
	RefMap<FixedString, uint32_t> TimeItemAddedToSkillManager;
	bool IsLoading;
	uint32_t FreeMemorySlots;
};

struct PlayerCustomData : public eoc::PlayerCustomData {};

struct PlayerData : public ProtectedGameObject<PlayerData>
{
	struct PickpocketData
	{
		int64_t Weight;
		int64_t Value;
	};

	ComponentHandle SomeObjectHandle;
	ObjectSet<eoc::SkillBarItem> SkillBar;
	ObjectSet<uint32_t> LockedAbility;
	Map<FixedString, void *> ShapeShiftVariableManagers;
	Map<FixedString, void *> ShapeShiftAttitudeMaps;
	bool LevelUpMarker;
	uint8_t SelectedSkillSetIndex;
	FixedString QuestSelected;
	PlayerCustomData CustomData;
	RefMap<ComponentHandle, PickpocketData> * PreviousPickpocketTargets;
	ComponentHandle PickpocketTarget;
	ObjectSet<glm::vec3> PreviousPositions;
	uint32_t PreviousPositionId;
	bool HelmetOption;
	bool ShouldReevaluateSkillBar;
	uint32_t Renown;
	uint32_t CachedTension;
	ComponentHandle SomeObjectHandle3;
	uint8_t HomesteadKeyState;
	ComponentHandle RecruiterHandle;
	FixedString OriginalTemplate;
	FixedString Region;
};


struct CharacterBody
{
	void* VMT;
	Character* Character;
	FixedString CurrentAnimation;
	FixedString AnimationFinshedEvent_M;
	float field_20;
	FixedString NextAnimation;
	float TimeOffset;
	bool ForceStartAnimation;
	ActionState* ActionState;
	Timeline* Timeline;
	AnimationResource* UpperAnimationResource;
};


struct Character : public IEoCServerObject
{
	static constexpr auto ComponentPoolIndex = EntityComponentIndex::Character;
	static constexpr auto ObjectTypeIndex = ObjectHandleType::ServerCharacter;

	using HitProc = void (esv::Character* self, stats::Character* attackerStats, stats::Item* itemStats, stats::DamagePairList* damageList,
		stats::HitType hitType, bool noHitRoll, stats::HitDamageInfo* damageInfo, int forceReduceDurability, stats::PropertyList* skillProperties, stats::HighGroundBonus highGroundFlag, bool procWindWalker, stats::CriticalRoll criticalRoll);
	using ApplyDamageProc = void (esv::Character* self, stats::HitDamageInfo& hit, uint64_t attackerHandle, CauseType causeType, glm::vec3& impactDirection, bool enterCombat);
	using CheckSkillRequirementsProc = uint64_t (Character* self, FixedString const& skillId, Item* item, bool checkAP, bool mustHaveSkill);

	Status* GetStatus(ComponentHandle handle, bool returnPending, bool returnUnapplied = false) const;
	Status* GetStatus(NetId handle) const;

	glm::vec3 WorldPos; // Saved
	CharacterFlags Flags; // Saved
	float Radius;
	FixedString CurrentLevel; // Saved
	glm::mat3 WorldRot;
	float Scale;
	ObjectSet<void *> PeerIDClassNames;
	CharacterTemplate* CurrentTemplate;
	CharacterTemplate* OriginalTemplate;
	CharacterTemplate* TemplateUsedForSkills;
	CharacterFlags2 Flags2;
	CharacterFlags3 Flags3;
	uint8_t FlagsEx;
	uint8_t Team;
	uint8_t Color;
	glm::vec3 LevelTemplatePosition;
	glm::vec3 AoOVelocityOverride;
	glm::vec3 SomePositionOverride;
	glm::vec3 NextCheckpointPosition;
	eoc::Ai* AI;
	FixedString AnimationOverride;
	float WalkSpeedOverride;
	float RunSpeedOverride;
	ObjectSet<FixedString> VoiceSet;
	uint8_t NeedsUpdateCount;
	uint8_t ScriptForceUpdateCount;
	uint8_t ForceSynchCount;
	bool U5;
	stats::Character * Stats;
	ComponentHandle InventoryHandle;
	MovementMachine * MovementMachine;
	ActionMachine * ActionMachine;
	void * SteeringMachine;
	void * BehaviorMachine;
	void * CharacterSupervisor;
	void * NetworkController;
	TaskController * OsirisController;
	void * RequestController;
	StatusController* StatusController;
#if !defined(OSI_EOCAPP)
	uint64_t U8[2];
#endif
	void * ScriptController;
	TaskController * DialogController;
	StatusMachine * StatusMachine;
	SkillManager * SkillManager;
	void * VariableManager;
	void * ShapeShiftVariableManager;
	Map<FixedString, void *> Attitudes; // Element type unknown
	FixedString SkillBeingPrepared;
	void* CurrentTemplate2; // Not a RootTemplate!
	uint32_t Dialog;
	bool IsDialogAiControlled;
	float LifeTime;
	float TurnTimer;
	float TriggerTrapsTimer;
	UserId UserID;
	UserId ReservedUserID;
	uint32_t U12;
	ComponentHandle OwnerHandle;
	ComponentHandle FollowCharacterHandle;
	ComponentHandle EnemyCharacterHandle;
	ComponentHandle SpiritCharacterHandle;
	ComponentHandle CorpseCharacterHandle;
	ComponentHandle DroppingItemHandle_M;
	ObjectSet<ComponentHandle> EnemyHandles;
	ObjectSet<SurfacePathInfluence> SurfacePathInfluences;
	ObjectSet<ComponentHandle> SummonHandles;
	PlanManager* PlanManager;
	float PartialAP;
	uint8_t AnimType;
	uint8_t DelayDeathCount;
	Status * DelayedDyingStatus;
	ObjectSet<ComponentHandle> RegisteredTriggerHandles;
	ObjectSet<FixedString> RegisteredTriggers;
	PlayerData * PlayerData;
	eoc::PlayerUpgrade PlayerUpgrade;
	/// Number of times ServerControl was acquired on this character
	uint32_t ServerControlRefCount;
	float NoiseTimer;
	float RealtimeWalkSpeed;
	STDWString * CustomDisplayName;
	void * StoryDisplayName;
	TranslatedString OriginalTransformDisplayName;
	int32_t MaxVitalityPatchCheck;
	int32_t MaxArmorPatchCheck;
	int32_t MaxMagicArmorPatchCheck;
	float SpeedMultiplier;
	FixedString AnimationSetOverride;
	ComponentHandle PartyHandle;
	ObjectSet<FixedString> CreatedTemplateItems;
	ObjectSet<FixedString> Treasures;
	FixedString CustomTradeTreasure;
	ObjectSet<FixedString> Tags;
	ComponentHandle CrimeHandle;
	ComponentHandle PreviousCrimeHandle;
	uint32_t CrimeState;
	uint32_t PreviousCrimeState;
	bool IsAlarmed;
	bool CrimeWarningsEnabled;
	bool CrimeInterrogationEnabled;
	uint32_t InvestigationTimer;
	ObjectSet<FixedString> DisabledCrime;
	uint64_t DamageCounter;
	uint64_t HealCounter;
	uint64_t KillCounter;
	ComponentHandle MovingCasterHandle;
	FixedString Archetype;
	FixedString EquipmentColor;
	FixedString ProjectileTemplate;
	uint32_t TimeElapsed;
	ObjectSet<FixedString> PreferredAiTarget;
	CharacterBody* CharacterBody;
	RefMap<ComponentHandle, ObjectSet<stats::PropertyStatus*>> StatusesFromItems;
	ObjectSet<FixedString> TagsFromItems;
	VisualSetIndices VisualSetIndices;
	bool ReadyCheckBlocked;
	bool CorpseLootable;
	uint8_t NumConsumables;
	SurfaceType CustomBloodSurface;
	FixedString PreviousLevel;

	// For compatibility with v55
	ObjectSet<FixedString> GetInventoryItemGuids();
	ObjectSet<Item*> GetInventoryItems();
	ObjectSet<FixedString> GetNearbyCharacters(float distance);
	ObjectSet<FixedString> GetSummonGuids();
	ObjectSet<Character*> GetSummons();
	ObjectSet<FixedString> GetSkillIds();
	Skill* GetSkillInfo(FixedString const& skill);
	std::optional<int> GetCustomStatValue(FixedString const& statId);
	bool SetCustomStatValue(FixedString const& statId, int value);

	CharacterFlags LuaGetFlags();
	void LuaSetFlags(CharacterFlags flags);
	bool LuaHasFlag(CharacterFlags flag);
	void LuaSetFlag(CharacterFlags flag, bool set);
	UserReturn LuaGetUserVariables(lua_State* L);
};

struct CharacterFactory : public NetworkComponentFactory<Character>
{
	void* VMT2;
	void* VMT3;
	Map<FixedString, void*> FSMap_ReloadComponent;
	EntityWorld* Entities;
	uint64_t Unkn8[2];
};

struct CharacterManager : public BaseComponentProcessingSystem<EntityWorld>
{
	struct AnimationBlueprintEntry
	{
		Character* Character;
		void* AnimationBlueprintComponent;
	};

	struct TransformParams
	{
		PeerId PeerID;
		ComponentHandle TargetCharacterHandle;
		TemplateHandle Template;
		ComponentHandle TemplateCharacterHandle;
		FixedString EquipmentSetName;
		CharacterTransformType TransformType;
		CharacterTransformFlags Flags;
	};

	void* GameEventManagerVMT;
	CharacterFactory* CharacterFactory;
	ObjectSet<Character*> RegisteredCharacters;
	ObjectSet<Character*> ActiveCharacters;
	ObjectSet<TransformParams> PendingTransforms;
	ObjectSet<TransformParams> NetPendingTransforms;
	ObjectSet<ComponentHandle> Unknown;
	ObjectSet<Character*> CharactersUpdatedLastTick_OS_pCharacter3;
	ObjectSet<AnimationBlueprintEntry> ActiveAnimationBlueprints;
};

END_NS()

BEGIN_NS(ecl)

struct PlayerCustomData : public eoc::PlayerCustomData {};

struct PlayerData : public ProtectedGameObject<PlayerData>
{
	ObjectSet<eoc::SkillBarItem> SkillBarItems;
	ObjectSet<uint32_t> LockedAbilities;
	Map<ComponentHandle, int> AttitudeOverrideMap;
	uint8_t SelectedSkillSet;
	__int64 field_60;
	bool HasLevelUpMarker;
	PlayerCustomData CustomData;
	char field_1C8;
	__int64 field_1D0;
	NetId PickpocketTargetNetID;
	NetId CorpseLootTargetNetID;
	bool HelmetOptionState;
	bool ArmorOptionState;
	int CachedTension;
	FixedString QuestSelected;
	ObjectSet<FixedString> MemorisedSkills;
	char field_210;
	char field_211;
	int NetID3;
	FixedString OriginalTemplate;
	FixedString Region;
};


struct Skill : public ProtectedGameObject<Skill>
{
	void* VMT;
	FixedString GUID_Unused;
	NetId NetID;
	ComponentHandle Handle;
	SkillType Type;
	ComponentHandle OwnerHandle;
	FixedString SkillId;
	float ActiveCooldown;
	bool CanActivate;
	bool IsActivated;
	bool HasCooldown;
	bool IsLearned;
	bool ZeroMemory;
	int32_t CauseListSize;
	int32_t NumCharges;
	int32_t MaxCharges;
};


struct SkillManager : public ProtectedGameObject<SkillManager>
{
	using CheckSkillRequirementsProc = bool (SkillManager* self, stats::SkillPrototype* proto);

	SkillState* CurrentSkill;
	ComponentHandle OwnerHandle;
	Map<FixedString, Skill*> Skills;
};


struct CharacterBody : public ProtectedGameObject<CharacterBody>
{
	ComponentHandle CharacterHandle;
	Character* Character;
	FixedString NextAnimation;
	FixedString CurrentAnimation;
	std::array<FixedString, 2> AnimationOverride;
	float AnimRandom;
	float ClimbLoopDuration;
	FixedString QueuedAnimation;
	bool AnimationRunning;
	bool IsImmediate;
	bool WaitForLoad;
	float AnimationDuration;
	float SpeedScaledAnimationDuration;
	ActionState* ActionState;
};


struct Character : public IEoCClientReplicatedObject
{
	static constexpr auto ComponentPoolIndex = EntityComponentIndex::Character;
	static constexpr auto ObjectTypeIndex = ObjectHandleType::ClientCharacter;

	using CheckSkillRequirementsProc = uint64_t (Character* self, FixedString const& skillId, Item* item);
	using GetSkillRequirementsProc = void (Character* character, FixedString const& skillId, Item* item, uint32_t checkRequirementFlags, eoc::Text& requirementsText);
	using RefreshEquipmentVisualsProc = void(Character* character);

	virtual void SetLight(FixedString*) = 0;
	virtual void* GetLight() = 0;
	virtual void RemoveLight() = 0;
	virtual FixedString* GetPlayerRace(bool returnPolymorph, bool excludeUninitialized) = 0;
	virtual FixedString* GetPlayerOrigin(bool returnPolymorph, bool excludeUninitialized) = 0;

	Status* GetStatus(ComponentHandle statusHandle) const;
	Status* GetStatus(NetId handle) const;

	glm::vec3 WorldPos;
	CharacterFlags Flags;
	float AIBoundSize;
	FixedString CurrentLevel;
	glm::mat3 WorldRot;
	float Scale;
	glm::vec3 Velocity;
	glm::vec3 MovementStartPosition;
	glm::vec3 LadderPosition;
	glm::vec3 MovementTargetPosition;
	PhysicsObject* Physics;
	void* Light;
	eoc::Ai* AI;
	int field_70;
	UserId UserID;
	int16_t ReservedForPlayerId;
	int16_t ReservedForPlayerId2;
	NetId NetID2;
	NetId NetID3;
	CharacterTemplate* CurrentTemplate;
	CharacterTemplate* OriginalTemplate;
	stats::Character* Stats;
	ComponentHandle InventoryHandle;
	MovementMachine* MovementMachine;
	ActionMachine* ActionMachine;
	void* SteeringMachine;
	void* BehaviorMachine;
	InputController* InputController;
	void* NetworkController;
	StatusController* StatusController;
	DialogController* DialogController;
	void* CharacterSupervisor;
	StatusMachine* StatusMachine;
	SkillManager* SkillManager;
	int AnimType;
	__int64 field_108;
	CharacterBody* CharacterBody;
	ComponentHandle OwnerCharacterHandle;
	ComponentHandle CorpseOwnerHandle;
	ComponentHandle CorpseCharacterHandle;
	ComponentHandle LootedByHandle;
	int field_138;
	ComponentHandle HighlightCircleEffect;
	ComponentHandle OH7;
	ComponentHandle ViewConeEffectHandle;
	int FeetStatusId;
	float FootStepEffectTimer;
	float RemainingMoveDistance;
	int FootStepMaterialType;
	PlayerData* PlayerData;
	eoc::PlayerUpgrade PlayerUpgrade;
	ObjectSet<SurfacePathInfluence> SurfacePathInfluences;
	ObjectSet<FixedString> Tags;
	float field_248;
	float LifeTime;
	float field_250;
	std::array<EntityHandle, 3> SoundComponents;
	ComponentHandle OH9;
	ComponentHandle FollowCharacterHandle;
	CharacterFlags3 Flags3;
	CharacterFlags2 Flags2;
	uint32_t field_284;
	uint8_t Color;
	uint8_t Team;
	TranslatedString* DisplayNameOverride;
	TranslatedString StoryDisplayName;
	TranslatedString OriginalDisplayName;
	ComponentHandle TalkingIconEffect;
	float AnimationSpeed;
	int SoundBoneIndex;
	int PhysicsBoneIndex;
	ObjectSet<FixedString> VoiceSet;
	FixedString AnimationSetOverride;
	float WalkSpeedOverride;
	float RunSpeedOverride;
	float SneakCheckTimer;
	float FadeTimer;
	ComponentHandle PartyHandle;
	int PhysicsPickingState;
	int CrimeState;
	void* SoftBodyCollisionTemplate;
	bool HasCloth;
	void* RagdollTemplate;
	uint32_t GMState;
	int64_t DamageCounter;
	int64_t HealCounter;
	int64_t KillCounter;
	ComponentHandle MovingProjectileHandle;
	FixedString Archetype;
	FixedString EquipmentColor;
	int DialogId;
	bool IsDialogAiControlled;
	ObjectSet<ComponentHandle> SummonHandles;
	Map<ComponentHandle, int> AttitudeMaps;
	uint64_t ImmuneFlags;
	__int64 field_4E0;
	ObjectSet<FixedString> ItemTags;
	VisualSetIndices VisualSetIndices;
	bool CorpseLootable;

	ObjectSet<FixedString> GetInventoryItemGuids();
	Item* GetItemBySlot(stats::ItemSlot32 slot);
	std::optional<FixedString> GetItemGuidBySlot(stats::ItemSlot32 slot);
	std::optional<int> GetCustomStatValue(FixedString const& statId);

	CharacterFlags LuaGetFlags();
	void LuaSetFlags(CharacterFlags flags);
	bool LuaHasFlag(CharacterFlags flag);
	void LuaSetFlag(CharacterFlags flag, bool set);
	UserReturn LuaGetUserVariables(lua_State* L);
};


struct EquipmentVisualsComponent : public BaseComponent
{
	struct LoadingVisual
	{
		void* DeferredVisual;
		ComponentHandle Handle;
	};
			
	struct VisualEntry
	{
		Visual* Visual;
		LoadingVisual* LoadingVisual;
		DeactivateEquipmentSlotMask EquipmentSlotMask;
		DeactivateVisualSetSlotMask VisualSetSlotMask;
	};

	std::array<VisualEntry, 14> Visuals;
};


struct EquipmentVisualSystemSetParam
{
	FixedString VisualResourceID;
	FixedString AttachmentBoneName;
	FixedString LevelName;
	ComponentHandle ClothHandle;
	void* PhysicsTemplate;
#if defined(OSI_EOCAPP)
	void* unknown_28;
#endif
	std::array<glm::vec4, 5> Colors;
	VisualAttachmentFlags AttachFlags;
	DeactivateEquipmentSlotMask EquipmentSlotMask;
	DeactivateVisualSetSlotMask VisualSetSlotMask;
	// 8b slot, 24b flags
	VisualSystemSetFlags SlotAndFlags;

	inline stats::ItemSlot32 GetSlot()
	{
		return (stats::ItemSlot32)((uint32_t)SlotAndFlags & 0xff);
	}

	void SetSlot(stats::ItemSlot32 slot)
	{
		SlotAndFlags = (VisualSystemSetFlags)(((uint32_t)SlotAndFlags & 0xffffff00) | (uint32_t)slot);
	}
};


struct EquipmentVisualsSystem
{
	using CreateVisualsProc = Visual* (EquipmentVisualsSystem*, EntityHandle entityHandle, EquipmentVisualSystemSetParam& params);

	struct UnloadRequest
	{
		ComponentHandleWithType ComponentHandle;
		VisualSystemSetFlags SlotAndFlags;
		uint8_t UnknFlag;
	};

	void* VMT;
	EntityWorld* EntityWorld;
	ObjectSet<UnloadRequest> UnloadRequests;
	void* ScopedThreadDispatcher;
	int field_38;
};

END_NS()
