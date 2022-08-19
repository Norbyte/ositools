#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/TurnManager.h>

BEGIN_NS(esv)

struct BaseController : public ProtectedGameObject<BaseController>
{
	virtual ~BaseController() = 0;
	virtual void Update() = 0;
	virtual void UpdateCombat(eoc::CombatTeamId& teamId) = 0;
	virtual uint64_t GetPriority(bool&) = 0;
	virtual uint64_t Unkn18() = 0;
	virtual uint64_t Unkn20() = 0;
	virtual uint64_t Unkn28() = 0;
	virtual char const* GetName() = 0;
	virtual uint64_t Unkn38() = 0;

	int TypeId;
	ComponentHandle Character;
};


struct StatusController : public BaseController
{
	StatusControllerFlags Flags;
	int SummoningTransactionId;
	int PolymorphingTransactionId;
	int ActionTransactionId;
	int TeleportFallingTransactionId;
	int DeathAnimationTransactionId;
	int SteerToEnemyTransactionId;
	int field_34;
	uint32_t Flags2;
	glm::vec3 CombatStartPosition;
	bool KnockDownQueued;
	bool CombatStartPositionFloodDone;
	FixedString ResurrectedEvent;
};

struct Task
{
	virtual ~Task() = 0;
	virtual bool Update(GameTime const& t) = 0;
	virtual void SyncData(void* buf) = 0;
	virtual void SavegameVisit(ObjectVisitor& visitor) = 0;
	virtual void OnStart(GameTime const& t) = 0;
	virtual void Exit() = 0;
	virtual void OnFinished() = 0;
	virtual uint64_t Ret0() = 0;
	virtual void RequestExit() = 0;
	virtual uint64_t Ret02() = 0;
	virtual void* CreateBehavior() = 0;
	virtual bool GetTarget(uint64_t targetHandle, float& minDistance, float& maxDistance, eoc::Ai** ai, int& targetCheckType) = 0;

	ComponentHandle Character;
	TaskType TaskTypeId;
	uint32_t TaskState;
	bool Failed;
	uint32_t Flags;
};


struct TaskController : public BaseController
{
	Queue<Task*> Tasks;
	bool RemoveNextTask_M;
	bool UpdateInProgress;
	bool FlushRequested;
};


struct OsirisTeleportToLocationTask : public Task
{
	glm::vec3 Position;
	int field_2C;
	FixedString PreviousLevel;
	FixedString Level;
	bool SetRotation;
	bool LeaveCombat;
	bool FindPosition;
	bool FindFleePosition;
	bool Executed;
	bool field_45;
	bool UnchainFollowers;
	glm::mat3 Rotation;
	STDString ArriveEvent;
};


struct MoveTask : public Task
{
	uint32_t MoveTransactionId;
	glm::vec3 CurrentTarget;
	STDString ArriveEvent;
	ObjectSet<eoc::AiPathCheckpoint> Checkpoints;
	float TimeSpent;
	float TimeTargetFound;
};


struct OsirisMoveToObjectTask : public MoveTask
{
	float MinDistance;
	float MaxDistance;
	ComponentHandle Target;
	float SpeedMultiplier;
	float DefaultSpeed;
	bool IncreaseSpeed;
};


struct OsirisMoveInRangeTask : public MoveTask
{
	ComponentHandle Target;
	glm::vec3 TargetPos;
	float MinRange;
	float WantedRange;
	float MaxRange;
	glm::vec3 CachedTargetPos;
	glm::vec3 CachedTarget;
	float CachedCloseEnough;
	bool CachedResult;
	bool MustBeInTrigger;
	bool FallbackMoveCloser;
	bool AttackMove;
	ObjectSet<AIHintAreaTrigger*> HintTriggers;
	GameObjectTemplate* ProjectileTemplate;
	SkillPrototype* Skill;
};


