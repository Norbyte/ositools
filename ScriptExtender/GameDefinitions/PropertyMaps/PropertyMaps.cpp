#include <stdafx.h>
#include <Extender/ScriptExtender.h>
#include <GameDefinitions/Components/Projectile.h>
#include <GameDefinitions/GameObjects/RootTemplates.h>
#include <GameDefinitions/GameObjects/Surface.h>
#include <GameDefinitions/GameObjects/Skill.h>
#include <GameDefinitions/Components/Trigger.h>
#include <GameDefinitions/PropertyMaps/PropertyMaps.h>

namespace dse
{
	LegacyPropertyMap<esv::Status, void> gStatusPropertyMap;
	LegacyPropertyMap<esv::StatusConsumeBase, esv::Status> gStatusConsumeBasePropertyMap;
	LegacyPropertyMap<esv::StatusHit, esv::Status> gStatusHitPropertyMap;
	LegacyPropertyMap<esv::StatusDying, esv::Status> gStatusDyingPropertyMap;
	LegacyPropertyMap<esv::StatusHeal, esv::Status> gStatusHealPropertyMap;
	LegacyPropertyMap<esv::StatusCharmed, esv::Status> gStatusCharmedPropertyMap;
	LegacyPropertyMap<esv::StatusKnockedDown, esv::Status> gStatusKnockedDownPropertyMap;
	LegacyPropertyMap<esv::StatusSummoning, esv::Status> gStatusSummoningPropertyMap;
	LegacyPropertyMap<esv::StatusHealing, esv::StatusConsume> gStatusHealingPropertyMap;
	LegacyPropertyMap<esv::StatusThrown, esv::Status> gStatusThrownPropertyMap;
	LegacyPropertyMap<esv::StatusTeleportFall, esv::Status> gStatusTeleportFallPropertyMap;
	LegacyPropertyMap<esv::StatusCombat, esv::Status> gStatusCombatPropertyMap;
	LegacyPropertyMap<esv::StatusAoO, esv::Status> gStatusAoOPropertyMap;
	LegacyPropertyMap<esv::StatusSneaking, esv::Status> gStatusSneakingPropertyMap;
	LegacyPropertyMap<esv::StatusUnlock, esv::Status> gStatusUnlockPropertyMap;
	LegacyPropertyMap<esv::StatusBoost, esv::Status> gStatusBoostPropertyMap;
	LegacyPropertyMap<esv::StatusUnsheathed, esv::Status> gStatusUnsheathedPropertyMap;
	LegacyPropertyMap<esv::StatusStance, esv::StatusConsumeBase> gStatusStancePropertyMap;
	LegacyPropertyMap<esv::StatusLying, esv::Status> gStatusLyingPropertyMap;
	LegacyPropertyMap<esv::StatusInfectiousDiseased, esv::StatusConsumeBase> gStatusInfectiousDiseasedPropertyMap;
	LegacyPropertyMap<esv::StatusInvisible, esv::StatusConsumeBase> gStatusInvisiblePropertyMap;
	LegacyPropertyMap<esv::StatusRotate, esv::Status> gStatusRotatePropertyMap;
	LegacyPropertyMap<esv::StatusIdentify, esv::Status> gStatusIdentifyPropertyMap;
	LegacyPropertyMap<esv::StatusRepair, esv::Status> gStatusRepairPropertyMap;
	LegacyPropertyMap<esv::StatusMaterial, esv::Status> gStatusMaterialPropertyMap;
	LegacyPropertyMap<esv::StatusExplode, esv::Status> gStatusExplodePropertyMap;
	LegacyPropertyMap<esv::StatusAdrenaline, esv::StatusConsumeBase> gStatusAdrenalinePropertyMap;
	LegacyPropertyMap<esv::StatusShacklesOfPain, esv::StatusConsumeBase> gStatusShacklesOfPainPropertyMap;
	LegacyPropertyMap<esv::StatusShacklesOfPainCaster, esv::StatusConsumeBase> gStatusShacklesOfPainCasterPropertyMap;
	LegacyPropertyMap<esv::StatusDrain, esv::Status> gStatusDrainPropertyMap;
	LegacyPropertyMap<esv::StatusSpiritVision, esv::StatusConsumeBase> gStatusSpiritVisionPropertyMap;
	LegacyPropertyMap<esv::StatusSpirit, esv::Status> gStatusSpiritPropertyMap;
	LegacyPropertyMap<esv::StatusDamage, esv::StatusConsumeBase> gStatusDamagePropertyMap;
	LegacyPropertyMap<esv::StatusClimbing, esv::Status> gStatusClimbingPropertyMap;
	LegacyPropertyMap<esv::StatusIncapacitated, esv::StatusConsumeBase> gStatusIncapacitatedPropertyMap;
	LegacyPropertyMap<esv::StatusInSurface, esv::Status> gStatusInSurfacePropertyMap;
	LegacyPropertyMap<esv::StatusPolymorphed, esv::StatusConsumeBase> gStatusPolymorphedPropertyMap;
	LegacyPropertyMap<esv::StatusDamageOnMove, esv::StatusDamage> gStatusDamageOnMovePropertyMap;
	LegacyPropertyMap<esv::StatusChallenge, esv::StatusConsumeBase> gStatusChallengePropertyMap;
	LegacyPropertyMap<esv::StatusHealSharing, esv::StatusConsumeBase> gStatusHealSharingPropertyMap;
	LegacyPropertyMap<esv::StatusHealSharingCaster, esv::StatusConsumeBase> gStatusHealSharingCasterPropertyMap;
	LegacyPropertyMap<esv::StatusActiveDefense, esv::StatusConsume> gStatusActiveDefensePropertyMap;
	LegacyPropertyMap<esv::StatusSpark, esv::StatusConsume> gStatusSparkPropertyMap;

