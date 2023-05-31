#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/Helpers.h>
#include <GameDefinitions/CommonTypes.h>

BEGIN_SE()

struct ZoneActionInfo
{
	int Shape;
	float Radius;
	float AngleOrBase;
	float BackStart;
	float FrontOffset;
	float MaxHeight;
};

END_NS()

BEGIN_NS(esv)

struct ProjectileTargetDesc
{
	ComponentHandle Target;
	glm::vec3 TargetPosition;
	glm::vec3 TargetPosition2;
};

struct SkillState : public ProtectedGameObject<SkillState>
{
	using DtorProc = void(SkillState* self);
	using GetTypeProc = SkillType(SkillState* self);
	using CheckTriggerProc = bool(SkillState* self, Trigger*);
	using InitProc = bool(SkillState* self);
	using EnterProc = bool(SkillState* self);
	using UpdateProc = bool(SkillState* self, GameTime const&);
	using TickProc = void(SkillState* self, int const& teamId);
	using CanExitProc = bool(SkillState* self);
	using ExitProc = void(SkillState* self);
	using IsFinishedProc = bool(SkillState* self);
	using EnterBehaviourProc = bool(SkillState* self);
	using ContinueBehaviourProc = bool(SkillState* self);
	using UpdateBehaviourProc = void(SkillState* self, GameTime const&);
	using TickBehaviourProc = void(SkillState* self, uint32_t* teamId, float timeDelta);
	using CanExitBehaviourProc = bool(SkillState* self);
	using ExitBehaviourProc = bool(SkillState* self);
	using IsFinishedBehaviourProc = bool(SkillState* self);
	using EnterActionProc = bool(SkillState* self);
	using ContinueActionProc = bool(SkillState* self);
	using UpdateActionProc = void(SkillState* self, GameTime const& time);
	using TickActionProc = void(SkillState* self);
	using CanExitActionProc = bool(SkillState* self);
	using ExitActionProc = void(SkillState* self);
	using IsFinishedActionProc = bool(SkillState* self);
	using GetSyncDataProc = bool(SkillState* self, ScratchBuffer&);
	using GetSyncDataUpdateProc = bool(SkillState* self, ScratchBuffer&);
	using SyncDataProc = bool(SkillState* self, ScratchBuffer&);
	using GetTargetDistanceProc = float(SkillState* self);
	using CloneProc = SkillState * (SkillState* self);
	using ResetProc = void(SkillState* self);
	using FinishProc = bool(SkillState* self);

	struct VMT
	{
		DtorProc* Dtor;
		GetTypeProc* GetType;
		CheckTriggerProc* CheckTrigger;
		InitProc* Init;
		EnterProc* Enter;
		UpdateProc* Update;
		TickProc* Tick;
		CanExitProc* CanExit;
		ExitProc* Exit;
		IsFinishedProc* IsFinished;
		EnterBehaviourProc* EnterBehaviour;
		ContinueBehaviourProc* ContinueBehaviour;
		UpdateBehaviourProc* UpdateBehaviour;
		TickBehaviourProc* TickBehaviour;
		CanExitBehaviourProc* CanExitBehaviour;
		ExitBehaviourProc* ExitBehaviour;
		IsFinishedBehaviourProc* IsFinishedBehaviour;
		EnterActionProc* EnterAction;
		ContinueActionProc* ContinueAction;
		UpdateActionProc* UpdateAction;
		TickActionProc* TickAction;
		CanExitActionProc* CanExitAction;
		ExitActionProc* ExitAction;
		IsFinishedActionProc* IsFinishedAction;
		GetSyncDataProc* GetSyncData;
		GetSyncDataUpdateProc* GetSyncDataUpdate;
		SyncDataProc* SyncData;
		GetTargetDistanceProc* GetTargetDistance;
		CloneProc* Clone;
		ResetProc* Reset;
		FinishProc* Finish;
	};

	virtual ~SkillState() = 0;
	virtual SkillType GetType() = 0;
	virtual bool CheckTrigger(Trigger*) = 0;
	virtual bool Init() = 0;
	virtual bool Enter() = 0;
	virtual bool Update(GameTime const&) = 0;
	virtual void Tick(int const& teamId) = 0;
	virtual bool CanExit() = 0;
	virtual void Exit() = 0;
	virtual bool IsFinished() = 0;
	virtual bool EnterBehaviour() = 0;
	virtual bool ContinueBehaviour() = 0;
	virtual void UpdateBehaviour(GameTime const&) = 0;
	virtual void TickBehaviour(uint32_t* teamId, float timeDelta) = 0;
	virtual bool CanExitBehaviour() = 0;
	virtual bool ExitBehaviour() = 0;
	virtual bool IsFinishedBehaviour() = 0;
	virtual bool EnterAction() = 0;
	virtual bool ContinueAction() = 0;
	virtual void UpdateAction(GameTime const& time) = 0;
	virtual void TickAction() = 0;
	virtual bool CanExitAction() = 0;
	virtual void ExitAction() = 0;
	virtual bool IsFinishedAction() = 0;
	virtual bool GetSyncData(ScratchBuffer&) = 0;
	virtual bool GetSyncDataUpdate(ScratchBuffer&) = 0;
	virtual bool SyncData(ScratchBuffer&) = 0;
	virtual float GetTargetDistance() = 0;
	virtual SkillState* Clone() = 0;
	virtual void Reset() = 0;
	virtual bool Finish() = 0;

