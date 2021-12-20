BEGIN_CLS(eoc::Ai)
P_REF(AiGrid)
P_REF(GameObject)
P_RO(AiBoundType)
P_RO(AIBoundsHeight)
P_RO(AIBoundsRadius)
P_RO(AIBoundsMin)
P_RO(AIBoundsMax)
P_RO(AIBoundsSize)
P_RO(XZ)
P_RO(Settings)
P_RO(AiFlags)
P_RO(AiFlags2)
P_RO(UseOnDistance)
P_RO(MyHandle)
END_CLS()


BEGIN_CLS(eoc::PlayerCustomData)
P_RO(CustomLookEnabled)
P(Name)
P(NameTranslated)
P(ClassType)
P(SkinColor)
P(HairColor)
P(ClothColor1)
P(ClothColor2)
P(ClothColor3)
P(IsMale)
P(Race)
P(OriginName)
P(Icon)
P(MusicInstrument)
P_RO(OwnerProfileID)
P_RO(ReservedProfileID)
P(AiPersonality)
P_RO(Speaker)
END_CLS()


BEGIN_CLS(eoc::PlayerUpgrade)
// No need to expose Stats again, it is already available from the character object
// P_REF(Stats)
P(AttributePoints)
P(CombatAbilityPoints)
P(CivilAbilityPoints)
P(TalentPoints)
P_REF(Attributes)
P_REF(Abilities)
// FIXME - TalentArray - P_REF(Talents)
P_REF(Traits)
P(IsCustom)
END_CLS()


BEGIN_CLS(esv::SkillConditions)
P(MinimumHealthPercentage)
P(MaximumHealthPercentage)
P(HasNoPhysicalArmor)
P(HasNoMagicalArmor)
P(Tags)
END_CLS()


BEGIN_CLS(esv::SkillInfo)
P(ScoreModifier)
P(StartRound)
P(MinimumImpact)
P(OnlyCastOnSelf)
P(AIFlags)
P_REF(SourceConditions)
P_REF(TargetConditions)
P(CasualExplorer)
P(Classic)
P(TacticianHardcore)
P(HonorHardcore)
END_CLS()


BEGIN_CLS(esv::Skill)
P_RO(NetID)
P_REF(Info)
P_REF(CauseList)
P_RO(OwnerHandle)
P_RO(SkillId)
P(ActiveCooldown)
P(IsActivated)
P(IsLearned)
P(ZeroMemory)
P(OncePerCombat)
P(NumCharges)
END_CLS()


BEGIN_CLS(esv::SkillBarItem)
P_RO(Type)
P_RO(SkillOrStatId)
P_RO(ItemHandle)
END_CLS()


BEGIN_CLS(esv::SkillManager)
P_RO(OwnerHandle)
P_REF(Skills)
P_REF(TimeItemAddedToSkillManager)
P_RO(IsLoading)
P_RO(FreeMemorySlots)
END_CLS()


BEGIN_CLS(esv::PlayerCustomData)
INHERIT(eoc::PlayerCustomData)
END_CLS()


BEGIN_CLS(esv::PlayerData)
P_RO(SomeObjectHandle)
P_REF(SkillBar)
P_REF(LockedAbility)
// ShapeShiftVariableManagers
// ShapeShiftAttitudeMaps
P(LevelUpMarker)
P(SelectedSkillSetIndex)
P(QuestSelected)
P_REF(CustomData)
// FIXME - RefMap - P_REF(PreviousPickpocketTargets)
P(PickpocketTarget)
P_REF(PreviousPositions)
P_RO(PreviousPositionId)
P(HelmetOption)
P(ShouldReevaluateSkillBar)
P(Renown)
P(CachedTension)
P(RecruiterHandle)
P(OriginalTemplate)
P(Region)
END_CLS()


BEGIN_CLS(esv::Character)
INHERIT(IEoCServerObject)
P_RO(WorldPos)
P_RO(Flags)

