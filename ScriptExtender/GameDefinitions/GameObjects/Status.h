#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/EntitySystem.h>
#include <GameDefinitions/Stats.h>

BEGIN_SE()

template <class TStatus>
struct StatusMachine : public NetworkComponentFactory<TStatus>
{
	TStatus * GetStatus(ComponentHandle handle) const
	{
		return this->Get(handle);
	}

	TStatus* GetStatus(StatusType type) const
	{
		for (auto status : Statuses) {
			if (status->GetStatusId() == type) {
				return status;
			}
		}

		return nullptr;
	}

	TStatus* GetStatus(FixedString const& statusId) const
	{
		for (auto status : Statuses) {
			if (status->StatusId == statusId) {
				return status;
			}
		}

		return nullptr;
	}

	TStatus* GetStatus(NetId netId) const
	{
		return this->FindByNetId(netId);
	}

	void* GameEventMgrVMT;
	bool IsStatusMachineActive;
	bool PreventStatusApply;
	ObjectSet<TStatus*> Statuses;
	ComponentHandle OwnerObjectHandle;
	uint32_t References;
};

END_SE()

BEGIN_NS(esv)

struct Status : public ProtectedGameObject<Status>
{
	static constexpr auto ObjectTypeIndex = ObjectHandleType::Unknown;

	using GetEnterChanceProc = int32_t(Status* self, bool isEnterCheck);
	using EnterProc = bool (Status* self);
	using ShouldDeleteProc = bool (Status* self);

	virtual ~Status() = 0;
	virtual void SetObjectHandle(ComponentHandle Handle) = 0;
	virtual void GetObjectHandle(ComponentHandle * Handle) = 0;
	virtual StatusType GetStatusId() = 0;
	// 0 - Stackable
	// 1 - Apply only the first instance, discard new ones
	// 2 - Apply only one instance, new instances replace old ones
	// 3 - Apply only the first instance; triggers combat?
	virtual uint32_t GetStatusType() = 0;
	virtual void AddStatsData2_Maybe() = 0;
	virtual void * GetStatsIdByIndex(int index) = 0;
	virtual void VMT38() = 0;
	virtual void VMT40() = 0;
	virtual void VMT48() = 0;
	virtual bool CanEnter() = 0;
	virtual void Init() = 0;
	virtual bool Enter() = 0;
	virtual bool Resume() = 0;
	virtual void Update(void * A, float Time_M) = 0;
	virtual void Tick(int * a2, float a3, float a4) = 0;
	virtual void Exit() = 0;
	virtual void VMT88() = 0;
	virtual void ConsumeStatsId() = 0;
	virtual void VMT98() = 0;
	virtual void IsImmobilizingStatus() = 0;
	virtual void VMTA8() = 0;
	virtual void VMTB0() = 0;
	virtual void GetSyncData() = 0;
	virtual void VMTC0() = 0;
	virtual void Serialize() = 0;
	virtual void VMTD0() = 0;
	virtual void CreateVisuals() = 0;
	virtual void DestroyVisuals() = 0;
	virtual void SetHostileFlagFromSavingThrow_M() = 0;
	virtual void GetEnterChance() = 0;
	virtual void AddStatsData_Maybe() = 0;

	// void * VMT;
	FixedString FS1;
	NetId NetID;
	uint64_t U1;
	FixedString StatusId; // Saved
	int32_t CanEnterChance; // Saved
	float StartTimer; // Saved
	float LifeTime; // Saved
	float CurrentLifeTime; // Saved
	float TurnTimer; // Saved
	float Strength; // Saved
	float StatsMultiplier; // Saved
	CauseType DamageSourceType; // Saved
	ComponentHandle StatusHandle;
	ComponentHandle OwnerHandle;
	ObjectSet<ComponentHandle> StatusOwner; // Saved
	ComponentHandle StatusSourceHandle; // Saved
	ComponentHandle CleansedByHandle;
	StatusFlags2 Flags2; // Saved
	StatusFlags0 Flags0; // Saved
	StatusFlags1 Flags1; // Saved
};

struct StatusConsumeBase : public Status
{
	struct StatsData
	{
		int32_t Turn;
		FixedString StatsId;
	};