	SkillType LuaGetType();

	uint8_t SomeHitFlag;
	eoc::SkillStateType State;
	FixedString SkillId;
	ComponentHandle CharacterHandle;
	ComponentHandle SourceItemHandle;
	bool ConsumeItem;
	bool ActionFinished;
	bool CastingFinished2;
	bool IgnoreChecks;
	bool IsStealthed;
	uint32_t BehaviourTransactionId;
	float ChargeTimerRemaining;
	float CombatCommentTextKeyOffset;
	float CombatCommentTextKeyOffsetRemaining;
	bool OwnsUseActionState;
	bool RequestExit;
	bool CastingFinished;
	FixedString CleanseStatuses;
	float StatusClearChance;
	bool CharacterHasSkill;
};

struct SkillStatePool
{
	SkillState* Prototype;
	int32_t PoolSize;
	Queue<SkillState*> Pool;
	ObjectSet<SkillState*> Set_pSkillState;
};

struct SkillStateFactory
{
	Array<SkillStatePool*> Pools;
};

struct SkillStateDome : public SkillState
{
	float AnimationTimeRemaining;
	float CastTextKeyTimeRemaining;
	float SelfCastTextKeyTimeRemaining;
	glm::vec3 Position;
};

struct SkillStateHeal : public SkillState
{
	ComponentHandle TargetHandle;
	float AnimationTimeRemaining;
	float CastTextKeyTimeRemaining;
};

struct SkillStateJump : public SkillState
{
	float NextTextKeyTimeRemaining;
	ObjectSet<float> TextKeys;
	float SelfCastTextKeyTimeRemaining;
	float CastTextKeyTimeRemaining;
	float AnimationTimeRemaining;
	bool CastEventThrown;
	glm::vec3 Position;
};

struct SkillStateMultiStrike : public SkillState
{
	ComponentHandle TargetHandle;
	float AnimationTimeRemaining;
	float CastTextKeyTimeRemaining;
	float SelfCastTextKeyTimeRemaining;
	float JumpDelay;
	glm::vec3 CasterPosition;
	glm::vec3 EndPosition;
	bool IsFinished;
	bool CastEventThrown;
	glm::vec3 CasterPosition2;
	ObjectSet<ComponentHandle> Targets;
};

struct SkillStatePath : public SkillState
{
	float CastTextKeyTimeRemaining;
	float AnimationTimeRemaining;
	glm::vec3 StartPosition;
	glm::vec3 UnknPosition;
	ObjectSet<glm::vec3> Path;
};

struct SkillStateProjectile : public SkillState
{
	struct Cast
	{
		float Delay;
		ComponentHandle TargetObjectHandle;
		glm::vec3 Position;
		int field_1C;
	};

	struct TargetData
	{
		glm::vec3 Position;
		glm::vec3 Position2;
		ComponentHandle TargetHandle;
		glm::vec3 StartPosition;
		glm::vec3 TargetPosition;
		ObjectSet<ProjectileResult> ProjectileResults;
		ObjectSet<Cast> Casts;
		uint64_t NumProjectiles;
		uint64_t NumProjectilesFired;
		ObjectSet<NumberDivider> DamageDividers;
	};


	ObjectSet<TargetData> Targets;
	float AnimationTimeRemaining;
	FixedString CastAnimation;
	float CastTimeRemaining;
	float SelfCastTextKeyTimeRemaining;
	float ProjectileTimeRemaining;
	float ProjectileDelay;
	uint64_t ProjectileCount;
	bool HasAngle;
	bool MovingObjectIsCaster;
	bool ThrowOnSkillCastEvent;
	int NumTotalCasts;
};

struct SkillStateProjectileStrike : public SkillState
{
	ComponentHandle NextTargetHandle;
	glm::vec3 NextTargetPosition;
	glm::vec3 SteeringTargetPosition;
	float AnimationTimeRemaining;
	glm::vec3 NextTargetPosition2;
	float CastTimeRemaining;
	float SelfCastTextKeyTimeRemaining;
	uint64_t ShotProjectileCount;
	uint64_t CurrentHitNum;
	float NextProjectileTimeRemaining;
	float ProjectileDelay;
	uint64_t ProjectileCount;
	ObjectSet<NumberDivider> DamageDividers;
	ObjectSet<ProjectileTargetDesc> Targets;
	ObjectSet<float> ProjectileTimers;
	bool NeedsToThrowSkillCastEvent;
};

