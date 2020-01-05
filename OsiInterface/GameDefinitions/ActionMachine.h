#pragma once

#include "BaseTypes.h"
#include "Enumerations.h"

namespace osidbg
{

#pragma pack(push, 1)
	namespace esv
	{
		enum class ActionStateType
		{
			ASAttack = 2,
			ASDie = 3,
			ASHit = 4,
			ASIdle = 5,
			ASAnimation = 6,
			ASPrepareSkill = 7,
			ASUseSkill = 8,
			ASPickUp = 9,
			ASDrop = 10,
			ASMoveItem = 11,
			ASKnockedDown = 12,
			ASSummoning = 13,
			ASPolymorphing = 14,
			ASUseItem = 15,
			ASCombineItem = 17,
			ASTeleportFall = 18,
			ASSheath = 19,
			ASUnsheath = 20,
			ASIdentify = 21,
			ASRepair = 22,
			ASLockpick = 23,
			ASDisarmTrap = 24,
			ASIncapacitated = 25,
			ASJumpFlight = 26
		};

		struct ActionState : public ProtectedGameObject<ActionState>
		{
			virtual ~ActionState() = 0;
			virtual ActionStateType GetType() = 0;
			virtual void Enter() = 0;
			virtual void Continue() = 0;
			virtual void Update() = 0;
			virtual void Tick() = 0;
			virtual bool CanExit() = 0;
			virtual void Exit() = 0;
			virtual bool IsFinished() = 0;
			virtual void UNK_48() = 0;
			virtual void UNK_50() = 0;
			virtual void UNK_58() = 0;
			virtual void GetSyncData() = 0;
			virtual void SyncData() = 0;
			virtual void GetSyncDataUpdate() = 0;
			// void * VMT;

			struct ActionMachine * Machine;
			uint8_t Unknown1;
			uint8_t _Pad1[3];
			uint32_t Unknown2;

			PropertyMapBase * GetPropertyMap();
			char const * GetTypeName();
		};

		struct ASPrepareSkill : public ActionState
		{
			FixedString SkillId;
			FixedString PrepareAnimationInit;
			FixedString PrepareAnimationLoop;
			bool IsFinished;
			bool IsEntered;
			bool Unknown;
		};

		struct ASUseSkill : public ActionState
		{
			void * OriginalSkill;
			bool OwnsSkillStateOriginal;
			uint8_t _Pad[7];
			void * Skill;
			bool OwnsSkillState;
		};

		struct ActionMachineLayer
		{
			ActionState * State;
			uint64_t Unknown;
			uint8_t Flags1;
			uint8_t Flags2;
			uint8_t Flags3;
			uint8_t _Pad[5];
		};

		struct ActionMachine : public ProtectedGameObject<ActionMachine>
		{
			typedef void(*UpdateSyncStateProc)(ActionMachine * self, uint8_t actionLayer, ActionState * actionState, bool force, bool setLayer);
			typedef bool(*SetStateProc)(esv::ActionMachine * self, uint64_t actionLayer, esv::ActionState * actionState, int * somePtr, bool force, bool setLayer);

			ObjectHandle CharacterHandle;
			ActionMachineLayer Layers[3];
			bool IsEntering[4];
			uint16_t Unknown;
			uint16_t _Pad1;
			ActionState * CachedActions[24];
		};
	}
#pragma pack(pop)

}
