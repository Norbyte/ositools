#pragma once

#include "BaseTypes.h"
#include "Enumerations.h"
#include "Wrappers.h"

namespace osidbg
{

#pragma pack(push, 1)
	struct CRPGStats_Requirement
	{
		RequirementType RequirementId;
		int32_t IntParam;
		FixedString StringParam;
		bool Negate;
		uint8_t _Pad[7];
	};

	template <class T>
	struct CNamedElementManager : public ProtectedGameObject<CNamedElementManager<T>>
	{
		void * VMT;
		void * PrimitiveSetVMT;
		PrimitiveSet<T *> Primitives;
		uint64_t Unknown;
		FixedStringMapBase<uint32_t> NameHashMap;
		uint32_t Unused;
		uint32_t NumItems;
		uint32_t NumSomeItems;

		int FindIndex(char const * str) const
		{
			auto fs = ToFixedString(str);
			if (fs) {
				return FindIndex(fs);
			} else {
				return -1;
			}
		}

		int FindIndex(FixedString str) const
		{
			auto ptr = NameHashMap.Find(str);
			if (ptr != nullptr) {
				return (int)*ptr;
			} else {
				return -1;
			}
		}

		T * Find(int index) const
		{
			if (index < 0 || index >= (int)Primitives.Size) {
				return nullptr;
			} else {
				return Primitives.Buf[index];
			}
		}

		T * Find(char const * str) const
		{
			auto fs = ToFixedString(str);
			if (fs) {
				return Find(fs);
			} else {
				return nullptr;
			}
		}

		T * Find(FixedString str) const
		{
			auto ptr = NameHashMap.Find(str);
			if (ptr != nullptr) {
				return Primitives.Buf[*ptr];
			} else {
				return nullptr;
			}
		}
	};

	struct CRPGStats_Object_Property : public Noncopyable<CRPGStats_Object_Property>
	{
		virtual ~CRPGStats_Object_Property() {}

		FixedString SomeHashedText;
	};

	struct CDivinityStats_Object_Property_Data : public CRPGStats_Object_Property
	{
		virtual ~CDivinityStats_Object_Property_Data() {}
		virtual CDivinityStats_Object_Property_Data * Clone() = 0;
		virtual bool GetDescription(STDWString * Line1) = 0;
		virtual bool GetDescription(TranslatedString * Line1, TranslatedString * Line2) = 0;
		virtual bool GetDescription(TranslatedString * Line1) = 0;
		virtual uint64_t Unknown() = 0;

		uint32_t TypeId;
		uint8_t PropertyContext;
		uint8_t _Pad1[3];
		void * ConditionBlockPtr;
	};

	struct CRPGStats_Object_Property_CustomDescription : public CDivinityStats_Object_Property_Data
	{
		CRPGStats_Object_Property_CustomDescription()
		{}

		virtual ~CRPGStats_Object_Property_CustomDescription() {}

		virtual CDivinityStats_Object_Property_Data * Clone()
		{
			auto cl = new CRPGStats_Object_Property_CustomDescription();
			cl->SomeHashedText = SomeHashedText;
			cl->TypeId = TypeId;
			cl->PropertyContext = PropertyContext;
			cl->ConditionBlockPtr = ConditionBlockPtr;
			cl->TextLine1 = TextLine1;
			return cl;
		}

		virtual bool GetDescription(STDWString * Line1)
		{
			Line1->Set(TextLine1);
			return true;

		}

		virtual bool GetDescription(TranslatedString * Line1, TranslatedString * Line2)
		{
			GetDescription(Line1);
			return true;
		}

		virtual bool GetDescription(TranslatedString * Line1)
		{
			GetDescription(&Line1->Str1.WStr);
			return true;
		}

		virtual uint64_t Unknown()
		{
			return 0;
		}

		std::wstring TextLine1;
	};


	struct CRPGStats_Object_Property_List
	{
		CNamedElementManager<CRPGStats_Object_Property> Properties;
		FixedString StatsObjName_PropertyName;
		bool Unknown;
		uint8_t _Pad[7];
	};

	struct CRPGStats_LevelMap : public ProtectedGameObject<CRPGStats_LevelMap>
	{
		virtual void Destroy() = 0;
		virtual void Unknown08(int a, int b) = 0;
		virtual void LoadFromModifierList(FixedString modifierListName, FixedString modifierName) = 0;
		virtual int64_t GetScaledValue(int difficultyScale, int level) = 0;

