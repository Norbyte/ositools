BEGIN_CLS(eoc::PlayerCustomData)
P_RO(CustomLookEnabled)
P(Name)
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


BEGIN_CLS(esv::Character)
// EoCServerObject
P_RO(NetID)
P_RO(MyGuid)
// Character
P_RO(WorldPos)
P_RO(CurrentLevel)
P(Flags)
P(Flags2)
P(Flags3)

/*
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

EnumInfo<esv::CharacterFlags>::Values.Iterate([&propertyMap](auto const& name, auto const& id) {
	auto& flag = propertyMap.Flags[name];
	if (flag.Flags & kPropWrite) {
		flag.Set = [id](void* obj, bool value) -> bool {
			auto ch = reinterpret_cast<esv::Character*>(obj);
			if (value) {
				ch->SetFlags((uint64_t)id);
			}
			else {
				ch->ClearFlags((uint64_t)id);
			}
			return true;
		};
	}
	});
*/

P_RO(Scale)
P(AnimationOverride)
P(WalkSpeedOverride)
P(RunSpeedOverride)
P_RO(NeedsUpdateCount)
P_RO(ScriptForceUpdateCount)
P_RO(ForceSynchCount)
P_RO(InventoryHandle)
P_RO(SkillBeingPrepared)
P(LifeTime)
P(TurnTimer)
P_RO(TriggerTrapsTimer)
P(UserID)
P(ReservedUserID)
P_RO(OwnerHandle)
P_RO(FollowCharacterHandle)
P_RO(SpiritCharacterHandle)
P_RO(CorpseCharacterHandle)
P(PartialAP)
P(AnimType)
P_RO(DelayDeathCount)
P(AnimationSetOverride)
P_RO(OriginalTransformDisplayName)
P_RO(PartyHandle)
P(CustomTradeTreasure)
P(IsAlarmed)
P(CrimeWarningsEnabled)
P(CrimeInterrogationEnabled)
P_RO(MovingCasterHandle)
P(Archetype)
P(EquipmentColor)
P(ProjectileTemplate)
P_RO(ReadyCheckBlocked)
P(CorpseLootable)
P(CustomBloodSurface)
P_RO(PreviousLevel)
END_CLS()



BEGIN_CLS(ecl::Character)
// EoCClientObject
P_RO(NetID)
P_RO(MyGuid)
// Character
P_RO(WorldPos)
P_RO(CurrentLevel)
P_RO(Scale)
P_RO(UserID)
P_RO(OwnerCharacterHandle)
P_RO(CorpseCharacterHandle)
P_RO(FollowCharacterHandle)
P_RO(StoryDisplayName)
P_RO(OriginalDisplayName)
P_RO(AnimationSetOverride)
P_RO(WalkSpeedOverride)
P_RO(RunSpeedOverride)
P_RO(Archetype)
P_RO(CorpseLootable)
END_CLS()
