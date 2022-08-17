BEGIN_CLS(SurfacePathInfluence)
P(MatchFlags)
P(Influence)
P(MaskFlags)
END_CLS()

BEGIN_CLS(stats::Requirement)
PN(Requirement, RequirementId)
P(Param)
P(Tag)
P(Not)
END_CLS()

BEGIN_CLS(stats::Reflection)
P(DamageType)
P(MeleeOnly)
END_CLS()

BEGIN_CLS(stats::ReflectionSet)
// FIXME - P_REF(DamageTypes)
END_CLS()

BEGIN_CLS_TN(stats::EquipmentAttributes, CDivinityStats_Equipment_Attributes)
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
P_REF(Reflection)
P(Skills)
P(ItemColor)
P_RO(ModifierType)
P_RO(ObjectInstanceName)
P_RO(BoostName)
P_RO(StatsType)

#if defined(GENERATING_TYPE_INFO)
for (auto const& label : EnumInfo<StatAttributeFlags>::Values) {
	ADD_TYPE(label.Key, bool)
}
for (auto const& label : EnumInfo<stats::AbilityType>::Values) {
	ADD_TYPE(label.Key, int32_t)
}
for (auto const& label : EnumInfo<stats::TalentType>::Values) {
	ADD_TYPE(STDString("TALENT_") + label.Key.GetString(), bool)
}
#endif

#if defined(GENERATING_PROPMAP)
// Attribute flag getters/setters
for (auto const& label : EnumInfo<StatAttributeFlags>::Values) {
	auto abilityId = label.Value;
	pm.AddProperty(label.Key.GetString(),
		[](lua_State* L, LifetimeHandle const& lifetime, stats::EquipmentAttributes* obj, std::size_t offset, uint64_t flag) {
			auto attrFlags = GetStaticSymbols().GetStats()->GetFlags((int)obj->AttributeFlagsObjectId);
			if (attrFlags) {
				push(L, (**attrFlags & flag) != 0);
			} else {
				push(L, false);
			}
			return PropertyOperationResult::Success;
		},
		[](lua_State* L, LifetimeHandle const& lifetime, stats::EquipmentAttributes* obj, int index, std::size_t offset, uint64_t flag) {
			auto val = get<bool>(L, index);
			int flagsId = (int)obj->AttributeFlagsObjectId;
			auto attrFlags = GetStaticSymbols().GetStats()->GetOrCreateFlags(flagsId);
			obj->AttributeFlagsObjectId = (int64_t)flagsId;
			if (attrFlags) {
				if (val) {
					*attrFlags |= flag;
				} else {
					*attrFlags &= ~flag;
				}
			}
			return PropertyOperationResult::Success;
		}, 0, (uint64_t)abilityId
	);
}

// Ability getters (eg. Aerothurge)
for (auto const& label : EnumInfo<stats::AbilityType>::Values) {
	auto abilityId = label.Value;
	pm.AddProperty(label.Key.GetString(),
		[](lua_State* L, LifetimeHandle const& lifetime, stats::EquipmentAttributes* obj, std::size_t offset, uint64_t flag) {
			push(L, obj->AbilityModifiers[flag]);
			return PropertyOperationResult::Success;
		},
		[](lua_State* L, LifetimeHandle const& lifetime, stats::EquipmentAttributes* obj, int index, std::size_t offset, uint64_t flag) {
			auto val = get<int32_t>(L, index);
			obj->AbilityModifiers[flag] = val;
			return PropertyOperationResult::Success;
		}, 0, (uint64_t)abilityId
	);
}

// Talent getter (eg. TALENT_)
for (auto const& label : EnumInfo<stats::TalentType>::Values) {
	auto talentId = label.Value;
	STDString talentName = STDString("TALENT_") + label.Key.GetString();
	pm.AddProperty(talentName.c_str(),
		[](lua_State* L, LifetimeHandle const& lifetime, stats::EquipmentAttributes* obj, std::size_t offset, uint64_t flag) {
			push(L, obj->Talents.HasTalent((stats::TalentType)flag));
			return PropertyOperationResult::Success;
		},
		[](lua_State* L, LifetimeHandle const& lifetime, stats::EquipmentAttributes* obj, int index, std::size_t offset, uint64_t flag) {
			auto val = get<bool>(L, index);
			if (val) {
				obj->Talents.Set((uint32_t)flag);
			} else {
				obj->Talents.Clear((uint32_t)flag);
			}
			return PropertyOperationResult::Success;
		}, 0, (uint64_t)talentId
	);
}
#endif

END_CLS()


