#pragma once

#include "BaseTypes.h"
#include "Enumerations.h"

namespace dse
{

#pragma pack(push, 1)
	struct TDamagePair
	{
		int32_t Amount;
		DamageType DamageType;
	};

	struct DamagePairList : public Array<TDamagePair>
	{
		void AddDamage(DamageType damageType, int32_t amount);
		void ClearDamage(DamageType damageType);
	};

	struct HitDamageInfo : public Noncopyable<CRPGStatsManager>
	{
		uint32_t Equipment{ 0 };
		// Sum of all damages in DamageList
		int32_t TotalDamage{ 0 };
		// Damage dealt after ApplyDamage() call
		int32_t DamageDealt{ 0 };
		DeathType DeathType{ DeathType::Sentinel };
		uint8_t _Pad1[3];
		DamageType DamageType{ DamageType::None };
		uint32_t AttackDirection{ 0 };
		int32_t ArmorAbsorption{ 0 };
		int32_t LifeSteal{ 0 };
		uint32_t EffectFlags{ 0 };
		bool HitWithWeapon{ false };
		uint8_t _Pad2[3];
		DamagePairList DamageList;

		inline HitDamageInfo() {}
		void ClearDamage();
		void ClearDamage(dse::DamageType damageType);
		void AddDamage(dse::DamageType damageType, int32_t amount);
	};

	namespace esv
	{
		struct StatusVMT
		{
			void * Destroy;
			void * SetObjectHandle;
			void * GetObjectHandle;
			void * GetStatusId;
			void * GetTriggerBehavior_M;
			void * AddStatsData2_Maybe;
			void * GetStatsIdByIndex;
			void * VMT38;
			void * VMT40;
			void * VMT48;
			void * CanEnter;
			void * Init;
			void * Enter;
			void * Resume;
			void * Update;
			void * Tick;
			void * Exit;
			void * VMT88;
			void * ConsumeStatsId;
			void * VMT98;
			void * IsImmobilizingStatus;
			void * VMTA8;
			void * VMTB0;
			void * GetSyncData;
			void * VMTC0;
			void * Serialize;
			void * VMTD0;
			void * CreateVisuals;
			void * DestroyVisuals;
			void * SetHostileFlagFromSavingThrow_M;
			void * GetEnterChance;
			void * AddStatsData_Maybe;
		};

		struct Status : public ProtectedGameObject<CRPGStatsManager>
		{
			virtual ~Status() = 0;
			virtual void SetObjectHandle(ObjectHandle Handle) = 0;
			virtual void GetObjectHandle(ObjectHandle * Handle) = 0;
			virtual StatusType GetStatusId() = 0;
			// 0 - Stackable
			// 1 - Apply only the first instance, discard new ones
			// 2 - Apply only one instance, new instances replace old ones
			// 3 - Apply only the first instance; triggers combat?
			virtual uint32_t GetStatusType() = 0;
			virtual void AddStatsData2_Maybe() = 0;
			virtual void * GetStatsIdByIndex(int index) = 0;
			virtual void VMT38() = 0;
			virtual void VMT40() = 0;
			virtual void VMT48() = 0;
			virtual bool CanEnter() = 0;
			virtual void Init() = 0;
			virtual bool Enter() = 0;
			virtual bool Resume() = 0;
			virtual void Update(void * A, float Time_M) = 0;
			virtual void Tick(int * a2, float a3, float a4) = 0;
			virtual void Exit() = 0;
			virtual void VMT88() = 0;
			virtual void ConsumeStatsId() = 0;
			virtual void VMT98() = 0;
			virtual void IsImmobilizingStatus() = 0;
			virtual void VMTA8() = 0;
			virtual void VMTB0() = 0;
			virtual void GetSyncData() = 0;
			virtual void VMTC0() = 0;
			virtual void Serialize() = 0;
			virtual void VMTD0() = 0;
			virtual void CreateVisuals() = 0;
			virtual void DestroyVisuals() = 0;
			virtual void SetHostileFlagFromSavingThrow_M() = 0;
			virtual void GetEnterChance() = 0;
			virtual void AddStatsData_Maybe() = 0;

			// void * VMT;
			FixedString FS1;
			NetId NetID;
			uint32_t _Pad0;
			uint64_t U1;
			FixedString StatusId; // Saved
			uint32_t CanEnterChance; // Saved
			float StartTimer; // Saved
			float LifeTime; // Saved
			float CurrentLifeTime; // Saved
			float TurnTimer; // Saved
			float Strength; // Saved
			float StatsMultiplier; // Saved
			CauseType DamageSourceType; // Saved
			uint8_t _Pad1[3];
			ObjectHandle StatusHandle;
			ObjectHandle TargetHandle;
			ObjectSet<ObjectHandle> StatusOwner; // Saved
			ObjectHandle StatusSourceHandle; // Saved
			ObjectHandle SomeHandle;
			uint8_t Flags2; // Saved
			uint8_t Flags0; // Saved
			uint8_t Flags1; // Saved
			uint8_t _Pad2;
			uint32_t _Pad3;
		};

