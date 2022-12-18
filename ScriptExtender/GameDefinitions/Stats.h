#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/GameObjects/Ai.h>
#include <GameHooks/Wrappers.h>

BEGIN_SE()

class MsgS2CSyncStat;
class StatRequirement;
class StatProperty;
class StatPropertyList;

struct IScriptCheckObject
{
	virtual ~IScriptCheckObject() = 0;
	virtual ScriptCheckType GetType() const = 0;
	virtual bool Process() = 0;
	virtual void ForEachCheckDo(void* unknown) = 0;
	virtual bool ValidateCondition(int conditionId, int maxConditionId) = 0;

	void Dump(STDString& buf, ScriptOperatorType parentOperator) const;
	STDString Dump() const;
};

struct ScriptCheckOperator : public IScriptCheckObject
{
	ScriptOperatorType Type;
	IScriptCheckObject* Left;
	IScriptCheckObject* Right;
};

struct ScriptCheckVariable : public IScriptCheckObject
{
	int ConditionId;
	FixedString Value;
};

END_SE()

BEGIN_NS(eoc)

struct GameRandom
{
	int* FixedRollList;
	int CurFixedRollIndex;
	int Steps;
	int RerolledLCG_X;
	int BucketCount;
	int LCG_X;
	int MinRoll;
	int StepSize;
	BYTE field_24;
};

END_NS()

BEGIN_NS(stats)

using GetDamageTypeProc = DamageType (char const*);
using GetDamageTypeStringProc = char const* (DamageType);
using DamageTypeToTranslateStringProc = TranslatedString* (TranslatedString*, DamageType);
using DamageDescriptionToTranslateStringProc = TranslatedString* (TranslatedString*, DamageType);
using DamageTypeToTranslateStringExtendedProc = TranslatedString* (TranslatedString*, DamageType);
using GetColorCodeDmgProc = uint64_t (DamageType);
#if defined(OSI_EOCAPP)
using ColorCodeAndTypeDmgProc = void (eoc::Text* text, unsigned int amount, bool reflected, DamageType damageType);
#else
using ColorCodeAndTypeDmgProc = void (eoc::Text* text, DamageType& damageType, unsigned int amount, bool reflected);
#endif

struct TDamagePair
{
	int32_t Amount;
	DamageType DamageType;
};

struct DamagePairList : public ObjectSet<TDamagePair>
{
	void AddDamage(DamageType damageType, int32_t amount);
	void ClearDamage(DamageType damageType);

	int GetByType(DamageType damageType);
	void ClearAll(std::optional<DamageType> damageType);
	void Multiply(float multiplier);
	void ConvertDamageType(DamageType newType);
	void AggregateSameTypeDamages();
	void LuaMerge(lua_State* L);
	UserReturn LuaToTable(lua_State* L);
	void LuaCopyFrom(lua_State* L);
};

struct HitDamageInfo
{
	uint32_t Equipment{ 0 };
	// Sum of all damages in DamageList
	int32_t TotalDamageDone{ 0 };
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
	void ClearDamage(stats::DamageType damageType);
	void AddDamage(stats::DamageType damageType, int32_t amount);
	void CopyFrom(HitDamageInfo const& src);
};

struct Requirement
{
	RequirementType RequirementId;
	int32_t Param;
	FixedString Tag;
	bool Not;

	void ToProtobuf(StatRequirement* msg) const;
	void FromProtobuf(StatRequirement const& msg);
};

enum class AttributeType
{
	Int,
	FixedString,
	Enumeration,
	Flags,
	PropertyList,
	Conditions,
	Requirements,
	Unknown
};

struct ValueList : public Noncopyable<ValueList>
{
	FixedString Name;
	Map<FixedString, int32_t> Values;

	static bool IsFlagType(FixedString const& typeName);
	AttributeType GetPropertyType() const;
};

struct Modifier : public Noncopyable<Modifier>
{
	int32_t ValueListIndex{ -1 };
	int32_t LevelMapIndex{ -1 };
	int32_t UnknownZero{ 0 };
	FixedString Name;
};

struct ModifierInfo
{
	Modifier* Modifier;
	ValueList* ValueList;
	int Index;
};

struct Condition : public Noncopyable<Modifier>
{
	IScriptCheckObject* ScriptCheckBlock;
	FixedString Name;

	STDString Dump() const;
};

template <class T>
struct NamedElementManager : public Noncopyable<NamedElementManager<T>>
{
	void* VMT{ nullptr };
	ObjectSet<T *> Primitives;
	Map<FixedString, uint32_t> NameHashMap;
	uint32_t NextHandle{ 0 };
	uint32_t NumSomeItems{ 0 };

	void Add(FixedString const& name, T* elem)
	{
		NameHashMap.insert(name, Primitives.size());
		Primitives.push_back(elem);
		NextHandle++;
	}

	std::optional<int> FindIndex(char const * str) const
	{
		FixedString fs(str);
		if (fs) {
			return FindIndex(fs);
		} else {
			return {};
		}
	}

	std::optional<int> FindIndex(FixedString const& str) const
	{
		auto it = NameHashMap.find(str);
		if (it) {
			return (int)it.Value();
		} else {
			return {};
		}
	}