BEGIN_CLS_TN(stats::EquipmentAttributesWeapon, CDivinityStats_Equipment_Attributes_Weapon)
INHERIT(stats::EquipmentAttributes)
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


BEGIN_CLS_TN(stats::EquipmentAttributesArmor, CDivinityStats_Equipment_Attributes_Armor)
INHERIT(stats::EquipmentAttributes)
P(ArmorValue)
P(ArmorBoost)
P(MagicArmorValue)
P(MagicArmorBoost)
END_CLS()


BEGIN_CLS_TN(stats::EquipmentAttributesShield, CDivinityStats_Equipment_Attributes_Shield)
INHERIT(stats::EquipmentAttributes)
P(ArmorValue)
P(ArmorBoost)
P(MagicArmorValue)
P(MagicArmorBoost)
P(Blocking)
END_CLS()



BEGIN_CLS(stats::CharacterDynamicStat)
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

#if defined(GENERATING_TYPE_INFO)
for (auto const& label : EnumInfo<StatAttributeFlags>::Values) {
	ADD_TYPE(label.Key, bool)
}
for (auto const& label : EnumInfo<stats::AbilityType>::Values) {
	ADD_TYPE(label.Key, int32_t)
}
for (auto const& label : EnumInfo<stats::TalentType>::Values) {
	ADD_TYPE(STDString("TALENT_") + label.Key.GetString(), bool)
}
#endif

#if defined(GENERATING_PROPMAP)
// Attribute flag getters/setters
for (auto const& label : EnumInfo<StatAttributeFlags>::Values) {
	auto abilityId = label.Value;
	pm.AddProperty(label.Key.GetString(),
		[](lua_State* L, LifetimeHandle const& lifetime, stats::CharacterDynamicStat* obj, std::size_t offset, uint64_t flag) {
			auto attrFlags = GetStaticSymbols().GetStats()->GetFlags((int)obj->AttributeFlagsObjectId);
			if (attrFlags) {
				push(L, (**attrFlags & flag) != 0);
			} else {
				push(L, false);
			}
			return PropertyOperationResult::Success;
		},
		[](lua_State* L, LifetimeHandle const& lifetime, stats::CharacterDynamicStat* obj, int index, std::size_t offset, uint64_t flag) {
			auto val = get<bool>(L, index);
			auto attrFlags = GetStaticSymbols().GetStats()->GetFlags((int)obj->AttributeFlagsObjectId);
			if (attrFlags) {
				if (val) {
					**attrFlags |= flag;
				} else {
					**attrFlags &= ~flag;
				}
			}
			return PropertyOperationResult::Success;
		}, 0, (uint64_t)abilityId
	);
}

// Ability getters (eg. Aerothurge)
for (auto const& label : EnumInfo<stats::AbilityType>::Values) {
	auto abilityId = label.Value;
	pm.AddProperty(label.Key.GetString(),
		[](lua_State* L, LifetimeHandle const& lifetime, stats::CharacterDynamicStat* obj, std::size_t offset, uint64_t flag) {
			push(L, obj->Abilities[flag]);
			return PropertyOperationResult::Success;
		},
		[](lua_State* L, LifetimeHandle const& lifetime, stats::CharacterDynamicStat* obj, int index, std::size_t offset, uint64_t flag) {
			auto val = get<int32_t>(L, index);
			obj->Abilities[flag] = val;
			return PropertyOperationResult::Success;
		}, 0, (uint64_t)abilityId
	);
}

// Talent getter (eg. TALENT_)
for (auto const& label : EnumInfo<stats::TalentType>::Values) {
	auto talentId = label.Value;
	STDString talentName = STDString("TALENT_") + label.Key.GetString();
	pm.AddProperty(talentName.c_str(),
		[](lua_State* L, LifetimeHandle const& lifetime, stats::CharacterDynamicStat* obj, std::size_t offset, uint64_t flag) {
			push(L, obj->Talents.HasTalent((stats::TalentType)flag));
			return PropertyOperationResult::Success;
		},
		[](lua_State* L, LifetimeHandle const& lifetime, stats::CharacterDynamicStat* obj, int index, std::size_t offset, uint64_t flag) {
			auto val = get<bool>(L, index);
			if (val) {
				obj->Talents.Set((uint32_t)flag);
			} else {
				obj->Talents.Clear((uint32_t)flag);
			}
			return PropertyOperationResult::Success;
		}, 0, (uint64_t)talentId
	);
}
#endif

END_CLS()


