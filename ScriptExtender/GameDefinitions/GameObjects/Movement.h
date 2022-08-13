#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/GameObjects/Ai.h>

BEGIN_NS(esv)

struct MovementMachine;

struct MovementState : public ProtectedGameObject<MovementState>
{
	virtual ~MovementState() = 0;


	MovementMachine* MovementMachine;
	int TransactionId;
	int field_14;
};

struct MSMovement : public MovementState
{
  __int64 field_18;
  int field_20;
  uint32_t field_24;
  int field_28;
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
