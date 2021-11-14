BEGIN_CLS(CDivinityStats_Equipment_Attributes)
P(Durability)
P(DurabilityDegradeSpeed)
P(StrengthBoost)
P(FinesseBoost)
P(IntelligenceBoost)
P(ConstitutionBoost)
P(MemoryBoost)
P(WitsBoost)
P(SightBoost)
P(HearingBoost)
P(VitalityBoost)
P(SourcePointsBoost)
P(MaxAP)
P(StartAP)
P(APRecovery)
P(AccuracyBoost)
P(DodgeBoost)
P(LifeSteal)
P(CriticalChance)
P(ChanceToHitBoost)
P(MovementSpeedBoost)
P(RuneSlots)
P(RuneSlots_V1)
P(FireResistance)
P(AirResistance)
P(WaterResistance)
P(EarthResistance)
P(PoisonResistance)
P(ShadowResistance)
P(PiercingResistance)
P(CorrosiveResistance)
P(PhysicalResistance)
P(MagicResistance)
P(CustomResistance)
P(Movement)
P(Initiative)
P(Willpower)
P(Bodybuilding)
P(MaxSummons)
P(Value)
P(Weight)
// TODO - Reflection
P(Skills)
P(ItemColor)
P_RO(ModifierType)
P_RO(ObjectInstanceName)
P_RO(BoostName)
P_RO(StatsType)

/*EnumInfo<StatAttributeFlags>::Values.Iterate([&propertyMap](auto const& name, auto const& id) {
	AddProperty<bool>(propertyMap, name.Str, 0);

	propertyMap.Properties[name].GetInt = [id](void* obj) -> std::optional<int64_t> {
		auto attrs = reinterpret_cast<CDivinityStats_Equipment_Attributes*>(obj);
		auto attrFlags = GetStaticSymbols().GetStats()->GetAttributeFlags((int)attrs->AttributeFlagsObjectId);
		if (attrFlags) {
			return (uint64_t)(**attrFlags & id) != 0 ? 1 : 0;
		}
		else {
			return 0;
		}
	};

	propertyMap.Properties[name].SetInt = [id](void* obj, int64_t value) -> bool {
		auto attrs = reinterpret_cast<CDivinityStats_Equipment_Attributes*>(obj);
		int flagsId = (int)attrs->AttributeFlagsObjectId;
		auto attrFlags = GetStaticSymbols().GetStats()->GetOrCreateAttributeFlags(flagsId);
		attrs->AttributeFlagsObjectId = flagsId;

		if (value) {
			*attrFlags |= id;
		}
		else {
			*attrFlags &= ~id;
		}
		return true;
	};
	});

EnumInfo<AbilityType>::Values.Iterate([&propertyMap](auto const& name, auto const& id) {
	AddProperty<int32_t>(propertyMap, name.Str, offsetof(TObject, AbilityModifiers) + (unsigned)id * sizeof(int32_t));
	});

AddTalentArray<CDivinityStats_Equipment_Attributes>(propertyMap, "TALENT_", [](CDivinityStats_Equipment_Attributes* obj) {
	return obj->Talents;
	});
}*/
END_CLS()


BEGIN_CLS(CDivinityStats_Equipment_Attributes_Weapon)
INHERIT(CDivinityStats_Equipment_Attributes)
P(DamageType)
P(MinDamage)
P(MaxDamage)
P(DamageBoost)
P(DamageFromBase)
P(CriticalDamage)
P(WeaponRange)
P(CleaveAngle)
P(CleavePercentage)
P(AttackAPCost)
P(Projectile)
END_CLS()


BEGIN_CLS(CDivinityStats_Equipment_Attributes_Armor)
INHERIT(CDivinityStats_Equipment_Attributes)
P(ArmorValue)
P(ArmorBoost)
P(MagicArmorValue)
P(MagicArmorBoost)
END_CLS()


BEGIN_CLS(CDivinityStats_Equipment_Attributes_Shield)
INHERIT(CDivinityStats_Equipment_Attributes)
P(ArmorValue)
P(ArmorBoost)
P(MagicArmorValue)
P(MagicArmorBoost)
P(Blocking)
END_CLS()



BEGIN_CLS(CharacterDynamicStat)
P(SummonLifelinkModifier)
P(Strength)
P(Memory)
P(Intelligence)
P(Movement)
P(MovementSpeedBoost)
P(Finesse)
P(Wits)
P(Constitution)

P(FireResistance)
P(EarthResistance)
P(WaterResistance)
P(AirResistance)
P(PoisonResistance)
P(ShadowResistance)

