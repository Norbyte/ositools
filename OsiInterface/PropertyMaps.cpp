#include <stdafx.h>
#include <OsirisProxy.h>
#include "PropertyMaps.h"

namespace dse
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
	PropertyMap<CDivinityStats_Item, void> gItemStatsPropertyMap;
	PropertyMap<esv::PlayerCustomData, void> gPlayerCustomDataPropertyMap;
	PropertyMap<esv::EoCServerObject, void> gEoCServerObjectPropertyMap;
	PropertyMap<esv::Character, void> gCharacterPropertyMap;
	PropertyMap<esv::Item, void> gItemPropertyMap;
	PropertyMap<esv::ASPrepareSkill, void> gASPrepareSkillStatPropertyMap;
	PropertyMap<esv::ASUseSkill, void> gASUseSkillStatPropertyMap;

#define PROP(cls, name) AddProperty<decltype(cls::name)>(propertyMap, #name, offsetof(cls, name))
#define PROP_RO(cls, name) AddPropertyRO<decltype(cls::name)>(propertyMap, #name, offsetof(cls, name))
#define PROP_ENUM(cls, name) AddPropertyEnum<decltype(cls::name)>(propertyMap, #name, offsetof(cls, name))
#define PROP_FLAGS(cls, name, enum, writeable) AddPropertyFlags<decltype(cls::name), enum>(propertyMap, #name, offsetof(cls, name), writeable)
#define PROP_GUID(cls, name, writeable) AddPropertyGuidString<decltype(cls::name)>(propertyMap, #name, offsetof(cls, name), writeable)

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
			PROP_RO(esv::Status, TargetHandle);
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
			PROP_ENUM(esv::StatusConsume, HealEffectOverride);
		}

		{
			auto & propertyMap = gStatusHealingPropertyMap;
			propertyMap.Parent = &gStatusConsumePropertyMap;
			PROP(esv::StatusHealing, HealAmount);
			PROP(esv::StatusHealing, TimeElapsed);
			PROP_ENUM(esv::StatusHealing, HealEffect);
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
			PROP_ENUM(esv::StatusHeal, HealEffect);
			PROP(esv::StatusHeal, HealEffectId);
			PROP_ENUM(esv::StatusHeal, HealType);
			PROP(esv::StatusHeal, AbsorbSurfaceRange);
			PROP(esv::StatusHeal, TargetDependentHeal);
		}

		{
			auto & propertyMap = gHitDamageInfoPropertyMap;
			PROP(HitDamageInfo, Equipment);
			PROP_RO(HitDamageInfo, TotalDamage);
			PROP_RO(HitDamageInfo, DamageDealt);
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
			PROP_GUID(eoc::ItemDefinition, RootTemplate, true);
			PROP_GUID(eoc::ItemDefinition, OriginalRootTemplate, true);
			PROP(eoc::ItemDefinition, Amount);
			PROP(eoc::ItemDefinition, Slot);
			PROP(eoc::ItemDefinition, GoldValueOverwrite);
			PROP(eoc::ItemDefinition, WeightValueOverwrite);
			PROP_ENUM(eoc::ItemDefinition, DamageTypeOverwrite);
			PROP(eoc::ItemDefinition, ItemType);
			PROP(eoc::ItemDefinition, CustomDisplayName);
			PROP(eoc::ItemDefinition, CustomDescription);
			PROP(eoc::ItemDefinition, CustomBookContent);
			PROP(eoc::ItemDefinition, GenerationStatsId);
			PROP(eoc::ItemDefinition, GenerationItemType);
			PROP(eoc::ItemDefinition, GenerationRandom);
			PROP(eoc::ItemDefinition, GenerationLevel);
			PROP(eoc::ItemDefinition, StatsLevel);
			PROP(eoc::ItemDefinition, Key);
			PROP(eoc::ItemDefinition, LockLevel);
			PROP(eoc::ItemDefinition, StatsEntryName);
			PROP(eoc::ItemDefinition, HasModifiedSkills);
			PROP(eoc::ItemDefinition, Skills);
			PROP(eoc::ItemDefinition, HasGeneratedStats);
			PROP(eoc::ItemDefinition, IsIdentified);
			PROP(eoc::ItemDefinition, GMFolding);
			PROP(eoc::ItemDefinition, CanUseRemotely);
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
			PROP(CDivinityStats_Equipment_Attributes, ModifierType);
			PROP(CDivinityStats_Equipment_Attributes, ObjectInstanceName);
			PROP(CDivinityStats_Equipment_Attributes, BoostName);
			PROP_ENUM(CDivinityStats_Equipment_Attributes, StatsType);
			// TODO - add attribute flags object support
			// PROP_FLAGS(CDivinityStats_Equipment_Attributes, AttributeFlags, StatAttributeFlags, true);
			// TODO - AbilityModifiers, Talents
		}

		{
			auto & propertyMap = gEquipmentAttributesWeaponPropertyMap;
			propertyMap.Parent = &gEquipmentAttributesPropertyMap;
			PROP_ENUM(CDivinityStats_Equipment_Attributes_Weapon, DamageType);
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
			PROP(CharacterDynamicStat, TranslationKey);
			PROP(CharacterDynamicStat, BonusWeapon);
			PROP(CharacterDynamicStat, StepsType);
			// TODO Abilities, Talents, RemovedTalents, Traits
		}

		{
			auto & propertyMap = gCharacterStatsPropertyMap;
			// CRPGStats_Object
			PROP_RO(CDivinityStats_Character, Level);
			PROP_RO(CDivinityStats_Character, Name);
			PROP_RO(CDivinityStats_Character, AIFlags);
			PROP_RO(CDivinityStats_Character, InstanceId);

			// CDivinityStats_Character
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
			PROP_FLAGS(CDivinityStats_Character, Flags, StatCharacterFlags, false);
			PROP_RO(CDivinityStats_Character, MaxResistance);
			PROP_RO(CDivinityStats_Character, HasTwoHandedWeapon);
			PROP_RO(CDivinityStats_Character, IsIncapacitatedRefCount);

			{
				PropertyMapBase::PropertyInfo info;
				info.Type = PropertyType::kFixedStringGuid;
				info.Offset = 0;
				info.Flags = kPropRead;
				info.GetString = [](void * obj) -> std::optional<char const *> {
					auto self = reinterpret_cast<CDivinityStats_Character *>(obj);
					if (self->Character != nullptr) {
						// MyGuid and WorldPos are in the same location in both esv::Character 
						// and ecl::Character, so it's safe to access them on both client and server
						return self->Character->MyGuid.Str;
					} else {
						return {};
					}
				};
				propertyMap.Properties.insert(std::make_pair("CharacterGuid", info));
			}

			{
				PropertyMapBase::PropertyInfo info;
				info.Type = PropertyType::kVector3;
				info.Offset = 0;
				info.Flags = kPropRead;
				info.GetVector3 = [](void * obj) -> std::optional<Vector3> {
					auto self = reinterpret_cast<CDivinityStats_Character *>(obj);
					if (self->Character != nullptr) {
						return self->Character->WorldPos;
					} else {
						return {};
					}
				};
				propertyMap.Properties.insert(std::make_pair("WorldPosition", info));
			}

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
			auto & propertyMap = gItemStatsPropertyMap;
			// CRPGStats_Object
			PROP_RO(CDivinityStats_Item, Level);
			PROP_RO(CDivinityStats_Item, Name);
			PROP_RO(CDivinityStats_Item, InstanceId);

			// CDivinityStats_Item
			PROP_ENUM(CDivinityStats_Item, ItemType);
			PROP_RO(CDivinityStats_Item, ItemSlot);
			PROP_ENUM(CDivinityStats_Item, WeaponType);
			PROP_RO(CDivinityStats_Item, AnimType);
			PROP_RO(CDivinityStats_Item, WeaponRange);
			PROP_RO(CDivinityStats_Item, IsIdentified);
			PROP_RO(CDivinityStats_Item, IsTwoHanded);
			PROP_RO(CDivinityStats_Item, ShouldSyncStats);
			PROP_RO(CDivinityStats_Item, HasModifiedSkills);
			PROP_RO(CDivinityStats_Item, Skills);
			PROP_ENUM(CDivinityStats_Item, DamageTypeOverwrite);
			PROP_RO(CDivinityStats_Item, Durability);
			PROP_RO(CDivinityStats_Item, DurabilityCounter);
			PROP_RO(CDivinityStats_Item, ItemTypeReal);
			PROP_FLAGS(CDivinityStats_Item, AttributeFlags, StatAttributeFlags, false);
			PROP_RO(CDivinityStats_Item, MaxCharges);
			PROP_RO(CDivinityStats_Item, Charges);
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

		{
			auto & propertyMap = gEoCServerObjectPropertyMap;
			PROP_RO(esv::EoCServerObject, NetID);
			PROP_RO(esv::EoCServerObject, MyGuid);
			PROP_RO(esv::EoCServerObject, WorldPos);
			PROP_RO(esv::EoCServerObject, CurrentLevel);
		}

		{
			auto & propertyMap = gCharacterPropertyMap;
			// EoCServerObject
			PROP_RO(esv::Character, MyGuid);
			PROP_RO(esv::Character, WorldPos);
			PROP_RO(esv::Character, CurrentLevel);
			PROP_FLAGS(esv::Character, Flags, EsvCharacterFlags, false);
			// Character
			PROP_FLAGS(esv::Character, Flags2, EsvCharacterFlags2, false);
			PROP_FLAGS(esv::Character, Flags3, EsvCharacterFlags3, false);
			PROP_RO(esv::Character, Scale);
			PROP_RO(esv::Character, AnimationOverride);
			PROP_RO(esv::Character, WalkSpeedOverride);
			PROP_RO(esv::Character, RunSpeedOverride);
			PROP_RO(esv::Character, NeedsUpdateCount);
			PROP_RO(esv::Character, ScriptForceUpdateCount);
			PROP_RO(esv::Character, ForceSynchCount);
			PROP_RO(esv::Character, InventoryHandle);
			PROP_RO(esv::Character, SkillBeingPrepared);
			PROP_RO(esv::Character, LifeTime);
			PROP_RO(esv::Character, OwnerHandle);
			PROP_RO(esv::Character, PartialAP);
			PROP_RO(esv::Character, AnimType);
			PROP_RO(esv::Character, DelayDeathCount);
			PROP_RO(esv::Character, AnimationSetOverride);
			PROP_RO(esv::Character, CustomTradeTreasure);
			PROP_RO(esv::Character, Archetype);
			PROP_RO(esv::Character, EquipmentColor);
		}

		{
			auto & propertyMap = gItemPropertyMap;
			// EoCServerObject
			PROP_RO(esv::Item, MyGuid);
			PROP_RO(esv::Item, WorldPos);
			PROP_RO(esv::Item, CurrentLevel);
			// Item
			PROP_RO(esv::Item, Scale);
			PROP_RO(esv::Item, CurrentTemplate);
			PROP_RO(esv::Item, CustomDisplayName);
			PROP_RO(esv::Item, CustomDescription);
			PROP_RO(esv::Item, CustomBookContent);
			PROP_RO(esv::Item, StatsId);
			PROP_RO(esv::Item, InventoryHandle);
			PROP_RO(esv::Item, ParentInventoryHandle);
			PROP_RO(esv::Item, Slot);
			PROP_RO(esv::Item, Amount);
			PROP_RO(esv::Item, Vitality);
			PROP_RO(esv::Item, Armor);
			PROP_RO(esv::Item, InUseByCharacterHandle);
			PROP_RO(esv::Item, Key);
			PROP_RO(esv::Item, LockLevel);
			PROP_RO(esv::Item, ComputedVitality);
			PROP_RO(esv::Item, ItemType);
			PROP_RO(esv::Item, GoldValueOverwrite);
			PROP_RO(esv::Item, WeightValueOverwrite);
			PROP_RO(esv::Item, TreasureLevel);
			PROP_RO(esv::Item, LevelOverride);
			PROP_RO(esv::Item, ForceSynch);
		}

		{
			auto & propertyMap = gASPrepareSkillStatPropertyMap;
			PROP_RO(esv::ASPrepareSkill, SkillId);
			PROP_RO(esv::ASPrepareSkill, PrepareAnimationInit);
			PROP_RO(esv::ASPrepareSkill, PrepareAnimationLoop);
			PROP_RO(esv::ASPrepareSkill, IsFinished);
			PROP_RO(esv::ASPrepareSkill, IsEntered);
		}

		{
			auto & propertyMap = gASUseSkillStatPropertyMap;
			// FIXME
		}
	}


	bool OsirisPropertyMapGetRaw(PropertyMapBase const & propertyMap, void * obj,
		OsiArgumentDesc & args, uint32_t firstArg, OsiPropertyMapType type, bool throwError)
	{
		auto propertyName = args[firstArg].String;

		if (obj == nullptr) {
			OsiError("Attempted to get property '" << propertyName << "' of null object!");
			return false;
		}

		switch (type) {
		case OsiPropertyMapType::Integer:
		{
			auto val = propertyMap.getInt(obj, propertyName, false, false);
			if (val) {
				args[firstArg + 1].Set((int32_t)*val);
			} else {
				auto boolval = propertyMap.getFlag(obj, propertyName, false, throwError);
				if (boolval) {
					args[firstArg + 1].Set((int32_t)(*boolval ? 1 : 0));
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

			args[firstArg + 1].Set((int64_t)*val);
			return true;
		}

		case OsiPropertyMapType::Real:
		{
			auto val = propertyMap.getFloat(obj, propertyName, false, throwError);
			if (!val) {
				return false;
			}

			args[firstArg + 1].Set(*val);
			return true;
		}

		case OsiPropertyMapType::String:
		{
			auto val = propertyMap.getString(obj, propertyName, false, throwError);
			if (!val) {
				return false;
			}

			args[firstArg + 1].Set(*val);
			return true;
		}

		case OsiPropertyMapType::GuidStringHandle:
		{
			auto val = propertyMap.getHandle(obj, propertyName, false, throwError);
			if (!val) {
				return false;
			}

			if (!*val) {
				args[firstArg + 1].Set(ToFixedString("NULL_00000000-0000-0000-0000-000000000000").Str);
				return true;
			}

			auto gameObject = FindGameObjectByHandle(*val);
			if (gameObject != nullptr) {
				args[firstArg + 1].Set(gameObject->GetGuid()->Str);
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

			args[firstArg + 1].Set(val->x);
			args[firstArg + 2].Set(val->y);
			args[firstArg + 3].Set(val->z);
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
		auto propertyName = args[firstArg].String;
		if (obj == nullptr) {
			OsiError("Attempted to set property '" << propertyName << "' of null object!");
			return false;
		}

		switch (type) {
		case OsiPropertyMapType::Integer:
		{
			auto val = args[firstArg + 1].Int32;

			if (!propertyMap.setFlag(obj, propertyName, val != 0, false, false)) {
				return propertyMap.setInt(obj, propertyName, val, false, throwError);
			} else {
				return true;
			}
		}

		case OsiPropertyMapType::Integer64:
		{
			auto val = args[firstArg + 1].Int64;
			return propertyMap.setInt(obj, propertyName, val, false, throwError);
		}

		case OsiPropertyMapType::Real:
		{
			auto val = args[firstArg + 1].Float;
			return propertyMap.setFloat(obj, propertyName, val, false, throwError);
		}

		case OsiPropertyMapType::String:
		{
			auto val = args[firstArg + 1].String;
			return propertyMap.setString(obj, propertyName, val, false, throwError);
		}

		case OsiPropertyMapType::GuidStringHandle:
		{
			auto guid = args[firstArg + 1].String;
			if (guid == nullptr
				|| !strlen(guid)
				|| NameGuidToFixedString(guid) == ToFixedString("00000000-0000-0000-0000-000000000000")) {
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
			auto x = args[firstArg + 1].Float;
			auto y = args[firstArg + 2].Float;
			auto z = args[firstArg + 3].Float;
			Vector3 vec(x, y, z);
			return propertyMap.setVector3(obj, propertyName, vec, false, throwError);
		}

		default:
			OsiError("Failed to set property '" << propertyName << "': Unknown Osi property type!");
			return false;
		}
	}


	bool LuaPropertyMapGet(lua_State * L, PropertyMapBase const & propertyMap, void * obj,
		char const * propertyName, bool throwError)
	{
		if (obj == nullptr) {
			if (throwError) {
				OsiError("Attempted to get property '" << propertyName << "' of null object!");
			}
			return false;
		}

		auto prop = propertyMap.findProperty(propertyName);
		if (prop == nullptr) {
			auto flag = propertyMap.findFlag(propertyName);
			if (flag == nullptr) {
				if (throwError) {
					OsiError("Failed to get property '" << propertyName << "': Property does not exist");
				}
				return {};
			} else {
				auto val = propertyMap.getFlag(obj, propertyName, false, throwError);
				if (val) {
					lua_pushboolean(L, *val);
					return true;
				} else {
					return false;
				}
			}
		}

		auto type = prop->Type;
		if (prop->GetString) {
			// Return enumeration labels instead of IDs if possible
			type = PropertyType::kFixedString;
		}

		switch (type) {
		case PropertyType::kBool:
		{
			auto val = propertyMap.getInt(obj, propertyName, false, throwError);
			if (val) {
				lua_pushboolean(L, *val != 0);
				return true;
			} else {
				return false;
			}
		}

		case PropertyType::kUInt8:
		case PropertyType::kInt16:
		case PropertyType::kUInt16:
		case PropertyType::kInt32:
		case PropertyType::kUInt32:
		case PropertyType::kInt64:
		case PropertyType::kUInt64:
		{
			auto val = propertyMap.getInt(obj, propertyName, false, throwError);
			if (val) {
				lua_pushinteger(L, *val);
				return true;
			} else {
				return false;
			}
		}

		case PropertyType::kFloat:
		{
			auto val = propertyMap.getFloat(obj, propertyName, false, throwError);
			if (val) {
				lua_pushnumber(L, *val);
				return true;
			} else {
				return false;
			}
		}

		case PropertyType::kFixedString:
		case PropertyType::kFixedStringGuid:
		case PropertyType::kStringPtr:
		case PropertyType::kStdString:
		case PropertyType::kStdWString:
		{
			auto val = propertyMap.getString(obj, propertyName, false, throwError);
			if (val) {
				lua_pushstring(L, *val);
				return true;
			} else {
				return false;
			}
		}

		case PropertyType::kObjectHandle:
		{
			auto val = propertyMap.getHandle(obj, propertyName, false, throwError);
			if (val) {
				if (*val) {
					lua_pushinteger(L, val->Handle);
				} else {
					lua_pushnil(L);
				}
				return true;
			} else {
				return false;
			}
		}

		default:
			OsiError("Failed to get property '" << propertyName << "': Unsupported property type for Lua fetch!");
			return false;
		}
	}


	bool LuaPropertyMapSet(lua_State * L, int index, PropertyMapBase const & propertyMap, 
		void * obj, char const * propertyName, bool throwError)
	{
		if (obj == nullptr) {
			if (throwError) {
				OsiError("Attempted to set property '" << propertyName << "' of null object!");
			}
			return false;
		}

		auto prop = propertyMap.findProperty(propertyName);
		if (prop == nullptr) {
			auto flag = propertyMap.findFlag(propertyName);
			if (flag == nullptr) {
				if (throwError) {
					OsiError("Failed to set property '" << propertyName << "': Property does not exist");
				}
				return {};
			} else {
				luaL_checktype(L, index, LUA_TBOOLEAN);
				auto val = lua_toboolean(L, index);
				return propertyMap.setFlag(obj, propertyName, val == 1, false, throwError);
			}
		}

		auto type = prop->Type;
		if (prop->SetString && lua_type(L, index) == LUA_TSTRING) {
			// Allow setting enumerations using labels
			type = PropertyType::kFixedString;
		}

		switch (type) {
		case PropertyType::kBool:
		{
			luaL_checktype(L, index, LUA_TBOOLEAN);
			auto val = lua_toboolean(L, index);
			return propertyMap.setFlag(obj, propertyName, val == 1, false, throwError);
		}

		case PropertyType::kUInt8:
		case PropertyType::kInt16:
		case PropertyType::kUInt16:
		case PropertyType::kInt32:
		case PropertyType::kUInt32:
		case PropertyType::kInt64:
		case PropertyType::kUInt64:
		{
			auto val = luaL_checkinteger(L, index);
			return propertyMap.setInt(obj, propertyName, val, false, throwError);
		}

		case PropertyType::kFloat:
		{
			auto val = luaL_checknumber(L, index);
			return propertyMap.setFloat(obj, propertyName, (float)val, false, throwError);
		}

		case PropertyType::kFixedString:
		case PropertyType::kFixedStringGuid:
		case PropertyType::kStringPtr:
		case PropertyType::kStdString:
		case PropertyType::kStdWString:
		{
			auto val = luaL_checkstring(L, index);
			return propertyMap.setString(obj, propertyName, val, false, throwError);
		}

		case PropertyType::kObjectHandle:
		{
			auto val = luaL_checkinteger(L, index);
			return propertyMap.setHandle(obj, propertyName, ObjectHandle(val), false, throwError);
		}

		default:
			OsiError("Failed to set property '" << propertyName << "': Unsupported property type for Lua fetch!");
			return false;
		}
	}
}