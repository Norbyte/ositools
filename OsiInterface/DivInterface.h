#pragma once

#include "DivBaseTypes.h"
#include "DivEnumerations.h"

namespace osidbg
{

#pragma pack(push, 1)
	struct CRPGStats_Object
	{
		void * VMT;
		uint32_t Handle;
		uint32_t Level;
		uint32_t ModifierListIndex;
		uint32_t Unused2;
		int32_t * IndexedProperties;
		uint64_t Unused3;
		uint64_t Unused4;
		char * Name;
		TranslatedString TranslatedStringX;
		uint64_t B[18];
		FixedString FS2;
		uint64_t EntryType;
		FixedStringMapBase<int> PropertyList;
		uint32_t Unused5;
		FixedStringMapBase<int> ConditionList;
		uint32_t Unused6;
		uint64_t AIFlags;
		ObjectSet<void *> Requirements; // Set<Requirement>
		ObjectSet<void *> MemorizationRequirements; // Set<Requirement>
		ObjectSet<void *> CrimeReactionPriorities; // Set<CrimeReactionPriority>
		ObjectSet<FixedString> StringProperties1;
		ObjectSet<FixedString> ComboCategories;
#if !defined(OSI_EOCAPP)
		STDWString SomeSTDWSTRING;
#endif
		uint32_t Using;
		uint32_t D;
	};

	struct CRPGStats_ObjectInstance : public CRPGStats_Object
	{
		uint32_t InstanceId;
	};

	struct CDivinityStats_Equipment_Attributes
	{
		void * VMT;
		uint32_t InstanceId;
		uint32_t Unkn0;
		uint64_t Unkn1;
		int32_t Durability;
		int32_t DurabilityDegradeSpeed;
		int32_t StrengthBoost;
		int32_t FinesseBoost;
		int32_t IntelligenceBoost;
		int32_t ConstitutionBoost;
		int32_t Memory;
		int32_t WitsBoost;
		int32_t SightBoost;
		int32_t HearingBoost;
		int32_t VitalityBoost;
		int32_t SourcePointsBoost;
		int32_t MaxAP;
		int32_t StartAP;
		int32_t APRecovery;
		int32_t AccuracyBoost;
		int32_t DodgeBoost;
		int32_t LifeSteal;
		int32_t CriticalChance;
		int32_t ChanceToHitBoost;
		int32_t MovementSpeedBoost;
		int32_t RuneSlots;
		int32_t RuneSlots_V1;
		int32_t FireResistance;
		int32_t AirResistance;
		int32_t WaterResistance;
		int32_t EarthResistance;
		int32_t PoisonResistance;
		int32_t TenebriumResistance;
		int32_t PiercingResistance;
		int32_t CorrosiveResistance;
		int32_t PhysicalResistance;
		int32_t MagicResistance;
		int32_t CustomResistance;
		int32_t Movement;
		int32_t Initiative;
		int32_t Willpower;
		int32_t Bodybuilding;
		int32_t MaxSummons;
		int32_t AbilityModifiers[40];
		int32_t Value;
		int32_t Weight;
		uint32_t Unkn2;
		void * Reflection;
		FixedString Skills;
		FixedString ItemColor;
		uint32_t ModifierType;
		uint32_t Unkn3;
		FixedString ObjectInstanceName;
		FixedString FS4;
		EquipmentStatsType StatsType;
		uint32_t Talents[4];
		uint32_t Unkn4;
		uint64_t Flags;
	};

	struct CDivinityStats_Equipment_Attributes_Weapon : public CDivinityStats_Equipment_Attributes
	{
		int32_t DamageType;
		int32_t MinDamage;
		int32_t MaxDamage;
		int32_t DamageBoost;
		int32_t DamageFromBase;
		int32_t CriticalDamage;
		float WeaponRange;
		int32_t CleaveAngle;
		float CleavePercentage;
		int32_t AttackAPCost;
	};

	struct CDivinityStats_Equipment_Attributes_Shield : public CDivinityStats_Equipment_Attributes
	{
		int32_t ArmorValue;
		int32_t ArmorBoost;
		int32_t MagicArmorValue;
		int32_t MagicArmorBoost;
		int32_t Blocking;
	};

	struct CDivinityStats_Equipment_Attributes_Armor : public CDivinityStats_Equipment_Attributes
	{
		int32_t ArmorValue;
		int32_t ArmorBoost;
		int32_t MagicArmorValue;
		int32_t MagicArmorBoost;
	};

	struct CDivinityStats_Item : public CRPGStats_ObjectInstance
	{
		uint32_t _Pad0;
		uint32_t U1;
		uint32_t ItemType;
		uint32_t ItemSlot;
		uint32_t WeaponType;
		uint32_t AnimType;
		uint32_t WeaponRange;
		uint32_t IsIdentified; // Saved
		uint32_t U2;
		uint32_t U3;
		uint32_t U4;
		bool IsTwoHanded;
		bool HasAddedItems_M;
		bool U5;
		bool HasModifiedSkills; // Saved
		uint32_t U6;
		FixedString Skills; // Saved
		uint32_t DamageTypeOverwrite; // Saved
		uint32_t Durability; // Saved
		uint32_t DurabilityCounter; // Saved
		uint32_t U7;
		FixedString ItemTypeReal; // Saved
		FixedString U8;
		CDivinityStats_Equipment_Attributes ** DynamicAttributes_Start;
		CDivinityStats_Equipment_Attributes ** DynamicAttributes_End;
		uint64_t U9[3];
		uint32_t MaxCharges;
		uint32_t Charges;
		uint8_t LevelGroupIndex;
		uint8_t RootGroupIndex;
		uint8_t NameGroupIndex;
		uint8_t NameIndex;
		uint32_t NameCool;
		ObjectSet<void *> Tags_Maybe;
		ObjectSet<FixedString> BoostNameSet;
		uint64_t U10[12];
	};