	T * Find(int index) const
	{
		if (index < 0 || index >= (int)Primitives.size()) {
			return nullptr;
		} else {
			return Primitives[index];
		}
	}

	T * Find(char const * str) const
	{
		FixedString fs(str);
		if (fs) {
			return Find(fs);
		} else {
			return nullptr;
		}
	}

	T * Find(FixedString const& str) const
	{
		auto ptr = NameHashMap.try_get_ptr(str);
		if (ptr != nullptr) {
			return Primitives[*ptr];
		} else {
			return nullptr;
		}
	}
};

struct ModifierList
{
	NamedElementManager<Modifier> Attributes;
	FixedString Name;

	std::optional<ModifierInfo> GetModifierInfo(FixedString const& name) const;
	bool IsItemType() const;
};

struct PropertyData
{
	virtual ~PropertyData() {}
	virtual PropertyData* Clone() { return nullptr; }
	virtual bool GetDescription(STDWString* Line1) { return false; }
	virtual bool GetDescription(TranslatedString * Line1, TranslatedString * Line2) { return false; }
	virtual bool GetDescription(TranslatedString * Line1) { return false; }
	virtual uint64_t Unknown() { return 0; }

	FixedString Name;
	PropertyType TypeId;
	PropertyContext Context;
	Condition* Conditions;

	void ToProtobuf(StatProperty* msg) const;
	void FromProtobuf(StatProperty const& msg);
};

struct PropertyCustom : public PropertyData
{};

struct PropertyStatus : public PropertyData
{
	FixedString Status;
	float StatusChance;
	float Duration;
	FixedString StatsId;
	int Arg4;
	int Arg5;
	bool SurfaceBoost;
	ObjectSet<SurfaceType> SurfaceBoosts;
};

struct PropertySurfaceChange : public PropertyData
{
	int SurfaceChange; // "Surface Change" from ValueLists
	float SurfaceChance;
	float Lifetime;
	float StatusChance;
	float Radius;
};
	
struct PropertySabotage : public PropertyData
{
	int Amount;
};
	
struct PropertySummon : public PropertyData
{
	FixedString Template;
	float Duration;
	bool IsTotem;
	FixedString Skill;
};

struct PropertyForce : public PropertyData
{
	int Distance;
	FixedString Unknown;
};

struct PropertyGameAction : public PropertyData
{
	int GameAction; // "Game Action" from ValueLists
	float Arg1;
	float Arg2;
	FixedString Arg3;
	float Arg4;
	float Arg5;
	int StatusHealType; // "StatusHealType" from ValueLists
};

struct PropertyOsirisTask : public PropertyData
{
	int OsirisTask; // "Osiris Task" from ValueLists
	float Chance;
	int VitalityOnRevive;
};

struct PropertyExtender : public PropertyData
{
	PropertyExtender()
	{}

	~PropertyExtender() override {}

	inline static void operator delete (void* ptr, size_t sz)
	{
		GameFree(ptr);
	}

	PropertyData* Clone() override
	{
		auto cl = GameAlloc<PropertyExtender>();
		cl->Name = Name;
		cl->TypeId = TypeId;
		cl->Context = Context;
		cl->Conditions = Conditions;
		cl->PropertyName = PropertyName;
		cl->Arg1 = Arg1;
		cl->Arg2 = Arg2;
		cl->Arg3 = Arg3;
		cl->Arg4 = Arg4;
		cl->Arg5 = Arg5;
		return cl;
	}

	bool GetDescription(STDWString* Line1) override;

	bool GetDescription(TranslatedString* Line1, TranslatedString* Line2) override
	{
		GetDescription(Line1);
		return true;
	}

	bool GetDescription(TranslatedString* Line1) override
	{
		GetDescription(&Line1->Handle.ReferenceString);
		return true;
	}

	uint64_t Unknown() override
	{
		return 0;
	}

	FixedString PropertyName;
	float Arg1;
	float Arg2;
	FixedString Arg3;
	int Arg4;
	int Arg5;
};

struct PropertyCustomDescription : public PropertyData
{
	PropertyCustomDescription()
	{}

	virtual ~PropertyCustomDescription() {}

	inline static void operator delete (void * ptr, size_t sz)
	{
		GameFree(ptr);
	}

	virtual PropertyData * Clone()
	{
		auto cl = GameAlloc<PropertyCustomDescription>();
		cl->Name = Name;
		cl->TypeId = TypeId;
		cl->Context = Context;
		cl->Conditions = Conditions;
		cl->TextLine1 = TextLine1;
		return cl;
	}

	virtual bool GetDescription(STDWString * Line1)
	{
		*Line1 = TextLine1;
		return true;

	}

	virtual bool GetDescription(TranslatedString * Line1, TranslatedString * Line2)
	{
		GetDescription(Line1);
		return true;
	}

	virtual bool GetDescription(TranslatedString * Line1)
	{
		GetDescription(&Line1->Handle.ReferenceString);
		return true;
	}

	virtual uint64_t Unknown()
	{
		return 0;
	}

	STDWString TextLine1;
};


struct PropertyList
{
	NamedElementManager<PropertyData> Properties;
	FixedString Name;
	PropertyContext AllPropertyContexts{ 0 };

