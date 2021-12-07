BEGIN_CLS(esv::Status)
P_RO(NetID)
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


#if defined(GENERATING_PROPMAP)
pm.AddProperty("StatusType",
	[](lua_State* L, LifetimeHolder const& lifetime, esv::Status* status, std::size_t offset, uint64_t flag) {
		push(L, status->GetStatusId());
		return true;
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
P_RO(OwnerTeamId)
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
P(StatusTargetHandle)
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


BEGIN_CLS(ecl::Status)
P_RO(NetID)
P_RO(OwnerHandle)
P_RO(StatusId)
P_RO(LifeTime)
P_RO(CurrentLifeTime)
P_RO(StatsMultiplier)
P_RO(Flags)
P_RO(StatusSourceHandle)

#if defined(GENERATING_PROPMAP)
pm.AddProperty("StatusType",
	[](lua_State* L, LifetimeHolder const& lifetime, ecl::Status* status, std::size_t offset, uint64_t flag) {
		push(L, status->GetStatusId());
		return true;
	}
);
#endif
END_CLS()