	struct EoCGameRandom
	{
		uint64_t Unkn0;
		uint32_t Unkn1[7];
		uint8_t Unkn2;
	};

	struct CharacterEquippedItem
	{
		uint32_t ItemStatsHandle;
		uint8_t ItemSlot;
		bool IsEquipped;
	};

	struct CharacterDynamicStat
	{
		uint64_t Unkn0;
		uint64_t Unkn1;

		int32_t SummonLifelinkModifier;
		int32_t Strength;
		int32_t Memory;
		int32_t Intelligence;
		int32_t Movement;
		int32_t MovementSpeedBoost;
		int32_t Finesse;
		int32_t Wits;
		int32_t Constitution;

		int32_t FireResistance;
		int32_t EarthResistance;
		int32_t WaterResistance;
		int32_t AirResistance;
		int32_t PoisonResistance;
		int32_t ShadowResistance;

		int32_t Willpower;
		int32_t Bodybuilding;
		int32_t PiercingResistance;
		int32_t PhysicalResistance;
		int32_t CorrosiveResistance;
		int32_t MagicResistance;
		int32_t CustomResistance;

		int32_t Sight;
		int32_t Hearing;
		int32_t FOV;
		int32_t APMaximum;
		int32_t APStart;
		int32_t APRecovery;
		int32_t CriticalChance;
		int32_t Initiative;
		int32_t Vitality;
		int32_t VitalityBoost;
		int32_t MagicPoints;
		int32_t Level;
		int32_t Gain;

		int32_t Armor;
		int32_t MagicArmor;
		int32_t ArmorBoost;
		int32_t MagicArmorBoost;
		int32_t ArmorBoostGrowthPerLevel;
		int32_t MagicArmorBoostGrowthPerLevel;
		int32_t DamageBoost;
		int32_t DamageBoostGrowthPerLevel;

		int32_t Accuracy;
		int32_t Dodge;
		int32_t MaxResistance;
		int32_t LifeSteal;
		int32_t Weight;
		int32_t ChanceToHitBoost;
		int32_t RangeBoost;
		int32_t APCostBoost;
		int32_t SPCostBoost;
		int32_t MaxSummons;
		int32_t Abilities[40];
		int32_t BonusWeaponDamageMultiplier;
		uint32_t Talents[4];
		uint32_t RemovedTalents[4];
		int16_t Traits[18];
		uint32_t BoostConditionsMask;
		FixedString TranslationKey;
		FixedString BonusWeapon;
		uint64_t Reflection;
		uint32_t Unkn2;
		uint32_t Unkn3;
		uint64_t AttributeFlags;
		uint32_t Unkn4;
	};

	struct CDivinityStats_Character : public CRPGStats_ObjectInstance
	{
		uint32_t _Pad0;
		uint32_t CurrentVitality;
		uint32_t CurrentArmor;
		uint32_t CurrentMagicArmor;
		uint32_t ArmorAfterHitCooldownMultiplier;
		uint32_t MagicArmorAfterHitCooldownMultiplier;
		uint32_t MPStart;
		uint32_t CurrentAP;
		uint32_t BonusActionPoints; // Saved
		uint32_t Experience; // Saved
		uint32_t Reputation;
		uint8_t Flanked; // Saved
		uint8_t _Pad1[3];
		uint32_t Karma; // Saved
		uint8_t Flags;
		uint8_t _Pad2[3];
		uint32_t Unkn0;
		ObjectSet<int> TraitOrder; // Saved
		uint32_t MaxResistance;
		uint32_t ItemsChanged_M;
		struct EsvCharacter * Character;
		uint32_t Unkn2;
		uint32_t IsIncapacitatedRefCount;
		CharacterDynamicStat ** DynamicStats;
		CharacterDynamicStat ** DynamicStatsEnd;
		uint64_t Unkn3[2];
		CharacterEquippedItem ** ItemStats;
		CharacterEquippedItem ** ItemStatsEnd;
		uint64_t Unkn4;
		ObjectSet<void *> SurfacePathInfluences;
		uint32_t ActiveBoostConditions[16]; // Saved
		EoCGameRandom DamageRng;
		uint8_t _Pad4[3];
		EoCGameRandom CriticalHitRng;
		uint8_t _Pad5[3];
		uint32_t MaxVitality;
		uint32_t BaseMaxVitality;
		uint32_t MaxArmor;
		uint32_t BaseMaxArmor;
		uint32_t MaxMagicArmor;
		uint32_t BaseMaxMagicArmor;
		uint32_t Sight;
		uint32_t BaseSight;
		uint32_t Flags0;
		uint32_t Flags1;
		uint32_t Flags2;
		uint32_t Flags3;
		uint32_t Unkn5[2];
		bool SomeComputePendingFlag;
		uint8_t _Pad6[3];
		uint32_t MaxSummons;
		uint32_t BaseMaxSummons;
		uint32_t MaxMpOverride; // Saved
		uint32_t DisabledTalents[4]; // Saved
	};

	struct RPGEnumeration
	{
		FixedString Name;
		FixedStringMapBase<int32_t> Values;
	};

