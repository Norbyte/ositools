#include <stdafx.h>
#include <OsirisProxy.h>
#include "PropertyMaps.h"

namespace osidbg
{
	PropertyMap<EsvStatus, void> gStatusPropertyMap;
	PropertyMap<EsvStatusConsume, EsvStatus> gStatusConsumePropertyMap;
	PropertyMap<EsvStatusHit, EsvStatus> gStatusHitPropertyMap;
	PropertyMap<EsvStatusHeal, EsvStatus> gStatusHealPropertyMap;
	PropertyMap<EsvStatusHealing, EsvStatusConsume> gStatusHealingPropertyMap;
	PropertyMap<HitDamageInfo, void> gHitDamageInfoPropertyMap;
	PropertyMap<EoCItemDefinition, void> gEoCItemDefinitionPropertyMap;
	PropertyMap<CDivinityStats_Equipment_Attributes, void> gEquipmentAttributesPropertyMap;
	PropertyMap<CDivinityStats_Equipment_Attributes_Weapon, CDivinityStats_Equipment_Attributes> gEquipmentAttributesWeaponPropertyMap;
	PropertyMap<CDivinityStats_Equipment_Attributes_Armor, CDivinityStats_Equipment_Attributes> gEquipmentAttributesArmorPropertyMap;
	PropertyMap<CDivinityStats_Equipment_Attributes_Shield, CDivinityStats_Equipment_Attributes> gEquipmentAttributesShieldPropertyMap;
	PropertyMap<CharacterDynamicStat, void> gCharacterDynamicStatPropertyMap;
	PropertyMap<CDivinityStats_Character, void> gCharacterStatsPropertyMap;

#define PROP(cls, name) AddProperty<decltype(cls::name)>(propertyMap, #name, offsetof(cls, name))
#define PROP_RO(cls, name) AddPropertyRO<decltype(cls::name)>(propertyMap, #name, offsetof(cls, name))
#define PROP_ENUM(cls, name) AddPropertyEnum<decltype(cls::name)>(propertyMap, #name, offsetof(cls, name))
#define PROP_FLAGS(cls, name, enum) AddPropertyFlags<decltype(cls::name), enum>(propertyMap, #name, offsetof(cls, name))

