#pragma once

#include "BaseTypes.h"
#include "Enumerations.h"

namespace dse
{

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

	struct HitDamageInfo
	{
		uint32_t Equipment{ 0 };
		// Sum of all damages in DamageList
		int32_t TotalDamage{ 0 };
		// Damage dealt after ApplyDamage() call
		int32_t DamageDealt{ 0 };
		DeathType DeathType{ DeathType::Sentinel };
		DamageType DamageType{ DamageType::None };
		uint32_t AttackDirection{ 0 };
		int32_t ArmorAbsorption{ 0 };
		int32_t LifeSteal{ 0 };
		HitFlag EffectFlags{ 0 };
		bool HitWithWeapon{ false };
		DamagePairList DamageList;

		inline HitDamageInfo() {}
		void ClearDamage();
		void ClearDamage(dse::DamageType damageType);
		void AddDamage(dse::DamageType damageType, int32_t amount);
		void CopyFrom(HitDamageInfo const& src);
	};

	namespace esv
	{
		struct Status : public ProtectedGameObject<Status>
		{
			using GetEnterChanceProc = int32_t(Status* self, bool isEnterCheck);
			using EnterProc = bool (Status* self);

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
			ObjectHandle StatusHandle;
			ObjectHandle TargetHandle;
			ObjectSet<ObjectHandle> StatusOwner; // Saved
			ObjectHandle StatusSourceHandle; // Saved
			ObjectHandle SomeHandle;
			StatusFlags2 Flags2; // Saved
			StatusFlags0 Flags0; // Saved
			StatusFlags1 Flags1; // Saved
		};

		struct StatusVMT
		{
			void* Destroy;
			void* SetObjectHandle;
			void* GetObjectHandle;
			void* GetStatusId;
			void* GetTriggerBehavior_M;
			void* AddStatsData2_Maybe;
			void* GetStatsIdByIndex;
			void* VMT38;
			void* VMT40;
			void* VMT48;
			void* CanEnter;
			void* Init;
			Status::EnterProc* Enter;
			void* Resume;
			void* Update;
			void* Tick;
			void* Exit;
			void* VMT88;
			void* ConsumeStatsId;
			void* VMT98;
			void* IsImmobilizingStatus;
			void* VMTA8;
			void* VMTB0;
			void* GetSyncData;
			void* VMTC0;
			void* Serialize;
			void* VMTD0;
			void* CreateVisuals;
			void* DestroyVisuals;
			void* SetHostileFlagFromSavingThrow_M;
			Status::GetEnterChanceProc* GetEnterChance;
			void* AddStatsData_Maybe;
		};

		struct StatusHit : public Status
		{
			using SetupProc = void (StatusHit* self, HitDamageInfo* hit);

			uint32_t Unk2;
			uint8_t Unk3;
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
			FixedString SkillId;
			bool Interruption;
			bool AllowInterruptAction;
			// 0 = only interrupt Idle and Animation behaviors
			// 1 = always interrupt character action
			bool ForceInterrupt;
			// Decrease characters' DelayDeathCount
			bool DecDelayDeathCount;
			uint8_t PropertyContext;
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
			FixedString HealEffectId;
			StatusHealType HealType;
			uint32_t AbsorbSurfaceRange;
			ObjectSet<void *> AbsorbSurfaceTypes;
			uint64_t Unkn2;
			bool TargetDependentHeal;
			uint32_t TargetDependentValue[3];
			uint32_t TargetDependentHealAmount[3];
			uint32_t Unkn3;
		};

		struct StatusConsume : public Status
		{
			ObjectSet<FixedString> Skill; // Saved
			ObjectSet<FixedString> Items; // Saved
			ObjectSet<uint32_t> ResetCooldownsSet; // Set<TSkillAbility>
			bool ResetAllCooldowns; // Saved
			bool ResetOncePerCombat; // Saved
			bool ScaleWithVitality; // Saved
			bool LoseControl; // Saved
			FixedString ApplyStatusOnTick;
			ObjectSet<ObjectHandle> ItemHandles; // Saved
			float EffectTime; // Saved
			FixedString StatsId; // Saved
			ObjectSet<void *> StatsIDs; // Set<StatsData>
			FixedString StackId;
			FixedString OriginalWeaponStatsId;
			FixedString OverrideWeaponStatsId;
			ObjectHandle OverrideWeaponHandle;
			int AttributeHandle;
			int SavingThrow; // TODO enum + enum prop!
			Vector3 SourceDirection; // Saved
			ObjectSet<void *> SurfaceChangeSet; // Set<SurfaceChange>
			int Turn; // Saved
			int field_1AC;
			Status * AuraStatus; // Saved
			HealEffect HealEffectOverride; // Saved
			bool Poisoned;
			char field_1BD;
			char field_1BE;
			char field_1BF;
		};