struct SkillStateQuake : public SkillState
{
	float CastTextKeyTimeRemaining;
	float SelfCastTextKeyTimeRemaining;
	float AnimationTimeRemaining;
};

struct SkillStateRain : public SkillState
{
	float AnimationTimeRemaining;
	float CastTextKeyTimeRemaining;
	float SelfCastTextKeyTimeRemaining;
	glm::vec3 TargetPosition;
};


struct SkillStateRush : public SkillState
{
	ComponentHandle TargetHandle;
	glm::vec3 TargetPosition;
	float AnimationTimeRemaining;
	glm::vec3 DamageAreaCenter;
	float DamageAreaRadius;
	float CastTextKeyTimeRemaining;
	float SelfCastTextKeyTimeRemaining;
	glm::vec3 StartPosition;
	float StartTextKeyTimeRemaining;
	float StepSize;
	float StepTimeRemaining;
	float HitRadius;
	float StartTextKeyTime;
	float StopTextKeyTime;
	int SteeringTransactionId;
	ObjectSet<ComponentHandle> DamagedTargets;
	ComponentHandle SurfaceHandle;
};

struct SkillStateShout : public SkillState
{
	float CastTextKeyTimeRemaining;
	float SelfCastTextKeyTimeRemaining;
	float AnimationTimeRemaining;
	glm::vec3 Position;
	ObjectSet<ComponentHandle> ObjectHandleSet;
};

struct SkillStateStorm : public SkillState
{
	float AnimationTimeRemaining;
	float CastTextKeyTimeRemaining;
	glm::vec3 Position;
};

struct SkillStateSummon : public SkillState
{
	float AnimationTimeRemaining;
	float CastTextKeyTimeRemaining;
	float SelfCastTextKeyTimeRemaining;
	bool HasHighLevelSummoning;
	uint64_t SummonCount;
	ObjectSet<glm::vec3> SummonPositions;
};

struct SkillStateTarget : public SkillState
{
	ComponentHandle TargetHandle;
	glm::vec3 TargetPosition;
	float AnimationTimeRemaining;
	float CastTextKeyTimeRemaining;
	ObjectSet<float> TextEventTimers;
	float SelfCastTextKeyTimeRemaining;
	bool NeedsToThrowCastEvent;
	ComponentHandle IncreasedDelayDeathCountHandle;
};

struct SkillStateTeleportation : public SkillState
{
	glm::vec3 SourcePosition;
	ComponentHandle SourceHandle;
	glm::vec3 TargetPosition;
	ComponentHandle TargetHandle;
	float AnimationTimeRemaining;
	float CastTextKeyTimeRemaining;
	bool TeleportSurface;
};

struct SkillStateTornado : public SkillState
{
	float AnimationTimeRemaining;
	float CastTextKeyTimeRemaining;
	glm::vec3 Position;
};

struct SkillStateWall : public SkillState
{
	float AnimationTimeRemaining;
	float CastTextKeyTimeRemaining;
	float SelfCastTextKeyTimeRemaining;
	glm::vec3 StartPosition;
	glm::vec3 EndPosition;
};

struct SkillStateZone : public SkillState
{
	glm::vec3 TargetPosition;
	float AnimationTimeRemaining;
	float CastTextKeyTimeRemaining;
	ObjectSet<IEoCServerObject*> Targets;
	ObjectSet<ComponentHandle> GameActions;
};

END_NS()


BEGIN_NS(ecl)

struct PreviewEffectMap
{
	Map<FixedString, ObjectSet<ComponentHandle>> Effects;
	bool HasStarted;
};


