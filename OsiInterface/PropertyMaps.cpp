#include <stdafx.h>
#include <OsirisProxy.h>
#include "PropertyMaps.h"

namespace osidbg
{
	PropertyMap<esv::Status, void> gStatusPropertyMap;
	PropertyMap<esv::StatusConsume, esv::Status> gStatusConsumePropertyMap;
	PropertyMap<esv::StatusHit, esv::Status> gStatusHitPropertyMap;
	PropertyMap<esv::StatusHeal, esv::Status> gStatusHealPropertyMap;
	PropertyMap<esv::StatusHealing, esv::StatusConsume> gStatusHealingPropertyMap;
	PropertyMap<HitDamageInfo, void> gHitDamageInfoPropertyMap;
	PropertyMap<eoc::ItemDefinition, void> gEoCItemDefinitionPropertyMap;
	PropertyMap<CDivinityStats_Equipment_Attributes, void> gEquipmentAttributesPropertyMap;
	PropertyMap<CDivinityStats_Equipment_Attributes_Weapon, CDivinityStats_Equipment_Attributes> gEquipmentAttributesWeaponPropertyMap;
	PropertyMap<CDivinityStats_Equipment_Attributes_Armor, CDivinityStats_Equipment_Attributes> gEquipmentAttributesArmorPropertyMap;
	PropertyMap<CDivinityStats_Equipment_Attributes_Shield, CDivinityStats_Equipment_Attributes> gEquipmentAttributesShieldPropertyMap;
	PropertyMap<CharacterDynamicStat, void> gCharacterDynamicStatPropertyMap;
	PropertyMap<CDivinityStats_Character, void> gCharacterStatsPropertyMap;
	PropertyMap<esv::PlayerCustomData, void> gPlayerCustomDataPropertyMap;

#define PROP(cls, name) AddProperty<decltype(cls::name)>(propertyMap, #name, offsetof(cls, name))
#define PROP_RO(cls, name) AddPropertyRO<decltype(cls::name)>(propertyMap, #name, offsetof(cls, name))
#define PROP_ENUM(cls, name) AddPropertyEnum<decltype(cls::name)>(propertyMap, #name, offsetof(cls, name))
#define PROP_FLAGS(cls, name, enum, writeable) AddPropertyFlags<decltype(cls::name), enum>(propertyMap, #name, offsetof(cls, name), writeable)