	LegacyPropertyMap<stats::HitDamageInfo, void> gHitDamageInfoPropertyMap;
	LegacyPropertyMap<esv::DamageHelpers, void> gDamageHelpersPropertyMap;
	LegacyPropertyMap<esv::ShootProjectileHelper, void> gShootProjectileHelperPropertyMap;
	LegacyPropertyMap<eoc::ItemDefinition, void> gEoCItemDefinitionPropertyMap;
	LegacyPropertyMap<stats::EquipmentAttributes, void> gEquipmentAttributesPropertyMap;
	LegacyPropertyMap<stats::EquipmentAttributesWeapon, stats::EquipmentAttributes> gEquipmentAttributesWeaponPropertyMap;
	LegacyPropertyMap<stats::EquipmentAttributesArmor, stats::EquipmentAttributes> gEquipmentAttributesArmorPropertyMap;
	LegacyPropertyMap<stats::EquipmentAttributesShield, stats::EquipmentAttributes> gEquipmentAttributesShieldPropertyMap;
	LegacyPropertyMap<stats::CharacterDynamicStat, void> gCharacterDynamicStatPropertyMap;
	LegacyPropertyMap<stats::Character, void> gCharacterStatsPropertyMap;
	LegacyPropertyMap<stats::Item, void> gItemStatsPropertyMap;
	LegacyPropertyMap<eoc::PlayerCustomData, void> gPlayerCustomDataPropertyMap;
	LegacyPropertyMap<esv::Character, void> gCharacterPropertyMap;
	LegacyPropertyMap<esv::Item, void> gItemPropertyMap;

	LegacyPropertyMap<esv::ASAttack, void> gASAttackPropertyMap;
	LegacyPropertyMap<esv::ASPrepareSkill, void> gASPrepareSkillPropertyMap;
	LegacyPropertyMap<esv::SkillState, void> gSkillStatePropertyMap;

	LegacyPropertyMap<GameObjectTemplate, void> gGameObjectTemplatePropertyMap;
	LegacyPropertyMap<EoCGameObjectTemplate, GameObjectTemplate> gEoCGameObjectTemplatePropertyMap;
	LegacyPropertyMap<CharacterTemplate, EoCGameObjectTemplate> gCharacterTemplatePropertyMap;
	LegacyPropertyMap<ItemTemplate, EoCGameObjectTemplate> gItemTemplatePropertyMap;

#define BEGIN_PROPERTIES(map, cls) auto& propertyMap = map; \
	using TObject = decltype(map)::ObjectType; \
	propertyMap.Name = #cls;

#define PROP(name) AddProperty<decltype(TObject::name)>(propertyMap, #name, offsetof(TObject, name))
#define PROP_TY(name, type) AddPropertyRO<type>(propertyMap, #name, offsetof(TObject, name))
#define PROP_RO(name) AddPropertyRO<decltype(TObject::name)>(propertyMap, #name, offsetof(TObject, name))
#define PROP_ENUM(name) AddPropertyEnum<decltype(TObject::name)>(propertyMap, #name, offsetof(TObject, name), true)
#define PROP_ENUM_RO(name) AddPropertyEnum<decltype(TObject::name)>(propertyMap, #name, offsetof(TObject, name), false)
#define PROP_FLAGS(name, enum, writeable) AddPropertyFlags<std::underlying_type_t<enum>, enum>(propertyMap, #name, offsetof(TObject, name), writeable)
#define PROP_GUID(name, writeable) AddPropertyGuidString<decltype(TObject::name)>(propertyMap, #name, offsetof(TObject, name), writeable)
#define PROP_DYN_FS(name) AddPropertyDynamicFixedString<decltype(TObject::name)>(propertyMap, #name, offsetof(TObject, name), true)

// For use with game object templates' OverrideableProperty type
#define PROP_TPL_RO(name) AddPropertyRO<decltype(TObject::name)::Type>(propertyMap, #name, offsetof(TObject, name))
#define PROP_TPL(name) AddProperty<decltype(TObject::name)::Type>(propertyMap, #name, offsetof(TObject, name))

	template <class T, class TalentArrayFetcher>
	void AddTalentArray(LegacyPropertyMapBase& propertyMap, STDString const& prefix, TalentArrayFetcher f)
	{
		for (auto const& val : EnumInfo<stats::TalentType>::Store->Values) {
			auto id = (stats::TalentType)val.Value;
			auto talentName = prefix + val.Key.GetString();
			FixedString talentFS(talentName.c_str());
			AddProperty<bool>(propertyMap, talentFS.GetString(), 0);

			propertyMap.Properties[talentFS].GetInt = [id, f](void* obj) -> std::optional<int64_t> {
				auto attrs = reinterpret_cast<T*>(obj);
				return f(attrs).HasTalent(id) ? 1 : 0;
			};

			propertyMap.Properties[talentFS].SetInt = [id, f](void* obj, int64_t value) -> bool {
				auto attrs = reinterpret_cast<T*>(obj);
				f(attrs).Toggle(id, value != 0);
				return true;
			};
		}
	}

