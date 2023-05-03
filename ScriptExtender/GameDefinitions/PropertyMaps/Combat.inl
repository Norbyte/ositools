BEGIN_CLS(eoc::CombatTeamId)
P_GETTER(TeamId, GetTeamId)
P_GETTER(CombatId, GetCombatId)
P_RO(CombinedId)
END_CLS()


BEGIN_CLS(eoc::CombatData)
P_REF(HasBeenHitBy)
P(Flags)
P_REF(HasHadStatus)
END_CLS()


BEGIN_CLS(eoc::CombatComponent)
INHERIT(BaseComponent)
P_REF(CombatData)
P(Alignment1)
P(Alignment2)
P(CombatGroupId)
P_REF(CombatAndTeamIndex)
P(Initiative)
P(HasAttackOfOpportunity)
P(Flags)
P_BITMASK(Flags)
END_CLS()


BEGIN_CLS(esv::CombatComponent)
INHERIT(eoc::CombatComponent)
END_CLS()


BEGIN_CLS(ecl::CombatComponent)
INHERIT(eoc::CombatComponent)
END_CLS()


BEGIN_CLS(esv::TurnManager)
P_REF(AttachedCombatComponents)
P_REF(CombatEntities)
P_REF(CombatEntities2)
P_REF(EntityWrappers)
P_REF(EntitiesLeftCombat)
P(TeamMode)
P_REF(Combats)
P_REF(FreeCombatIds)
P_RO(NextCombatId)
P_REF(TimeoutOverrides)
P_REF(CombatGroups)
P_REF(CombatParticipants)
END_CLS()


BEGIN_CLS(esv::TurnManager::TimeoutOverride)
P_REF(Handle)
P(Timeout)
END_CLS()


BEGIN_CLS(esv::TurnManager::Combat)
P_REF(CurrentRoundTeams)
P_REF(NextRoundTeams)
P_REF(CombatGroups)
P_REF(CurrentTurnChangeNotificationTeamIds)
P_REF(NextTurnChangeNotificationTeamIds)
P_REF(Teams)
P_RO(NextTeamId)
P_REF(WaitingForCharComponents)
P(InitialPlayerHandle)
P(InitialEnemyHandle)
P(LevelName)
P(TimeSpentInTurn)
P(TimeLeft_M)
P(TimeSpentTryingToEndTurn)
P(TurnTimer_M)
P(HasParticipantSurfacesNumTicks)
P(CombatRound)
P(IsActive)
P(HasParticipantSurfaces)
P(CombatStartEventSent)
P(NewRound)
P(IsFightBetweenPlayers)

P_FUN(GetCurrentTurnOrder, LuaGetCurrentTurnOrder)
P_FUN(GetNextTurnOrder, LuaGetNextTurnOrder)
P_FUN(UpdateCurrentTurnOrder, LuaUpdateCurrentTurnOrder)
P_FUN(UpdateNextTurnOrder, LuaUpdateNextTurnOrder)
P_FUN(GetAllTeams, LuaGetAllTeams)
END_CLS()


BEGIN_CLS(esv::TurnManager::CombatTeam)
P_REF(Id)
P(Initiative)
P(CombatTeamRound)
P(StillInCombat)
P(AddedNextTurnNotification)
P_REF(CombatGroup)
P_REF(EntityWrapper)
P_REF(ComponentHandle)

P_GETTER(TeamId, GetTeamId)
P_GETTER(CombatId, GetCombatId)
P_GETTER(Character, GetCharacter)
P_GETTER(Item, GetItem)
END_CLS()


BEGIN_CLS(esv::TurnManager::CombatGroup)
P_REF(CombatTeamsOrdered)
P(Initiative)
P(Party)
P(LastAddedTeamIndex)
END_CLS()


BEGIN_CLS(esv::TurnManager::EntityWrapper)
P(Handle)
P_REF(CombatComponentPtr)
P_REF(Character)
P_REF(Item)
END_CLS()


BEGIN_CLS(eoc::CombatGroupInfo)
P_RO(Id)
P(Name)
END_CLS()


BEGIN_CLS(ecl::TurnManager::CombatOrderChange)
P(TeamId)
P(OrderIndex)
END_CLS()


BEGIN_CLS(ecl::TurnManager::Combat)
P_REF(CurrentRoundTeams)
P_REF(NextRoundTeams)
P_REF(CombatGroups)
P_REF(Teams)
P_REF(InitialPlayerTeamId)
P_REF(InitialEnemyTeamId)
P(LevelName)
P(TurnTimer)
P_REF(OrderChanges)
END_CLS()


BEGIN_CLS(ecl::TurnManager)
P_REF(EntityHandles)
P_REF(Combats)
P_REF(CombatGroups)
P_REF(PlayerIdToCombatantNetId)
P(CameraControl)
END_CLS()