struct SkillState : public ProtectedGameObject<SkillState>
{
	using GetCharacterPositionProc = bool (SkillState* self, glm::vec3&);
	using GetTargetHandleProc = ComponentHandle& (SkillState* self, ComponentHandle&);
	using DtorProc = void (SkillState* self);
	using GetTypeProc = SkillType (SkillState* self);
	using EnterProc = bool (SkillState* self);
	using ContinueProc = bool (SkillState* self);
	using UpdateProc = bool (SkillState* self);
	using TickProc = void (SkillState* self);
	using CanExitProc = bool (SkillState* self);
	using ExitProc = bool (SkillState* self);
	using IsFinishedProc = bool (SkillState* self);
	using OnInputEventProc = uint16_t* (SkillState* self, uint16_t* eventRetVal, InputEvent const& inputEvent);
	using EnterBehaviourProc = bool (SkillState* self);
	using ContinueBehaviourProc = bool (SkillState* self);
	using UpdateBehaviourProc = bool (SkillState* self);
	using TickBehaviourProc = void (SkillState* self, uint32_t* teamId, float timeDelta);
	using CanExitBehaviourProc = bool (SkillState* self);
	using ExitBehaviourProc = bool (SkillState* self);
	using IsFinishedBehaviourProc = bool (SkillState* self);
	using EnterActionProc = bool (SkillState* self);
	using ContinueActionProc = bool (SkillState* self);
	using UpdateActionProc = bool (SkillState* self, GameTime const& time);
	using TickActionProc = void (SkillState* self);
	using CanExitActionProc = bool (SkillState* self);
	using ExitActionProc = bool (SkillState* self);
	using IsFinishedActionProc = bool (SkillState* self);
	using GetSyncDataProc = bool (SkillState* self, ScratchBuffer&);
	using SyncDataProc = bool (SkillState* self, ScratchBuffer&);
	using SyncDataUpdateProc = bool (SkillState* self, ScratchBuffer&);
	using GetTargetProc = bool (SkillState* self, ComponentHandle& target, glm::vec3& targetPosition);
	using GetTargetPositionProc = bool (SkillState* self, glm::vec3& targetPosition);
	using GetTargetMoveDistanceProc = float (SkillState* self);
	using GetTargetRadiusProc = float (SkillState* self);
	using ClearAICollidingProc = void (SkillState* self);
	using SetAICollidingProc = void (SkillState* self);
	using ValidateTargetSightProc = int32_t (SkillState* self, glm::vec3 const& target);
	using ValidateTargetProc = uint32_t (SkillState* self, uint64_t targetHandle, glm::vec3 const* targetPos, bool snapToGrid, bool fillInHeight);
	using GetErrorReasonProc = uint32_t (SkillState* self);
	using GetAbilityAndFlagsCheckProc = bool (SkillState* self);
	using GetNameTranslatedStringProc = TranslatedString* (SkillState* self, TranslatedString&);
	using GetActionDescriptionProc = eoc::Text* (SkillState* self, eoc::Text&);
	using DestroyProjectileEffectsProc = void (SkillState* self);
	using Unknown_150Proc = void (SkillState* self);
	using SyncSurfaceCellsProc = bool (SkillState* self);
	using CanExitCharacterTaskProc = bool (SkillState* self);
	using FinishSkillStateProc = void (SkillState* self);
	using CloneProc = SkillState* (SkillState* self);
	using ResetProc = void (SkillState* self);
	using ClearHighlightsProc = void (SkillState* self);
	using AddTargetProc = void (SkillState* self, IEoCClientObject* target);
	using RemoveTargetProc = void (SkillState* self, IEoCClientObject* target);
	using AddTarget2Proc = void (SkillState* self, IEoCClientObject* target);
	using RemoveTarget2Proc = void (SkillState* self, IEoCClientObject* target);
	using HighlightPickTargetProc = void (SkillState* self, stats::Condition* conditions, bool highlightCharacters, bool highlightItems);
	using PlayCastEffectProc = void (SkillState* self, glm::vec3 const& position);

	struct VMT
	{
		GetCharacterPositionProc* GetCharacterPosition;
		GetTargetHandleProc* GetTargetHandle;
		DtorProc* Dtor;
		GetTypeProc* GetType;
		EnterProc* Enter;
		ContinueProc* Continue;
		UpdateProc* Update;
		TickProc* Tick;
		CanExitProc* CanExit;
		ExitProc* Exit;
		IsFinishedProc* IsFinished;
		OnInputEventProc* OnInputEvent;
		EnterBehaviourProc* EnterBehaviour;
		ContinueBehaviourProc* ContinueBehaviour;
		UpdateBehaviourProc* UpdateBehaviour;
		TickBehaviourProc* TickBehaviour;
		CanExitBehaviourProc* CanExitBehaviour;
		ExitBehaviourProc* ExitBehaviour;
		IsFinishedBehaviourProc* IsFinishedBehaviour;
		EnterActionProc* EnterAction;
		ContinueActionProc* ContinueAction;
		UpdateActionProc* UpdateAction;
		TickActionProc* TickAction;
		CanExitActionProc* CanExitAction;
		ExitActionProc* ExitAction;
		IsFinishedActionProc* IsFinishedAction;
		GetSyncDataProc* GetSyncData;
		SyncDataProc* SyncData;
		SyncDataUpdateProc* SyncDataUpdate;
		GetTargetProc* GetTarget;
		GetTargetPositionProc* GetTargetPosition;
		GetTargetMoveDistanceProc* GetTargetMoveDistance;
		GetTargetRadiusProc* GetTargetRadius;
		ClearAICollidingProc* ClearAIColliding;
		SetAICollidingProc* SetAIColliding;
		ValidateTargetSightProc* ValidateTargetSight;
		ValidateTargetProc* ValidateTarget;
		GetErrorReasonProc* GetErrorReason;
		GetAbilityAndFlagsCheckProc* GetAbilityAndFlagsCheck;
		GetNameTranslatedStringProc* GetNameTranslatedString;
		GetActionDescriptionProc* GetActionDescription;
		DestroyProjectileEffectsProc* DestroyProjectileEffects;
		Unknown_150Proc* Unknown_150;
		SyncSurfaceCellsProc* SyncSurfaceCells;
		CanExitCharacterTaskProc* CanExitCharacterTask;
		FinishSkillStateProc* FinishSkillState;
		CloneProc* Clone;
		ResetProc* Reset;
		ClearHighlightsProc* ClearHighlights;
		AddTargetProc* AddTarget;
		RemoveTargetProc* RemoveTarget;
		AddTarget2Proc* AddTarget2;
		RemoveTarget2Proc* RemoveTarget2;
		HighlightPickTargetProc* HighlightPickTarget;
		PlayCastEffectProc* PlayCastEffect;
	};

