BEGIN_CLS(esv::Status)
P_RO(NetID)
P(StartTime)
P_RO(StatusId)
P(CanEnterChance)
P(StartTimer)
P(LifeTime)
P(CurrentLifeTime)
P(TurnTimer)
P(Strength)
P(StatsMultiplier)
P(DamageSourceType)
P_RO(StatusHandle)
P_RO(OwnerHandle)
PN_RO(TargetHandle, OwnerHandle) // OwnerHandle was called TargetHandle before v55
P_REF(StatusOwner)
P(StatusSourceHandle)
P_RO(CleansedByHandle)
P(Flags0)
P(Flags1)
P(Flags2)
P_BITMASK(Flags0)
P_BITMASK(Flags1)
P_BITMASK(Flags2)

#if defined(GENERATING_TYPE_INFO)
ADD_TYPE("StatusType", FixedString)
#endif

#if defined(GENERATING_PROPMAP)
pm.AddProperty("StatusType",
	[](lua_State* L, LifetimeHandle const& lifetime, esv::Status* status, std::size_t offset, uint64_t flag) {
		push(L, status->GetStatusId());
		return PropertyOperationResult::Success;
	}
);
#endif

/*
* FIXME!
propertyMap.Properties[GFS.strLifeTime].SetFloat = [](void* st, float value) -> bool {
	auto status = reinterpret_cast<esv::Status*>(st);
	if (value < 0.0f && value != -1.0f) return false;
	status->LifeTime = value;
	if (status->CurrentLifeTime > status->LifeTime) {
		status->CurrentLifeTime = status->LifeTime;
	}

	return true;
};

propertyMap.Properties[GFS.strCurrentLifeTime].SetFloat = [](void* st, float value) -> bool {
	auto status = reinterpret_cast<esv::Status*>(st);
	if (value < 0.0f && value != -1.0f) return false;
	status->CurrentLifeTime = value;
	if (status->LifeTime < status->CurrentLifeTime) {
		status->LifeTime = status->CurrentLifeTime;
	}

	return true;
};
}*/
END_CLS()

BEGIN_CLS(esv::StatusConsumeBase::StatsData)
P(StatsId)
P(Turn)
END_CLS()

BEGIN_CLS(esv::StatusConsumeBase)
INHERIT(esv::Status)
P_REF(Skill)
P_REF(Items)
P_REF(ResetCooldownsAbilities)
P(ResetAllCooldowns)
P(ResetOncePerCombat)
P(ScaleWithVitality)
P(LoseControl)
P(ApplyStatusOnTick)
P_REF(ItemHandles)
P(EffectTime)
P(StatsId)
P_REF(StatsIds)
P(StackId)
P(OriginalWeaponStatsId)
P(OverrideWeaponStatsId)
P_RO(OverrideWeaponHandle)
P(SavingThrow)
P(SourceDirection)
P_REF(SurfaceChanges)
P(Turn)
P(HealEffectOverride)
P(Poisoned)
END_CLS()


BEGIN_CLS(esv::StatusHit)
INHERIT(esv::Status)
P_REF(PropertyList)
P_REF(Hit)
P_RO(HitByHandle)
P_RO(HitWithHandle)
P_RO(WeaponHandle)
P(HitReason)
P(SkillId)
P(Interruption)
P(AllowInterruptAction)
P(ForceInterrupt)
P_RO(DecDelayDeathCount)
P(ImpactPosition)
P(ImpactOrigin)
P(ImpactDirection)
END_CLS()


BEGIN_CLS(esv::StatusDying)
INHERIT(esv::Status)
P(SourceHandle)
P(SourceType)
P(DeathType)
P(AttackDirection)
P(ImpactDirection)
P(IsAlreadyDead)
P(DieActionsCompleted)
P(ForceNoGhost)
P(SkipAnimation)
P(DontThrowDeathEvent)
P(InflicterHandle)
P(DisputeTargetHandle)
P_RO(CombatId)
P(IgnoreGodMode)
END_CLS()


BEGIN_CLS(esv::StatusHeal)
INHERIT(esv::Status)
P(EffectTime)
P(HealAmount)
P(HealEffect)
P(HealEffectId)
P(HealType)
P(AbsorbSurfaceRange)
P_REF(AbsorbSurfaceTypes)
P(TargetDependentHeal)
P_REF(TargetDependentValue)
P_REF(TargetDependentHealAmount)
END_CLS()


BEGIN_CLS(esv::StatusCharmed)
INHERIT(esv::StatusConsumeBase)
P_RO(UserId)
P_RO(OriginalOwnerCharacterHandle)
END_CLS()