BEGIN_CLS(stats::Object)
P_RO(Handle)
P(Level)
P_RO(ModifierListIndex)
P_RO(Name)
P_REF(DisplayName)
P_RO(FS2)
// IS DivStats useful for scripts?
// P_REF(DivStats)
P_REF(PropertyLists)
// P_REF(Conditions)
P_RO(AIFlags)
P_REF(Requirements)
P_REF(MemorizationRequirements)
P_RO(StringProperties1)
P_RO(ComboCategories)
// FIXME
// Add using custom property! - P_RO(Using)

#if defined(GENERATING_TYPE_INFO)
ADD_TYPE("StatsEntry", stats::Object)
ADD_TYPE("ModId", FixedString)
#endif

#if defined(GENERATING_PROPMAP)
pm.AddProperty("ModifierList",
	[](lua_State* L, LifetimeHandle const& lifetime, stats::Object* obj, std::size_t offset, uint64_t flag) {
		push(L, obj->GetModifierList()->Name);
		return PropertyOperationResult::Success;
	}
);

pm.AddProperty("StatsEntry",
	[](lua_State* L, LifetimeHandle const& lifetime, stats::Object* obj, std::size_t offset, uint64_t flag) {
		ObjectProxy2::MakeImpl<StatsEntryProxyRefImpl, stats::Object>(L, obj, lifetime, std::optional<int>(), false);
		return PropertyOperationResult::Success;
	}
);

pm.AddProperty("ModId",
	[](lua_State* L, LifetimeHandle const& lifetime, stats::Object* obj, std::size_t offset, uint64_t flag) {
		push(L, gExtender->GetStatLoadOrderHelper().GetStatsEntryMod(obj->Name));
		return PropertyOperationResult::Success;
	}
);
#endif

P_FALLBACK(&stats::Object::LuaFallbackGet, &stats::Object::LuaFallbackSet)
END_CLS()

BEGIN_CLS(stats::ObjectInstance)
INHERIT(stats::Object)
P_RO(InstanceId)
END_CLS()

BEGIN_CLS_TN(stats::Character, CDivinityStats_Character)
INHERIT(stats::ObjectInstance)
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
P_BITMASK(Flags)
P_REF(TraitOrder)
P(MaxResistance)
P_RO(HasTwoHandedWeapon)
PN_REF(Character, GameObject)
P_RO(IsIncapacitatedRefCount)
P_REF(DynamicStats)
P_REF(StatsFromStatsEntry)
// FIXME - via custom map!
// P_REF(EquippedItems)
P_REF(SurfacePathInfluences)
P_RO(MaxVitality)
P_RO(BaseMaxVitality)
P_RO(MaxArmor)
P_RO(BaseMaxArmor)
P_RO(MaxMagicArmor)
P_RO(BaseMaxMagicArmor)
// Sight/BaseSight are fetched through CharacterStatGetters
// P_RO(Sight)
// P_RO(BaseSight)
P_RO(AttributeFlags)
P_BITMASK(AttributeFlags)
P_RO(BaseAttributeFlags)
P_RO(ItemBoostedAttributeFlags)
P_RO(AttributeFlagsUpdated)
P_RO(MaxSummons)
P_RO(BaseMaxSummons)
P(MaxMpOverride)
// FIXME - special class for TalentArray? P_REF(DisabledTalents)

/*AddTalentArray<Character>(propertyMap, "DISABLED_ALENT_", [](Character* obj) {
	return obj->DisabledTalents;
	});
// TODO - TraitOrder?
*/

// v55 compatibility
P_FUN(GetItemBySlot, GetItemBySlot)
P_GETTER(MainWeapon, GetMainWeapon)
P_GETTER(OffHandWeapon, GetOffHandWeapon)

#if defined(GENERATING_TYPE_INFO)
ADD_TYPE("Position", glm::vec3)
ADD_TYPE("MyGuid", FixedString)
ADD_TYPE("NetID", NetId)
for (auto const& label : EnumInfo<stats::CharacterStatGetterType>::Values) {
	ADD_TYPE(label.Key, int32_t)
	ADD_TYPE(STDString("Base") + label.Key.GetString(), int32_t)
}
for (auto const& label : EnumInfo<stats::AbilityType>::Values) {
	ADD_TYPE(label.Key, int32_t)
	ADD_TYPE(STDString("Base") + label.Key.GetString(), int32_t)
}
for (auto const& label : EnumInfo<stats::TalentType>::Values) {
	ADD_TYPE(STDString("TALENT_") + label.Key.GetString(), bool)
}
#endif

#if defined(GENERATING_PROPMAP)
// TODO - GFS.strCharacter
pm.AddProperty("Rotation",
	[](lua_State* L, LifetimeHandle const& lifetime, stats::Character* obj, std::size_t offset, uint64_t flag) {
		if (obj->GameObject) {
			push(L, *obj->GameObject->GetRotation());
		} else {
			push(L, nullptr);
		}

		return PropertyOperationResult::Success;
	}
);