	void InitPropertyMaps()
	{
		{
			auto & propertyMap = gStatusPropertyMap;
			PROP_RO(esv::Status, StatusId);
			PROP_RO(esv::Status, CanEnterChance);
			PROP_RO(esv::Status, StartTimer);
			PROP_RO(esv::Status, LifeTime);
			PROP_RO(esv::Status, CurrentLifeTime);
			PROP_RO(esv::Status, TurnTimer);
			PROP_RO(esv::Status, Strength);
			PROP_RO(esv::Status, StatsMultiplier);
			PROP_ENUM(esv::Status, DamageSourceType);
			PROP_RO(esv::Status, StatusHandle);
			PROP_RO(esv::Status, TargetCIHandle);
			PROP_RO(esv::Status, StatusSourceHandle);
			PROP_RO(esv::Status, SomeHandle);
			PROP_FLAGS(esv::Status, Flags0, StatusFlags0, false);
			PROP_FLAGS(esv::Status, Flags1, StatusFlags1, false);
			PROP_FLAGS(esv::Status, Flags2, StatusFlags2, false);

			propertyMap.Flags["ForceStatus"].Flags |= kPropWrite;
			propertyMap.Flags["ForceFailStatus"].Flags |= kPropWrite;

			propertyMap.Properties["LifeTime"].SetFloat = [](void * st, float value) -> bool {
				auto status = reinterpret_cast<esv::Status *>(st);
				if (value < 0.0f) return false;
				status->LifeTime = value;
				if (status->CurrentLifeTime > status->LifeTime) {
					status->CurrentLifeTime = status->LifeTime;
				}

				return true;
			};

			propertyMap.Properties["CurrentLifeTime"].SetFloat = [](void * st, float value) -> bool {
				auto status = reinterpret_cast<esv::Status *>(st);
				if (value < 0.0f) return false;
				status->CurrentLifeTime = value;
				if (status->LifeTime < status->CurrentLifeTime) {
					status->LifeTime = status->CurrentLifeTime;
				}

				return true;
			};
		}

		{
			auto & propertyMap = gStatusHitPropertyMap;
			propertyMap.Parent = &gStatusPropertyMap;
			PROP_RO(esv::StatusHit, HitByHandle);
			PROP_RO(esv::StatusHit, HitWithHandle);
			PROP_RO(esv::StatusHit, WeaponHandle);
			PROP(esv::StatusHit, HitReason);
			PROP(esv::StatusHit, SkillId);
			PROP(esv::StatusHit, Interruption);
			PROP(esv::StatusHit, AllowInterruptAction);
			PROP(esv::StatusHit, ForceInterrupt);
			PROP_RO(esv::StatusHit, DecDelayDeathCount);
			PROP(esv::StatusHit, ImpactPosition);
			PROP(esv::StatusHit, ImpactOrigin);
			PROP(esv::StatusHit, ImpactDirection);
		}

		{
			auto & propertyMap = gStatusConsumePropertyMap;
			propertyMap.Parent = &gStatusPropertyMap;
			// TODO - Skills, Items, ResetCooldownsSet, StatsIDs?
			PROP(esv::StatusConsume, ResetAllCooldowns);
			PROP(esv::StatusConsume, ResetOncePerCombat);
			PROP(esv::StatusConsume, ScaleWithVitality);
			PROP(esv::StatusConsume, LoseControl);
			PROP(esv::StatusConsume, ApplyStatusOnTick);
			PROP(esv::StatusConsume, EffectTime);
			PROP(esv::StatusConsume, StatsId);
			PROP(esv::StatusConsume, StackId);
			PROP(esv::StatusConsume, OriginalWeaponStatsId);
			PROP(esv::StatusConsume, OverrideWeaponStatsId);
			PROP(esv::StatusConsume, OverrideWeaponHandle);
			PROP(esv::StatusConsume, SavingThrow);
			PROP(esv::StatusConsume, SourceDirection);
			PROP(esv::StatusConsume, Turn);
			PROP(esv::StatusConsume, HealEffectOverride);
		}

		{
			auto & propertyMap = gStatusHealingPropertyMap;
			propertyMap.Parent = &gStatusConsumePropertyMap;
			PROP(esv::StatusHealing, HealAmount);
			PROP(esv::StatusHealing, TimeElapsed);
			PROP(esv::StatusHealing, HealEffect);
			PROP(esv::StatusHealing, HealEffectId);
			PROP(esv::StatusHealing, SkipInitialEffect);
			PROP(esv::StatusHealing, HealingEvent);
			PROP_ENUM(esv::StatusHealing, HealStat);
			PROP(esv::StatusHealing, AbsorbSurfaceRange);
		}

		{
			auto & propertyMap = gStatusHealPropertyMap;
			propertyMap.Parent = &gStatusPropertyMap;
			PROP(esv::StatusHeal, EffectTime);
			PROP(esv::StatusHeal, HealAmount);
			PROP(esv::StatusHeal, HealEffect);
			PROP(esv::StatusHeal, HealEffectId);
			PROP_ENUM(esv::StatusHeal, HealType);
			PROP(esv::StatusHeal, AbsorbSurfaceRange);
			PROP(esv::StatusHeal, TargetDependentHeal);
		}

		{
			auto & propertyMap = gHitDamageInfoPropertyMap;
			PROP(HitDamageInfo, Equipment);
			PROP(HitDamageInfo, TotalDamageDone);
			// PROP(HitDamageInfo, Unknown);
			PROP_ENUM(HitDamageInfo, DeathType);
			PROP_ENUM(HitDamageInfo, DamageType);
			PROP(HitDamageInfo, AttackDirection);
			PROP(HitDamageInfo, ArmorAbsorption);
			PROP(HitDamageInfo, LifeSteal);
			PROP_FLAGS(HitDamageInfo, EffectFlags, HitFlag, true);
			PROP(HitDamageInfo, HitWithWeapon);
		}

		{
			auto & propertyMap = gEoCItemDefinitionPropertyMap;
			PROP(eoc::ItemDefinition, RootTemplate);
			PROP(eoc::ItemDefinition, OriginalRootTemplate);
			PROP(eoc::ItemDefinition, Amount);
			PROP(eoc::ItemDefinition, Slot);
			PROP(eoc::ItemDefinition, GoldValueOverwrite);
			PROP(eoc::ItemDefinition, WeightValueOverwrite);
			PROP(eoc::ItemDefinition, DamageTypeOverwrite);
			PROP(eoc::ItemDefinition, ItemType);
			PROP(eoc::ItemDefinition, GenerationStatsId);
			PROP(eoc::ItemDefinition, GenerationItemType);
			PROP(eoc::ItemDefinition, GenerationRandom);
			PROP(eoc::ItemDefinition, GenerationLevel);
			PROP(eoc::ItemDefinition, StatsName);
			PROP(eoc::ItemDefinition, Level);
			PROP(eoc::ItemDefinition, StatsEntryName);
			PROP(eoc::ItemDefinition, HasModifiedSkills);
			PROP(eoc::ItemDefinition, Skills);
		}

		{
			auto & propertyMap = gEquipmentAttributesPropertyMap;
			PROP(CDivinityStats_Equipment_Attributes, Durability);
			PROP(CDivinityStats_Equipment_Attributes, DurabilityDegradeSpeed);
			PROP(CDivinityStats_Equipment_Attributes, StrengthBoost);
			PROP(CDivinityStats_Equipment_Attributes, FinesseBoost);
			PROP(CDivinityStats_Equipment_Attributes, IntelligenceBoost);
			PROP(CDivinityStats_Equipment_Attributes, ConstitutionBoost);
			PROP(CDivinityStats_Equipment_Attributes, Memory);
			PROP(CDivinityStats_Equipment_Attributes, WitsBoost);
			PROP(CDivinityStats_Equipment_Attributes, SightBoost);
			PROP(CDivinityStats_Equipment_Attributes, HearingBoost);
			PROP(CDivinityStats_Equipment_Attributes, VitalityBoost);
			PROP(CDivinityStats_Equipment_Attributes, SourcePointsBoost);
			PROP(CDivinityStats_Equipment_Attributes, MaxAP);
			PROP(CDivinityStats_Equipment_Attributes, StartAP);
			PROP(CDivinityStats_Equipment_Attributes, APRecovery);
			PROP(CDivinityStats_Equipment_Attributes, AccuracyBoost);
			PROP(CDivinityStats_Equipment_Attributes, DodgeBoost);
			PROP(CDivinityStats_Equipment_Attributes, LifeSteal);
			PROP(CDivinityStats_Equipment_Attributes, CriticalChance);
			PROP(CDivinityStats_Equipment_Attributes, ChanceToHitBoost);
			PROP(CDivinityStats_Equipment_Attributes, MovementSpeedBoost);
			PROP(CDivinityStats_Equipment_Attributes, RuneSlots);
			PROP(CDivinityStats_Equipment_Attributes, RuneSlots_V1);
			PROP(CDivinityStats_Equipment_Attributes, FireResistance);
			PROP(CDivinityStats_Equipment_Attributes, AirResistance);
			PROP(CDivinityStats_Equipment_Attributes, WaterResistance);
			PROP(CDivinityStats_Equipment_Attributes, EarthResistance);
			PROP(CDivinityStats_Equipment_Attributes, PoisonResistance);
			PROP(CDivinityStats_Equipment_Attributes, TenebriumResistance);
			PROP(CDivinityStats_Equipment_Attributes, PiercingResistance);
			PROP(CDivinityStats_Equipment_Attributes, CorrosiveResistance);
			PROP(CDivinityStats_Equipment_Attributes, PhysicalResistance);
			PROP(CDivinityStats_Equipment_Attributes, MagicResistance);
			PROP(CDivinityStats_Equipment_Attributes, CustomResistance);
			PROP(CDivinityStats_Equipment_Attributes, Movement);
			PROP(CDivinityStats_Equipment_Attributes, Initiative);
			PROP(CDivinityStats_Equipment_Attributes, Willpower);
			PROP(CDivinityStats_Equipment_Attributes, Bodybuilding);
			PROP(CDivinityStats_Equipment_Attributes, MaxSummons);
			PROP(CDivinityStats_Equipment_Attributes, Value);
			PROP(CDivinityStats_Equipment_Attributes, Weight);
			// TODO - Reflection
			PROP(CDivinityStats_Equipment_Attributes, Skills);
			PROP(CDivinityStats_Equipment_Attributes, ItemColor);
			PROP_FLAGS(CDivinityStats_Equipment_Attributes, AttributeFlags, StatAttributeFlags, true);
			// TODO - ObjectInstanceName?, AbilityModifiers, Talents
		}

		{
			auto & propertyMap = gEquipmentAttributesWeaponPropertyMap;
			propertyMap.Parent = &gEquipmentAttributesPropertyMap;
			PROP(CDivinityStats_Equipment_Attributes_Weapon, DamageType);
			PROP(CDivinityStats_Equipment_Attributes_Weapon, MinDamage);
			PROP(CDivinityStats_Equipment_Attributes_Weapon, MaxDamage);
			PROP(CDivinityStats_Equipment_Attributes_Weapon, DamageBoost);
			PROP(CDivinityStats_Equipment_Attributes_Weapon, DamageFromBase);
			PROP(CDivinityStats_Equipment_Attributes_Weapon, CriticalDamage);
			PROP(CDivinityStats_Equipment_Attributes_Weapon, WeaponRange);
			PROP(CDivinityStats_Equipment_Attributes_Weapon, CleaveAngle);
			PROP(CDivinityStats_Equipment_Attributes_Weapon, CleavePercentage);
			PROP(CDivinityStats_Equipment_Attributes_Weapon, AttackAPCost);
		}

		{
			auto & propertyMap = gEquipmentAttributesArmorPropertyMap;
			propertyMap.Parent = &gEquipmentAttributesPropertyMap;
			PROP(CDivinityStats_Equipment_Attributes_Armor, ArmorValue);
			PROP(CDivinityStats_Equipment_Attributes_Armor, ArmorBoost);
			PROP(CDivinityStats_Equipment_Attributes_Armor, MagicArmorValue);
			PROP(CDivinityStats_Equipment_Attributes_Armor, MagicArmorBoost);
		}

		{
			auto & propertyMap = gEquipmentAttributesShieldPropertyMap;
			propertyMap.Parent = &gEquipmentAttributesPropertyMap;
			PROP(CDivinityStats_Equipment_Attributes_Shield, ArmorValue);
			PROP(CDivinityStats_Equipment_Attributes_Shield, ArmorBoost);
			PROP(CDivinityStats_Equipment_Attributes_Shield, MagicArmorValue);
			PROP(CDivinityStats_Equipment_Attributes_Shield, MagicArmorBoost);
			PROP(CDivinityStats_Equipment_Attributes_Shield, Blocking);
		}

		{
			auto & propertyMap = gCharacterDynamicStatPropertyMap;
			PROP(CharacterDynamicStat, SummonLifelinkModifier);
			PROP(CharacterDynamicStat, Strength);
			PROP(CharacterDynamicStat, Memory);
			PROP(CharacterDynamicStat, Intelligence);
			PROP(CharacterDynamicStat, Movement);
			PROP(CharacterDynamicStat, MovementSpeedBoost);
			PROP(CharacterDynamicStat, Finesse);
			PROP(CharacterDynamicStat, Wits);
			PROP(CharacterDynamicStat, Constitution);

			PROP(CharacterDynamicStat, FireResistance);
			PROP(CharacterDynamicStat, EarthResistance);
			PROP(CharacterDynamicStat, WaterResistance);
			PROP(CharacterDynamicStat, AirResistance);
			PROP(CharacterDynamicStat, PoisonResistance);
			PROP(CharacterDynamicStat, ShadowResistance);

			PROP(CharacterDynamicStat, Willpower);
			PROP(CharacterDynamicStat, Bodybuilding);
			PROP(CharacterDynamicStat, PiercingResistance);
			PROP(CharacterDynamicStat, PhysicalResistance);
			PROP(CharacterDynamicStat, CorrosiveResistance);
			PROP(CharacterDynamicStat, MagicResistance);
			PROP(CharacterDynamicStat, CustomResistance);

			PROP(CharacterDynamicStat, Sight);
			PROP(CharacterDynamicStat, Hearing);
			PROP(CharacterDynamicStat, FOV);
			PROP(CharacterDynamicStat, APMaximum);
			PROP(CharacterDynamicStat, APStart);
			PROP(CharacterDynamicStat, APRecovery);
			PROP(CharacterDynamicStat, CriticalChance);
			PROP(CharacterDynamicStat, Initiative);
			PROP(CharacterDynamicStat, Vitality);
			PROP(CharacterDynamicStat, VitalityBoost);
			PROP(CharacterDynamicStat, MagicPoints);
			PROP(CharacterDynamicStat, Level);
			PROP(CharacterDynamicStat, Gain);

			PROP(CharacterDynamicStat, Armor);
			PROP(CharacterDynamicStat, MagicArmor);
			PROP(CharacterDynamicStat, ArmorBoost);
			PROP(CharacterDynamicStat, MagicArmorBoost);
			PROP(CharacterDynamicStat, ArmorBoostGrowthPerLevel);
			PROP(CharacterDynamicStat, MagicArmorBoostGrowthPerLevel);
			PROP(CharacterDynamicStat, DamageBoost);
			PROP(CharacterDynamicStat, DamageBoostGrowthPerLevel);

			PROP(CharacterDynamicStat, Accuracy);
			PROP(CharacterDynamicStat, Dodge);
			PROP(CharacterDynamicStat, MaxResistance);
			PROP(CharacterDynamicStat, LifeSteal);
			PROP(CharacterDynamicStat, Weight);
			PROP(CharacterDynamicStat, ChanceToHitBoost);
			PROP(CharacterDynamicStat, RangeBoost);
			PROP(CharacterDynamicStat, APCostBoost);
			PROP(CharacterDynamicStat, SPCostBoost);
			PROP(CharacterDynamicStat, MaxSummons);
			PROP(CharacterDynamicStat, BonusWeaponDamageMultiplier);
			// TODO Abilities, Talents, RemovedTalents, Traits
		}

		{
			auto & propertyMap = gCharacterStatsPropertyMap;
			PROP(CDivinityStats_Character, CurrentVitality);
			PROP(CDivinityStats_Character, CurrentArmor);
			PROP(CDivinityStats_Character, CurrentMagicArmor);
			PROP_RO(CDivinityStats_Character, ArmorAfterHitCooldownMultiplier);
			PROP_RO(CDivinityStats_Character, MagicArmorAfterHitCooldownMultiplier);
			PROP_RO(CDivinityStats_Character, MPStart);
			PROP_RO(CDivinityStats_Character, CurrentAP);
			PROP_RO(CDivinityStats_Character, BonusActionPoints);
			PROP_RO(CDivinityStats_Character, Experience);
			PROP_RO(CDivinityStats_Character, Reputation);
			PROP_RO(CDivinityStats_Character, Flanked);
			PROP_RO(CDivinityStats_Character, Karma);
			PROP_RO(CDivinityStats_Character, IsIncapacitatedRefCount);

			PROP_RO(CDivinityStats_Character, MaxVitality);
			PROP_RO(CDivinityStats_Character, BaseMaxVitality);
			PROP_RO(CDivinityStats_Character, MaxArmor);
			PROP_RO(CDivinityStats_Character, BaseMaxArmor);
			PROP_RO(CDivinityStats_Character, MaxMagicArmor);
			PROP_RO(CDivinityStats_Character, BaseMaxMagicArmor);
			PROP_RO(CDivinityStats_Character, Sight);
			PROP_RO(CDivinityStats_Character, BaseSight);
			PROP_RO(CDivinityStats_Character, MaxSummons);
			PROP_RO(CDivinityStats_Character, BaseMaxSummons);
			PROP_RO(CDivinityStats_Character, MaxMpOverride);
			PROP_FLAGS(CDivinityStats_Character, AttributeFlags, StatAttributeFlags, false);

			// TODO - DisabledTalents, TraitOrder?
		}

		{
			auto & propertyMap = gPlayerCustomDataPropertyMap;
			PROP_RO(esv::PlayerCustomData, CustomLookEnabled);
			PROP(esv::PlayerCustomData, Name);
			PROP(esv::PlayerCustomData, ClassType);
			PROP(esv::PlayerCustomData, SkinColor);
			PROP(esv::PlayerCustomData, HairColor);
			PROP(esv::PlayerCustomData, ClothColor1);
			PROP(esv::PlayerCustomData, ClothColor2);
			PROP(esv::PlayerCustomData, ClothColor3);
			PROP(esv::PlayerCustomData, IsMale);
			PROP(esv::PlayerCustomData, Race);
			PROP(esv::PlayerCustomData, OriginName);
			PROP(esv::PlayerCustomData, Icon);
			PROP(esv::PlayerCustomData, MusicInstrument);
			PROP_RO(esv::PlayerCustomData, OwnerProfileID);
			PROP_RO(esv::PlayerCustomData, ReservedProfileID);
			PROP(esv::PlayerCustomData, AiPersonality);
			PROP_RO(esv::PlayerCustomData, Speaker);
		}
	}


