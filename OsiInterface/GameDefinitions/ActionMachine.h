#pragma once

#include "BaseTypes.h"
#include "Enumerations.h"
#include "Projectile.h"

namespace dse
{

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
			uint32_t Unknown2;

			PropertyMapBase * GetPropertyMap();
			char const * GetTypeName();
		};

		struct NumberDivider
		{
			float DamagePerHit;
			int field_4;
		};

		struct NumberDividers
		{
			NumberDivider ArmorAbsorptionDivider;
			ObjectSet<void*> DamageDividers; // DamageDivider::Damage ?
		};

		struct ASAttack : public ProtectedGameObject<ActionState>
		{
			ObjectHandle TargetHandle;
			glm::vec3 TargetPosition;
			ObjectSet<ObjectHandle> ObjectHandleSet;
			bool IsFinished;
			bool AlwaysHit;
			__int64 AttackAnimation;
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
			ObjectHandle MainWeaponHandle;
			ObjectHandle OffWeaponHandle;
			RefMap<ObjectHandle, HitDamageInfo> MainWeaponDamageList;
			RefMap<ObjectHandle, HitDamageInfo> OffHandDamageList;
			RefMap<ObjectHandle, NumberDividers> MainWeaponNumberDividers;
			RefMap<ObjectHandle, NumberDividers> OffHandNumberDividers;
			ObjectSet<void*> DamageDividerDamage; // DamageDivider::Damage ?
			ObjectSet<void*> DamageDividerDamage2; // DamageDivider::Damage ?
			__int64 field_118;
			__int64 field_120;
			int MainHandHitType;
			int OffHandHitType;
			ShootProjectileHelperHitObject HitObject1;
			ShootProjectileHelperHitObject HitObject2;
			bool ProjectileUsesHitObject;
			glm::vec3 ProjectileStartPosition;
			glm::vec3 ProjectileTargetPosition;
			bool DamageDurability;
			ObjectSet<ObjectHandle> DelayDeathCharacterHandles;
		};

		struct SkillState
		{
			void* VMT;
			int StateIndex;
			int State;
			FixedString SkillId;
			ObjectHandle CharacterHandle;
			ObjectHandle SourceItemHandle;
			bool CanEnter;
			bool IsFinished;
			bool SkillStateOver6_2;
			bool IgnoreChecks;
			bool IsStealthed;
			int field_30;
			float PrepareTimerRemaining;
			float Unkn_OnSkillCombatCommentEventCondition;
			float SomeCombatCommentEventTimeoutValue;
			bool field_40;
			bool ShouldExit;
			bool SkillStateOver6;
			FixedString CleanseStatuses;
			float StatusClearChance;
			bool CharacterHasSkill;
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
			SkillState* OriginalSkill;
			bool OwnsSkillStateOriginal;
			SkillState* Skill;
			bool OwnsSkillState;
		};

		struct ActionMachineLayer
		{
			ActionState * State;
			uint64_t Unknown;
			uint8_t Flags1;
			uint8_t Flags2;
			uint8_t Flags3;
		};

		struct ActionMachine : public ProtectedGameObject<ActionMachine>
		{
			typedef void(*UpdateSyncStateProc)(ActionMachine * self, uint8_t actionLayer, ActionState * actionState, bool force, bool setLayer);
			typedef bool(*SetStateProc)(esv::ActionMachine * self, uint64_t actionLayer, esv::ActionState * actionState, int * somePtr, bool force, bool setLayer);
			typedef bool(*ResetStateProc)(esv::ActionMachine* self, bool force);

			ObjectHandle CharacterHandle;
			ActionMachineLayer Layers[3];
			bool IsEntering[4];
			uint16_t Unknown;
			ActionState * CachedActions[24];
		};
	}

}