	virtual bool GetCharacterPosition(glm::vec3&) = 0;
	virtual ComponentHandle& GetTargetHandle(ComponentHandle&) = 0;
	virtual ~SkillState() = 0;
	virtual SkillType GetType() = 0;
	virtual bool Enter() = 0;
	virtual bool Continue() = 0;
	virtual bool Update() = 0;
	virtual void Tick() = 0;
	virtual bool CanExit() = 0;
	virtual bool Exit() = 0;
	virtual bool IsFinished() = 0;
	virtual uint16_t* OnInputEvent(uint16_t* eventRetVal, InputEvent const& inputEvent) = 0;
	virtual bool EnterBehaviour() = 0;
	virtual bool ContinueBehaviour() = 0;
	virtual bool UpdateBehaviour() = 0;
	virtual void TickBehaviour(uint32_t* teamId, float timeDelta) = 0;
	virtual bool CanExitBehaviour() = 0;
	virtual bool ExitBehaviour() = 0;
	virtual bool IsFinishedBehaviour() = 0;
	virtual bool EnterAction() = 0;
	virtual bool ContinueAction() = 0;
	virtual bool UpdateAction(GameTime const& time) = 0;
	virtual void TickAction() = 0;
	virtual bool CanExitAction() = 0;
	virtual bool ExitAction() = 0;
	virtual bool IsFinishedAction() = 0;
	virtual bool GetSyncData(ScratchBuffer&) = 0;
	virtual bool SyncData(ScratchBuffer&) = 0;
	virtual bool SyncDataUpdate(ScratchBuffer&) = 0;
	virtual bool GetTarget(ComponentHandle& target, glm::vec3& targetPosition) = 0;
	virtual bool GetTargetPosition(glm::vec3& targetPosition) = 0;
	virtual float GetTargetMoveDistance() = 0;
	virtual float GetTargetRadius() = 0;
	virtual void ClearAIColliding() = 0;
	virtual void SetAIColliding() = 0;
	virtual bool ValidateTargetSight(glm::vec3 const& target) = 0;
	virtual uint32_t ValidateTarget(uint64_t targetHandle, glm::vec3 const* targetPos, bool, bool) = 0;
	virtual uint32_t GetErrorReason() = 0;
	virtual bool GetAbilityAndFlagsCheck() = 0;
	virtual TranslatedString* GetNameTranslatedString(TranslatedString&) = 0;
	virtual eoc::Text* GetActionDescription(eoc::Text&) = 0;
	virtual void DestroyProjectileEffects() = 0;
	virtual void Unknown_150() = 0;
	virtual bool SyncSurfaceCells() = 0;
	virtual bool CanExitCharacterTask() = 0;
	virtual void FinishSkillState() = 0;
	virtual SkillState* Clone() = 0;
	virtual void Reset() = 0;
	virtual void ClearHighlights() = 0;
	virtual void AddTarget(IEoCClientObject* target) = 0;
	virtual void RemoveTarget(IEoCClientObject* target) = 0;
	virtual void AddTarget2(IEoCClientObject* target) = 0;
	virtual void RemoveTarget2(IEoCClientObject* target) = 0;
	virtual void HighlightPickTarget(stats::Condition* conditions, bool highlightCharacters, bool highlightItems) = 0;
	virtual void PlayCastEffect(glm::vec3 const& position) = 0;

	SkillType LuaGetType();