	void ToProtobuf(FixedString const& name, StatPropertyList* msg) const;
	void FromProtobuf(StatPropertyList const& msg);
};

struct LevelMap : public Noncopyable<LevelMap>
{
	virtual ~LevelMap() {}
	virtual void SetModifierList(int modifierListIndex, int modifierIndex) = 0;
	virtual void SetModifierList(FixedString const& modifierListName, FixedString const& modifierName) = 0;
	virtual int64_t GetScaledValue(int difficultyScale, int level) = 0;

	int ModifierListIndex{ -1 };
	int ModifierIndex{ -1 };
	int ValueListIndex{ -1 };
	FixedString Name;
};

struct Object : public Noncopyable<Object>
{
	void* VMT{ nullptr };
	int32_t Handle{ -1 };
	int32_t Level{ -1 };
	int32_t ModifierListIndex{ -1 };
	Vector<int32_t> IndexedProperties;
	FixedString Name;
	TranslatedString DisplayName;
	FixedString FS2;
	struct CDivinityStats* DivStats{ nullptr };
	Map<FixedString, PropertyList *> PropertyLists;
	Map<FixedString, Condition *> Conditions;
	FixedString AIFlags;
	ObjectSet<Requirement, GameMemoryAllocator, true> Requirements;
	ObjectSet<Requirement, GameMemoryAllocator, true> MemorizationRequirements;
	ObjectSet<void *> CrimeReactionPriorities; // Set<CrimeReactionPriority>
	ObjectSet<FixedString, GameMemoryAllocator, true> StringProperties1;
	ObjectSet<FixedString, GameMemoryAllocator, true> ComboCategories;
#if !defined(OSI_EOCAPP)
	STDWString CurrentModName;
#endif
	int32_t Using{ -1 };
	uint32_t D{ 0 };

	ModifierList* GetModifierList() const;
	std::optional<ModifierInfo> GetModifierInfo(FixedString const& attributeName) const;

	std::optional<char const*> GetString(FixedString const& modifierName) const;
	std::optional<char const*> GetString(ModifierInfo const& modifier) const;
	std::optional<int> GetInt(FixedString const& modifierName) const;
	std::optional<int> GetInt(ModifierInfo const& modifier) const;
	std::optional<int> GetIntScaled(ModifierInfo const& modifier, int level) const;
	std::optional<ObjectSet<FixedString>> GetFlags(ModifierInfo const& modifier) const;
	std::optional<uint64_t> GetFlagsInt64(ModifierInfo const& modifier) const;
	std::optional<PropertyList*> GetPropertyList(ModifierInfo const& modifier) const;
	bool SetString(ModifierInfo const& modifier, const char* value);
	bool SetInt(ModifierInfo const& modifier, int32_t value);
	bool SetFlags(ModifierInfo const& modifier, ObjectSet<FixedString> const& value);
	bool SetFlags(ModifierInfo const& modifier, uint64_t value);
	bool SetPropertyList(ModifierInfo const& modifier, std::optional<PropertyList*> const& value);

	void ToProtobuf(MsgS2CSyncStat* msg) const;
	void FromProtobuf(MsgS2CSyncStat const& msg);
	void BroadcastSyncMessage(bool syncDuringLoading) const;

	int LuaGetAttributeLegacy(lua_State* L, FixedString const& attribute, std::optional<int> level);
	int LuaGetAttribute(lua_State* L, FixedString const& attribute, std::optional<int> level);
	bool LuaSetAttribute(lua_State* L, FixedString const& attribute, int index);

	static PropertyOperationResult LuaFallbackGet(lua_State* L, lua::LifetimeHandle const& lifetime, Object* object, FixedString const& prop);
	static PropertyOperationResult LuaFallbackSet(lua_State* L, lua::LifetimeHandle const& lifetime, Object* object, FixedString const& prop, int index);

private:
	void LuaGetAttributeShared(lua_State* L, FixedString const& attribute, std::optional<int> level, bool byRef);
};

struct ObjectInstance : public Object
{
	uint32_t InstanceId;
};

struct TalentArray : public BitArray<4>
{
	inline bool Toggle(TalentType talent, bool enabled)
	{
		if (enabled) {
			return Set((uint32_t)talent);
		} else {
			return Clear((uint32_t)talent);
		}
	}

	inline bool HasTalent(TalentType talent) const
	{
		return IsSet((uint32_t)talent);
	}
};

struct Reflection
{
	DamageType DamageType{ DamageType::Sentinel };
	bool MeleeOnly{ false };
};

struct ReflectionSet
{
	Map<Reflection, Reflection> DamageTypes;
};

struct EquipmentAttributes : public ProtectedGameObject<EquipmentAttributes>
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
	int32_t MemoryBoost;
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
	int32_t ShadowResistance;
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
	ReflectionSet* Reflection;
	FixedString Skills;
	FixedString ItemColor;
	uint32_t ModifierType;
	uint32_t Unkn3;
	FixedString ObjectInstanceName;
	FixedString BoostName;
	EquipmentStatsType StatsType;
	TalentArray Talents;
	uint32_t Unkn4;
	int64_t AttributeFlagsObjectId;

	struct LegacyPropertyMapBase & GetPropertyMap() const;
};