	struct CRPGStats_Modifier
	{
		int32_t RPGEnumerationIndex;
		int32_t Unknown2;
		int32_t UnknownZero;
		uint32_t _Padding;
		FixedString Name;
	};

	template <class T>
	struct CNamedElementManager
	{
		void * VMT;
		void * PrimitiveSetVMT;
		PrimitiveSet<T *> Primitives;
		uint64_t Unknown;
		FixedStringMapBase<uint32_t> NameHashMap;
		uint32_t Unused;
		uint32_t NumItems;
		uint32_t NumSomeItems;

		int FindIndex(FixedString str) const
		{
			auto ptr = NameHashMap.Find(str.Str);
			if (ptr != nullptr) {
				return (int)*ptr;
			}
			else {
				return -1;
			}
		}

		T * Find(int index) const
		{
			if (index < 0 || index >= (int)Primitives.ItemCount) {
				return nullptr;
			}
			else {
				return Primitives.Buf[index];
			}
		}

		T * Find(FixedString str) const
		{
			auto ptr = NameHashMap.Find(str.Str);
			if (ptr != nullptr) {
				return Primitives.Buf[*ptr];
			}
			else {
				return nullptr;
			}
		}
	};

	struct ModifierList
	{
		CNamedElementManager<CRPGStats_Modifier> Attributes;
		FixedString Name;

		CRPGStats_Modifier * GetAttributeInfo(const char * name, int * attributeIndex) const;
	};

	struct CRPGStatsManager
	{
		CNamedElementManager<RPGEnumeration> modifierValueList;
		CNamedElementManager<ModifierList> modifierList;
		CNamedElementManager<CRPGStats_Object> objects;
		CNamedElementManager<uint64_t> levelMods;
		CNamedElementManager<uint64_t> deltaMods;
		CNamedElementManager<uint64_t> treasureSubtables;
		CNamedElementManager<uint64_t> treasureTables;
		CNamedElementManager<uint64_t> itemTypes;
		uint64_t unknown[47];
		PrimitiveSet<FixedString> ModifierFSSet;
		uint64_t unknown2[165];

		ModifierList * GetTypeInfo(CRPGStats_Object * object);
		RPGEnumeration * GetAttributeInfo(CRPGStats_Object * object, const char * attributeName, int & attributeIndex);
		std::optional<FixedString> GetAttributeFixedString(CRPGStats_Object * object, const char * attributeName);
		std::optional<int> GetAttributeInt(CRPGStats_Object * object, const char * attributeName);
		bool SetAttributeString(CRPGStats_Object * object, const char * attributeName, const char * value);
		bool SetAttributeInt(CRPGStats_Object * object, const char * attributeName, int32_t value);
		bool ObjectExists(FixedString statsId, FixedString type);
	};

	struct NetworkObjectFactory : public ObjectFactory
	{
		FixedStringMapBase<void *> CharacterMap;
		uint32_t _Pad;
		uint32_t Unkn4;
		uint32_t _Pad2;
		uint64_t Unkn5;
		uint32_t Unkn6;
		uint32_t _Pad3;
		uint64_t Unkn7[7];
		Array<unsigned short> ShortArray;
		uint64_t Unkn8;
	};

	struct ComponentEntry
	{
		// todo
		Component * component;
		uint64_t dummy[31];
	};

	struct EntityWorld
	{
		uint64_t Unkn[17];
		Array<ComponentEntry> Components;
	};

	struct CharacterFactory : public NetworkObjectFactory
	{
		void * VMT2;
		void * VMT3;
		FixedStringMapBase<void *> FSMap_ReloadComponent;
		uint32_t _Pad4;
		EntityWorld * Entities;
		uint64_t Unkn8[2];
	};

	struct ItemFactory : public NetworkObjectFactory
	{
		void * VMT2;
		void * VMT3;
		FixedStringMapBase<void *> FSMap_ReloadComponent;
		uint32_t _Pad4;
		EntityWorld * Entities;
		uint64_t Unkn8[2];
	};

	struct EsvStatusVMT
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

	struct EsvStatus
	{
		virtual ~EsvStatus() = 0;
		virtual void SetObjectHandle(ObjectHandle Handle) = 0;
		virtual void GetObjectHandle(ObjectHandle * Handle) = 0;
		virtual StatusType GetStatusId() = 0;
		// 3 - Toggle, 0 - Normal, 1 - ???, 2 - OnlyOnce?
		virtual uint32_t GetTriggerBehavior_M() = 0;
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
		uint32_t NetID;
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
		ObjectHandle TargetCIHandle;
		ObjectSet<ObjectHandle> StatusOwner; // Saved
		ObjectHandle StatusSourceHandle; // Saved
		ObjectHandle SomeHandle;
		uint8_t Flags2; // Saved
		uint8_t Flags0; // Saved
		uint8_t Flags1; // Saved
		uint8_t _Pad2;
		uint32_t _Pad3;
	};

	struct TDamagePair
	{
		int32_t Amount;
		DamageType DamageType;
	};

	struct DamagePairList : public Array<TDamagePair>
	{
		void AddDamage(DamageType DamageType, int32_t Amount);
	};

	struct HitDamageInfo
	{
		uint32_t Equipment{ 0 };
		int32_t TotalDamageDone{ 0 };
		uint32_t Unknown{ 0 };
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

		bool SetFlag(char const * flag);
		bool ClearFlag(char const * flag);
		std::optional<bool> HasFlag(char const * flag);

		std::optional<int> GetInt(char const * prop);
		bool SetInt(char const * prop, int value);