P(Willpower)
P(Bodybuilding)
P(PiercingResistance)
P(PhysicalResistance)
P(CorrosiveResistance)
P(MagicResistance)
P(CustomResistance)

P(Sight)
P(Hearing)
P(FOV)
P(APMaximum)
P(APStart)
P(APRecovery)
P(CriticalChance)
P(Initiative)
P(Vitality)
P(VitalityBoost)
P(MagicPoints)
P(Level)
P(Gain)

P(Armor)
P(MagicArmor)
P(ArmorBoost)
P(MagicArmorBoost)
P(ArmorBoostGrowthPerLevel)
P(MagicArmorBoostGrowthPerLevel)
P(DamageBoost)
P(DamageBoostGrowthPerLevel)

P(Accuracy)
P(Dodge)
P(MaxResistance)
P(LifeSteal)
P(Weight)
P(ChanceToHitBoost)
P(RangeBoost)
P(APCostBoost)
P(SPCostBoost)
P(MaxSummons)
P(BonusWeaponDamageMultiplier)
P(TranslationKey)
P(BonusWeapon)
P(StepsType)

/*EnumInfo<StatAttributeFlags>::Values.Iterate([&propertyMap](auto const& name, auto const& id) {
	AddProperty<bool>(propertyMap, name.Str, 0);

	propertyMap.Properties[name].GetInt = [id](void* obj) -> std::optional<int64_t> {
		auto attrs = reinterpret_cast<CharacterDynamicStat*>(obj);
		auto attrFlags = GetStaticSymbols().GetStats()->GetAttributeFlags((int)attrs->AttributeFlagsObjectId);
		if (attrFlags) {
			return (uint64_t)(**attrFlags & id) != 0 ? 1 : 0;
		}
		else {
			return 0;
		}
	};

	propertyMap.Properties[name].SetInt = [id](void* obj, int64_t value) -> bool {
		auto attrs = reinterpret_cast<CharacterDynamicStat*>(obj);
		int flagsId = (int)attrs->AttributeFlagsObjectId;
		auto attrFlags = GetStaticSymbols().GetStats()->GetOrCreateAttributeFlags(flagsId);
		attrs->AttributeFlagsObjectId = flagsId;

		if (value) {
			*attrFlags |= id;
		}
		else {
			*attrFlags &= ~id;
		}
		return true;
	};
	});

EnumInfo<AbilityType>::Values.Iterate([&propertyMap](auto const& name, auto const& id) {
	AddProperty<int32_t>(propertyMap, name.Str, offsetof(TObject, Abilities) + (unsigned)id * sizeof(int32_t));
	});

AddTalentArray<CharacterDynamicStat>(propertyMap, "TALENT_", [](CharacterDynamicStat* obj) {
	return obj->Talents;
	});

AddTalentArray<CharacterDynamicStat>(propertyMap, "REMOVED_TALENT_", [](CharacterDynamicStat* obj) {
	return obj->RemovedTalents;
	});
}*/
END_CLS()


// FIXME - add polymorphic getter?
BEGIN_CLS(CRPGStats_Object)
P_RO(Handle)
P(Level)
P_RO(ModifierListIndex)
P_RO(Name)
P_RO(TranslatedStringX)
P_RO(FS2)
// IS DivStats useful for scripts?
// P_REF(DivStats)
P_REF(PropertyList)
// P_REF(ConditionList)
P_RO(AIFlags)
// FIXME - these use different memory allocators than normal ObjectSets
// P_REF(Requirements)
// P_REF(MemorizationRequirements)
P_RO(StringProperties1)
P_RO(ComboCategories)
// FIXME
// Add using custom property! - P_RO(Using)
END_CLS()

BEGIN_CLS(CRPGStats_ObjectInstance)
INHERIT(CRPGStats_Object)
P_RO(InstanceId)
END_CLS()