BEGIN_CLS(esv::StatusKnockedDown)
INHERIT(esv::Status)
P_RO(KnockedDownState)
P(IsInstant)
END_CLS()


BEGIN_CLS(esv::StatusSummoning)
INHERIT(esv::Status)
P_RO(AnimationDuration)
P(SummonLevel)
END_CLS()


BEGIN_CLS(esv::StatusHealing)
INHERIT(esv::StatusConsumeBase)
P(HealAmount)
P(TimeElapsed)
P(HealEffect)
P(HealEffectId)
P(SkipInitialEffect)
P(HealingEvent)
P(HealStat)
P(AbsorbSurfaceRange)
END_CLS()


BEGIN_CLS(esv::StatusThrown)
INHERIT(esv::Status)
P(Level)
P(CasterHandle)
P_RO(AnimationDuration)
P(IsThrowingSelf)
P(LandingEstimate)
P(Landed)
END_CLS()


BEGIN_CLS(esv::StatusTeleportFall)
INHERIT(esv::Status)
P(Target)
P(ReappearTime)
P(SkillId)
P(HasDamage)
P(HasDamageBeenApplied)
END_CLS()


BEGIN_CLS(esv::StatusCombat)
INHERIT(esv::Status)
P_RO(ReadyForCombat)
P_REF(OwnerTeamId)
END_CLS()


BEGIN_CLS(esv::StatusAoO)
INHERIT(esv::Status)
P(SourceHandle)
P(AoOTargetHandle)
P(PartnerHandle)
P(ActivateAoOBoost)
P(ShowOverhead)
END_CLS()


BEGIN_CLS(esv::StatusSneaking)
INHERIT(esv::Status)
P(ClientRequestStop)
END_CLS()


BEGIN_CLS(esv::StatusUnlock)
INHERIT(esv::Status)
P(SourceHandle)
P(Key)
P(Level)
P(Unlocked)
END_CLS()


BEGIN_CLS(esv::StatusBoost)
INHERIT(esv::Status)
P(EffectTime)
P(BoostId)
END_CLS()


BEGIN_CLS(esv::StatusUnsheathed)
INHERIT(esv::Status)
P(Force)
END_CLS()


BEGIN_CLS(esv::StatusStance)
INHERIT(esv::StatusConsumeBase)
P(SkillId)
END_CLS()


BEGIN_CLS(esv::StatusLying)
INHERIT(esv::Status)
P(ItemHandle)
P(Position)
P(Index)
P(TimeElapsed)
P(Heal)
END_CLS()


BEGIN_CLS(esv::StatusInfectiousDiseased)
INHERIT(esv::StatusConsumeBase)
P(Infections)
P(InfectTimer)
P(Radius)
P(StatusTargetHandle)
END_CLS()


BEGIN_CLS(esv::StatusInvisible)
INHERIT(esv::StatusConsumeBase)
P(InvisiblePosition)
END_CLS()


BEGIN_CLS(esv::StatusRotate)
INHERIT(esv::Status)
P(Yaw)
P(RotationSpeed)
END_CLS()


BEGIN_CLS(esv::StatusIdentify)
INHERIT(esv::Status)
P(Level)
P(Identified)
P(IdentifierHandle)
END_CLS()


BEGIN_CLS(esv::StatusRepair)
INHERIT(esv::Status)
P(Level)
P(Repaired)
P(RepairerHandle)
END_CLS()


BEGIN_CLS(esv::StatusMaterial)
INHERIT(esv::Status)
P(MaterialUUID)
P(ApplyFlags)
P(IsOverlayMaterial)
P(Fading)
P(ApplyNormalMap)
P(Force)
END_CLS()


BEGIN_CLS(esv::StatusExplode)
INHERIT(esv::Status)
P(Projectile)
END_CLS()


BEGIN_CLS(esv::StatusAdrenaline)
INHERIT(esv::StatusConsumeBase)
P(InitialAPMod)
P(SecondaryAPMod)
P(CombatTurn)
END_CLS()


BEGIN_CLS(esv::StatusShacklesOfPain)
INHERIT(esv::StatusConsumeBase)
P(CasterHandle)
END_CLS()


BEGIN_CLS(esv::StatusShacklesOfPainCaster)
INHERIT(esv::StatusConsumeBase)
P(VictimHandle)
END_CLS()


BEGIN_CLS(esv::StatusDrain)
INHERIT(esv::Status)
P(Infused)
END_CLS()