struct EquipmentAttributesWeapon : public EquipmentAttributes
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
	FixedString Projectile;
};

struct EquipmentAttributesShield : public EquipmentAttributes
{
	int32_t ArmorValue;
	int32_t ArmorBoost;
	int32_t MagicArmorValue;
	int32_t MagicArmorBoost;
	int32_t Blocking;
};

struct EquipmentAttributesArmor : public EquipmentAttributes
{
	int32_t ArmorValue;
	int32_t ArmorBoost;
	int32_t MagicArmorValue;
	int32_t MagicArmorBoost;
};

struct Item : public ObjectInstance
{
	uint32_t U1;
	EquipmentStatsType ItemType;
	ItemSlot32 ItemSlot;
	WeaponType WeaponType;
	int32_t AnimType; // -1 = Not overridden
	uint32_t WeaponRange;
	uint32_t IsIdentified; // Saved
	uint32_t U2;
	uint32_t U3;
	uint32_t U4;
	bool IsTwoHanded;
	bool ShouldSyncStats;
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
	Vector<EquipmentAttributes *> DynamicStats;
	IGameObject* GameObject;
	StatAttributeFlags AttributeFlags;
	int32_t MaxCharges; // Saved; -1 = Not overridden
	int32_t Charges; // Saved; -1 = Not overridden
	uint8_t LevelGroupIndex;
	uint8_t RootGroupIndex;
	uint8_t NameGroupIndex;
	uint8_t NameIndex;
	uint32_t NameCool;
	ObjectSet<void *> Tags_Maybe;
	ObjectSet<FixedString> DeltaMods;

	int32_t GetPhysicalResistance();
	int32_t GetPiercingResistance();
	int32_t GetMagicResistance();
	int32_t GetCorrosiveResistance();

	bool HasTalent(TalentType talent);
	int32_t GetAbility(AbilityType ability);
};

struct CharacterEquippedItem
{
	int32_t ItemStatsHandle;
	ItemSlot ItemSlot;
	bool IsEquipped;
};

struct CharacterDynamicStat : public ProtectedGameObject<CharacterDynamicStat>
{
	int32_t CharacterInstanceId;
	CDivinityStats * DivStats;

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
	TalentArray Talents;
	TalentArray RemovedTalents;
	int16_t Traits[18];
	uint32_t BoostConditionsMask;
	FixedString TranslationKey;
	FixedString BonusWeapon;
	uint64_t Reflection;
	uint32_t AttributeHandle;
	uint32_t StepsType;
	int64_t AttributeFlagsObjectId;
	uint32_t Unkn4;
};

struct Character : public ObjectInstance
{
	using HitInternalProc = void (Character * self, Character *attackerStats,
		Item *item, DamagePairList *damageList, HitType hitType, bool noHitRoll,
		bool forceReduceDurability, HitDamageInfo *damageInfo, PropertyList *skillProperties,
		HighGroundBonus highGroundFlag, CriticalRoll criticalRoll);

	int32_t CurrentVitality;
	int32_t CurrentArmor;
	int32_t CurrentMagicArmor;
	int32_t ArmorAfterHitCooldownMultiplier;
	int32_t MagicArmorAfterHitCooldownMultiplier;
	int32_t MPStart;
	int32_t CurrentAP;
	int32_t BonusActionPoints; // Saved
	int32_t Experience; // Saved
	int32_t Reputation;
	uint8_t Flanked; // Saved
	int32_t Karma; // Saved
	CharacterFlags Flags;
	uint32_t Unkn0;
	ObjectSet<int> TraitOrder; // Saved
	int32_t MaxResistance;
	int32_t HasTwoHandedWeapon;
	IGameObject * GameObject;
	int32_t Unkn2;
	int32_t IsIncapacitatedRefCount;
	Vector<CharacterDynamicStat*> DynamicStats;
	CharacterDynamicStat * StatsFromStatsEntry;
	Vector<CharacterEquippedItem*> EquippedItems;
	ObjectSet<SurfacePathInfluence> SurfacePathInfluences;
	int32_t ActiveBoostConditions[16]; // Saved
	eoc::GameRandom DamageRng;
	eoc::GameRandom CriticalHitRng;
	int32_t MaxVitality;
	int32_t BaseMaxVitality;
	int32_t MaxArmor;
	int32_t BaseMaxArmor;
	int32_t MaxMagicArmor;
	int32_t BaseMaxMagicArmor;
	float Sight;
	float BaseSight;
	StatAttributeFlags AttributeFlags;
	StatAttributeFlags BaseAttributeFlags;
	StatAttributeFlags ItemBoostedAttributeFlags;
	bool AttributeFlagsUpdated;
	int32_t MaxSummons;
	int32_t BaseMaxSummons;
	int32_t MaxMpOverride; // Saved
	TalentArray DisabledTalents; // Saved

