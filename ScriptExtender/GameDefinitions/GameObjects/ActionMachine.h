#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/Helpers.h>
#include <GameDefinitions/GameObjects/Effect.h>

BEGIN_SE()

template <class TActionState>
struct ActionMachineBase : public ProtectedGameObject<ActionMachineBase<TActionState>>
{
	using UpdateSyncStateProc = void(ActionMachineBase* self, uint8_t actionLayer, TActionState* actionState, bool force, bool setLayer);
	using SetStateProc = bool(ActionMachineBase* self, uint64_t actionLayer, TActionState* actionState, int* somePtr, bool force, bool setLayer);
	using ResetStateProc = bool(ActionMachineBase* self, bool force);
	
	struct Layer : public ProtectedGameObject<Layer>
	{
		TActionState* State;
		uint64_t Unknown;
		uint8_t Flags1;
		uint8_t Flags2;
		uint8_t Flags3;
	};

	ComponentHandle CharacterHandle;
	std::array<Layer, 3> Layers;
	bool IsEntering[4];
	uint16_t Unknown;
	TActionState* CachedActions[24];
};

END_SE()

BEGIN_NS(esv)

struct ActionState : public ProtectedGameObject<ActionState>
{
	virtual ~ActionState() = 0;
	virtual ActionStateType GetType() = 0;
	virtual bool Enter() = 0;
	virtual bool Continue() = 0;
	virtual void Update() = 0;
	virtual void Tick() = 0;
	virtual bool CanExit() = 0;
	virtual void Exit() = 0;
	virtual bool IsFinished() = 0;
	virtual void UNK_48() = 0;
	virtual void UNK_50() = 0;
	virtual void UNK_58() = 0;
	virtual void UNK_60() = 0;
	virtual bool GetSyncData(ScratchBuffer&) = 0;
	virtual bool SyncData(ScratchBuffer&) = 0;

	struct ActionMachine * Machine;
	uint8_t Unknown1;
	uint32_t TransactionId;

	LegacyPropertyMapBase * GetPropertyMap();
	char const * GetTypeName();
	ActionStateType LuaGetTypeId();
};

struct ASAttack : public ActionState
{
	ComponentHandle TargetHandle;
	glm::vec3 TargetPosition;
	ObjectSet<ComponentHandle> CleaveTargets;
	bool IsFinished;
	bool AlwaysHit;
	FixedString AttackAnimation;
	float TimeRemaining;
	bool AnimationFinished;
	int TotalHits;
	int TotalHitOffHand;
	int TotalShoots;
	int TotalShootsOffHand;
	int HitCount;
	int HitCountOffHand;
	int ShootCount;
	int ShootCountOffHand;
	ComponentHandle MainWeaponHandle;
	ComponentHandle OffWeaponHandle;
	RefMap<ComponentHandle, stats::HitDamageInfo> MainWeaponDamageList;
	RefMap<ComponentHandle, stats::HitDamageInfo> OffHandDamageList;
	RefMap<ComponentHandle, NumberDividers> MainWeaponNumberDividers;
	RefMap<ComponentHandle, NumberDividers> OffHandNumberDividers;
	ObjectSet<NumberDivider> RangedMainHandDamage;
	ObjectSet<NumberDivider> RangedOffHandDamage;
	__int64 field_118;
	__int64 field_120;
	stats::CriticalRoll MainHandHitType;
	stats::CriticalRoll OffHandHitType;
	ProjectileResult RangedMainHandHitObject;
	ProjectileResult RangedOffHandHitObject;
	bool ProjectileUsesHitObject;
	glm::vec3 ProjectileStartPosition;
	glm::vec3 ProjectileTargetPosition;
	bool DamageDurability;
	ObjectSet<ComponentHandle> DelayDeathCharacterHandles;
};


struct ASDie : public ActionState
{
	bool DieLogicPending;
	bool ExecutedDieLogic;
	bool DieActionsPending;
	bool PlayingDeathAnimation;
	ComponentHandle CauseeHandle;
};