#if defined(GENERATING_TYPE_INFO)
for (auto const& label : EnumInfo<esv::CharacterFlags>::Values) {
	ADD_TYPE(label.Key, bool)
}
ADD_TYPE("PlayerCustomData", eoc::PlayerCustomData)
#endif

#if defined(GENERATING_PROPMAP)
for (auto const& label : EnumInfo<esv::CharacterFlags>::Values) {
	pm.AddRawProperty(label.Key.GetString(),
		&(GenericGetOffsetBitmaskFlag<esv::CharacterFlags>),
		&CharacterSetFlag,
		offsetof(esv::Character, Flags),
		(uint64_t)label.Value
	);
}

pm.AddProperty("PlayerCustomData",
	[](lua_State* L, LifetimeHolder const& lifetime, esv::Character* obj, std::size_t offset, uint64_t flag) {
		if (obj->PlayerData) {
			MakeObjectRef(L, &obj->PlayerData->CustomData);
		} else {
			push(L, nullptr);
		}
		return true;
	}
);
#endif

/*
// FIXME Make dangerous flags read-only
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
*/

P_RO(CurrentLevel)
// FIXME - no mat3 serializer yet! P_RO(WorldRot)
// Available via IGameObject
// P_RO(Scale)
P_REF(CurrentTemplate)
P_REF(OriginalTemplate)
P_REF(TemplateUsedForSkills)
P_RO(Flags2)
P_BITMASK(Flags2)
P_RO(Flags3)
P_BITMASK(Flags3)
P_RO(FlagsEx)
P_RO(Team)
P_RO(Color)
P_REF(AI)
P(AnimationOverride)
P(WalkSpeedOverride)
P(RunSpeedOverride)
P_REF(VoiceSet)
P_RO(NeedsUpdateCount)
P_RO(ScriptForceUpdateCount)
P_RO(ForceSynchCount)
P_REF(Stats)
// FIXME - convert to a TypedHandle? / add resolve mechanism
P_RO(InventoryHandle)
// FIXME - refs to all state machines 
// P_REF(ActionMachine)
// P_REF(SteeringMachine)
// P_REF(BehaviourMachine)
// P_REF(CharacterSupervisor)
// P_REF(NetworkController)
// P_REF(OsirisController)
// P_REF(RequestController)
// P_REF(StatusController)
// P_REF(ScriptController)
// P_REF(DialogController)
P_REF(StatusMachine)
P_REF(SkillManager)
// P_REF(VariableManager)
// P_REF(ShapeShiftVariableManager)
// P_REF(Attitudes)
P_RO(SkillBeingPrepared)
P_RO(Dialog)
P_RO(IsDialogAiControlled)
P(LifeTime)
P(TurnTimer)
P_RO(TriggerTrapsTimer)
P_RO(UserID)
P_RO(ReservedUserID)
P_RO(OwnerHandle)
P_RO(FollowCharacterHandle)
P_RO(EnemyCharacterHandle)
P_RO(SpiritCharacterHandle)
P_RO(CorpseCharacterHandle)
P_RO(ObjectHandle6)
P_REF(EnemyHandles)
// P_RO(SurfacePathInfluenceSet)
P_REF(SummonHandles)
P(PartialAP)
P(AnimType)
P_RO(DelayDeathCount)
P_REF(DelayedDyingStatus)
P_REF(RegisteredTriggerHandles)
P_REF(RegisteredTriggers)
P_REF(PlayerData)
P_REF(PlayerUpgrade)
P_RO(ServerControlRefCount)
P(NoiseTimer)
P(CustomDisplayName)
// P(StoryDisplayName)
P(OriginalTransformDisplayName)
P(AnimationSetOverride)
P(PartyHandle)
P_REF(CreatedTemplateItems)
P_REF(Treasures)
P(CustomTradeTreasure)
P_REF(Tags)
P_RO(CrimeHandle)
P_RO(PreviousCrimeHandle)
P_RO(CrimeState)
P_RO(PreviousCrimeState)
P(IsAlarmed)
P(CrimeWarningsEnabled)
P(CrimeInterrogationEnabled)
P(InvestigationTimer)
P_REF(DisabledCrime)
P(DamageCounter)
P(HealCounter)
P(KillCounter)
P_RO(MovingCasterHandle)
P(Archetype)
P(EquipmentColor)
P(ProjectileTemplate)
P(TimeElapsed)
P_REF(PreferredAiTarget)
// FIXME - P_REF(CharacterBody)
P_REF(StatusesFromItems)
P_REF(TagsFromItems)
// FIXME - VisualSetIndices
P_RO(ReadyCheckBlocked)
P(CorpseLootable)
P(NumConsumables)
P(CustomBloodSurface)
P_RO(PreviousLevel)