	ObjectSet<FixedString> Skill; // Saved
	ObjectSet<FixedString> Items; // Saved
	ObjectSet<uint32_t> ResetCooldownsAbilities; // Set<TSkillAbility>
	bool ResetAllCooldowns; // Saved
	bool ResetOncePerCombat; // Saved
	bool ScaleWithVitality; // Saved
	bool LoseControl; // Saved
	FixedString ApplyStatusOnTick;
	ObjectSet<ComponentHandle> ItemHandles; // Saved
	float EffectTime; // Saved
	FixedString StatsId; // Saved
	ObjectSet<StatsData> StatsIds;
	FixedString StackId;
	FixedString OriginalWeaponStatsId;
	FixedString OverrideWeaponStatsId;
	ComponentHandle OverrideWeaponHandle;
	int AttributeHandle;
	int SavingThrow; // TODO enum + enum prop!
	glm::vec3 SourceDirection; // Saved
	ObjectSet<SurfaceTransformActionType> SurfaceChanges;
	int Turn; // Saved
	int field_1AC;
	Status* AuraStatus; // Saved
	HealEffect HealEffectOverride; // Saved
	bool Poisoned;
	char field_1BD;
	char field_1BE;
	char field_1BF;
};

struct StatusVMT
{
	void* Destroy;
	void* SetObjectHandle;
	void* GetObjectHandle;
	void* GetStatusId;
	void* GetTriggerBehavior_M;
	void* AddStatsData2_Maybe;
	void* GetStatsIdByIndex;
	void* VMT38;
	void* VMT40;
	void* VMT48;
	void* CanEnter;
	void* Init;
	Status::EnterProc* Enter;
	void* Resume;
	void* Update;
	void* Tick;
	void* Exit;
	void* VMT88;
	void* ConsumeStatsId;
	void* VMT98;
	void* IsImmobilizingStatus;
	void* VMTA8;
	void* VMTB0;
	void* GetSyncData;
	void* VMTC0;
	void* Serialize;
	void* VMTD0;
	void* CreateVisuals;
	void* DestroyVisuals;
	void* SetHostileFlagFromSavingThrow_M;
	Status::GetEnterChanceProc* GetEnterChance;
	void* AddStatsData_Maybe;
};

struct StatusHit : public Status
{
	using SetupProc = void (StatusHit* self, stats::HitDamageInfo* hit);

	uint32_t Unk2;
	uint8_t Unk3;
	stats::PropertyList * PropertyList;
	stats::HitDamageInfo Hit;
	ComponentHandle HitByHandle;
	// Character, Item or Projectile
	ComponentHandle HitWithHandle;
	ComponentHandle WeaponHandle;
	// 0 - ASAttack
	// 1 - Character::ApplyDamage, StatusDying, ExecPropertyDamage, StatusDamage
	// 2 - AI hit test
	// 5 - InSurface
	// 6 - SetHP, osi::ApplyDamage, StatusConsume
	uint32_t HitReason;
	FixedString SkillId;
	bool Interruption;
	bool AllowInterruptAction;
	// 0 = only interrupt Idle and Animation behaviors
	// 1 = always interrupt character action
	bool ForceInterrupt;
	// Decrease characters' DelayDeathCount
	bool DecDelayDeathCount;
	uint8_t PropertyContext;
	glm::vec3 ImpactPosition;
	glm::vec3 ImpactOrigin;
	glm::vec3 ImpactDirection;
	uint32_t Unk4;
};

struct StatusDying : public Status
{
	ComponentHandle SourceHandle;
	int SourceType;
	stats::DeathType DeathType;
	int AttackDirection;
	glm::vec3 ImpactDirection;
	bool IsAlreadyDead;
	bool DieActionsCompleted;
	bool field_B2;
	bool ForceNoGhost;
	bool SkipAnimation;
	bool DontThrowDeathEvent;
	ComponentHandle InflicterHandle;
	ComponentHandle DisputeTargetHandle;
	ComponentHandle OH4;
	uint8_t CombatId;
	bool IgnoreGodMode;
};