BEGIN_CLS(CDivinityStats_Character)
INHERIT(CRPGStats_ObjectInstance)
P(CurrentVitality)
P(CurrentArmor)
P(CurrentMagicArmor)
P(ArmorAfterHitCooldownMultiplier)
P(MagicArmorAfterHitCooldownMultiplier)
P_RO(MPStart)
P(CurrentAP)
P(BonusActionPoints)
P(Experience)
P(Reputation)
P_RO(Flanked)
P(Karma)
P_RO(Flags)
P_REF(TraitOrder)
P(MaxResistance)
P_RO(HasTwoHandedWeapon)
// FIXME - polymorphic ref? P_REF(Character)
P_RO(IsIncapacitatedRefCount)
P_REF(DynamicStats)
P_REF(StatsFromStatsEntry)
// FIXME - via custom map!
// P_REF(EquippedItems)
// FIXME - SurfacePathInfluences
P_RO(MaxVitality)
P_RO(BaseMaxVitality)
P_RO(MaxArmor)
P_RO(BaseMaxArmor)
P_RO(MaxMagicArmor)
P_RO(BaseMaxMagicArmor)
P_RO(Sight)
P_RO(BaseSight)
P_RO(AttributeFlags)
P_RO(BaseAttributeFlags)
P_RO(ItemBoostedAttributeFlags)
P_RO(AttributeFlagsUpdated)
P_RO(MaxSummons)
P_RO(BaseMaxSummons)
P(MaxMpOverride)
// FIXME - special class for TalentArray? P_REF(DisabledTalents)

/*AddTalentArray<CDivinityStats_Character>(propertyMap, "DISABLED_ALENT_", [](CDivinityStats_Character* obj) {
	return obj->DisabledTalents;
	});
// TODO - TraitOrder?
*/

// FIXME - add props for v55 compat:
P_GETTER(MainWeapon, GetMainWeapon)
P_GETTER(OffHandWeapon, GetOffHandWeapon)
// ModId
// NotSneaking (wtf?)
// Base* (Base version of stats)
// * (Boosted version of stats)
// ----- Do we need these?
// Rotation
// Position
// MyGuid
// NetID

END_CLS()

BEGIN_CLS(CDivinityStats_Item)
INHERIT(CRPGStats_ObjectInstance)
P_RO(ItemType)
P_RO(ItemSlot)
P(WeaponType)
P(AnimType)
P(WeaponRange)
P(IsIdentified)
P_RO(IsTwoHanded)
P(ShouldSyncStats)
P(HasModifiedSkills)
P(Skills)
P(DamageTypeOverwrite)
P(Durability)
P(DurabilityCounter)
P(ItemTypeReal)
P_REF(DynamicAttributes)
P_RO(AttributeFlags)
P(MaxCharges)
P(Charges)
P_REF(BoostNameSet)
END_CLS()


BEGIN_CLS(StatusPrototype)
P_RO(StatusId)
P_RO(StatusName)
P_RO(DisplayName)
P_RO(Icon)
P_RO(HasStats)
P_RO(AbsorbSurfaceTypes)
END_CLS()


BEGIN_CLS(SkillPrototype)
P_RO(SkillTypeId)
P_RO(SkillId)
P_RO(Ability)
P_RO(Tier)
P_RO(Requirement)
P_RO(Level)
P_RO(MagicCost)
P_RO(MemoryCost)
P_RO(ActionPoints)
P_RO(Cooldown)
P_RO(CooldownReduction)
P_RO(ChargeDuration)
P_RO(DisplayName)
P_RO(Icon)
P_RO(AiFlags)
P_REF(RootSkillPrototype)
P_REF(ChildPrototypes)
END_CLS()


BEGIN_CLS(CDivinityStats_Object_Property_Data)
P_RO(Name)
P_RO(TypeId)
P_RO(Context)
END_CLS()


BEGIN_CLS(CDivinityStats_Object_Property_Status)
INHERIT(CDivinityStats_Object_Property_Data)
P(Status)
P(StatusChance)
P(Duration)
P(StatsId)
P(Arg4)
P(Arg5)
P(SurfaceBoost)
P_REF(SurfaceBoosts)
END_CLS()


BEGIN_CLS(CRPGStats_Object_Property_Extender)
INHERIT(CDivinityStats_Object_Property_Data)
P(PropertyName)
P(Arg1)
P(Arg2)
P(Arg3)
P(Arg4)
P(Arg5)
END_CLS()


// FIXME - property list API!
BEGIN_CLS(CRPGStats_Object_Property_List)
P_RO(Name)
P_RO(AllPropertyContexts)
END_CLS()


BEGIN_CLS(esv::PendingHit)
P_RO(Id)
P_RO(TargetHandle)
P_RO(AttackerHandle)
P_RO(CapturedCharacterHit)
P_REF(WeaponStats)
P_REF(CharacterHitPointer)
P_REF(CharacterHitDamageList)
P_REF(CharacterHit)
P_RO(HitType)
P_RO(NoHitRoll)
P_RO(ProcWindWalker)
P_RO(ForceReduceDurability)
P_RO(HighGround)
P_RO(CriticalRoll)
P_REF(Status)
END_CLS()
