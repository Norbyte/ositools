BEGIN_CLS(esv::ActionState)
P_GETTER(Type, LuaGetTypeId)
P_RO(TransactionId)
END_CLS()


BEGIN_CLS(esv::ASAttack)
INHERIT(esv::ActionState)
P(TargetHandle)
P(TargetPosition)
P_REF(CleaveTargets)
P(IsFinished)
P(AlwaysHit)
P(AttackAnimation)
P(TimeRemaining)
P(AnimationFinished)
P(TotalHits)
P(TotalHitOffHand)
P(TotalShoots)
P(TotalShootsOffHand)
P(HitCount)
P(HitCountOffHand)
P(ShootCount)
P(ShootCountOffHand)
P(MainWeaponHandle)
P(OffWeaponHandle)
P_REF(MainWeaponDamageList)
P_REF(OffHandDamageList)
P_REF(MainWeaponNumberDividers)
P_REF(OffHandNumberDividers)
P_REF(RangedMainHandDamage)
P_REF(RangedOffHandDamage)
P(MainHandHitType)
P(OffHandHitType)
P_REF(RangedMainHandHitObject)
P_REF(RangedOffHandHitObject)
P(ProjectileUsesHitObject)
P(ProjectileStartPosition)
P(ProjectileTargetPosition)
P(DamageDurability)
P_REF(DelayDeathCharacterHandles)
END_CLS()


BEGIN_CLS(esv::ASDie)
INHERIT(esv::ActionState)
P(DieLogicPending)
P(ExecutedDieLogic)
P(DieActionsPending)
P(PlayingDeathAnimation)
P(CauseeHandle)
END_CLS()


BEGIN_CLS(esv::ASHit)
INHERIT(esv::ActionState)
P(AnimationFlag1)
P(AnimationFlag2)
P(AnimationFlag3)
P(StillAnimation)
P(ShouldResetState)
P(HitStatusHandle)
END_CLS()


BEGIN_CLS(esv::ASIdle)
INHERIT(esv::ActionState)
END_CLS()


BEGIN_CLS(esv::ASAnimation)
INHERIT(esv::ActionState)
P(Animation)
P(AnimationTimeAfterTick)
P(AnimationTime)
P(AnimationDuration)
P(NullAnimation)
P(IsCompleted)
P(NoBlend)
P(field_2F)
P(ExitOnFinish)
END_CLS()


BEGIN_CLS(esv::ActionStateRequest)
INHERIT(esv::ActionState)
P(RequestTimeRemaining)
P_RO(RequestId)
END_CLS()


BEGIN_CLS(esv::ASPickUp)
INHERIT(esv::ActionStateRequest)
P(ItemHandle)
P(TargetItemSlot)
P(Flags)
P(NetID)
P(SplitAmount)
P(ShouldGenerateTreasure)
P(MovedItem)
P(StartPosition)
P(TargetPosition)
P(IsFinished)
P(TimeRemaining)
END_CLS()


BEGIN_CLS(esv::ASDrop)
INHERIT(esv::ActionState)
P(ItemHandle)
P(TargetPosition)
P(TargetRotation)
P(Amount)
P(TimeRemaining)
P(IsFinished)
END_CLS()


BEGIN_CLS(esv::ASMoveItem)
INHERIT(esv::ActionStateRequest)
P_REF(Items)
P_REF(Positions)
P_REF(Rotations)
P_REF(Timers)
P(Amount)
P(IsFinished)
P(ItemMoveFinished)
END_CLS()


BEGIN_CLS(esv::ASPrepareSkill)
INHERIT(esv::ActionState)
P(SkillId)
P(PrepareAnimationInit)
P(PrepareAnimationLoop)
P(IsFinished)
P(IsEntered)
P(AnimApplyPending)
END_CLS()


BEGIN_CLS(esv::ASUseSkill)
INHERIT(esv::ActionState)
P_REF(OriginalSkill)
P_REF(Skill)
END_CLS()


BEGIN_CLS(esv::ASKnockedDown)
INHERIT(esv::ActionState)
P(KnockedDownState)
P(AnimationApplied)
END_CLS()


BEGIN_CLS(esv::ASSummoning)
INHERIT(esv::ActionState)
P(IsFinished)
END_CLS()


BEGIN_CLS(esv::ASPolymorphing)
INHERIT(esv::ActionState)
P(IsFinished)
END_CLS()


BEGIN_CLS(esv::ASUseItem)
INHERIT(esv::ActionStateRequest)
P(ItemHandle)
P(ActionTimeRemaining)
P(UseTimeRemaining)
P(UseCompleted)
P(IgnoreAP)
P(RequestStarted)
P(Slot)
END_CLS()


BEGIN_CLS(esv::ASCombineItem)
INHERIT(esv::ActionStateRequest)
P(CraftingStation)
P_REF(Ingredients)
P(Recipe)
P(AnimationTime)
P(ConsumeAP)
P(OpenUI)
P(IsFinished)
END_CLS()


BEGIN_CLS(esv::ASTeleportFall)
INHERIT(esv::ActionState)
P(HitTimeRemaining)
P(AnimationTimeRemaining)
P(IsFinished)
P(IsFromItem)
P(Height)
P(Acceleration)
P(HitApplied)
P(SkillId)
END_CLS()


BEGIN_CLS(esv::ASSheath)
INHERIT(esv::ActionState)
P(IsFinished)
END_CLS()


BEGIN_CLS(esv::ASUnsheath)
INHERIT(esv::ActionState)
P(IsFinished)
END_CLS()


BEGIN_CLS(esv::ASIdentify)
INHERIT(esv::ActionState)
P(TargetItem)
P(SourceItem)
P(AnimationTimeRemaining)
P(IsFinished)
END_CLS()


BEGIN_CLS(esv::ASRepair)
INHERIT(esv::ActionState)
P(TargetItem)
P(SourceItem)
P(AnimationTimeRemaining)
P(IsFinished)
END_CLS()


BEGIN_CLS(esv::ASLockpick)
INHERIT(esv::ActionStateRequest)
P(TargetItem)
P(SourceItem)
P(AnimationTimeRemaining)
P(IsFinished)
END_CLS()


BEGIN_CLS(esv::ASDisarmTrap)
INHERIT(esv::ActionState)
P(TargetItem)
P(SourceItem)
P(IsFinished)
P(AnimationTimeRemaining)
END_CLS()


BEGIN_CLS(esv::ASIncapacitated)
INHERIT(esv::ActionState)
END_CLS()


BEGIN_CLS(esv::ASJumpFlight)
INHERIT(esv::ActionState)
P(Animation1Duration)
P(Animation2Duration)
P(Animation1Started)
P(Animation2Started)
P(IsFinished)
END_CLS()


BEGIN_CLS(esv::ActionMachine::Layer)
P_REF(State)
END_CLS()


BEGIN_CLS(esv::ActionMachine)
P_RO(CharacterHandle)
P_REF(Layers)
END_CLS()


BEGIN_CLS(ecl::ActionState)
P_GETTER(Type, LuaGetTypeId)
P_RO(TransactionId)
END_CLS()


BEGIN_CLS(ecl::ActionMachine::Layer)
P_REF(State)
END_CLS()


BEGIN_CLS(ecl::ActionMachine)
P_RO(CharacterHandle)
P_REF(Layers)
END_CLS()