BEGIN_CLS(esv::StatusSpiritVision)
INHERIT(esv::StatusConsumeBase)
P(SpiritVisionSkillId)
END_CLS()


BEGIN_CLS(esv::StatusSpirit)
INHERIT(esv::Status)
P_REF(Characters)
END_CLS()


BEGIN_CLS(esv::StatusDamage)
INHERIT(esv::StatusConsumeBase)
P(DamageEvent)
P(HitTimer)
P(TimeElapsed)
P(DamageLevel)
P(DamageStats)
P(SpawnBlood)
END_CLS()


BEGIN_CLS(esv::StatusClimbing)
INHERIT(esv::Status)
P(MoveDirection)
P(LadderHandle)
P(Level)
P(Status)
P(Direction)
P(JumpUpLadders)
END_CLS()


BEGIN_CLS(esv::StatusIncapacitated)
INHERIT(esv::StatusConsumeBase)
P(CurrentFreezeTime)
P(FreezeTime)
P(FrozenFlag)
END_CLS()


BEGIN_CLS(esv::StatusInSurface)
INHERIT(esv::Status)
P(SurfaceTimerCheck)
P(SurfaceDistanceCheck)
P(Translate)
P(Force)
P(Layers)
END_CLS()


BEGIN_CLS(esv::StatusPolymorphed)
INHERIT(esv::StatusConsumeBase)
P(OriginalTemplate)
P(TransformedRace)
P(OriginalTemplateType)
P(PolymorphResult)
P(DisableInteractions)
END_CLS()


BEGIN_CLS(esv::StatusDamageOnMove)
INHERIT(esv::StatusDamage)
P(DistancePerDamage)
P(DistanceTraveled)
END_CLS()


BEGIN_CLS(esv::StatusChallenge)
INHERIT(esv::StatusConsumeBase)
P(SourceHandle)
P(Target)
END_CLS()


BEGIN_CLS(esv::StatusHealSharing)
INHERIT(esv::StatusConsumeBase)
P(CasterHandle)
END_CLS()


BEGIN_CLS(esv::StatusHealSharingCaster)
INHERIT(esv::StatusConsumeBase)
P_REF(StatusTargets)
P_REF(BeamEffects)
END_CLS()


BEGIN_CLS(esv::StatusActiveDefense)
INHERIT(esv::StatusConsumeBase)
P(Charges)
P(TargetPos)
P(StatusTargetHandle)
P(Radius)
P(Projectile)
// FIXME - P_REF(TargetConditions)
// FIXME - Targets
P_REF(PreviousTargets)
END_CLS()


BEGIN_CLS(esv::StatusSpark)
INHERIT(esv::StatusConsumeBase)
P(Charges)
P(Radius)
P(Projectile)
// FIXME - P_REF(TargetConditions)
END_CLS()


BEGIN_CLS(esv::StatusConsume)
INHERIT(esv::StatusConsumeBase)
END_CLS()


BEGIN_CLS(esv::Aura::AppliedAura)
P(LifeTime)
P_REF(AppliedStatuses)
END_CLS()


BEGIN_CLS(esv::Aura)
P_REF(AuraSelf)
P_REF(AuraAllies)
P_REF(AuraNeutrals)
P_REF(AuraEnemies)
P_REF(AuraItems)
P_RO(Owner)
P_RO(SomeObjHandle)
P_REF(AppliedAuras)
P(TickTimer)
END_CLS()


BEGIN_CLS(esv::SkillStatusAura)
INHERIT(esv::Aura)
P(Position)
P(AreaRadius)
END_CLS()


BEGIN_CLS(esv::StatusMachine)
P_RO(IsStatusMachineActive)
P_RO(PreventStatusApply)
P_REF(Statuses)
P_RO(OwnerObjectHandle)
END_CLS()


BEGIN_CLS(ecl::Status)
P_RO(NetID)
P_RO(OwnerHandle)
P_RO(StatusHandle)
P_RO(StatusId)
P(StartTimer)
P(LifeTime)
P(CurrentLifeTime)
P(StatsMultiplier)
P_RO(Flags)
P_RO(StatusSourceHandle)

#if defined(GENERATING_TYPE_INFO)
ADD_TYPE("StatusType", FixedString)
#endif

#if defined(GENERATING_PROPMAP)
pm.AddProperty("StatusType",
	[](lua_State* L, LifetimeHandle const& lifetime, ecl::Status* status, std::size_t offset, uint64_t flag) {
		push(L, status->GetStatusId());
		return PropertyOperationResult::Success;
	}
);
#endif
END_CLS()


