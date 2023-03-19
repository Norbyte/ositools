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
	virtual void Finish() = 0;
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


struct OsirisTaskFactory
{
	using CreateTaskProc = Task * (OsirisTaskFactory* self, TaskType type, uint64_t characterHandle);
	using ReleaseTaskProc = void (OsirisTaskFactory* self, Task* task);

	Pool Pools[19];
};


struct TaskController : public BaseController
{
	Queue<Task*> Tasks;
	bool RemoveNextTask_M;
	bool UpdateInProgress;
	bool FlushRequested;

	void PurgeQueue();
	void FlushQueue();
	void QueueTask(Task* task);
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
	stats::SkillPrototype* Skill;
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

BEGIN_NS(ecl)

struct CursorControl : public ProtectedGameObject<CursorControl>
{
	void* InputEventListenerVMT;
	void* InputDeviceListenerVMT;
	bool CursorShown;
	bool HasTextDisplay;
	float HideTimer;
	float InputHideTimer;
	FixedString HWMouseCursor;
	uint32_t Flags;
	CursorType MouseCursor;
	uint32_t RequestedFlags;
	uint32_t field_34;
	ComponentHandle TextDisplayUIHandle;
};


struct CharacterTask
{
	virtual ~CharacterTask() {}
	virtual CharacterTask* CreateNew() = 0;
	virtual void RaiseFlags(CharacterTaskFlags flags) = 0;
	virtual void ClearFlags(CharacterTaskFlags flags) = 0;
	virtual void SetCharacter(Character* c) = 0;
	virtual void Start() = 0;
	virtual void Stop() = 0;
	virtual bool CanEnter() = 0;
	virtual int GetPriority(int previousPriority) = 0;
	virtual int GetExecutePriority(int previousPriority) = 0;
	virtual int GetActionCost() = 0;
	virtual int GetTotalAPCost() = 0;
	virtual uint64_t GetAPWarning() = 0;
	virtual uint64_t GetError() = 0;
	virtual void SetCursor() = 0;
	virtual void GetTaskInfo(eoc::Text& taskInfo, bool showAll) = 0;
	virtual WORD* HandleInputEvent(WORD& result, InputEvent* e) = 0;
	virtual void EnterPreview() = 0;
	virtual void UpdatePreview() = 0;
	virtual void ExitPreview() = 0;
	virtual bool IsInPreview() = 0;
	virtual bool GetAbilityAndFlags(uint32_t*) = 0;
	virtual float GetSightRange() = 0;
	virtual void ClearAIColliding() = 0;
	virtual void SetAIColliding() = 0;
	virtual int ValidateTargetRange() = 0;
	virtual bool Enter() = 0;
	virtual bool Update() = 0;
	virtual void Exit() = 0;
	virtual bool CanExit() = 0;
	virtual bool CanExit2() = 0;
	virtual void GetDescription(eoc::Text& desc) = 0;
	virtual bool SyncSurfaceCells(ObjectSet<SurfaceCell>*) = 0;
	virtual ComponentHandle* CreatePreviewEffect(ComponentHandle& pEffect) = 0;
	virtual void GetAPDescription(eoc::Text& desc) = 0;

	struct Context
	{
		glm::vec3 Position{.0f, .0f, .0f};
		int CurrentAP{0};
		int RemainingMoveDistance{0};
		ObjectSet<ComponentHandle> field_18;
	};

	Character *BoundCharacter{nullptr};
	int ActionTypeId{ 0 };
	CharacterTaskFlags Flags{ 0 };
	Context CurrentContext;
	Context SavedContext;
	bool IsInPreviewMode{false};
	bool RequestRun{false};
};


struct BaseController : public ProtectedGameObject<BaseController>
{
	void* VMT;
	int TypeId;
	ComponentHandle Character;
};


struct InputController : public BaseController
{
	bool IsActive;
	bool HasExited;
	float TimeElapsed;
	bool IsDragging;
	bool field_21;
	bool IsPicking;
	bool PickedNoObject;
	glm::vec2 MousePos;
	int field_2C;
	ObjectSet<CharacterTask*> TaskPrototypes;
	ComponentHandle DraggingObjectHandle;
	uint8_t Flags;
	CharacterTask* CurrentTask;
	CharacterTask* PreviewTask;
	ObjectSet<CharacterTask*> ChosenTasks;
	uint8_t TaskFlags;
};


struct StatusController : public BaseController
{
	ecl::StatusControllerFlags Flags;
	uint32_t SummoningTransactionId;
	uint32_t PolymorphingTransactionId;
	uint32_t KnockedDownTransactionId;
	uint32_t TeleportFallTransactionId;
	bool SetKnockedDown;
};

struct DialogController : public BaseController
{
	bool IsSteering;
};


END_NS()
