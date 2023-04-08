#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/GameObjects/Ai.h>

BEGIN_NS(esv)

struct MovementMachine;

struct MovementState : public ProtectedGameObject<MovementState>
{
	virtual ~MovementState() = 0;
	virtual bool Enter() = 0;
	virtual bool Continue() = 0;
	virtual void Update(GameTime const& t) = 0;
	virtual uint32_t Unknown1() = 0;
	virtual void Exit() = 0;
	virtual bool IsFinished() = 0;
	virtual MovementStateType GetTypeId() = 0;

	MovementStateType LuaGetTypeId();

	MovementMachine* MovementMachine;
	int TransactionId;
	int field_14;
};

struct MSMovement : public MovementState
{
	glm::vec3 Velocity;
	bool IsMoving;
	bool FitToGrid;
	bool IsRunning;
	bool MoveTo;
	float TimeRemaining;
};


struct MSMoveTo : public MovementState
{
	glm::vec3 Position;
	glm::vec3 Position2;
	ComponentHandle NextAiHandle;
	int NextTargetCheckType;
	float NextCloseEnoughDistMax;
	float NextCloseEnoughDistMin;
	int16_t NextHorizon;
	uint8_t Nextfield_46;
	int NextSurfaceNavigationType;
	bool NextUseCharacterRadius;
	glm::vec3 CurPosition;
	glm::vec3 AiBounds_M;
	glm::vec3 CurPositionOld;
	ComponentHandle AiHandle;
	int TargetCheckType;
	float CloseEnoughDistMax;
	float CloseEnoughDistMin;
	float CharacterScale;
	int16_t Horizon;
	uint64_t AiFlags;
	bool CanRun;
	int SurfaceNavigationType;
	glm::vec3 CurPositionOld2;
	float TimeUntilNextPathfinding;
	float PathTimeout2;
	float Speed;
	float MovementAP;
	float TimeUntilNextPathing;
	int AiPathId;
	glm::vec2 CharCurrentPositionXY;
	int field_D4;
	bool Paused;
	bool CannotMove;
	bool IsPathfinding;
	bool AiFloodDone;
	bool field_DC;
	bool field_DD;
	bool TargetAiGridFlag0x10;
	bool UseCharacterRadius;
	bool PositionUpdated;
	bool FreeMovement;
	bool AutoFreeMovement;
	int field_E4;
	eoc::PathParams PathParams;
	ObjectSet<ComponentHandle> Set_ObjHandle;
};


struct MovementMachine : public ProtectedGameObject<MovementMachine>
{
	MSMovement* CachedMSMovement;
	MSMoveTo* CachedMSMoveTo;
	bool Active;
	std::array<esv::MovementState*, 2> Layers;
	ComponentHandle CharacterHandle;
};

END_NS()


BEGIN_NS(ecl)

struct MovementMachine;

struct MovementState : public ProtectedGameObject<MovementState>
{
	virtual ~MovementState() = 0;
	virtual bool Enter() = 0;
	virtual bool Continue() = 0;
	virtual void Update(GameTime const& t) = 0;
	virtual uint32_t Unknown1() = 0;
	virtual void Exit() = 0;
	virtual bool IsFinished() = 0;
	virtual MovementStateType GetTypeId() = 0;

	MovementStateType LuaGetTypeId();

	MovementMachine* MovementMachine;
	uint32_t TransactionId;
};

struct MSMoveTo : public MovementState
{
	struct PlayerWeightFunc
	{
		void* VMT;
		uint32_t SizeInAiGridCells;
		bool IgnoreMovementBlock;
		bool IsController_M;
		bool InCombat;
		ObjectSet<SurfacePathInfluence> SurfacePathInfluences;
	};


	glm::vec3 Target;
	glm::vec3 AiGridValidatedTarget;
	glm::vec3 SourcePath;
	glm::vec3 AiGridValidatedSourcePath;
	glm::vec3 AiGridValidatedTarget2;
	ComponentHandle AiHandle;
	uint32_t TargetCheckType;
	float CloseEnoughDistMax;
	float CloseEnoughDistMin;
	float CharacterRadius;
	uint16_t Horizon;
	uint64_t AiGridFlags;
	bool IsRunning;
	bool field_81;
	bool UseSurfacePathInfluences;
	bool UseCharacterRadius;
	bool CanOpenDoors;
	glm::vec3 AiGridValidatedTarget_Copy;
	float TimeUntilNextPathfinding2;
	float TimeRemaining_M;
	float TimeUntilNextTargetPoint;
	int CustomAiPathId;
	glm::vec2 SourcePathXZ;
	int field_AC;
	bool IsFinished;
	bool PathfindingComplete_M;
	bool field_B2;
	bool FoundNextTargetPoint;
	bool field_B4;
	bool DisabledStraightPathfinding_M;
	PlayerWeightFunc WeightFunc;
	glm::vec3 ParamTarget2;
	ComponentHandle ParamAiHandle;
	uint32_t ParamTargetCheckType;
	float ParamCloseEnoughDistMax;
	float ParamCloseEnoughDistMin;
	uint16_t ParamHorizon;
	bool ParamIsRunning;
	bool Paramfield_81;
	bool ParamUseSurfacePathInfluences;
	bool ParamUseCharacterRadius;
	bool ParamCanOpenDoors;
	bool field_113;
	bool WithinFreeMovementDistance;
	bool IsAttack_M;
	float ConsumedMoveDistance;
	float TimeUntilNextPathfinding;
	ObjectSet<ComponentHandle> IgnoreCollisionAiHandles;
};


struct MovementMachine : public ProtectedGameObject<MovementMachine>
{
	void* CachedMSMovement;
	MSMoveTo* CachedMSMoveTo;
	bool Active;
	std::array<esv::MovementState*, 2> Layers;
	ComponentHandle CharacterHandle;
};

END_NS()