struct ASHit : public ActionState
{
	bool AnimationFlag1;
	bool AnimationFlag2;
	bool AnimationFlag3;
	bool StillAnimation;
	bool ShouldResetState;
	ComponentHandle HitStatusHandle;
};


struct ASIdle : public ActionState
{};


struct ASAnimation : public ActionState
{
	FixedString Animation;
	float AnimationTimeAfterTick;
	float AnimationTime;
	float AnimationDuration;
	bool NullAnimation;
	bool IsCompleted;
	bool NoBlend;
	bool field_2F;
	bool ExitOnFinish;
};


struct ActionStateRequest : public ActionState
{
	uint8_t State;
	float RequestTimeRemaining;
	uint8_t field_20;
	uint32_t RequestId;
};


struct ASPickUp : public ActionStateRequest
{
	ComponentHandle ItemHandle;
	int16_t TargetItemSlot;
	int32_t Flags;
	NetId NetID;
	int32_t SplitAmount;
	bool ShouldGenerateTreasure;
	bool MovedItem;
	glm::vec3 StartPosition;
	glm::vec3 TargetPosition;
	bool IsFinished;
	float TimeRemaining;
};


struct ASDrop : public ActionState
{
	ComponentHandle ItemHandle;
	glm::vec3 TargetPosition;
	glm::mat3 TargetRotation;
	int32_t Amount;
	float TimeRemaining;
	bool IsFinished;
	bool field_59;
	bool field_5A;
};


struct ASMoveItem : public ActionStateRequest
{
	ObjectSet<ComponentHandle> Items;
	ObjectSet<glm::vec3> Positions;
	ObjectSet<glm::mat3> Rotations;
	ObjectSet<float> Timers;
	int32_t Amount;
	bool IsFinished;
	bool ItemMoveFinished;
};


struct ASPrepareSkill : public ActionState
{
	FixedString SkillId;
	FixedString PrepareAnimationInit;
	FixedString PrepareAnimationLoop;
	bool IsFinished;
	bool IsEntered;
	bool AnimApplyPending;
};

struct ASUseSkill : public ActionState
{
	SkillState* OriginalSkill;
	bool OwnsSkillStateOriginal;
	SkillState* Skill;
	bool OwnsSkillState;
};

struct ASKnockedDown : public ActionState
{
	int KnockedDownState;
	bool AnimationApplied;
};

struct ASSummoning : public ActionState
{
	FixedString field_18;
	bool IsFinished;
};

struct ASPolymorphing : public ActionState
{
	FixedString field_18;
	bool IsFinished;
};

struct ASUseItem : public ActionStateRequest
{
	ComponentHandle ItemHandle;
	float ActionTimeRemaining;
	float UseTimeRemaining;
	bool UseCompleted;
	bool IgnoreAP;
	bool RequestStarted;
	int32_t Slot;
};

struct ASCombineItem : public ActionStateRequest
{
	int CraftingStation;
	ObjectSet<ComponentHandle> Ingredients;
	FixedString Recipe;
	float AnimationTime;
	bool ConsumeAP;
	bool OpenUI;
	bool IsFinished;
};

struct ASTeleportFall : public ActionState
{
	float HitTimeRemaining;
	float AnimationTimeRemaining;
	bool IsFinished;
	bool IsFromItem;
	float Height;
	float Acceleration;
	bool HitApplied;
	FixedString SkillId;
	ComponentHandle OH;
};

struct ASSheath : public ActionState
{
	bool IsFinished;
};

struct ASUnsheath : public ActionState
{
	bool IsFinished;
};

struct ASIdentify : public ActionState
{
	ComponentHandle TargetItem;
	ComponentHandle SourceItem;
	float AnimationTimeRemaining;
	bool IsFinished;
};

struct ASRepair : public ActionState
{
	ComponentHandle TargetItem;
	ComponentHandle SourceItem;
	float AnimationTimeRemaining;
	bool IsFinished;
};