BEGIN_CLS(ecl::StatusConsumeBase::MaterialParam)
P(Value)
P(Name)
END_CLS()


BEGIN_CLS(ecl::StatusConsumeBase::StatsData)
P(TurnIndex)
P(StatsId)
END_CLS()


BEGIN_CLS(ecl::StatusConsumeBase)
INHERIT(ecl::Status)
P(TooltipText)
P_REF(StatsDataPerTurn)
P(StatsId)
P(StackId)
P(Icon)
P(OriginalWeaponId)
P(OverrideWeaponId)
P(OverrideWeaponHandle)
P_RO(AttributeHandle)
P(StatusEffectOverrideForItems)
P_REF(EffectHandler)
P_REF(ApplyEffectHandler)
P(SourceDirection)
P(MaterialType)
P(Material)
P(MaterialApplyFlags)
P(MaterialApplyNormalMap)
P(MaterialFadeAmount)
P(MaterialOverlayOffset)
P_REF(MaterialParams)
P(Turn)
P(AuraRadius)
P(AuraFX)
P_RO(AuraFXHandle)
P(CurrentStatsMultiplier)
P(HasItemFlag0x200000000000)
P(TooltipTextNeedsUpdating)
P_REF(TooltipTexts)
END_CLS()


BEGIN_CLS(ecl::StatusDying)
INHERIT(ecl::Status)
P_RO(InflicterNetId)
P(DeathType)
P(AttackDirection)
P(ImpactDirection)
P(IsAlreadyDead)
P(DieActionsCompleted)
P(SkipAnimation)
P(CombatId)
END_CLS()


BEGIN_CLS(ecl::StatusCharmed)
INHERIT(ecl::StatusConsumeBase)
P_RO(EffectHandle)
P_RO(OriginalOwnerCharacterHandle)
END_CLS()


BEGIN_CLS(ecl::StatusFloating)
INHERIT(ecl::StatusConsumeBase)
P(OnlyWhileMoving)
END_CLS()


BEGIN_CLS(ecl::StatusIncapacitated)
INHERIT(ecl::StatusConsumeBase)
P(CurrentFreezeTime)
P(FreezeTime)
END_CLS()


BEGIN_CLS(ecl::StatusEncumbered)
INHERIT(ecl::StatusConsumeBase)
P_RO(State)
P(NextMessageDelay)
END_CLS()


BEGIN_CLS(ecl::StatusDamage)
INHERIT(ecl::StatusConsumeBase)
P(DamageLevel)
P(DamageStats)
END_CLS()


BEGIN_CLS(ecl::StatusHealing)
INHERIT(ecl::StatusConsumeBase)
P(HealAmount)
P(HealStat)
END_CLS()


BEGIN_CLS(ecl::StatusLeadership)
INHERIT(ecl::StatusConsumeBase)
P(Strength)
END_CLS()


BEGIN_CLS(ecl::StatusAdrenaline)
INHERIT(ecl::StatusConsumeBase)
P(InitialAPMod)
P(SecondaryAPMod)
END_CLS()


BEGIN_CLS(ecl::StatusPolymorphed)
INHERIT(ecl::StatusConsumeBase)
P(DisableInteractions)
P(PolymorphResult)
END_CLS()


BEGIN_CLS(ecl::StatusDamageOnMove)
INHERIT(ecl::StatusConsumeBase)
P(DamageAmount)
P(DistancePerDamage)
END_CLS()


BEGIN_CLS(ecl::StatusGuardianAngel)
INHERIT(ecl::StatusConsumeBase)
P_RO(EffectHandle)
END_CLS()


BEGIN_CLS(ecl::StatusChallenge)
INHERIT(ecl::StatusConsumeBase)
P(SourceHandle)
P(Target)
END_CLS()


BEGIN_CLS(ecl::StatusActiveDefense)
INHERIT(ecl::StatusConsumeBase)
P(Charges)
P(TargetPos)
P_RO(TargetHandle)
P(TargetSize)
P_RO(EffectHandle)
END_CLS()


BEGIN_CLS(ecl::StatusSpark)
INHERIT(ecl::StatusConsumeBase)
P(Charges)
END_CLS()


BEGIN_CLS(ecl::StatusAoO)
INHERIT(ecl::Status)
P_RO(SourceHandle)
P_RO(TargetHandle)
P(ShowOverhead)
END_CLS()


BEGIN_CLS(ecl::StatusBoost)
INHERIT(ecl::Status)
P(BoostId)
END_CLS()


BEGIN_CLS(ecl::StatusClean)
INHERIT(ecl::Status)
P_REF(EffectHandler)
P(OverlayMaterial)
END_CLS()