	void InitPropertyMaps()
	{
		{
			BEGIN_PROPERTIES(gStatusPropertyMap, esv::Status);
			PROP_RO(NetID);
			PROP_RO(StatusId);
			PROP(CanEnterChance);
			PROP(StartTimer);
			PROP(LifeTime);
			PROP(CurrentLifeTime);
			PROP(TurnTimer);
			PROP(Strength);
			PROP(StatsMultiplier);
			PROP_ENUM(DamageSourceType);
			PROP_RO(StatusHandle);
			PROP_RO(OwnerHandle);
			PROP(StatusSourceHandle);
			PROP_RO(CleansedByHandle);
			PROP_FLAGS(Flags0, esv::StatusFlags0, true);
			PROP_FLAGS(Flags1, esv::StatusFlags1, true);
			PROP_FLAGS(Flags2, esv::StatusFlags2, true);

			propertyMap.Flags[GFS.strForceStatus].Flags |= kPropWrite;
			propertyMap.Flags[GFS.strForceFailStatus].Flags |= kPropWrite;
			propertyMap.Flags[GFS.strRequestDeleteAtTurnEnd].Flags |= kPropWrite;

			propertyMap.Properties[GFS.strLifeTime].SetFloat = [](void * st, float value) -> bool {
				auto status = reinterpret_cast<esv::Status *>(st);
				if (value < 0.0f && value != -1.0f) return false;
				status->LifeTime = value;
				if (status->CurrentLifeTime > status->LifeTime) {
					status->CurrentLifeTime = status->LifeTime;
				}

				return true;
			};

			propertyMap.Properties[GFS.strCurrentLifeTime].SetFloat = [](void * st, float value) -> bool {
				auto status = reinterpret_cast<esv::Status *>(st);
				if (value < 0.0f && value != -1.0f) return false;
				status->CurrentLifeTime = value;
				if (status->LifeTime < status->CurrentLifeTime) {
					status->LifeTime = status->CurrentLifeTime;
				}

				return true;
			};
		}

		{
			BEGIN_PROPERTIES(gStatusConsumeBasePropertyMap, esv::StatusConsumeBase);
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
			PROP_RO(OverrideWeaponHandle);
			PROP(SavingThrow);
			PROP(SourceDirection);
			PROP(Turn);
			PROP_ENUM(HealEffectOverride);
			PROP(Poisoned);
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
			BEGIN_PROPERTIES(gStatusDyingPropertyMap, esv::StatusDying);
			propertyMap.Parent = &gStatusPropertyMap;
			PROP(SourceHandle);
			PROP_ENUM(DeathType);
			PROP(AttackDirection);
			PROP(ImpactDirection);
			PROP(IsAlreadyDead);
			PROP(DieActionsCompleted);
			PROP(ForceNoGhost);
			PROP(SkipAnimation);
			PROP(DontThrowDeathEvent);
			PROP(InflicterHandle);
			PROP(DisputeTargetHandle);
			PROP_RO(CombatId);
			PROP(IgnoreGodMode);
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
			BEGIN_PROPERTIES(gStatusCharmedPropertyMap, esv::StatusCharmed);
			propertyMap.Parent = &gStatusConsumeBasePropertyMap;
			PROP_RO(UserId);
			PROP_RO(OriginalOwnerCharacterHandle);
		}

		{
			BEGIN_PROPERTIES(gStatusKnockedDownPropertyMap, esv::StatusKnockedDown);
			propertyMap.Parent = &gStatusPropertyMap;
			PROP_RO(KnockedDownState);
			PROP(IsInstant);
		}

		{
			BEGIN_PROPERTIES(gStatusSummoningPropertyMap, esv::StatusSummoning);
			propertyMap.Parent = &gStatusPropertyMap;
			PROP_RO(AnimationDuration);
			PROP(SummonLevel);
		}

		{
			BEGIN_PROPERTIES(gStatusHealingPropertyMap, esv::StatusHeal);
			propertyMap.Parent = &gStatusConsumeBasePropertyMap;
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
			BEGIN_PROPERTIES(gStatusThrownPropertyMap, esv::StatusThrown);
			propertyMap.Parent = &gStatusPropertyMap;
			PROP(Level);
			PROP(CasterHandle);
			PROP_RO(AnimationDuration);
			PROP(IsThrowingSelf);
			PROP(LandingEstimate);
			PROP(Landed);
		}

		{
			BEGIN_PROPERTIES(gStatusTeleportFallPropertyMap, esv::StatusTeleportFall);
			propertyMap.Parent = &gStatusPropertyMap;
			PROP(Target);
			PROP(ReappearTime);
			PROP(SkillId);
			PROP(HasDamage);
			PROP(HasDamageBeenApplied);
		}

		{
			BEGIN_PROPERTIES(gStatusCombatPropertyMap, esv::StatusCombat);
			propertyMap.Parent = &gStatusPropertyMap;
			PROP_RO(ReadyForCombat);
			PROP_RO(OwnerTeamId);
		}

		{
			BEGIN_PROPERTIES(gStatusAoOPropertyMap, esv::StatusAoO);
			propertyMap.Parent = &gStatusPropertyMap;
			PROP(SourceHandle);
			PROP(AoOTargetHandle);
			PROP(PartnerHandle);
			PROP(ActivateAoOBoost);
			PROP(ShowOverhead);
		}

		{
			BEGIN_PROPERTIES(gStatusSneakingPropertyMap, esv::StatusSneaking);
			propertyMap.Parent = &gStatusPropertyMap;
			PROP(ClientRequestStop);
		}

		{
			BEGIN_PROPERTIES(gStatusUnlockPropertyMap, esv::StatusUnlock);
			propertyMap.Parent = &gStatusPropertyMap;
			PROP(SourceHandle);
			PROP(Key);
			PROP(Level);
			PROP(Unlocked);
		}

		{
			BEGIN_PROPERTIES(gStatusBoostPropertyMap, esv::StatusBoost);
			propertyMap.Parent = &gStatusPropertyMap;
			PROP(EffectTime);
			PROP(BoostId);
		}

		{
			BEGIN_PROPERTIES(gStatusUnsheathedPropertyMap, esv::StatusUnsheathed);
			propertyMap.Parent = &gStatusPropertyMap;
			PROP(Force);
		}

		{
			BEGIN_PROPERTIES(gStatusStancePropertyMap, esv::StatusStance);
			propertyMap.Parent = &gStatusConsumeBasePropertyMap;
			PROP(SkillId);
		}

		{
			BEGIN_PROPERTIES(gStatusLyingPropertyMap, esv::StatusLying);
			propertyMap.Parent = &gStatusPropertyMap;
			PROP(ItemHandle);
			PROP(Position);
			PROP(Index);
			PROP(TimeElapsed);
			PROP(Heal);
		}

		{
			BEGIN_PROPERTIES(gStatusInfectiousDiseasedPropertyMap, esv::StatusInfectiousDiseased);
			propertyMap.Parent = &gStatusConsumeBasePropertyMap;
			PROP(Infections);
			PROP(InfectTimer);
			PROP(Radius);
			PROP(StatusTargetHandle);
		}

		{
			BEGIN_PROPERTIES(gStatusInvisiblePropertyMap, esv::StatusInvisible);
			propertyMap.Parent = &gStatusConsumeBasePropertyMap;
			PROP(InvisiblePosition);
		}

		{
			BEGIN_PROPERTIES(gStatusRotatePropertyMap, esv::StatusRotate);
			propertyMap.Parent = &gStatusPropertyMap;
			PROP(Yaw);
			PROP(RotationSpeed);
		}

		{
			BEGIN_PROPERTIES(gStatusIdentifyPropertyMap, esv::StatusIdentify);
			propertyMap.Parent = &gStatusPropertyMap;
			PROP(Level);
			PROP(Identified);
			PROP(IdentifierHandle);
		}

		{
			BEGIN_PROPERTIES(gStatusRepairPropertyMap, esv::StatusRepair);
			propertyMap.Parent = &gStatusPropertyMap;
			PROP(Level);
			PROP(Repaired);
			PROP(RepairerHandle);
		}

		{
			BEGIN_PROPERTIES(gStatusMaterialPropertyMap, esv::StatusMaterial);
			propertyMap.Parent = &gStatusPropertyMap;
			PROP(MaterialUUID);
			PROP_FLAGS(ApplyFlags, StatusMaterialApplyFlags, true);
			PROP(IsOverlayMaterial);
			PROP(Fading);
			PROP(ApplyNormalMap);
			PROP(Force);
		}

		{
			BEGIN_PROPERTIES(gStatusExplodePropertyMap, esv::StatusExplode);
			propertyMap.Parent = &gStatusPropertyMap;
			PROP(Projectile);
		}

		{
			BEGIN_PROPERTIES(gStatusAdrenalinePropertyMap, esv::StatusAdrenaline);
			propertyMap.Parent = &gStatusConsumeBasePropertyMap;
			PROP(InitialAPMod);
			PROP(SecondaryAPMod);
			PROP(CombatTurn);
		}

		{
			BEGIN_PROPERTIES(gStatusShacklesOfPainPropertyMap, esv::StatusShacklesOfPain);
			propertyMap.Parent = &gStatusConsumeBasePropertyMap;
			PROP(CasterHandle);
		}

		{
			BEGIN_PROPERTIES(gStatusShacklesOfPainCasterPropertyMap, esv::StatusShacklesOfPainCaster);
			propertyMap.Parent = &gStatusConsumeBasePropertyMap;
			PROP(VictimHandle);
		}

		{
			BEGIN_PROPERTIES(gStatusDrainPropertyMap, esv::StatusDrain);
			propertyMap.Parent = &gStatusPropertyMap;
			PROP(Infused);
		}

		{
			BEGIN_PROPERTIES(gStatusSpiritVisionPropertyMap, esv::StatusSpiritVision);
			propertyMap.Parent = &gStatusConsumeBasePropertyMap;
			PROP(SpiritVisionSkillId);
		}

		{
			BEGIN_PROPERTIES(gStatusSpiritPropertyMap, esv::StatusSpirit);
			propertyMap.Parent = &gStatusPropertyMap;
			// TODO - Characters
		}

		{
			BEGIN_PROPERTIES(gStatusDamagePropertyMap, esv::StatusDamage);
			propertyMap.Parent = &gStatusConsumeBasePropertyMap;
			PROP(DamageEvent);
			PROP(HitTimer);
			PROP(TimeElapsed);
			PROP(DamageLevel);
			PROP(DamageStats);
			PROP(SpawnBlood);
		}

		{
			BEGIN_PROPERTIES(gStatusClimbingPropertyMap, esv::StatusClimbing);
			propertyMap.Parent = &gStatusPropertyMap;
			PROP(MoveDirection);
			PROP(LadderHandle);
			PROP(Level);
			PROP(Status);
			PROP(Direction);
			PROP(JumpUpLadders);
		}

		{
			BEGIN_PROPERTIES(gStatusIncapacitatedPropertyMap, esv::StatusIncapacitated);
			propertyMap.Parent = &gStatusConsumeBasePropertyMap;
			PROP(CurrentFreezeTime);
			PROP(FreezeTime);
			PROP(FrozenFlag);
		}

		{
			BEGIN_PROPERTIES(gStatusInSurfacePropertyMap, esv::StatusInSurface);
			propertyMap.Parent = &gStatusPropertyMap;
			PROP(SurfaceTimerCheck);
			PROP(SurfaceDistanceCheck);
			PROP(Translate);
			PROP(Force);
			PROP_FLAGS(Layers, ESurfaceFlag, true);
		}

		{
			BEGIN_PROPERTIES(gStatusPolymorphedPropertyMap, esv::StatusPolymorphed);
			propertyMap.Parent = &gStatusConsumeBasePropertyMap;
			PROP(OriginalTemplate);
			PROP(TransformedRace);
			PROP(OriginalTemplateType);
			PROP(PolymorphResult);
			PROP(DisableInteractions);
		}

		{
			BEGIN_PROPERTIES(gStatusDamageOnMovePropertyMap, esv::StatusDamageOnMove);
			propertyMap.Parent = &gStatusDamagePropertyMap;
			PROP(DistancePerDamage);
			PROP(DistanceTraveled);
		}

		{
			BEGIN_PROPERTIES(gStatusChallengePropertyMap, esv::StatusChallenge);
			propertyMap.Parent = &gStatusConsumeBasePropertyMap;
			PROP(SourceHandle);
			PROP(Target);
		}

		{
			BEGIN_PROPERTIES(gStatusHealSharingPropertyMap, esv::StatusHealSharing);
			propertyMap.Parent = &gStatusConsumeBasePropertyMap;
			PROP(CasterHandle);
		}

		{
			BEGIN_PROPERTIES(gStatusHealSharingCasterPropertyMap, esv::StatusHealSharingCaster);
			propertyMap.Parent = &gStatusConsumeBasePropertyMap;
		}

		{
			BEGIN_PROPERTIES(gStatusActiveDefensePropertyMap, esv::StatusActiveDefense);
			propertyMap.Parent = &gStatusConsumeBasePropertyMap;
			PROP(Charges);
			PROP(TargetPos);
			PROP(StatusTargetHandle);
			PROP(Radius);
			PROP(Projectile);
			// TODO - Conditions, PreviousTargets
		}

		{
			BEGIN_PROPERTIES(gStatusSparkPropertyMap, esv::StatusSpark);
			propertyMap.Parent = &gStatusConsumeBasePropertyMap;
			PROP(Charges);
			PROP(Radius);
			PROP(Projectile);
		}

		{
			BEGIN_PROPERTIES(gHitDamageInfoPropertyMap, stats::HitDamageInfo);
			PROP(Equipment);
			PROP(TotalDamageDone);
			PROP(DamageDealt);
			PROP_ENUM(DeathType);
			PROP_ENUM(DamageType);
			PROP(AttackDirection);
			PROP(ArmorAbsorption);
			PROP(LifeSteal);
			PROP_FLAGS(EffectFlags, stats::HitFlag, true);
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
			PROP(Slot);
			PROP(Amount);
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
			PROP(EquipmentStatsType);
			PROP(HasModifiedSkills);
			PROP_DYN_FS(Skills);
			PROP(HasGeneratedStats);
			PROP(IsIdentified);
			PROP(GMFolding);
			PROP(CanUseRemotely);
		}

		{
			BEGIN_PROPERTIES(gEquipmentAttributesPropertyMap, stats::EquipmentAttributes);

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
			PROP_RO(ModifierType);
			PROP_RO(ObjectInstanceName);
			PROP_RO(BoostName);
			PROP_ENUM_RO(StatsType);

			for (auto const& val : EnumInfo<StatAttributeFlags>::Store->Values) {
				auto id = val.Value;
				AddProperty<bool>(propertyMap, val.Key.GetString(), 0);

				propertyMap.Properties[val.Key].GetInt = [id](void* obj) -> std::optional<int64_t> {
					auto attrs = reinterpret_cast<stats::EquipmentAttributes*>(obj);
					auto attrFlags = GetStaticSymbols().GetStats()->GetFlags((int)attrs->AttributeFlagsObjectId);
					if (attrFlags) {
						return (**attrFlags & id) != 0 ? 1 : 0;
					} else {
						return 0;
					}
				};

				propertyMap.Properties[val.Key].SetInt = [id](void* obj, int64_t value) -> bool {
					auto attrs = reinterpret_cast<stats::EquipmentAttributes*>(obj);
					int flagsId = (int)attrs->AttributeFlagsObjectId;
					auto attrFlags = GetStaticSymbols().GetStats()->GetOrCreateFlags(flagsId);
					attrs->AttributeFlagsObjectId = flagsId;

					if (value) {
						*attrFlags |= id;
					} else {
						*attrFlags &= ~id;
					}
					return true;
				};
			}

			for (auto const& v : EnumInfo<stats::AbilityType>::Store->Values) {
				AddProperty<int32_t>(propertyMap, v.Key.GetString(), offsetof(TObject, AbilityModifiers) + (unsigned)v.Value * sizeof(int32_t));
			}

			AddTalentArray<stats::EquipmentAttributes>(propertyMap, "TALENT_", [](stats::EquipmentAttributes* obj) {
				return obj->Talents;
			});
		}

		{
			BEGIN_PROPERTIES(gEquipmentAttributesWeaponPropertyMap, stats::EquipmentAttributesWeapon);
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
			BEGIN_PROPERTIES(gEquipmentAttributesArmorPropertyMap, stats::EquipmentAttributesArmor);
			propertyMap.Parent = &gEquipmentAttributesPropertyMap;
			PROP(ArmorValue);
			PROP(ArmorBoost);
			PROP(MagicArmorValue);
			PROP(MagicArmorBoost);
		}

		{
			BEGIN_PROPERTIES(gEquipmentAttributesShieldPropertyMap, stats::EquipmentAttributesShield);
			propertyMap.Parent = &gEquipmentAttributesPropertyMap;
			PROP(ArmorValue);
			PROP(ArmorBoost);
			PROP(MagicArmorValue);
			PROP(MagicArmorBoost);
			PROP(Blocking);
		}

		{
			BEGIN_PROPERTIES(gCharacterDynamicStatPropertyMap, stats::CharacterDynamicStat);
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

			for (auto const& val : EnumInfo<StatAttributeFlags>::Store->Values) {
				auto id = val.Value;
				AddProperty<bool>(propertyMap, val.Key.GetString(), 0);

				propertyMap.Properties[val.Key].GetInt = [id](void* obj) -> std::optional<int64_t> {
					auto attrs = reinterpret_cast<stats::CharacterDynamicStat*>(obj);
					auto attrFlags = GetStaticSymbols().GetStats()->GetFlags((int)attrs->AttributeFlagsObjectId);
					if (attrFlags) {
						return (**attrFlags & id) != 0 ? 1 : 0;
					} else {
						return 0;
					}
				};

				propertyMap.Properties[val.Key].SetInt = [id](void* obj, int64_t value) -> bool {
					auto attrs = reinterpret_cast<stats::CharacterDynamicStat*>(obj);
					int flagsId = (int)attrs->AttributeFlagsObjectId;
					auto attrFlags = GetStaticSymbols().GetStats()->GetOrCreateFlags(flagsId);
					attrs->AttributeFlagsObjectId = flagsId;

					if (value) {
						*attrFlags |= id;
					} else {
						*attrFlags &= ~id;
					}
					return true;
				};
			}

			for (auto const& val : EnumInfo<stats::AbilityType>::Store->Values) {
				AddProperty<int32_t>(propertyMap, val.Key.GetString(), offsetof(TObject, Abilities) + (unsigned)val.Value * sizeof(int32_t));
			}

			AddTalentArray<stats::CharacterDynamicStat>(propertyMap, "TALENT_", [](stats::CharacterDynamicStat* obj) {
				return obj->Talents;
			});

			AddTalentArray<stats::CharacterDynamicStat>(propertyMap, "REMOVED_TALENT_", [](stats::CharacterDynamicStat* obj) {
				return obj->RemovedTalents;
			});
		}

		{
			BEGIN_PROPERTIES(gCharacterStatsPropertyMap, stats::Character);
			// Object
			PROP_RO(Level);
			PROP_RO(Name);
			PROP_RO(AIFlags);
			PROP_RO(InstanceId);

			// Character
			PROP(CurrentVitality);
			PROP(CurrentArmor);
			PROP(CurrentMagicArmor);
			PROP(ArmorAfterHitCooldownMultiplier);
			PROP(MagicArmorAfterHitCooldownMultiplier);
			PROP_RO(MPStart);
			PROP(CurrentAP);
			PROP(BonusActionPoints);
			PROP(Experience);
			PROP(Reputation);
			PROP_RO(Flanked);
			PROP(Karma);
			PROP_FLAGS(Flags, stats::CharacterFlags, false);
			PROP(MaxResistance);
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
			PROP(MaxMpOverride);
			PROP_FLAGS(AttributeFlags, StatAttributeFlags, false);

			AddTalentArray<stats::Character>(propertyMap, "DISABLED_ALENT_", [](stats::Character* obj) {
				return obj->DisabledTalents;
			});
			// TODO - TraitOrder?
		}

		{
			BEGIN_PROPERTIES(gItemStatsPropertyMap, stats::Item);
			// Object
			PROP_RO(Level);
			PROP_RO(Name);
			PROP_RO(InstanceId);

			// Item
			PROP_ENUM_RO(ItemType);
			PROP_ENUM_RO(ItemSlot);
			PROP_ENUM(WeaponType);
			PROP(AnimType);
			PROP(WeaponRange);
			PROP(IsIdentified);
			PROP_RO(IsTwoHanded);
			PROP(ShouldSyncStats);
			PROP(HasModifiedSkills);
			PROP(Skills);
			PROP_ENUM(DamageTypeOverwrite);
			PROP(Durability);
			PROP(DurabilityCounter);
			PROP(ItemTypeReal);
			PROP_FLAGS(AttributeFlags, StatAttributeFlags, false);
			PROP(MaxCharges);
			PROP(Charges);
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
			PROP_FLAGS(Flags, esv::CharacterFlags, true);
			PROP_FLAGS(Flags2, esv::CharacterFlags2, true);
			PROP_FLAGS(Flags3, esv::CharacterFlags3, true);

			// Make dangerous flags read-only
			propertyMap.Flags[GFS.strIsPlayer].Flags &= ~kPropWrite;
			propertyMap.Flags[GFS.strMultiplayer].Flags &= ~kPropWrite;
			propertyMap.Flags[GFS.strInParty].Flags &= ~kPropWrite;
			propertyMap.Flags[GFS.strHostControl].Flags &= ~kPropWrite;
			propertyMap.Flags[GFS.strActivated].Flags &= ~kPropWrite;
			propertyMap.Flags[GFS.strOffStage].Flags &= ~kPropWrite;
			propertyMap.Flags[GFS.strDead].Flags &= ~kPropWrite;
			propertyMap.Flags[GFS.strHasOwner].Flags &= ~kPropWrite;
			propertyMap.Flags[GFS.strInDialog].Flags &= ~kPropWrite;
			propertyMap.Flags[GFS.strSummon].Flags &= ~kPropWrite;
			propertyMap.Flags[GFS.strCharacterControl].Flags &= ~kPropWrite;
			propertyMap.Flags[GFS.strInArena].Flags &= ~kPropWrite;
			propertyMap.Flags[GFS.strDeactivated].Flags &= ~kPropWrite;
			propertyMap.Flags[GFS.strMadePlayer].Flags &= ~kPropWrite;
			propertyMap.Flags[GFS.strMadePlayer].Flags &= ~kPropWrite;
			propertyMap.Flags[GFS.strGlobal].Flags &= ~kPropWrite;
			propertyMap.Flags[GFS.strIsGameMaster].Flags &= ~kPropWrite;

			for (auto const& val : EnumInfo<esv::CharacterFlags>::Store->Values) {
				auto id = val.Value;
				auto& flag = propertyMap.Flags[val.Key];
				if (flag.Flags & kPropWrite) {
					flag.Set = [id](void* obj, bool value) -> bool {
						auto ch = reinterpret_cast<esv::Character*>(obj);
						if (value) {
							ch->SetFlags(id);
						} else {
							ch->ClearFlags(id);
						}
						return true;
					};
				}
			}

			PROP_RO(Scale);
			PROP(AnimationOverride);
			PROP(WalkSpeedOverride);
			PROP(RunSpeedOverride);
			PROP_RO(NeedsUpdateCount);
			PROP_RO(ScriptForceUpdateCount);
			PROP_RO(ForceSynchCount);
			PROP_RO(InventoryHandle);
			PROP_RO(SkillBeingPrepared);
			PROP(LifeTime);
			PROP(TurnTimer);
			PROP_RO(TriggerTrapsTimer);
			PROP_TY(UserID, int32_t);
			PROP_TY(ReservedUserID, int32_t);
			PROP_RO(OwnerHandle);
			PROP_RO(FollowCharacterHandle);
			PROP_RO(SpiritCharacterHandle);
			PROP_RO(CorpseCharacterHandle);
			PROP(PartialAP);
			PROP(AnimType);
			PROP_RO(DelayDeathCount);
			PROP(AnimationSetOverride);
			PROP_RO(OriginalTransformDisplayName);
			PROP_RO(PartyHandle);
			PROP(CustomTradeTreasure);
			PROP(IsAlarmed);
			PROP(CrimeWarningsEnabled);
			PROP(CrimeInterrogationEnabled);
			PROP_RO(MovingCasterHandle);
			PROP(Archetype);
			PROP(EquipmentColor);
			PROP(ProjectileTemplate);
			PROP_RO(ReadyCheckBlocked);
			PROP(CorpseLootable);
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
			PROP_FLAGS(Flags, esv::ItemFlags, true);
			PROP_FLAGS(Flags2, esv::ItemFlags2, true);

			// Make dangerous flags read-only
			propertyMap.Flags[GFS.strActivated].Flags &= ~kPropWrite;
			propertyMap.Flags[GFS.strOffStage].Flags &= ~kPropWrite;
			propertyMap.Flags[GFS.strDestroyed].Flags &= ~kPropWrite;
			propertyMap.Flags[GFS.strGlobal].Flags &= ~kPropWrite;

			for (auto const& val : EnumInfo<esv::ItemFlags>::Store->Values) {
				auto id = val.Value;
				auto& flag = propertyMap.Flags[val.Key];
				if (flag.Flags & kPropWrite) {
					flag.Set = [id](void* obj, bool value) -> bool {
						auto ch = reinterpret_cast<esv::Item*>(obj);
						if (value) {
							ch->SetFlags(id);
						} else {
							ch->ClearFlags(id);
						}
						return true;
					};
				}
			}

			PROP_RO(CurrentLevel);
			PROP_RO(Scale);
			PROP(CustomDisplayName);
			PROP(CustomDescription);
			PROP(CustomBookContent);
			PROP(StatsId);
			PROP_RO(InventoryHandle);
			PROP_RO(ParentInventoryHandle);
			PROP_RO(Slot);
			PROP(Amount);
			PROP(Vitality);
			PROP(Armor);
			PROP_RO(InUseByCharacterHandle);
			PROP(Key);
			PROP(LockLevel);
			PROP_RO(OwnerHandle);
			PROP(ComputedVitality);
			PROP(Rarity);
			PROP(GoldValueOverwrite);
			PROP(WeightValueOverwrite);
			PROP(TreasureLevel);
			PROP(LevelOverride);
			PROP_RO(ForceSynch);
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
			PROP_RO(IgnoreChecks);
			PROP_RO(IsStealthed);
			PROP_RO(CleanseStatuses);
			PROP_RO(StatusClearChance);
			PROP_RO(CharacterHasSkill);
		}

		{
			BEGIN_PROPERTIES(gGameObjectTemplatePropertyMap, GameObjectTemplate);
			// TODO Tags
			PROP_RO(Id);
			PROP(Name);
			PROP_RO(RootTemplate);
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
			propertyMap.Parent = &gGameObjectTemplatePropertyMap;
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
			propertyMap.Parent = &gEoCGameObjectTemplatePropertyMap;
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
			propertyMap.Parent = &gEoCGameObjectTemplatePropertyMap;
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
	}


	bool OsirisPropertyMapGetRaw(LegacyPropertyMapBase const & propertyMap, void * obj,
		OsiArgumentDesc & args, uint32_t firstArg, OsiPropertyMapType type, bool throwError)
	{
		auto propertyNameStr = args[firstArg].String;
		FixedString propertyName(propertyNameStr);

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
				args[firstArg + 1].Set(FixedString("NULL_00000000-0000-0000-0000-000000000000").GetString());
				return true;
			}

			auto gameObject = esv::GetEntityWorld()->GetGameObject(*val);
			if (gameObject != nullptr) {
				args[firstArg + 1].Set(*gameObject->GetGuid());
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


	bool OsirisPropertyMapSetRaw(LegacyPropertyMapBase const & propertyMap, void * obj,
		OsiArgumentDesc const & args, uint32_t firstArg, OsiPropertyMapType type, bool throwError)
	{
		auto propertyNameStr = args[firstArg].String;
		FixedString propertyName(propertyNameStr);

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
				|| NameGuidToFixedString(guid) == FixedString("00000000-0000-0000-0000-000000000000")) {
				return propertyMap.setHandle(obj, propertyName, ComponentHandle{}, false, throwError);
			}

			auto gameObject = esv::GetEntityWorld()->GetGameObject(guid);
			if (gameObject != nullptr) {
				ComponentHandle handle;
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
			glm::vec3 vec(x, y, z);
			return propertyMap.setVector3(obj, propertyName, vec, false, throwError);
		}

		default:
			OsiError("Failed to set property '" << propertyNameStr << "': Unknown Osi property type!");
			return false;
		}
	}