pm.AddProperty("Position",
	[](lua_State* L, LifetimeHandle const& lifetime, stats::Character* obj, std::size_t offset, uint64_t flag) {
		if (obj->GameObject) {
			push(L, *obj->GameObject->GetTranslate());
		} else {
			push(L, nullptr);
		}

		return PropertyOperationResult::Success;
	}
);

pm.AddProperty("MyGuid",
	[](lua_State* L, LifetimeHandle const& lifetime, stats::Character* obj, std::size_t offset, uint64_t flag) {
		if (obj->GameObject) {
			push(L, *obj->GameObject->GetGuid());
		} else {
			push(L, nullptr);
		}

		return PropertyOperationResult::Success;
	}
);

pm.AddProperty("NetID",
	[](lua_State* L, LifetimeHandle const& lifetime, stats::Character* obj, std::size_t offset, uint64_t flag) {
		if (obj->GameObject) {
			NetId netId;
			obj->GameObject->GetNetID(netId);
			push(L, netId);
		} else {
			push(L, nullptr);
		}

		return PropertyOperationResult::Success;
	}
);

for (auto const& label : EnumInfo<stats::CharacterStatGetterType>::Values) {
	auto statId = label.Value;

	// Boosted property getter (eg. BlockChance)
	pm.AddProperty(label.Key.GetString(),
		[](lua_State* L, LifetimeHandle const& lifetime, stats::Character* obj, std::size_t offset, uint64_t flag) {
			push(L, GetStaticSymbols().CharStatsGetters.GetStat(obj, (stats::CharacterStatGetterType)flag, false, false));
			return PropertyOperationResult::Success;
		},
		nullptr, 0, (uint64_t)statId
	);

	// Base property getter (eg. BaseBlockChance)
	STDString basePropName = STDString("Base") + label.Key.GetString();
	pm.AddProperty(basePropName.c_str(),
		[](lua_State* L, LifetimeHandle const& lifetime, stats::Character* obj, std::size_t offset, uint64_t flag) {
			push(L, GetStaticSymbols().CharStatsGetters.GetStat(obj, (stats::CharacterStatGetterType)flag, false, true));
			return PropertyOperationResult::Success;
		},
		nullptr, 0, (uint64_t)statId
	);
}

for (auto const& label : EnumInfo<stats::AbilityType>::Values) {
	auto abilityId = label.Value;

	// Boosted ability getter (eg. Aerothurge)
	pm.AddProperty(label.Key.GetString(),
		[](lua_State* L, LifetimeHandle const& lifetime, stats::Character* obj, std::size_t offset, uint64_t flag) {
			push(L, obj->GetAbility((stats::AbilityType)flag, false));
			return PropertyOperationResult::Success;
		},
		nullptr, 0, (uint64_t)abilityId
	);

	// Base ability getter (eg. BaseAerothurge)
	STDString baseAbilityName = STDString("Base") + label.Key.GetString();
	pm.AddProperty(baseAbilityName.c_str(),
		[](lua_State* L, LifetimeHandle const& lifetime, stats::Character* obj, std::size_t offset, uint64_t flag) {
			push(L, obj->GetAbility((stats::AbilityType)flag, true));
			return PropertyOperationResult::Success;
		},
		nullptr, 0, (uint64_t)abilityId
	);
}

for (auto const& label : EnumInfo<stats::TalentType>::Values) {
	auto talentId = label.Value;

	// Talent getter (eg. TALENT_ResistDead)
	STDString talentName = STDString("TALENT_") + label.Key.GetString();
	pm.AddProperty(talentName.c_str(),
		[](lua_State* L, LifetimeHandle const& lifetime, stats::Character* obj, std::size_t offset, uint64_t flag) {
			push(L, obj->HasTalent((stats::TalentType)flag, false));
			return PropertyOperationResult::Success;
		},
		nullptr, 0, (uint64_t)talentId
	);

	// Base talent getter (eg. TALENT_BaseResistDead)
	STDString baseTalentName = STDString("TALENT_Base") + label.Key.GetString();
	pm.AddProperty(baseTalentName.c_str(),
		[](lua_State* L, LifetimeHandle const& lifetime, stats::Character* obj, std::size_t offset, uint64_t flag) {
			push(L, obj->HasTalent((stats::TalentType)flag, true));
			return PropertyOperationResult::Success;
		},
		nullptr, 0, (uint64_t)talentId
	);
}
#endif

END_CLS()