struct OsirisMoveToLocationTask : public MoveTask
{
  float TargetRotation;
  float MinDistance;
  float MaxDistance;
  ComponentHandle TargetToIgnore;
  bool TargetRotationSet;
};


struct OsirisFleeTask : public MoveTask
{
	int FleeFromRelation;
	float FleeFromRelationRange;
	glm::vec3 StartPosition;
	// TODO - AiFlags?
	uint64_t FleeFromTileStates;
	ObjectSet<SurfacePathInfluence> SurfacePathInfluences;
	bool OutOfSight;
	bool StartedMoving;
	uint64_t FindFleePositionFlood[16];
};


struct OsirisPlayAnimationTask : public Task
{
  FixedString OriginalAnimation;
  FixedString Animation;
  FixedString EndAnimation;
  STDString FinishedEvent;
  bool WaitForCompletion;
  bool ExitOnFinish;
  bool NoBlend;
  uint32_t ActionTransactionId;
  float Time;
  float Timer;
  float AnimationDuration;
  ObjectSet<FixedString> AnimationNames;
  float CurrentTime;
};


struct OsirisAttackTask : public Task
{
	ComponentHandle Target;
	glm::vec3 TargetPos;
	STDString ArriveEvent;
	uint32_t BehaviorTransactionId;
	bool WithoutMove;
	bool AlwaysHit;
};


struct OsirisUseSkillTask : public Task
{
	FixedString Skill;
	bool Force;
	ComponentHandle Target;
	glm::vec3 TargetPos;
	uint32_t BehaviorTransactionId;
	bool Success;
	bool IgnoreHasSkill;
	bool IgnoreChecks;
};


struct OsirisAppearTask : public Task
{
	ComponentHandle Target;
	glm::vec3 TargetPos;
	float Angle;
	uint32_t SpawnState;
	bool PlayerSpawn;
	bool OutOfSight;
	bool OnTrail;
	STDString FinishedEvent;
	FixedString Animation;
};


struct OsirisDisappearTask : public Task
{
	ComponentHandle Target;
	float Angle;
	bool OutOfSight;
	bool OffStage;
	bool Running;
	bool ValidTarget;
	glm::vec3 TargetPos;
	STDString FinishedEvent;
	float SpeedMultiplier;
	float DefaultSpeed;
	bool IncreaseSpeed;
	int DisappearCount;
};


struct OsirisFollowNPCTask : public Task
{
	ComponentHandle Target;
};


struct OsirisWanderTask : public Task
{
	float Range;
	ComponentHandle Trigger;
	ComponentHandle Anchor;
	float Duration;
	bool Running;
	glm::vec3 Start;
};


struct OsirisSteerTask : public Task
{
	ComponentHandle Target;
	glm::vec3 TargetPos;
	float AngleTolerance;
	bool LookAt;
	bool SnapToTarget;
	uint32_t SteeringTransactionId;
};


struct OsirisDropTask : public Task
{
	ComponentHandle Item;
	glm::vec3 TargetPos;
};


struct OsirisPickupItemTask : public Task
{
	ComponentHandle Item;
	STDString ArriveEvent;
	uint32_t BehaviorTransactionId;
};


struct OsirisUseItemTask : public Task
{
	ComponentHandle Item;
	STDString ArriveEvent;
	uint32_t BehaviorTransactionId;
};


struct OsirisMoveItemTask : public Task
{
	ComponentHandle Item;
	glm::vec3 Position;
	int Amount;
	STDString ArriveEvent;
	uint32_t BehaviorTransactionId;
};


struct OsirisResurrectTask : public Task
{
	int HPPercentage;
	bool IsResurrected;
	FixedString Animation;
};


struct OsirisMoveToAndTalkTask : public Task
{
	ComponentHandle Target;
	uint32_t BehaviorTransactionId;
	FixedString Movement;
	FixedString DialogInstanceID;
	bool IsAutomatedDialog;
	float Timeout;
};


END_NS()