struct StatusHeal : public Status
{
	float EffectTime;
	int32_t HealAmount;
	HealEffect HealEffect;
	FixedString HealEffectId;
	StatusHealType HealType;
	int32_t AbsorbSurfaceRange;
	ObjectSet<SurfaceType> AbsorbSurfaceTypes;
	uint64_t Unkn2;
	bool TargetDependentHeal;
	std::array<int32_t, 3> TargetDependentValue;
	std::array<int32_t, 3> TargetDependentHealAmount;
	uint32_t Unkn3;
};

struct StatusMuted : public StatusConsumeBase {};

struct StatusCharmed : public StatusConsumeBase
{
	uint32_t UserId;
	ComponentHandle OriginalOwnerCharacterHandle;
};

struct StatusKnockedDown : public Status
{
	int KnockedDownState; // Saved
	bool IsInstant; // Saved
};

struct StatusSummoning : public Status
{
	float AnimationDuration;
	int SummonLevel; // Saved
};

struct StatusHealing : public StatusConsumeBase
{
	int32_t HealAmount; // Saved
	float TimeElapsed; // Saved
	HealEffect HealEffect; // Saved
	FixedString HealEffectId; // Saved
	bool SkipInitialEffect; // Saved
	int32_t HealingEvent; // Saved
	StatusHealType HealStat; // Saved
	int32_t AbsorbSurfaceRange; // Saved
};

struct StatusThrown : public Status
{
	int Level;
	ComponentHandle CasterHandle;
	float AnimationDuration;
	bool IsThrowingSelf; // Saved
	float LandingEstimate; // Saved
	bool Landed; // Saved
};

struct StatusTeleportFall : public Status
{
	glm::vec3 Target; // Saved
	float ReappearTime; // Saved
	FixedString SkillId; // Saved
	bool HasDamage; // Saved
	bool HasDamageBeenApplied; // Saved
};

struct StatusConsume : public StatusConsumeBase {};

struct StatusCombat : public Status
{
	bool ReadyForCombat; // Saved
	float field_94;
	int OwnerTeamId; // Saved
	__int64 field_A0;
};

struct StatusAoO : public Status
{
	ComponentHandle SourceHandle; // Saved
	ComponentHandle AoOTargetHandle; // Saved
	ComponentHandle PartnerHandle; // Saved
	bool ActivateAoOBoost; // Saved
	bool ShowOverhead; // Saved
};

struct StatusStoryFrozen : public Status {};

struct StatusSneaking : public Status
{
	bool ClientRequestStop; // Saved
};

struct StatusUnlock : public Status
{
	ComponentHandle SourceHandle; // Saved
	FixedString Key;
	int Level; // Saved
	int Unlocked; // Saved
};

struct StatusFear : public StatusConsumeBase {};

struct StatusBoost : public Status
{
	float EffectTime; // Saved
	FixedString BoostId; // Saved
};

struct StatusUnsheathed : public Status
{
	bool Force; // Saved
};

struct StatusStance : public StatusConsumeBase
{
	FixedString SkillId; // Saved
};

struct StatusLying : public Status
{
	ComponentHandle ItemHandle; // Saved
	glm::vec3 Position; // Saved
	int Index; // Saved
	int field_A8;
	float TimeElapsed; // Saved
	float Heal; // Saved
};

struct StatusSitting : public StatusLying {};

struct StatusBlind : public StatusConsumeBase {};

struct StatusSmelly : public Status {};

struct StatusClean : public Status {};

struct StatusInfectiousDiseased : public StatusConsumeBase
{
	int Infections; // Saved
	float InfectTimer; // Saved
	float Radius;
	ComponentHandle StatusTargetHandle; // Saved
};

struct StatusInvisible : public StatusConsumeBase
{
	glm::vec3 InvisiblePosition; // Saved
};

struct StatusRotate : public Status
{
	float Yaw; // Saved
	float RotationSpeed; // Saved
};

struct StatusEncumbered : public StatusConsumeBase {};

struct StatusIdentify : public Status
{
	int Level; // Saved
	int Identified; // Saved
	ComponentHandle IdentifierHandle; // Saved
};

struct StatusRepair : public Status
{
	int Level; // Saved
	int Repaired; // Saved
	ComponentHandle RepairerHandle; // Saved
};