	bool OsirisPropertyMapGetRaw(PropertyMapBase const & propertyMap, void * obj,
		OsiArgumentDesc & args, uint32_t firstArg, OsiPropertyMapType type, bool throwError)
	{
		auto propertyName = args.Get(firstArg).String;

		if (obj == nullptr) {
			OsiError("Attempted to get property '" << propertyName << "' of null object!");
			return false;
		}

		switch (type) {
		case OsiPropertyMapType::Integer:
		{
			auto val = propertyMap.getInt(obj, propertyName, false, false);
			if (val) {
				args.Get(firstArg + 1).Int32 = (int32_t)*val;
			} else {
				auto boolval = propertyMap.getFlag(obj, propertyName, false, throwError);
				if (boolval) {
					args.Get(firstArg + 1).Int32 = *boolval ? 1 : 0;
				} else {
					return false;
				}
			}

			return true;
		}

		case OsiPropertyMapType::Integer64:
		{
			auto val = propertyMap.getInt(obj, propertyName, false, throwError);
			if (!val) {
				return false;
			}

			args.Get(firstArg + 1).Int64 = (int64_t)*val;
			return true;
		}

		case OsiPropertyMapType::Real:
		{
			auto val = propertyMap.getFloat(obj, propertyName, false, throwError);
			if (!val) {
				return false;
			}

			args.Get(firstArg + 1).Float = *val;
			return true;
		}

		case OsiPropertyMapType::String:
		{
			auto val = propertyMap.getString(obj, propertyName, false, throwError);
			if (!val) {
				return false;
			}

			args.Get(firstArg + 1).String = *val;
			return true;
		}

		case OsiPropertyMapType::GuidStringHandle:
		{
			auto val = propertyMap.getHandle(obj, propertyName, false, throwError);
			if (!val) {
				return false;
			}

			if (!*val) {
				args.Get(firstArg + 1).String = ToFixedString("NULL_00000000-0000-0000-0000-000000000000").Str;
				return true;
			}

			auto gameObject = FindGameObjectByHandle(*val);
			if (gameObject != nullptr) {
				args.Get(firstArg + 1).String = gameObject->GetGuid()->Str;
				return true;
			}

			OsiError("Failed to get property '" << propertyName << "': Could not map handle to game object: " << (int64_t)*val);
			return false;
		}

		case OsiPropertyMapType::Vector3:
		{
			auto val = propertyMap.getVector3(obj, propertyName, false, throwError);
			if (!val) {
				return false;
			}

			args.Get(firstArg + 1).Float = val->x;
			args.Get(firstArg + 2).Float = val->y;
			args.Get(firstArg + 3).Float = val->z;
			return true;
		}

		default:
			OsiError("Failed to get property '" << propertyName << "': Unknown Osi property type!");
			return false;
		}
	}