	ObjectSet<ComponentHandle> HighlightedCharacters;
	ObjectSet<ComponentHandle> HighlightedItems;
	MultiEffectHandler* SkillEffect;
	ObjectSet<MultiEffectHandler*> EffectHandlers;
	ObjectSet<BeamEffectHandler*> BeamEffects;
	ObjectSet<WeaponAnimData> WeaponAnimData;
	uint64_t NextWeaponAnimationIndex;
	float TimeElapsed;
	eoc::SkillStateType State;
	FixedString SkillId;
	ComponentHandle CharacterHandle;
	ComponentHandle ItemHandle;
	bool IsFromItem;
	uint32_t ActionMachineTransactionId;
	float ChargeDuration;
	bool OwnsActionState;
	bool CastingFinished;
};

struct SkillStatePool
{
	SkillState* Prototype;
	int32_t Size;
	Queue<SkillState*> Pool;
	ObjectSet<SkillState*> OS_pSkillState;
};

struct SkillStateFactory
{
	void* VMT;
	CRITICAL_SECTION CriticalSection;
	Array<SkillStatePool*> Pools;
};


struct SkillStatePath : public SkillState
{
	uint8_t TargetSelectionState;
	glm::vec3 TargetPosition;
	glm::vec3 SelectedPosition;
	glm::vec3 CastEffectPosition;
	ObjectSet<ComponentHandle> WaypointEffectHandles;
	ObjectSet<ComponentHandle> WaypointEffectHandles2;
	ObjectSet<float> TextKeys;
	Effect* TargetingEffect;
	float AnimationTimeRemaining;
	float CastTextKeyTimeRemaining;
	float CastEffectTextKeyTimeRemaining;
	float MaxDistance2;
	float PickingDistance;
	float MaxDistance;
};


struct SkillStateTarget : public SkillState
{
	bool CanTargetCharacters;
	bool CanTargetItems;
	bool CanTargetTerrain;
	bool IsTargeting;
	glm::vec3 Position;
	ComponentHandle Target;
	ComponentHandle InfluenceRadiusEffect;
	ComponentHandle TargetingShapeEffect;
	float CastSelfAnimationDuration;
	float TargetRadius;
	float AreaRadius;
	float CastTextKeyTimeRemaining;
	float CastEndTextKeyTimeRemaining;
	float AnimationTimeRemaining;
	ObjectSet<float> CastTextKeyOffsets;
	stats::Condition* TargetConditions;
	stats::Condition* AoEConditions;
	ObjectSet<SurfaceTransformActionType> SurfaceTransformActions;
	int AmountOfTargets;
	int Autocast;
	PrimitiveSmallSet<SurfaceCell> SurfaceCells;
	PrimitiveSmallSet<SurfaceCell> SurfaceCells2;
	uint32_t MultiEffectIndex1;
	uint32_t MultiEffectIndex2;
	uint32_t MultiEffectIndex3;
	bool HasValidTarget_M;
};


struct SkillStateTeleportation : public SkillState
{
	ComponentHandle SourceTargetingEffect;
	ComponentHandle TargetTargetingEffect;
	ComponentHandle PathTargetingShapeEffect;
	MultiEffectHandler* EffectHandler2;
	glm::vec3 DisappearPosition;
	ComponentHandle Target;
	glm::vec3 Position;
	ComponentHandle Highlight;
	int TargetingState;
	float AnimationTimeRemaining;
	float CastTextKeyTimeRemaining;
	float TargetRadius;
	float AreaRadius;
	bool ForceMove;
	int field_150;
	eoc::Text Description;
	bool TeleportSelf;
	bool TeleportSurface;
	bool CanTargetCharacters;
	bool CanTargetItems;
	bool CanTargetTerrain;
	bool TickUI;
	stats::Condition* TargetConditions;
	PathPreviewer* PathPreviewer;
};


struct SkillStateProjectileStrike : public SkillState
{
	struct Cast
	{
		ComponentHandle Target;
		ComponentHandle InfluenceRadiusEffect;
		ComponentHandle TargetPreviewEffect;
		eoc::PathMover* PathMover;
		Visual* Visual;
		glm::vec3 Position;
	};


	bool CanTargetCharacters;
	bool CanTargetItems;
	bool CanTargetTerrain;
	glm::vec3 TargetPosition;
	ComponentHandle Target;
	bool IsTargeting;
	ComponentHandle TargetingPreviewEffect;
	ComponentHandle InfluenceRadiusEffect;
	float HighlightRadius;
	float AngleTan;
	float Height;
	float field_124;
	Array<Visual*> Visuals;
	Array<eoc::PathMover*> PathMovers;
	ObjectSet<ComponentHandle> EffectHandles3;
	float AngleTan2;
	float Height2;
	float ZWithHeight;
	glm::vec3 Position2;
	ObjectSet<glm::vec3> PathPreview;
	uint32_t field_1C0;
	uint32_t ProjectileCount;
	float PathSpeed;
	float Acceleration;
	uint8_t field_1D0;
	ObjectSet<ComponentHandle> Targets;
	int TargetRadiusSquare;
	float AnimationTimeRemaining;
	float TargetRadius;
	float ExplodeRadius;
	float AreaRadius;
	float Angle;
	float CastTextKeyTimeRemaining;
	stats::Condition* TargetConditions;
	int StrikeCount;
	ObjectSet<float> TextKeys;
	bool field_248;
	bool NeedsHitVisualUpdate;
	ObjectSet<Cast> Casts;
};