	std::optional<int32_t> GetStat(FixedString const& name, bool excludeBoosts);
	std::optional<int32_t> GetHitChance(Character * target);
	bool HasTalent(TalentType talent, bool excludeBoosts);
	int32_t GetAbility(AbilityType ability, bool excludeBoosts);
	Item * GetItemBySlot(ItemSlot slot, std::optional<bool> mustBeEquipped);
	Item * GetMainWeapon();
	Item * GetOffHandWeapon();
	int32_t GetPhysicalResistance(bool excludeBoosts);
	int32_t GetPiercingResistance(bool excludeBoosts);
	int32_t GetMagicResistance(bool excludeBoosts);
	int32_t GetCorrosiveResistance(bool excludeBoosts);
	int32_t GetDamageBoost();
	bool IsBoostActive(uint32_t conditionsMask);
};

typedef int32_t (CDivinityStats_Character__GetStat)(Character * self, bool baseStats);
typedef int32_t (CDivinityStats_Character__GetStatNoBoosts)(Character * self);
typedef int32_t (CDivinityStats_Character__GetStatWithBoosts)(Character * self, bool baseStats, bool excludeBoosts);
typedef int32_t (CDivinityStats_Character__GetStatWithInit)(Character * self, bool baseStats, int32_t initialValue);
typedef float (CDivinityStats_Character__GetStatHearing)(Character * self, Character * other, bool baseStats);
typedef int32_t (CDivinityStats_Character__GetHitChance)(Character * attacker, Character * target);
typedef bool (CDivinityStats_Character__GetTalent)(Character * self, TalentType talentId, char excludeBoosts);
typedef int32_t (CDivinityStats_Character__GetAbility)(Character * self, AbilityType abilityId, char excludeBoosts, char noLoneWolfBonus);
	
typedef void (* eoc__SkillPrototypeManager__Init)(void * self);

template <class T>
struct CHandleArray : public ProtectedGameObject<CHandleArray<T>>
{
	void* VMT;
	Vector<T*> Handles;
	__int64 field_20;
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
	CDivinityStatsList<Item> * ItemList;
	CDivinityStatsList<Character> * CharacterList;
	char SomeBuf0x50b[80];
	Item * DefaultWeaponStats;
	ObjectSet<FixedString> FSSetUnkn;
	uint64_t Unkn;
};



struct ExtraData : public ProtectedGameObject<ExtraData>
{
	Map<FixedString, float> Properties;
};

struct ConditionsManager
{
	Map<FixedString, Condition *> Conditions;
	ObjectSet<STDString> Variables;
};

struct ItemTypeManager : public NamedElementManager<uint64_t>
{
	uint64_t Unknown;
};

struct SkillPrototype
{
	using FormatDescriptionParamProc = void (SkillPrototype *skillPrototype, Character *tgtCharStats,
		eoc::Text *eocText, int paramIndex, bool isFromItem, float xmm9_4_0, FixedString * paramText,
		ObjectSet<STDString> * stdStringSet);

	using GetSkillDamageProc = void (SkillPrototype * self, struct DamagePairList * damageList,
		ObjectInstance *attackerStats, bool isFromItem, bool stealthed, glm::vec3 const& attackerPosition,
		glm::vec3 const& targetPosition, DeathType * pDeathType, int level, bool noRandomization);
#if defined(OSI_EOCAPP)
	using GetAttackAPCostProc = int(SkillPrototype* self, Character* character, eoc::AiGrid* aiGrid, glm::vec3* position, float* radius, int* pEelementalAffinity);
#else
	using GetAttackAPCostProc = int(SkillPrototype* self, Character* character, eoc::AiGrid* aiGrid, glm::vec3* position, float* radius, bool unused, int* pEelementalAffinity);
#endif

	int StatsObjectIndex;
	SkillType SkillTypeId;
	FixedString SkillId;
	int16_t Ability;
	int32_t Tier;
	int32_t Requirement;
	int32_t Level;
	int32_t MagicCost;
	int32_t MemoryCost;
	int32_t ActionPoints;
	float Cooldown;
	float CooldownReduction;
	float ChargeDuration;
	STDWString DisplayName;
	FixedString Icon;
	AIFlags AiFlags;
	SkillPrototype * RootSkillPrototype;
	ObjectSet<SkillPrototype *> ChildPrototypes;

	Object * GetStats() const;

	static PropertyOperationResult LuaFallbackGet(lua_State* L, lua::LifetimeHandle const& lifetime, SkillPrototype* object, FixedString const& prop);
	static PropertyOperationResult LuaFallbackSet(lua_State* L, lua::LifetimeHandle const& lifetime, SkillPrototype* object, FixedString const& prop, int index);
};

struct SkillPrototypeManager
{
	void* VMT;
	Map<FixedString, SkillPrototype*> Prototypes;
	ObjectSet<FixedString> PrototypeNames;
	bool Initialized;

	void SyncSkillStat(Object* object);
	void SyncSkillStat(Object* object, SkillPrototype* proto);
};

struct StatusPrototype
{
	using FormatDescriptionParamProc = void (StatusPrototype *prototype, ObjectInstance* owner,
		ObjectInstance* statusSource, float multiplier, eoc::Text * text, int paramIndex,
		FixedString * param, ObjectSet<STDString> * paramSet);

	void* VMT{ nullptr };
	int StatsObjectIndex{ -1 };
	StatusType StatusId;
	FixedString StatusName;
	TranslatedString DisplayName;
	FixedString Icon;
	bool HasStats{ false };
	ObjectSet<SurfaceType>* AbsorbSurfaceTypes{ nullptr };

