#include <stdafx.h>
#include <OsirisProxy.h>
#include <GameDefinitions/Projectile.h>
#include <GameDefinitions/RootTemplates.h>
#include <GameDefinitions/Surface.h>
#include "PropertyMaps.h"

namespace dse
{
	PropertyMap<esv::Status, void> gStatusPropertyMap;
	PropertyMap<esv::StatusConsume, esv::Status> gStatusConsumePropertyMap;
	PropertyMap<esv::StatusHit, esv::Status> gStatusHitPropertyMap;
	PropertyMap<esv::StatusHeal, esv::Status> gStatusHealPropertyMap;
	PropertyMap<esv::StatusHealing, esv::StatusConsume> gStatusHealingPropertyMap;
	PropertyMap<HitDamageInfo, void> gHitDamageInfoPropertyMap;
	PropertyMap<esv::DamageHelpers, void> gDamageHelpersPropertyMap;
	PropertyMap<esv::ShootProjectileHelper, void> gShootProjectileHelperPropertyMap;
	PropertyMap<eoc::ItemDefinition, void> gEoCItemDefinitionPropertyMap;
	PropertyMap<CDivinityStats_Equipment_Attributes, void> gEquipmentAttributesPropertyMap;
	PropertyMap<CDivinityStats_Equipment_Attributes_Weapon, CDivinityStats_Equipment_Attributes> gEquipmentAttributesWeaponPropertyMap;
	PropertyMap<CDivinityStats_Equipment_Attributes_Armor, CDivinityStats_Equipment_Attributes> gEquipmentAttributesArmorPropertyMap;
	PropertyMap<CDivinityStats_Equipment_Attributes_Shield, CDivinityStats_Equipment_Attributes> gEquipmentAttributesShieldPropertyMap;
	PropertyMap<CharacterDynamicStat, void> gCharacterDynamicStatPropertyMap;
	PropertyMap<CDivinityStats_Character, void> gCharacterStatsPropertyMap;
	PropertyMap<CDivinityStats_Item, void> gItemStatsPropertyMap;
	PropertyMap<eoc::PlayerCustomData, void> gPlayerCustomDataPropertyMap;
	PropertyMap<esv::Character, void> gCharacterPropertyMap;
	PropertyMap<esv::Item, void> gItemPropertyMap;
	PropertyMap<esv::Projectile, void> gProjectilePropertyMap;
	PropertyMap<esv::Surface, void> gEsvSurfacePropertyMap;

	PropertyMap<esv::ASAttack, void> gASAttackPropertyMap;
	PropertyMap<esv::ASPrepareSkill, void> gASPrepareSkillPropertyMap;
	PropertyMap<esv::SkillState, void> gSkillStatePropertyMap;

	PropertyMap<ecl::Character, void> gEclCharacterPropertyMap;
	PropertyMap<ecl::Item, void> gEclItemPropertyMap;
	PropertyMap<ecl::Status, void> gEclStatusPropertyMap;

	PropertyMap<GameObjectTemplate, void> gGameObjectTemplatePropertyMap;
	PropertyMap<EoCGameObjectTemplate, GameObjectTemplate> gEoCGameObjectTemplatePropertyMap;
	PropertyMap<CharacterTemplate, EoCGameObjectTemplate> gCharacterTemplatePropertyMap;
	PropertyMap<ItemTemplate, EoCGameObjectTemplate> gItemTemplatePropertyMap;
	PropertyMap<ProjectileTemplate, EoCGameObjectTemplate> gProjectileTemplatePropertyMap;
	PropertyMap<CombatComponentTemplate, void> gCombatComponentTemplatePropertyMap;

#define BEGIN_PROPERTIES(map, cls) auto& propertyMap = map; \
	using TObject = decltype(map)::ObjectType; \
	propertyMap.Name = #cls;

#define PROP(name) AddProperty<decltype(TObject::name)>(propertyMap, #name, offsetof(TObject, name))
#define PROP_TY(name, type) AddPropertyRO<type>(propertyMap, #name, offsetof(TObject, name))
#define PROP_RO(name) AddPropertyRO<decltype(TObject::name)>(propertyMap, #name, offsetof(TObject, name))
#define PROP_ENUM(name) AddPropertyEnum<decltype(TObject::name)>(propertyMap, #name, offsetof(TObject, name))
#define PROP_FLAGS(name, enum, writeable) AddPropertyFlags<std::underlying_type_t<enum>, enum>(propertyMap, #name, offsetof(TObject, name), writeable)
#define PROP_GUID(name, writeable) AddPropertyGuidString<decltype(TObject::name)>(propertyMap, #name, offsetof(TObject, name), writeable)
#define PROP_DYN_FS(name) AddPropertyDynamicFixedString<decltype(TObject::name)>(propertyMap, #name, offsetof(TObject, name), true)

// For use with game object templates' OverrideableProperty type
#define PROP_TPL(name) AddProperty<decltype(TObject::name)::Type>(propertyMap, #name, offsetof(TObject, name))