BEGIN_CLS_TN(stats::Item, CDivinityStats_Item)
INHERIT(stats::ObjectInstance)
P_RO(ItemType) // For compatibility with v55; new name is EquipmentType
PN_RO(EquipmentType, ItemType)
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
P(ItemTypeReal) // For compatibility with v55; new name is Rarity
PN(Rarity, ItemTypeReal)
P_REF(GameObject)
P_REF(DynamicStats)
P_RO(AttributeFlags)
P_BITMASK(AttributeFlags)
P(MaxCharges)
P(Charges)
P_REF(DeltaMods)

#if defined(GENERATING_TYPE_INFO)
for (auto const& label : EnumInfo<stats::AbilityType>::Values) {
	ADD_TYPE(label.Key, int32_t)
	ADD_TYPE(STDString("Base") + label.Key.GetString(), int32_t)
}
for (auto const& label : EnumInfo<stats::TalentType>::Values) {
	ADD_TYPE(STDString("TALENT_") + label.Key.GetString(), bool)
}
#endif

#if defined(GENERATING_PROPMAP)
for (auto const& label : EnumInfo<stats::AbilityType>::Values) {
	auto abilityId = label.Value;

	// Ability stat getter (eg. Aerothurge)
	pm.AddProperty(label.Key.GetString(),
		[](lua_State* L, LifetimeHandle const& lifetime, stats::Item* obj, std::size_t offset, uint64_t flag) {
			push(L, obj->GetAbility((stats::AbilityType)flag));
			return PropertyOperationResult::Success;
		},
		nullptr, 0, (uint64_t)abilityId
	);
}

for (auto const& label : EnumInfo<stats::TalentType>::Values) {
	auto talentId = label.Value;

	// Talent stat getter (eg. TALENT_)
	STDString talentName = STDString("TALENT_") + label.Key.GetString();
	pm.AddProperty(talentName.c_str(),
		[](lua_State* L, LifetimeHandle const& lifetime, stats::Item* obj, std::size_t offset, uint64_t flag) {
			push(L, obj->HasTalent((stats::TalentType)flag));
			return PropertyOperationResult::Success;
		},
		nullptr, 0, (uint64_t)talentId
	);
}
#endif
END_CLS()


BEGIN_CLS(stats::StatusPrototype)
P_RO(StatusId)
P_RO(StatusName)
P_REF(DisplayName)
P_RO(Icon)
P_RO(HasStats)
P_RO(AbsorbSurfaceTypes)

#if defined(GENERATING_TYPE_INFO)
ADD_TYPE("StatsObject", stats::Object)
#endif

#if defined(GENERATING_PROPMAP)
pm.AddProperty("StatsObject",
	[](lua_State* L, LifetimeHandle const& lifetime, stats::StatusPrototype* obj, std::size_t offset, uint64_t flag) {
		MakeObjectRef(L, obj->GetStats());
		return PropertyOperationResult::Success;
	}
);
#endif

P_FALLBACK(&stats::StatusPrototype::LuaFallbackGet, &stats::StatusPrototype::LuaFallbackSet)
END_CLS()


BEGIN_CLS(stats::SkillPrototype)
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

#if defined(GENERATING_TYPE_INFO)
ADD_TYPE("StatsObject", stats::Object)
#endif

#if defined(GENERATING_PROPMAP)
pm.AddProperty("StatsObject",
	[](lua_State* L, LifetimeHandle const& lifetime, stats::SkillPrototype* obj, std::size_t offset, uint64_t flag) {
		MakeObjectRef(L, obj->GetStats());
		return PropertyOperationResult::Success;
	}
);
#endif

P_FALLBACK(&stats::SkillPrototype::LuaFallbackGet, &stats::SkillPrototype::LuaFallbackSet)
END_CLS()


BEGIN_CLS(stats::PropertyData)
P_RO(Name)
P_RO(TypeId)
P_RO(Context)
END_CLS()


BEGIN_CLS(stats::PropertyStatus)
INHERIT(stats::PropertyData)
P(Status)
P(StatusChance)
P(Duration)
P(StatsId)
P(Arg4)
P(Arg5)
P(SurfaceBoost)
P_REF(SurfaceBoosts)
END_CLS()


BEGIN_CLS(stats::PropertyExtender)
INHERIT(stats::PropertyData)
P(PropertyName)
PN(Action, PropertyName)
P(Arg1)
P(Arg2)
P(Arg3)
P(Arg4)
P(Arg5)
END_CLS()


// FIXME - property list API!
BEGIN_CLS(stats::PropertyList)
P_RO(Name)
P_RO(AllPropertyContexts)
END_CLS()


BEGIN_CLS(stats::ItemColorDefinition)
P(Color1)
P(Color2)
P(Color3)
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
