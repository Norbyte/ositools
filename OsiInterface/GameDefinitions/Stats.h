#pragma once

#include "BaseTypes.h"
#include "Enumerations.h"
#include "Wrappers.h"

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
		uint64_t AttributeFlags;
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
		int32_t AnimType; // -1 = Not overridden
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
		int32_t DamageTypeOverwrite; // Saved // -1 = Not overridden
		uint32_t Durability; // Saved
		uint32_t DurabilityCounter; // Saved
		uint32_t U7;
		FixedString ItemTypeReal; // Saved
		FixedString U8;
		CDivinityStats_Equipment_Attributes ** DynamicAttributes_Start;
		CDivinityStats_Equipment_Attributes ** DynamicAttributes_End;
		uint64_t U9[3];
		int32_t MaxCharges; // -1 = Not overridden
		int32_t Charges; // -1 = Not overridden
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
		uint32_t StepsType;
		uint64_t AttributeFlagsObjectId;
		uint32_t Unkn4;

		bool RemoveTalent(TalentType talent, bool remove);
		bool IsTalentRemoved(TalentType talent);
	};

	namespace esv { struct Character; }

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
		esv::Character * Character;
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
		uint64_t AttributeFlags;
		uint64_t BaseAttributeFlags;
		uint64_t ItemBoostedAttributeFlags;
		bool AttributeFlagsUpdated;
		uint8_t _Pad6[3];
		uint32_t MaxSummons;
		uint32_t BaseMaxSummons;
		uint32_t MaxMpOverride; // Saved
		uint32_t DisabledTalents[4]; // Saved

		std::optional<int32_t> GetStat(char const * name, bool baseStats);
		std::optional<int32_t> GetHitChance(CDivinityStats_Character * target);
	};

	typedef int32_t (CDivinityStats_Character__GetStat)(CDivinityStats_Character * self, bool baseStats);
	typedef int32_t (CDivinityStats_Character__GetStatWithBoosts)(CDivinityStats_Character * self, bool baseStats, bool excludeBoosts);
	typedef int32_t (CDivinityStats_Character__GetStatWithInit)(CDivinityStats_Character * self, bool baseStats, int32_t initialValue);
	typedef float (CDivinityStats_Character__GetStatHearing)(CDivinityStats_Character * self, CDivinityStats_Character * other, bool baseStats);
	typedef int32_t (CDivinityStats_Character__GetHitChance)(CDivinityStats_Character * attacker, CDivinityStats_Character * target);

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
			} else {
				return -1;
			}
		}

		T * Find(int index) const
		{
			if (index < 0 || index >= (int)Primitives.ItemCount) {
				return nullptr;
			} else {
				return Primitives.Buf[index];
			}
		}

		T * Find(FixedString str) const
		{
			auto ptr = NameHashMap.Find(str.Str);
			if (ptr != nullptr) {
				return Primitives.Buf[*ptr];
			} else {
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
		uint64_t unknown[46];
		ObjectSet<FixedString> ModifierFSSet;
		ObjectSet<uint64_t> AttributeFlags;
		uint64_t unknown2[165];

		ModifierList * GetTypeInfo(CRPGStats_Object * object);
		RPGEnumeration * GetAttributeInfo(CRPGStats_Object * object, const char * attributeName, int & attributeIndex);
		std::optional<FixedString> GetAttributeFixedString(CRPGStats_Object * object, const char * attributeName);
		std::optional<int> GetAttributeInt(CRPGStats_Object * object, const char * attributeName);
		bool SetAttributeString(CRPGStats_Object * object, const char * attributeName, const char * value);
		bool SetAttributeInt(CRPGStats_Object * object, const char * attributeName, int32_t value);
		bool ObjectExists(FixedString statsId, FixedString type);
	};
#pragma pack(pop)

	template <class TTag>
	std::optional<int32_t> CharacterStatGetter(CDivinityStats_Character__GetStat * getter,
		WrappableFunction<TTag, CDivinityStats_Character__GetStat> & wrapper,
		CDivinityStats_Character * character, bool original, bool baseStats)
	{
		if (getter == nullptr) {
			return {};
		}

		if (original || !wrapper.IsWrapped()) {
			return getter(character, baseStats);
		} else {
			return wrapper.CallWithHooks(character, baseStats);
		}
	}

	template <class TTag>
	std::optional<int32_t> CharacterStatGetter(CDivinityStats_Character__GetStatWithBoosts * getter,
		WrappableFunction<TTag, CDivinityStats_Character__GetStatWithBoosts> & wrapper,
		CDivinityStats_Character * character, bool original, bool baseStats)
	{
		if (getter == nullptr) {
			return {};
		}

		if (original || !wrapper.IsWrapped()) {
			return getter(character, baseStats, false);
		} else {
			return wrapper.CallWithHooks(character, baseStats, false);
		}
	}

	template <class TTag>
	std::optional<int32_t> CharacterStatGetter(CDivinityStats_Character__GetStatWithInit * getter,
		WrappableFunction<TTag, CDivinityStats_Character__GetStatWithInit> & wrapper,
		CDivinityStats_Character * character, bool original, bool baseStats)
	{
		if (getter == nullptr) {
			return {};
		}

		if (original || !wrapper.IsWrapped()) {
			return getter(character, baseStats, 0);
		} else {
			return wrapper.CallWithHooks(character, baseStats, 0);
		}
	}

	template <class TTag>
	std::optional<int32_t> CharacterStatGetter(CDivinityStats_Character__GetStatHearing * getter,
		WrappableFunction<TTag, CDivinityStats_Character__GetStatHearing> & wrapper,
		CDivinityStats_Character * character, bool original, bool baseStats)
	{
		if (getter == nullptr) {
			return {};
		}

		if (original || !wrapper.IsWrapped()) {
			return (int)(getter(character, nullptr, baseStats) * 100.0f);
		} else {
			return (int)(wrapper.CallWithHooks(character, nullptr, baseStats) * 100.0f);
		}
	}

	struct CharacterStatsGetters
	{

		union {
			void * Ptrs[26];
			struct {
#define DEFN_GETTER(type, name) CDivinityStats_Character__Get##type * Get##name;
#include <GameDefinitions/CharacterGetters.inl>
#undef DEFN_GETTER

				CDivinityStats_Character__GetHitChance * GetHitChance;
			};
		};

#define DEFN_GETTER(type, name) \
	enum class name##Tag{}; \
	WrappableFunction<name##Tag, CDivinityStats_Character__Get##type> Wrapper##name;

#include <GameDefinitions/CharacterGetters.inl>
#undef DEFN_GETTER

		enum class HitChanceTag{}; \
		WrappableFunction<HitChanceTag, CDivinityStats_Character__GetHitChance> WrapperHitChance;

		bool Wrapped{ false };

		void WrapAll();
		void ResetExtension();

		std::optional<int32_t> GetStat(CDivinityStats_Character * character, char const * name, 
			bool original, bool baseValues);
	};

	extern CharacterStatsGetters gCharacterStatsGetters;
}