		struct StatusHealing : public StatusConsume
		{
			uint32_t HealAmount; // Saved
			float TimeElapsed; // Saved
			HealEffect HealEffect; // Saved
			FixedString HealEffectId; // Saved
			bool SkipInitialEffect; // Saved
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
			FixedString DamageStats; // Saved
			bool SpawnBlood; // Saved
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


		struct StatusMachine : public NetworkObjectFactory<Status, (uint32_t)ObjectType::Unknown>
		{
			using CreateStatusProc = Status* (esv::StatusMachine* StatusMachine, FixedString& StatusId, uint64_t ObjectHandle);
			using ApplyStatusProc = void (esv::StatusMachine* StatusMachine, Status* Status);

			Status * GetStatus(ObjectHandle handle) const;
			Status* GetStatus(NetId netId) const;
			Status* GetStatus(FixedString const& statusId) const;

			void* GameEventMgrVMT;
			bool IsStatusMachineActive;
			bool PreventStatusApply;
			ObjectSet<Status *> Statuses;
			ObjectHandle OwnerObjectHandle;
			uint32_t References;
		};
	}

	namespace ecl
	{
		struct Status
		{
			virtual void Destroy() = 0;
			virtual void SetOwnerHandle(ObjectHandle handle) = 0;
			virtual ObjectHandle GetOwnerHandle(ObjectHandle *) = 0;
			virtual StatusType GetStatusId() = 0;
			virtual unsigned int GetStatusType() = 0;
			virtual unsigned int BeforeAddStatus() = 0;
			virtual bool Enter() = 0;
			virtual bool Resume() = 0;
			virtual void Update() = 0;
			virtual uint32_t Tick(uint32_t * combatTeamId, float delta) = 0;
			virtual void Exit() = 0;
			virtual void OnEarlyDestroy() = 0;
			virtual void ApplyStatsId() = 0;
			virtual FixedString* GetIcon() = 0;
			virtual void field_70() = 0;
			virtual void GetDescriptionWithoutPrototype() = 0;
			virtual void GetEffectTexts(ObjectSet<STDWString> & texts) = 0;
			virtual void ConcatTooltipText(eoc::Text& text) = 0;
			virtual void ConcatDescription(eoc::Text& text) = 0;
			virtual FixedString* GetStatsId() = 0;
			virtual int GetCauseLevel() = 0;
			virtual void field_A8() = 0;
			virtual bool ShouldShowVisuals() = 0;
			virtual bool SyncData(ScratchBuffer& buffer) = 0;
			virtual void DestroyVisuals() = 0;
			virtual void RecreateVisuals() = 0;
			virtual void field_D0() = 0;
			virtual void OnSetRequestDelete() = 0;
			virtual void GetTooltipText(eoc::Text& text) = 0;
			virtual void FetchData() = 0;

			FixedString field_8;
			NetId NetID;
			ObjectHandle OwnerHandle;
			__int64 field_20;
			FixedString StatusId;
			int field_30;
			float LifeTime;
			float CurrentLifeTime;
			float StatsMultiplier;
			StatusFlags Flags;
			ObjectHandle StatusSourceHandle;
		};


		struct StatusMachine : public NetworkObjectFactory<Status, (uint32_t)ObjectType::ClientStatus>
		{
			Status* GetStatus(StatusType type) const;
			Status* GetStatus(FixedString const& statusId) const;
			Status* GetStatus(NetId netId) const;

			void* GameEventMgrVMT;
			bool IsStatusMachineActive;
			bool PreventStatusApply;
			ObjectSet<Status*> Statuses;
			ObjectHandle OwnerObjectHandle;
			uint32_t References;
			int field_154;
			__int64 field_158;
		};

	}

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