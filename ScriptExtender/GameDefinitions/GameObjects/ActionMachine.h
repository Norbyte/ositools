#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/Components/Projectile.h>

namespace dse
{

	namespace esv
	{
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
			uint32_t TransactionId;

			LegacyPropertyMapBase * GetPropertyMap();
			char const * GetTypeName();
			ActionStateType LuaGetTypeId();
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

		struct ASAttack : public ActionState
		{
			ComponentHandle TargetHandle;
			glm::vec3 TargetPosition;
			ObjectSet<ComponentHandle> ObjectHandleSet;
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
			ComponentHandle MainWeaponHandle;
			ComponentHandle OffWeaponHandle;
			RefMap<ComponentHandle, stats::HitDamageInfo> MainWeaponDamageList;
			RefMap<ComponentHandle, stats::HitDamageInfo> OffHandDamageList;
			RefMap<ComponentHandle, NumberDividers> MainWeaponNumberDividers;
			RefMap<ComponentHandle, NumberDividers> OffHandNumberDividers;
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
			ObjectSet<ComponentHandle> DelayDeathCharacterHandles;
		};

		struct SkillState : public ProtectedGameObject<SkillState>
		{
			void* VMT;
			int StateIndex;
			int State;
			FixedString SkillId;
			ComponentHandle CharacterHandle;
			ComponentHandle SourceItemHandle;
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

		struct ActionMachineLayer : public ProtectedGameObject<ActionMachineLayer>
		{
			ActionState * State;
			uint64_t Unknown;
			uint8_t Flags1;
			uint8_t Flags2;
			uint8_t Flags3;
		};

		struct ActionMachine : public ProtectedGameObject<ActionMachine>
		{
			using UpdateSyncStateProc = void(ActionMachine * self, uint8_t actionLayer, ActionState * actionState, bool force, bool setLayer);
			using SetStateProc = bool (esv::ActionMachine * self, uint64_t actionLayer, esv::ActionState * actionState, int * somePtr, bool force, bool setLayer);
			using ResetStateProc = bool (esv::ActionMachine* self, bool force);

			ComponentHandle CharacterHandle;
			std::array<ActionMachineLayer, 3> Layers;
			bool IsEntering[4];
			uint16_t Unknown;
			ActionState * CachedActions[24];
		};
	}

}