		int ModifierListIndex;
		int ModifierIndex;
		int RPGEnumerationIndex;
		uint8_t _Pad[4];
		FixedString Name;
	};

	struct CRPGStats_Object : public ProtectedGameObject<CRPGStats_Object>
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
		FixedString FS2;
		struct CDivinityStats * DivStats;
		FixedStringMapBase<CRPGStats_Object_Property_List *> PropertyList;
		uint32_t Unused5;
		FixedStringMapBase<void *> ConditionList;
		uint32_t Unused6;
		uint64_t AIFlags;
		ObjectSet<CRPGStats_Requirement> Requirements;
		ObjectSet<CRPGStats_Requirement> MemorizationRequirements;
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

	struct CDivinityStats_Equipment_Attributes : public ProtectedGameObject<CDivinityStats_Equipment_Attributes>
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
		FixedString BoostName;
		EquipmentStatsType StatsType;
		BitArray<4> Talents;
		uint32_t Unkn4;
		uint64_t AttributeFlagsObjectId;

		struct PropertyMapBase & GetPropertyMap() const;
	};

	struct CDivinityStats_Equipment_Attributes_Weapon : public CDivinityStats_Equipment_Attributes
	{
		DamageType DamageType;
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
		EquipmentStatsType ItemType;
		uint32_t ItemSlot;
		WeaponType WeaponType;
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
		DamageType DamageTypeOverwrite; // Saved // -1 = Not overridden
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
		int32_t ItemStatsHandle;
		ItemSlot ItemSlot;
		bool IsEquipped;
	};

	struct CharacterDynamicStat : public ProtectedGameObject<CharacterDynamicStat>
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
		BitArray<4> Talents;
		BitArray<4> RemovedTalents;
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
		int32_t ActiveBoostConditions[16]; // Saved
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
		float Sight;
		float BaseSight;
		uint64_t AttributeFlags;
		uint64_t BaseAttributeFlags;
		uint64_t ItemBoostedAttributeFlags;
		bool AttributeFlagsUpdated;
		uint8_t _Pad6[3];
		uint32_t MaxSummons;
		uint32_t BaseMaxSummons;
		uint32_t MaxMpOverride; // Saved
		BitArray<4> DisabledTalents; // Saved

		std::optional<int32_t> GetStat(char const * name, bool baseStats);
		std::optional<int32_t> GetHitChance(CDivinityStats_Character * target);
		bool HasTalent(TalentType talent, bool excludeBoosts);
		int32_t GetAbility(AbilityType ability, bool excludeBoosts);
		CDivinityStats_Item * GetItemBySlot(ItemSlot slot, bool mustBeEquipped);
		CDivinityStats_Item * GetMainWeapon();
		CDivinityStats_Item * GetOffHandWeapon();
		int32_t GetDamageBoost();
		bool IsBoostActive(uint32_t conditionsMask);
	};

	typedef int32_t (CDivinityStats_Character__GetStat)(CDivinityStats_Character * self, bool baseStats);
	typedef int32_t (CDivinityStats_Character__GetStatWithBoosts)(CDivinityStats_Character * self, bool baseStats, bool excludeBoosts);
	typedef int32_t (CDivinityStats_Character__GetStatWithInit)(CDivinityStats_Character * self, bool baseStats, int32_t initialValue);
	typedef float (CDivinityStats_Character__GetStatHearing)(CDivinityStats_Character * self, CDivinityStats_Character * other, bool baseStats);
	typedef int32_t (CDivinityStats_Character__GetHitChance)(CDivinityStats_Character * attacker, CDivinityStats_Character * target);
	typedef bool (CDivinityStats_Character__GetTalent)(CDivinityStats_Character * self, TalentType talentId, char excludeBoosts);
	typedef int32_t (CDivinityStats_Character__GetAbility)(CDivinityStats_Character * self, AbilityType abilityId, char excludeBoosts, char noLoneWolfBonus);
	
	typedef void (* eoc__SkillPrototypeManager__Init)(void * self);

	template <class T>
	struct CHandleArray : public ProtectedGameObject<CHandleArray<T>>
	{
		__int64 VMT;
		T ** Start;
		T ** End;
		T ** BufAllocationTail;
		__int64 field_20;

		inline std::size_t Size() const
		{
			return End - Start;
		}
	};

	template <class T>
	struct CDivinityStatsList : public ProtectedGameObject<CDivinityStatsList<T>>
	{
		void * VMT;
		CHandleArray<T> Handles;
		struct CDivinityStats * Stats;

		T * Get(int32_t index) const
		{
			if (index >= 0 && index < Handles.Size()) {
				return Handles.Start[index];
			} else {
				return nullptr;
			}
		}
	};

	struct CDivinityStats : public ProtectedGameObject<CDivinityStats>
	{
		void * VMT;
		bool SomeFlag;
		uint8_t _Pad[7];
		CDivinityStatsList<CDivinityStats_Item> * ItemList;
		CDivinityStatsList<CDivinityStats_Character> * CharacterList;
		char SomeBuf0x50b[80];
		CDivinityStats_Item * DefaultWeaponStats;
		ObjectSet<FixedString> FSSetUnkn;
		uint64_t Unkn;
	};

	struct RPGEnumeration : public ProtectedGameObject<RPGEnumeration>
	{
		FixedString Name;
		FixedStringMapBase<int32_t> Values;
	};

	struct CRPGStats_Modifier : public ProtectedGameObject<CRPGStats_Modifier>
	{
		int32_t RPGEnumerationIndex;
		int32_t LevelMapIndex;
		int32_t UnknownZero;
		uint32_t _Padding;
		FixedString Name;
	};

	struct ModifierList
	{
		CNamedElementManager<CRPGStats_Modifier> Attributes;
		FixedString Name;

		CRPGStats_Modifier * GetAttributeInfo(const char * name, int * attributeIndex) const;
	};

	struct CRPGStats_ExtraData : public ProtectedGameObject<CRPGStats_ExtraData>
	{
		FixedStringMapBase<float> Properties;
	};

	struct CDivinityStats_Condition : public ProtectedGameObject<CDivinityStats_Condition>
	{
		void * ScriptCheckBlock;
		FixedString Id;
	};

	struct CRPGStats_Conditions_Manager
	{
		FixedStringMapBase<CDivinityStats_Condition *> Conditions;
		uint8_t _Pad1[4];
		ObjectSet<STDString> Strs;
	};

	struct CRPGStats_ItemType_Manager : public CNamedElementManager<uint64_t>
	{
		uint64_t Unknown;
	};

	struct TextBuffer
	{
		wchar_t * Buf;
		uint64_t Capacity;
		uint64_t Length;
		uint64_t Unknown;

		void Replace(std::wstring const & replacement);
	};

	namespace eoc
	{
		struct Text
		{
			struct Param
			{
				int32_t PlaceholderOffset;
				int32_t Unknown[2];
				int32_t PlaceholderSize;
			};

			void * VMT;
			TextBuffer * Buf;
			uint32_t Unknown[2];
			Param Params[8];

			void ReplaceParam(int index, std::wstring const & replacement);
		};
	}

	struct SkillPrototype
	{
		typedef void (* FormatDescriptionParam)(SkillPrototype *skillPrototype, CDivinityStats_Character *tgtCharStats, 
			eoc::Text *eocText, int paramIndex, __int64 isFromItem, float xmm9_4_0, FixedString * paramText, 
			ObjectSet<STDString> * stdStringSet);

		typedef void (* GetSkillDamage)(SkillPrototype * self, struct DamagePairList * damageList, 
			CDivinityStats_Character *attackerStats, bool isFromItem, bool stealthed, float * attackerPosition, 
			float * targetPosition, DeathType * pDeathType, int level, bool noRandomization);

		// void * VMT;
		int RPGStatsObjectIndex;
		SkillType SkillTypeId;
		FixedString SkillId;
		uint16_t Ability;
		uint8_t _Pad[2];
		uint32_t Tier;
		uint32_t Requirement;
		uint32_t Level;
		uint32_t MagicCost;
		uint32_t MemoryCost;
		uint32_t ActionPoints;
		float Cooldown;
		float CooldownReduction;
		float ChargeDuration;
		STDWString DisplayName;
		FixedString Icon;
		uint64_t AiFlags;
		SkillPrototype * RootSkillPrototype;
		uint64_t Unknown0[2];
		uint32_t Unknown1;
		uint32_t RootSkillPrototypeIndex_M;
		uint64_t Unknown2[6];
	};

	struct StatusPrototype
	{
		typedef void (* FormatDescriptionParam)(StatusPrototype *prototype, CDivinityStats_Character *statusSource,
			CDivinityStats_Character *targetCharacter, float multiplier, eoc::Text * text, int paramIndex, 
			FixedString * param, ObjectSet<STDString> * paramSet);

		void * VMT;
		int RPGStatsObjectIndex;
		int StatusIdExternal;
		FixedString StatusName;
		TranslatedString translatedString1;
		FixedString FS2;
		bool HasStats;
		uint8_t _Pad[7];
		uint64_t Unknown;
	};


	struct CRPGStatsManager : public ProtectedGameObject<CRPGStatsManager>
	{
		CNamedElementManager<RPGEnumeration> modifierValueList;
		CNamedElementManager<ModifierList> modifierList;
		CNamedElementManager<CRPGStats_Object> objects;
		CNamedElementManager<CRPGStats_LevelMap> LevelMaps;
		CNamedElementManager<uint64_t> deltaMods;
		CNamedElementManager<uint64_t> treasureSubtables;
		CNamedElementManager<uint64_t> treasureTables;
		CRPGStats_ItemType_Manager itemTypes;
		FixedStringMapBase<CRPGStats_Object_Property_List> PropertyLists;
		uint8_t _Pad1[4];
		CRPGStats_Conditions_Manager ConditionsManager;
		STDWString WStr1;
		uint64_t Unkn1[5];
		CRPGStats_ExtraData * ExtraData;
		FixedStringRefMap<void *> RefMap1;
		FixedStringRefMap<void *> RefMap2;
		FixedStringMapBase<FixedString> FSMap1;
		uint8_t _Pad2[4];
		FixedStringMapBase<int> FSMapInt1;
		uint8_t _Pad3[4];
		FixedStringMapBase<int> FSMapInt2;
		uint8_t _Pad4[4];
		FixedStringMapBase<int> FSMapInt3;
		uint8_t _Pad5[4];
		FixedStringRefMap<void *> FSMap2;
		uint64_t Unkn2[7];
		ObjectSet<FixedString> ModifierFSSet;
		ObjectSet<uint64_t> AttributeFlags;
		uint64_t unknown2[140];
		void * EquipmentSetManager;
		void * SkillSetManager;
		uint64_t Unkn3;
		void * ItemCombinationManager;
		uint64_t Unkn4;
		FixedString FS1;
		FixedStringMapBase<uint64_t> FSMapUInt64;
		uint8_t _Pad6[4];
		uint64_t Unkn5;
		PrimitiveSet<void *> DataBufferSet;
		uint64_t Unkn6;
		void * DivinityStats;
		void * CritSection;
		uint64_t Unkn7[5];

		ModifierList * GetTypeInfo(CRPGStats_Object * object);
		RPGEnumeration * GetAttributeInfo(CRPGStats_Object * object, const char * attributeName, int & attributeIndex);
		std::optional<FixedString> GetAttributeFixedString(CRPGStats_Object * object, const char * attributeName);
		std::optional<int> GetAttributeInt(CRPGStats_Object * object, const char * attributeName);
		std::optional<int> GetAttributeIntScaled(CRPGStats_Object * object, const char * attributeName, int level);
		bool SetAttributeString(CRPGStats_Object * object, const char * attributeName, const char * value);
		bool SetAttributeInt(CRPGStats_Object * object, const char * attributeName, int32_t value);
		bool ObjectExists(FixedString statsId, FixedString type);

		std::optional<int> EnumLabelToIndex(const char * enumName, const char * enumLabel);
		int GetOrCreateFixedString(const char * value);
	};

	CRPGStats_Object * StatFindObject(char const * name);
	CRPGStats_Object * StatFindObject(int index);
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
				CDivinityStats_Character__GetTalent * GetTalent;
				CDivinityStats_Character__GetAbility * GetAbility;
			};
		};

#define DEFN_GETTER(type, name) \
	enum class name##Tag{}; \
	WrappableFunction<name##Tag, CDivinityStats_Character__Get##type> Wrapper##name;

#include <GameDefinitions/CharacterGetters.inl>
#undef DEFN_GETTER

		enum class HitChanceTag {}; \
		WrappableFunction<HitChanceTag, CDivinityStats_Character__GetHitChance> WrapperHitChance;

		bool Wrapped{ false };

		void WrapAll();
		void ResetExtension();

		std::optional<int32_t> GetStat(CDivinityStats_Character * character, char const * name, 
			bool original, bool baseValues);
	};

	extern CharacterStatsGetters gCharacterStatsGetters;
}