	bool LuaPropertyMapGet(lua_State* L, LegacyPropertyMapBase const& propertyMap, void* obj,
		char const* propertyName, bool throwError)
	{
		return LuaPropertyMapGet(L, propertyMap, obj, FixedString(propertyName), throwError);
	}

	bool LuaPropertyMapGet(lua_State * L, LegacyPropertyMapBase const & propertyMap, void * obj,
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
			type = LegacyPropertyType::kFixedString;
		}

		switch (type) {
		case LegacyPropertyType::kBool:
		{
			auto val = propertyMap.getInt(obj, propertyName, false, throwError);
			if (val) {
				lua::push(L, *val != 0);
				return true;
			} else {
				return false;
			}
		}

		case LegacyPropertyType::kUInt8:
		case LegacyPropertyType::kInt16:
		case LegacyPropertyType::kUInt16:
		case LegacyPropertyType::kInt32:
		case LegacyPropertyType::kUInt32:
		case LegacyPropertyType::kInt64:
		case LegacyPropertyType::kUInt64:
		{
			auto val = propertyMap.getInt(obj, propertyName, false, throwError);
			if (val) {
				lua::push(L, *val);
				return true;
			} else {
				return false;
			}
		}

		case LegacyPropertyType::kFloat:
		{
			auto val = propertyMap.getFloat(obj, propertyName, false, throwError);
			if (val) {
				lua::push(L, *val);
				return true;
			} else {
				return false;
			}
		}

		case LegacyPropertyType::kFixedString:
		case LegacyPropertyType::kDynamicFixedString:
		case LegacyPropertyType::kFixedStringGuid:
		case LegacyPropertyType::kStringPtr:
		case LegacyPropertyType::kStdString:
		case LegacyPropertyType::kStdWString:
		case LegacyPropertyType::kTranslatedString:
		{
			auto val = propertyMap.getString(obj, propertyName, false, throwError);
			if (val) {
				lua::push(L, *val);
				return true;
			} else {
				return false;
			}
		}

		case LegacyPropertyType::kObjectHandle:
		{
			auto val = propertyMap.getHandle(obj, propertyName, false, throwError);
			if (val) {
				lua::push(L, *val);
				return true;
			} else {
				return false;
			}
		}

		case LegacyPropertyType::kVector3:
		{
			auto val = propertyMap.getVector3(obj, propertyName, false, throwError);
			if (val) {
				lua::push(L, *val);
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


	bool LuaPropertyMapSet(lua_State * L, int index, LegacyPropertyMapBase const & propertyMap, 
		void * obj, char const * propertyName, bool throwError)
	{
		if (obj == nullptr) {
			if (throwError) {
				OsiError("Attempted to set property '" << propertyName << "' of null object!");
			}
			return false;
		}

		FixedString propertyFS(propertyName);

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
			type = LegacyPropertyType::kFixedString;
		}

		switch (type) {
		case LegacyPropertyType::kBool:
		{
			luaL_checktype(L, index, LUA_TBOOLEAN);
			auto val = lua_toboolean(L, index);
			return propertyMap.setInt(obj, propertyFS, val, false, throwError);
		}

		case LegacyPropertyType::kUInt8:
		case LegacyPropertyType::kInt16:
		case LegacyPropertyType::kUInt16:
		case LegacyPropertyType::kInt32:
		case LegacyPropertyType::kUInt32:
		case LegacyPropertyType::kInt64:
		case LegacyPropertyType::kUInt64:
		{
			auto val = luaL_checkinteger(L, index);
			return propertyMap.setInt(obj, propertyFS, val, false, throwError);
		}

		case LegacyPropertyType::kFloat:
		{
			auto val = luaL_checknumber(L, index);
			return propertyMap.setFloat(obj, propertyFS, (float)val, false, throwError);
		}

		case LegacyPropertyType::kFixedString:
		case LegacyPropertyType::kDynamicFixedString:
		case LegacyPropertyType::kFixedStringGuid:
		case LegacyPropertyType::kStringPtr:
		case LegacyPropertyType::kStdString:
		case LegacyPropertyType::kStdWString:
		case LegacyPropertyType::kTranslatedString:
		{
			auto val = luaL_checkstring(L, index);
			return propertyMap.setString(obj, propertyFS, val, false, throwError);
		}

		case LegacyPropertyType::kObjectHandle:
		{
			auto handle = lua::get<ComponentHandle>(L, index);
			return propertyMap.setHandle(obj, propertyFS, handle, false, throwError);
		}

		case LegacyPropertyType::kVector3:
		{
			auto val = lua::get<glm::vec3>(L, index);
			return propertyMap.setVector3(obj, propertyFS, val, false, throwError);
		}

		default:
			OsiError("Failed to set property '" << propertyName << "': Unsupported property type for Lua update!");
			return false;
		}
	}
}