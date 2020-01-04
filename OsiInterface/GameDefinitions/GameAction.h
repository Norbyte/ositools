#pragma once

#include "BaseTypes.h"
#include "Enumerations.h"

namespace osidbg
{

#pragma pack(push, 1)
	namespace esv
	{
		struct GameAction : public ProtectedGameObject<GameAction>
		{
			void * VMT;
			FixedString SomeFS;
			NetId NetID;
			uint8_t _Unk0[4];
			PrimitiveSet<uint16_t> PeerIDClassNames;
			uint64_t _Unk[2];
			ObjectHandle MyHandle;
			uint32_t GameActionType;
			bool Active;
			bool Dirty;
			uint8_t _Pad1[2];
			float ActivateTimer;
			uint32_t _Pad2;
		};

		struct GameActionManager : public ProtectedGameObject<GameActionManager>
		{
			uint8_t Unmapped[0x130];
			Set<GameAction *> GameActions;
		};

		struct TornadoAction : public GameAction
		{
			FixedString SkillId;
			ObjectHandle OwnerHandle;
			glm::vec3 Position;
			glm::vec3 Target;
			float TurnTimer;
			bool Finished;
			bool IsFromItem;
			uint8_t _Pad3[2];
			float HitRadius;
			uint32_t _Pad4;
			uint64_t _Unk2;
			ObjectSet<Vector3> AnchorList;
			uint64_t Anchor;
			float Interpolation;
			uint32_t _Unk3;
			ObjectHandle SurfaceActionHandle;
			ObjectSet<ObjectHandle> HitCharacterHandles;
			ObjectSet<ObjectHandle> HitItemHandles;
			FixedString CleanseStatuses;
			float StatusClearChance;
		};

		struct StormAction : public GameAction
		{
			ObjectHandle OwnerHandle;
			glm::vec3 Position;
			float LifeTime;
			FixedString SkillId;
			float TurnTimer;
			float StrikeTimer;
			bool Finished;
			bool IsFromItem;
			uint8_t _Pad3[6];
			uint64_t Unkn[3 * 5];
			ObjectSet<FixedString> FSSet;
			ObjectSet<void *> ProjectileTargetDescSet;
			ObjectSet<void *> StrikeSet;
		};

		struct RainAction : public GameAction
		{
			ObjectHandle OwnerHandle;
			glm::vec3 Position;
			float AreaRadius;
			float LifeTime;
			float Duration;
			bool FirstTick;
			uint8_t _Pad3[7];
			FixedString SkillId;
			float ConsequencesStartTime;
			float TurnTimer;
			bool Finished;
			bool IsFromItem;
			uint8_t _Pad4[2];
			FixedString SkillProperties;
		};

		struct WallAction : public GameAction
		{
			FixedString SkillId;
			ObjectHandle OwnerHandle;
			glm::vec3 Target;
			glm::vec3 Source;
			float LifeTime;
			uint8_t _Pad3[4];
			ObjectSet<void *> Walls;
			float TurnTimer;
			bool Finished;
			bool IsFromItem;
			uint8_t _Pad4[2];
			uint64_t Unk1;
			uint64_t Unk2;
			uint32_t Unk3;
			uint32_t Unk4;
		};

		struct StatusDomeAction : public GameAction
		{
			ObjectHandle OwnerHandle;
			glm::vec3 Position;
			uint8_t _Pad3[4];
			float LifeTime;
			uint8_t _Pad4[4];
			FixedString SkillId;
			bool Finished;
			uint8_t _Pad5[7];
			void * SkillStatusAura;
		};

		struct GameObjectMoveAction : public GameAction
		{
			void * PathMover;
			ObjectHandle ObjectToMove;
			bool DoneMoving;
			uint8_t _Pad3[3];
			uint32_t U3[2];
			uint8_t _Pad4[4];
			ObjectHandle CasterCharacterHandle;
			FixedString BeamEffectName;
			ObjectHandle SomeHandle;
			uint64_t _U4[8];
		};

		typedef void * (*GameActionManager__CreateAction)(GameActionManager * GameActionManager, GameActionType actionId, uint64_t SomeHandle);
		typedef void(*GameActionManager__AddAction)(GameActionManager * GameActionManager, void * Action);
		typedef void(*TornadoAction__Setup)(void * TornadoAction);
		typedef void(*GameObjectMoveAction__Setup)(void * Action, ObjectHandle & ObjectToMove, glm::vec3 * TargetPosition);

	}
#pragma pack(pop)
}