struct StatusMaterial : public Status
{
	FixedString MaterialUUID; // Saved
	StatusMaterialApplyFlags ApplyFlags; // Saved
	bool IsOverlayMaterial; // Saved
	bool Fading; // Saved
	bool ApplyNormalMap; // Saved
	bool Force; // Saved
};

struct StatusLeadership : public StatusConsumeBase {}; 
		
struct StatusExplode : public Status
{
	FixedString Projectile; // Saved
};

struct StatusAdrenaline : public StatusConsumeBase
{
	int InitialAPMod; // Saved
	int SecondaryAPMod; // Saved
	int CombatTurn; // Saved
};

struct StatusShacklesOfPain : public StatusConsumeBase
{
	ComponentHandle CasterHandle; // Saved
};

struct StatusShacklesOfPainCaster : public StatusConsumeBase
{
	ComponentHandle VictimHandle; // Saved
};

struct StatusWindWalker : public StatusConsumeBase {};

struct StatusDarkAvenger : public StatusConsumeBase {};

struct StatusRemorse : public StatusConsumeBase {};

struct StatusDecayingTouch : public StatusConsumeBase {};

struct StatusUnhealable : public Status {};

struct StatusFlanked : public Status {};

struct StatusChanneling : public StatusStance {};

struct StatusDrain : public Status
{
	int Infused; // Saved
};

struct StatusLingeringWounds : public StatusConsumeBase {};

struct StatusInfused : public Status {};

struct StatusSpiritVision : public StatusConsumeBase
{
	FixedString SpiritVisionSkillId; // Saved
};

struct StatusSpirit : public Status
{
	ObjectSet<ComponentHandle> Characters; // Saved
};

struct StatusDamage : public StatusConsumeBase
{
	int32_t DamageEvent; // Saved
	float HitTimer; // Saved
	float TimeElapsed; // Saved
	int DamageLevel; // Saved
	FixedString DamageStats; // Saved
	bool SpawnBlood; // Saved
};

struct StatusForceMove : public StatusConsumeBase {};

struct StatusClimbing : public Status
{
	void* LadderData;
	int LadderData2;
	glm::vec3 MoveDirection;
	ComponentHandle LadderHandle; // Saved
	FixedString Level; // Saved
	int Status; // Saved
	bool Direction; // Saved
	bool JumpUpLadders;
	bool Incapacitated;
	bool Started;
};
		
struct StatusIncapacitated : public StatusConsumeBase
{
	float CurrentFreezeTime;
	float FreezeTime;
	uint8_t FrozenFlag;
};

struct StatusInSurface : public Status
{
	float SurfaceTimerCheck; // Saved
	float SurfaceDistanceCheck; // Saved
	glm::vec3 Translate; // Saved
	bool Force; // Saved
	ESurfaceFlag Layers; // Saved
};

struct StatusSourceMuted : public Status {};

struct StatusOverpowered : public StatusConsumeBase {};

struct StatusCombustion : public StatusConsumeBase {};

struct StatusPolymorphed : public StatusConsumeBase
{
	FixedString OriginalTemplate; // Saved
	FixedString TransformedRace; // Saved
	int OriginalTemplateType; // Saved
	FixedString PolymorphResult; // Saved
	bool DisableInteractions; // Saved
};

struct StatusDamageOnMove : public StatusDamage
{
	float DistancePerDamage; // Saved
	float DistanceTraveled; // Saved
};

struct StatusDemonicBargain : public Status {};

struct StatusGuardianAngel : public StatusConsumeBase {};

struct StatusFloating : public StatusConsumeBase {};

struct StatusChallenge : public StatusConsumeBase
{
	ComponentHandle SourceHandle; // Saved
	bool Target; // Saved
};

struct StatusDisarmed : public StatusConsumeBase {};

struct StatusHealSharing : public StatusConsumeBase
{
	ComponentHandle CasterHandle; // Saved
};

struct StatusHealSharingCaster : public StatusConsumeBase
{
	ComponentHandle StatusTargetHandle; // Saved
};

struct StatusExtraTurn : public StatusConsumeBase {};

struct StatusActiveDefense : public StatusConsumeBase
{
	int Charges; // Saved
	glm::vec3 TargetPos; // Saved
	ComponentHandle StatusTargetHandle; // Saved
	float Radius;
	FixedString Projectile;
	stats::Condition* TargetConditions;
	ObjectSet<void *> Targets; // ObjectSet<StatusActiveDefenseTargetData>
	ObjectSet<ComponentHandle> PreviousTargets; // Saved
};