	void InitPropertyMaps()
	{
		{
			auto & propertyMap = gStatusPropertyMap;
			PROP_RO(EsvStatus, StatusId);
			PROP_RO(EsvStatus, CanEnterChance);
			PROP_RO(EsvStatus, StartTimer);
			PROP_RO(EsvStatus, LifeTime);
			PROP_RO(EsvStatus, CurrentLifeTime);
			PROP_RO(EsvStatus, TurnTimer);
			PROP_RO(EsvStatus, Strength);
			PROP_RO(EsvStatus, StatsMultiplier);
			PROP_ENUM(EsvStatus, DamageSourceType);
			PROP_RO(EsvStatus, StatusHandle);
			PROP_RO(EsvStatus, TargetCIHandle);
			PROP_RO(EsvStatus, StatusSourceHandle);
			PROP_RO(EsvStatus, SomeHandle);
			PROP_FLAGS(EsvStatus, Flags0, StatusFlags0);
			PROP_FLAGS(EsvStatus, Flags1, StatusFlags1);
			PROP_FLAGS(EsvStatus, Flags2, StatusFlags2);

			propertyMap.Properties["LifeTime"].SetFloat = [](EsvStatus * status, float value) -> bool {
				if (value < 0.0f) return false;
				status->LifeTime = value;
				if (status->CurrentLifeTime > status->LifeTime) {
					status->CurrentLifeTime = status->LifeTime;
				}

				return true;
			};

			propertyMap.Properties["CurrentLifeTime"].SetFloat = [](EsvStatus * status, float value) -> bool {
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
			PROP_RO(EsvStatusHit, HitByHandle);
			PROP_RO(EsvStatusHit, HitWithHandle);
			PROP_RO(EsvStatusHit, WeaponHandle);
			PROP(EsvStatusHit, HitReason);
			PROP(EsvStatusHit, SkillId);
			PROP(EsvStatusHit, Interruption);
			PROP(EsvStatusHit, AllowInterruptAction);
			PROP(EsvStatusHit, ForceInterrupt);
			PROP_RO(EsvStatusHit, DecDelayDeathCount);
			PROP(EsvStatusHit, ImpactPosition);
			PROP(EsvStatusHit, ImpactOrigin);
			PROP(EsvStatusHit, ImpactDirection);
		}

		{
			auto & propertyMap = gStatusConsumePropertyMap;
			propertyMap.Parent = &gStatusPropertyMap;
			// TODO - Skills, Items, ResetCooldownsSet, StatsIDs?
			PROP(EsvStatusConsume, ResetAllCooldowns);
			PROP(EsvStatusConsume, ResetOncePerCombat);
			PROP(EsvStatusConsume, ScaleWithVitality);
			PROP(EsvStatusConsume, LoseControl);
			PROP(EsvStatusConsume, ApplyStatusOnTick);
			PROP(EsvStatusConsume, EffectTime);
			PROP(EsvStatusConsume, StatsId);
			PROP(EsvStatusConsume, StackId);
			PROP(EsvStatusConsume, OriginalWeaponStatsId);
			PROP(EsvStatusConsume, OverrideWeaponStatsId);
			PROP(EsvStatusConsume, OverrideWeaponHandle);
			PROP(EsvStatusConsume, SavingThrow);
			PROP(EsvStatusConsume, SourceDirection);
			PROP(EsvStatusConsume, Turn);
			PROP(EsvStatusConsume, HealEffectOverride);
		}

		{
			auto & propertyMap = gStatusHealingPropertyMap;
			propertyMap.Parent = &gStatusConsumePropertyMap;
			PROP(EsvStatusHealing, HealAmount);
			PROP(EsvStatusHealing, TimeElapsed);
			PROP(EsvStatusHealing, HealEffect);
			PROP(EsvStatusHealing, HealEffectId);
			PROP(EsvStatusHealing, SkipInitialEffect);
			PROP(EsvStatusHealing, HealingEvent);
			PROP(EsvStatusHealing, HealStat);
			PROP(EsvStatusHealing, AbsorbSurfaceRange);
		}

		{
			auto & propertyMap = gStatusHealPropertyMap;
			propertyMap.Parent = &gStatusPropertyMap;
			PROP(EsvStatusHeal, EffectTime);
			PROP(EsvStatusHeal, HealAmount);
			PROP(EsvStatusHeal, HealEffect);
			PROP(EsvStatusHeal, HealEffectId);
			PROP(EsvStatusHeal, HealType);
			PROP(EsvStatusHeal, AbsorbSurfaceRange);
			PROP(EsvStatusHeal, TargetDependentHeal);
		}

		{
			auto & propertyMap = gHitDamageInfoPropertyMap;
			PROP(HitDamageInfo, Equipment);
			PROP(HitDamageInfo, TotalDamageDone);
			PROP(HitDamageInfo, Unknown);
			PROP_ENUM(HitDamageInfo, DeathType);
			PROP_ENUM(HitDamageInfo, DamageType);
			PROP(HitDamageInfo, AttackDirection);
			PROP(HitDamageInfo, ArmorAbsorption);
			PROP(HitDamageInfo, LifeSteal);
			PROP_FLAGS(HitDamageInfo, EffectFlags, HitFlag);
			PROP(HitDamageInfo, HitWithWeapon);
		}

		{
			auto & propertyMap = gEoCItemDefinitionPropertyMap;
			PROP(EoCItemDefinition, RootTemplate);
			PROP(EoCItemDefinition, OriginalRootTemplate);
			PROP(EoCItemDefinition, Amount);
			PROP(EoCItemDefinition, Slot);
			PROP(EoCItemDefinition, GoldValueOverwrite);
			PROP(EoCItemDefinition, WeightValueOverwrite);
			PROP(EoCItemDefinition, DamageTypeOverwrite);
			PROP(EoCItemDefinition, ItemType);
			PROP(EoCItemDefinition, GenerationStatsId);
			PROP(EoCItemDefinition, GenerationItemType);
			PROP(EoCItemDefinition, GenerationRandom);
			PROP(EoCItemDefinition, GenerationLevel);
			PROP(EoCItemDefinition, StatsName);
			PROP(EoCItemDefinition, Level);
			PROP(EoCItemDefinition, StatsEntryName);
			PROP(EoCItemDefinition, HasModifiedSkills);
			PROP(EoCItemDefinition, Skills);
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
			PROP_RO(CDivinityStats_Character, CurrentVitality);
			PROP_RO(CDivinityStats_Character, CurrentArmor);
			PROP_RO(CDivinityStats_Character, CurrentMagicArmor);
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

			// TODO - DisabledTalents, TraitOrder, flags01/23?
		}
	}
}