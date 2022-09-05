#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/Stats.h>
#include <GameDefinitions/Components/Projectile.h>

BEGIN_NS(esv)

struct GameAction : public ProtectedGameObject<GameAction>
{
	static constexpr auto ObjectTypeIndex = ObjectHandleType::Unknown;

	void * VMT;
	FixedString SomeFS;
	NetId NetID;
	ObjectSet<uint16_t> PeerIDClassNames;
	ComponentHandle Handle;
	GameActionType ActionType;
	bool Active;
	bool Dirty;
	float ActivateTimer;

	void Destroy();
};

struct GameActionManager : public NetworkComponentFactory<GameAction>
{
	BaseComponentProcessingSystem<EntityWorld> ComponentProcessingSystem;
	ObjectSet<GameAction *> GameActions;
};

struct TornadoAction : public GameAction
{
	FixedString SkillId;
	ComponentHandle Owner;
	glm::vec3 Position;
	glm::vec3 Target;
	float TurnTimer;
	bool Finished;
	bool IsFromItem;
	float HitRadius;
	stats::PropertyList SkillProperties;
	ObjectSet<glm::vec3> AnchorList;
	uint64_t Anchor;
	float Interpolation;
	ComponentHandle SurfaceActionHandle;
	ObjectSet<ComponentHandle> HitCharacterHandles;
	ObjectSet<ComponentHandle> HitItemHandles;
	FixedString CleanseStatuses;
	float StatusClearChance;
};

struct StormAction : public GameAction
{
	struct Strike
	{
		ComponentHandle Object;
		glm::vec3 Target;
		glm::vec3 Source;
		FixedString SkillId;
	};


	ComponentHandle Owner;
	glm::vec3 Position;
	float LifeTime;
	FixedString SkillId;
	float TurnTimer;
	float StrikeTimer;
	bool Finished;
	bool IsFromItem;
	eoc::GameRandom Random1;
	eoc::GameRandom Random2;
	eoc::GameRandom Random3;
	ObjectSet<FixedString> ProjectileSkills;
	ObjectSet<ProjectileTargetDesc> ProjectileTargets;
	ObjectSet<Strike> Strikes;
};

struct RainAction : public GameAction
{
	ComponentHandle Owner;
	glm::vec3 Position;
	float AreaRadius;
	float LifeTime;
	float Duration;
	bool FirstTick;
	FixedString SkillId;
	float ConsequencesStartTime;
	float TurnTimer;
	bool Finished;
	bool IsFromItem;
	FixedString SkillProperties;
};

struct WallAction : public GameAction
{
	FixedString SkillId;
	ComponentHandle Owner;
	glm::vec3 Target;
	glm::vec3 Source;
	float LifeTime;
	ObjectSet<ComponentHandle> Walls;
	float TurnTimer;
	bool Finished;
	bool IsFromItem;
	uint64_t NumWallsGrown;
	float TimeSinceLastWall;
	float GrowTimePerWall;
	float GrowTimeout;
	int State;
};

struct StatusDomeAction : public GameAction
{
	ComponentHandle Owner;
	glm::vec3 Position;
	float LifeTime;
	FixedString SkillId;
	bool Finished;
	void * SkillStatusAura;
};

struct PathAction : public GameAction
{
	FixedString SkillId;
	ComponentHandle Owner;
	glm::vec3 Position;
	glm::vec3 Target;
	float TurnTimer;
	float Speed;
	float HitRadius;
	stats::PropertyList* SkillProperties;
	ObjectSet<glm::vec3> Waypoints;
	uint64_t Anchor;
	uint64_t PreviousAnchor;
	float Interpolation;
	float Distance;
	ComponentHandle SurfaceAction;
	ObjectSet<ComponentHandle> HitCharacters;
	ObjectSet<ComponentHandle> HitItems;
	bool Finished;
	bool IsFromItem;
};

struct GameObjectMoveAction : public GameAction
{
	eoc::PathMover * PathMover;
	ComponentHandle Owner;
	bool DoneMoving;
	float NextMoveTime;
	float LastMoveTime;
	ComponentHandle CasterCharacterHandle;
	FixedString BeamEffectName;
	ComponentHandle SomeHandle;
	uint64_t DoneMovingCallback[8];
};

typedef void * (*GameActionManager__CreateAction)(GameActionManager * GameActionManager, GameActionType actionId, uint64_t SomeHandle);
#if defined(OSI_EOCAPP)
typedef void(*GameActionManager__AddAction)(Set<GameAction *> * GameActionManager, esv::GameAction ** Action);
#else
typedef void(*GameActionManager__AddAction)(GameActionManager * GameActionManager, esv::GameAction * Action);
#endif
typedef void(*TornadoAction__Setup)(TornadoAction* self);
typedef void(*WallAction__CreateWall)(WallAction * self);
typedef void(*GameObjectMoveAction__Setup)(GameObjectMoveAction* self, ComponentHandle & ObjectToMove, glm::vec3 const& TargetPosition);

END_NS()