struct StatusSpark : public StatusConsumeBase
{
	int Charges; // Saved
	float Radius;
	FixedString Projectile;
	stats::Condition* TargetConditions;
	ObjectSet<void*> SparkTargetData;
};

struct StatusPlayDead : public StatusConsumeBase {};

struct StatusConstrained : public StatusLying {};

struct StatusEffect : public Status {};

struct StatusDeactivated : public StatusConsumeBase {};

struct StatusTutorialBed : public Status {};

struct StatusMachine : public dse::StatusMachine<Status>
{
	using CreateStatusProc = Status * (StatusMachine* StatusMachine, FixedString const& StatusId, uint64_t ComponentHandle);
	using ApplyStatusProc = void(StatusMachine* StatusMachine, Status* Status);
	using UpdateProc = void(StatusMachine* StatusMachine, GameTime* time);
	using DeleteStatusByHandleProc = bool(StatusMachine* self, ComponentHandle* handle);

	esv::Status* GetServerStatus(ComponentHandle handle, bool returnUnapplied) const;
};

END_NS()

BEGIN_NS(ecl)

struct Status
{
	static constexpr auto ObjectTypeIndex = ObjectHandleType::ClientStatus;

	virtual void Destroy() = 0;
	virtual void SetOwnerHandle(ComponentHandle handle) = 0;
	virtual ComponentHandle GetOwnerHandle(ComponentHandle *) = 0;
	virtual StatusType GetStatusId() = 0;
	virtual unsigned int GetStatusType() = 0;
	virtual unsigned int BeforeAddStatus() = 0;
	virtual bool Enter() = 0;
	virtual bool Resume() = 0;
	virtual void Update() = 0;
	virtual uint32_t Tick(uint32_t * combatTeamId, float delta) = 0;
	virtual void Exit() = 0;
	virtual void OnEarlyDestroy() = 0;
	virtual void ApplyStatsId() = 0;
	virtual FixedString* GetIcon() = 0;
	virtual void field_70() = 0;
	virtual void GetDescriptionWithoutPrototype() = 0;
	virtual void GetEffectTexts(ObjectSet<STDWString> & texts) = 0;
	virtual void ConcatTooltipText(eoc::Text& text) = 0;
	virtual void ConcatDescription(eoc::Text& text) = 0;
	virtual FixedString* GetStatsId() = 0;
	virtual int GetCauseLevel() = 0;
	virtual void field_A8() = 0;
	virtual bool ShouldShowVisuals() = 0;
	virtual bool SyncData(ScratchBuffer& buffer) = 0;
	virtual void DestroyVisuals() = 0;
	virtual void RecreateVisuals() = 0;
	virtual void field_D0() = 0;
	virtual void OnSetRequestDelete() = 0;
	virtual void GetTooltipText(eoc::Text& text) = 0;
	virtual void FetchData() = 0;

	FixedString field_8;
	NetId NetID;
	ComponentHandle OwnerHandle;
	__int64 field_20;
	FixedString StatusId;
	int field_30;
	float LifeTime;
	float CurrentLifeTime;
	float StatsMultiplier;
	StatusFlags Flags;
	ComponentHandle StatusSourceHandle;
};

struct StatusMachine : public dse::StatusMachine<Status>
{};

END_NS()

BEGIN_SE()

struct PendingStatus
{
	esv::Status * Status;
	bool PreventApply;
};

class PendingStatuses
{
public:
	void Add(esv::Status * status);
	void Remove(esv::Status * status);
	PendingStatus * Find(ComponentHandle owner, ComponentHandle statusHandle);

private:
	struct StatusHasher
	{
		std::size_t operator() (const std::pair<ComponentHandle, ComponentHandle>& pair) const {
			return std::hash<ComponentHandle>()(pair.first) ^ std::hash<ComponentHandle>()(pair.second);
		}
	};

	std::unordered_map<std::pair<ComponentHandle, ComponentHandle>, PendingStatus, StatusHasher> statuses_;
};

END_SE()