	Object * GetStats() const;

	static PropertyOperationResult LuaFallbackGet(lua_State* L, lua::LifetimeHandle const& lifetime, StatusPrototype* object, FixedString const& prop);
	static PropertyOperationResult LuaFallbackSet(lua_State* L, lua::LifetimeHandle const& lifetime, StatusPrototype* object, FixedString const& prop, int index);
};

struct StatusPrototypeManager
{
	void* VMT;
	Map<FixedString, StatusPrototype*> Prototypes;
	ObjectSet<FixedString> PrototypeNames;
	bool Initialized;

	void SyncStatusStat(Object* object);
};


struct SkillSet
{
	FixedString Name;
	ObjectSet<FixedString> Skills;
};

struct SkillSetManager : public NamedElementManager<SkillSet>
{
	SkillSet * ParsedSkillSet;
};

struct EquipmentGroup
{
	FixedString Name;
	ObjectSet<FixedString> Equipment;
};

struct EquipmentSet
{
	FixedString Name;
	Vector<EquipmentGroup*> Groups;
};

struct EquipmentSetManager : public NamedElementManager<EquipmentSet>
{
	EquipmentSet * ParsedEquipmentSet;
};

struct DeltaModifier : Noncopyable<DeltaModifier>
{
	ObjectSet<int> BoostIndices;
	ObjectSet<int> BoostCounts;
	FixedString ModifierType;
	ItemSlot32 SlotType{ ItemSlot32::Sentinel };
	WeaponType WeaponType{ WeaponType::Sentinel };
	ArmorType ArmorType{ ArmorType::Sentinel };
	HandednessType Handedness{ HandednessType::Any };
	FixedString Name;
	FixedString BoostType;
	int MinLevel{ -1 };
	int MaxLevel{ -1 };
	int MinLevelBoosted{ -1 };
	int MaxLevelBoosted{ -1 };
	int Frequency{ 1 };
	int Frequency2{ 1 };
};

struct DeltaModifierList : public NamedElementManager<DeltaModifier>
{};

struct CRPGStatsVMTMappings
{
	bool VMTsMapped{ false };
	void* SkillPropertiesVMT{ nullptr };
	void* ObjectVMT{ nullptr };
	std::unordered_map<PropertyType, void*> PropertyTypes;

	CRPGStatsVMTMappings();
	void MapVMTs();
};

struct TreasureSubTableDescription
{
	struct Category
	{
		int Index;
		int Frequency;
		std::array<uint16_t, 7> Frequencies;
		std::array<uint16_t, 7> CurrentFrequencies;
		bool IsTreasureTable;
		bool IsTreasureTable2;
	};

	struct DropCount
	{
		int Amount;
		int Chance;
	};

	ObjectSet<Category*> Categories;
	ObjectSet<int> CategoryFrequencies;
	int TotalFrequency;
	ObjectSet<DropCount> DropCounts;
	ObjectSet<int> Amounts;
	int TotalCount;
	int StartLevel;
	int EndLevel;
};


struct TreasureTable
{
	FixedString Name;
	int MinLevel;
	int MaxLevel;
	bool IgnoreLevelDiff;
	bool UseTreasureGroupContainers;
	bool CanMerge;
	ObjectSet<TreasureSubTableDescription*> SubTables;
};

struct TreasureCategory
{
	struct Item
	{
		FixedString Name;
		int Priority;
		int MinAmount;
		int MaxAmount;
		int ActPart;
		int Unique;
		int MinLevel;
		int MaxLevel;
	};


	FixedString Category;
	Vector<Item*> Items;
	uint64_t Unknown[3];
};

struct TreasureObjectInfo
{
	FixedString ItemType;
	FixedString StatsId;
	__int64 field_10;
	int field_18;
	int Amount;
};

struct NameGroupName
{
	int field_0;
	TranslatedString Name;
	TranslatedString Name2;
};


struct NameGroup
{
	FixedString Name;
	ObjectSet<NameGroupName*> Names;
	ObjectSet<NameGroupName*> NamesCool;
};


struct CNameGroupLink
{
	int field_0;
	FixedString NameGroup;
	int NoneCoolSuffix;
	FixedString ItemName;
};


struct CRootGroup
{
	int MinLevel;
	int MaxLevel;
	FixedString RootGroup;
	FixedString Color;
	ObjectSet<CNameGroupLink*> NameGroupLinks;
};


struct CLevelGroup
{
	int MinLevel;
	int MaxLevel;
	FixedString Name;
	ObjectSet<CRootGroup*> RootGroups;
};


struct CItemGroup
{
	FixedString Name;
	ObjectSet<CLevelGroup*> LevelGroups;
};


struct CItemProgressionManager : public ProtectedGameObject<CItemProgressionManager>
{
	Map<FixedString, CItemGroup*> ItemGroups;
	Map<FixedString, NameGroup*> NameGroups;
	CItemGroup* CurrentItemGroup;
	CLevelGroup* CurrentLevelGroup;
	CRootGroup* CurrentRootGroup;
	NameGroup* CurrentNameGroup;
};


struct ItemCombinationProperty
{
	struct Entry
	{
		FixedString ObjectId;
		IngredientType IngredientType;
		FixedString Result;
	};