BEGIN_CLS(ecl::StatusCombat)
INHERIT(ecl::Status)
P_REF(CombatTeamId)
END_CLS()


BEGIN_CLS(ecl::StatusHeal)
INHERIT(ecl::Status)
P(EffectTime)
P(HealAmount)
P(TargetDependentHeal)
P_REF(TargetDependentHealAmount)
P(HealEffect)
P(HealEffectId)
P(HealType)
P_REF(EffectHandler)
END_CLS()


BEGIN_CLS(ecl::StatusHit)
INHERIT(ecl::Status)
P(TimeRemaining)
P(DeleteRequested)
P_REF(DamageInfo)
P_RO(HitByHandle)
P(HitByType)
P_RO(WeaponHandle)
P(HitReason)
P(Interruption)
P(AllowInterruptAction_IsIdleAction)
P(ImpactPosition)
P(ImpactDirection)
END_CLS()


BEGIN_CLS(ecl::StatusIdentify)
INHERIT(ecl::Status)
P(Level)
P_RO(Identifier)
END_CLS()


BEGIN_CLS(ecl::StatusKnockedDown)
INHERIT(ecl::Status)
P_RO(KnockedDownState)
P_REF(EffectHandler)
END_CLS()


BEGIN_CLS(ecl::StatusSitting)
INHERIT(ecl::Status)
P_RO(ItemHandle)
P_RO(NetId)
P(Position)
P(Index)
END_CLS()


BEGIN_CLS(ecl::StatusRepair)
INHERIT(ecl::Status)
P_RO(Repaired)
P_RO(RepairerHandle)
END_CLS()


BEGIN_CLS(ecl::StatusRotate)
INHERIT(ecl::Status)
P(Yaw)
END_CLS()


BEGIN_CLS(ecl::StatusSmelly)
INHERIT(ecl::Status)
P_REF(EffectHandler)
P(OverlayMaterial)
END_CLS()


BEGIN_CLS(ecl::StatusSneaking)
INHERIT(ecl::Status)
P(ClientRequestStop)
P(Failed)
P_REF(EffectHandler)
END_CLS()


BEGIN_CLS(ecl::StatusSummoning)
INHERIT(ecl::Status)
P(AnimationDuration)
P(SummonLevel)
END_CLS()


BEGIN_CLS(ecl::StatusTeleportFall)
INHERIT(ecl::Status)
P(Target)
P(ReappearTime)
P(SkillId)
P(HasDamage)
P_REF(EffectHandler)
END_CLS()


BEGIN_CLS(ecl::StatusThrown)
INHERIT(ecl::Status)
P(Level)
P(AnimationDuration)
P(IsThrowingSelf)
P(LandingEstimate)
P(Landed)
END_CLS()


BEGIN_CLS(ecl::StatusUnlock)
INHERIT(ecl::Status)
P(Unlocked)
END_CLS()


BEGIN_CLS(ecl::StatusUnsheathed)
INHERIT(ecl::Status)
P(Force)
END_CLS()


BEGIN_CLS(ecl::StatusMaterial)
INHERIT(ecl::Status)
P(MaterialUUID)
P(ApplyFlags)
P(IsOverlayMaterial)
P(Fading)
P(ApplyNormalMap)
P(Force)
P(HasVisuals)
END_CLS()


BEGIN_CLS(ecl::StatusSpirit)
INHERIT(ecl::Status)
P_REF(Characters)
END_CLS()


BEGIN_CLS(ecl::StatusClimbing)
INHERIT(ecl::Status)
P_RO(LadderHandle)
P(UpdateTimer)
P(Direction)
P(Started)
P(JumpUpLadders)
P(NeedsCharacterUpdate)
P(ClimbLoopDuration)
END_CLS()


BEGIN_CLS(ecl::StatusEffect)
INHERIT(ecl::Status)
P(Active)
P(PlayerSameParty)
P(PeaceOnly)
P(PlayerHasTag)
P(Icon)
P(StatusEffect)
P_REF(EffectHandler)
P(BeamEffect)
// P_REF(BeamHandler)
END_CLS()


BEGIN_CLS(ecl::StatusTutorialBed)
INHERIT(ecl::Status)
P_RO(BedHandle)
P(AnimationDuration)
P(AnimationDuration2)
END_CLS()


BEGIN_CLS(ecl::StatusMachine)
P_RO(IsStatusMachineActive)
P_RO(PreventStatusApply)
P_REF(Statuses)
P_RO(OwnerObjectHandle)
END_CLS()