// v55 compatibility
PN_REF(RootTemplate, CurrentTemplate)
// FIXME - PlayerCustomData - PlayerData->CustomData
// if (character->PlayerData != nullptr
// && character->PlayerData->CustomData.Initialized) {

P_FUN(GetInventoryItems, GetInventoryItemGuids)
P_FUN(GetSkills, GetSkillIds)
P_FUN(GetSkillInfo, GetSkillInfo)
P_FUN(GetNearbyCharacters, GetNearbyCharacters)
P_FUN(GetSummons, GetSummonGuids)
P_FUN(GetCustomStat, GetCustomStatValue)
P_FUN(SetCustomStat, SetCustomStatValue)
// FIXME - 
// SetScale

END_CLS()



BEGIN_CLS(ecl::PlayerCustomData)
INHERIT(eoc::PlayerCustomData)
END_CLS()

BEGIN_CLS(ecl::PlayerData)
// P_REF(SomeObjectHandle)
P_REF(LockedAbilities)
P_RO(SelectedSkillSet)
P_REF(CustomData)
P_RO(PickpocketTargetNetID)
P_RO(CorpseLootTargetNetID)
P_RO(HelmetOptionState)
P_RO(ArmorOptionState)
P_RO(CachedTension)
P_RO(QuestSelected)
P_REF(MemorisedSkills)
P_RO(OriginalTemplate)
P_RO(Region)
END_CLS()

BEGIN_CLS(ecl::Character)
INHERIT(IEoCClientObject)
P_RO(WorldPos)
P_RO(Flags)
P_RO(CurrentLevel)
// Available via IGameObject
// P_RO(Scale)
// P_RO(Velocity)
P_RO(UserID)
P_REF(CurrentTemplate)
P_REF(OriginalTemplate)
P_REF(Stats)
P_RO(InventoryHandle)
P_REF(StatusMachine)
P_RO(OwnerCharacterHandle)
P_RO(CorpseCharacterHandle)
P_REF(PlayerData)
P_REF(PlayerUpgrade)
// FIXME - P_REF(SurfacePathInfluences)
P_REF(Tags)
P_RO(FollowCharacterHandle)
P_RO(DisplayNameOverride)
P_RO(StoryDisplayName)
P_RO(OriginalDisplayName)
P_RO(TalkingIconEffect)
P_RO(AnimationSetOverride)
P_RO(WalkSpeedOverride)
P_RO(RunSpeedOverride)
P_RO(Archetype)
P_REF(ItemTags)
P_RO(CorpseLootable)

#if defined(GENERATING_TYPE_INFO)
ADD_TYPE("PlayerCustomData", eoc::PlayerCustomData)
#endif

#if defined(GENERATING_PROPMAP)
pm.AddProperty("PlayerCustomData",
	[](lua_State* L, LifetimeHolder const& lifetime, ecl::Character* obj, std::size_t offset, uint64_t flag) {
		if (obj->PlayerData) {
			MakeObjectRef(L, &obj->PlayerData->CustomData);
		} else {
			push(L, nullptr);
		}
		return true;
	}
);
#endif

// v55 compatibility
PN_REF(RootTemplate, OriginalTemplate)

P_FUN(GetInventoryItems, GetInventoryItemGuids)
P_FUN(GetItemBySlot, GetItemGuidBySlot)
P_FUN(GetItemObjectBySlot, GetItemBySlot)
P_FUN(GetCustomStat, GetCustomStatValue)
END_CLS()