	FixedString Name;
	FixedString PreviewIcon;
	FixedString PreviewTooltip;
	ObjectSet<Entry, GameMemoryAllocator, true> Entries;
};


struct ItemCombinationPreviewData
{
	FixedString Name;
	FixedString Type;
	FixedString StatsId;
	FixedString Tooltip;
	FixedString Icon;
};


struct ItemCombinationIngredient
{
	FixedString Object;
	IngredientType IngredientType;
	IngredientTransformType Transform;
	ItemDataRarity ItemRarity;
};


struct ItemCombinationResult
{
	struct CombinationResult
	{
		FixedString Result;
		FixedString Boost;
		int ResultAmount;
	};


	ObjectSet<CombinationResult, GameMemoryAllocator, true> Results;
	AbilityType Requirement;
	int ReqLevel;
	FixedString PreviewStatsId;
	FixedString PreviewIcon;
	FixedString PreviewTooltip;
	FixedString Name;
};


struct ItemCombination
{
	FixedString Name;
	ObjectSet<ItemCombinationIngredient, GameMemoryAllocator, true> Ingredients;
	ObjectSet<ItemCombinationResult, GameMemoryAllocator, true> Results;
	RecipeCategory RecipeCategory;
	CraftingStationType CraftingStation;
	bool AutoLevel;
};


struct ItemCombinationManager : public NamedElementManager<ItemCombination>
{
	ItemCombination* CurrentItemCombo;
	ItemCombinationPreviewData* CurrentPreviewData;
	ItemCombinationProperty* CurrentComboProperties;
	RefMap<FixedString, ItemCombinationPreviewData*> PreviewData;
	RefMap<FixedString, ItemCombinationProperty*> ComboProperties;
};

struct ItemSet
{
	FixedString Name;
	TranslatedString TranslatedName;
	TranslatedString TranslatedSetEffect;
	ObjectSet<FixedString> Items;
	ObjectSet<FixedString> GrantedSkills;
	ObjectSet<FixedString> GrantedStatuses;
	int32_t ItemsUpgradeLevel;
};

struct ItemSetManager : public NamedElementManager<ItemSet>
{
	void* Unknown[2];
	Map<FixedString, ItemSet*> ItemSets;
	void* Unknown2;
	ItemSet* ParsedItemSet;
};


extern CRPGStatsVMTMappings gCRPGStatsVMTMappings;

struct ItemColorDefinition
{
	uint32_t Color1;
	uint32_t Color2;
	uint32_t Color3;
};

struct RPGStats : public ProtectedGameObject<RPGStats>
{
	typedef void (LoadProc)(RPGStats* self, ObjectSet<STDString>* paths);
	typedef PropertyData* (ParsePropertiesProc)(RPGStats* self, STDString* str);
	typedef void (ParseStructureFolderProc)(RPGStats* self, ObjectSet<STDString>* paths);

	NamedElementManager<ValueList> ModifierValueLists;
	NamedElementManager<ModifierList> ModifierLists;
	NamedElementManager<Object> Objects;
	NamedElementManager<LevelMap> LevelMaps;
	NamedElementManager<DeltaModifierList> DeltaMods;
	NamedElementManager<TreasureCategory> TreasureCategories;
	NamedElementManager<TreasureTable> TreasureTables;
	ItemTypeManager ItemTypes;
	Map<FixedString, PropertyList*> PropertyLists;
	ConditionsManager ConditionsManager;
	STDWString LoadingModName;
	uint64_t Unkn1[5];
	ExtraData * ExtraData;
	RefMap<FixedString, void *> RefMap1;
	RefMap<FixedString, ItemColorDefinition> Colors;
	Map<FixedString, FixedString> TreasureCategoryMaps;
	Map<FixedString, int> TreasureWeaponCounters;
	Map<FixedString, int> TreasureArmorCounters;
	Map<FixedString, int> TreasureSkillbookCounters;
	RefMap<FixedString, void *> FSMap2;
	FixedString TreasureItemTypes[7];
	ObjectSet<FixedString, GameMemoryAllocator, true> FixedStrings;
	ObjectSet<uint64_t*> Flags;
	uint64_t unknown2[140];
	EquipmentSetManager * EquipmentSetManager;
	SkillSetManager * SkillSetManager;
	CItemProgressionManager * ItemProgressionManager;
	ItemCombinationManager * ItemCombinationManager;
	ItemSetManager* ItemSetsManager;
	ScratchBuffer* CurrentPreParseBuf;
	FixedString CurrentStatsEntryName;
	Map<FixedString, uint64_t> PreParsedDataBufferMap;
	ObjectSet<void *> PreParsedDataBuffers;
	CDivinityStats* DivinityStats;
	CRITICAL_SECTION CritSection;
	uint16_t Unkn7[5];

	Modifier * GetModifierInfo(FixedString const& modifierListName, FixedString const& modifierName);
	ModifierList * GetTypeInfo(Object * object);

	bool ObjectExists(FixedString const& statsId, FixedString const& type);
	std::optional<Object*> CreateObject(FixedString const& name, FixedString const& type);
	std::optional<Object*> CreateObject(FixedString const& name, int32_t modifierListIndex);
	void SyncObjectFromServer(MsgS2CSyncStat const& msg);
	void SyncWithPrototypeManager(Object* object);
	void BroadcastSyncAll();