struct SkillStateSummon : public SkillState
{
	struct SummonData
	{
		glm::vec3 Position;
		int field_C;
		Effect* Effect;
	};

	FixedString TargetEffect;
	FixedString Template;
	FixedString TemplateOverride;
	FixedString TemplateAdvanced;
	bool IsAdvancedSummon;
	uint64_t SummonCount;
	uint64_t NextSummonIndex;
	bool IsTargeting;
	float CastTextKeyTimeRemaining;
	float CastEffectTextKeyTimeRemaining;
	float AnimationTimeRemaining;
	stats::Condition* AoEConditions;
	float AreaRadius;
	ComponentHandle EffectHandle;
	float TargetRadius;
	uint32_t TargetValidityFlags;
	ObjectSet<SummonData> Summons;
};


struct SkillStateRain : public SkillState
{
	bool HasTargetingEffect;
	glm::vec3 Position;
	Effect* InfluenceRadiusEffect;
	float AnimationTimeRemaining;
	float CastTextKeyTimeRemaining;
	float CastEffectTextKeyTimeRemaining;
	stats::Condition* TargetConditions;
	Effect* TargetingEffect;
	float TargetRadius;
	float AreaRadius;
};


struct SkillStateZone : public SkillState
{
	glm::vec3 Position;
	bool IsTargeting;
	float AnimationTimeRemaining;
	float CastTextKeyTimeRemaining;
	float CastEffectTextKeyTimeRemaining;
	ZoneActionInfo ActionInfo;
	PrimitiveSmallSet<SurfaceCell> SurfaceCells;
	bool CanTargetCharacters;
	bool CanTargetItems;
	stats::Condition* TargetConditions;
	float PushDistance;
	Map<ComponentHandle, PathPreviewer*> PathPreviewers;
};


struct SkillStateHeal : public SkillState
{
	ComponentHandle Highlight;
	bool IsTargeting;
	uint8_t field_F1;
	float AnimationTimeRemaining;
	float CastTextKeyTimeRemaining;
	float CastEffectTextKeyTimeRemaining;
	float TargetRadius;
	stats::Condition* TargetConditions;
};


struct SkillStateProjectile : public SkillState
{
	struct Cast
	{
		float TextKeyOffset;
		ComponentHandle TargetHandle;
		glm::vec3 TargetPosition;
	};

	struct TargetData
	{
		glm::vec3 CasterPosition2;
		glm::vec3 TargetPosition2;
		glm::vec3 TargetPosition;
		ComponentHandle TargetHandle;
		bool SomeDistanceFlag;
		bool StillTooFar;
		bool field_32;
		ComponentHandle PreviewEffect;
		ComponentHandle InfluenceRadiusEffect;
		ObjectSet<PathPreviewer*> PathPreviews;
		ObjectSet<ComponentHandle> ProjectileTargetEffects;
		ObjectSet<ProjectileTemplate*> ProjectileTemplates;
		glm::vec3 CasterPosition;
		glm::vec3 TargetPosition3;
		ObjectSet<ProjectileResult> Results;
		ObjectSet<ComponentHandle> Targets;
		ObjectSet<Cast> Casts;
	};

	bool CanTargetCharacters;
	bool CanTargetItems;
	bool CanTargetTerrain;
	bool MovingCaster;
	bool TargetingObject;
	bool IsTargeting;
	bool CasterMovingObject;
	bool AutoAim;
	FixedString CastAnimation;
	int32_t AmountOfTargets;
	int32_t NextProjectileTargetIndex;
	int32_t ProjectileCount;
	float ProjectileTerrainOffset;
	float TargetRadiusSquare;
	float AnimationTimeRemaining;
	float TargetRadius;
	float ExplodeRadius;
	float AreaRadius;
	int32_t Angle;
	float NextProjectileTimeRemaining;
	stats::Condition* TargetConditions;
	uint8_t field_130;
	eoc::Text AimStatusText;
	ObjectSet<TargetData> Targets;
};


struct SkillStateWall : public SkillState
{
	uint32_t TargetingState;
	glm::vec3 Position;
	glm::vec3 EndPosition;
	float AnimationTimeRemaining;
	float CastTextKeyTimeRemaining;
	float CastEffectTextKeyTimeRemaining;
	float EffectDistance;
	int NumEffects;
	Effect* SourceTargetEffect;
	Effect* TargetTargetEffect;
	float TargetRadius;
	float MaxDistance;
	ObjectSet<Effect*> WallEffects;
	uint32_t TargetRangeValidation;
};