		std::optional<char const *> GetString(char const * prop);
		bool SetString(char const * prop, char const * value);


	};

	struct EsvStatusHit : public EsvStatus
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

	struct EsvStatusHeal : public EsvStatus
	{
		float EffectTime;
		uint32_t HealAmount;
		uint32_t HealEffect;
		uint8_t _Pad4[4];
		FixedString HealEffectId;
		uint32_t HealType;
		uint32_t AbsorbSurfaceRange;
		ObjectSet<void *> AbsorbSurfaceTypes;
		uint64_t Unkn2;
		bool TargetDependentHeal;
		uint8_t _Pad5[3];
		uint32_t TargetDependentValue[3];
		uint32_t TargetDependentHealAmount[3];
		uint32_t Unkn3;
	};

	struct EsvStatusConsume : public EsvStatus
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
		FixedString ApplyStatusOnTick;
		ObjectSet<ObjectHandle> ItemHandles; // Saved
		float EffectTime; // Saved
		FixedString StatsId; // Saved
		ObjectSet<void *> StatsIDs; // Set<StatsData>
		FixedString StackId;
		FixedString OriginalWeaponStatsId;
		FixedString OverrideWeaponStatsId;
		ObjectHandle OverrideWeaponHandle;
		int field_170;
		int SavingThrow; // TODO enum + enum prop!
		Vector3 SourceDirection; // Saved
		uint8_t _Pad10[4];
		PrimitiveSet<void *> SurfaceChangeSet; // Set<SurfaceChange>
		int field_198;
		int field_19C;
		int64_t field_1A0;
		int Turn; // Saved
		int field_1AC;
		EsvStatus * AuraStatus; // Saved
		int HealEffectOverride; // Saved // TODO enum + enum prop!
		char field_1BC;
		char field_1BD;
		char field_1BE;
		char field_1BF;
	};

	struct EsvStatusHealing : public EsvStatusConsume
	{
		uint32_t HealAmount; // Saved
		float TimeElapsed; // Saved
		uint32_t HealEffect; // Saved // TODO enum + enum prop!
		uint8_t _Pad5[4];
		FixedString HealEffectId; // Saved
		bool SkipInitialEffect; // Saved
		uint8_t _Pad6[3];
		uint32_t HealingEvent; // Saved
		uint32_t HealStat; // Saved // TODO enum + enum prop!
		uint32_t AbsorbSurfaceRange; // Saved
	};

	struct EsvStatusDamage : public EsvStatusConsume
	{
		int32_t DamageEvent; // Saved
		float HitTimer; // Saved
		float TimeElapsed; // Saved
		int DamageLevel; // Saved
		uint64_t DamageStats; // Saved
		bool SpawnBlood; // Saved
		uint8_t _Pad20[7];
	};


	struct EsvStatusDamageOnMove : public EsvStatusDamage
	{
		float DistancePerDamage; // Saved
		float DistanceTraveled; // Saved
		FixedString _Unknown;
	};

	struct EsvStatusActiveDefense : public EsvStatusConsume
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


	struct EsvStatusManager
	{
		uint64_t U1[8];
		uint8_t U2[240];
		EsvStatus ** Statuses;
		uint32_t U3;
		uint32_t StatusCount;
		uint64_t U4[4];
	};

	struct EsvSkillConditions
	{
		int32_t MinimumHealthPercentage;
		int32_t MaximumHealthPercentage;
		bool HasNoPhysicalArmor;
		bool HasNoMagicalArmor;
		uint8_t _Pad[6];
		ObjectSet<FixedString> Tags;
	};

	struct EsvSkillInfo
	{
		float ScoreModifier;
		int32_t StartRound;
		int32_t MinimumImpact;
		bool OnlyCastOnSelf;
		uint8_t AIFlags; // Enum
		uint8_t _Pad[2];
		EsvSkillConditions SourceConditions;
		EsvSkillConditions TargetConditions;
		bool CasualExplorer;
		bool Classic;
		bool TacticianHardcore;
		bool HonorHardcore;
		uint32_t Unknown;
	};

	struct EsvSkill
	{
		void * VMT;
		FixedString UnknownFS;
		uint32_t NetID;
		uint8_t _Pad[4];
		EsvSkillInfo Info;
		ObjectSet<ObjectHandle> CauseList;
		ObjectHandle UnknownHandle;
		uint32_t Unknown1;
		uint8_t _Pad2[4];
		ObjectHandle OwnerHandle;
		FixedString SkillId;
		float ActiveCooldown;
		bool IsActivated;
		bool IsLearned;
		bool ZeroMemory;
		bool OncePerCombat;
		bool Unknown2;
		uint8_t _Pad3[3];
		uint32_t NumCharges;
		uint8_t _Pad4[4];
		uint64_t Unknown3;
	};

	struct EsvSkillManager
	{
		void * FreeSkillState;
		ObjectHandle OwnerHandle;
		FixedStringMapBase<EsvSkill *> Skills;
		uint8_t _Pad[4];
		FixedStringRefMap<uint32_t> TimeItemAddedToSkillManager;
		bool IsLoading;
		uint8_t _Pad2[3];
		uint32_t SomeCount;
	};

	struct EsvSkillBarItem
	{
		enum ItemType : uint32_t
		{
			kNone = 0,
			kSkill = 1,
			kItem = 2
		};

		ItemType Type;
		uint32_t _Pad;
		FixedString SkillOrStatId;
		ObjectHandle ItemHandle;
	};

	struct EsvPlayerCustomData
	{
		void * VMT;
		bool Initialized;
		bool CustomLookEnabled;
		uint8_t _Pad[6];
		STDWString Name;
		TranslatedString NameTranslated;
		uint64_t Unkn1[18];
		FixedString ClassType;
		uint32_t SkinColor;
		uint32_t HairColor;
		uint32_t ClothColor1;
		uint32_t ClothColor2;
		uint32_t ClothColor3;
		bool IsMale;
		uint8_t _Pad2[3];
		FixedString Race;
		FixedString OriginName;
		FixedString Icon;
		FixedString MusicInstrument;
		FixedString OwnerProfileID;
		FixedString ReservedProfileID;
		FixedString AiPersonality;
		FixedString Speaker;
		void * CustomIconImg;
	};

	struct EsvPlayerData
	{
		ObjectHandle SomeObjectHandle;
		ObjectSet<EsvSkillBarItem> SkillBar;
		ObjectSet<uint32_t> LockedAbility;
		FixedStringMapBase<void *> ShapeShiftVariableManagers;
		uint8_t _Pad1[4];
		FixedStringMapBase<void *> ShapeShiftAttitudeMaps;
		uint8_t _Pad2[4];
		bool LevelUpMarker;
		uint8_t SelectedSkillSetIndex;
		uint8_t _Pad3[6];
		FixedString QuestSelected;
		EsvPlayerCustomData CustomData;
		uint64_t Unkn1[3];
		void * PreviousPickpocketTargets;
		ObjectHandle SomeObjectHandle2;
		ObjectSet<glm::vec3> PreviousPositions;
		uint32_t PreviousPositionId;
		bool HelmetOption;
		bool Dirty;
		uint8_t _Pad4[2];
		uint32_t Renown;
		uint8_t CachedTension;
		uint8_t _Pad5[3];
		ObjectHandle SomeObjectHandle3;
		uint8_t HomesteadKeyState;
		uint8_t _Pad6[7];
		ObjectHandle RecruiterHandle;
		FixedString OriginalTemplate;
		FixedString Region;
	};

	struct EocPlayerUpgrade
	{
		CDivinityStats_Character * Stats;
		uint32_t AttributePoints;
		uint32_t CombatAbilityPoints;
		uint32_t CivilAbilityPoints;
		uint32_t TalentPoints;
		uint64_t Unknown;
		ObjectSet<int> Attributes;
		ObjectSet<int> Abilities;
		uint32_t Talents[4];
		ObjectSet<uint16_t> Traits;
		bool IsCustom;
	};

	struct EsvCharacter
	{
		virtual ~EsvCharacter() = 0;
		virtual void VMT08() = 0;
		virtual uint64_t Ret5() = 0;
		virtual void SetObjectHandle(ObjectHandle Handle) = 0;
		virtual void GetObjectHandle(ObjectHandle * Handle) = 0;
		virtual void SetGuid(FixedString const & fs) = 0;
		virtual FixedString * GetGuid() = 0;
		virtual void SetNetID(uint32_t netId) = 0;
		virtual void GetNetID(uint32_t & netId) = 0;
		virtual void SetCurrentTemplate(void * esvTemplate) = 0;
		virtual void * GetCurrentTemplate() = 0;
		virtual void SetGlobal() = 0;
		virtual bool IsGlobal() = 0;
		virtual uint32_t GetComponentType() = 0;
		virtual void * GetEntityObjectByHandle(ObjectHandle handle) = 0;
		virtual STDWString * GetName() = 0;
		virtual void SetFlags(uint64_t flag) = 0;
		virtual void ClearFlags(uint64_t flag) = 0;
		virtual bool HasFlag(uint64_t flag) = 0;
		virtual void SetAiColliding(bool colliding) = 0;
		virtual void GetTags() = 0;
		virtual void IsTagged() = 0;
		virtual Vector3 const * GetTranslate() = 0;
		virtual glm::mat3 const * GetRotation() = 0;
		virtual float GetScale() = 0;
		virtual void SetTranslate(Vector3 const & translate) = 0;
		virtual void SetRotation(glm::mat3 const & rotate) = 0;
		virtual void SetScale(float scale) = 0;
		virtual Vector3 const * GetVelocity() = 0;
		virtual void SetVelocity(Vector3 const & translate) = 0;
		virtual void LoadVisual() = 0;
		virtual void UnloadVisual() = 0;
		virtual void ReloadVisual() = 0;
		virtual void GetVisual() = 0;
		virtual void GetPhysics() = 0;
		virtual void SetPhysics() = 0;
		virtual void LoadPhysics() = 0;
		virtual void UnloadPhysics() = 0;
		virtual void ReloadPhysics() = 0;
		virtual void GetHeight() = 0;
		virtual void GetParentUUID() = 0;
		virtual FixedString * GetCurrentLevel() = 0;
		virtual void SetCurrentLevel(FixedString const & level) = 0;
		virtual void AddPeer() = 0;
		virtual void UNK1() = 0;
		virtual void UNK2() = 0;
		virtual void UNK3() = 0;
		virtual void GetAi() = 0;
		virtual void LoadAi() = 0;
		virtual void UnloadAi() = 0;
		virtual void GetDisplayName() = 0;
		virtual void SavegameVisit() = 0;
		virtual void GetEntityNetworkId() = 0;
		virtual void SetTemplate() = 0;
		virtual void SetOriginalTemplate_M() = 0;

		EsvStatus * GetStatusByHandle(ObjectHandle handle) const;

		BaseComponent Base;
		FixedString MyGuid;
		uint32_t NetID;
		uint32_t _Pad1;
		glm::vec3 WorldPos;
		uint32_t _Pad2;
		EsvCharacterFlags Flags;
		uint32_t U2;
		uint32_t _Pad3;
		void * CurrentLevel;
		glm::mat3 WorldRot;
		float Scale;
		ObjectSet<void *> PeerIDClassNames;
		void * CurrentTemplate;
		void * OriginalTemplate;
		void * TemplateUsedForSkills;
		EsvCharacterFlags2 Flags2;
		EsvCharacterFlags3 Flags3;
		uint8_t FlagsEx;
		uint8_t Team;
		uint8_t Color;
		uint8_t _Pad4[3];
		uint64_t U4[6];
		void * AI;
		FixedString AnimationOverride;
		uint32_t WalkSpeedOverride;
		uint32_t RunSpeedOverride;
		ObjectSet<FixedString> VoiceSet;
		uint8_t NeedsUpdateCount;
		uint8_t ScriptForceUpdateCount;
		uint8_t ForceSynchCount;
		bool U5;
		uint8_t _Pad5[4];
		CDivinityStats_Character * Stats;
		ObjectHandle InventoryHandle;
		void * MovementMachine;
		void * ActionMachine;
		void * SteeringMachine;
		void * BehaviourMachine;
		uint64_t U7[2];
		void * OsirisController;
		void * RequestController;
		void * StatusController;
#if !defined(OSI_EOCAPP)
		uint64_t U8[2];
#endif
		void * ScriptController;
		void * TaskController;
		EsvStatusManager * StatusManager;
		EsvSkillManager * SkillManager;
		void * VariableManager;
		void * RaceVariableManager_M;
		void * Attitudes;
		uint64_t U9[4];
		uint32_t Dialog;
		bool IsDialogAiControlled;
		uint8_t U10[3];
		float LifeTime;
		float TurnTimer;
		float TriggerTrapsTimer;
		uint32_t U11;
		uint64_t U12;
		ObjectHandle OwnerHandle;
		ObjectHandle FollowCharacterHandle;
		ObjectHandle EnemyCharacterHandle;
		ObjectHandle SpiritCharacterHandle;
		ObjectHandle CorpseCharacterHandle;
		ObjectHandle ObjectHandle6;
		ObjectSet<ObjectHandle> EnemyHandles;
		ObjectSet<void *> SurfacePathInfluenceSet; // Set<eoc::SurfacePAthInfluence>
		ObjectSet<ObjectHandle> SummonHandles;
		void * PlanManager;
		uint32_t PartialAP;
		uint8_t AnimType;
		uint8_t DelayDeathCount;
		uint16_t _Pad6;
		void * StatusManagerDirty_M;
		ObjectSet<ObjectHandle> ObjectHandleSet3;
		ObjectSet<FixedString> RegisteredTriggers;
		EsvPlayerData * PlayerData;
		EocPlayerUpgrade PlayerUpgrade;
		uint8_t _Pad61[7];
		uint64_t U13[2];
		void * CustomDisplayName;
		void * StoryDisplayName;
		void * OriginalTransformDisplayName;
		uint64_t U14[20];
		uint32_t MaxVitalityPatchCheck;
		uint32_t MaxArmorPatchCheck;
		uint32_t MaxMagicArmorPatchCheck;
		uint32_t _Pad7;
		void * AnimationSetOverride;
		ObjectHandle PartyHandle;
		ObjectSet<FixedString> CreatedTemplateItems;
		void * Treasures;
		Set<ObjectHandle> CustomTradeTreasure;
		uint64_t U141;
		void * Target;
		uint64_t U15[3];
		ObjectHandle CrimeHandle;
		ObjectHandle PreviousCrimeHandle;
		uint32_t CrimeState;
		uint32_t PreviousCrimeState;
		bool IsAlarmed;
		bool CrimeWarningsEnabled;
		bool CrimeInterrogationEnabled;
		bool _Pad8;
		uint32_t InvestigationTimer;
		void * DisabledCrime;
		uint64_t U16[3];
		uint64_t DamageCounter;
		uint64_t HealCounter;
		uint64_t KillCounter;
		uint64_t U17;
		uint64_t Archetype;
		uint64_t EquipmentColor;
		uint64_t ProjectileTemplate;
		uint64_t TimeElapsed;
		uint64_t PreferredAiTarget;
		uint64_t U18[10];
		void * VisualSetIndices;
		bool ReadyCheckBlocked;
		bool CorpseLootable;
		uint8_t NumConsumables;
		uint8_t _Pad9[5];
		void * PreviousLevel;
	};

	struct EoCItemDefinition
	{
		uint32_t Unknown;
		uint32_t NetId;
		uint32_t ItemNetId;
		uint8_t _Pad0[4];
		FixedString FS1;
		// eg. "f14b8136-c4c6-4d7a-bc04-639d5a2397e7
		FixedString RootTemplate;
		uint32_t Unkn1;
		uint8_t _Pad1[4];
		// eg. "f14b8136-c4c6-4d7a-bc04-639d5a2397e7"
		FixedString OriginalRootTemplate;
		uint32_t Unkn2[4];
		uint32_t Unkn3;
		float Unkn3Flt;
		uint32_t Unkn4[2];
		float Unkn4Flt;
		uint32_t Unkn41[4];
		uint32_t Amount;
		uint32_t InventoryNetID;
		uint32_t InventorySubContainerNetID;
		uint16_t Slot;
		uint8_t _Pad3[2];
		uint32_t Unkn6;
		uint32_t GoldValueOverwrite;
		uint32_t WeightValueOverwrite;
		uint32_t DamageTypeOverwrite;
		uint32_t SomeOverwrite;
		FixedString FS4;
		// eg. "Uncommon"
		FixedString ItemType;
		STDWString CustomDisplayName;
		STDWString CustomDescription;
		STDWString CustomBookContent;
		STDWString GenerationBase;
		// eg. "WPN_Shield"
		FixedString GenerationStatsId;
		// eg. "Uncommon"
		FixedString GenerationItemType;
		uint32_t GenerationRandom;
		uint16_t GenerationLevel;
		uint8_t _Pad4[2];
		ObjectSet<FixedString> GenerationBoosts;
		uint8_t LevelGroupIndex;
		uint16_t RootGroupIndex;
		uint8_t NameIndex;
		uint8_t NameCool;
		uint8_t _Pad5[3];
		uint32_t Unkn;
		uint8_t _Pad6[4];
		FixedString StatsName;
		uint32_t Level;
		uint32_t Unkn11;
		// eg. "WPN_Shield"
		FixedString StatsEntryName;
		uint32_t EquipmentStatsType;
		uint8_t _Pad7[4];
		ScratchBuffer PermanentBoostsBuf;
		uint8_t _Pad8[4];
		ScratchBuffer BaseStatsBuf;
		uint8_t _Pad9[4];
		bool HasModifiedSkills;
		uint8_t _Pad10[7];
		FixedString Skills;
		ObjectSet<FixedString> FSSet2;
		ObjectSet<FixedString> RuneBoostNames;
		uint8_t Flags0[2];
		uint8_t HasItemGeneration_M;
		uint8_t Flags1[12];
		uint8_t IsIdentified;
		uint8_t Flags2[3];
	};
	
	struct EsvItemGeneration
	{
		FixedString Base; // Saved
		FixedString ItemType; // Saved
		uint32_t Random; // Saved
		uint16_t Level; // Saved
		uint16_t _Pad0;
		Set<FixedString> Boosts; // Saved
	};

	struct EsvItem
	{
		virtual ~EsvItem() = 0;
		virtual void HandleTextKeyEvent() = 0;
		virtual uint64_t Ret5() = 0;
		virtual void SetObjectHandle(ObjectHandle Handle) = 0;
		virtual void GetObjectHandle(ObjectHandle * Handle) = 0;
		virtual void SetGuid(FixedString const & fs) = 0;
		virtual FixedString * GetGuid() = 0;
		virtual void SetNetID(uint32_t netId) = 0;
		virtual void GetNetID(uint32_t & netId) = 0;
		virtual void SetCurrentTemplate() = 0;
		virtual void GetCurrentTemplate() = 0;
		virtual void SetGlobal() = 0;
		virtual void GetGlobal() = 0;
		virtual void GetComponentType() = 0;
		virtual void GetEntityObjectByHandle() = 0;
		virtual void GetName() = 0;
		virtual void SetFlags(uint64_t flag) = 0;
		virtual void ClearFlags(uint64_t flag) = 0;
		virtual void HasFlag() = 0;
		virtual void SetAiColliding() = 0;
		virtual void GetTags() = 0;
		virtual void IsTagged() = 0;
		virtual Vector3 const * GetTranslate() = 0;
		virtual void GetRotation() = 0;
		virtual float GetScale() = 0;
		virtual void SetTranslate(Vector3 const & translate) = 0;
		virtual void SetRotation() = 0;
		virtual void SetScale(float scale) = 0;
		virtual void GetVelocity() = 0;
		virtual void SetVelocity(Vector3 const & translate) = 0;
		virtual void LoadVisual() = 0;
		virtual void UnloadVisual() = 0;
		virtual void ReloadVisual() = 0;
		virtual void GetVisual() = 0;
		virtual void GetPhysics() = 0;
		virtual void SetPhysics() = 0;
		virtual void LoadPhysics() = 0;
		virtual void UnloadPhysics() = 0;
		virtual void ReloadPhysics() = 0;
		virtual void GetHeight() = 0;
		virtual void GetParentUUID() = 0;
		virtual FixedString * GetCurrentLevel() = 0;
		virtual void SetCurrentLevel(FixedString const & level) = 0;
		virtual void AddPeer() = 0;
		virtual void UNK1() = 0;
		virtual void UNK2() = 0;
		virtual void UNK3() = 0;
		virtual void GetAi() = 0;
		virtual void LoadAi() = 0;
		virtual void UnloadAi() = 0;
		virtual void GetDisplayName() = 0;
		virtual void SavegameVisit() = 0;
		virtual void GetEntityNetworkId() = 0;
		virtual void SetTemplate() = 0;
		virtual void SetOriginalTemplate_M() = 0;

		BaseComponent Base;
		FixedString MyGuid;
		uint32_t NetID;
		uint32_t _Pad1;
		glm::vec3 WorldPos; // Saved
		uint32_t _Pad2;
		uint64_t Flags2; // Saved
		uint32_t U2;
		uint32_t _Pad3;
		void * CurrentLevel; // Saved
		glm::mat3 WorldRot; // Saved
		float Scale; // Saved
		uint8_t Flags3;
		uint8_t _Pad4[7];
		uint64_t U3[4];
		glm::vec3 WorldVelocity; // Saved
		uint32_t _Pad5;
		uint64_t U4;
		FixedString CurrentTemplate; // Saved
		uint64_t OriginalTemplateType; // Saved
		STDWString CustomDisplayName; // Saved
		STDWString CustomDescription; // Saved
		STDWString CustomBookContent; // Saved
		FixedString StatsId; // Saved
		CDivinityStats_Item * StatsDynamic;
		CRPGStats_Object * StatsFromName;
		EsvItemGeneration * Generation; // Saved
		ObjectHandle InventoryHandle; // Saved
		ObjectHandle ParentInventoryHandle; // Saved
		uint16_t Slot; // Saved
		uint16_t _Pad6;
		uint32_t Amount; // Saved
		uint32_t Vitality; // Saved
		uint32_t Armor; // Saved
		ObjectHandle InUseByCharacterHandle;
		uint64_t U5[2];
		uint32_t LockLevel; // Saved
		uint32_t _Pad7;
		void * ItemMachine; // Saved
		void * PlanMachine; // Saved
		void * VariableManager; // Saved
		EsvStatusManager * StatusMachine; // Saved
		void * U6;
		ObjectHandle OwnerHandle; // Saved
		ObjectHandle OriginalOwnerCharacter; // Saved
		void * Sockets; // Saved
		uint64_t U7;
		uint32_t ComputedVitality;
		uint32_t MaxDurabilityPatchCheck; // Saved
		FixedString ItemType; // Saved
		uint32_t GoldValueOverwrite; // Saved
		uint32_t WeightValueOverwrite; // Saved
		uint64_t U8;
		void * Tags; // Saved
		void * DynamicTags;
		uint32_t U9;
		uint32_t HasDynamicTags;
		uint64_t U10;
		uint64_t TeleportTargetOverride; // Saved
		uint32_t TreasureLevel; // Saved
		uint32_t LevelOverride; // Saved
		bool ForceSynch; // Saved
		uint8_t _Pad8[3];
		uint32_t TeleportUseCount; // Saved
		uint64_t PreviousLevel; // Saved
	};

	struct ShootProjectileHelper
	{
		FixedString SkillId;
		ObjectHandle SourceCharacter;
		ObjectHandle Target;
		ObjectHandle Target2;
		glm::vec3 StartPosition;
		glm::vec3 EndPosition;
		uint8_t Random;
		uint8_t _Pad[7];
		void * DamageList;
		int32_t CasterLevel;
		uint32_t _Pad2;
		void * CollisionInfo;
		bool IsTrap;
		bool UnknownFlag1;
		uint8_t _Pad3[6];
		FixedString FS2;
		float StatusClearChance;
		bool HasCaster;
		bool IsStealthed;
		bool UnknownFlag2;
	};

	struct EsvGameAction
	{
		void * VMT;
		FixedString SomeFS;
		uint64_t NetID;
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

	struct EsvGameActionManager
	{
		uint8_t Unmapped[0x130];
		Set<EsvGameAction *> GameActions;
	};

	struct EsvTornadoAction : public EsvGameAction
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

	struct EsvStormAction : public EsvGameAction
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

	struct EsvRainAction : public EsvGameAction
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

	struct EsvWallAction : public EsvGameAction
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

	struct EsvStatusDomeAction : public EsvGameAction
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

	struct EsvGameObjectMoveAction : public EsvGameAction
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

	struct SummonHelperResults
	{
		ObjectHandle SummonHandle;
		uint32_t Unknown;
	};

	struct SummonHelperSummonArgs
	{
		ObjectHandle OwnerCharacterHandle;
		FixedString GameObjectTemplateFS;
		FixedString Level;
		glm::vec3 Position;
		uint32_t SummonLevel;
		float Lifetime;
		bool IsTotem;
		bool MapToAiGrid;
	};
#pragma pack(pop)

	typedef void(*ProjectileHelpers_ShootProjectile)(void * ShootProjectileHelper, float Unknown);
	typedef void * (*GameActionManager__CreateAction)(EsvGameActionManager * GameActionManager, GameActionType actionId, uint64_t SomeHandle);
	typedef void(*GameActionManager__AddAction)(EsvGameActionManager * GameActionManager, void * Action);
	typedef void(*TornadoAction__Setup)(void * TornadoAction);
	typedef void(*GameObjectMoveAction__Setup)(void * Action, ObjectHandle & ObjectToMove, glm::vec3 * TargetPosition);
	typedef void(*SummonHelpers__Summon)(SummonHelperResults * Results, SummonHelperSummonArgs * Args);
	typedef EsvStatus * (*StatusMachine__CreateStatus)(void * StatusMachine, FixedString & StatusId, uint64_t ObjectHandle);
	typedef void(*StatusMachine__ApplyStatus)(void * StatusMachine, EsvStatus * Status);
	typedef void(*Character__Hit)(EsvCharacter * self, CDivinityStats_Character * attackerStats, CDivinityStats_Item * itemStats, DamagePairList * damageList,
		uint32_t hitType, bool rollForDamage, HitDamageInfo * damageInfo, int forceReduceDurability, void * skillProperties, HighGroundBonus highGroundFlag, bool procWindWalker, CriticalRoll criticalRoll);
	typedef bool(*Status__Enter)(EsvStatus * Status);
	typedef void(*esv__ParseItem)(EsvItem * Item, ObjectSet<EoCItemDefinition> * ParsedItems, bool CopyNetId, bool CopyContainerContents);
	typedef EsvItem * (*esv__CreateItemFromParsed)(ObjectSet<EoCItemDefinition> * ParsedItems, uint32_t Index);

}