	std::optional<FixedString*> GetFixedString(int stringId);
	FixedString* GetOrCreateFixedString(int& stringId);
	int GetOrCreateFixedString(const char* value);
	std::optional<uint64_t*> GetFlags(int flagsId);
	uint64_t* GetOrCreateFlags(int& flagsId);
	Condition* CreateConditions(FixedString const& statName, FixedString const& modifierName, STDString const& conditions);

	PropertyList* ConstructPropertyList(FixedString const& propertyName);
	PropertyData* ConstructProperty(PropertyType type);

	std::optional<int> EnumLabelToIndex(FixedString const& enumName, char const* enumLabel);
	FixedString EnumIndexToLabel(FixedString const& enumName, int index);
	std::optional<StatAttributeFlags> StringToAttributeFlags(const char * value);
	IScriptCheckObject* BuildScriptCheckBlock(STDString const& source);
	IScriptCheckObject* BuildScriptCheckBlockFromProperties(STDString const& source);
};

typedef IScriptCheckObject* (*ScriptCheckBlock__Build)(STDString const& str, ObjectSet<STDString> const& variables, int offset, int length);

Object * StatFindObject(char const * name, bool warnOnError = false);
Object * StatFindObject(int index);

template <class TTag>
std::optional<int32_t> CharacterStatGetter(CDivinityStats_Character__GetStat * getter,
	WrappableFunction<TTag, CDivinityStats_Character__GetStat> & wrapper,
	Character * character, bool original, bool baseStats)
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
std::optional<int32_t> CharacterStatGetter(CDivinityStats_Character__GetStatNoBoosts * getter,
	WrappableFunction<TTag, CDivinityStats_Character__GetStatNoBoosts> & wrapper,
	Character * character, bool original, bool baseStats)
{
	if (getter == nullptr) {
		return {};
	}

	if (original || !wrapper.IsWrapped()) {
		return getter(character);
	} else {
		return wrapper.CallWithHooks(character);
	}
}

template <class TTag>
std::optional<int32_t> CharacterStatGetter(CDivinityStats_Character__GetStatWithBoosts * getter,
	WrappableFunction<TTag, CDivinityStats_Character__GetStatWithBoosts> & wrapper,
	Character * character, bool original, bool baseStats)
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
	Character * character, bool original, bool baseStats)
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
	Character * character, bool original, bool baseStats)
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
	void UnwrapAll();

	std::optional<int32_t> GetStat(Character * character, FixedString const& name, 
		bool original, bool baseValues);
	std::optional<int32_t> GetStat(Character * character, CharacterStatGetterType type, 
		bool original, bool baseValues);
};

END_NS()

BEGIN_NS(esv)

using ExecutePropertyDataOnGroundHitProc = void(glm::vec3* position, uint64_t casterHandle, stats::DamagePairList* damageList, stats::PropertyList* propertyList, stats::DamageType damageType);
using ExecuteCharacterSetExtraPropertiesProc = void(stats::PropertyList* extraProperties, uint64_t attackerHandle,
	ObjectSet<Character*> const& targets, glm::vec3 const& impactOrigin, stats::PropertyContext propertyContext,
	bool isFromItem, stats::SkillPrototype* skillProto, stats::HitDamageInfo* damageInfo, float statusStartTimer, Character* refTarget,
	bool statusFlag0x40, float a12);
using ExecutePropertyDataOnPositionOnlyProc = void(stats::PropertyList* properties, uint64_t attackerHandle,
	glm::vec3 const* position, float areaRadius, stats::PropertyContext propertyContext, bool isFromItem,
	stats::SkillPrototype* skillPrototype, stats::HitDamageInfo* damageInfo, float unkn);

END_NS()

BEGIN_SE()

using namespace dse::stats;

MARK_ALLOCATABLE(PropertyList);
MARK_ALLOCATABLE(SkillSet);
MARK_ALLOCATABLE(EquipmentGroup);
MARK_ALLOCATABLE(EquipmentSet);
MARK_ALLOCATABLE(DeltaModifier);
MARK_ALLOCATABLE(TreasureSubTableDescription);
MARK_ALLOCATABLE(TreasureSubTableDescription::Category);
MARK_ALLOCATABLE(TreasureTable);
MARK_ALLOCATABLE(TreasureCategory);
MARK_ALLOCATABLE(TreasureCategory::Item);
MARK_ALLOCATABLE(NameGroupName);
MARK_ALLOCATABLE(NameGroup);
MARK_ALLOCATABLE(CNameGroupLink);
MARK_ALLOCATABLE(CRootGroup);
MARK_ALLOCATABLE(CLevelGroup);
MARK_ALLOCATABLE(CItemGroup);
MARK_ALLOCATABLE(ItemCombinationProperty);
MARK_ALLOCATABLE(ItemCombinationPreviewData);
MARK_ALLOCATABLE(ItemCombinationIngredient);
MARK_ALLOCATABLE(ItemCombinationResult);
MARK_ALLOCATABLE(ItemCombinationResult::CombinationResult);
MARK_ALLOCATABLE(ItemCombination);

END_SE()