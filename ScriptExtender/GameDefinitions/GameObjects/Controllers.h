#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>

BEGIN_NS(esv)

struct BaseController : public ProtectedGameObject<BaseController>
{
	__int64 VMT;
	int TypeId;
	ComponentHandle CharacterHandle;
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

END_NS()
