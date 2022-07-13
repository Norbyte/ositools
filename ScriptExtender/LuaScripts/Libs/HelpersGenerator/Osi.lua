---@diagnostic disable
local text = [[--#region Osiris Calls

--- @param Character string
--- @param Talent string
function Osi.CharacterAddTalent(Character, Talent) end
	
--- @param Character string
--- @param Talent string
function Osi.CharacterRemoveTalent(Character, Talent) end
	
--- @param Character string
function Osi.CharacterFreeze(Character) end
	
--- @param Character string
function Osi.CharacterUnfreeze(Character) end
	
--- @param Trigger string
--- @param TemplateId string
--- @param PlaySpawn integer
function Osi.CharacterCreateAtTrigger(Trigger, TemplateId, PlaySpawn) end
	
--- @param Trigger string
--- @param TemplateId string
--- @param PlaySpawn integer
function Osi.TemporaryCharacterCreateAtTrigger(Trigger, TemplateId, PlaySpawn) end
	
--- @param Character string
--- @param Message string
function Osi.OpenMessageBox(Character, Message) end
	
--- @param Character string
--- @param Message string
function Osi.OpenMessageBoxYesNo(Character, Message) end
	
--- @param Character string
--- @param Message string
--- @param Choice_1 string
--- @param Choice_2 string
function Osi.OpenMessageBoxChoice(Character, Message, Choice_1, Choice_2) end
	
--- @param Character string
function Osi.ShowCredits(Character) end
	
--- @param Character string
--- @param MaxRange number
--- @param Event string
function Osi.TeleportToRandomPosition(Character, MaxRange, Event) end
	
--- @param SourceObject string
--- @param X number
--- @param Y number
--- @param Z number
--- @param Event string
--- @param TeleportLinkedCharacters integer
--- @param ExcludePartyFollowers integer
function Osi.TeleportToPosition(SourceObject, X, Y, Z, Event, TeleportLinkedCharacters, ExcludePartyFollowers) end
	
--- @param SourceObject string
--- @param TargetObject string
--- @param Event string
--- @param TeleportLinkedCharacters integer
--- @param ExcludePartyFollowers integer
--- @param LeaveCombat integer
function Osi.TeleportTo(SourceObject, TargetObject, Event, TeleportLinkedCharacters, ExcludePartyFollowers, LeaveCombat) end
	
--- @param Character string
--- @param X number
--- @param Y number
--- @param Z number
--- @param Running integer
--- @param Event string
function Osi.CharacterMoveToPosition(Character, X, Y, Z, Running, Event) end
	
--- @param Character string
--- @param Target string
--- @param Running integer
--- @param Event string
--- @param IncreaseSpeed integer
function Osi.CharacterMoveTo(Character, Target, Running, Event, IncreaseSpeed) end
	
--- @param Character string
--- @param Trigger string
--- @param SnapToTarget integer
function Osi.CharacterLookFromTrigger(Character, Trigger, SnapToTarget) end
	
--- @param Character string
--- @param Item string
function Osi.CharacterEquipItem(Character, Item) end
	
--- @param Character string
--- @param ToCharacter string
function Osi.TransferItemsToCharacter(Character, ToCharacter) end
	
--- @param Character string
function Osi.TransferItemsToParty(Character) end
	
--- @param Character string
function Osi.TransferItemsToUser(Character) end
	
--- @param Character string
--- @param Item string
function Osi.CharacterUnequipItem(Character, Item) end
	
--- @param Character string
--- @param ToCharacter string
function Osi.CharacterFollowCharacter(Character, ToCharacter) end
	
--- @param Character string
function Osi.CharacterStopFollow(Character) end
	
--- @param Character string
function Osi.CharacterClearFollow(Character) end
	
--- @param Trigger string
--- @param Event string
--- @param Movie string
function Osi.CharacterTeleportPartiesToTriggerWithMovie(Trigger, Event, Movie) end
	
--- @param Trigger string
--- @param Event string
function Osi.CharacterTeleportPartiesToTriggerWithMovieRequestCallback(Trigger, Event) end
	
--- @param UserId integer
--- @param Movie string
function Osi.CharacterSetTeleportMovie(UserId, Movie) end
	
--- @param Trigger string
--- @param Event string
function Osi.CharacterTeleportPartiesToTrigger(Trigger, Event) end
	
--- @param Character string
function Osi.CharacterClearTradeGeneratedItems(Character) end
	
--- @param Character string
--- @param Treasure string
function Osi.CharacterSetCustomTradeTreasure(Character, Treasure) end
	
--- @param Player string
--- @param Trader string
function Osi.GenerateItems(Player, Trader) end
	
--- @param Player string
--- @param Treasure string
--- @param Identified integer
function Osi.CharacterGiveReward(Player, Treasure, Identified) end
	
--- @param Player string
--- @param Quest string
--- @param RewardState string
function Osi.CharacterGiveQuestReward(Player, Quest, RewardState) end
	
--- @param Character string
--- @param GenerateTreasure integer
--- @param DeathType string
--- @param Source string
function Osi.CharacterDie(Character, GenerateTreasure, DeathType, Source) end
	
--- @param Character string
--- @param GenerateTreasure integer
--- @param DeathType string
--- @param Source string
function Osi.CharacterDieImmediate(Character, GenerateTreasure, DeathType, Source) end
	
--- @param Character string
--- @param SurfaceType string
function Osi.CharacterSetCustomBloodSurface(Character, SurfaceType) end
	
--- @param Character string
--- @param Skill string
--- @param ShowNotification integer
function Osi.CharacterAddSkill(Character, Skill, ShowNotification) end
	
--- @param Character string
--- @param Skill string
function Osi.CharacterRemoveSkill(Character, Skill) end
	
--- @param Character string
--- @param Amount integer
function Osi.CharacterAddAttributePoint(Character, Amount) end
	
--- @param Character string
--- @param Amount integer
function Osi.CharacterAddAbilityPoint(Character, Amount) end
	
--- @param Character string
--- @param Amount integer
function Osi.CharacterAddCivilAbilityPoint(Character, Amount) end
	
--- @param Character string
--- @param Amount integer
function Osi.CharacterAddActionPoints(Character, Amount) end
	
--- @param Character string
--- @param Amount integer
function Osi.CharacterAddTalentPoint(Character, Amount) end
	
--- @param Character string
function Osi.CharacterResurrect(Character) end
	
--- @param Character string
function Osi.CharacterResurrectAndResetXPReward(Character) end
	
--- @param Character string
--- @param ResurrectAnimation string
function Osi.CharacterResurrectCustom(Character, ResurrectAnimation) end
	
--- @param Character string
--- @param Count integer
function Osi.CharacterAddGold(Character, Count) end
	
--- @param Character string
--- @param Count integer
function Osi.PartyAddGold(Character, Count) end
	
--- @param Character string
--- @param Count integer
function Osi.UserAddGold(Character, Count) end
	
--- @param Character string
--- @param Id string
function Osi.CharacterIncreaseSocialStat(Character, Id) end
	
--- @param Character string
--- @param Id string
function Osi.CharacterDecreaseSocialStat(Character, Id) end
	
--- @param Character string
--- @param Spectating integer
function Osi.CharacterSetSpectating(Character, Spectating) end
	
--- @param Character string
--- @param Text string
function Osi.CharacterSetCustomName(Character, Text) end
	
--- @param Character string
--- @param PlaySpawn integer
--- @param Event string
function Osi.CharacterAppear(Character, PlaySpawn, Event) end
	
--- @param Character string
--- @param Animation string
--- @param Event string
function Osi.CharacterAppearCustom(Character, Animation, Event) end
	
--- @param Character string
--- @param Target string
--- @param PlaySpawn integer
--- @param Event string
function Osi.CharacterAppearAt(Character, Target, PlaySpawn, Event) end
	
--- @param Character string
--- @param Target string
--- @param Animation string
--- @param Event string
function Osi.CharacterAppearAtCustom(Character, Target, Animation, Event) end
	
--- @param Character string
--- @param X number
--- @param Y number
--- @param Z number
--- @param PlaySpawn integer
--- @param Event string
function Osi.CharacterAppearAtPosition(Character, X, Y, Z, PlaySpawn, Event) end
	
--- @param Character string
--- @param X number
--- @param Y number
--- @param Z number
--- @param Animation string
--- @param Event string
function Osi.CharacterAppearAtPositionCustom(Character, X, Y, Z, Animation, Event) end
	
--- @param Character string
--- @param Target string
--- @param Angle integer
--- @param PlaySpawn integer
--- @param Event string
function Osi.CharacterAppearOutOfSightTo(Character, Target, Angle, PlaySpawn, Event) end
	
--- @param Character string
--- @param Target string
--- @param Angle integer
--- @param Animation string
--- @param Event string
function Osi.CharacterAppearOutOfSightToCustom(Character, Target, Angle, Animation, Event) end
	
--- @param Character string
--- @param Target string
--- @param Object string
--- @param PlaySpawn integer
--- @param Event string
function Osi.CharacterAppearOutOfSightToObject(Character, Target, Object, PlaySpawn, Event) end
	
--- @param Character string
--- @param Target string
--- @param Object string
--- @param Animation string
--- @param Event string
function Osi.CharacterAppearOutOfSightToObjectCustom(Character, Target, Object, Animation, Event) end
	
--- @param Character string
--- @param Target string
--- @param Angle integer
--- @param PlaySpawn integer
--- @param Event string
function Osi.CharacterAppearOnTrailOutOfSightTo(Character, Target, Angle, PlaySpawn, Event) end
	
--- @param Character string
--- @param Target string
--- @param Angle integer
--- @param Animation string
--- @param Event string
function Osi.CharacterAppearOnTrailOutOfSightToCustom(Character, Target, Angle, Animation, Event) end
	
--- @param Character string
--- @param Target string
--- @param Object string
--- @param PlaySpawn integer
--- @param Event string
function Osi.CharacterAppearOnTrailOutOfSightToObject(Character, Target, Object, PlaySpawn, Event) end
	
--- @param Character string
--- @param Target string
--- @param Object string
--- @param Animation string
--- @param Event string
function Osi.CharacterAppearOnTrailOutOfSightToObjectCustom(Character, Target, Object, Animation, Event) end
	
--- @param Character string
--- @param X number
--- @param Y number
--- @param Z number
--- @param Angle integer
--- @param PlaySpawn integer
--- @param Event string
function Osi.CharacterAppearAtPositionOutOfSightTo(Character, X, Y, Z, Angle, PlaySpawn, Event) end
	
--- @param Character string
--- @param X number
--- @param Y number
--- @param Z number
--- @param Angle integer
--- @param Animation string
--- @param Event string
function Osi.CharacterAppearAtPositionOutOfSightToCustom(Character, X, Y, Z, Angle, Animation, Event) end
	
--- @param Character string
--- @param X number
--- @param Y number
--- @param Z number
--- @param Object string
--- @param PlaySpawn integer
--- @param Event string
function Osi.CharacterAppearAtPositionOutOfSightToObject(Character, X, Y, Z, Object, PlaySpawn, Event) end
	
--- @param Character string
--- @param X number
--- @param Y number
--- @param Z number
--- @param Object string
--- @param Animation string
--- @param Event string
function Osi.CharacterAppearAtPositionOutOfSightToObjectCustom(Character, X, Y, Z, Object, Animation, Event) end
	
--- @param Character string
--- @param Angle integer
--- @param Running integer
--- @param Event string
--- @param IncreaseSpeed integer
function Osi.CharacterDisappearOutOfSight(Character, Angle, Running, Event, IncreaseSpeed) end
	
--- @param Character string
--- @param Object string
--- @param Running integer
--- @param Event string
--- @param IncreaseSpeed integer
function Osi.CharacterDisappearOutOfSightToObject(Character, Object, Running, Event, IncreaseSpeed) end
	
--- @param Character string
--- @param Event string
function Osi.CharacterFleeOutOfSight(Character, Event) end
	
--- @param Character string
--- @param Target string
function Osi.CharacterAttack(Character, Target) end
	
--- @param TargetCharacter string
--- @param OwnerCharacter string
function Osi.CharacterMakePlayer(TargetCharacter, OwnerCharacter) end
	
--- @param Character string
--- @param Character2 string
function Osi.CharacterRecruitCharacter(Character, Character2) end
	
--- @param UserID integer
function Osi.CharacterAssign(UserID) end
	
--- @param UserID integer
--- @param Character string
function Osi.CharacterAssignToUser(UserID, Character) end
	
--- @param Character string
--- @param Character2 string
function Osi.CharacterMakeCompanion(Character, Character2) end
	
--- @param Character string
function Osi.CharacterMakeNPC(Character) end
	
--- @param Character string
--- @param Character2 string
function Osi.CharacterAddToParty(Character, Character2) end
	
--- @param Character string
function Osi.CharacterRemoveFromParty(Character) end
	
--- @param Character string
--- @param Owner string
function Osi.CharacterAddToPlayerCharacter(Character, Owner) end
	
--- @param Character string
--- @param Owner string
function Osi.CharacterRemoveFromPlayerCharacter(Character, Owner) end
	
--- @param Character string
function Osi.CharacterRemoveAllPartyFollowers(Character) end
	
--- @param Character string
--- @param OtherCharacter string
--- @param Relation integer
function Osi.CharacterSetRelationIndivFactionToIndivFaction(Character, OtherCharacter, Relation) end
	
--- @param Character string
--- @param OtherFaction string
--- @param Relation integer
function Osi.CharacterSetRelationIndivFactionToFaction(Character, OtherFaction, Relation) end
	
--- @param Faction string
--- @param OtherCharacter string
--- @param Relation integer
function Osi.CharacterSetRelationFactionToIndivFaction(Faction, OtherCharacter, Relation) end
	
--- @param Faction string
--- @param otherFaction string
--- @param Relation integer
function Osi.CharacterSetRelationFactionToFaction(Faction, otherFaction, Relation) end
	
--- @param Character string
--- @param OtherCharacter string
function Osi.CharacterSetTemporaryHostileRelation(Character, OtherCharacter) end
	
--- @param Character string
--- @param Reaction string
--- @param Priority integer
function Osi.CharacterSetReactionPriority(Character, Reaction, Priority) end
	
--- @param Character string
--- @param Percentage number
function Osi.CharacterSetHitpointsPercentage(Character, Percentage) end
	
--- @param Character string
--- @param Percentage number
function Osi.CharacterSetArmorPercentage(Character, Percentage) end
	
--- @param Character string
--- @param Percentage number
function Osi.CharacterSetMagicArmorPercentage(Character, Percentage) end
	
--- @param Character string
--- @param Target string
--- @param SnapToTarget integer
function Osi.CharacterLookAt(Character, Target, SnapToTarget) end
	
--- @param Character string
function Osi.CharacterLevelUp(Character) end
	
--- @param Character string
--- @param Level integer
function Osi.CharacterLevelUpTo(Character, Level) end
	
--- @param Character string
--- @param XP integer
function Osi.PartyAddActualExperience(Character, XP) end
	
--- @param Character string
--- @param Act integer
--- @param ActPart integer
--- @param Gain integer
function Osi.PartyAddExperience(Character, Act, ActPart, Gain) end
	
--- @param Character string
--- @param Act integer
--- @param ActPart integer
--- @param Gain integer
function Osi.PartyAddExplorationExperience(Character, Act, ActPart, Gain) end
	
--- @param Character string
--- @param Act integer
--- @param ActPart integer
--- @param Gain integer
function Osi.CharacterAddExplorationExperience(Character, Act, ActPart, Gain) end
	
--- @param Character string
--- @param Act integer
--- @param ActPart integer
--- @param Gain integer
function Osi.PartyAddCharismaExperience(Character, Act, ActPart, Gain) end
	
--- @param Character string
--- @param Text string
function Osi.CharacterStatusText(Character, Text) end
	
--- @param Character string
--- @param Text string
function Osi.CharacterEnteredSubRegion(Character, Text) end
	
--- @param Character string
--- @param Text string
--- @param Parameter integer
function Osi.CharacterDisplayTextWithParam(Character, Text, Parameter) end
	
--- @param Character string
--- @param Bool integer
function Osi.CharacterSetImmortal(Character, Bool) end
	
--- @param Character string
function Osi.CharacterFlushQueue(Character) end
	
--- @param Character string
function Osi.CharacterPurgeQueue(Character) end
	
--- @param Event string
function Osi.CharacterLaunchIterator(Event) end
	
--- @param Center string
--- @param Radius number
--- @param Event string
function Osi.CharacterLaunchIteratorAroundObject(Center, Radius, Event) end
	
--- @param Event string
function Osi.CharacterLaunchOsirisOnlyIterator(Event) end
	
--- @param Trader string
--- @param Bool integer
function Osi.CharacterSetCanTrade(Trader, Bool) end
	
--- @param Character string
function Osi.CharacterSetStill(Character) end
	
--- @param Character string
--- @param Enabled integer
--- @param Immediately integer
function Osi.CharacterSetFightMode(Character, Enabled, Immediately) end
	
--- @param Character string
--- @param Bool integer
function Osi.CharacterMakeStoryNpc(Character, Bool) end
	
--- @param Character string
--- @param FxName string
function Osi.CharacterStopAllEffectsWithName(Character, FxName) end
	
--- @param Character string
--- @param Item string
--- @param Event string
function Osi.CharacterPickupItem(Character, Item, Event) end
	
--- @param Character string
--- @param Item string
--- @param Event string
function Osi.CharacterItemSetEvent(Character, Item, Event) end
	
--- @param Character string
--- @param Character2 string
--- @param Event string
function Osi.CharacterCharacterSetEvent(Character, Character2, Event) end
	
--- @param Character string
--- @param Item string
--- @param Event string
function Osi.CharacterUseItem(Character, Item, Event) end
	
--- @param Character string
--- @param Item string
--- @param Trigger string
--- @param Amount integer
--- @param Event string
function Osi.CharacterMoveItemToTrigger(Character, Item, Trigger, Amount, Event) end
	
--- @param Character string
--- @param ConsumeHandle integer
function Osi.CharacterUnconsume(Character, ConsumeHandle) end
	
--- @param Character string
--- @param Attribute string
--- @param Value integer
function Osi.CharacterAddAttribute(Character, Attribute, Value) end
	
--- @param Character string
--- @param Attribute string
--- @param Value integer
function Osi.CharacterRemoveAttribute(Character, Attribute, Value) end
	
--- @param Character string
--- @param Ability string
--- @param Value integer
function Osi.CharacterAddAbility(Character, Ability, Value) end
	
--- @param Character string
--- @param Ability string
--- @param Value integer
function Osi.CharacterRemoveAbility(Character, Ability, Value) end
	
--- @param Player string
--- @param Trader string
--- @param CanRepair integer
--- @param CanIdentify integer
--- @param CanSell integer
function Osi.ActivateTrade(Player, Trader, CanRepair, CanIdentify, CanSell) end
	
--- @param Player string
--- @param NPC string
--- @param Success integer
function Osi.StartPickpocket(Player, NPC, Success) end
	
--- @param Character string
--- @param Deal integer
--- @param AttitudeDiff integer
function Osi.ExecuteDeal(Character, Deal, AttitudeDiff) end
	
--- @param Character string
--- @param CharacterToFollow string
function Osi.CharacterSetFollowCharacter(Character, CharacterToFollow) end
	
--- @param Src string
--- @param Target string
function Osi.CharacterAttachToGroup(Src, Target) end
	
--- @param Character string
function Osi.CharacterDetachFromGroup(Character) end
	
--- @param Character string
--- @param Player string
--- @param Delta integer
function Osi.CharacterAddAttitudeTowardsPlayer(Character, Player, Delta) end
	
--- @param Character string
--- @param CanSpotSneakers integer
function Osi.CharacterSetCanSpotSneakers(Character, CanSpotSneakers) end
	
--- @param Character string
--- @param Container string
function Osi.CharacterMoveWeaponsToContainer(Character, Container) end
	
--- @param Character string
--- @param Ability string
function Osi.CharacterLockAbility(Character, Ability) end
	
--- @param Character string
--- @param Ability string
function Osi.CharacterUnlockAbility(Character, Ability) end
	
--- @param Character string
--- @param RecipeID string
--- @param ShowNotification integer
function Osi.CharacterUnlockRecipe(Character, RecipeID, ShowNotification) end
	
--- @param Character string
--- @param Animation string
function Osi.CharacterSetAnimationOverride(Character, Animation) end
	
--- @param Character string
--- @param AnimationSetResource string
function Osi.CharacterSetAnimationSetOverride(Character, AnimationSetResource) end
	
--- @param Character string
--- @param PartyMember string
--- @param Modifier integer
function Osi.PartySetIdentifyPriceModifier(Character, PartyMember, Modifier) end
	
--- @param Character string
--- @param PartyMember string
--- @param Modifier integer
function Osi.PartySetRepairPriceModifier(Character, PartyMember, Modifier) end
	
--- @param Character string
--- @param PartyMember string
--- @param Modifier integer
function Osi.PartySetShopPriceModifier(Character, PartyMember, Modifier) end
	
--- @param Character string
--- @param Tag string
--- @param Modifier integer
function Osi.SetTagPriceModifier(Character, Tag, Modifier) end
	
--- @param Character string
function Osi.CharacterResetCooldowns(Character) end
	
--- @param Character string
--- @param VisualSlot integer
--- @param ElementName string
function Osi.CharacterSetVisualElement(Character, VisualSlot, ElementName) end
	
--- @param Character string
--- @param Type integer
--- @param UIInstance string
function Osi.CharacterShowStoryElementUI(Character, Type, UIInstance) end
	
--- @param Character string
--- @param Type integer
--- @param UIInstance string
function Osi.CharacterCloseStoryElementUI(Character, Type, UIInstance) end
	
--- @param Character string
--- @param Turn integer
function Osi.CharacterSendGlobalCombatCounter(Character, Turn) end
	
--- @param Character string
--- @param Sound string
function Osi.CharacterPlayHUDSound(Character, Sound) end
	
--- @param Character string
--- @param SoundResource string
function Osi.CharacterPlayHUDSoundResource(Character, SoundResource) end
	
--- @param Player string
--- @param CrimeType string
--- @param Evidence string
--- @param Witness string
--- @param CrimeID integer
function Osi.CharacterRegisterCrime(Player, CrimeType, Evidence, Witness, CrimeID) end
	
--- @param Player string
--- @param CrimeType string
--- @param Evidence string
--- @param Witness string
--- @param X number
--- @param Y number
--- @param Z number
--- @param CrimeID integer
function Osi.CharacterRegisterCrimeWithPosition(Player, CrimeType, Evidence, Witness, X, Y, Z, CrimeID) end
	
--- @param Player string
--- @param CrimeType string
--- @param Evidence string
function Osi.CharacterStopCrime(Player, CrimeType, Evidence) end
	
--- @param Player string
--- @param Crime integer
function Osi.CharacterStopCrimeWithID(Player, Crime) end
	
--- @param Character string
--- @param Player string
--- @param Timer number
function Osi.CharacterIgnoreCharacterActiveCrimes(Character, Player, Timer) end
	
--- @param Character string
--- @param Die integer
function Osi.CharacterRemoveSummons(Character, Die) end
	
--- @param Character string
--- @param Ghost string
function Osi.CharacterLinkGhost(Character, Ghost) end
	
--- @param Character string
--- @param Ghost string
function Osi.CharacterUnlinkGhost(Character, Ghost) end
	
--- @param Ghost string
function Osi.DestroyGhost(Ghost) end
	
--- @param Character string
--- @param SkillID string
--- @param Target string
--- @param ForceResetCooldown integer
--- @param IgnoreHasSkill integer
--- @param IgnoreChecks integer
function Osi.CharacterUseSkill(Character, SkillID, Target, ForceResetCooldown, IgnoreHasSkill, IgnoreChecks) end
	
--- @param Character string
--- @param SkillID string
--- @param X number
--- @param Y number
--- @param Z number
--- @param ForceResetCooldown integer
--- @param IgnoreHasSkill integer
function Osi.CharacterUseSkillAtPosition(Character, SkillID, X, Y, Z, ForceResetCooldown, IgnoreHasSkill) end
	
--- @param Character string
--- @param Crime string
function Osi.CharacterDisableCrime(Character, Crime) end
	
--- @param Character string
--- @param Crime string
function Osi.CharacterEnableCrime(Character, Crime) end
	
--- @param Character string
function Osi.CharacterDisableAllCrimes(Character) end
	
--- @param Character string
function Osi.CharacterEnableAllCrimes(Character) end
	
--- @param Character string
--- @param Enable integer
function Osi.CharacterEnableCrimeWarnings(Character, Enable) end
	
--- @param Player string
function Osi.CharacterRemoveTension(Player) end
	
--- @param Character string
--- @param Amount integer
function Osi.CharacterAddSourcePoints(Character, Amount) end
	
--- @param Character string
--- @param Amount integer
function Osi.CharacterOverrideMaxSourcePoints(Character, Amount) end
	
--- @param Character string
function Osi.CharacterRemoveMaxSourcePointsOverride(Character) end
	
--- @param Character string
--- @param Preset string
function Osi.CharacterApplyPreset(Character, Preset) end
	
--- @param Character string
--- @param Preset string
function Osi.CharacterApplyHenchmanPreset(Character, Preset) end
	
--- @param Character string
--- @param Preset string
function Osi.CharacterApplyRacePreset(Character, Preset) end
	
--- @param Character string
--- @param Target string
--- @param DialogID string
--- @param IsAutomated integer
--- @param MoveID string
--- @param Running integer
--- @param Timeout number
function Osi.CharacterMoveToAndTalk(Character, Target, DialogID, IsAutomated, MoveID, Running, Timeout) end
	
--- @param Character string
--- @param Target string
--- @param MoveId string
function Osi.CharacterMoveToAndTalkRequestDialogFailed(Character, Target, MoveId) end
	
--- @param Character string
--- @param Bool integer
function Osi.CharacterEnableWaypointUsage(Character, Bool) end
	
--- @param Character string
--- @param Race string
function Osi.CharacterReservePolymorphShape(Character, Race) end
	
--- @param Character string
--- @param Bool integer
function Osi.CharacterSetForceSynch(Character, Bool) end
	
--- @param Character string
--- @param Bool integer
function Osi.CharacterSetForceUpdate(Character, Bool) end
	
--- @param Character string
--- @param ObjectTemplate string
--- @param ReplaceScripts integer
--- @param ReplaceScale integer
--- @param ReplaceStats integer
--- @param ReplaceEquipment integer
--- @param ReplaceSkills integer
--- @param UseCustomLooks integer
--- @param ReleasePlayerData integer
function Osi.CharacterTransform(Character, ObjectTemplate, ReplaceScripts, ReplaceScale, ReplaceStats, ReplaceEquipment, ReplaceSkills, UseCustomLooks, ReleasePlayerData) end
	
--- @param Target string
--- @param Source string
--- @param ReplaceScripts integer
--- @param ReplaceScale integer
--- @param ReplaceStats integer
--- @param ReplaceEquipment integer
--- @param ReplaceSkills integer
--- @param UseCustomLooks integer
--- @param ReleasePlayerData integer
function Osi.CharacterTransformFromCharacter(Target, Source, ReplaceScripts, ReplaceScale, ReplaceStats, ReplaceEquipment, ReplaceSkills, UseCustomLooks, ReleasePlayerData) end
	
--- @param Character string
--- @param Target string
--- @param CopyEquipment integer
--- @param CopyDisplayNameAndIcon integer
function Osi.CharacterTransformAppearanceTo(Character, Target, CopyEquipment, CopyDisplayNameAndIcon) end
	
--- @param Character string
--- @param Target string
--- @param EquipmentSet string
--- @param CopyDisplayNameAndIcon integer
function Osi.CharacterTransformAppearanceToWithEquipmentSet(Character, Target, EquipmentSet, CopyDisplayNameAndIcon) end
	
--- @param Character string
--- @param Tag string
function Osi.CharacterReceivedTag(Character, Tag) end
	
--- @param Character string
--- @param Tag string
function Osi.CharacterAddPreferredAiTargetTag(Character, Tag) end
	
--- @param Character string
--- @param Tag string
function Osi.CharacterRemovePreferredAiTargetTag(Character, Tag) end
	
--- @param Character string
function Osi.CharacterOriginIntroStopped(Character) end
	
--- @param Character string
--- @param Value integer
function Osi.CharacterSetDoNotFaceFlag(Character, Value) end
	
--- @param Character string
--- @param Value integer
function Osi.CharacterSetReadyCheckBlocked(Character, Value) end
	
--- @param Character string
--- @param Value integer
function Osi.CharacterSetCorpseLootable(Character, Value) end
	
--- @param Character string
--- @param Value integer
function Osi.CharacterSetDetached(Character, Value) end
	
--- @param From string
--- @param To string
--- @param MemorizedOnly integer
function Osi.CharacterCloneSkillsTo(From, To, MemorizedOnly) end
	
--- @param Attacker string
--- @param Attacked string
--- @param MinDistance number
--- @param MaxDistance number
function Osi.CharacterJitterbugTeleport(Attacker, Attacked, MinDistance, MaxDistance) end
	
--- @param Character string
function Osi.RemoveTemporaryCharacter(Character) end
	
--- @param Target string
--- @param Owner string
function Osi.CharacterChangeToSummon(Target, Owner) end
	
--- @param Target string
--- @param Turns integer
function Osi.CharacterSetSummonLifetime(Target, Turns) end
	
--- @param Item string
--- @param Trigger string
function Osi.ItemDragToTrigger(Item, Trigger) end
	
--- @param Item string
--- @param X number
--- @param Y number
--- @param Z number
function Osi.ItemDragToPosition(Item, X, Y, Z) end
	
--- @param Item string
--- @param Trigger string
--- @param Speed number
--- @param Acceleration number
--- @param UseRotation integer
--- @param Event string
--- @param DoHits integer
function Osi.ItemMoveToTrigger(Item, Trigger, Speed, Acceleration, UseRotation, Event, DoHits) end
	
--- @param Item string
--- @param X number
--- @param Y number
--- @param Z number
--- @param Speed number
--- @param Acceleration number
--- @param Event string
--- @param DoHits integer
function Osi.ItemMoveToPosition(Item, X, Y, Z, Speed, Acceleration, Event, DoHits) end
	
--- @param string string
--- @param X number
--- @param Y number
--- @param Z number
--- @param Pitch number
--- @param Yaw number
--- @param Roll number
--- @param Amount integer
--- @param OwnerCharacter string
function Osi.ItemToTransform(string, X, Y, Z, Pitch, Yaw, Roll, Amount, OwnerCharacter) end
	
--- @param Item string
--- @param TargetObject string
--- @param Amount integer
--- @param ShowNotification integer
--- @param ClearOriginalOwner integer
function Osi.ItemToInventory(Item, TargetObject, Amount, ShowNotification, ClearOriginalOwner) end
	
--- @param ItemTemplate string
--- @param Character string
--- @param Count integer
function Osi.ItemTemplateDropFromCharacter(ItemTemplate, Character, Count) end
	
--- @param Item string
--- @param X number
--- @param Y number
--- @param Z number
function Osi.ItemScatterAt(Item, X, Y, Z) end
	
--- @param ItemTemplate string
--- @param Object string
--- @param Count integer
function Osi.ItemTemplateRemoveFrom(ItemTemplate, Object, Count) end
	
--- @param ItemTemplate string
--- @param Character string
--- @param Count integer
function Osi.ItemTemplateRemoveFromParty(ItemTemplate, Character, Count) end
	
--- @param ItemTemplate string
--- @param Character string
--- @param Count integer
function Osi.ItemTemplateRemoveFromUser(ItemTemplate, Character, Count) end
	
--- @param ItemTemplate string
--- @param Object string
--- @param Count integer
--- @param ShowNotification integer
function Osi.ItemTemplateAddTo(ItemTemplate, Object, Count, ShowNotification) end
	
--- @param Item string
function Osi.ItemDrop(Item) end
	
--- @param Item string
function Osi.ItemRemove(Item) end
	
--- @param FromObject string
--- @param ToObject string
--- @param MoveEquippedArmor integer
--- @param MoveEquippedWeapons integer
--- @param ClearOriginalOwner integer
function Osi.MoveAllItemsTo(FromObject, ToObject, MoveEquippedArmor, MoveEquippedWeapons, ClearOriginalOwner) end
	
--- @param FromContainer string
function Osi.ContainerIdentifyAll(FromContainer) end
	
--- @param Item string
--- @param Key string
function Osi.ItemLock(Item, Key) end
	
--- @param Item string
function Osi.ItemUnLock(Item) end
	
--- @param Item string
--- @param lock integer
function Osi.ItemLockUnEquip(Item, lock) end
	
--- @param Item string
--- @param IsPoisoned integer
function Osi.ItemIsPoisoned(Item, IsPoisoned) end
	
--- @param Item string
function Osi.ItemOpen(Item) end
	
--- @param Item string
function Osi.ItemClose(Item) end
	
--- @param Item string
function Osi.ItemDestroy(Item) end
	
--- @param Item string
function Osi.ItemClearOwner(Item) end
	
--- @param Item string
--- @param bool integer
function Osi.ItemSetCanInteract(Item, bool) end
	
--- @param Item string
--- @param bool integer
function Osi.ItemSetUseRemotely(Item, bool) end
	
--- @param Item string
--- @param bool integer
function Osi.ItemSetCanPickUp(Item, bool) end
	
--- @param Item string
--- @param bool integer
function Osi.ItemSetCanMove(Item, bool) end
	
--- @param Item string
--- @param NewOwner string
function Osi.ItemSetOwner(Item, NewOwner) end
	
--- @param Item string
--- @param NewOwner string
function Osi.ItemSetOriginalOwner(Item, NewOwner) end
	
--- @param Item string
--- @param bool integer
function Osi.ItemSetOnlyOwnerCanUse(Item, bool) end
	
--- @param Item string
--- @param bool integer
function Osi.ItemSetStoryItem(Item, bool) end
	
--- @param Event string
function Osi.ItemLaunchIterator(Event) end
	
--- @param Trigger string
--- @param TemplateId string
function Osi.ItemCreateAtTrigger(Trigger, TemplateId) end
	
--- @param Trigger string
--- @param TemplateId string
function Osi.CreateKickstarterMessageInABottleItemAtTrigger(Trigger, TemplateId) end
	
--- @param Item string
--- @param Angle number
--- @param Speed number
function Osi.ItemRotateY(Item, Angle, Speed) end
	
--- @param Item string
--- @param Angle number
--- @param Speed number
function Osi.ItemRotateToAngleY(Item, Angle, Speed) end
	
--- @param Item string
--- @param Angle number
--- @param Speed number
--- @param Animation string
--- @param Event string
function Osi.ItemRotateToAngleY_Animate(Item, Angle, Speed, Animation, Event) end
	
--- @param Item string
--- @param Charges integer
function Osi.ItemAddCharges(Item, Charges) end
	
--- @param Item string
function Osi.ItemResetChargesToMax(Item) end
	
--- @param Item string
--- @param Durability integer
function Osi.ItemSetDurability(Item, Durability) end
	
--- @param Item string
--- @param Bool integer
function Osi.ItemSetForceSynch(Item, Bool) end
	
--- @param Item string
--- @param Bool integer
function Osi.ItemSetKnown(Item, Bool) end
	
--- @param Item string
function Osi.ItemLevelUp(Item) end
	
--- @param Item string
--- @param Level integer
function Osi.ItemLevelUpTo(Item, Level) end
	
--- @param Character string
--- @param Item string
--- @param RuneTemplate string
--- @param Slot integer
function Osi.ItemInsertRune(Character, Item, RuneTemplate, Slot) end
	
--- @param Item string
--- @param Boost string
function Osi.ItemAddDeltaModifier(Item, Boost) end
	
--- @param Dialog string
--- @param Speaker string
function Osi.DialogRequestStopForDialog(Dialog, Speaker) end
	
--- @param Speaker string
function Osi.DialogRequestStop(Speaker) end
	
--- @param Flag string
function Osi.GlobalSetFlag(Flag) end
	
--- @param Flag string
function Osi.GlobalClearFlag(Flag) end
	
--- @param Target string
--- @param Flag string
--- @param DialogInstance integer
function Osi.ObjectSetFlag(Target, Flag, DialogInstance) end
	
--- @param Target string
--- @param Flag string
function Osi.ObjectShareFlag(Target, Flag) end
	
--- @param Flag string
function Osi.GlobalShareFlag(Flag) end
	
--- @param Target string
--- @param Flag string
--- @param DialogInstance integer
function Osi.ObjectClearFlag(Target, Flag, DialogInstance) end
	
--- @param Target string
--- @param Flag string
--- @param DialogInstance integer
function Osi.ObjectSetDialogFlag(Target, Flag, DialogInstance) end
	
--- @param Target string
--- @param Flag string
--- @param DialogInstance integer
function Osi.ObjectClearDialogFlag(Target, Flag, DialogInstance) end
	
--- @param Character string
--- @param Flag string
--- @param DialogInstance integer
function Osi.UserSetFlag(Character, Flag, DialogInstance) end
	
--- @param Character string
--- @param Flag string
--- @param DialogInstance integer
function Osi.UserClearFlag(Character, Flag, DialogInstance) end
	
--- @param Character string
--- @param Flag string
--- @param DialogInstance integer
function Osi.PartySetFlag(Character, Flag, DialogInstance) end
	
--- @param Character string
--- @param Flag string
--- @param DialogInstance integer
function Osi.PartyClearFlag(Character, Flag, DialogInstance) end
	
--- @param Character string
--- @param Location string
--- @param Question string
--- @param Answer string
function Osi.AddFeedbackString(Character, Location, Question, Answer) end
	

function Osi.SaveFeedback() end
	
--- @param Object string
--- @param FxName string
--- @param Bone string
function Osi.PlayEffect(Object, FxName, Bone) end
	
--- @param Object string
--- @param Target string
--- @param FxName string
--- @param SourceBone string
--- @param TargetBone string
function Osi.PlayBeamEffect(Object, Target, FxName, SourceBone, TargetBone) end
	
--- @param Object string
--- @param SoundEvent string
function Osi.PlaySound(Object, SoundEvent) end
	
--- @param Object string
--- @param SoundResource string
function Osi.PlaySoundResource(Object, SoundResource) end
	
--- @param Object string
--- @param Text string
function Osi.DebugText(Object, Text) end
	
--- @param Object string
--- @param Text string
function Osi.DisplayText(Object, Text) end
	
--- @param Object string
--- @param Bool integer
function Osi.SetOnStage(Object, Bool) end
	
--- @param Object string
--- @param Bool integer
function Osi.SetVisible(Object, Bool) end
	
--- @param Character string
--- @param MarkerID string
--- @param Show integer
function Osi.ShowMapMarker(Character, MarkerID, Show) end
	
--- @param MarkerID string
--- @param NewRegionID string
function Osi.MapMarkerChangeLevel(MarkerID, NewRegionID) end
	
--- @param Distance number
function Osi.SetCameraDistanceOverride(Distance) end
	
--- @param Timer string
--- @param Time2 integer
function Osi.TimerLaunch(Timer, Time2) end
	
--- @param Timer string
function Osi.TimerCancel(Timer) end
	
--- @param Timer string
function Osi.TimerPause(Timer) end
	
--- @param Timer string
function Osi.TimerUnpause(Timer) end
	
--- @param Character string
--- @param EventName string
function Osi.MusicPlayForPeer(Character, EventName) end
	
--- @param Character string
--- @param Character2 string
--- @param EventName string
function Osi.MusicPlayForPeerWithInstrument(Character, Character2, EventName) end
	
--- @param Character string
--- @param EventName string
function Osi.MusicPlayOnCharacter(Character, EventName) end
	
--- @param EventName string
function Osi.MusicPlayGeneral(EventName) end
	
--- @param Character string
--- @param Event string
function Osi.MoviePlay(Character, Event) end
	
--- @param Character string
--- @param DialogName string
--- @param NodePrefix string
function Osi.PlayMovieForDialog(Character, DialogName, NodePrefix) end
	
--- @param Character string
--- @param Name string
--- @param Time number
--- @param HideUI integer
--- @param Smooth integer
--- @param HideShroud integer
function Osi.CameraActivate(Character, Name, Time, HideUI, Smooth, HideShroud) end
	
--- @param Character string
--- @param Bool integer
function Osi.SetSelectorCameraMode(Character, Bool) end
	
--- @param Character string
--- @param Quest string
--- @param Status string
function Osi.QuestUpdate(Character, Quest, Status) end
	
--- @param SrcCharacter string
--- @param Character2 string
--- @param Quest string
--- @param Status string
function Osi.QuestReceiveSharedUpdate(SrcCharacter, Character2, Quest, Status) end
	
--- @param Character string
--- @param Quest string
function Osi.QuestAdd(Character, Quest) end
	
--- @param Character string
--- @param Quest string
function Osi.QuestClose(Character, Quest) end
	
--- @param Quest string
function Osi.QuestCloseAll(Quest) end
	
--- @param Character string
--- @param SubquestID string
--- @param ParentQuestID string
function Osi.QuestAddSubquest(Character, SubquestID, ParentQuestID) end
	
--- @param Target string
--- @param InArena integer
function Osi.SetInArena(Target, InArena) end
	
--- @param NotificationType integer
--- @param StringParam string
--- @param InNumberOfRounds integer
function Osi.SendArenaNotification(NotificationType, StringParam, InNumberOfRounds) end
	
--- @param Quest string
--- @param CategoryID string
function Osi.QuestSetCategory(Quest, CategoryID) end
	
--- @param Character string
--- @param Quest string
--- @param DoArchive integer
function Osi.QuestArchive(Character, Quest, DoArchive) end
	
--- @param Character string
--- @param CategoryID string
--- @param DoArchive integer
function Osi.QuestArchiveCategory(Character, CategoryID, DoArchive) end
	
--- @param Character string
--- @param Secret string
function Osi.AddSecret(Character, Secret) end
	
--- @param RecipeID string
function Osi.UnlockJournalRecipe(RecipeID) end
	
--- @param Character string
--- @param MysteryID string
function Osi.UnlockJournalMystery(Character, MysteryID) end
	
--- @param Character string
--- @param MysteryID string
function Osi.CloseJournalMystery(Character, MysteryID) end
	

function Osi.GameEnd() end
	
--- @param Movie string
function Osi.GameEndWithMovie(Movie) end
	
--- @param CallbackID string
function Osi.GameEndWithMovieRequestCallback(CallbackID) end
	
--- @param UserID integer
--- @param Movie string
--- @param Music string
function Osi.EnqueueGameEndMovie(UserID, Movie, Music) end
	
--- @param Character string
--- @param DialogName string
--- @param PlaylistId string
--- @param Music string
function Osi.EnqueueGameEndDialogMovie(Character, DialogName, PlaylistId, Music) end
	
--- @param UserID integer
function Osi.FinalizeGameEndMovieQueue(UserID) end
	
--- @param UserID integer
--- @param Movie string
function Osi.SetGameEndMovie(UserID, Movie) end
	
--- @param Enable integer
function Osi.ShroudRender(Enable) end
	
--- @param Message string
function Osi.DebugBreak(Message) end
	
--- @param Character string
--- @param Seconds number
--- @param ToBlack integer
--- @param FadeID string
function Osi.FadeToBlack(Character, Seconds, ToBlack, FadeID) end
	
--- @param Character string
--- @param Seconds number
--- @param ToWhite integer
--- @param FadeID string
function Osi.FadeToWhite(Character, Seconds, ToWhite, FadeID) end
	
--- @param Character string
--- @param Seconds number
--- @param FadeID string
function Osi.FadeOutBlack(Character, Seconds, FadeID) end
	
--- @param Character string
--- @param Seconds number
--- @param FadeID string
function Osi.FadeOutWhite(Character, Seconds, FadeID) end
	
--- @param Character string
--- @param Seconds number
--- @param FadeID string
function Osi.FadeIn(Character, Seconds, FadeID) end
	
--- @param Character string
--- @param Bookname string
function Osi.OpenCustomBookUI(Character, Bookname) end
	
--- @param Bookname string
--- @param Entryname string
function Osi.AddEntryToCustomBook(Bookname, Entryname) end
	
--- @param Bookname string
--- @param Entryname string
function Osi.RemoveEntryFromCustomBook(Bookname, Entryname) end
	
--- @param Character string
--- @param CurrentWaypoint string
--- @param Item string
--- @param IsFleeing integer
function Osi.OpenWaypointUI(Character, CurrentWaypoint, Item, IsFleeing) end
	
--- @param Character string
--- @param UIName string
function Osi.CloseUI(Character, UIName) end
	
--- @param Character string
--- @param Item string
function Osi.OpenCraftUI(Character, Item) end
	
--- @param WaypointName string
--- @param Trigger string
--- @param Character string
function Osi.UnlockWaypoint(WaypointName, Trigger, Character) end
	
--- @param WaypointName string
--- @param Character string
function Osi.LockWaypoint(WaypointName, Character) end
	
--- @param WaypointName string
--- @param Item string
function Osi.RegisterWaypoint(WaypointName, Item) end
	
--- @param SecretRegionTrigger string
function Osi.UnlockSecretRegion(SecretRegionTrigger) end
	
--- @param SecretRegionTrigger string
function Osi.LockSecretRegion(SecretRegionTrigger) end
	
--- @param Source string
--- @param SurfaceType string
--- @param Radius number
--- @param Lifetime number
function Osi.CreateSurface(Source, SurfaceType, Radius, Lifetime) end
	
--- @param x number
--- @param Y number
--- @param Z number
--- @param SurfaceType string
--- @param Radius number
--- @param Lifetime number
function Osi.CreateSurfaceAtPosition(x, Y, Z, SurfaceType, Radius, Lifetime) end
	
--- @param Source string
--- @param SurfaceLayer integer
--- @param Radius number
function Osi.RemoveSurfaceLayer(Source, SurfaceLayer, Radius) end
	
--- @param x number
--- @param Y number
--- @param Z number
--- @param SurfaceLayer integer
--- @param Radius number
function Osi.RemoveSurfaceLayerAtPosition(x, Y, Z, SurfaceLayer, Radius) end
	
--- @param Source string
--- @param SurfaceType string
--- @param CellAmountMin integer
--- @param CellAmountMax integer
--- @param GrowAmountMin integer
--- @param GrowAmountMax integer
--- @param GrowTime number
function Osi.CreatePuddle(Source, SurfaceType, CellAmountMin, CellAmountMax, GrowAmountMin, GrowAmountMax, GrowTime) end
	
--- @param Source string
--- @param TransformType string
--- @param TransformLayer string
--- @param Radius number
--- @param Lifetime number
--- @param Owner string
function Osi.TransformSurface(Source, TransformType, TransformLayer, Radius, Lifetime, Owner) end
	
--- @param X number
--- @param Y number
--- @param Z number
--- @param TransformType string
--- @param TransformLayer string
--- @param Radius number
--- @param Lifetime number
--- @param Owner string
function Osi.TransformSurfaceAtPosition(X, Y, Z, TransformType, TransformLayer, Radius, Lifetime, Owner) end
	
--- @param SurfaceActionHandle integer
function Osi.StopDrawSurfaceOnPath(SurfaceActionHandle) end
	
--- @param FxName string
--- @param X number
--- @param Y number
--- @param Z number
function Osi.PlayEffectAtPosition(FxName, X, Y, Z) end
	
--- @param FxName string
--- @param X number
--- @param Y number
--- @param Z number
--- @param Yangle number
function Osi.PlayEffectAtPositionAndRotation(FxName, X, Y, Z, Yangle) end
	
--- @param FxName string
--- @param Scale number
--- @param X number
--- @param Y number
--- @param Z number
function Osi.PlayScaledEffectAtPosition(FxName, Scale, X, Y, Z) end
	
--- @param CombatID integer
function Osi.EndCombat(CombatID) end
	
--- @param FxHandle integer
function Osi.StopLoopEffect(FxHandle) end
	
--- @param Target string
function Osi.MakePlayerActive(Target) end
	
--- @param Target string
--- @param SkillID string
--- @param CasterLevel integer
function Osi.CreateExplosion(Target, SkillID, CasterLevel) end
	
--- @param X number
--- @param Y number
--- @param Z number
--- @param SkillID string
--- @param CasterLevel integer
function Osi.CreateExplosionAtPosition(X, Y, Z, SkillID, CasterLevel) end
	
--- @param Target string
--- @param SkillID string
--- @param CasterLevel integer
function Osi.CreateProjectileStrikeAt(Target, SkillID, CasterLevel) end
	
--- @param X number
--- @param Y number
--- @param Z number
--- @param SkillID string
--- @param CasterLevel integer
function Osi.CreateProjectileStrikeAtPosition(X, Y, Z, SkillID, CasterLevel) end
	
--- @param SkillID string
--- @param Source string
--- @param MinAngle number
--- @param MaxAngle number
--- @param Distance number
function Osi.LaunchProjectileAtCone(SkillID, Source, MinAngle, MaxAngle, Distance) end
	

function Osi.AutoSave() end
	

function Osi.ShowGameOverMenu() end
	
--- @param Character string
function Osi.OnCompanionDismissed(Character) end
	
--- @param Character string
--- @param Text string
function Osi.ShowNotification(Character, Text) end
	
--- @param Character string
--- @param Error string
function Osi.ShowError(Character, Error) end
	
--- @param Character string
--- @param Text string
--- @param Category string
--- @param Title string
--- @param ControllerType integer
--- @param ModalType integer
--- @param Duration integer
--- @param Priority integer
--- @param Flags integer
--- @param MinimumPlaytimeInMinutes integer
function Osi.ShowTutorial(Character, Text, Category, Title, ControllerType, ModalType, Duration, Priority, Flags, MinimumPlaytimeInMinutes) end
	
--- @param Character string
--- @param Text string
function Osi.CompleteTutorial(Character, Text) end
	
--- @param AchievementID string
--- @param Character string
function Osi.UnlockAchievement(AchievementID, Character) end
	
--- @param AchievementID string
--- @param Character string
--- @param Progress integer
function Osi.ProgressAchievement(AchievementID, Character, Progress) end
	
--- @param AchievementID string
--- @param Value integer
function Osi.SetAchievementProgress(AchievementID, Value) end
	
--- @param State integer
--- @param Character string
function Osi.SetHomesteadKeyState(State, Character) end
	
--- @param Bool integer
function Osi.EnableSendToHomestead(Bool) end
	
--- @param Character string
function Osi.KillCombatFor(Character) end
	
--- @param Item string
--- @param TreasureID string
--- @param Level integer
--- @param Character string
function Osi.GenerateTreasure(Item, TreasureID, Level, Character) end
	
--- @param SourceObject string
--- @param Animation string
--- @param Event string
function Osi.PlayAnimation(SourceObject, Animation, Event) end
	
--- @param Source string
--- @param VarName string
--- @param Object string
function Osi.SetVarObject(Source, VarName, Object) end
	
--- @param Source string
--- @param VarName string
function Osi.ClearVarObject(Source, VarName) end
	
--- @param Target string
--- @param Scriptframe string
function Osi.SetScriptframe(Target, Scriptframe) end
	
--- @param Target string
function Osi.ClearScriptframe(Target) end
	
--- @param Source string
--- @param Tag string
function Osi.SetTag(Source, Tag) end
	
--- @param Source string
--- @param Tag string
function Osi.ClearTag(Source, Tag) end
	
--- @param Target string
--- @param VarName string
--- @param VarValue integer
function Osi.SetVarInteger(Target, VarName, VarValue) end
	
--- @param Target string
--- @param VarName string
--- @param VarValue number
function Osi.SetVarFloat(Target, VarName, VarValue) end
	
--- @param Target string
--- @param VarName string
--- @param VarValue string
function Osi.SetVarString(Target, VarName, VarValue) end
	
--- @param Target string
--- @param VarName string
--- @param VarValue string
function Osi.SetVarFixedString(Target, VarName, VarValue) end
	
--- @param Target string
--- @param VarName string
--- @param X number
--- @param Y number
--- @param Z number
function Osi.SetVarFloat3(Target, VarName, X, Y, Z) end
	
--- @param Target string
--- @param Status string
function Osi.RemoveStatus(Target, Status) end
	
--- @param Target string
function Osi.RemoveHarmfulStatuses(Target) end
	
--- @param Object string
--- @param ObjectTemplate string
--- @param ReplaceScripts integer
--- @param ReplaceScale integer
--- @param ReplaceStats integer
function Osi.Transform(Object, ObjectTemplate, ReplaceScripts, ReplaceScale, ReplaceStats) end
	
--- @param Object string
--- @param Status string
--- @param Duration number
--- @param Force integer
--- @param Source string
function Osi.ApplyStatus(Object, Status, Duration, Force, Source) end
	
--- @param Object string
--- @param Event string
function Osi.SetStoryEvent(Object, Event) end
	
--- @param Character string
--- @param RequestId integer
--- @param RequestAccepted integer
function Osi.RequestProcessed(Character, RequestId, RequestAccepted) end
	
--- @param Character string
--- @param Event string
function Osi.IterateParty(Character, Event) end
	
--- @param Event string
function Osi.IterateParties(Event) end
	
--- @param Event string
function Osi.IterateUsers(Event) end
	
--- @param Savegame string
function Osi.LoadGame(Savegame) end
	
--- @param UserId integer
function Osi.LeaveParty(UserId) end
	
--- @param Source integer
--- @param Target integer
function Osi.AddToParty(Source, Target) end
	
--- @param Preset string
--- @param TeleportToTarget string
function Osi.LoadPartyPreset(Preset, TeleportToTarget) end
	

function Osi.CrimeClearAll() end
	
--- @param CrimeID integer
--- @param Interrogator string
--- @param FoundEvidence integer
--- @param Criminal1 string
--- @param Criminal2 string
--- @param Criminal3 string
--- @param Criminal4 string
function Osi.CrimeInterrogationDone(CrimeID, Interrogator, FoundEvidence, Criminal1, Criminal2, Criminal3, Criminal4) end
	
--- @param CrimeID integer
--- @param Interrogator string
function Osi.CrimeConfrontationDone(CrimeID, Interrogator) end
	
--- @param Target string
--- @param Enabled integer
function Osi.SetCanFight(Target, Enabled) end
	
--- @param Target string
--- @param Enabled integer
function Osi.SetCanJoinCombat(Target, Enabled) end
	
--- @param InventoryHolder string
--- @param Event string
--- @param CompletionEvent string
function Osi.InventoryLaunchIterator(InventoryHolder, Event, CompletionEvent) end
	
--- @param InventoryHolder string
--- @param TagA string
--- @param OptionalTagB string
--- @param Event string
--- @param CompletionEvent string
function Osi.InventoryLaunchTagIterator(InventoryHolder, TagA, OptionalTagB, Event, CompletionEvent) end
	
--- @param InventoryHolder string
--- @param Template string
--- @param Event string
--- @param CompletionEvent string
function Osi.InventoryLaunchTemplateIterator(InventoryHolder, Template, Event, CompletionEvent) end
	
--- @param Target string
--- @param Enabled integer
function Osi.SetIsBoss(Target, Enabled) end
	
--- @param Target string
--- @param GroupID string
function Osi.SetCombatGroupID(Target, GroupID) end
	
--- @param Target string
function Osi.EndTurn(Target) end
	
--- @param CrimeArea string
--- @param Modifier integer
function Osi.CrimeAreaSetTensionModifier(CrimeArea, Modifier) end
	
--- @param CrimeArea string
function Osi.CrimeAreaResetTensionModifier(CrimeArea) end
	
--- @param CrimeID integer
--- @param Target string
function Osi.CrimeTransferEvidenceTo(CrimeID, Target) end
	

function Osi.ShutdownCrimeSystem() end
	
--- @param NPC string
--- @param Bool integer
function Osi.CrimeEnableInterrogation(NPC, Bool) end
	
--- @param Criminal string
--- @param NPC string
--- @param IgnoreDuration integer
function Osi.CrimeIgnoreAllCrimesForCriminal(Criminal, NPC, IgnoreDuration) end
	
--- @param CrimeID integer
--- @param Criminal1 string
--- @param Criminal2 string
--- @param Criminal3 string
--- @param Criminal4 string
function Osi.CrimeResetInterrogationForCriminals(CrimeID, Criminal1, Criminal2, Criminal3, Criminal4) end
	
--- @param Target string
function Osi.JumpToTurn(Target) end
	
--- @param CrimeID integer
--- @param NPC string
function Osi.CrimeIgnoreCrime(CrimeID, NPC) end
	

function Osi.NotifyCharacterCreationFinished() end
	
--- @param Speaker string
--- @param Dialog integer
function Osi.SetHasDialog(Speaker, Dialog) end
	
--- @param WinnerTeamId integer
function Osi.ShowArenaResult(WinnerTeamId) end
	
--- @param Source string
--- @param Target string
function Osi.EnterCombat(Source, Target) end
	
--- @param Target string
function Osi.LeaveCombat(Target) end
	
--- @param Target string
--- @param Faction string
function Osi.SetFaction(Target, Faction) end
	
--- @param Target string
--- @param Bool integer
function Osi.SetInvulnerable_UseProcSetInvulnerable(Target, Bool) end
	
--- @param Character string
--- @param Restconsumable string
--- @param PartyRadius number
--- @param MinSafeDistance number
function Osi.UserRest(Character, Restconsumable, PartyRadius, MinSafeDistance) end
	

function Osi.FireOsirisEvents() end
	
--- @param Spline string
--- @param Character string
--- @param FadeTime number
--- @param HideUI integer
--- @param Freeze integer
--- @param StartIndex integer
function Osi.StartCameraSpline(Spline, Character, FadeTime, HideUI, Freeze, StartIndex) end
	
--- @param Spline string
--- @param Character string
function Osi.StopCameraSpline(Spline, Character) end
	
--- @param Object string
--- @param Damage integer
--- @param DamageType string
--- @param Source string
function Osi.ApplyDamage(Object, Damage, DamageType, Source) end
	
--- @param Source string
--- @param Target string
--- @param IgnoreVote integer
function Osi.MakePeace(Source, Target, IgnoreVote) end
	
--- @param Source string
--- @param Target string
--- @param IgnoreVote integer
function Osi.MakeWar(Source, Target, IgnoreVote) end
	
--- @param LevelTemplate string
function Osi.ActivatePersistentLevelTemplateWithCombat(LevelTemplate) end
	
--- @param LevelTemplate string
function Osi.ActivatePersistentLevelTemplate(LevelTemplate) end
	
--- @param Player string
--- @param Id string
function Osi.ReadyCheckStart(Player, Id) end
	
--- @param Modifier integer
function Osi.SetGlobalPriceModifier(Modifier) end
	
--- @param Character string
--- @param EventName string
function Osi.SonyRealtimeMultiplayerEvent(Character, EventName) end
	
--- @param Trigger string
--- @param Event string
function Osi.TriggerLaunchIterator(Trigger, Event) end
	
--- @param Trigger string
--- @param ItemTemplate string
function Osi.TriggerRemoveAllItemTemplates(Trigger, ItemTemplate) end
	
--- @param Trigger string
--- @param Character string
function Osi.TriggerRegisterForCharacter(Trigger, Character) end
	
--- @param Trigger string
--- @param Character string
function Osi.TriggerUnregisterForCharacter(Trigger, Character) end
	
--- @param Trigger string
function Osi.TriggerRegisterForPlayers(Trigger) end
	
--- @param Trigger string
function Osi.TriggerUnregisterForPlayers(Trigger) end
	
--- @param Trigger string
function Osi.TriggerRegisterForItems(Trigger) end
	
--- @param Trigger string
function Osi.TriggerUnregisterForItems(Trigger) end
	
--- @param Trigger string
--- @param Atmospherestring string
function Osi.TriggerSetAtmosphere(Trigger, Atmospherestring) end
	
--- @param Trigger string
function Osi.TriggerResetAtmosphere(Trigger) end
	
--- @param Trigger string
--- @param StateGroup string
--- @param State2 string
--- @param Recursive integer
function Osi.TriggerSetSoundState(Trigger, StateGroup, State2, Recursive) end
	
--- @param Trigger string
--- @param Name string
--- @param Value number
--- @param Recursive integer
function Osi.TriggerSetSoundRTPC(Trigger, Name, Value, Recursive) end
	
--- @param AreaTrigger string
--- @param Owner string
function Osi.TriggerSetItemOwner(AreaTrigger, Owner) end
	
--- @param AreaTrigger string
function Osi.TriggerClearItemOwner(AreaTrigger) end
	
--- @param Trigger string
--- @param ItemTemplate string
function Osi.TriggerClearItemTemplateOwners(Trigger, ItemTemplate) end
	
--- @param InstanceID integer
--- @param Actor string
function Osi.DialogAddActor(InstanceID, Actor) end
	
--- @param InstanceID integer
--- @param Actor string
--- @param Index integer
function Osi.DialogAddActorAt(InstanceID, Actor, Index) end
	
--- @param InstanceID integer
function Osi.DialogResume(InstanceID) end
	
--- @param Bark string
--- @param Source string
function Osi.StartVoiceBark(Bark, Source) end
	
--- @param Dialog string
--- @param Variable string
--- @param Value string
function Osi.DialogSetVariableString(Dialog, Variable, Value) end
	
--- @param Dialog string
--- @param Variable string
--- @param Value integer
function Osi.DialogSetVariableInt(Dialog, Variable, Value) end
	
--- @param Dialog string
--- @param Variable string
--- @param Value number
function Osi.DialogSetVariableFloat(Dialog, Variable, Value) end
	
--- @param Dialog string
--- @param Variable string
--- @param Value string
function Osi.DialogSetVariableFixedString(Dialog, Variable, Value) end
	
--- @param Dialog string
--- @param Variable string
--- @param StringHandleValue string
--- @param ReferenceStringValue string
function Osi.DialogSetVariableTranslatedString(Dialog, Variable, StringHandleValue, ReferenceStringValue) end
	
--- @param InstanceID integer
--- @param Variable string
--- @param Value string
function Osi.DialogSetVariableStringForInstance(InstanceID, Variable, Value) end
	
--- @param InstanceID integer
--- @param Variable string
--- @param Value integer
function Osi.DialogSetVariableIntForInstance(InstanceID, Variable, Value) end
	
--- @param InstanceID integer
--- @param Variable string
--- @param Value number
function Osi.DialogSetVariableFloatForInstance(InstanceID, Variable, Value) end
	
--- @param InstanceID integer
--- @param Variable string
--- @param Value string
function Osi.DialogSetVariableFixedStringForInstance(InstanceID, Variable, Value) end
	
--- @param InstanceID integer
--- @param Variable string
--- @param StringHandleValue string
--- @param ReferenceStringValue string
function Osi.DialogSetVariableTranslatedStringForInstance(InstanceID, Variable, StringHandleValue, ReferenceStringValue) end

--- @param object string
--- @param template string
--- @param replaceScripts boolean
--- @param replaceScale boolean
--- @param replaceStats boolean
function Osi.TransformKeepIcon(object, template, replaceScripts, replaceScale, replaceStats) end

--- @param skill string
--- @param source string
--- @param angleVariationDegrees number
--- @param distance number
function Osi.LaunchProjectileFromCharacterInCone(skill, source, angleVariationDegrees, distance) end

--#endregion

--#region Osiris Queries

--- @param GoalTitle string
--- @return integer Status
function Osi.SysStatus(GoalTitle) end
	
--- @param GoalTitle string
function Osi.SysIsCompleted(GoalTitle) end
	
--- @param GoalTitle string
function Osi.SysIsActive(GoalTitle) end
	
--- @param GoalTitle string
function Osi.SysIsSleeping(GoalTitle) end
	
--- @param GoalTitle string
function Osi.SysWasCompleted(GoalTitle) end
	
--- @param GoalTitle string
function Osi.SysWasActive(GoalTitle) end
	
--- @param GoalTitle string
function Osi.SysWasSleeping(GoalTitle) end
	
--- @param GoalTitle string
function Osi.SysWasDefined(GoalTitle) end
	
--- @param Predicate string
--- @param Arity integer
--- @return integer Count
function Osi.SysCount(Predicate, Arity) end
	
--- @return integer Major
--- @return integer Minor
--- @return integer V3
--- @return integer V4
function Osi.SysStoryVersion() end
	
--- @param A integer
--- @param B integer
--- @return integer Sum
function Osi.IntegerSum(A, B) end
	
--- @param A integer
--- @param B integer
--- @return integer Result
function Osi.IntegerSubtract(A, B) end
	
--- @param A integer
--- @param B integer
--- @return integer Product
function Osi.IntegerProduct(A, B) end
	
--- @param A integer
--- @param B integer
--- @return integer Quotient
function Osi.IntegerDivide(A, B) end
	
--- @param A integer
--- @param B integer
--- @return integer Minimum
function Osi.IntegerMin(A, B) end
	
--- @param A integer
--- @param B integer
--- @return integer Maximum
function Osi.IntegerMax(A, B) end
	
--- @param Num integer
--- @param Mod integer
--- @return integer Return
function Osi.IntegerModulo(Num, Mod) end
	
--- @param A number
--- @param B number
--- @return number Sum
function Osi.RealSum(A, B) end
	
--- @param A number
--- @param B number
--- @return number Result
function Osi.RealSubtract(A, B) end
	
--- @param A number
--- @param B number
--- @return number Product
function Osi.RealProduct(A, B) end
	
--- @param A number
--- @param B number
--- @return number Quotient
function Osi.RealDivide(A, B) end
	
--- @param A number
--- @param B number
--- @return number Minimum
function Osi.RealMin(A, B) end
	
--- @param A number
--- @param B number
--- @return number Maximum
function Osi.RealMax(A, B) end
	
--- @param R number
--- @return integer I
function Osi.Integer(R) end
	
--- @param I integer
--- @return number R
function Osi.Real(I) end
	
--- @param Modulo integer
--- @return integer Random
function Osi.Random(Modulo) end
	
--- @param Character string
--- @param Talent string
--- @return integer Bool
function Osi.CharacterHasTalent(Character, Talent) end
	
--- @param Character string
--- @return integer Level
function Osi.CharacterGetLevel(Character) end
	
--- @param Character string
--- @return integer Bool
function Osi.CharacterCanFight(Character) end
	
--- @param X number
--- @param Y number
--- @param Z number
--- @param TemplateId string
--- @param PlaySpawn integer
--- @return string Created
function Osi.CharacterCreateAtPosition(X, Y, Z, TemplateId, PlaySpawn) end
	
--- @param X number
--- @param Y number
--- @param Z number
--- @param TemplateId string
--- @param PlaySpawn integer
--- @return string Created
function Osi.TemporaryCharacterCreateAtPosition(X, Y, Z, TemplateId, PlaySpawn) end
	
--- @param Caster string
--- @param SummonTemplateId string
--- @param X number
--- @param Y number
--- @param Z number
--- @param SummonLifetime number
--- @param SummonCharacterLevel integer
--- @param SummoningAbilityLevel integer
--- @return string SummonedCharacter
function Osi.CharacterSummonAtPosition(Caster, SummonTemplateId, X, Y, Z, SummonLifetime, SummonCharacterLevel, SummoningAbilityLevel) end
	
--- @param X number
--- @param Y number
--- @param Z number
--- @param TemplateId string
--- @param Angle integer
--- @param PlaySpawn integer
--- @param Event string
--- @return string Created
function Osi.CharacterCreateAtPositionOutOfSightTo(X, Y, Z, TemplateId, Angle, PlaySpawn, Event) end
	
--- @param X number
--- @param Y number
--- @param Z number
--- @param TemplateId string
--- @param Angle integer
--- @param PlaySpawn integer
--- @param Event string
--- @return string Created
function Osi.TemporaryCharacterCreateAtPositionOutOfSightTo(X, Y, Z, TemplateId, Angle, PlaySpawn, Event) end
	
--- @param TemplateId string
--- @param ToTarget string
--- @param FromObject string
--- @param PlaySpawn integer
--- @param Event string
--- @return string Created
function Osi.CharacterCreateOutOfSightToObject(TemplateId, ToTarget, FromObject, PlaySpawn, Event) end
	
--- @param TemplateId string
--- @param ToTarget string
--- @param FromObject string
--- @param PlaySpawn integer
--- @param Event string
--- @return string Created
function Osi.TemporaryCharacterCreateOutOfSightToObject(TemplateId, ToTarget, FromObject, PlaySpawn, Event) end
	
--- @param Character string
--- @param Skill string
--- @return integer Bool
function Osi.CharacterHasSkill(Character, Skill) end
	
--- @param Character string
--- @return integer Amount
function Osi.CharacterGetAttributePoints(Character) end
	
--- @param Character string
--- @return integer Amount
function Osi.CharacterGetAbilityPoints(Character) end
	
--- @param Character string
--- @return integer Amount
function Osi.CharacterGetCivilAbilityPoints(Character) end
	
--- @param Character string
--- @return integer Amount
function Osi.CharacterGetTalentPoints(Character) end
	
--- @param Character string
--- @return integer Amount
function Osi.CharacterGetBaseSourcePoints(Character) end
	
--- @param Character string
--- @return integer Amount
function Osi.CharacterGetSourcePoints(Character) end
	
--- @param Character string
--- @return integer Amount
function Osi.CharacterGetMaxSourcePoints(Character) end
	
--- @param Character string
--- @return integer User
function Osi.CharacterGetReservedUserID(Character) end
	
--- @param User integer
--- @return string Character
function Osi.GetCurrentCharacter(User) end
	
--- @param Character string
--- @return integer IsControlled
function Osi.CharacterIsControlled(Character) end
	
--- @param Character string
--- @return integer Count
function Osi.CharacterGetGold(Character) end
	
--- @param Character string
--- @return integer Gold
function Osi.PartyGetGold(Character) end
	
--- @param Character string
--- @return integer Gold
function Osi.UserGetGold(Character) end
	
--- @param Character string
--- @return integer Bool
function Osi.CharacterIsSpectating(Character) end
	
--- @param Character string
--- @param Target string
--- @return integer Bool
function Osi.CharacterCanSee(Character, Target) end
	
--- @param Character string
--- @param Respec integer
--- @return integer Success
function Osi.CharacterAddToCharacterCreation(Character, Respec) end
	
--- @param Character string
--- @param Respec integer
--- @return integer Success
function Osi.GameMasterAddToCharacterCreation(Character, Respec) end
	
--- @param Character string
--- @return integer Bool
function Osi.CharacterIsPartyMember(Character) end
	
--- @param Character string
--- @param Target string
--- @return integer Bool
function Osi.CharacterIsInPartyWith(Character, Target) end
	
--- @param Character string
--- @param OtherCharacter string
--- @return integer Relation
function Osi.CharacterGetRelationToCharacter(Character, OtherCharacter) end
	
--- @param Character string
--- @param Player string
--- @return integer Attitude
function Osi.CharacterGetAttitudeTowardsPlayer(Character, Player) end
	
--- @param Character string
--- @return number Percentage
function Osi.CharacterGetHitpointsPercentage(Character) end
	
--- @param Character string
--- @return number Percentage
function Osi.CharacterGetArmorPercentage(Character) end
	
--- @param Character string
--- @return number Percentage
function Osi.CharacterGetMagicArmorPercentage(Character) end
	
--- @return string Character
function Osi.CharacterGetHostCharacter() end
	
--- @param Character string
--- @return string stringHandle
--- @return string referenceString
function Osi.CharacterGetDisplayName(Character) end
	
--- @param Trader string
--- @return integer Bool
function Osi.CharacterCanTrade(Trader) end
	
--- @param Character string
--- @return integer Bool
function Osi.CharacterIsInCombat(Character) end
	
--- @param Character string
--- @return integer Bool
function Osi.CharacterIsMoving(Character) end
	
--- @param Character string
--- @return integer Bool
function Osi.CharacterIsInFightMode(Character) end
	
--- @param Character string
--- @param Potion string
--- @return integer ConsumeHandle
function Osi.CharacterConsume(Character, Potion) end
	
--- @param Character string
--- @param Attribute string
--- @return integer Value
function Osi.CharacterGetBaseAttribute(Character, Attribute) end
	
--- @param Character string
--- @param Attribute string
--- @return integer Value
function Osi.CharacterGetAttribute(Character, Attribute) end
	
--- @param Character string
--- @return integer Incapacitated
function Osi.CharacterIsIncapacitated(Character) end
	
--- @param Character string
--- @param Ability string
--- @return integer Value
function Osi.CharacterGetAbility(Character, Ability) end
	
--- @param Character string
--- @param Ability string
--- @return integer Value
function Osi.CharacterGetBaseAbility(Character, Ability) end
	
--- @param Character string
--- @return string Owner
function Osi.CharacterGetOwner(Character) end
	
--- @param Character string
--- @return string ItemGUID
function Osi.CharacterGetEquippedWeapon(Character) end
	
--- @param Character string
--- @return string ItemGUID
function Osi.CharacterGetEquippedShield(Character) end
	
--- @param Character string
--- @param Slotname string
--- @return string ItemGUID
function Osi.CharacterGetEquippedItem(Character, Slotname) end
	
--- @param Character1 string
--- @param Character2 string
--- @return integer Bool
function Osi.CharactersAreGrouped(Character1, Character2) end
	
--- @param Character string
--- @return integer Value
function Osi.CharacterGetInventoryGoldValue(Character) end
	
--- @param Character string
--- @param ItemTemplate string
--- @return integer Value
function Osi.CharacterGetItemTemplateCount(Character, ItemTemplate) end
	
--- @param Character string
--- @return integer Bool
function Osi.CharacterIsFemale(Character) end
	
--- @param Character string
--- @return integer Bool
function Osi.CharacterCanSpotSneakers(Character) end
	
--- @param Character string
--- @param RecipeID string
--- @return integer Bool
function Osi.CharacterHasRecipeUnlocked(Character, RecipeID) end
	
--- @param Character string
--- @return integer Bool
function Osi.CharacterIsDead(Character) end
	
--- @param Character string
--- @return integer Bool
function Osi.CharacterIsDeadOrFeign(Character) end
	
--- @param Character string
--- @return integer Bool
function Osi.CharacterCanIgnoreActiveCrimes(Character) end
	
--- @param Character string
--- @return integer Bool
function Osi.CharacterIgnoreActiveCrimes(Character) end
	
--- @param Character string
--- @param Crime string
--- @return integer Bool
function Osi.CharacterIsCrimeEnabled(Character, Crime) end
	
--- @param Character string
--- @return string Region
function Osi.CharacterGetCrimeRegion(Character) end
	
--- @param Character string
--- @return integer InstanceID
function Osi.CharacterGetCrimeDialog(Character) end
	
--- @param Character string
--- @param OtherCharacter string
--- @return integer Bool
function Osi.CharacterIsEnemy(Character, OtherCharacter) end
	
--- @param Character string
--- @param OtherCharacter string
--- @return integer Bool
function Osi.CharacterIsAlly(Character, OtherCharacter) end
	
--- @param Character string
--- @param OtherCharacter string
--- @return integer Bool
function Osi.CharacterIsNeutral(Character, OtherCharacter) end
	
--- @param Character string
--- @return integer Bool
function Osi.CharacterCanUseWaypoints(Character) end
	
--- @param Character string
--- @param Ghost string
--- @return integer Bool
function Osi.CharacterCanSeeGhost(Character, Ghost) end
	
--- @param Character string
--- @return integer Bool
function Osi.CharacterIsSummon(Character) end
	
--- @param Character string
--- @return integer Bool
function Osi.CharacterIsPartyFollower(Character) end
	
--- @param Character string
--- @param TargetRace string
--- @return integer Bool
function Osi.CharacterIsPolymorphedInto(Character, TargetRace) end
	
--- @param Character string
--- @return integer Bool
function Osi.CharacterIsPolymorphInteractionDisabled(Character) end
	
--- @param Character string
--- @return integer Bool
function Osi.CharacterGameMaster(Character) end
	
--- @param Character string
--- @return integer Bool
function Osi.CharacterIsPlayer(Character) end
	
--- @param Character string
--- @return integer Bool
function Osi.CharacterHasLinkedGhost(Character) end
	
--- @param Character string
--- @return integer Bool
function Osi.CharacterCanSpotCrimes(Character) end
	
--- @param Character string
--- @param CrimeType string
--- @return integer Bool
function Osi.CharacterCanReactToCrime(Character, CrimeType) end
	
--- @param Player string
--- @param CanPolymorphOverride integer
--- @return string Race
function Osi.CharacterGetRace(Player, CanPolymorphOverride) end
	
--- @param Player string
--- @param CanPolymorphOverride integer
--- @return string Origin
function Osi.CharacterGetOrigin(Player, CanPolymorphOverride) end
	
--- @param Player string
--- @return string Instrument
function Osi.CharacterGetInstrument(Player) end
	
--- @param Owner string
--- @param Preset string
--- @return integer Price
function Osi.CharacterGetHenchmanPresetPrice(Owner, Preset) end
	
--- @param ItemTemplate string
--- @param X number
--- @param Y number
--- @param Z number
--- @return string Item
function Osi.CreateItemTemplateAtPosition(ItemTemplate, X, Y, Z) end
	
--- @param Index integer
--- @return string Template
function Osi.GetDebugItem(Index) end
	
--- @param Item string
--- @param Character string
--- @return integer Bool
function Osi.ItemIsInCharacterInventory(Item, Character) end
	
--- @param Item string
--- @param Character string
--- @param MoveAndReport integer
--- @return integer Bool
function Osi.ItemIsInPartyInventory(Item, Character, MoveAndReport) end
	
--- @param Item string
--- @param Character string
--- @param MoveAndReport integer
--- @return integer Bool
function Osi.ItemIsInUserInventory(Item, Character, MoveAndReport) end
	
--- @param Item string
--- @return integer Bool
function Osi.ItemIsInInventory(Item) end
	
--- @param Character string
--- @param Template string
--- @return integer Count
function Osi.ItemTemplateIsInCharacterInventory(Character, Template) end
	
--- @param Character string
--- @param Tag string
--- @return integer Count
function Osi.ItemTagIsInCharacterInventory(Character, Tag) end
	
--- @param Character string
--- @param Template string
--- @param Tag string
--- @return integer Count
function Osi.ItemTemplateTagIsInCharacterInventory(Character, Template, Tag) end
	
--- @param Character string
--- @param Template string
--- @param MoveAndReport integer
--- @return integer Count
function Osi.ItemTemplateIsInPartyInventory(Character, Template, MoveAndReport) end
	
--- @param Character string
--- @param Template string
--- @param MoveAndReport integer
--- @return integer Count
function Osi.ItemTemplateIsInUserInventory(Character, Template, MoveAndReport) end
	
--- @param Template string
--- @return string stringHandle
--- @return string referenceString
function Osi.ItemTemplateGetDisplayString(Template) end
	
--- @param Item string
--- @param Template string
--- @return integer Count
function Osi.ItemTemplateIsInContainer(Item, Template) end
	
--- @param Item string
--- @return integer Opened
function Osi.ItemIsOpened(Item) end
	
--- @param Door string
--- @return integer Opening
function Osi.DoorIsOpening(Door) end
	
--- @param Item string
--- @return integer Closed
function Osi.ItemIsClosed(Item) end
	
--- @param Door string
--- @return integer Closing
function Osi.DoorIsClosing(Door) end
	
--- @param Item string
--- @return integer Locked
function Osi.ItemIsLocked(Item) end
	
--- @param Item string
--- @return integer IsContainer
function Osi.ItemIsContainer(Item) end
	
--- @param Item string
--- @param OnUse string
--- @return integer Bool
function Osi.ItemHasOnUse(Item, OnUse) end
	
--- @param Item string
--- @return integer Bool
function Osi.ItemIsTorch(Item) end
	
--- @param Item string
--- @return integer Bool
function Osi.ItemIsEquipable(Item) end
	
--- @param Item string
--- @return string SlotName
function Osi.ItemGetEquipmentSlot(Item) end
	
--- @param Item string
--- @return integer Destroyed
function Osi.ItemIsDestroyed(Item) end
	
--- @param Item string
--- @return string Character
function Osi.ItemGetOwner(Item) end
	
--- @param Item string
--- @return string Character
function Osi.ItemGetOriginalOwner(Item) end
	
--- @param Item string
--- @return string Owner
function Osi.GetInventoryOwner(Item) end
	
--- @param Item string
--- @return integer bool
function Osi.ItemGetUseRemotely(Item) end
	
--- @param Item string
--- @return integer Bool
function Osi.ItemIsStoryItem(Item) end
	
--- @param Item string
--- @return integer Charges
function Osi.ItemGetCharges(Item) end
	
--- @param Item string
--- @return integer InitialCharges
function Osi.ItemGetMaxCharges(Item) end
	
--- @param Item string
--- @return integer Durability
function Osi.ItemGetDurability(Item) end
	
--- @param Item string
--- @return integer Amount
function Osi.ItemGetAmount(Item) end
	
--- @param Item string
--- @return integer HP
function Osi.ItemGetHealthPoints(Item) end
	
--- @param Item string
--- @return integer Bool
function Osi.ItemIsDestructible(Item) end
	
--- @param Item string
--- @return integer Bool
function Osi.ItemCanSitOn(Item) end
	
--- @param Item string
--- @return integer Bool
function Osi.ItemIsLadder(Item) end
	
--- @param Template string
--- @return integer Bool
function Osi.ItemTemplateCanSitOn(Template) end
	
--- @param Container string
--- @return integer Value
function Osi.ContainerGetGoldValue(Container) end
	
--- @param Item string
--- @return integer Value
function Osi.ItemGetGoldValue(Item) end
	
--- @param Character string
--- @param Template string
--- @return string Item
function Osi.GetItemForItemTemplateInInventory(Character, Template) end
	
--- @param Character string
--- @param Template string
--- @return string Item
function Osi.GetItemForItemTemplateInPartyInventory(Character, Template) end
	
--- @param Character string
--- @param Tag string
--- @return string Item
function Osi.CharacterFindTaggedItem(Character, Tag) end
	
--- @param Character string
--- @param Tag string
--- @param MoveAndReport integer
--- @return string Item
function Osi.PartyFindTaggedItem(Character, Tag, MoveAndReport) end
	
--- @param Character string
--- @param Tag string
--- @param MoveAndReport integer
--- @return string Item
function Osi.UserFindTaggedItem(Character, Tag, MoveAndReport) end
	
--- @param Character string
--- @param Tag string
--- @param Amount integer
--- @return integer AmountRemoved
function Osi.CharacterRemoveTaggedLocalItems(Character, Tag, Amount) end
	
--- @param Character string
--- @param Tag string
--- @param Amount integer
--- @return integer AmountRemoved
function Osi.PartyRemoveTaggedLocalItems(Character, Tag, Amount) end
	
--- @param Character string
--- @param Tag string
--- @param Amount integer
--- @return integer AmountRemoved
function Osi.UserRemoveTaggedLocalItems(Character, Tag, Amount) end
	
--- @param Character string
--- @param ToObject string
--- @param Tag string
--- @param Amount integer
--- @return integer AmountTransfered
function Osi.UserTransferTaggedLocalItems(Character, ToObject, Tag, Amount) end
	
--- @param Item string
--- @return integer Bool
function Osi.ItemIsPublicDomain(Item) end
	
--- @param Item string
--- @param Slot integer
--- @return string Template
function Osi.ItemGetRuneItemTemplate(Item, Slot) end
	
--- @param Character string
--- @param Item string
--- @param Slot integer
--- @return string Rune
function Osi.ItemRemoveRune(Character, Item, Slot) end
	
--- @param Item string
--- @param Boost string
--- @return integer Count
function Osi.ItemHasDeltaModifier(Item, Boost) end
	
--- @param InstanceID integer
--- @return integer Count
function Osi.DialogGetNumberOfInvolvedPlayers(InstanceID) end
	
--- @param InstanceID integer
--- @param Index integer
--- @return string Player
function Osi.DialogGetInvolvedPlayer(InstanceID, Index) end
	
--- @param InstanceID integer
--- @return string Category
function Osi.DialogGetCategory(InstanceID) end
	
--- @param InstanceID integer
--- @return integer NumberOfNPCs
function Osi.DialogGetNumberOfInvolvedNPCs(InstanceID) end
	
--- @param InstanceID integer
--- @param Index integer
--- @return string NPC
function Osi.DialogGetInvolvedNPC(InstanceID, Index) end
	
--- @param Flag string
--- @return integer FlagState
function Osi.GlobalGetFlag(Flag) end
	
--- @param Target string
--- @param Flag string
--- @return integer FlagState
function Osi.ObjectGetFlag(Target, Flag) end
	
--- @param Target string
--- @param Flag string
--- @return integer FlagState
function Osi.ObjectGetDialogFlag(Target, Flag) end
	
--- @param Character string
--- @param Flag string
--- @return integer FlagState
function Osi.UserGetFlag(Character, Flag) end
	
--- @param Character string
--- @param Flag string
--- @return integer FlagState
function Osi.PartyGetFlag(Character, Flag) end
	
--- @param DialogInstance integer
--- @param LocalEvent string
--- @return integer Value
function Osi.DialogGetLocalFlag(DialogInstance, LocalEvent) end
	
--- @param Object string
--- @return integer Bool
function Osi.ObjectIsCharacter(Object) end
	
--- @param Object string
--- @return string Statname
function Osi.GetStatString(Object) end
	
--- @param Object string
--- @return integer Exists
function Osi.ObjectExists(Object) end
	
--- @param Object string
--- @return integer IsGlobal
function Osi.ObjectIsGlobal(Object) end
	
--- @param Object string
--- @return integer Bool
function Osi.ObjectIsItem(Object) end
	
--- @param Object string
--- @param Trigger string
--- @return integer Bool
function Osi.ObjectIsInTrigger(Object, Trigger) end
	
--- @param x number
--- @param Y number
--- @param Z number
--- @param Trigger string
--- @return integer Bool
function Osi.PositionIsInTrigger(x, Y, Z, Trigger) end
	
--- @param Object string
--- @return integer Bool
function Osi.ObjectIsOnStage(Object) end
	
--- @param SourceX number
--- @param SourceY number
--- @param SourceZ number
--- @param Radius number
--- @param Object string
--- @return number ValidPositionX
--- @return number ValidPositionY
--- @return number ValidPositionZ
function Osi.FindValidPosition(SourceX, SourceY, SourceZ, Radius, Object) end
	
--- @param Source string
--- @param Target string
--- @return integer Bool
function Osi.HasLineOfSight(Source, Target) end
	
--- @param Object string
--- @param FxName string
--- @param BoneName string
--- @return integer FxHandle
function Osi.PlayLoopEffect(Object, FxName, BoneName) end
	
--- @param Object string
--- @param Target string
--- @param FxName string
--- @param SourceBone string
--- @param TargetBone string
--- @return integer FxHandle
function Osi.PlayLoopBeamEffect(Object, Target, FxName, SourceBone, TargetBone) end
	
--- @return number Distance
function Osi.GetMaxCameraDistance() end
	
--- @param Quest string
--- @param Status string
--- @return integer Result
function Osi.QuestUpdateExists(Quest, Status) end
	
--- @param Character string
--- @param Quest string
--- @return integer Bool
function Osi.QuestAccepted(Character, Quest) end
	
--- @param Character string
--- @param Quest string
--- @return integer Bool
function Osi.QuestIsShared(Character, Quest) end
	
--- @param QuestID string
--- @param StateID string
--- @return integer Bool
function Osi.QuestIsSubquestEntry(QuestID, StateID) end
	
--- @param QuestID string
--- @param StateID string
--- @return integer Bool
function Osi.QuestIsMysteryEntry(QuestID, StateID) end
	
--- @param Target string
--- @return integer Bool
function Osi.IsInArena(Target) end
	
--- @param Character string
--- @param Quest string
--- @return integer Bool
function Osi.QuestIsClosed(Character, Quest) end
	
--- @param Quest string
--- @return string Level
function Osi.QuestGetBroadcastLevel(Quest) end
	
--- @param Character string
--- @param Quest string
--- @param Update string
--- @return integer Bool
function Osi.QuestHasUpdate(Character, Quest, Update) end
	
--- @param X number
--- @param Z number
--- @param Threshold number
--- @return integer Bool
function Osi.IsShrouded(X, Z, Threshold) end
	
--- @param Target string
--- @return string Surface
function Osi.GetSurfaceGroundAt(Target) end
	
--- @param Target string
--- @return string Surface
function Osi.GetSurfaceCloudAt(Target) end
	
--- @param Target string
--- @return string OwnerCharacter
--- @return string OwnerItem
function Osi.GetSurfaceGroundOwnerAt(Target) end
	
--- @param Target string
--- @return string OwnerCharacter
--- @return string OwnerItem
function Osi.GetSurfaceCloudOwnerAt(Target) end
	
--- @param Surface string
--- @return integer Index
function Osi.GetSurfaceTypeIndex(Surface) end
	
--- @param SurfaceIndex integer
--- @return string SurfaceName
function Osi.GetSurfaceNameByTypeIndex(SurfaceIndex) end
	
--- @param OwnerObject string
--- @param FollowObject string
--- @param SurfaceType string
--- @param Radius number
--- @param LifeTime number
--- @return integer SurfaceActionHandle
function Osi.DrawSurfaceOnPath(OwnerObject, FollowObject, SurfaceType, Radius, LifeTime) end
	
--- @param Target string
--- @param SurfaceLayer integer
--- @return integer SurfaceSize
function Osi.GetSurfaceSize(Target, SurfaceLayer) end
	
--- @param Target string
--- @param SurfaceLayer integer
--- @return integer SurfaceTurns
function Osi.GetSurfaceTurns(Target, SurfaceLayer) end
	
--- @param FxName string
--- @param X number
--- @param Y number
--- @param Z number
--- @return integer FxHandle
function Osi.PlayLoopEffectAtPosition(FxName, X, Y, Z) end
	
--- @param FxName string
--- @param Scale number
--- @param X number
--- @param Y number
--- @param Z number
--- @return integer FxHandle
function Osi.PlayScaledLoopEffectAtPosition(FxName, Scale, X, Y, Z) end
	
--- @param CombatID integer
--- @return integer NumPlayers
function Osi.CombatGetNumberOfInvolvedPlayers(CombatID) end
	
--- @param CombatID integer
--- @return integer NumPartyMembers
function Osi.CombatGetNumberOfInvolvedPartyMembers(CombatID) end
	
--- @param CombatID integer
--- @param PlayerIndex integer
--- @return string Player
function Osi.CombatGetInvolvedPlayer(CombatID, PlayerIndex) end
	
--- @param CombatID integer
--- @param PartyMemberIndex integer
--- @return string PartyMember
function Osi.CombatGetInvolvedPartyMember(CombatID, PartyMemberIndex) end
	
--- @param Player string
--- @return integer CombatID
function Osi.CombatGetIDForCharacter(Player) end
	
--- @param CombatID integer
--- @return integer Active
function Osi.IsCombatActive(CombatID) end
	
--- @param CombatID integer
--- @return string CurrentEntity
function Osi.CombatGetActiveEntity(CombatID) end
	
--- @param Character string
--- @return string MultiplayerCharacter
function Osi.GetMultiplayerCharacter(Character) end
	
--- @return integer Bool
function Osi.HasKickstarterDialogReward() end
	
--- @return integer Bool
function Osi.IsHardcoreMode() end
	
--- @param Character string
--- @param DLCName string
--- @return integer HasDLC
function Osi.CharacterHasDLC(Character, DLCName) end
	
--- @return integer UserCount
function Osi.GetUserCount() end
	
--- @param Source string
--- @param VarName string
--- @return string UUID
function Osi.GetVarObject(Source, VarName) end
	
--- @param Source string
--- @param VarName string
--- @return integer VarValue
function Osi.GetVarInteger(Source, VarName) end
	
--- @param Source string
--- @param VarName string
--- @return number VarValue
function Osi.GetVarFloat(Source, VarName) end
	
--- @param Source string
--- @param VarName string
--- @return string VarValue
function Osi.GetVarString(Source, VarName) end
	
--- @param Source string
--- @param VarName string
--- @return string VarValue
function Osi.GetVarFixedString(Source, VarName) end
	
--- @param Target string
--- @param VarName string
--- @return number X
--- @return number Y
--- @return number Z
function Osi.GetVarFloat3(Target, VarName) end
	
--- @param Target string
--- @param Tag string
--- @return integer Bool
function Osi.IsTagged(Target, Tag) end
	
--- @param Target string
--- @param Status string
--- @return integer Bool
function Osi.HasActiveStatus(Target, Status) end
	
--- @param Target string
--- @param Status string
--- @return integer Bool
function Osi.HasAppliedStatus(Target, Status) end
	
--- @param Target string
--- @return string UUID
function Osi.GetUUID(Target) end
	
--- @param Target string
--- @return number X
--- @return number Y
--- @return number Z
function Osi.GetPosition(Target) end
	
--- @param Target string
--- @return number X
--- @return number Y
--- @return number Z
function Osi.GetRotation(Target) end
	
--- @param Target string
--- @return string Player
--- @return number Distance
function Osi.GetClosestPlayer(Target) end
	
--- @param Target string
--- @param Talent string
--- @return string Player
--- @return number Distance
function Osi.GetClosestPlayerWithTalent(Target, Talent) end
	
--- @param Target string
--- @return string Player
--- @return number Distance
function Osi.GetClosestAlivePlayer(Target) end
	
--- @param Target string
--- @param UserID integer
--- @return string Player
--- @return number Distance
function Osi.GetClosestAliveUserPlayer(Target, UserID) end
	
--- @param X number
--- @param Y number
--- @param Z number
--- @return string Player
--- @return number Distance
function Osi.GetClosestPlayerToPosition(X, Y, Z) end
	
--- @param Object string
--- @return string Region
function Osi.GetRegion(Object) end
	
--- @param UserId integer
--- @return string UserName
function Osi.GetUserName(UserId) end
	
--- @param UserId integer
--- @return string UserProfileID
function Osi.GetUserProfileID(UserId) end
	
--- @param CrimeID integer
--- @return string Type
function Osi.CrimeGetType(CrimeID) end
	
--- @param CrimeID integer
--- @return number Range
function Osi.CrimeGetDetectionRange(CrimeID) end
	
--- @param CrimeID integer
--- @return integer Tension
function Osi.CrimeGetTension(CrimeID) end
	
--- @return integer CrimeID
function Osi.CrimeGetNewID() end
	
--- @param Character string
--- @return integer Bool
function Osi.CrimeIsTensionOverWarningTreshold(Character) end
	
--- @param CrimeID integer
--- @param Searcher string
--- @param Criminal1 string
--- @param Criminal2 string
--- @param Criminal3 string
--- @param Criminal4 string
--- @return integer EvidenceFoundForCurrentCrime
--- @return integer EvidenceFound2
--- @return integer GuiltyFound
function Osi.CrimeFindEvidence(CrimeID, Searcher, Criminal1, Criminal2, Criminal3, Criminal4) end
	
--- @param CrimeID integer
--- @return string LeadInvestigator
function Osi.CrimeGetLeadInvestigator(CrimeID) end
	
--- @param Target string
--- @return integer Bool
function Osi.IsBoss(Target) end
	
--- @param object1 string
--- @param object2 string
--- @return number Dist
function Osi.GetDistanceTo(object1, object2) end
	
--- @param Object string
--- @param X number
--- @param Y number
--- @param Z number
--- @return number Dist
function Osi.GetDistanceToPosition(Object, X, Y, Z) end
	
--- @param x0 number
--- @param z0 number
--- @param x1 number
--- @param z1 number
--- @return integer Angle
function Osi.GetAngleTo(x0, z0, x1, z1) end
	
--- @param Object string
--- @param LocX number
--- @param LocY number
--- @param LocZ number
--- @param LocRotX number
--- @param LocRotY number
--- @param LocRotZ number
--- @return number WorldX
--- @return number WorldY
--- @return number WorldZ
--- @return number WorldRotX
--- @return number WorldRotY
--- @return number WorldRotZ
function Osi.GetWorldTransformFromLocal(Object, LocX, LocY, LocZ, LocRotX, LocRotY, LocRotZ) end
	
--- @param CrimeArea string
--- @return integer Modifier
function Osi.CrimeAreaGetTensionModifier(CrimeArea) end
	
--- @param CrimeID integer
--- @return string CrimeVictim
function Osi.CrimeGetVictim(CrimeID) end
	
--- @param CrimeID integer
--- @param Index integer
--- @return string Evidence
function Osi.CrimeGetEvidence(CrimeID, Index) end
	
--- @param CrimeID integer
--- @return integer NumEvidence
function Osi.CrimeGetNumberOfEvidence(CrimeID) end
	
--- @param CrimeID integer
--- @return integer Bool
function Osi.CrimeIsContinuous(CrimeID) end
	
--- @param CrimeID integer
--- @return string Criminal1
--- @return string Criminal2
--- @return string Criminal3
--- @return string Criminal4
function Osi.CrimeGetCriminals(CrimeID) end
	
--- @param Criminal string
--- @param CrimeType string
--- @param Victim string
--- @param X number
--- @param Y number
--- @param Z number
--- @return integer Bool
function Osi.CrimeIsAnyNPCGoingToReact(Criminal, CrimeType, Victim, X, Y, Z) end
	
--- @param OldLead string
--- @param CrimeID integer
--- @param NewLead string
--- @return integer Bool
function Osi.CrimeTransferLeadershipTo(OldLead, CrimeID, NewLead) end
	
--- @param CrimeID integer
--- @param Criminal string
--- @return integer Bool
function Osi.CrimeAddCriminal(CrimeID, Criminal) end
	
--- @param Target string
--- @return string Faction
function Osi.GetFaction(Target) end
	
--- @param Target string
--- @return string Template
function Osi.GetTemplate(Target) end
	
--- @param Character string
--- @param MinSafeDistance number
--- @return integer Bool
function Osi.HasEnemyInRange(Character, MinSafeDistance) end
	
--- @param Character string
--- @param Restconsumable string
--- @param PartyRadius number
--- @param MinSafeDistance number
--- @return integer Bool
function Osi.CanUserRest(Character, Restconsumable, PartyRadius, MinSafeDistance) end
	
--- @param Character string
--- @param SkillID string
--- @return integer Bool
function Osi.IsSkillActive(Character, SkillID) end
	
--- @param SkillID string
--- @return integer Bool
function Osi.IsSourceSkill(SkillID) end
	
--- @param StatusID string
--- @return string StatusType
function Osi.GetStatusType(StatusID) end
	
--- @param Source string
--- @param StatusID string
--- @return integer Turns
function Osi.GetStatusTurns(Source, StatusID) end
	
--- @param StatusID string
--- @return string HealStat
function Osi.GetHealStat(StatusID) end
	
--- @param LevelName string
--- @return integer Bool
function Osi.IsGameLevel(LevelName) end
	
--- @param LevelName string
--- @return integer Bool
function Osi.IsCharacterCreationLevel(LevelName) end
	
--- @param Player string
--- @param ItemTemplate string
--- @return integer Bool
function Osi.HasRecipeUnlockedWithIngredient(Player, ItemTemplate) end
	
--- @return integer Modifier
function Osi.GetGlobalPriceModifier() end
	
--- @param Difficulty string
--- @param Level integer
--- @return integer AttributeValue
function Osi.AttributeGetDifficultyLevelMappedValue(Difficulty, Level) end
	
--- @param Template string
--- @param Trigger string
--- @return integer Count
function Osi.CharacterTemplatesInTrigger(Template, Trigger) end
	
--- @param Trigger string
--- @return number X
--- @return number Y
--- @return number Z
function Osi.GetRandomPositionInTrigger(Trigger) end
	
--- @param StringA string
--- @param StringB string
--- @return string Result
function Osi.StringConcatenate(StringA, StringB) end
	
--- @param StringA string
--- @param StringB string
--- @return integer Bool
function Osi.StringContains(StringA, StringB) end
	
--- @param String string
--- @param Start integer
--- @param Count integer
--- @return string Result
function Osi.StringSub(String, Start, Count) end
	
--- @param Integer integer
--- @return string Result
function Osi.IntegertoString(Integer) end
	
--- @param GUIDstring string
--- @return string Result
function Osi.String(GUIDstring) end
	
--- @return integer Bool
function Osi.IsSwitch() end
	
--- @param Dialog string
--- @param MarkForInteractiveDialog integer
--- @param Speaker1 string
--- @param Speaker2 string
--- @param Speaker3 string
--- @param Speaker4 string
--- @param Speaker5 string
--- @param Speaker6 string
--- @return integer Success
function Osi.StartDialog_Internal(Dialog, MarkForInteractiveDialog, Speaker1, Speaker2, Speaker3, Speaker4, Speaker5, Speaker6) end
	
--- @param Dialog string
--- @param MarkForInteractiveDialog integer
--- @param Speaker1 string
--- @param Speaker2 string
--- @param Speaker3 string
--- @param Speaker4 string
--- @param Speaker5 string
--- @param Speaker6 string
--- @return integer Success
function Osi.StartDialog_Internal_NoDeadCheck(Dialog, MarkForInteractiveDialog, Speaker1, Speaker2, Speaker3, Speaker4, Speaker5, Speaker6) end
	
--- @param CrimeID integer
--- @param Dialog string
--- @param MarkForInteractiveDialog integer
--- @param NPC string
--- @param Criminal1 string
--- @param Criminal2 string
--- @param Criminal3 string
--- @param Criminal4 string
--- @return integer success
function Osi.DialogStartCrimeDialog(CrimeID, Dialog, MarkForInteractiveDialog, NPC, Criminal1, Criminal2, Criminal3, Criminal4) end
	
--- @param InstanceID integer
--- @return integer IsCrimeDialog
function Osi.DialogIsCrimeDialog(InstanceID) end
	
--- @param InstanceID integer
--- @param Actor string
--- @return integer success
function Osi.DialogRemoveActorFromDialog(InstanceID, Actor) end
	
--- @param Dialog string
--- @param ParentInstanceID integer
--- @param NewInstanceID integer
--- @param Player1 string
--- @param Player2 string
--- @param Player3 string
--- @param Player4 string
--- @return integer success
function Osi.DialogStartPartyDialog(Dialog, ParentInstanceID, NewInstanceID, Player1, Player2, Player3, Player4) end
	
--- @param Dialog string
--- @param ParentInstanceID integer
--- @param NewInstanceID integer
--- @param Player1 string
--- @param Player2 string
--- @param Player3 string
--- @param Player4 string
--- @return integer success
function Osi.DialogStartChildDialog(Dialog, ParentInstanceID, NewInstanceID, Player1, Player2, Player3, Player4) end
	
--- @param Speaker string
--- @return integer success
function Osi.IsSpeakerReserved(Speaker) end
	
--- @param Character string
--- @return integer HasDefaultDialog
function Osi.HasDefaultDialog(Character) end
	
--- @param Character string
--- @param Player string
--- @return string Dialog
--- @return integer Automated
function Osi.StartDefaultDialog(Character, Player) end
	
--- @param Number integer
--- @return string Value
function Osi.GetTextEventParamString(Number) end
	
--- @param Number integer
--- @return integer Value
function Osi.GetTextEventParamInteger(Number) end
	
--- @param Number integer
--- @return number Value
function Osi.GetTextEventParamReal(Number) end
	
--- @param Number integer
--- @return string Value
function Osi.GetTextEventParamUUID(Number) end

--- @param item string
--- @return integer
function Osi.ItemGetLevel(item) end

--#endregion

--#region Extender Calls/Queries

--- @param Message string
function Osi.NRD_DebugLog(Message) end
	
--- @param Event string
--- @param Count integer
function Osi.NRD_ForLoop(Event, Count) end
	
--- @param Object string
--- @param Event string
--- @param Count integer
function Osi.NRD_ForLoop(Object, Event, Count) end
	
--- @param ObjectGuid string
--- @param Event string
function Osi.NRD_IterateStatuses(ObjectGuid, Event) end
	
--- @param Object string
--- @param StatusHandle integer
--- @param Attribute string
--- @param Value integer
function Osi.NRD_StatusSetInt(Object, StatusHandle, Attribute, Value) end
	
--- @param Object string
--- @param StatusHandle integer
--- @param Attribute string
--- @param Value number
function Osi.NRD_StatusSetReal(Object, StatusHandle, Attribute, Value) end
	
--- @param Object string
--- @param StatusHandle integer
--- @param Attribute string
--- @param Value string
function Osi.NRD_StatusSetString(Object, StatusHandle, Attribute, Value) end
	
--- @param Object string
--- @param StatusHandle integer
--- @param Attribute string
--- @param Value string
function Osi.NRD_StatusSetGuidString(Object, StatusHandle, Attribute, Value) end
	
--- @param Object string
--- @param StatusHandle integer
--- @param Attribute string
--- @param X number
--- @param Y number
--- @param Z number
function Osi.NRD_StatusSetVector3(Object, StatusHandle, Attribute, X, Y, Z) end
	
--- @param Object string
--- @param StatusHandle integer
--- @param PreventApply integer
function Osi.NRD_StatusPreventApply(Object, StatusHandle, PreventApply) end
	
--- @param GameActionHandle integer
function Osi.NRD_GameActionDestroy(GameActionHandle) end
	
--- @param GameActionHandle integer
--- @param LifeTime number
function Osi.NRD_GameActionSetLifeTime(GameActionHandle, LifeTime) end
	

function Osi.NRD_ProjectilePrepareLaunch() end
	

function Osi.NRD_ProjectileLaunch() end
	
--- @param Property string
--- @param Value integer
function Osi.NRD_ProjectileSetInt(Property, Value) end
	
--- @param Property string
--- @param Value string
function Osi.NRD_ProjectileSetString(Property, Value) end
	
--- @param Property string
--- @param X number
--- @param Y number
--- @param Z number
function Osi.NRD_ProjectileSetVector3(Property, X, Y, Z) end
	
--- @param Property string
--- @param Value string
function Osi.NRD_ProjectileSetGuidString(Property, Value) end
	
--- @param DamageType string
--- @param Amount integer
function Osi.NRD_ProjectileAddDamage(DamageType, Amount) end
	
--- @param HitHandle integer
function Osi.NRD_HitExecute(HitHandle) end
	
--- @param HitHandle integer
--- @param Property string
--- @param Value integer
function Osi.NRD_HitSetInt(HitHandle, Property, Value) end
	
--- @param HitHandle integer
--- @param Property string
--- @param Value string
function Osi.NRD_HitSetString(HitHandle, Property, Value) end
	
--- @param HitHandle integer
--- @param Property string
--- @param X number
--- @param Y number
--- @param Z number
function Osi.NRD_HitSetVector3(HitHandle, Property, X, Y, Z) end
	
--- @param HitHandle integer
function Osi.NRD_HitClearAllDamage(HitHandle) end
	
--- @param HitHandle integer
--- @param DamageType string
function Osi.NRD_HitClearDamage(HitHandle, DamageType) end
	
--- @param HitHandle integer
--- @param DamageType string
--- @param Amount integer
function Osi.NRD_HitAddDamage(HitHandle, DamageType, Amount) end
	
--- @param Object string
--- @param StatusHandle integer
function Osi.NRD_HitStatusClearAllDamage(Object, StatusHandle) end
	
--- @param Object string
--- @param StatusHandle integer
--- @param DamageType string
function Osi.NRD_HitStatusClearDamage(Object, StatusHandle, DamageType) end
	
--- @param Object string
--- @param StatusHandle integer
--- @param DamageType string
--- @param Amount integer
function Osi.NRD_HitStatusAddDamage(Object, StatusHandle, DamageType, Amount) end
	
--- @param Character string
--- @param SkillId string
--- @param Cooldown number
function Osi.NRD_SkillSetCooldown(Character, SkillId, Cooldown) end
	
--- @param Character string
--- @param Slot integer
--- @param SkillId string
function Osi.NRD_SkillBarSetSkill(Character, Slot, SkillId) end
	
--- @param Character string
--- @param Slot integer
--- @param Item string
function Osi.NRD_SkillBarSetItem(Character, Slot, Item) end
	
--- @param Character string
--- @param Slot integer
function Osi.NRD_SkillBarClear(Character, Slot) end
	
--- @param Player string
--- @param Attribute string
--- @param Value integer
function Osi.NRD_PlayerSetBaseAttribute(Player, Attribute, Value) end
	
--- @param Player string
--- @param Ability string
--- @param Value integer
function Osi.NRD_PlayerSetBaseAbility(Player, Ability, Value) end
	
--- @param Player string
--- @param Talent string
--- @param HasTalent integer
function Osi.NRD_PlayerSetBaseTalent(Player, Talent, HasTalent) end
	
--- @param Player string
--- @param Property string
--- @param Value integer
function Osi.NRD_PlayerSetCustomDataInt(Player, Property, Value) end
	
--- @param Player string
--- @param Property string
--- @param Value string
function Osi.NRD_PlayerSetCustomDataString(Player, Property, Value) end
	
--- @param Item string
--- @param EventName string
function Osi.NRD_ItemIterateDeltaModifiers(Item, EventName) end
	
--- @param Item string
--- @param IsIdentified integer
function Osi.NRD_ItemSetIdentified(Item, IsIdentified) end
	
--- @param Item string
--- @param Stat string
--- @param Value integer
function Osi.NRD_ItemSetPermanentBoostInt(Item, Stat, Value) end
	
--- @param Item string
--- @param Stat string
--- @param Value number
function Osi.NRD_ItemSetPermanentBoostReal(Item, Stat, Value) end
	
--- @param Item string
--- @param Stat string
--- @param Value string
function Osi.NRD_ItemSetPermanentBoostString(Item, Stat, Value) end
	
--- @param Item string
--- @param Ability string
--- @param Points integer
function Osi.NRD_ItemSetPermanentBoostAbility(Item, Ability, Points) end
	
--- @param Item string
--- @param Talent string
--- @param HasTalent integer
function Osi.NRD_ItemSetPermanentBoostTalent(Item, Talent, HasTalent) end
	
--- @param TemplateGuid string
function Osi.NRD_ItemConstructBegin(TemplateGuid) end
	
--- @param Item string
function Osi.NRD_ItemCloneBegin(Item) end
	
--- @param Property string
--- @param Value integer
function Osi.NRD_ItemCloneSetInt(Property, Value) end
	
--- @param Property string
--- @param Value string
function Osi.NRD_ItemCloneSetString(Property, Value) end
	

function Osi.NRD_ItemCloneResetProgression() end
	
--- @param BoostType string
--- @param Boost2 string
function Osi.NRD_ItemCloneAddBoost(BoostType, Boost2) end
	
--- @param Character string
--- @param Stat string
--- @param Value integer
function Osi.NRD_CharacterSetStatInt(Character, Stat, Value) end
	
--- @param Character string
--- @param Stat string
--- @param Value integer
function Osi.NRD_CharacterSetPermanentBoostInt(Character, Stat, Value) end
	
--- @param Character string
--- @param Talent string
--- @param HasTalent integer
function Osi.NRD_CharacterSetPermanentBoostTalent(Character, Talent, HasTalent) end
	
--- @param Character string
--- @param Talent string
--- @param IsDisabled integer
function Osi.NRD_CharacterDisableTalent(Character, Talent, IsDisabled) end
	
--- @param Character string
--- @param IsGlobal integer
function Osi.NRD_CharacterSetGlobal(Character, IsGlobal) end
	
--- @param Character string
--- @param Event string
function Osi.NRD_CharacterIterateSkills(Character, Event) end
	
--- @param Character string
--- @param Item string
--- @param Slot string
--- @param ConsumeAP integer
--- @param CheckRequirements integer
--- @param UpdateVitality integer
--- @param UseWeaponAnimType integer
function Osi.NRD_CharacterEquipItem(Character, Item, Slot, ConsumeAP, CheckRequirements, UpdateVitality, UseWeaponAnimType) end
	
--- @param Object string
--- @param Flag integer
--- @param Value integer
function Osi.NRD_ObjectSetInternalFlag(Object, Flag, Value) end
	
--- @param Character string
--- @param StatId string
--- @param Value integer
function Osi.NRD_CharacterSetCustomStat(Character, StatId, Value) end
	
--- @param BootstrapMods integer
function Osi.NRD_LuaReset(BootstrapMods) end
	
--- @param BootstrapMods integer
--- @param ResetServer integer
--- @param ResetClient integer
function Osi.NRD_LuaReset(BootstrapMods, ResetServer, ResetClient) end
	
--- @param ModNameGuid string
--- @param FileName string
function Osi.NRD_LuaLoad(ModNameGuid, FileName) end
	
--- @param Func string
function Osi.NRD_LuaCall(Func) end
	
--- @param Func string
--- @param Arg1 string
function Osi.NRD_LuaCall(Func, Arg1) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
function Osi.NRD_LuaCall(Func, Arg1, Arg2) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
function Osi.NRD_LuaCall(Func, Arg1, Arg2, Arg3) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
function Osi.NRD_LuaCall(Func, Arg1, Arg2, Arg3, Arg4) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
function Osi.NRD_LuaCall(Func, Arg1, Arg2, Arg3, Arg4, Arg5) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
function Osi.NRD_LuaCall(Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
function Osi.NRD_LuaCall(Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
function Osi.NRD_LuaCall(Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
--- @param Arg9 string
function Osi.NRD_LuaCall(Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
--- @param Arg9 string
--- @param Arg10 string
function Osi.NRD_LuaCall(Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10) end
	
--- @param Mod string
--- @param Func string
function Osi.NRD_ModCall(Mod, Func) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
function Osi.NRD_ModCall(Mod, Func, Arg1) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
function Osi.NRD_ModCall(Mod, Func, Arg1, Arg2) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
function Osi.NRD_ModCall(Mod, Func, Arg1, Arg2, Arg3) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
function Osi.NRD_ModCall(Mod, Func, Arg1, Arg2, Arg3, Arg4) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
function Osi.NRD_ModCall(Mod, Func, Arg1, Arg2, Arg3, Arg4, Arg5) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
function Osi.NRD_ModCall(Mod, Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
function Osi.NRD_ModCall(Mod, Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
function Osi.NRD_ModCall(Mod, Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
--- @param Arg9 string
function Osi.NRD_ModCall(Mod, Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
--- @param Arg9 string
--- @param Arg10 string
function Osi.NRD_ModCall(Mod, Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10) end
	
--- @param Character string
function Osi.NRD_BreakOnCharacter(Character) end
	
--- @param Item string
function Osi.NRD_BreakOnItem(Item) end
	
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
function Osi.NRD_Experiment(Arg1, Arg2, Arg3) end
	
--- @param Message string
function Osi.NRD_ShowErrorMessage(Message) end
	
--- @param Path string
--- @param Contents string
function Osi.NRD_SaveFile(Path, Contents) end
	
--- @param A string
--- @param B string
--- @return integer Result
function Osi.NRD_StringCompare(A, B) end
	
--- @param String string
--- @return integer Length
function Osi.NRD_StringLength(String) end
	
--- @param Format string
--- @return string Result
function Osi.NRD_StringFormat(Format) end
	
--- @param Format string
--- @param Arg1 string
--- @return string Result
function Osi.NRD_StringFormat(Format, Arg1) end
	
--- @param Format string
--- @param Arg1 string
--- @param Arg2 string
--- @return string Result
function Osi.NRD_StringFormat(Format, Arg1, Arg2) end
	
--- @param Format string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @return string Result
function Osi.NRD_StringFormat(Format, Arg1, Arg2, Arg3) end
	
--- @param Format string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @return string Result
function Osi.NRD_StringFormat(Format, Arg1, Arg2, Arg3, Arg4) end
	
--- @param Format string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @return string Result
function Osi.NRD_StringFormat(Format, Arg1, Arg2, Arg3, Arg4, Arg5) end
	
--- @param Format string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @return string Result
function Osi.NRD_StringFormat(Format, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) end
	
--- @param Format string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @return string Result
function Osi.NRD_StringFormat(Format, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) end
	
--- @param Format string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
--- @return string Result
function Osi.NRD_StringFormat(Format, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8) end
	
--- @param Format string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
--- @param Arg9 string
--- @return string Result
function Osi.NRD_StringFormat(Format, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9) end
	
--- @param Format string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
--- @param Arg9 string
--- @param Arg10 string
--- @return string Result
function Osi.NRD_StringFormat(Format, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10) end
	
--- @param String string
--- @param From integer
--- @param Length integer
--- @return string Result
function Osi.NRD_Substring(String, From, Length) end
	
--- @param String string
--- @param Regex string
--- @param FullMatch integer
--- @return integer Matched
function Osi.NRD_RegexMatch(String, Regex, FullMatch) end
	
--- @param String string
--- @param Regex string
--- @param Replacement string
--- @return string Result
function Osi.NRD_RegexReplace(String, Regex, Replacement) end
	
--- @param String string
--- @return integer Result
function Osi.NRD_StringToInt(String) end
	
--- @param String string
--- @return number Result
function Osi.NRD_StringToReal(String) end
	
--- @param String string
--- @return string Result
function Osi.NRD_GuidString(String) end
	
--- @param Real number
--- @return string Result
function Osi.NRD_RealToString(Real) end
	
--- @param Integer integer
--- @return string Result
function Osi.NRD_IntegerToString(Integer) end
	
--- @param Min number
--- @param Max number
--- @return number Result
function Osi.NRD_RandomReal(Min, Max) end
	
--- @param In integer
--- @return integer Out
function Osi.NRD_Factorial(In) end
	
--- @param Base number
--- @param Exp integer
--- @return number Out
function Osi.NRD_Pow(Base, Exp) end
	
--- @param In number
--- @return number Out
function Osi.NRD_Sin(In) end
	
--- @param In number
--- @return number Out
function Osi.NRD_Cos(In) end
	
--- @param In number
--- @return number Out
function Osi.NRD_Tan(In) end
	
--- @param In number
--- @return number Out
function Osi.NRD_Round(In) end
	
--- @param In number
--- @return number Out
function Osi.NRD_Ceil(In) end
	
--- @param In number
--- @return number Out
function Osi.NRD_Floor(In) end
	
--- @param In number
--- @return number Out
function Osi.NRD_Abs(In) end
	
--- @param In number
--- @return number Out
function Osi.NRD_Sqrt(In) end
	
--- @param In number
--- @return number Out
function Osi.NRD_Exp(In) end
	
--- @param In number
--- @return number Out
function Osi.NRD_Log(In) end
	
--- @param Numerator integer
--- @param Denominator integer
--- @return integer Divisible
function Osi.NRD_IsDivisible(Numerator, Denominator) end
	
--- @param StatsId string
function Osi.NRD_StatExists(StatsId) end
	
--- @param StatsId string
--- @param Attribute string
function Osi.NRD_StatAttributeExists(StatsId, Attribute) end
	
--- @param StatsId string
--- @param Attribute string
--- @return integer Value
function Osi.NRD_StatGetInt(StatsId, Attribute) end
	
--- @param StatsId string
--- @param Attribute string
--- @return string Value
function Osi.NRD_StatGetString(StatsId, Attribute) end
	
--- @param StatsId string
--- @return string Type
function Osi.NRD_StatGetType(StatsId) end
	
--- @param Key string
--- @return number Value
function Osi.NRD_StatGetExtraData(Key) end
	
--- @param Object string
--- @param StatusType string
--- @return integer HasStatus
function Osi.NRD_ObjectHasStatusType(Object, StatusType) end
	
--- @param Object string
--- @param StatusId string
--- @return integer StatusHandle
function Osi.NRD_StatusGetHandle(Object, StatusId) end
	
--- @param Object string
--- @param StatusHandle integer
--- @param Attribute string
--- @return integer Value
function Osi.NRD_StatusGetInt(Object, StatusHandle, Attribute) end
	
--- @param Object string
--- @param StatusHandle integer
--- @param Attribute string
--- @return number Value
function Osi.NRD_StatusGetReal(Object, StatusHandle, Attribute) end
	
--- @param Object string
--- @param StatusHandle integer
--- @param Attribute string
--- @return string Value
function Osi.NRD_StatusGetString(Object, StatusHandle, Attribute) end
	
--- @param Object string
--- @param StatusHandle integer
--- @param Attribute string
--- @return string Value
function Osi.NRD_StatusGetGuidString(Object, StatusHandle, Attribute) end
	
--- @param Character string
--- @param StatusId string
--- @param LifeTime number
--- @return integer StatusHandle
function Osi.NRD_ApplyActiveDefense(Character, StatusId, LifeTime) end
	
--- @param Character string
--- @param StatusId string
--- @param SourceCharacter string
--- @param LifeTime number
--- @param DistancePerDamage number
--- @return integer StatusHandle
function Osi.NRD_ApplyDamageOnMove(Character, StatusId, SourceCharacter, LifeTime, DistancePerDamage) end
	
--- @param Character string
--- @return string Action
function Osi.NRD_CharacterGetCurrentAction(Character) end
	
--- @param Character string
--- @param Property string
--- @return string Value
function Osi.NRD_ActionStateGetString(Character, Property) end
	
--- @param OwnerCharacter string
--- @param SkillId string
--- @param X number
--- @param Y number
--- @param Z number
--- @return integer GameActionHandle
function Osi.NRD_CreateRain(OwnerCharacter, SkillId, X, Y, Z) end
	
--- @param OwnerCharacter string
--- @param SkillId string
--- @param X number
--- @param Y number
--- @param Z number
--- @return integer GameActionHandle
function Osi.NRD_CreateStorm(OwnerCharacter, SkillId, X, Y, Z) end
	
--- @param OwnerCharacter string
--- @param SkillId string
--- @param SourceX number
--- @param SourceY number
--- @param SourceZ number
--- @param TargetX number
--- @param TargetY number
--- @param TargetZ number
--- @return integer GameActionHandle
function Osi.NRD_CreateWall(OwnerCharacter, SkillId, SourceX, SourceY, SourceZ, TargetX, TargetY, TargetZ) end
	
--- @param OwnerCharacter string
--- @param SkillId string
--- @param PositionX number
--- @param PositionY number
--- @param PositionZ number
--- @param TargetX number
--- @param TargetY number
--- @param TargetZ number
--- @return integer GameActionHandle
function Osi.NRD_CreateTornado(OwnerCharacter, SkillId, PositionX, PositionY, PositionZ, TargetX, TargetY, TargetZ) end
	
--- @param OwnerCharacter string
--- @param SkillId string
--- @param X number
--- @param Y number
--- @param Z number
--- @return integer GameActionHandle
function Osi.NRD_CreateDome(OwnerCharacter, SkillId, X, Y, Z) end
	
--- @param TargetObject string
--- @param X number
--- @param Y number
--- @param Z number
--- @param BeamEffectName string
--- @param CasterCharacter string
--- @return integer GameActionHandle
function Osi.NRD_CreateGameObjectMove(TargetObject, X, Y, Z, BeamEffectName, CasterCharacter) end
	
--- @param GameActionHandle integer
--- @return number LifeTime
function Osi.NRD_GameActionGetLifeTime(GameActionHandle) end
	
--- @param OwnerCharacter string
--- @param Template string
--- @param X number
--- @param Y number
--- @param Z number
--- @param Lifetime number
--- @param Level integer
--- @param IsTotem integer
--- @param MapToAiGrid integer
--- @return string Summon
function Osi.NRD_Summon(OwnerCharacter, Template, X, Y, Z, Lifetime, Level, IsTotem, MapToAiGrid) end
	
--- @param Target string
--- @param Source string
--- @return integer HitHandle
function Osi.NRD_HitPrepare(Target, Source) end
	
--- @param HitHandle integer
--- @return integer StatusHandle
function Osi.NRD_HitQryExecute(HitHandle) end
	
--- @param HitHandle integer
--- @param Property string
--- @return integer Value
function Osi.NRD_HitGetInt(HitHandle, Property) end
	
--- @param HitHandle integer
--- @param Property string
--- @return string Value
function Osi.NRD_HitGetString(HitHandle, Property) end
	
--- @param HitHandle integer
--- @param DamageType string
--- @return integer Amount
function Osi.NRD_HitGetDamage(HitHandle, DamageType) end
	
--- @param Object string
--- @param StatusHandle integer
--- @param DamageType string
--- @return integer Amount
function Osi.NRD_HitStatusGetDamage(Object, StatusHandle, DamageType) end
	
--- @param Character string
--- @param SkillId string
--- @return number Cooldown
function Osi.NRD_SkillGetCooldown(Character, SkillId) end
	
--- @param Character string
--- @param SkillId string
--- @param Property string
--- @return integer Value
function Osi.NRD_SkillGetInt(Character, SkillId, Property) end
	
--- @param Character string
--- @param Slot integer
--- @return string Item
function Osi.NRD_SkillBarGetItem(Character, Slot) end
	
--- @param Character string
--- @param Slot integer
--- @return string Skill
function Osi.NRD_SkillBarGetSkill(Character, Slot) end
	
--- @param Character string
--- @param Skill string
--- @return integer Slot
function Osi.NRD_SkillBarFindSkill(Character, Skill) end
	
--- @param Character string
--- @param Item string
--- @return integer Slot
function Osi.NRD_SkillBarFindItem(Character, Item) end
	
--- @param Player string
--- @param Property string
--- @return integer Value
function Osi.NRD_PlayerGetCustomDataInt(Player, Property) end
	
--- @param Player string
--- @param Property string
--- @return string Value
function Osi.NRD_PlayerGetCustomDataString(Player, Property) end
	
--- @param Item string
--- @return string StatsId
function Osi.NRD_ItemGetStatsId(Item) end
	
--- @param Item string
--- @return string Base
--- @return string ItemType
--- @return integer Level
function Osi.NRD_ItemGetGenerationParams(Item) end
	
--- @param Item string
--- @return string Base
--- @return string ItemType
--- @return integer Level
--- @return integer Random
function Osi.NRD_ItemGetGenerationParams(Item) end
	
--- @param Item string
--- @return string DeltaMod
--- @return integer Count
function Osi.NRD_ItemHasDeltaModifier(Item) end
	
--- @param Item string
--- @return string Parent
function Osi.NRD_ItemGetParent(Item) end
	
--- @param Item string
--- @param Property string
--- @return integer Value
function Osi.NRD_ItemGetInt(Item, Property) end
	
--- @param Item string
--- @param Property string
--- @return string Value
function Osi.NRD_ItemGetString(Item, Property) end
	
--- @param Item string
--- @param Property string
--- @return string Value
function Osi.NRD_ItemGetGuidString(Item, Property) end
	
--- @param Item string
--- @param Stat string
--- @return integer Value
function Osi.NRD_ItemGetPermanentBoostInt(Item, Stat) end
	
--- @param Item string
--- @param Stat string
--- @return number Value
function Osi.NRD_ItemGetPermanentBoostReal(Item, Stat) end
	
--- @param Item string
--- @param Stat string
--- @return string Value
function Osi.NRD_ItemGetPermanentBoostString(Item, Stat) end
	
--- @param Item string
--- @param Ability string
--- @return integer Points
function Osi.NRD_ItemGetPermanentBoostAbility(Item, Ability) end
	
--- @param Item string
--- @param Talent string
--- @return integer HasTalent
function Osi.NRD_ItemGetPermanentBoostTalent(Item, Talent) end
	
--- @return string NewItem
function Osi.NRD_ItemClone() end
	
--- @param Character string
--- @param Stat string
--- @param ExcludeBoosts integer
--- @return integer Value
function Osi.NRD_CharacterGetComputedStat(Character, Stat, ExcludeBoosts) end
	
--- @param Attacker string
--- @param Target string
--- @return integer HitChance
function Osi.NRD_CharacterGetHitChance(Attacker, Target) end
	
--- @param Character string
--- @param Stat string
--- @return integer Value
function Osi.NRD_CharacterGetStatInt(Character, Stat) end
	
--- @param Character string
--- @param Stat string
--- @return string Value
function Osi.NRD_CharacterGetStatString(Character, Stat) end
	
--- @param Character string
--- @param Stat string
--- @return integer Value
function Osi.NRD_CharacterGetPermanentBoostInt(Character, Stat) end
	
--- @param Character string
--- @param Talent string
--- @return integer IsDisabled
function Osi.NRD_CharacterIsTalentDisabled(Character, Talent) end
	
--- @param Character string
--- @param Attribute string
--- @return integer Value
function Osi.NRD_CharacterGetInt(Character, Attribute) end
	
--- @param Character string
--- @param Attribute string
--- @return number Value
function Osi.NRD_CharacterGetReal(Character, Attribute) end
	
--- @param Character string
--- @param Attribute string
--- @return string Value
function Osi.NRD_CharacterGetString(Character, Attribute) end
	
--- @param Object string
--- @param Flag integer
--- @return integer Value
function Osi.NRD_ObjectGetInternalFlag(Object, Flag) end
	
--- @param Character string
--- @param StatId string
--- @return integer Value
function Osi.NRD_CharacterGetCustomStat(Character, StatId) end
	
--- @param Name string
--- @param Description string
--- @return string StatId
function Osi.NRD_CreateCustomStat(Name, Description) end
	
--- @param Name string
--- @return string StatId
function Osi.NRD_GetCustomStat(Name) end
	
--- @param Func string
function Osi.NRD_LuaQuery0(Func) end
	
--- @param Func string
--- @return string Out1
function Osi.NRD_LuaQuery0(Func) end
	
--- @param Func string
--- @return string Out1
--- @return string Out2
function Osi.NRD_LuaQuery0(Func) end
	
--- @param Func string
--- @return string Out1
--- @return string Out2
--- @return string Out3
function Osi.NRD_LuaQuery0(Func) end
	
--- @param Func string
--- @return string Out1
--- @return string Out2
--- @return string Out3
--- @return string Out4
function Osi.NRD_LuaQuery0(Func) end
	
--- @param Func string
--- @return string Out1
--- @return string Out2
--- @return string Out3
--- @return string Out4
--- @return string Out5
function Osi.NRD_LuaQuery0(Func) end
	
--- @param Func string
--- @param Arg1 string
function Osi.NRD_LuaQuery1(Func, Arg1) end
	
--- @param Func string
--- @param Arg1 string
--- @return string Out1
function Osi.NRD_LuaQuery1(Func, Arg1) end
	
--- @param Func string
--- @param Arg1 string
--- @return string Out1
--- @return string Out2
function Osi.NRD_LuaQuery1(Func, Arg1) end
	
--- @param Func string
--- @param Arg1 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
function Osi.NRD_LuaQuery1(Func, Arg1) end
	
--- @param Func string
--- @param Arg1 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
--- @return string Out4
function Osi.NRD_LuaQuery1(Func, Arg1) end
	
--- @param Func string
--- @param Arg1 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
--- @return string Out4
--- @return string Out5
function Osi.NRD_LuaQuery1(Func, Arg1) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
function Osi.NRD_LuaQuery2(Func, Arg1, Arg2) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @return string Out1
function Osi.NRD_LuaQuery2(Func, Arg1, Arg2) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @return string Out1
--- @return string Out2
function Osi.NRD_LuaQuery2(Func, Arg1, Arg2) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
function Osi.NRD_LuaQuery2(Func, Arg1, Arg2) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
--- @return string Out4
function Osi.NRD_LuaQuery2(Func, Arg1, Arg2) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
--- @return string Out4
--- @return string Out5
function Osi.NRD_LuaQuery2(Func, Arg1, Arg2) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
function Osi.NRD_LuaQuery3(Func, Arg1, Arg2, Arg3) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @return string Out1
function Osi.NRD_LuaQuery3(Func, Arg1, Arg2, Arg3) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @return string Out1
--- @return string Out2
function Osi.NRD_LuaQuery3(Func, Arg1, Arg2, Arg3) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
function Osi.NRD_LuaQuery3(Func, Arg1, Arg2, Arg3) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
--- @return string Out4
function Osi.NRD_LuaQuery3(Func, Arg1, Arg2, Arg3) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
--- @return string Out4
--- @return string Out5
function Osi.NRD_LuaQuery3(Func, Arg1, Arg2, Arg3) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
function Osi.NRD_LuaQuery4(Func, Arg1, Arg2, Arg3, Arg4) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @return string Out1
function Osi.NRD_LuaQuery4(Func, Arg1, Arg2, Arg3, Arg4) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @return string Out1
--- @return string Out2
function Osi.NRD_LuaQuery4(Func, Arg1, Arg2, Arg3, Arg4) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
function Osi.NRD_LuaQuery4(Func, Arg1, Arg2, Arg3, Arg4) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
--- @return string Out4
function Osi.NRD_LuaQuery4(Func, Arg1, Arg2, Arg3, Arg4) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
--- @return string Out4
--- @return string Out5
function Osi.NRD_LuaQuery4(Func, Arg1, Arg2, Arg3, Arg4) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
function Osi.NRD_LuaQuery5(Func, Arg1, Arg2, Arg3, Arg4, Arg5) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @return string Out1
function Osi.NRD_LuaQuery5(Func, Arg1, Arg2, Arg3, Arg4, Arg5) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @return string Out1
--- @return string Out2
function Osi.NRD_LuaQuery5(Func, Arg1, Arg2, Arg3, Arg4, Arg5) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
function Osi.NRD_LuaQuery5(Func, Arg1, Arg2, Arg3, Arg4, Arg5) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
--- @return string Out4
function Osi.NRD_LuaQuery5(Func, Arg1, Arg2, Arg3, Arg4, Arg5) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
--- @return string Out4
--- @return string Out5
function Osi.NRD_LuaQuery5(Func, Arg1, Arg2, Arg3, Arg4, Arg5) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
function Osi.NRD_LuaQuery6(Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @return string Out1
function Osi.NRD_LuaQuery6(Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @return string Out1
--- @return string Out2
function Osi.NRD_LuaQuery6(Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
function Osi.NRD_LuaQuery6(Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
--- @return string Out4
function Osi.NRD_LuaQuery6(Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
--- @return string Out4
--- @return string Out5
function Osi.NRD_LuaQuery6(Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
function Osi.NRD_LuaQuery7(Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @return string Out1
function Osi.NRD_LuaQuery7(Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @return string Out1
--- @return string Out2
function Osi.NRD_LuaQuery7(Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
function Osi.NRD_LuaQuery7(Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
--- @return string Out4
function Osi.NRD_LuaQuery7(Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
--- @return string Out4
--- @return string Out5
function Osi.NRD_LuaQuery7(Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
function Osi.NRD_LuaQuery8(Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
--- @return string Out1
function Osi.NRD_LuaQuery8(Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
--- @return string Out1
--- @return string Out2
function Osi.NRD_LuaQuery8(Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
function Osi.NRD_LuaQuery8(Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
--- @return string Out4
function Osi.NRD_LuaQuery8(Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
--- @return string Out4
--- @return string Out5
function Osi.NRD_LuaQuery8(Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
--- @param Arg9 string
function Osi.NRD_LuaQuery9(Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
--- @param Arg9 string
--- @return string Out1
function Osi.NRD_LuaQuery9(Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
--- @param Arg9 string
--- @return string Out1
--- @return string Out2
function Osi.NRD_LuaQuery9(Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
--- @param Arg9 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
function Osi.NRD_LuaQuery9(Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
--- @param Arg9 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
--- @return string Out4
function Osi.NRD_LuaQuery9(Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
--- @param Arg9 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
--- @return string Out4
--- @return string Out5
function Osi.NRD_LuaQuery9(Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
--- @param Arg9 string
--- @param Arg10 string
function Osi.NRD_LuaQuery10(Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
--- @param Arg9 string
--- @param Arg10 string
--- @return string Out1
function Osi.NRD_LuaQuery10(Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
--- @param Arg9 string
--- @param Arg10 string
--- @return string Out1
--- @return string Out2
function Osi.NRD_LuaQuery10(Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
--- @param Arg9 string
--- @param Arg10 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
function Osi.NRD_LuaQuery10(Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
--- @param Arg9 string
--- @param Arg10 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
--- @return string Out4
function Osi.NRD_LuaQuery10(Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10) end
	
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
--- @param Arg9 string
--- @param Arg10 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
--- @return string Out4
--- @return string Out5
function Osi.NRD_LuaQuery10(Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10) end
	
--- @param Mod string
--- @param Func string
function Osi.NRD_ModQuery0(Mod, Func) end
	
--- @param Mod string
--- @param Func string
--- @return string Out1
function Osi.NRD_ModQuery0(Mod, Func) end
	
--- @param Mod string
--- @param Func string
--- @return string Out1
--- @return string Out2
function Osi.NRD_ModQuery0(Mod, Func) end
	
--- @param Mod string
--- @param Func string
--- @return string Out1
--- @return string Out2
--- @return string Out3
function Osi.NRD_ModQuery0(Mod, Func) end
	
--- @param Mod string
--- @param Func string
--- @return string Out1
--- @return string Out2
--- @return string Out3
--- @return string Out4
function Osi.NRD_ModQuery0(Mod, Func) end
	
--- @param Mod string
--- @param Func string
--- @return string Out1
--- @return string Out2
--- @return string Out3
--- @return string Out4
--- @return string Out5
function Osi.NRD_ModQuery0(Mod, Func) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
function Osi.NRD_ModQuery1(Mod, Func, Arg1) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @return string Out1
function Osi.NRD_ModQuery1(Mod, Func, Arg1) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @return string Out1
--- @return string Out2
function Osi.NRD_ModQuery1(Mod, Func, Arg1) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
function Osi.NRD_ModQuery1(Mod, Func, Arg1) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
--- @return string Out4
function Osi.NRD_ModQuery1(Mod, Func, Arg1) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
--- @return string Out4
--- @return string Out5
function Osi.NRD_ModQuery1(Mod, Func, Arg1) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
function Osi.NRD_ModQuery2(Mod, Func, Arg1, Arg2) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @return string Out1
function Osi.NRD_ModQuery2(Mod, Func, Arg1, Arg2) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @return string Out1
--- @return string Out2
function Osi.NRD_ModQuery2(Mod, Func, Arg1, Arg2) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
function Osi.NRD_ModQuery2(Mod, Func, Arg1, Arg2) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
--- @return string Out4
function Osi.NRD_ModQuery2(Mod, Func, Arg1, Arg2) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
--- @return string Out4
--- @return string Out5
function Osi.NRD_ModQuery2(Mod, Func, Arg1, Arg2) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
function Osi.NRD_ModQuery3(Mod, Func, Arg1, Arg2, Arg3) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @return string Out1
function Osi.NRD_ModQuery3(Mod, Func, Arg1, Arg2, Arg3) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @return string Out1
--- @return string Out2
function Osi.NRD_ModQuery3(Mod, Func, Arg1, Arg2, Arg3) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
function Osi.NRD_ModQuery3(Mod, Func, Arg1, Arg2, Arg3) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
--- @return string Out4
function Osi.NRD_ModQuery3(Mod, Func, Arg1, Arg2, Arg3) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
--- @return string Out4
--- @return string Out5
function Osi.NRD_ModQuery3(Mod, Func, Arg1, Arg2, Arg3) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
function Osi.NRD_ModQuery4(Mod, Func, Arg1, Arg2, Arg3, Arg4) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @return string Out1
function Osi.NRD_ModQuery4(Mod, Func, Arg1, Arg2, Arg3, Arg4) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @return string Out1
--- @return string Out2
function Osi.NRD_ModQuery4(Mod, Func, Arg1, Arg2, Arg3, Arg4) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
function Osi.NRD_ModQuery4(Mod, Func, Arg1, Arg2, Arg3, Arg4) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
--- @return string Out4
function Osi.NRD_ModQuery4(Mod, Func, Arg1, Arg2, Arg3, Arg4) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
--- @return string Out4
--- @return string Out5
function Osi.NRD_ModQuery4(Mod, Func, Arg1, Arg2, Arg3, Arg4) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
function Osi.NRD_ModQuery5(Mod, Func, Arg1, Arg2, Arg3, Arg4, Arg5) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @return string Out1
function Osi.NRD_ModQuery5(Mod, Func, Arg1, Arg2, Arg3, Arg4, Arg5) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @return string Out1
--- @return string Out2
function Osi.NRD_ModQuery5(Mod, Func, Arg1, Arg2, Arg3, Arg4, Arg5) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
function Osi.NRD_ModQuery5(Mod, Func, Arg1, Arg2, Arg3, Arg4, Arg5) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
--- @return string Out4
function Osi.NRD_ModQuery5(Mod, Func, Arg1, Arg2, Arg3, Arg4, Arg5) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
--- @return string Out4
--- @return string Out5
function Osi.NRD_ModQuery5(Mod, Func, Arg1, Arg2, Arg3, Arg4, Arg5) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
function Osi.NRD_ModQuery6(Mod, Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @return string Out1
function Osi.NRD_ModQuery6(Mod, Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @return string Out1
--- @return string Out2
function Osi.NRD_ModQuery6(Mod, Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
function Osi.NRD_ModQuery6(Mod, Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
--- @return string Out4
function Osi.NRD_ModQuery6(Mod, Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
--- @return string Out4
--- @return string Out5
function Osi.NRD_ModQuery6(Mod, Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
function Osi.NRD_ModQuery7(Mod, Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @return string Out1
function Osi.NRD_ModQuery7(Mod, Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @return string Out1
--- @return string Out2
function Osi.NRD_ModQuery7(Mod, Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
function Osi.NRD_ModQuery7(Mod, Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
--- @return string Out4
function Osi.NRD_ModQuery7(Mod, Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
--- @return string Out4
--- @return string Out5
function Osi.NRD_ModQuery7(Mod, Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
function Osi.NRD_ModQuery8(Mod, Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
--- @return string Out1
function Osi.NRD_ModQuery8(Mod, Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
--- @return string Out1
--- @return string Out2
function Osi.NRD_ModQuery8(Mod, Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
function Osi.NRD_ModQuery8(Mod, Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
--- @return string Out4
function Osi.NRD_ModQuery8(Mod, Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
--- @return string Out4
--- @return string Out5
function Osi.NRD_ModQuery8(Mod, Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
--- @param Arg9 string
function Osi.NRD_ModQuery9(Mod, Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
--- @param Arg9 string
--- @return string Out1
function Osi.NRD_ModQuery9(Mod, Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
--- @param Arg9 string
--- @return string Out1
--- @return string Out2
function Osi.NRD_ModQuery9(Mod, Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
--- @param Arg9 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
function Osi.NRD_ModQuery9(Mod, Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
--- @param Arg9 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
--- @return string Out4
function Osi.NRD_ModQuery9(Mod, Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
--- @param Arg9 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
--- @return string Out4
--- @return string Out5
function Osi.NRD_ModQuery9(Mod, Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
--- @param Arg9 string
--- @param Arg10 string
function Osi.NRD_ModQuery10(Mod, Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
--- @param Arg9 string
--- @param Arg10 string
--- @return string Out1
function Osi.NRD_ModQuery10(Mod, Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
--- @param Arg9 string
--- @param Arg10 string
--- @return string Out1
--- @return string Out2
function Osi.NRD_ModQuery10(Mod, Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
--- @param Arg9 string
--- @param Arg10 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
function Osi.NRD_ModQuery10(Mod, Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
--- @param Arg9 string
--- @param Arg10 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
--- @return string Out4
function Osi.NRD_ModQuery10(Mod, Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10) end
	
--- @param Mod string
--- @param Func string
--- @param Arg1 string
--- @param Arg2 string
--- @param Arg3 string
--- @param Arg4 string
--- @param Arg5 string
--- @param Arg6 string
--- @param Arg7 string
--- @param Arg8 string
--- @param Arg9 string
--- @param Arg10 string
--- @return string Out1
--- @return string Out2
--- @return string Out3
--- @return string Out4
--- @return string Out5
function Osi.NRD_ModQuery10(Mod, Func, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10) end
	
--- @param ModGUID string
--- @return integer IsLoaded
function Osi.NRD_IsModLoaded(ModGUID) end
	
--- @return integer Version
function Osi.NRD_GetVersion() end
	
--- @param Path string
--- @return string Contents
function Osi.NRD_LoadFile(Path) end

--#endregion]]
return text