struct ASLockpick : public ActionStateRequest
{
	ComponentHandle TargetItem;
	ComponentHandle SourceItem;
	float AnimationTimeRemaining;
	bool IsFinished;
};

struct ASDisarmTrap : public ActionState
{
	ComponentHandle TargetItem;
	ComponentHandle SourceItem;
	bool IsFinished;
	float AnimationTimeRemaining;
};

struct ASIncapacitated : public ActionState
{
};

struct ASJumpFlight : public ActionState
{
	float field_18;
	float Animation1Duration;
	float Animation2Duration;
	bool Animation1Started;
	bool Animation2Started;
	bool IsFinished;
};


struct ActionMachine : public ActionMachineBase<esv::ActionState>
{};

END_NS()


BEGIN_NS(ecl)

struct ActionState : public ProtectedGameObject<ActionState>
{
	virtual ~ActionState() = 0;
	virtual bool Enter() = 0;
	virtual bool Continue() = 0;
	virtual void Update(GameTime const&) = 0;
	virtual void Tick(uint32_t* teamId) = 0;
	virtual bool CanExit() = 0;
	virtual void Exit() = 0;
	virtual void Fail() = 0;
	virtual bool IsFinished() = 0;
	virtual void UNK_58() = 0;
	virtual void UNK_60() = 0;
	virtual void UNK_68() = 0;
	virtual bool GetSyncData(ScratchBuffer&) = 0;
	virtual bool SyncData(ScratchBuffer&) = 0;
	virtual ActionStateType GetType() = 0;

	struct ActionMachine* Machine;
	uint8_t Unknown1;
	uint32_t TransactionId;

	char const* GetTypeName();
	ActionStateType LuaGetTypeId();
};

struct ActionMachine : public ActionMachineBase<ecl::ActionState>
{};


struct ASAttack : public ActionState
{
	ComponentHandle Target;
	glm::vec3 Position;
	glm::vec3 field_2C;
	bool Finished;
	FixedString UpperAnimation;
	ObjectSet<WeaponAnimData> WeaponAnimData;
	uint64_t NextWeaponAnimationIndex;
	float TimeElapsed;
	bool field_74;
	bool IsPositionalTarget;
	glm::vec3 HitPositionMainHand;
	glm::vec3 HitPositionOffHand;
	ProjectileResult ProjectileResultMainHand;
	ProjectileResult ProjectileResultOffHand;
	stats::CriticalRoll CriticalHitMainHand;
	stats::CriticalRoll CriticalHitOffHand;
};


struct ASDie : public ActionState
{
	float AnimationDuration;
	stats::DeathType DeathType;
	int AttackDirection;
	FixedString ExplodeResourceID;
	glm::vec3 ImpactDirection;
	bool SkipAnimation;
	bool field_3D;
	MultiEffectHandler EffectHandler;
};


struct ASHit : public ActionState
{
	bool IdleAnimation;
	bool BackstabOrFlankingAnimation;
	bool field_1A;
	bool HitIdle;
	bool Still;
	ComponentHandle HitStatus;
};


struct ASIdle : public ActionState
{};


struct ASAnimation : public ActionState
{
	FixedString Animation;
	float TimeElapsed;
	float AnimationDuration;
	bool ExitOnFinish;
	bool HasAnimation;
	bool IsFinished;
	bool NoBlend;
	bool WaitForLoad;
};


struct ASPrepareSkill : public ActionState
{
	FixedString SkillId;
	FixedString PrepareAnimationInit;
	FixedString PrepareAnimationLoop;
	FixedString PrepareEffect;
	MultiEffectHandler EffectHandler;
	ObjectSet<WeaponAnimData> WeaponAnimData;
	uint64_t WeaponAnimationIndex;
	float TimeElapsed;
	bool NeedsEffectInitialization;
	bool IsFinished;
};


struct ASUseSkill : public ActionState
{
	SkillState* CurrentSkill;
	bool OwnsCurrentSkill;
	SkillState* LastSkill;
	bool OwnsLastSkill;
};

END_NS()