	bool OsirisPropertyMapSetRaw(PropertyMapBase const & propertyMap, void * obj,
		OsiArgumentDesc const & args, uint32_t firstArg, OsiPropertyMapType type, bool throwError)
	{
		auto propertyName = args.Get(firstArg).String;
		if (obj == nullptr) {
			OsiError("Attempted to set property '" << propertyName << "' of null object!");
			return false;
		}

		switch (type) {
		case OsiPropertyMapType::Integer:
		{
			auto val = args.Get(firstArg + 1).Int32;

			if (!propertyMap.setFlag(obj, propertyName, val != 0, false, false)) {
				return propertyMap.setInt(obj, propertyName, val, false, throwError);
			} else {
				return true;
			}
		}

		case OsiPropertyMapType::Integer64:
		{
			auto val = args.Get(firstArg + 1).Int64;
			return propertyMap.setInt(obj, propertyName, val, false, throwError);
		}

		case OsiPropertyMapType::Real:
		{
			auto val = args.Get(firstArg + 1).Float;
			return propertyMap.setFloat(obj, propertyName, val, false, throwError);
		}

		case OsiPropertyMapType::String:
		{
			auto val = args.Get(firstArg + 1).String;
			return propertyMap.setString(obj, propertyName, val, false, throwError);
		}

		case OsiPropertyMapType::GuidStringHandle:
		{
			auto guid = args.Get(firstArg + 1).String;
			if (guid == nullptr
				|| !strlen(guid)
				|| NameGuidToFixedString(guid) == ToFixedString("00000000-0000-0000-0000-000000000000").Str) {
				return propertyMap.setHandle(obj, propertyName, ObjectHandle{}, false, throwError);
			}

			auto gameObject = FindGameObjectByNameGuid(guid);
			if (gameObject != nullptr) {
				ObjectHandle handle;
				gameObject->GetObjectHandle(&handle);
				return propertyMap.setHandle(obj, propertyName, handle, false, throwError);
			}

			OsiError("Failed to set property '" << propertyName << "': Could not map GUID to game object: " << guid);
			return false;
		}

		case OsiPropertyMapType::Vector3:
		{
			auto x = args.Get(firstArg + 1).Float;
			auto y = args.Get(firstArg + 2).Float;
			auto z = args.Get(firstArg + 3).Float;
			Vector3 vec(x, y, z);
			return propertyMap.setVector3(obj, propertyName, vec, false, throwError);
		}

		default:
			OsiError("Failed to set property '" << propertyName << "': Unknown Osi property type!");
			return false;
		}
	}
}