		struct StatusHit : public Status
		{
			uint32_t Unk2;
			uint8_t Unk3;
			uint8_t _Pad4[3];
			void * RPGPropertyList;
			HitDamageInfo DamageInfo;
			ObjectHandle HitByHandle;
			// Character, Item or Projectile
			ObjectHandle HitWithHandle;
			ObjectHandle WeaponHandle;
			// 0 - ASAttack
			// 1 - Character::ApplyDamage, StatusDying, ExecPropertyDamage, StatusDamage
			// 2 - AI hit test
			// 5 - InSurface
			// 6 - SetHP, osi::ApplyDamage, StatusConsume
			uint32_t HitReason;
			uint8_t _Pad5[4];
			FixedString SkillId;
			bool Interruption;
			bool AllowInterruptAction;
			// 0 = only interrupt Idle and Animation behaviors
			// 1 = always interrupt character action
			bool ForceInterrupt;
			// Decrease characters' DelayDeathCount
			bool DecDelayDeathCount;
			uint8_t PropertyContext;
			uint8_t _Pad7[3];
			Vector3 ImpactPosition;
			Vector3 ImpactOrigin;
			Vector3 ImpactDirection;
			uint32_t Unk4;
		};

		struct StatusHeal : public Status
		{
			float EffectTime;
			uint32_t HealAmount;
			HealEffect HealEffect;
			uint8_t _Pad4[4];
			FixedString HealEffectId;
			StatusHealType HealType;
			uint32_t AbsorbSurfaceRange;
			ObjectSet<void *> AbsorbSurfaceTypes;
			uint64_t Unkn2;
			bool TargetDependentHeal;
			uint8_t _Pad5[3];
			uint32_t TargetDependentValue[3];
			uint32_t TargetDependentHealAmount[3];
			uint32_t Unkn3;
		};

		struct StatusConsume : public Status
		{
			ObjectSet<FixedString> Skill; // Saved
			ObjectSet<FixedString> Items; // Saved
			PrimitiveSet<uint32_t> ResetCooldownsSet; // Set<TSkillAbility>
			int64_t field_E0;
			int64_t field_E8;
			bool ResetAllCooldowns; // Saved
			bool ResetOncePerCombat; // Saved
			bool ScaleWithVitality; // Saved
			bool LoseControl; // Saved
			uint8_t _Pad10[4];
			FixedString ApplyStatusOnTick;
			ObjectSet<ObjectHandle> ItemHandles; // Saved
			float EffectTime; // Saved
			uint8_t _Pad11[4];
			FixedString StatsId; // Saved
			ObjectSet<void *> StatsIDs; // Set<StatsData>
			FixedString StackId;
			FixedString OriginalWeaponStatsId;
			FixedString OverrideWeaponStatsId;
			ObjectHandle OverrideWeaponHandle;
			int field_170;
			int SavingThrow; // TODO enum + enum prop!
			Vector3 SourceDirection; // Saved
			uint8_t _Pad12[4];
			PrimitiveSet<void *> SurfaceChangeSet; // Set<SurfaceChange>
			int field_198;
			int field_19C;
			int64_t field_1A0;
			int Turn; // Saved
			int field_1AC;
			Status * AuraStatus; // Saved
			HealEffect HealEffectOverride; // Saved
			char field_1BC;
			char field_1BD;
			char field_1BE;
			char field_1BF;
		};

		struct StatusHealing : public StatusConsume
		{
			uint32_t HealAmount; // Saved
			float TimeElapsed; // Saved
			HealEffect HealEffect; // Saved
			uint8_t _Pad5[4];
			FixedString HealEffectId; // Saved
			bool SkipInitialEffect; // Saved
			uint8_t _Pad6[3];
			uint32_t HealingEvent; // Saved
			StatusHealType HealStat; // Saved
			uint32_t AbsorbSurfaceRange; // Saved
		};

		struct StatusDamage : public StatusConsume
		{
			int32_t DamageEvent; // Saved
			float HitTimer; // Saved
			float TimeElapsed; // Saved
			int DamageLevel; // Saved
			uint64_t DamageStats; // Saved
			bool SpawnBlood; // Saved
			uint8_t _Pad20[7];
		};


		struct StatusDamageOnMove : public StatusDamage
		{
			float DistancePerDamage; // Saved
			float DistanceTraveled; // Saved
			FixedString _Unknown;
		};

		struct StatusActiveDefense : public StatusConsume
		{
			int Charges;
			Vector3 TargetPos;
			ObjectHandle TargetHandle;
			int _Unkn10;
			FixedString FixedStr;
			uint64_t _Unkn1011;
			ObjectSet<void *> Targets; // ObjectSet<StatusActiveDefenseTargetData>
			ObjectSet<ObjectHandle> PreviousTargets;
		};


		struct StatusMachine : public NetworkObjectFactory<esv::Status, (uint32_t)ObjectType::Character>
		{
			Status * GetStatusByHandle(ObjectHandle handle) const;

			void* GameEventMgrVMT;
			bool IsStatusMachineActive;
			bool PreventStatusApply;
			uint8_t _Pad1[6];
			ObjectSet<Status *> Statuses;
			ObjectHandle OwnerObjectHandle;
			uint32_t References;
		};

		typedef Status * (*StatusMachine__CreateStatus)(esv::StatusMachine * StatusMachine, FixedString & StatusId, uint64_t ObjectHandle);
		typedef void(*StatusMachine__ApplyStatus)(esv::StatusMachine * StatusMachine, Status * Status);
		typedef bool(*Status__Enter)(Status * Status);
		typedef int32_t(*Status__GetEnterChance)(Status * self, bool useCharacterStats);

	}
#pragma pack(pop)

	struct PendingStatus
	{
		esv::Status * Status;
		bool PreventApply;
	};

	class PendingStatuses
	{
	public:
		void Add(esv::Status * status);
		void Remove(esv::Status * status);
		PendingStatus * Find(ObjectHandle owner, ObjectHandle statusHandle);

	private:
		std::unordered_map<ObjectHandle, PendingStatus> statuses_;
	};
}