struct SkillStateJump : public SkillState
{
	bool IsFinished;
	ComponentHandle TargetingShapeEffect;
	glm::vec3 Position;
	float AnimationTimeRemaining;
	float TeleportFirstTextKeyTimeRemaining;
	float TeleportTextKeyTimeRemaining;
	ObjectSet<float> TeleportTextKeys;
	FixedString field_130;
	stats::Condition* TargetConditions;
	ComponentHandle InfluenceRadiusEffectTarget;
	ComponentHandle InfluenceRadiusEffectSource;
	float TargetRadius;
	float HitRadius;
	uint32_t TargetingState;
	bool DamageOnJump;
	bool DamageOnLanding;
};


struct SkillStateQuake : public SkillState
{
	uint8_t TargetingState;
	float AnimationTimeRemaining;
	float AreaRadius;
	float CastTextKeyTimeRemaining;
	float CastEffectTextKeyTimeRemaining;
	stats::Condition* TargetConditions;
};


struct SkillStateMultiStrike : public SkillState
{
	ComponentHandle HoverCharacter;
	uint32_t TargetValidationResult;
	bool IsTargeting;
	float CastTextKeyTimeRemaining;
	float CastEffectTextKeyTimeRemaining;
	float ReappearTextKeyTimeRemaining;
	glm::vec3 EndPosition;
	glm::vec3 StartPosition;
	stats::Condition* TargetConditions;
	bool ActionExited;
	char field_129;
	ObjectSet<STDString> Effects;
	int NextEffectTextIndex;
	Visual* Visual;
	PreviewEffectMap PreviewEffects;
	ObjectSet<ComponentHandle> Targets;
	float TargetRadius;
	float JumpDelay;
};


struct SkillStateStorm : public SkillState
{
	bool IsTargeting;
	glm::vec3 Position;
	float AnimationTimeRemaining;
	float CastTextKeyTimeRemaining;
	float CastEffectTextKeyTimeRemaining;
	stats::Condition* TargetConditions;
	Effect* Effect1;
	float AreaRadius;
	float TargetRadius;
	Effect* InfluenceRadiusEffect;
	uint32_t TargetingState;
};


struct SkillStateTornado : public SkillState
{
	bool IsTargeting;
	glm::vec3 Position;
	float AnimationTimeRemaining;
	float CastTextKeyTimeRemaining;
	float CastEffectTextKeyTimeRemaining;
	stats::Condition* TargetConditions;
	Effect* Effect;
	float TargetRadius;
};


struct SkillStateShout : public SkillState
{
	uint8_t TargetingState;
	glm::vec3 Position;
	bool CanTargetCharacters;
	bool CanTargetItems;
	bool IgnoreVisionBlock;
	float AnimationTimeRemaining;
	float CastTextKeyTimeRemaining;
	float CastEffectTextKeyTimeRemaining;
	stats::Condition* TargetConditions;
	float AreaRadius;
	PrimitiveSmallSet<float> TextKeyOffsets;
	__int64 field_130;
	float PushDistance;
	Map<ComponentHandle, PathPreviewer*> PathPreviewers;
	ObjectSet<ComponentHandle> Targets;
	stats::PropertyList* SkillProperties;
};


struct SkillStateRush : public SkillState
{
	bool CanTargetCharacters;
	bool CanTargetItems;
	bool CanTargetTerrain;
	glm::vec3 Position;
	glm::vec3 TargetPosition;
	ComponentHandle TargetHandle;
	bool IsTargeting;
	Effect* TargetingShape;
	Effect* TargetingCircle;
	Effect* TargetingCross;
	float AnimationTimeRemaining;
	float TargetRadius;
	float CharacterRadius;
	uint32_t OverlayTypeId;
	float HitRadius;
	float CastTextKeyTimeRemaining;
	float CastEffectTextKeyTimeRemaining;
	glm::vec3 TargetEntityPosition;
	float StartTextKeyTimeRemaining;
	float NumDistanceSteps;
	float SomeDistanceStep2;
	float SomeDistanceStep;
	bool MaterialParametersChanged;
	bool ActiveOnServer;
	stats::Condition* TargetConditions;
	ObjectSet<ComponentHandle> OS_OH;
};


struct SkillStateDome : public SkillState
{
	uint8_t TargetingState;
	glm::vec3 Position;
	Effect* InfluenceRadiusEffect;
	float AnimationTimeRemaining;
	float CastTextKeyTimeRemaining;
	float CastEffectTextKeyTimeRemaining;
	stats::Condition* TargetConditions;
	Effect* TargetEffect;
	float TargetRadius;
	float AreaRadius;
	uint32_t TargetRangeValidation;
};

END_NS()