	void InitPropertyMaps()
	{
		{
			BEGIN_PROPERTIES(gStatusPropertyMap, esv::Status);
			PROP_RO(NetID);
			PROP_RO(StatusId);
			PROP_RO(CanEnterChance);
			PROP_RO(StartTimer);
			PROP_RO(LifeTime);
			PROP_RO(CurrentLifeTime);
			PROP_RO(TurnTimer);
			PROP_RO(Strength);
			PROP_RO(StatsMultiplier);
			PROP_ENUM(DamageSourceType);
			PROP_RO(StatusHandle);
			PROP_RO(TargetHandle);
			PROP_RO(StatusSourceHandle);
			PROP_RO(SomeHandle);
			PROP_FLAGS(Flags0, esv::StatusFlags0, false);
			PROP_FLAGS(Flags1, esv::StatusFlags1, false);
			PROP_FLAGS(Flags2, esv::StatusFlags2, false);

			propertyMap.Flags[GFS.strForceStatus].Flags |= kPropWrite;
			propertyMap.Flags[GFS.strForceFailStatus].Flags |= kPropWrite;

			propertyMap.Properties[GFS.strLifeTime].SetFloat = [](void * st, float value) -> bool {
				auto status = reinterpret_cast<esv::Status *>(st);
				if (value < 0.0f) return false;
				status->LifeTime = value;
				if (status->CurrentLifeTime > status->LifeTime) {
					status->CurrentLifeTime = status->LifeTime;
				}

				return true;
			};

			propertyMap.Properties[GFS.strCurrentLifeTime].SetFloat = [](void * st, float value) -> bool {
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
			BEGIN_PROPERTIES(gStatusHitPropertyMap, esv::StatusHit);
			propertyMap.Parent = &gStatusPropertyMap;
			PROP_RO(HitByHandle);
			PROP_RO(HitWithHandle);
			PROP_RO(WeaponHandle);
			PROP(HitReason);
			PROP(SkillId);
			PROP(Interruption);
			PROP(AllowInterruptAction);
			PROP(ForceInterrupt);
			PROP_RO(DecDelayDeathCount);
			PROP(ImpactPosition);
			PROP(ImpactOrigin);
			PROP(ImpactDirection);
		}

		{
			BEGIN_PROPERTIES(gStatusConsumePropertyMap, esv::StatusConsume);
			propertyMap.Parent = &gStatusPropertyMap;
			// TODO - Skills, Items, ResetCooldownsSet, StatsIDs?
			PROP(ResetAllCooldowns);
			PROP(ResetOncePerCombat);
			PROP(ScaleWithVitality);
			PROP(LoseControl);
			PROP(ApplyStatusOnTick);
			PROP(EffectTime);
			PROP(StatsId);
			PROP(StackId);
			PROP(OriginalWeaponStatsId);
			PROP(OverrideWeaponStatsId);
			PROP(OverrideWeaponHandle);
			PROP(SavingThrow);
			PROP(SourceDirection);
			PROP(Turn);
			PROP_ENUM(HealEffectOverride);
			PROP(Poisoned);
		}

		{
			BEGIN_PROPERTIES(gStatusHealingPropertyMap, esv::StatusHeal);
			propertyMap.Parent = &gStatusConsumePropertyMap;
			PROP(HealAmount);
			PROP(TimeElapsed);
			PROP_ENUM(HealEffect);
			PROP(HealEffectId);
			PROP(SkipInitialEffect);
			PROP(HealingEvent);
			PROP_ENUM(HealStat);
			PROP(AbsorbSurfaceRange);
		}

		{
			BEGIN_PROPERTIES(gStatusHealPropertyMap, esv::StatusHealing);
			propertyMap.Parent = &gStatusPropertyMap;
			PROP(EffectTime);
			PROP(HealAmount);
			PROP_ENUM(HealEffect);
			PROP(HealEffectId);
			PROP_ENUM(HealType);
			PROP(AbsorbSurfaceRange);
			PROP(TargetDependentHeal);
		}

		{
			BEGIN_PROPERTIES(gHitDamageInfoPropertyMap, HitDamageInfo);
			PROP(Equipment);
			PROP_RO(TotalDamage);
			PROP_RO(DamageDealt);
			PROP_ENUM(DeathType);
			PROP_ENUM(DamageType);
			PROP(AttackDirection);
			PROP(ArmorAbsorption);
			PROP(LifeSteal);
			PROP_FLAGS(EffectFlags, HitFlag, true);
			PROP(HitWithWeapon);
		}

		{
			BEGIN_PROPERTIES(gDamageHelpersPropertyMap, esv::DamageHelpers);
			PROP(SimulateHit);
			PROP_ENUM(HitType);
			PROP(NoHitRoll);
			PROP(ProcWindWalker);
			PROP(ForceReduceDurability);
			PROP_ENUM(HighGround);
			PROP_ENUM(CriticalRoll);
			PROP(HitReason);
			PROP_ENUM(DamageSourceType);
			PROP(Strength);
		}

		{
			BEGIN_PROPERTIES(gShootProjectileHelperPropertyMap, esv::ShootProjectileHelper);
			PROP(SkillId);
			PROP(Caster);
			PROP(Source);
			PROP(Target);
			PROP(StartPosition);
			PROP(EndPosition);
			PROP(Random);
			PROP(CasterLevel);
			PROP(IsTrap);
			PROP(UnknownFlag1);
			PROP(CleanseStatuses);
			PROP(StatusClearChance);
			PROP(IsFromItem);
			PROP(IsStealthed);
			PROP(IgnoreObjects);
		}

		{
			BEGIN_PROPERTIES(gEoCItemDefinitionPropertyMap, eoc::ItemDefinition);
			PROP_GUID(RootTemplate, true);
			PROP_GUID(OriginalRootTemplate, true);
			PROP(Amount);
			PROP(Slot);
			PROP(GoldValueOverwrite);
			PROP(WeightValueOverwrite);
			PROP_ENUM(DamageTypeOverwrite);
			PROP(ItemType);
			PROP(CustomDisplayName);
			PROP(CustomDescription);
			PROP(CustomBookContent);
			PROP(GenerationStatsId);
			PROP(GenerationItemType);
			PROP(GenerationRandom);
			PROP(GenerationLevel);
			PROP(StatsLevel);
			PROP(Key);
			PROP(LockLevel);
			PROP(StatsEntryName);
			PROP(HasModifiedSkills);
			PROP_DYN_FS(Skills);
			PROP(HasGeneratedStats);
			PROP(IsIdentified);
			PROP(GMFolding);
			PROP(CanUseRemotely);
		}

		{
			BEGIN_PROPERTIES(gEquipmentAttributesPropertyMap, CDivinityStats_Equipment_Attributes);

			PROP(Durability);
			PROP(DurabilityDegradeSpeed);
			PROP(StrengthBoost);
			PROP(FinesseBoost);
			PROP(IntelligenceBoost);
			PROP(ConstitutionBoost);
			PROP(MemoryBoost);
			PROP(WitsBoost);
			PROP(SightBoost);
			PROP(HearingBoost);
			PROP(VitalityBoost);
			PROP(SourcePointsBoost);
			PROP(MaxAP);
			PROP(StartAP);
			PROP(APRecovery);
			PROP(AccuracyBoost);
			PROP(DodgeBoost);
			PROP(LifeSteal);
			PROP(CriticalChance);
			PROP(ChanceToHitBoost);
			PROP(MovementSpeedBoost);
			PROP(RuneSlots);
			PROP(RuneSlots_V1);
			PROP(FireResistance);
			PROP(AirResistance);
			PROP(WaterResistance);
			PROP(EarthResistance);
			PROP(PoisonResistance);
			PROP(ShadowResistance);
			PROP(PiercingResistance);
			PROP(CorrosiveResistance);
			PROP(PhysicalResistance);
			PROP(MagicResistance);
			PROP(CustomResistance);
			PROP(Movement);
			PROP(Initiative);
			PROP(Willpower);
			PROP(Bodybuilding);
			PROP(MaxSummons);
			PROP(Value);
			PROP(Weight);
			// TODO - Reflection
			PROP_DYN_FS(Skills);
			PROP(ItemColor);
			PROP(ModifierType);
			PROP(ObjectInstanceName);
			PROP(BoostName);
			PROP_ENUM(StatsType);
			// TODO - add attribute flags object support
			// PROP_FLAGS(AttributeFlags, StatAttributeFlags, true);
			// TODO - AbilityModifiers, Talents
		}

		{
			BEGIN_PROPERTIES(gEquipmentAttributesWeaponPropertyMap, CDivinityStats_Equipment_Attributes_Weapon);
			propertyMap.Parent = &gEquipmentAttributesPropertyMap;
			PROP_ENUM(DamageType);
			PROP(MinDamage);
			PROP(MaxDamage);
			PROP(DamageBoost);
			PROP(DamageFromBase);
			PROP(CriticalDamage);
			PROP(WeaponRange);
			PROP(CleaveAngle);
			PROP(CleavePercentage);
			PROP(AttackAPCost);
			PROP(Projectile);
		}

		{
			BEGIN_PROPERTIES(gEquipmentAttributesArmorPropertyMap, CDivinityStats_Equipment_Attributes_Armor);
			propertyMap.Parent = &gEquipmentAttributesPropertyMap;
			PROP(ArmorValue);
			PROP(ArmorBoost);
			PROP(MagicArmorValue);
			PROP(MagicArmorBoost);
		}

		{
			BEGIN_PROPERTIES(gEquipmentAttributesShieldPropertyMap, CDivinityStats_Equipment_Attributes_Shield);
			propertyMap.Parent = &gEquipmentAttributesPropertyMap;
			PROP(ArmorValue);
			PROP(ArmorBoost);
			PROP(MagicArmorValue);
			PROP(MagicArmorBoost);
			PROP(Blocking);
		}

		{
			BEGIN_PROPERTIES(gCharacterDynamicStatPropertyMap, CharacterDynamicStat);
			PROP(SummonLifelinkModifier);
			PROP(Strength);
			PROP(Memory);
			PROP(Intelligence);
			PROP(Movement);
			PROP(MovementSpeedBoost);
			PROP(Finesse);
			PROP(Wits);
			PROP(Constitution);

			PROP(FireResistance);
			PROP(EarthResistance);
			PROP(WaterResistance);
			PROP(AirResistance);
			PROP(PoisonResistance);
			PROP(ShadowResistance);

			PROP(Willpower);
			PROP(Bodybuilding);
			PROP(PiercingResistance);
			PROP(PhysicalResistance);
			PROP(CorrosiveResistance);
			PROP(MagicResistance);
			PROP(CustomResistance);

			PROP(Sight);
			PROP(Hearing);
			PROP(FOV);
			PROP(APMaximum);
			PROP(APStart);
			PROP(APRecovery);
			PROP(CriticalChance);
			PROP(Initiative);
			PROP(Vitality);
			PROP(VitalityBoost);
			PROP(MagicPoints);
			PROP(Level);
			PROP(Gain);

			PROP(Armor);
			PROP(MagicArmor);
			PROP(ArmorBoost);
			PROP(MagicArmorBoost);
			PROP(ArmorBoostGrowthPerLevel);
			PROP(MagicArmorBoostGrowthPerLevel);
			PROP(DamageBoost);
			PROP(DamageBoostGrowthPerLevel);

			PROP(Accuracy);
			PROP(Dodge);
			PROP(MaxResistance);
			PROP(LifeSteal);
			PROP(Weight);
			PROP(ChanceToHitBoost);
			PROP(RangeBoost);
			PROP(APCostBoost);
			PROP(SPCostBoost);
			PROP(MaxSummons);
			PROP(BonusWeaponDamageMultiplier);
			PROP(TranslationKey);
			PROP(BonusWeapon);
			PROP(StepsType);
			// TODO Abilities, Talents, RemovedTalents, Traits
		}

		{
			BEGIN_PROPERTIES(gCharacterStatsPropertyMap, CDivinityStats_Character);
			// CRPGStats_Object
			PROP_RO(Level);
			PROP_RO(Name);
			PROP_RO(AIFlags);
			PROP_RO(InstanceId);

			// CDivinityStats_Character
			PROP(CurrentVitality);
			PROP(CurrentArmor);
			PROP(CurrentMagicArmor);
			PROP_RO(ArmorAfterHitCooldownMultiplier);
			PROP_RO(MagicArmorAfterHitCooldownMultiplier);
			PROP_RO(MPStart);
			PROP_RO(CurrentAP);
			PROP_RO(BonusActionPoints);
			PROP_RO(Experience);
			PROP_RO(Reputation);
			PROP_RO(Flanked);
			PROP_RO(Karma);
			PROP_FLAGS(Flags, StatCharacterFlags, false);
			PROP_RO(MaxResistance);
			PROP_RO(HasTwoHandedWeapon);
			PROP_RO(IsIncapacitatedRefCount);
			PROP_RO(MaxVitality);
			PROP_RO(BaseMaxVitality);
			PROP_RO(MaxArmor);
			PROP_RO(BaseMaxArmor);
			PROP_RO(MaxMagicArmor);
			PROP_RO(BaseMaxMagicArmor);
			PROP_RO(Sight);
			PROP_RO(BaseSight);
			PROP_RO(MaxSummons);
			PROP_RO(BaseMaxSummons);
			PROP_RO(MaxMpOverride);
			PROP_FLAGS(AttributeFlags, StatAttributeFlags, false);

			// TODO - DisabledTalents, TraitOrder?
		}

		{
			BEGIN_PROPERTIES(gItemStatsPropertyMap, CDivinityStats_Item);
			// CRPGStats_Object
			PROP_RO(Level);
			PROP_RO(Name);
			PROP_RO(InstanceId);

			// CDivinityStats_Item
			PROP_ENUM(ItemType);
			// ItemSlot is stored on 4 bytes, but the enum is only 1-byte, so we need to remap it
			AddPropertyEnum<ItemSlot>(propertyMap, "ItemSlot", offsetof(CDivinityStats_Item, ItemSlot));
			PROP_ENUM(WeaponType);
			PROP_RO(AnimType);
			PROP_RO(WeaponRange);
			PROP_RO(IsIdentified);
			PROP_RO(IsTwoHanded);
			PROP_RO(ShouldSyncStats);
			PROP_RO(HasModifiedSkills);
			PROP_RO(Skills);
			PROP_ENUM(DamageTypeOverwrite);
			PROP_RO(Durability);
			PROP_RO(DurabilityCounter);
			PROP_RO(ItemTypeReal);
			PROP_FLAGS(AttributeFlags, StatAttributeFlags, false);
			PROP_RO(MaxCharges);
			PROP_RO(Charges);
		}

		{
			BEGIN_PROPERTIES(gPlayerCustomDataPropertyMap, eoc::PlayerCustomData);
			PROP_RO(CustomLookEnabled);
			PROP(Name);
			PROP(ClassType);
			PROP(SkinColor);
			PROP(HairColor);
			PROP(ClothColor1);
			PROP(ClothColor2);
			PROP(ClothColor3);
			PROP(IsMale);
			PROP(Race);
			PROP(OriginName);
			PROP(Icon);
			PROP(MusicInstrument);
			PROP_RO(OwnerProfileID);
			PROP_RO(ReservedProfileID);
			PROP(AiPersonality);
			PROP_RO(Speaker);
		}

		{
			BEGIN_PROPERTIES(gCharacterPropertyMap, esv::Character);
			// EoCServerObject
			PROP_RO(NetID);
			PROP_RO(MyGuid);
			// Character
			PROP_RO(WorldPos);
			PROP_RO(CurrentLevel);
			PROP_FLAGS(Flags, esv::CharacterFlags, false);
			PROP_FLAGS(Flags2, esv::CharacterFlags2, false);
			PROP_FLAGS(Flags3, esv::CharacterFlags3, false);
			PROP_RO(Scale);
			PROP_RO(AnimationOverride);
			PROP_RO(WalkSpeedOverride);
			PROP_RO(RunSpeedOverride);
			PROP_RO(NeedsUpdateCount);
			PROP_RO(ScriptForceUpdateCount);
			PROP_RO(ForceSynchCount);
			PROP_RO(InventoryHandle);
			PROP_RO(SkillBeingPrepared);
			PROP_RO(LifeTime);
			PROP_RO(TurnTimer);
			PROP_RO(TriggerTrapsTimer);
			PROP_TY(UserID, int32_t);
			PROP_RO(OwnerHandle);
			PROP_RO(FollowCharacterHandle);
			PROP_RO(SpiritCharacterHandle);
			PROP_RO(CorpseCharacterHandle);
			PROP_RO(PartialAP);
			PROP_RO(AnimType);
			PROP_RO(DelayDeathCount);
			PROP_RO(AnimationSetOverride);
			PROP_RO(OriginalTransformDisplayName);
			PROP_RO(PartyHandle);
			PROP_RO(CustomTradeTreasure);
			PROP_RO(IsAlarmed);
			PROP_RO(CrimeWarningsEnabled);
			PROP_RO(CrimeInterrogationEnabled);
			PROP_RO(MovingCasterHandle);
			PROP_RO(Archetype);
			PROP_RO(EquipmentColor);
			PROP_RO(ProjectileTemplate);
			PROP_RO(ReadyCheckBlocked);
			PROP_RO(CorpseLootable);
			PROP_ENUM(CustomBloodSurface);
			PROP_RO(PreviousLevel);
		}

		{
			BEGIN_PROPERTIES(gItemPropertyMap, esv::Item);
			// EoCServerObject
			PROP_RO(NetID);
			PROP_RO(MyGuid);
			// Item
			PROP_RO(WorldPos);
			PROP_RO(CurrentLevel);
			PROP_RO(Scale);
			PROP_RO(CustomDisplayName);
			PROP_RO(CustomDescription);
			PROP_RO(CustomBookContent);
			PROP_RO(StatsId);
			PROP_RO(InventoryHandle);
			PROP_RO(ParentInventoryHandle);
			PROP_RO(Slot);
			PROP_RO(Amount);
			PROP_RO(Vitality);
			PROP_RO(Armor);
			PROP_RO(InUseByCharacterHandle);
			PROP_RO(Key);
			PROP_RO(LockLevel);
			PROP_RO(OwnerHandle);
			PROP_RO(ComputedVitality);
			PROP_RO(ItemType);
			PROP_RO(GoldValueOverwrite);
			PROP_RO(WeightValueOverwrite);
			PROP_RO(TreasureLevel);
			PROP_RO(LevelOverride);
			PROP_RO(ForceSynch);
		}

		{
			BEGIN_PROPERTIES(gProjectilePropertyMap, esv::Projectile);
			// EoCServerObject
			PROP_RO(NetID);
			PROP_RO(MyGuid);
			// Projectile
			PROP_RO(CasterHandle);
			PROP_RO(SourceHandle);
			PROP_RO(TargetObjectHandle);
			PROP_RO(HitObjectHandle);
			PROP_RO(SourcePosition);
			PROP_RO(TargetPosition);
			PROP_ENUM(DamageType);
			PROP_ENUM(DamageSourceType);
			PROP_RO(LifeTime);
			PROP_RO(HitInterpolation);
			PROP_RO(ExplodeRadius0);
			PROP_RO(ExplodeRadius1);
			PROP_ENUM(DeathType);
			PROP_RO(SkillId);
			PROP_RO(WeaponHandle);
			PROP_RO(MovingEffectHandle);
			PROP_RO(SpawnEffect);
			PROP_RO(SpawnFXOverridesImpactFX);
			PROP_RO(EffectHandle);
			PROP_RO(RequestDelete);
			PROP_RO(Launched);
			PROP_RO(IsTrap);
			PROP_RO(UseCharacterStats);
			PROP_RO(ReduceDurability);
			PROP_RO(AlwaysDamage);
			PROP_RO(ForceTarget);
			PROP_RO(IsFromItem);
			PROP_RO(DivideDamage);
			PROP_RO(IgnoreRoof);
			PROP_RO(CanDeflect);
			PROP_RO(IgnoreObjects);
			PROP_RO(CleanseStatuses);
			PROP_RO(StatusClearChance);
			PROP_RO(Position);
			PROP_RO(PrevPosition);
			PROP_RO(Velocity);
			PROP_RO(Scale);
			PROP_RO(CurrentLevel);
		}

		{
			BEGIN_PROPERTIES(gEsvSurfacePropertyMap, esv::Surface);
			PROP_RO(NetID);
			PROP_RO(MyHandle);
			PROP_ENUM(SurfaceType);
			PROP_RO(Flags);
			PROP_RO(TeamId);
			PROP_RO(OwnerHandle);
			PROP_RO(LifeTime);
			PROP_RO(LifeTimeFromTemplate);
			PROP_RO(StatusChance);
			PROP_RO(Index);
			PROP_RO(NeedsSplitEvaluation);
			PROP_RO(OwnershipTimer);
		}

		{
			BEGIN_PROPERTIES(gASAttackPropertyMap, esv::ASAttack);
			PROP_RO(TargetHandle);
			PROP_RO(TargetPosition);
			PROP_RO(IsFinished);
			PROP_RO(AlwaysHit);
			PROP_RO(TimeRemaining);
			PROP_RO(AnimationFinished);
			PROP_RO(TotalHits);
			PROP_RO(TotalHitOffHand);
			PROP_RO(TotalShoots);
			PROP_RO(TotalShootsOffHand);
			PROP_RO(HitCount);
			PROP_RO(HitCountOffHand);
			PROP_RO(ShootCount);
			PROP_RO(ShootCountOffHand);
			PROP_RO(MainWeaponHandle);
			PROP_RO(OffWeaponHandle);
			PROP_RO(MainHandHitType);
			PROP_RO(OffHandHitType);
			PROP_RO(ProjectileUsesHitObject);
			PROP_RO(ProjectileStartPosition);
			PROP_RO(ProjectileTargetPosition);
			PROP_RO(DamageDurability);
		}

		{
			BEGIN_PROPERTIES(gASPrepareSkillPropertyMap, esv::ASPrepareSkill);
			PROP_RO(SkillId);
			PROP_RO(PrepareAnimationInit);
			PROP_RO(PrepareAnimationLoop);
			PROP_RO(IsFinished);
			PROP_RO(IsEntered);
		}

		{
			BEGIN_PROPERTIES(gSkillStatePropertyMap, esv::SkillState);
			PROP_RO(SkillId);
			PROP_RO(CharacterHandle);
			PROP_RO(SourceItemHandle);
			PROP_RO(CanEnter);
			PROP_RO(IsFinished);
			PROP_RO(IgnoreChecks);
			PROP_RO(IsStealthed);
			PROP_RO(PrepareTimerRemaining);
			PROP_RO(ShouldExit);
			PROP_RO(CleanseStatuses);
			PROP_RO(StatusClearChance);
			PROP_RO(CharacterHasSkill);
		}

		{
			BEGIN_PROPERTIES(gEclStatusPropertyMap, ecl::Status);
			PROP_RO(NetID);
			PROP_RO(OwnerHandle);
			PROP_RO(StatusId);
			PROP_RO(LifeTime);
			PROP_RO(CurrentLifeTime);
			PROP_RO(StatsMultiplier);
			PROP_FLAGS(Flags, ecl::StatusFlags, false);
			PROP_RO(StatusSourceHandle);
		}

		{
			BEGIN_PROPERTIES(gEclCharacterPropertyMap, ecl::Character);
			// EoCClientObject
			PROP_RO(NetID);
			PROP_RO(MyGuid);
			// Character
			PROP_RO(WorldPos);
			PROP_RO(CurrentLevel);
			PROP_RO(Scale);
			PROP_TY(UserID, int32_t);
			PROP_RO(OwnerCharacterHandle);
			PROP_RO(CorpseCharacterHandle);
			PROP_RO(FollowCharacterHandle);
			PROP_RO(OwnerCharacterHandle);
			PROP_RO(StoryDisplayName);
			PROP_RO(OriginalDisplayName);
			PROP_RO(AnimationSetOverride);
			PROP_RO(WalkSpeedOverride);
			PROP_RO(RunSpeedOverride);
			PROP_RO(Archetype);
			PROP_RO(CorpseLootable);
		}

		{
			BEGIN_PROPERTIES(gEclItemPropertyMap, ecl::Item);
			// EoCClientObject
			PROP_RO(NetID);
			PROP_RO(MyGuid);
			// Item
			PROP_RO(WorldPos);
			PROP_RO(CurrentLevel);
			PROP_RO(Scale);
			PROP_RO(StatsId);
			PROP_RO(Weight);
			PROP_RO(KeyName);
			PROP_RO(Level);
			PROP_RO(ItemType);
			PROP_RO(GoldValueOverride);
			PROP_RO(BaseWeightOverwrite);
			PROP_RO(ItemColorOverride);
		}

		{
			BEGIN_PROPERTIES(gGameObjectTemplatePropertyMap, GameObjectTemplate);
			// TODO Tags
			PROP_RO(Id);
			PROP(Name);
			PROP_RO(TemplateName);
			PROP_RO(IsGlobal);
			PROP_RO(IsDeleted);
			PROP_RO(LevelName);
			PROP_RO(ModFolder);
			PROP_RO(GroupID);
			PROP_TPL(VisualTemplate);
			PROP_TPL(PhysicsTemplate);
			PROP_TPL(CastShadow);
			PROP_TPL(ReceiveDecal);
			PROP_TPL(AllowReceiveDecalWhenAnimated);
			PROP_TPL(IsReflecting);
			PROP_TPL(IsShadowProxy);
			PROP_TPL(RenderChannel);
			PROP_TPL(CameraOffset);
			PROP_RO(HasParentModRelation);
			PROP_TPL(HasGameplayValue);
			// TODO FileName
		}

		{
			BEGIN_PROPERTIES(gEoCGameObjectTemplatePropertyMap, EoCGameObjectTemplate);
			PROP_TPL(AIBoundsRadius);
			PROP_TPL(AIBoundsHeight);
			PROP_TPL(DisplayName);
			PROP_TPL(Opacity);
			PROP_TPL(Fadeable);
			PROP_TPL(FadeIn);
			PROP_TPL(SeeThrough);
			PROP_TPL(FadeGroup);
			PROP(GameMasterSpawnSection);
			PROP_TPL(GameMasterSpawnSubSection);
		}

		{
			BEGIN_PROPERTIES(gCharacterTemplatePropertyMap, CharacterTemplate);
			PROP_TPL(Icon);
			PROP_TPL(Stats);
			PROP_TPL(SkillSet);
			PROP_TPL(Equipment);
			// TODO - Treasures
			PROP_TPL(LightID);
			PROP_TPL(HitFX);
			PROP_TPL(DefaultDialog);
			PROP_TPL(SpeakerGroup);
			PROP_TPL(GeneratePortrait);
			PROP_TPL(WalkSpeed);
			PROP_TPL(RunSpeed);
			PROP_TPL(ClimbAttachSpeed);
			PROP_TPL(ClimbLoopSpeed);
			PROP_TPL(ClimbDetachSpeed);
			PROP_TPL(CanShootThrough);
			PROP_TPL(WalkThrough);
			PROP_TPL(CanClimbLadders);
			PROP_TPL(IsPlayer);
			PROP_TPL(Floating);
			PROP_TPL(SpotSneakers);
			PROP_TPL(CanOpenDoors);
			PROP_TPL(AvoidTraps);
			PROP_TPL(InfluenceTreasureLevel);
			PROP_TPL(HardcoreOnly);
			PROP_TPL(NotHardcore);
			PROP_TPL(JumpUpLadders);
			PROP_TPL(NoRotate);
			PROP_TPL(IsHuge);
			PROP_TPL(EquipmentClass);
			// TODO - OnDeathActions
			// TODO - PROP_TPL(BloodSurfaceType);
			PROP_TPL(ExplodedResourceID);
			PROP_TPL(ExplosionFX);
			// TODO - Scripts, SkillList, ItemList
			PROP_TPL(VisualSetResourceID);
			PROP_TPL(VisualSetIndices);
			PROP_TPL(TrophyID);
			PROP_TPL(SoundInitEvent);
			PROP_TPL(SoundAttachBone);
			PROP_TPL(SoundAttenuation);
			PROP_TPL(CoverAmount);
			PROP_TPL(LevelOverride);
			PROP_TPL(ForceUnsheathSkills);
			PROP_TPL(CanBeTeleported);
			PROP_TPL(ActivationGroupId);
			PROP_TPL(SoftBodyCollisionTemplate);
			PROP_TPL(RagdollTemplate);
			PROP_TPL(DefaultState);
			PROP_TPL(GhostTemplate);
			PROP_TPL(IsLootable);
			PROP_TPL(IsEquipmentLootable);
			PROP_TPL(InventoryType);
			PROP_TPL(IsArenaChampion);
			PROP_TPL(FootstepWeight);
		}

		{
			BEGIN_PROPERTIES(gItemTemplatePropertyMap, ItemTemplate);
			PROP_TPL(Icon);
			PROP_TPL(CanBePickedUp);
			PROP_TPL(CanBeMoved);
			PROP_TPL(CoverAmount);
			PROP_TPL(CanShootThrough);
			PROP_TPL(CanClickThrough);
			PROP_TPL(Destroyed);
			PROP_TPL(WalkThrough);
			PROP_TPL(WalkOn);
			PROP_TPL(Wadable);
			PROP_TPL(IsInteractionDisabled);
			PROP_TPL(IsPinnedContainer);
			PROP_TPL(StoryItem);
			PROP_TPL(FreezeGravity);
			PROP_TPL(IsKey);
			PROP_TPL(IsTrap);
			PROP_TPL(IsSurfaceBlocker);
			PROP_TPL(IsSurfaceCloudBlocker);
			PROP_TPL(TreasureOnDestroy);
			PROP_TPL(IsHuge);
			PROP_TPL(HardcoreOnly);
			PROP_TPL(NotHardcore);
			PROP_TPL(UsePartyLevelForTreasureLevel);
			PROP_TPL(Unimportant);
			PROP_TPL(Hostile);
			PROP_TPL(UseOnDistance);
			PROP_TPL(UseRemotely);
			PROP_TPL(IsBlocker);
			PROP_TPL(IsPointerBlocker);
			PROP_TPL(UnknownDisplayName);
			PROP_TPL(Tooltip);
			PROP_TPL(Stats);
			PROP_TPL(OnUseDescription);
			PROP_TPL(DefaultState);
			PROP_TPL(Owner);
			PROP_TPL(Key);
			PROP_TPL(HitFX);
			PROP_TPL(LockLevel);
			PROP_TPL(Amount);
			PROP_TPL(MaxStackAmount);
			PROP_TPL(TreasureLevel);
			PROP_TPL(DropSound);
			PROP_TPL(PickupSound);
			PROP_TPL(UseSound);
			PROP_TPL(EquipSound);
			PROP_TPL(UnequipSound);
			PROP_TPL(InventoryMoveSound);
			PROP_TPL(LoopSound);
			PROP_TPL(SoundInitEvent);
			PROP_TPL(SoundAttachBone);
			PROP_TPL(SoundAttenuation);
			PROP_TPL(BloodSurfaceType);
			PROP_TPL(Description);
			PROP_TPL(UnknownDescription);
			PROP_TPL(Speaker);
			PROP_TPL(AltSpeaker);
			PROP_TPL(SpeakerGroup);
			PROP_TPL(ActivationGroupId);
			PROP_TPL(Race);
			PROP_TPL(IsWall);
			PROP_TPL(LevelOverride);
			PROP_TPL(Floating);
			PROP_TPL(IsSourceContainer);
			PROP_TPL(MeshProxy);
			PROP_TPL(IsPublicDomain);
			PROP_TPL(AllowSummonTeleport);
		}

		{
			BEGIN_PROPERTIES(gProjectileTemplatePropertyMap, ProjectileTemplate);
			PROP(LifeTime);
			PROP(Speed);
			PROP(Acceleration);
			PROP(CastBone);
			PROP(ImpactFX);
			PROP(TrailFX);
			PROP(DestroyTrailFXOnImpact);
			PROP(BeamFX);
			PROP(PreviewPathMaterial);
			PROP(PreviewPathImpactFX);
			PROP(PreviewPathRadius);
			PROP(ImpactFXSize);
			PROP(RotateImpact);
			PROP(IgnoreRoof);
			PROP(DetachBeam);
			PROP(NeedsArrowImpactSFX);
			PROP(ProjectilePath);
			PROP(PathShift);
			PROP(PathRadius);
			PROP(PathMinArcDist);
			PROP(PathMaxArcDist);
			PROP(PathRepeat);
		}

		{
			BEGIN_PROPERTIES(gCombatComponentTemplatePropertyMap, CombatComponentTemplate);
			PROP_TPL(Alignment);
			PROP_TPL(CanFight);
			PROP_TPL(CanJoinCombat);
			PROP_TPL(CombatGroupID);
			PROP_TPL(IsBoss);
			PROP_TPL(IsInspector);
			PROP_TPL(StartCombatRange);
		}
	}


	bool OsirisPropertyMapGetRaw(PropertyMapBase const & propertyMap, void * obj,
		OsiArgumentDesc & args, uint32_t firstArg, OsiPropertyMapType type, bool throwError)
	{
		auto propertyNameStr = args[firstArg].String;
		auto propertyName = ToFixedString(propertyNameStr);

		if (obj == nullptr) {
			OsiError("Attempted to get property '" << propertyNameStr << "' of null object!");
			return false;
		}

		if (!propertyName) {
			OsiError("Failed to get property '" << propertyNameStr << "' of [" << propertyMap.Name << "]: Property does not exist!");
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

			auto gameObject = esv::GetEntityWorld()->GetGameObject(*val);
			if (gameObject != nullptr) {
				args[firstArg + 1].Set(gameObject->GetGuid()->Str);
				return true;
			}

			OsiError("Failed to get property '" << propertyNameStr << "': Could not map handle to game object: " << (int64_t)*val);
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
			OsiError("Failed to get property '" << propertyNameStr << "': Unknown Osi property type!");
			return false;
		}
	}


	bool OsirisPropertyMapSetRaw(PropertyMapBase const & propertyMap, void * obj,
		OsiArgumentDesc const & args, uint32_t firstArg, OsiPropertyMapType type, bool throwError)
	{
		auto propertyNameStr = args[firstArg].String;
		auto propertyName = ToFixedString(propertyNameStr);

		if (obj == nullptr) {
			OsiError("Attempted to set property '" << propertyNameStr << "' of null object!");
			return false;
		}

		if (!propertyName) {
			OsiError("Failed to set property '" << propertyNameStr << "' of [" << propertyMap.Name << "]: Property does not exist!");
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

			auto gameObject = esv::GetEntityWorld()->GetGameObject(guid);
			if (gameObject != nullptr) {
				ObjectHandle handle;
				gameObject->GetObjectHandle(handle);
				return propertyMap.setHandle(obj, propertyName, handle, false, throwError);
			}

			OsiError("Failed to set property '" << propertyNameStr << "': Could not map GUID to game object: " << guid);
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
			OsiError("Failed to set property '" << propertyNameStr << "': Unknown Osi property type!");
			return false;
		}
	}


	bool LuaPropertyMapGet(lua_State* L, PropertyMapBase const& propertyMap, void* obj,
		char const* propertyName, bool throwError)
	{
		auto propertyFS = ToFixedString(propertyName);
		if (!propertyFS) {
			OsiError("Failed to get property '" << propertyName << "' of [" << propertyMap.Name << "]: Property does not exist!");
			return false;
		}

		return LuaPropertyMapGet(L, propertyMap, obj, propertyFS, throwError);
	}

	bool LuaPropertyMapGet(lua_State * L, PropertyMapBase const & propertyMap, void * obj,
		FixedString const& propertyName, bool throwError)
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
					OsiError("Failed to get property '" << propertyName << "' of [" << propertyMap.Name << "]: Property does not exist!");
				}
				return {};
			} else {
				auto val = propertyMap.getFlag(obj, propertyName, false, throwError);
				if (val) {
					lua::push(L, *val);
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
				lua::push(L, *val != 0);
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
				lua::push(L, *val);
				return true;
			} else {
				return false;
			}
		}

		case PropertyType::kFloat:
		{
			auto val = propertyMap.getFloat(obj, propertyName, false, throwError);
			if (val) {
				lua::push(L, *val);
				return true;
			} else {
				return false;
			}
		}

		case PropertyType::kFixedString:
		case PropertyType::kDynamicFixedString:
		case PropertyType::kFixedStringGuid:
		case PropertyType::kStringPtr:
		case PropertyType::kStdString:
		case PropertyType::kStdWString:
		case PropertyType::kTranslatedString:
		{
			auto val = propertyMap.getString(obj, propertyName, false, throwError);
			if (val) {
				lua::push(L, *val);
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
					lua::push(L, val->Handle);
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

		auto propertyFS = ToFixedString(propertyName);
		if (!propertyFS) {
			OsiError("Failed to set property '" << propertyName << "' of [" << propertyMap.Name << "]: Property does not exist!");
			return false;
		}

		auto prop = propertyMap.findProperty(propertyFS);
		if (prop == nullptr) {
			auto flag = propertyMap.findFlag(propertyFS);
			if (flag == nullptr) {
				if (throwError) {
					OsiError("Failed to set property '" << propertyName << "' of [" << propertyMap.Name << "]: Property does not exist!");
				}
				return {};
			} else {
				luaL_checktype(L, index, LUA_TBOOLEAN);
				auto val = lua_toboolean(L, index);
				return propertyMap.setFlag(obj, propertyFS, val == 1, false, throwError);
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
			return propertyMap.setFlag(obj, propertyFS, val == 1, false, throwError);
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
			return propertyMap.setInt(obj, propertyFS, val, false, throwError);
		}

		case PropertyType::kFloat:
		{
			auto val = luaL_checknumber(L, index);
			return propertyMap.setFloat(obj, propertyFS, (float)val, false, throwError);
		}

		case PropertyType::kFixedString:
		case PropertyType::kDynamicFixedString:
		case PropertyType::kFixedStringGuid:
		case PropertyType::kStringPtr:
		case PropertyType::kStdString:
		case PropertyType::kStdWString:
		case PropertyType::kTranslatedString:
		{
			auto val = luaL_checkstring(L, index);
			return propertyMap.setString(obj, propertyFS, val, false, throwError);
		}

		case PropertyType::kObjectHandle:
		{
			auto val = luaL_checkinteger(L, index);
			return propertyMap.setHandle(obj, propertyFS, ObjectHandle(val), false, throwError);
		}

		default:
			OsiError("Failed to set property '" << propertyName << "': Unsupported property type for Lua fetch!");
			return false;
		}
	}
}