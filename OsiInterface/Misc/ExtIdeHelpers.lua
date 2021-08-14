-- Special global value that contains the current mod UUID during load
ModuleUUID = "UUID"

Osi = {}

--- From v52 onwards object handles are a type of userdata (lightuserdata) 
--- instead of integers.
--- @alias ObjectHandle userdata
--- @alias StatusHandle userdata

--- @class DamageItem
--- @field DamageType string
--- @field Amount integer
local DamageItem = {}

--- @class DamageList
local DamageList = {}

--- Returns the amount of damage with the specified type
--- @param damageType string DamageType enumeration
--- @return integer
function DamageList:GetByType(damageType) end
    
--- Add damage
--- @param damageType string DamageType enumeration
--- @param amount integer
function DamageList:Add(damageType, amount) end
    
--- Clear damage list.
--- If damageType is specified it only removes damage items with the specified type.
--- @param damageType string|nil DamageType enumeration
function DamageList:Clear(damageType) end
    
--- Multiply every damage item with the specified value
--- @param multiplier number
function DamageList:Multiply(multiplier) end
    
--- Merge another DamageList into this list
--- @param list DamageList List to merge
function DamageList:Merge(list) end
    
--- Converts all damages to the specified type
--- @param damageType string DamageType enumeration
function DamageList:ConvertDamageType(damageType) end
    
--- Aggregate all damage items with the same type
function DamageList:AggregateSameTypeDamages() end
    
--- Returns all damage items as a table
--- @return DamageItem[]
function DamageList:ToTable() end

--- @class InputEvent
--- @field EventId integer
--- @field InputDeviceId integer
--- @field InputPlayerIndex integer
--- @field Press boolean
--- @field Release boolean
--- @field ValueChange boolean
--- @field Hold boolean
--- @field Repeat boolean
--- @field AcceleratedRepeat boolean


--- @class AiGrid
--- @field OffsetX number X coordinate the grid starts at
--- @field OffsetY number Y coordinate the grid starts at
--- @field OffsetZ number Z coordinate the grid starts at
--- @field GridScale number Size of each cell in the grid
--- @field Width number Total width of the grid
--- @field Height number Total height of the grid
local AiGrid = {}

--- Scans the vicinity of the specified points for surfaces that match the specified flags.
--- Returns true if a surface was found, false otherwise.
--- @param x number X coordinate of point to search
--- @param z number Z coordinate of point to search
--- @param radius number Radius to search
--- @param flags string[] AI flags to look for
--- @param bias number Height bias
--- @return boolean
function AiGrid:SearchForCell(x, z, radius, flags, bias) end

--- Returns the contents (game objects, surfaces, etc.) of the specified cell
--- @param x number X coordinate of point to search
--- @param z number Z coordinate of point to search
--- @return table
function AiGrid:GetCellInfo(x, z) end

--- Returns the AI flags (navigation info, surface flags) of the specified cell
--- @param x number X coordinate of point to search
--- @param z number Z coordinate of point to search
--- @return integer
function AiGrid:GetAiFlags(x, z) end

--- Updates the AI flags (navigation info) of the specified cell
--- @param x number X coordinate of point to search
--- @param z number Z coordinate of point to search
--- @param flags integer AI flags
function AiGrid:SetAiFlags(x, z, flags) end

--- Returns the height of the specified cell
--- @param x number X coordinate of point to search
--- @param z number Z coordinate of point to search
--- @return number
function AiGrid:GetHeight(x, z) end

--- Updates the height of the specified cell
--- @param x number X coordinate of point to search
--- @param z number Z coordinate of point to search
--- @param height number Cell height
function AiGrid:SetHeight(x, z, height) end


--- @class LevelDesc
--- @field LevelName string
--- @field UniqueKey string
--- @field CustomDisplayLevelName string
--- @field Type integer


--- @class CombatComponentTemplate
--- @field Alignment string
--- @field CanFight boolean
--- @field CanJoinCombat boolean
--- @field CombatGroupID string
--- @field IsBoss boolean
--- @field IsInspector boolean
--- @field StartCombatRange number


--- @class EoCGameObjectTemplate
--- @field Id string
--- @field Name string
--- @field TemplateName string
--- @field IsGlobal boolean
--- @field IsDeleted boolean
--- @field LevelName string
--- @field ModFolder string
--- @field GroupID string
--- @field VisualTemplate string
--- @field PhysicsTemplate string
--- @field CastShadow boolean
--- @field ReceiveDecal boolean
--- @field AllowReceiveDecalWhenAnimated boolean
--- @field IsReflecting boolean
--- @field IsShadowProxy boolean
--- @field RenderChannel number
--- @field CameraOffset number[]
--- @field HasParentModRelation boolean
--- @field HasGameplayValue boolean
--- @field DevComment string
--- @field AIBoundsRadius number
--- @field AIBoundsHeight number
--- @field DisplayName string
--- @field Opacity number
--- @field Fadeable boolean
--- @field FadeIn boolean
--- @field SeeThrough boolean
--- @field FadeGroup string
--- @field GameMasterSpawnSection integer
--- @field GameMasterSpawnSubSection string


--- @class CharacterTemplate : EoCGameObjectTemplate
--- @field CombatTemplate CombatComponentTemplate
--- @field Icon string
--- @field Stats string
--- @field SkillSet string
--- @field Equipment string
--- @field LightID string
--- @field HitFX string
--- @field DefaultDialog string
--- @field SpeakerGroup string
--- @field GeneratePortrait string
--- @field WalkSpeed number
--- @field RunSpeed number
--- @field ClimbAttachSpeed number
--- @field ClimbLoopSpeed number
--- @field ClimbDetachSpeed number
--- @field CanShootThrough boolean
--- @field WalkThrough boolean
--- @field CanClimbLadders boolean
--- @field IsPlayer boolean
--- @field Floating boolean
--- @field SpotSneakers boolean
--- @field CanOpenDoors boolean
--- @field AvoidTraps boolean
--- @field InfluenceTreasureLevel boolean
--- @field HardcoreOnly boolean
--- @field NotHardcore boolean
--- @field JumpUpLadders boolean
--- @field NoRotate boolean
--- @field IsHuge boolean
--- @field EquipmentClass number
--- @field ExplodedResourceID string
--- @field ExplosionFX string
--- @field VisualSetResourceID string
--- @field VisualSetIndices number
--- @field TrophyID string
--- @field SoundInitEvent string
--- @field SoundAttachBone string
--- @field SoundAttenuation number
--- @field CoverAmount number
--- @field LevelOverride number
--- @field ForceUnsheathSkills boolean
--- @field CanBeTeleported boolean
--- @field ActivationGroupId string
--- @field SoftBodyCollisionTemplate string
--- @field RagdollTemplate string
--- @field DefaultState number
--- @field GhostTemplate string
--- @field IsLootable boolean
--- @field IsEquipmentLootable boolean
--- @field InventoryType number
--- @field IsArenaChampion boolean
--- @field FootstepWeight string
local CharacterTemplate = {}
    
--- Returns the list of visuals that can be selected for the specified slot
--- @param slot string Slot name (HairHelmet, Head, Torso, Arms, Trousers, Boots, Beard, Visual8, Visual9)
--- @return string[]
function CharacterTemplate:GetVisualChoices(slot) end
    
--- Returns the list of colors that can be selected for the specified slot
--- @param slot string Slot name (Skin, Hair, Cloth)
--- @return number[]
function CharacterTemplate:GetColorChoices(slot) end


--- @class ItemTemplate : EoCGameObjectTemplate
--- @field CombatTemplate CombatComponentTemplate
--- @field Icon string
--- @field CanBePickedUp boolean
--- @field CanBeMoved boolean
--- @field CoverAmount number
--- @field CanShootThrough boolean
--- @field CanClickThrough boolean
--- @field Destroyed boolean
--- @field WalkThrough boolean
--- @field WalkOn boolean
--- @field Wadable boolean
--- @field IsInteractionDisabled boolean
--- @field IsPinnedContainer boolean
--- @field StoryItem boolean
--- @field FreezeGravity boolean
--- @field IsKey boolean
--- @field IsTrap boolean
--- @field IsSurfaceBlocker boolean
--- @field IsSurfaceCloudBlocker boolean
--- @field TreasureOnDestroy boolean
--- @field IsHuge boolean
--- @field HardcoreOnly boolean
--- @field NotHardcore boolean
--- @field UsePartyLevelForTreasureLevel boolean
--- @field Unimportant boolean
--- @field Hostile boolean
--- @field UseOnDistance boolean
--- @field UseRemotely boolean
--- @field IsBlocker boolean
--- @field IsPointerBlocker boolean
--- @field ItemDisplayName boolean
--- @field Tooltip number
--- @field Stats string
--- @field OnUseDescription string
--- @field DefaultState string
--- @field Owner string
--- @field Key string
--- @field HitFX string
--- @field LockLevel number 
--- @field Amount number
--- @field MaxStackAmount number
--- @field TreasureLevel number
--- @field DropSound string
--- @field PickupSound string
--- @field UseSound string
--- @field EquipSound string
--- @field UnequipSound string
--- @field InventoryMoveSound string
--- @field LoopSound string
--- @field SoundInitEvent string
--- @field SoundAttachBone string
--- @field SoundAttenuation number
--- @field Description string
--- @field ItemDescription string
--- @field Speaker string
--- @field AltSpeaker string
--- @field SpeakerGroup string
--- @field ActivationGroupId string
--- @field Race number
--- @field IsWall boolean
--- @field LevelOverride number
--- @field Floating boolean
--- @field IsSourceContainer boolean
--- @field MeshProxy string
--- @field IsPublicDomain boolean
--- @field AllowSummonTeleport boolean


--- @class ProjectileTemplate : EoCGameObjectTemplate
--- @field LifeTime number
--- @field Speed number
--- @field Acceleration number
--- @field CastBone string
--- @field ImpactFX string
--- @field TrailFX string
--- @field DestroyTrailFXOnImpact boolean
--- @field BeamFX string
--- @field PreviewPathMaterial string
--- @field PreviewPathImpactFX string
--- @field PreviewPathRadius number
--- @field ImpactFXSize number
--- @field RotateImpact boolean
--- @field IgnoreRoof boolean
--- @field DetachBeam boolean
--- @field NeedsArrowImpactSFX boolean
--- @field ProjectilePath boolean
--- @field PathShift string
--- @field PathRadius number
--- @field PathMinArcDist number
--- @field PathMaxArcDist number
--- @field PathRepeat number


--- @class TriggerTemplate : EoCGameObjectTemplate
--- @field TriggerType string
--- @field PhysicsType string
--- @field TriggerGizmoOverride string


--- @class SurfaceTemplateStatus
--- @field StatusId string
--- @field Chance number
--- @field Duration number
--- @field RemoveStatus boolean
--- @field OnlyWhileMoving boolean
--- @field ApplyToCharacters boolean
--- @field ApplyToItems boolean
--- @field KeepAlive boolean
--- @field VanishOnReapply boolean
--- @field ForceStatus boolean


--- @class SurfaceTemplate : GameObjectTemplate
--- @field SurfaceTypeId integer
--- @field SurfaceType string
--- @field DisplayName string
--- @field Description string
--- @field DecalMaterial string
--- @field CanEnterCombat boolean
--- @field AlwaysUseDefaultLifeTime boolean
--- @field DefaultLifeTime number
--- @field SurfaceGrowTimer number
--- @field FadeInSpeed number
--- @field FadeOutSpeed number
--- @field Seed integer
--- @field Statuses SurfaceTemplateStatus[]
--- @field DamageWeapon string
--- @field Summon string
--- @field DamageCharacters boolean
--- @field DamageItems boolean
--- @field DamageTorches boolean
--- @field RemoveDestroyedItems boolean
--- @field CanSeeThrough boolean
--- @field CanShootThrough boolean

--TODO
--- @class EclStatus
--- @field StatusId string

--- @class EsvStatus
--- @field StatusType string Engine status type (HIT, DAMAGE, CONSUME, ...)
--- @field StatusId string
--- @field CanEnterChance integer
--- @field StartTimer number
--- @field LifeTime number
--- @field CurrentLifeTime number
--- @field TurnTimer number
--- @field Strength number
--- @field StatsMultiplier number
--- @field DamageSourceType string See CauseType enumeration
--- @field StatusHandle StatusHandle
--- @field TargetHandle ObjectHandle Handle of game object this status was applied to
--- @field StatusSourceHandle ObjectHandle Handle of game object that caused this status
---
--- StatusFlags0
--- @field KeepAlive boolean
--- @field IsOnSourceSurface boolean
--- @field IsFromItem boolean
--- @field Channeled boolean
--- @field IsLifeTimeSet boolean
--- @field InitiateCombat boolean
--- @field Influence boolean
--- StatusFlags1
--- @field BringIntoCombat boolean
--- @field IsHostileAct boolean
--- @field IsInvulnerable boolean
--- @field IsResistingDeath boolean
--- StatusFlags2
--- @field ForceStatus boolean
--- @field ForceFailStatus boolean
--- @field RequestClientSync boolean
--- @field RequestDelete boolean
--- @field RequestDeleteAtTurnEnd boolean
--- @field Started boolean


--- @class EsvStatusHit : EsvStatus
--- @field Hit HitRequest
--- @field HitByHandle ObjectHandle
--- @field HitWithHandle ObjectHandle
--- @field WeaponHandle ObjectHandle
--- @field HitReason string
--- @field SkillId string
--- @field Interruption boolean
--- @field AllowInterruptAction boolean
--- @field ForceInterrupt boolean
--- @field DecDelayDeathCount boolean
--- @field ImpactPosition number[]
--- @field ImpactOrigin number[]
--- @field ImpactDirection number[]


--- @class EsvStatusConsumeBase : EsvStatus
--- TODO - Skills, Items, ResetCooldownsSet, StatsIDs?
--- @field ResetAllCooldowns boolean
--- @field ResetOncePerCombat boolean
--- @field ScaleWithVitality boolean
--- @field LoseControl boolean
--- @field ApplyStatusOnTick string
--- @field EffectTime number
--- @field StatsId string
--- @field StackId string
--- @field OriginalWeaponStatsId string
--- @field OverrideWeaponStatsId string
--- @field OverrideWeaponHandle ObjectHandle
--- @field SavingThrow integer TODO enum + enum prop!
--- @field SourceDirection number[]
--- @field Turn integer
--- @field HealEffectOverride string See HealEffect enumeration
--- @field Poisoned boolean


--- @class EsvStatusDying : EsvStatus
--- @field SourceHandle ObjectHandle
--- @field SourceType integer
--- @field DeathType string See DeathType enumeration
--- @field AttackDirection integer
--- @field ImpactDirection number[]
--- @field IsAlreadyDead boolean
--- @field DieActionsCompleted boolean
--- @field ForceNoGhost boolean
--- @field SkipAnimation boolean
--- @field DontThrowDeathEvent boolean
--- @field InflicterHandle ObjectHandle
--- @field DisputeTargetHandle ObjectHandle
--- @field CombatId integer
--- @field IgnoreGodMode boolean

--- @class EsvStatusHeal : EsvStatus
--- @field EffectTime number
--- @field HealAmount integer
--- @field HealEffect string See HealEffect enumeration
--- @field HealEffectId string
--- @field HealType string See StatusHealType enumeration
--- @field AbsorbSurfaceRange integer
--- @field TargetDependentHeal boolean


--- @class EsvStatusMuted : EsvStatusConsumeBase

--- @class EsvStatusCharmed : EsvStatusConsumeBase
--- @field UserId integer
--- @field OriginalOwnerCharacterHandle ObjectHandle


--- @class EsvStatusKnockedDown : EsvStatus
--- @field KnockedDownState integer
--- @field IsInstant boolean


--- @class EsvStatusSummoning : EsvStatus
--- @field AnimationDuration number
--- @field SummonLevel integer


--- @class EsvStatusHealing : EsvStatus
--- @field HealAmount integer
--- @field TimeElapsed number
--- @field HealEffect string See HealEffect enumeration
--- @field HealEffectId string
--- @field SkipInitialEffect boolean
--- @field HealingEvent number
--- @field HealStat string See StatusHealType enumeration
--- @field AbsorbSurfaceRange integer


--- @class EsvStatusThrown : EsvStatus
--- @field Level integer
--- @field CasterHandle ObjectHandle
--- @field AnimationDuration number
--- @field IsThrowingSelf boolean
--- @field LandingEstimate number
--- @field Landed boolean


--- @class EsvStatusTeleportFall : EsvStatus
--- @field Target number[]
--- @field ReappearTime number
--- @field SkillId string
--- @field HasDamage boolean
--- @field HasDamageBeenApplied boolean


--- @class EsvStatusConsume : EsvStatusConsumeBase

--- @class EsvStatusCombat : EsvStatus
--- @field ReadyForCombat boolean
--- @field OwnerTeamId integer


--- @class EsvStatusAoO : EsvStatus
--- @field SourceHandle ObjectHandle
--- @field TargetHandle ObjectHandle
--- @field PartnerHandle ObjectHandle
--- @field ActivateAoOBoost boolean
--- @field ShowOverhead boolean


--- @class EsvStatusStoryFrozen : EsvStatus

--- @class EsvStatusSneaking : EsvStatus
--- @field ClientRequestStop boolean


--- @class EsvStatusUnlock : EsvStatus
--- @field SourceHandle ObjectHandle
--- @field Key string
--- @field Level integer
--- @field Unlocked integer


--- @class EsvStatusFear : EsvStatus

--- @class EsvStatusBoost : EsvStatus
--- @field EffectTime number
--- @field BoostId string


--- @class EsvStatusUnsheathed : EsvStatus
--- @field Force boolean


--- @class EsvStatusStance : EsvStatusConsumeBase
--- @field SkillId string


--- @class EsvStatusLying : EsvStatus
--- @field ItemHandle ObjectHandle
--- @field Position number[]
--- @field Index integer
--- @field TimeElapsed number
--- @field Heal number


--- @class EsvStatusBlind : EsvStatusConsumeBase

--- @class EsvStatusSmelly : EsvStatus

--- @class EsvStatusClean : EsvStatus

--- @class EsvStatusInfectiousDiseased : EsvStatusConsumeBase
--- @field Infections integer
--- @field InfectTimer number
--- @field Radius number
--- @field TargetHandle ObjectHandle

--- @class EsvStatusInvisible : EsvStatusConsumeBase
--- @field InvisiblePosition number[]

--- @class EsvStatusRotate : EsvStatus
--- @field Yaw number
--- @field RotationSpeed number

--- @class EsvStatusEncumbered : EsvStatusConsumeBase

--- @class EsvStatusIdentify : EsvStatus
--- @field Level integer
--- @field Identified integer
--- @field IdentifierHandle ObjectHandle

--- @class EsvStatusRepair : EsvStatus
--- @field Level integer
--- @field Repaired integer
--- @field RepairerHandle ObjectHandle

--- @class EsvStatusMaterial : EsvStatus
--- @field MaterialUUID string
--- @field ApplyOnBody boolean
--- @field ApplyOnArmor boolean
--- @field ApplyOnWeapon boolean
--- @field ApplyOnWings boolean
--- @field ApplyOnHorns boolean
--- @field ApplyOnOverhead boolean
--- @field IsOverlayMaterial boolean
--- @field Fading boolean
--- @field ApplyNormalMap boolean
--- @field Force boolean

--- @class EsvStatusLeadership : EsvStatusConsumeBase

--- @class EsvStatusExplode : EsvStatus
--- @field Projectile string

--- @class EsvStatusAdrenaline : EsvStatusConsumeBase
--- @field InitialAPMod integer
--- @field SecondaryAPMod integer
--- @field CombatTurn integer

--- @class EsvStatusShacklesOfPain : EsvStatusConsumeBase
--- @field CasterHandle ObjectHandle

--- @class EsvStatusShacklesOfPainCaster : EsvStatusConsumeBase
--- @field VictimHandle ObjectHandle

--- @class EsvStatusWindWalker : EsvStatusConsumeBase

--- @class EsvStatusDarkAvenger : EsvStatusConsumeBase

--- @class EsvStatusRemorse : EsvStatusConsumeBase

--- @class EsvStatusDecayingTouch : EsvStatusConsumeBase

--- @class EsvStatusUnhealable : EsvStatus

--- @class EsvStatusFlanked : EsvStatus

--- @class EsvStatusChanneling : EsvStatusStance

--- @class EsvStatusDrain : EsvStatus
--- @field Infused integer

--- @class EsvStatusLingeringWounds : EsvStatusConsumeBase

--- @class EsvStatusInfused : EsvStatus

--- @class EsvStatusSpiritVision : EsvStatusConsumeBase
--- @field SpiritVisionSkillId string

--- @class EsvStatusSpirit : EsvStatus

--- @class EsvStatusDamage : EsvStatusConsumeBase
--- @field DamageEvent integer
--- @field HitTimer number
--- @field TimeElapsed number
--- @field DamageLevel integer
--- @field DamageStats string
--- @field SpawnBlood boolean

--- @class EsvStatusForceMove : EsvStatusConsumeBase

--- @class EsvStatusClimbing : EsvStatus
--- @field MoveDirection number[]
--- @field LadderHandle ObjectHandle
--- @field Level string
--- @field Status integer
--- @field Direction boolean

--- @class EsvStatusIncapacitated : EsvStatusConsumeBase
--- @field CurrentFreezeTime number
--- @field FreezeTime number
--- @field FrozenFlag integer

--- @class EsvStatusInSurface : EsvStatus
--- @field SurfaceTimerCheck number
--- @field SurfaceDistanceCheck number
--- @field Translate number[]
--- @field Force boolean
--- TODO -- document surface layer flags!

--- @class EsvStatusSourceMuted : EsvStatus

--- @class EsvStatusOverpowered : EsvStatusConsumeBase

--- @class EsvStatusCombustion : EsvStatusConsumeBase

--- @class EsvStatusPolymorphed : EsvStatusConsumeBase
--- @field OriginalTemplate string
--- @field TransformedRace string
--- @field OriginalTemplateType integer
--- @field PolymorphResult string
--- @field DisableInteractions boolean

--- @class EsvStatusDamageOnMove : EsvStatusDamage
--- @field DistancePerDamage number
--- @field DistanceTraveled number

--- @class EsvStatusDemonicBargain : EsvStatus

--- @class EsvStatusGuardianAngel : EsvStatusConsumeBase

--- @class EsvStatusFloating : EsvStatusConsumeBase

--- @class EsvStatusChallenge : EsvStatusConsumeBase
--- @field SourceHandle ObjectHandle
--- @field Target boolean

--- @class EsvStatusDisarmed : EsvStatusConsumeBase

--- @class EsvStatusHealSharing : EsvStatusConsumeBase
--- @field CasterHandle ObjectHandle

--- @class EsvStatusHealSharingCaster : EsvStatusConsumeBase
--- @field TargetHandle ObjectHandle

--- @class EsvStatusExtraTurn : EsvStatusConsumeBase

--- @class EsvStatusActiveDefense : EsvStatusConsumeBase
--- @field Charges integer
--- @field TargetPos number[]
--- @field TargetHandle ObjectHandle
--- @field Radius number
--- @field Projectile string

--- @class EsvStatusSpark : EsvStatusConsumeBase
--- @field Charges integer
--- @field Radius number
--- @field Projectile string

--- @class EsvStatusPlayDead : EsvStatusConsumeBase

--- @class EsvStatusConstrained : EsvStatusLying

--- @class EsvStatusEffect : EsvStatus

--- @class EsvStatusDeactivated : EsvStatusConsumeBase

--- @class EsvStatusTutorialBed : EsvStatus



--- @class StatCharacterDynamic : StatBase
--- Properties from PropertyMap
--- @field SummonLifelinkModifier integer
--- @field Strength integer
--- @field Memory integer
--- @field Intelligence integer
--- @field Movement integer
--- @field MovementSpeedBoost integer
--- @field Finesse integer
--- @field Wits integer
--- @field Constitution integer
--- @field FireResistance integer
--- @field EarthResistance integer
--- @field WaterResistance integer
--- @field AirResistance integer
--- @field PoisonResistance integer
--- @field ShadowResistance integer
--- @field Willpower integer
--- @field Bodybuilding integer
--- @field PiercingResistance integer
--- @field PhysicalResistance integer
--- @field CorrosiveResistance integer
--- @field MagicResistance integer
--- @field CustomResistance integer
--- @field Sight integer
--- @field Hearing integer
--- @field FOV integer
--- @field APMaximum integer
--- @field APStart integer
--- @field APRecovery integer
--- @field CriticalChance integer
--- @field Initiative integer
--- @field Vitality integer
--- @field VitalityBoost integer
--- @field MagicPoints integer
--- @field Level integer
--- @field Gain integer
--- @field Armor integer
--- @field MagicArmor integer
--- @field ArmorBoost integer
--- @field MagicArmorBoost integer
--- @field ArmorBoostGrowthPerLevel integer
--- @field MagicArmorBoostGrowthPerLevel integer
--- @field DamageBoost integer
--- @field DamageBoostGrowthPerLevel integer
--- @field Accuracy integer
--- @field Dodge integer
--- @field MaxResistance integer
--- @field LifeSteal integer
--- @field Weight integer
--- @field ChanceToHitBoost integer
--- @field RangeBoost integer
--- @field APCostBoost integer
--- @field SPCostBoost integer
--- @field MaxSummons integer
--- @field BonusWeaponDamageMultiplier integer
--- @field TranslationKey integer
--- @field BonusWeapon integer
--- @field StepsType integer
-- TODO Abilities, Talents, RemovedTalents, Traits


--- @class StatBase
--- "Fake" base class to avoid declaring attribute flags/talents/abilities twice
---
--- StatAttributeFlags
--- @field FreezeImmunity boolean
--- @field BurnImmunity boolean
--- @field StunImmunity boolean
--- @field PoisonImmunity boolean
--- @field CharmImmunity boolean
--- @field FearImmunity boolean
--- @field KnockdownImmunity boolean
--- @field MuteImmunity boolean
--- @field ChilledImmunity boolean
--- @field WarmImmunity boolean
--- @field WetImmunity boolean
--- @field BleedingImmunity boolean
--- @field CrippledImmunity boolean
--- @field BlindImmunity boolean
--- @field CursedImmunity boolean
--- @field WeakImmunity boolean
--- @field SlowedImmunity boolean
--- @field DiseasedImmunity boolean
--- @field InfectiousDiseasedImmunity boolean
--- @field PetrifiedImmunity boolean
--- @field DrunkImmunity boolean
--- @field SlippingImmunity boolean
--- @field FreezeContact boolean
--- @field BurnContact boolean
--- @field StunContact boolean
--- @field PoisonContact boolean
--- @field ChillContact boolean
--- @field Torch boolean
--- @field Arrow boolean
--- @field Unbreakable boolean
--- @field Unrepairable boolean
--- @field Unstorable boolean
--- @field Grounded boolean
--- @field HastedImmunity boolean
--- @field TauntedImmunity boolean
--- @field SleepingImmunity boolean
--- @field AcidImmunity boolean
--- @field SuffocatingImmunity boolean
--- @field RegeneratingImmunity boolean
--- @field DisarmedImmunity boolean
--- @field DecayingImmunity boolean
--- @field ClairvoyantImmunity boolean
--- @field EnragedImmunity boolean
--- @field BlessedImmunity boolean
--- @field ProtectFromSummon boolean
--- @field Floating boolean
--- @field DeflectProjectiles boolean
--- @field IgnoreClouds boolean
--- @field MadnessImmunity boolean
--- @field ChickenImmunity boolean
--- @field IgnoreCursedOil boolean
--- @field ShockedImmunity boolean
--- @field WebImmunity boolean
--- @field LootableWhenEquipped boolean
--- @field PickpocketableWhenEquipped boolean
--- @field LoseDurabilityOnCharacterHit boolean
--- @field EntangledContact boolean
--- @field ShacklesOfPainImmunity boolean
--- @field MagicalSulfur boolean
--- @field ThrownImmunity boolean
--- @field InvisibilityImmunity boolean
---
--- Talents (prefix TALENT_)
--- @field TALENT_None boolean
--- @field TALENT_ItemMovement boolean
--- @field TALENT_ItemCreation boolean
--- @field TALENT_Flanking boolean
--- @field TALENT_AttackOfOpportunity boolean
--- @field TALENT_Backstab boolean
--- @field TALENT_Trade boolean
--- @field TALENT_Lockpick boolean
--- @field TALENT_ChanceToHitRanged boolean
--- @field TALENT_ChanceToHitMelee boolean
--- @field TALENT_Damage boolean
--- @field TALENT_ActionPoints boolean
--- @field TALENT_ActionPoints2 boolean
--- @field TALENT_Criticals boolean
--- @field TALENT_IncreasedArmor boolean
--- @field TALENT_Sight boolean
--- @field TALENT_ResistFear boolean
--- @field TALENT_ResistKnockdown boolean
--- @field TALENT_ResistStun boolean
--- @field TALENT_ResistPoison boolean
--- @field TALENT_ResistSilence boolean
--- @field TALENT_ResistDead boolean
--- @field TALENT_Carry boolean
--- @field TALENT_Throwing boolean
--- @field TALENT_Repair boolean
--- @field TALENT_ExpGain boolean
--- @field TALENT_ExtraStatPoints boolean
--- @field TALENT_ExtraSkillPoints boolean
--- @field TALENT_Durability boolean
--- @field TALENT_Awareness boolean
--- @field TALENT_Vitality boolean
--- @field TALENT_FireSpells boolean
--- @field TALENT_WaterSpells boolean
--- @field TALENT_AirSpells boolean
--- @field TALENT_EarthSpells boolean
--- @field TALENT_Charm boolean
--- @field TALENT_Intimidate boolean
--- @field TALENT_Reason boolean
--- @field TALENT_Luck boolean
--- @field TALENT_Initiative boolean
--- @field TALENT_InventoryAccess boolean
--- @field TALENT_AvoidDetection boolean
--- @field TALENT_AnimalEmpathy boolean
--- @field TALENT_Escapist boolean
--- @field TALENT_StandYourGround boolean
--- @field TALENT_SurpriseAttack boolean
--- @field TALENT_LightStep boolean
--- @field TALENT_ResurrectToFullHealth boolean
--- @field TALENT_Scientist boolean
--- @field TALENT_Raistlin boolean
--- @field TALENT_MrKnowItAll boolean
--- @field TALENT_WhatARush boolean
--- @field TALENT_FaroutDude boolean
--- @field TALENT_Leech boolean
--- @field TALENT_ElementalAffinity boolean
--- @field TALENT_FiveStarRestaurant boolean
--- @field TALENT_Bully boolean
--- @field TALENT_ElementalRanger boolean
--- @field TALENT_LightningRod boolean
--- @field TALENT_Politician boolean
--- @field TALENT_WeatherProof boolean
--- @field TALENT_LoneWolf boolean
--- @field TALENT_Zombie boolean
--- @field TALENT_Demon boolean
--- @field TALENT_IceKing boolean
--- @field TALENT_Courageous boolean
--- @field TALENT_GoldenMage boolean
--- @field TALENT_WalkItOff boolean
--- @field TALENT_FolkDancer boolean
--- @field TALENT_SpillNoBlood boolean
--- @field TALENT_Stench boolean
--- @field TALENT_Kickstarter boolean
--- @field TALENT_WarriorLoreNaturalArmor boolean
--- @field TALENT_WarriorLoreNaturalHealth boolean
--- @field TALENT_WarriorLoreNaturalResistance boolean
--- @field TALENT_RangerLoreArrowRecover boolean
--- @field TALENT_RangerLoreEvasionBonus boolean
--- @field TALENT_RangerLoreRangedAPBonus boolean
--- @field TALENT_RogueLoreDaggerAPBonus boolean
--- @field TALENT_RogueLoreDaggerBackStab boolean
--- @field TALENT_RogueLoreMovementBonus boolean
--- @field TALENT_RogueLoreHoldResistance boolean
--- @field TALENT_NoAttackOfOpportunity boolean
--- @field TALENT_WarriorLoreGrenadeRange boolean
--- @field TALENT_RogueLoreGrenadePrecision boolean
--- @field TALENT_WandCharge boolean
--- @field TALENT_DualWieldingDodging boolean
--- @field TALENT_Human_Inventive boolean
--- @field TALENT_Human_Civil boolean
--- @field TALENT_Elf_Lore boolean
--- @field TALENT_Elf_CorpseEating boolean
--- @field TALENT_Dwarf_Sturdy boolean
--- @field TALENT_Dwarf_Sneaking boolean
--- @field TALENT_Lizard_Resistance boolean
--- @field TALENT_Lizard_Persuasion boolean
--- @field TALENT_Perfectionist boolean
--- @field TALENT_Executioner boolean
--- @field TALENT_ViolentMagic boolean
--- @field TALENT_QuickStep boolean
--- @field TALENT_Quest_SpidersKiss_Str boolean
--- @field TALENT_Quest_SpidersKiss_Int boolean
--- @field TALENT_Quest_SpidersKiss_Per boolean
--- @field TALENT_Quest_SpidersKiss_Null boolean
--- @field TALENT_Memory boolean
--- @field TALENT_Quest_TradeSecrets boolean
--- @field TALENT_Quest_GhostTree boolean
--- @field TALENT_BeastMaster boolean
--- @field TALENT_LivingArmor boolean
--- @field TALENT_Torturer boolean
--- @field TALENT_Ambidextrous boolean
--- @field TALENT_Unstable boolean
--- @field TALENT_ResurrectExtraHealth boolean
--- @field TALENT_NaturalConductor boolean
--- @field TALENT_Quest_Rooted boolean
--- @field TALENT_PainDrinker boolean
--- @field TALENT_DeathfogResistant boolean
--- @field TALENT_Sourcerer boolean
--- @field TALENT_Rager boolean
--- @field TALENT_Elementalist boolean
--- @field TALENT_Sadist boolean
--- @field TALENT_Haymaker boolean
--- @field TALENT_Gladiator boolean
--- @field TALENT_Indomitable boolean
--- @field TALENT_WildMag boolean
--- @field TALENT_Jitterbug boolean
--- @field TALENT_Soulcatcher boolean
--- @field TALENT_MasterThief boolean
--- @field TALENT_GreedyVessel boolean
--- @field TALENT_MagicCycles boolean
---
--- Abilities
--- @field WarriorLore integer
--- @field RangerLore integer
--- @field RogueLore integer
--- @field SingleHanded integer
--- @field TwoHanded integer
--- @field PainReflection integer
--- @field Ranged integer
--- @field Shield integer
--- @field Reflexes integer
--- @field PhysicalArmorMastery integer
--- @field MagicArmorMastery integer
--- @field VitalityMastery integer
--- @field Sourcery integer
--- @field FireSpecialist integer
--- @field WaterSpecialist integer
--- @field AirSpecialist integer
--- @field EarthSpecialist integer
--- @field Necromancy integer
--- @field Summoning integer
--- @field Polymorph integer
--- @field Telekinesis integer
--- @field Repair integer
--- @field Sneaking integer
--- @field Pickpocket integer
--- @field Thievery integer
--- @field Loremaster integer
--- @field Crafting integer
--- @field Barter integer
--- @field Charm integer
--- @field Intimidate integer
--- @field Reason integer
--- @field Persuasion integer
--- @field Leadership integer
--- @field Luck integer
--- @field DualWielding integer
--- @field Wand integer
--- @field Perseverance integer
--- @field Runecrafting integer
--- @field Brewmaster integer
--- @field Sulfurology integer


--- @class StatCharacter : StatBase
--- Properties from PropertyMap
--- @field Level integer
--- @field Name string
--- @field AIFlags integer
--- @field CurrentVitality integer
--- @field CurrentArmor integer
--- @field CurrentMagicArmor integer
--- @field ArmorAfterHitCooldownMultiplier integer
--- @field MagicArmorAfterHitCooldownMultiplier integer
--- @field MPStart integer
--- @field CurrentAP integer
--- @field BonusActionPoints integer
--- @field Experience integer
--- @field Reputation integer
--- @field Flanked integer
--- @field Karma integer
--- @field MaxResistance integer
--- @field HasTwoHandedWeapon integer
--- @field IsIncapacitatedRefCount integer
--- @field MaxVitality integer
--- @field BaseMaxVitality integer
--- @field MaxArmor integer
--- @field BaseMaxArmor integer
--- @field MaxMagicArmor integer
--- @field BaseMaxMagicArmor integer
--- @field Sight number
--- @field BaseSight number
--- @field MaxSummons integer
--- @field BaseMaxSummons integer
--- @field MaxMpOverride integer
--- StatCharacterFlags
--- @field IsPlayer boolean
--- @field InParty boolean
--- @field IsSneaking boolean
--- @field Invisible boolean
--- @field Blind boolean
--- @field DrinkedPotion boolean
--- @field EquipmentValidated boolean
--- Properties from CDivinityStats_Character::GetStat
--- @field PhysicalResistance integer
--- @field PiercingResistance integer
--- @field CorrosiveResistance integer
--- @field MagicResistance integer
--- Base properties from CDivinityStats_Character::GetStat
--- @field BasePhysicalResistance integer
--- @field BasePiercingResistance integer
--- @field BaseCorrosiveResistance integer
--- @field BaseMagicResistance integer
--- Properties from CharacterStatsGetters::GetStat
--- @field MaxMp integer
--- @field APStart integer
--- @field APRecovery integer
--- @field APMaximum integer
--- @field Strength integer
--- @field Finesse integer
--- @field Intelligence integer
--- @field Constitution integer
--- @field Memory integer
--- @field Wits integer
--- @field Accuracy integer
--- @field Dodge integer
--- @field CriticalChance integer
--- @field FireResistance integer
--- @field EarthResistance integer
--- @field WaterResistance integer
--- @field AirResistance integer
--- @field PoisonResistance integer
--- @field ShadowResistance integer
--- @field CustomResistance integer
--- @field LifeSteal integer
--- @field Hearing integer
--- @field Movement integer
--- @field Initiative integer
--- @field BlockChance integer
--- @field ChanceToHitBoost integer
--- Base properties from CharacterStatsGetters::GetStat
--- @field BaseMaxMp integer
--- @field BaseAPStart integer
--- @field BaseAPRecovery integer
--- @field BaseAPMaximum integer
--- @field BaseStrength integer
--- @field BaseFinesse integer
--- @field BaseIntelligence integer
--- @field BaseConstitution integer
--- @field BaseMemory integer
--- @field BaseWits integer
--- @field BaseAccuracy integer
--- @field BaseDodge integer
--- @field BaseCriticalChance integer
--- @field BaseFireResistance integer
--- @field BaseEarthResistance integer
--- @field BaseWaterResistance integer
--- @field BaseAirResistance integer
--- @field BasePoisonResistance integer
--- @field BaseShadowResistance integer
--- @field BaseCustomResistance integer
--- @field BaseLifeSteal integer
--- @field BaseHearing integer
--- @field BaseMovement integer
--- @field BaseInitiative integer
--- @field BaseBlockChance integer
--- @field BaseChanceToHitBoost integer
--- Properties from CharacterFetchStat
--- @field DynamicStats StatCharacterDynamic[]
--- @field MainWeapon StatItem
--- @field OffHandWeapon StatItem
--- @field DamageBoost integer
--- @field Character EsvCharacter
--- @field Rotation number[]
--- @field Position number[]
--- @field MyGuid string
--- @field NetID integer
local StatCharacter = {}

--- Returns the item equipped in the specified slot
--- @param slot string See Itemslot enumeration
--- @return StatItem|nil
function StatCharacter:GetItemBySlot(slot) end

--- @class StatItemDynamic : StatBase
--- @field Durability integer
--- @field DurabilityDegradeSpeed integer
--- @field StrengthBoost integer
--- @field FinesseBoost integer
--- @field IntelligenceBoost integer
--- @field ConstitutionBoost integer
--- @field MemoryBoost integer
--- @field WitsBoost integer
--- @field SightBoost integer
--- @field HearingBoost integer
--- @field VitalityBoost integer
--- @field SourcePointsBoost integer
--- @field MaxAP integer
--- @field StartAP integer
--- @field APRecovery integer
--- @field AccuracyBoost integer
--- @field DodgeBoost integer
--- @field LifeSteal integer
--- @field CriticalChance integer
--- @field ChanceToHitBoost integer
--- @field MovementSpeedBoost integer
--- @field RuneSlots integer
--- @field RuneSlots_V1 integer
--- @field FireResistance integer
--- @field AirResistance integer
--- @field WaterResistance integer
--- @field EarthResistance integer
--- @field PoisonResistance integer
--- @field ShadowResistance integer
--- @field PiercingResistance integer
--- @field CorrosiveResistance integer
--- @field PhysicalResistance integer
--- @field MagicResistance integer
--- @field CustomResistance integer
--- @field Movement integer
--- @field Initiative integer
--- @field Willpower integer
--- @field Bodybuilding integer
--- @field MaxSummons integer
--- @field Value integer
--- @field Weight integer
--- @field Skills string
--- @field ItemColor string
--- @field ModifierType integer
--- @field ObjectInstanceName string
--- @field BoostName string
--- @field StatsType string See EquipmentStatsType enumeration
--- @field DamageType integer Weapon only! - See DamageType enumeration
--- @field MinDamage integer Weapon only!
--- @field MaxDamage integer Weapon only!
--- @field DamageBoost integer Weapon only!
--- @field DamageFromBase integer Weapon only!
--- @field CriticalDamage integer Weapon only!
--- @field WeaponRange integer Weapon only!
--- @field CleaveAngle integer Weapon only!
--- @field CleavePercentage integer Weapon only!
--- @field AttackAPCost integer Weapon only!
--- @field ArmorValue integer Shield/Armor only!
--- @field ArmorBoost integer Shield/Armor only!
--- @field MagicArmorValue integer Shield/Armor only!
--- @field MagicArmorBoost integer Shield/Armor only!
--- @field Blocking integer Shield only!
--- TODO - add attribute flags object support
--- TODO - AbilityModifiers, Talents, Reflection

--- @class StatProperty
--- @field Context string[] Target|Self|AoE|SelfOnHit|SelfOnEquip
--- @field Type string Custom|Status|SurfaceChange|GameAction|OsirisTask|Sabotage|Summon|Force
--- @field Condition string|nil

--- @class StatPropertyCustom : StatProperty
--- @field Type string Custom
--- @field Action string

--- @class StatPropertyStatus : StatProperty
--- @field Type string Status
--- @field Action string Status name
--- @field StatusChance number
--- @field Duration number
--- @field StatsId string
--- @field Arg4 integer
--- @field Arg5 integer
--- @field SurfaceBoost boolean
--- @field SurfaceBoosts string[] Labels from "Surface Type" enumeration

--- @class StatPropertySurfaceChange : StatProperty
--- @field Type string SurfaceChange
--- @field Action string Label from "Surface Change" enumeration
--- @field SurfaceChance number
--- @field Lifetime number
--- @field StatusChance number
--- @field Radius number

--- @class StatPropertySabotage : StatProperty
--- @field Type string Sabotage
--- @field Amount integer

--- @class StatPropertySummon : StatProperty
--- @field Type string Summon
--- @field Template string
--- @field Duration number
--- @field IsTotem boolean
--- @field Skill string

--- @class StatPropertyForce : StatProperty
--- @field Type string Force
--- @field Distance integer

--- @class StatPropertyGameAction : StatProperty
--- @field Type string GameAction
--- @field Action string Label from "Game Action" enumeration
--- @field Arg1 number
--- @field Arg2 number
--- @field Arg3 string
--- @field Arg4 number
--- @field Arg5 number
--- @field StatusHealType string Label from "StatusHealType" enumeration

--- @class StatPropertyOsirisTask : StatProperty
--- @field Type string OsirisTask
--- @field Action string Label from "Osiris Task" enumeration
--- @field Chance number
--- @field VitalityOnRevive integer

--- @class StatPropertyExtender : StatProperty
--- @field Type string Extender
--- @field Action string Custom SkillProperties action name
--- @field Arg1 number
--- @field Arg2 number
--- @field Arg3 string
--- @field Arg4 integer
--- @field Arg5 integer

--- @class StatItem : StatBase
--- Properties from property map
--- @field Level integer
--- @field Name string
--- @field InstanceId integer
--- @field ItemType string See EquipmentStatsType enumeration
--- @field ItemSlot string See ItemSlot enumeration
--- @field WeaponType string See WeaponType enumeration
--- @field AnimType integer TODO maybe an enum?
--- @field WeaponRange integer
--- @field IsIdentified integer
--- @field IsTwoHanded boolean
--- @field ShouldSyncStats boolean
--- @field HasModifiedSkills boolean
--- @field Skills string
--- @field DamageTypeOverwrite string See DamageType enumeration
--- @field Durability integer
--- @field DurabilityCounter integer
--- @field ItemTypeReal string
--- @field MaxCharges integer
--- @field Charges integer
---
--- Properties from ItemFetchStat
--- @field DynamicStats StatItemDynamic[]
---
--- Indirect properties from StatEntryWeapon/Armor/Shield
--- @field Using string
--- @field Damage integer
--- @field Act string See Act enumeration
--- @field Handedness string See Handedness enumeration
--- @field DamageBoost integer
--- @field DamageFromBase integer
--- @field CriticalDamage integer
--- @field CriticalChance integer
--- @field Movement integer
--- @field Initiative integer
--- @field Requirements StatRequirement[]
--- @field Slot string See Itemslot enumeration
--- @field DurabilityDegradeSpeed string See Qualifier enumeration
--- @field Value integer
--- @field ModifierType string See ModifierType enumeration
--- @field Projectile string
--- @field StrengthBoost string See Penalty Qualifier enumeration
--- @field FinesseBoost string See Penalty Qualifier enumeration
--- @field IntelligenceBoost string See Penalty Qualifier enumeration
--- @field ConstitutionBoost string See Penalty Qualifier enumeration
--- @field MemoryBoost string See Penalty Qualifier enumeration
--- @field WitsBoost string See Penalty Qualifier enumeration
--- @field SingleHanded integer
--- @field TwoHanded integer
--- @field Ranged integer
--- @field DualWielding integer
--- @field RogueLore integer
--- @field WarriorLore integer
--- @field RangerLore integer
--- @field FireSpecialist integer
--- @field WaterSpecialist integer
--- @field AirSpecialist integer
--- @field EarthSpecialist integer
--- @field Sourcery integer
--- @field Necromancy integer
--- @field Polymorph integer
--- @field Summoning integer
--- @field Leadership integer
--- @field PainReflection integer
--- @field Perseverance integer
--- @field Telekinesis integer
--- @field Sneaking integer
--- @field Thievery integer
--- @field Loremaster integer
--- @field Repair integer
--- @field Barter integer
--- @field Persuasion integer
--- @field Luck integer
--- @field Fire integer
--- @field Earth integer
--- @field Water integer
--- @field Air integer
--- @field Poison integer
--- @field Physical integer
--- @field Piercing integer
--- @field SightBoost string See Penalty Qualifier enumeration
--- @field HearingBoost string See Penalty Qualifier enumeration
--- @field VitalityBoost integer
--- @field MagicPointsBoost string See Penalty Qualifier enumeration
--- @field ChanceToHitBoost integer
--- @field APMaximum integer
--- @field APStart integer
--- @field APRecovery integer
--- @field AccuracyBoost integer
--- @field DodgeBoost integer
--- @field Weight integer
--- @field AttackAPCost integer
--- @field ComboCategory string
--- @field Flags string See AttributeFlags enumeration
--- @field Boosts string
--- @field InventoryTab string See InventoryTabs enumeration
--- @field Reflection string
--- @field ItemGroup string
--- @field ObjectCategory string
--- @field MinAmount integer
--- @field MaxAmount integer
--- @field Priority integer
--- @field Unique integer
--- @field MinLevel integer
--- @field MaxLevel integer
--- @field ItemColor string
--- @field MaxSummons integer
--- @field RuneSlots integer
--- @field RuneSlots_V1 integer
--- @field NeedsIdentification string See YesNo enumeration
--- @field LifeSteal integer
--- @field CleavePercentage integer
--- @field CleaveAngle integer
--- @field Talents string
--- @field IgnoreVisionBlock string See YesNo enumeration
--- @field Tags string
--- @field ArmorBoost integer
--- @field MagicArmorBoost integer
--- @field Blocking integer
--- @field ExtraProperties StatProperty[]
local StatItem = {}


--- @class EclGameObject
--- @field NetID integer
--- @field MyGuid string
--- TODO - Add more properties via virtual getters?
--- (IsGlobal, GetComponentType, GetFlags, GetTags, T/R/S, Velocity, Height, Level?)
local EclGameObject = {}


--- @class EclItem : EclGameObject
--- @field RootTemplate ItemTemplate
--- @field Stats StatItem
--- @field Handle ObjectHandle
---
--- @field WorldPos number[]
--- @field CurrentLevel string
--- @field Scale number
--- @field StatsId string
--- @field Weight integer
--- @field KeyName string
--- @field Level integer
--- @field ItemType string
--- @field GoldValueOverride integer
--- @field BaseWeightOverwrite integer
--- @field ItemColorOverride integer
--- @field DisplayName string
local EclItem = {}

--- Returns all delta mods on the item
--- @return string[]
function EclItem:GetDeltaMods() end

--- Returns the GUID of all items within the inventory of the item
--- @return string[]
function EclItem:GetInventoryItems() end

--- Returns the GUID of character that currently owns the item; nil if the item is not in a character inventory
--- @return string|nil
function EclItem:GetOwnerCharacter() end

--- Returns whether the item has the specified tag
--- @param tag string
--- @return boolean
function EclItem:HasTag(tag) end

--- Returns all tags on the item
--- @return string[]
function EclItem:GetTags() end

--- Returns the first status with the specified status ID, if one exists.
--- @param statusId string Status ID
--- @return EclStatus|nil
function EclItem:GetStatus(statusId) end

--- Returns the first status with the specified engine status type, if one exists.
--- @param type string Status type
--- @return EclStatus|nil
function EclItem:GetStatusByType(type) end

--- Returns all statuses on the item
--- @return string[]
function EclItem:GetStatuses() end

--- Returns all statuses on the item
--- @return EclStatus[]
function EclItem:GetStatusObjects() end



--- @class EclCharacter : EclGameObject
--- @field RootTemplate CharacterTemplate
--- @field PlayerCustomData PlayerCustomData
--- @field Stats StatCharacter
--- @field Handle ObjectHandle
---
--- @field WorldPos number[]
--- @field CurrentLevel string
--- @field Scale number
--- @field AnimationOverride string
--- @field UserID integer
--- @field DisplayName string
--- @field StoryDisplayName string
--- @field OriginalDisplayName string
--- @field WalkSpeedOverride number
--- @field RunSpeedOverride number
--- @field Archetype string
--- @field CorpseLootable boolean
local EclCharacter = {}

--- Returns the GUID of all items within the inventory of the character
--- @return string[]
function EclCharacter:GetInventoryItems() end

--- Returns the item equipped in the specified slot
--- @param slot string See ItemSlot enumeration
--- @return EclItem|nil
function EclCharacter:GetItemBySlot(slot) end

--- Returns whether the character has the specified tag
--- @param tag string
--- @return boolean
function EclCharacter:HasTag(tag) end

--- Returns all tags on the character
--- @return string[]
function EclCharacter:GetTags() end

--- Returns the first status with the specified status ID, if one exists.
--- @param statusId string Status ID
--- @return EclStatus|nil
function EclCharacter:GetStatus(statusId) end

--- Returns the first status with the specified engine status type, if one exists.
--- @param type string Status type
--- @return EclStatus|nil
function EclCharacter:GetStatusByType(type) end

--- Returns all statuses on the character
--- @return string[]
function EclCharacter:GetStatuses() end

--- Returns all statuses on the character
--- @return EclStatus[]
function EclCharacter:GetStatusObjects() end

--- Update model scale of the character.
--- @param scale number 
function EclCharacter:SetScale(scale) end

--- Returns the value of the specified custom stat
--- @param statId string Custom stat UUID
--- @return number Stat value
function EclCharacter:GetCustomStat(statId) end



--- @class EsvGameObject
--- @field NetID integer
--- @field MyGuid string
--- TODO - Add more properties via virtual getters?
--- (IsGlobal, GetComponentType, GetFlags, GetTags, T/R/S, Velocity, Height, Level?)
local EsvGameObject = {}


--- @class EsvItem : EsvGameObject
--- @field Handle ObjectHandle
--- @field RootTemplate ItemTemplate
--- @field WorldPos number[]
--- @field CurrentLevel string
--- @field Scale number
--- @field CustomDisplayName string
--- @field CustomDescription string
--- @field CustomBookContent string
--- @field StatsId string
--- @field InventoryHandle ObjectHandle
--- @field ParentInventoryHandle ObjectHandle
--- @field Slot integer
--- @field Amount integer
--- @field Vitality integer
--- @field Armor integer
--- @field InUseByCharacterHandle ObjectHandle
--- @field Key string
--- @field LockLevel integer
--- @field OwnerHandle ObjectHandle
--- @field ItemType string
--- @field GoldValueOverwrite integer
--- @field WeightValueOverwrite integer
--- @field TreasureLevel integer
--- @field LevelOverride integer
--- @field ForceSynch boolean
--- @field DisplayName string
--- From ItemFlags
--- @field Activated boolean
--- @field OffStage boolean
--- @field CanBePickedUp boolean
--- @field CanBeMoved boolean
--- @field WalkOn boolean
--- @field WalkThrough boolean
--- @field NoCover boolean
--- @field CanShootThrough boolean
--- @field CanUse boolean
--- @field InteractionDisabled boolean
--- @field Destroyed boolean
--- @field LoadedTemplate boolean
--- @field IsDoor boolean
--- @field StoryItem boolean
--- @field Summon boolean
--- @field FreezeGravity boolean
--- @field ForceSync boolean
--- @field IsLadder boolean
--- @field PositionChanged boolean
--- @field Totem boolean
--- @field Destroy boolean
--- @field GMFolding boolean
--- @field Sticky boolean
--- @field DoorFlag boolean
--- @field Floating boolean
--- @field IsSurfaceBlocker boolean
--- @field IsSurfaceCloudBlocker boolean
--- @field SourceContainer boolean
--- @field Frozen boolean
--- @field TeleportOnUse boolean
--- @field PinnedContainer boolean
--- From ItemFlags2
--- @field UnsoldGenerated boolean
--- @field IsKey boolean
--- @field Global boolean
--- @field CanConsume boolean
--- @field TreasureGenerated boolean
--- @field UnEquipLocked boolean
--- @field UseRemotely boolean
--- 
--- @field Stats StatItem
local EsvItem = {}

--- Returns all delta mods on the item
--- @return string[]
function EsvItem:GetDeltaMods() end

--- Updates the delta mod list on the item.
--- The new deltamods will only be applied after a save/reload!
--- @param deltamods string[]
function EsvItem:SetDeltaMods(deltamods) end

--- Returns all boosts that were added during treasure generation
--- @return string[]
function EsvItem:GetGeneratedBoosts() end

--- Updates the list of boosts that were added during treasure generation.
--- The new boosts will only be applied after a save/reload!
--- @param boosts string[]
function EsvItem:SetGeneratedBoosts(boosts) end

--- Returns the GUID of all items within the inventory of the item
--- @return string[]
function EsvItem:GetInventoryItems() end

--- Returns the GUID of all characters within the specified radius
--- @param radius number
--- @return string[]
function EsvItem:GetNearbyCharacters(radius) end

--- Returns whether the item has the specified tag
--- @param tag string
--- @return boolean
function EsvItem:HasTag(tag) end

--- Returns all tags on the item
--- @return string[]
function EsvItem:GetTags() end

--- Returns the first status with the specified status ID, if one exists.
--- @param statusId string Status ID
--- @return EsvStatus|nil
function EsvItem:GetStatus(statusId) end

--- Returns the first status with the specified engine status type, if one exists.
--- @param type string Status type
--- @return EsvStatus|nil
function EsvItem:GetStatusByType(type) end

--- Returns all statuses on the item
--- @return string[]
function EsvItem:GetStatuses() end

--- Returns all statuses on the character
--- @return EsvStatus[]
function EsvItem:GetStatusObjects() end




--- @class ItemDefinition
--- @field RootTemplate string
--- @field OriginalRootTemplate string
--- @field Slot integer
--- @field Amount integer
--- @field GoldValueOverwrite integer
--- @field WeightValueOverwrite integer
--- @field DamageTypeOverwrite string DamageType enumeration
--- @field ItemType string
--- @field CustomDisplayName string
--- @field CustomDescription string
--- @field CustomBookContent string
--- @field GenerationStatsId string
--- @field GenerationItemType string
--- @field GenerationRandom integer
--- @field GenerationLevel integer
--- @field StatsLevel integer
--- @field Key string
--- @field LockLevel integer
--- @field EquipmentStatsType integer
--- @field HasModifiedSkills boolean
--- @field Skills string
--- @field HasGeneratedStats boolean
--- @field IsIdentified boolean
--- @field GMFolding boolean
--- @field CanUseRemotely boolean
--- @field GenerationBoosts string[]
--- @field RuneBoosts string[]
--- @field DeltaMods string[]
local ItemDefinition = {}

--- Clears item progression data (name group, level group, etc.)
function ItemDefinition:ResetProgression() end



--- @class ItemConstructor
--- Creates an item based on a parsed item or newly created item definition.
--- Should be initialized using Ext.CreateItemConstructor(item/template) first.
--- Item definitions can be accessed using c[1], c[2], etc.
--- For non-recursive item cloning (i.e. creating a single item), there is only one item (c[1]).
--- For container cloning, the contained items are accessible using c[2], c[3], etc.
local ItemConstructor = {}

--- Constructs an instance of the item contained in the constructor definition.
--- The definition is cleared after the item is created.
--- @return EsvItem|nil
function ItemConstructor:Construct() end



--- @class PlayerCustomData
--- @field CustomLookEnabled boolean
--- @field Name string
--- @field ClassType string
--- @field SkinColor integer
--- @field HairColor integer
--- @field ClothColor1 integer
--- @field ClothColor2 integer
--- @field ClothColor3 integer
--- @field IsMale boolean
--- @field Race string
--- @field OriginName string
--- @field Icon string
--- @field MusicInstrument string
--- @field OwnerProfileID string
--- @field ReservedProfileID string
--- @field AiPersonality string
--- @field Speaker string
local PlayerCustomData = {}

--- @class EsvSkillInfo
--- @field ActiveCooldown number
--- @field CauseList StatusHandle[] Array of statuses handles granting this skill, if any. Use individual entries together with Ext.GetStatus.
--- @field IsActivated boolean
--- @field IsLearned boolean
--- @field ZeroMemory boolean
--- @field OncePerCombat boolean
--- @field NumCharges number
--- @see EsvCharacter#GetSkillInfo
local EsvSkillInfo = {}

--- @class EsvCharacter : EsvGameObject
--- @field RootTemplate CharacterTemplate
--- @field PlayerCustomData PlayerCustomData
--- @field Stats StatCharacter
--- @field DisplayName string
--- @field Handle ObjectHandle
---
--- @field NetID integer
--- @field MyGuid string
--- @field WorldPos number[]
--- @field CurrentLevel string
--- @field Scale number
--- @field AnimationOverride string
--- @field WalkSpeedOverride number
--- @field RunSpeedOverride number
--- @field NeedsUpdateCount integer
--- @field ScriptForceUpdateCount integer
--- @field ForceSynchCount integer
--- @field InventoryHandle ObjectHandle Not useful yet as you can't use inventory handles for any API
--- @field SkillBeingPrepared string Set while preparing a skill, cleared afterwards
--- @field LifeTime number Used for summons to indicate remaining lifetime
--- @field TurnTimer number
--- @field TriggerTrapsTimer number
--- @field UserID integer
--- @field ReservedUserID integer
--- @field OwnerHandle ObjectHandle Handle of owner character (in case of summons/followers)
--- @field FollowCharacterHandle ObjectHandle
--- @field SpiritCharacterHandle ObjectHandle
--- @field CorpseCharacterHandle ObjectHandle
--- @field PartialAP number Movement AP
--- @field AnimType integer
--- @field DelayDeathCount integer
--- @field AnimationSetOverride string
--- @field OriginalTransformDisplayName string
--- @field PartyHandle ObjectHandle
--- @field CustomTradeTreasure string
--- @field IsAlarmed boolean
--- @field CrimeWarningsEnabled boolean
--- @field CrimeInterrogationEnabled boolean
--- @field MovingCasterHandle ObjectHandle
--- @field Archetype string
--- @field EquipmentColor string
--- @field ProjectileTemplate string
--- @field ReadyCheckBlocked boolean
--- @field CorpseLootable boolean
--- @field CustomBloodSurface string
--- @field PreviousLevel string
---
--- CharacterFlags 0
--- @field IsPlayer boolean
--- @field Multiplayer boolean
--- @field InParty boolean
--- @field HostControl boolean
--- @field Activated boolean
--- @field OffStage boolean
--- @field Dead boolean
--- @field HasOwner boolean
--- @field InDialog boolean
--- @field Summon boolean
--- @field CannotDie boolean
--- @field CharacterControl boolean
--- @field Loaded boolean
--- @field InArena boolean
--- @field CharacterCreationFinished boolean
--- @field Floating boolean
--- @field SpotSneakers boolean
--- @field Temporary boolean
--- @field WalkThrough boolean
--- @field CoverAmount boolean
--- @field CanShootThrough boolean
--- @field PartyFollower boolean
--- @field Totem boolean
--- @field NoRotate boolean
--- @field Deactivated boolean
--- @field IsHuge boolean
--- @field MadePlayer boolean
--- @field LevelTransitionPending boolean
--- @field RegisteredForAutomatedDialog boolean
---
--- CharacterFlags 2
--- @field Global boolean
--- @field HasOsirisDialog boolean
--- @field HasDefaultDialog boolean
--- @field TreasureGeneratedForTrader boolean
--- @field Trader boolean
--- @field Resurrected boolean
---
--- CharacterFlags 3
--- @field IsPet boolean
--- @field IsSpectating boolean
--- @field NoReptuationEffects boolean
--- @field HasWalkSpeedOverride boolean
--- @field HasRunSpeedOverride boolean
--- @field IsGameMaster boolean
--- @field IsPossessed boolean
--- @field ManuallyLeveled boolean
local EsvCharacter = {}

--- Returns the GUID of all items in the characters inventory
--- @return string[]
function EsvCharacter:GetInventoryItems() end

--- Returns detailed information about the specified skill
--- @param skillId string
--- @return EsvSkillInfo
function EsvCharacter:GetSkillInfo(skillId) end

--- Returns the name of all skills available to the character
--- @return string[]
function EsvCharacter:GetSkills() end

--- Returns the GUID of all characters within the specified radius
--- @return string[]
function EsvCharacter:GetNearbyCharacters(radius) end

--- Returns the GUID of all summons owned by the character
--- @return string[]
function EsvCharacter:GetSummons() end

--- Returns whether the character has the specified tag
--- @param tag string
--- @return boolean
function EsvCharacter:HasTag(tag) end

--- Returns all tags on the character
--- @return string[]
function EsvCharacter:GetTags() end

--- Returns the first status with the specified status ID, if one exists.
--- @param statusId string Status ID
--- @return EsvStatus|nil
function EsvCharacter:GetStatus(statusId) end

--- Returns the first status with the specified engine status type, if one exists.
--- @param type string Status type
--- @return EsvStatus|nil
function EsvCharacter:GetStatusByType(type) end

--- Returns all statuses on the character
--- @return string[]
function EsvCharacter:GetStatuses() end

--- Returns all statuses on the character
--- @return EsvStatus[]
function EsvCharacter:GetStatusObjects() end

--- Update model scale of the character.
--- NOTE: This change must be manually synchronized to the client!
--- @param scale number 
function EsvCharacter:SetScale(scale) end

--- Returns the value of the specified custom stat
--- @param statId string Custom stat UUID
--- @return number Stat value
function EsvCharacter:GetCustomStat(statId) end

--- Updates the value of the specified custom stat
--- @param statId string Custom stat UUID
--- @param statValue number Stat value
function EsvCharacter:SetCustomStat(statId, statValue) end



--- @class EsvSurface
--- @field NetId integer
--- @field MyHandle ObjectHandle
--- @field SurfaceType string See SurfaceType enumeration
--- @field RootTemplate SurfaceTemplate
--- @field Flags integer
--- @field TeamId integer
--- @field OwnerHandle ObjectHandle
--- @field LifeTime number
--- @field LifeTimeFromTemplate boolean
--- @field StatusChance number
--- @field Index integer
--- @field OwnershipTimer number


--- @class EsvShootProjectileRequest
--- @field SkillId string
--- @field Caster ObjectHandle
--- @field Source ObjectHandle
--- @field Target ObjectHandle
--- @field StartPosition number[]
--- @field EndPosition number[]
--- @field Random integer
--- @field CasterLevel integer
--- @field IsTrap boolean
--- @field UnknownFlag1 boolean
--- @field CleanseStatuses string
--- @field StatusClearChance integer
--- @field IsFromItem boolean
--- @field IsStealthed boolean
--- @field IgnoreObjects boolean
--- TODO - DamageList, HitObject
local EsvShootProjectileRequest = {}


--- @class EsvProjectile : EsvGameObject
--- @field RootTemplate ProjectileTemplate
--- @field Handle ObjectHandle
--- @field NetID integer
--- @field MyGuid string
--- @field CasterHandle ObjectHandle
--- @field SourceHandle ObjectHandle
--- @field TargetObjectHandle ObjectHandle
--- @field HitObjectHandle ObjectHandle
--- @field SourcePosition number[]
--- @field TargetPosition number[]
--- @field DamageType string
--- @field DamageSourceType string
--- @field LifeTime number
--- @field HitInterpolation integer
--- @field ExplodeRadius0 number
--- @field ExplodeRadius1 number
--- @field DeathType string
--- @field SkillId string
--- @field WeaponHandle ObjectHandle
--- @field MovingEffectHandle ObjectHandle
--- @field SpawnEffect string
--- @field SpawnFXOverridesImpactFX boolean
--- @field EffectHandle string
--- @field RequestDelete boolean
--- @field Launched boolean
--- @field IsTrap boolean
--- @field UseCharacterStats boolean
--- @field ReduceDurability boolean
--- @field AlwaysDamage boolean
--- @field ForceTarget boolean
--- @field IsFromItem boolean
--- @field DivideDamage boolean
--- @field IgnoreRoof boolean
--- @field CanDeflect boolean
--- @field IgnoreObjects boolean
--- @field CleanseStatuses string
--- @field StatusClearChance integer
--- @field Position number[]
--- @field PrevPosition number[]
--- @field Velocity number[]
--- @field Scale number
--- @field CurrentLevel string
local EsvProjectile = {}


--- @class EsvSoundVolumeTriggerData
--- @field AmbientSound string
--- @field Occlusion number
--- @field AuxBus1 integer
--- @field AuxBus2 integer
--- @field AuxBus3 integer
--- @field AuxBus4 integer


--- @class EsvAtmosphereTriggerData
--- @field Atmospheres string[]
--- @field FadeTime number


--- @class EsvTrigger : EsvGameObject
--- @field RootTemplate TriggerTemplate
--- @field Handle ObjectHandle
--- @field UUID string
--- @field SyncFlags integer
--- @field Translate number[]
--- @field TriggerType string
--- @field IsGlobal boolean
--- @field Level string
--- @field TriggerData EsvAtmosphereTriggerData|EsvSoundVolumeTriggerData


--- @class EsvSurfaceAction
--- @field MyHandle ObjectHandle
local EsvSurfaceAction = {}


--- @class EsvCreateSurfaceActionBase : EsvSurfaceAction
--- @field OwnerHandle ObjectHandle
--- @field Duration number
--- @field StatusChance number
--- @field Position number[]
--- @field SurfaceType string
local EsvCreateSurfaceActionBase = {}


--- @class EsvCreateSurfaceAction : EsvCreateSurfaceActionBase
--- @field Radius number
--- @field ExcludeRadius number
--- @field MaxHeight number
--- @field IgnoreIrreplacableSurfaces boolean
--- @field CheckExistingSurfaces boolean
--- @field SurfaceCollisionFlags integer
--- @field SurfaceCollisionNotOnFlags integer
--- @field Timer number
--- @field GrowTimer number
--- @field GrowStep integer
--- @field SurfaceLayer integer
local EsvCreateSurfaceAction = {}


--- @class EsvChangeSurfaceOnPathAction : EsvCreateSurfaceActionBase
--- @field FollowObject ObjectHandle
--- @field Radius number
--- @field IgnoreIrreplacableSurfaces boolean
--- @field CheckExistingSurfaces boolean
--- @field SurfaceCollisionFlags integer
--- @field SurfaceCollisionNotOnFlags integer
--- @field IgnoreOwnerCells boolean
local EsvChangeSurfaceOnPathAction = {}


--- @class EsvCreatePuddleAction : EsvCreateSurfaceActionBase
--- @field SurfaceCells integer
--- @field Step integer
--- @field GrowSpeed number
--- @field IgnoreIrreplacableSurfaces boolean
--- @field GrowTimer number
local EsvCreatePuddleAction = {}


--- @class EsvExtinguishFireAction : EsvCreateSurfaceActionBase
--- @field Position number[]
--- @field Radius number
--- @field Percentage number
--- @field GrowTimer number
--- @field Step number
local EsvExtinguishFireAction = {}


--- @class EsvRectangleSurfaceAction : EsvCreateSurfaceActionBase
--- @field DamageList DamageList
--- @field Target number[]
--- @field SurfaceArea number
--- @field Width number
--- @field Length number
--- @field GrowTimer number
--- @field MaxHeight number
--- @field GrowStep integer
--- @field AiFlags integer
--- @field DeathType string
--- @field LineCheckBlock integer
local EsvRectangleSurfaceAction = {}


--- @class EsvPolygonSurfaceAction : EsvCreateSurfaceActionBase
--- @field DamageList DamageList
--- @field Vertices number[][]
--- @field PositionX number
--- @field PositionZ number
--- @field GrowTimer number
--- @field GrowStep integer
local EsvPolygonSurfaceAction = {}


--- @class EsvSwapSurfaceAction : EsvCreateSurfaceActionBase
--- @field Radius number
--- @field ExcludeRadius number
--- @field MaxHeight number
--- @field Target number[]
--- @field IgnoreIrreplacableSurfaces boolean
--- @field CheckExistingSurfaces boolean
--- @field SurfaceCollisionFlags integer
--- @field SurfaceCollisionNotOnFlags integer
--- @field LineCheckBlock integer
--- @field GrowTimer number
--- @field GrowStep integer
local EsvSwapSurfaceAction = {}


--- @class EsvZoneAction : EsvCreateSurfaceActionBase
--- @field SkillId string
--- @field DamageList DamageList
--- @field Target number[]
--- @field Shape integer
--- @field Radius number
--- @field AngleOrBase number
--- @field BackStart number
--- @field FrontOffset number
--- @field MaxHeight number
--- @field GrowTimer number
--- @field GrowStep integer
--- @field AiFlags integer
--- @field DeathType string
local EsvZoneAction = {}


--- @class EsvTransformSurfaceAction : EsvSurfaceAction
--- @field SurfaceTransformAction string
--- @field OriginSurface string
--- @field SurfaceLayer integer
--- @field GrowCellPerSecond number
--- @field OwnerHandle2 ObjectHandle
--- @field Position number[]
--- @field SurfaceLifetime number
--- @field SurfaceStatusChance number
local EsvTransformSurfaceAction = {}


--- @class ModInfo
--- @field UUID string
--- @field Name string
--- @field Version integer
--- @field PublishVersion integer
--- @field Directory string
--- @field Author string
--- @field Description string
--- @field ModuleType string
--- @field Dependencies string[] Dependency mod UUID-s
local ModInfo = {}


--- @class CustomSkillProperty
--- @field GetDescription fun(property:StatPropertyExtender):string|nil
--- @field ExecuteOnPosition fun(property:StatPropertyExtender, attacker: EsvGameObject, position: number[], areaRadius: number, isFromItem: boolean, skill: StatEntrySkillData|nil, hit: HitRequest|nil)
--- @field ExecuteOnTarget fun(property:StatPropertyExtender, attacker: EsvGameObject, target: EsvGameObject, position: number[], isFromItem: boolean, skill: StatEntrySkillData|nil, hit: HitRequest|nil)


--- @class StatSkillSet
--- @field Name string
--- @field Skills string[]

--- @class StatEquipmentGroup
--- @field Name string
--- @field Equipment string[]

--- @class StatEquipmentSet
--- @field Name string
--- @field Groups StatEquipmentGroup[]

--- @class DeltaModBoost
--- @field Boost string
--- @field Count integer
local DeltaModBoost = {}

--- @class DeltaMod
--- @field ModifierType string
--- @field SlotType string
--- @field WeaponType string
--- @field ArmorType string
--- @field Handedness string
--- @field Name string
--- @field BoostType string
--- @field MinLevel integer
--- @field MaxLevel integer
--- @field Frequency integer
--- @field Boosts DeltaModBoost[]
local DeltaMod = {}

---@class DeltaModStatEntry
---@field Name string
---@field ModifierType string

--- @class ItemComboIngredients
--- @field Object string
--- @field IngredientType string See IngredientType enumeration
--- @field Transform string See IngredientTransformType enumeration
--- @field ItemRarity string See ItemDataRarity enumeration (ValueLists.txt only!)
local ItemComboIngredients = {}

--- @class ItemComboResultElement
--- @field Result string
--- @field Boost string
--- @field ResultAmount number
local ItemComboResultElement = {}

--- @class ItemComboResult
--- @field Requirement string See Ability enum
--- @field ReqLevel number
--- @field PreviewStatsId string
--- @field PreviewIcon string
--- @field PreviewTooltip string
--- @field Name string
--- @field Results ItemComboResultElement[]
local ItemComboResult = {}

--- @class ItemCombo
--- @field Name string
--- @field RecipeCategory string See RecipeCategory enum
--- @field CraftingStation string See CraftingStationType enum
--- @field Ingredients ItemComboIngredients[]
--- @field Results ItemComboResult[]
local ItemCombo = {}

--- @class ItemComboPreviewData
--- @field Name string
--- @field Type string
--- @field StatsId string
--- @field Tooltip string
--- @field Icon string
local ItemComboPreviewData = {}

--- @class ItemComboPropertyElement
--- @field ObjectId string
--- @field IngredientType string See IngredientType enumeration
--- @field Result string
local ItemComboPropertyElement = {}

--- @class ItemComboProperty
--- @field Name string
--- @field PreviewIcon string
--- @field PreviewTooltip string
--- @field Entries ItemComboPropertyElement[]
local ItemComboProperty = {}

---@class StatTreasureCategory
---@field Frequency integer
---@field TreasureTable string|nil
---@field TreasureCategory string|nil
---@field Common integer
---@field Uncommon integer
---@field Rare integer
---@field Epic integer
---@field Legendary integer
---@field Divine integer
---@field Unique integer

---@class StatTreasureDropCount
---@field Chance integer
---@field Amount integer

---@class StatTreasureSubTable
---@field Categories StatTreasureCategory[]
---@field DropCounts StatTreasureDropCount[]
---@field Amounts integer[]
---@field StartLevel integer
---@field EndLevel integer
---@field TotalCount integer
---@field TotalFrequency integer

---@class StatTreasureTable
---@field Name string
---@field MinLevel integer
---@field MaxLevel integer
---@field IgnoreLevelDiff boolean
---@field UseTreasureGroupContainers boolean
---@field CanMerge boolean
---@field SubTables StatTreasureSubTable[]

--- @class ItemNameGroupLink
--- @field NameGroup string
--- @field NoneCoolSuffix number
--- @field ItemName string
local ItemNameGroupLink = {}

--- @class ItemRootGroup
--- @field MinLevel number
--- @field MaxLevel number
--- @field RootGroup string
--- @field NameGroupLinks ItemNameGroupLink[]
local ItemRootGroup = {}

--- @class ItemLevelGroup
--- @field MinLevel number
--- @field MaxLevel number
--- @field Name string
--- @field RootGroups ItemRootGroup[]
local ItemLevelGroup = {}

--- @class ItemGroup
--- @field Name string
--- @field LevelGroups ItemLevelGroup[]
local ItemGroup = {}

--- @class ItemNameGroupName
--- @field Name string
--- @field Name2 string
local ItemNameGroupName = {}

--- @class ItemNameGroup
--- @field Name string
--- @field Names ItemNameGroupName[]
--- @field NamesCool ItemNameGroupName[]
local ItemNameGroup = {}


--- @class StatRequirement
--- @field Requirement string
--- @field Param string|integer
--- @field Not boolean
local StatRequirement = {}


--- @class StatEntryWeapon
--- @field Name string
--- @field Level integer
--- @field Using string
--- @field Damage integer
--- @field Act string See Act enumeration
--- @field Handedness string See Handedness enumeration
--- @field IsTwoHanded string See YesNo enumeration
--- @field DamageBoost integer
--- @field DamageFromBase integer
--- @field CriticalDamage integer
--- @field CriticalChance integer
--- @field Movement integer
--- @field Initiative integer
--- @field Requirements StatRequirement[]
--- @field Slot string See Itemslot enumeration
--- @field Durability integer
--- @field DurabilityDegradeSpeed string See Qualifier enumeration
--- @field Value integer
--- @field WeaponType string See WeaponType enumeration
--- @field AnimType string See AnimType enumeration
--- @field WeaponRange integer
--- @field ModifierType string See ModifierType enumeration
--- @field Projectile string
--- @field StrengthBoost string See Penalty Qualifier enumeration
--- @field FinesseBoost string See Penalty Qualifier enumeration
--- @field IntelligenceBoost string See Penalty Qualifier enumeration
--- @field ConstitutionBoost string See Penalty Qualifier enumeration
--- @field MemoryBoost string See Penalty Qualifier enumeration
--- @field WitsBoost string See Penalty Qualifier enumeration
--- @field SingleHanded integer
--- @field TwoHanded integer
--- @field Ranged integer
--- @field DualWielding integer
--- @field RogueLore integer
--- @field WarriorLore integer
--- @field RangerLore integer
--- @field FireSpecialist integer
--- @field WaterSpecialist integer
--- @field AirSpecialist integer
--- @field EarthSpecialist integer
--- @field Sourcery integer
--- @field Necromancy integer
--- @field Polymorph integer
--- @field Summoning integer
--- @field Leadership integer
--- @field PainReflection integer
--- @field Perseverance integer
--- @field Telekinesis integer
--- @field Sneaking integer
--- @field Thievery integer
--- @field Loremaster integer
--- @field Repair integer
--- @field Barter integer
--- @field Persuasion integer
--- @field Luck integer
--- @field Fire integer
--- @field Earth integer
--- @field Water integer
--- @field Air integer
--- @field Poison integer
--- @field Physical integer
--- @field Piercing integer
--- @field SightBoost string See Penalty Qualifier enumeration
--- @field HearingBoost string See Penalty Qualifier enumeration
--- @field VitalityBoost integer
--- @field MagicPointsBoost string See Penalty Qualifier enumeration
--- @field ChanceToHitBoost integer
--- @field APMaximum integer
--- @field APStart integer
--- @field APRecovery integer
--- @field AccuracyBoost integer
--- @field DodgeBoost integer
--- @field Weight integer
--- @field AttackAPCost integer
--- @field ComboCategory string
--- @field Flags string See AttributeFlags enumeration
--- @field Boosts string
--- @field InventoryTab string See InventoryTabs enumeration
--- @field Charges integer
--- @field MaxCharges integer
--- @field Skills string
--- @field Reflection string
--- @field ItemGroup string
--- @field ObjectCategory string
--- @field MinAmount integer
--- @field MaxAmount integer
--- @field Priority integer
--- @field Unique integer
--- @field MinLevel integer
--- @field MaxLevel integer
--- @field ItemColor string
--- @field MaxSummons integer
--- @field RuneSlots integer
--- @field RuneSlots_V1 integer
--- @field NeedsIdentification string See YesNo enumeration
--- @field LifeSteal integer
--- @field CleavePercentage integer
--- @field CleaveAngle integer
--- @field Talents string
--- @field IgnoreVisionBlock string See YesNo enumeration
--- @field Tags string
--- @field ExtraProperties StatProperty[]
local StatEntryWeapon = {
    --- @type string See Damage Type enumeration
    ['Damage Type'] = "",
    --- @type integer
    ['Damage Range'] = 0,
    --- @type string See ActPart enumeration
    ['Act part'] = ""
}


--- @class StatEntryArmor
--- @field Name string
--- @field Level integer
--- @field Using string
--- @field ArmorBoost integer
--- @field MagicArmorBoost integer
--- @field Movement integer
--- @field Initiative integer
--- @field Requirements StatRequirement[]
--- @field Slot string See Itemslot enumeration
--- @field Durability integer
--- @field DurabilityDegradeSpeed string See Qualifier enumeration
--- @field Value integer
--- @field ModifierType string See ModifierType enumeration
--- @field Act string See Act enumeration
--- @field Fire integer
--- @field Air integer
--- @field Water integer
--- @field Earth integer
--- @field Poison integer
--- @field Piercing integer
--- @field Physical integer
--- @field StrengthBoost string See Penalty Qualifier enumeration
--- @field FinesseBoost string See Penalty Qualifier enumeration
--- @field IntelligenceBoost string See Penalty Qualifier enumeration
--- @field ConstitutionBoost string See Penalty Qualifier enumeration
--- @field MemoryBoost string See Penalty Qualifier enumeration
--- @field WitsBoost string See Penalty Qualifier enumeration
--- @field SingleHanded integer
--- @field TwoHanded integer
--- @field Ranged integer
--- @field DualWielding integer
--- @field RogueLore integer
--- @field WarriorLore integer
--- @field RangerLore integer
--- @field FireSpecialist integer
--- @field WaterSpecialist integer
--- @field AirSpecialist integer
--- @field EarthSpecialist integer
--- @field Sourcery integer
--- @field Necromancy integer
--- @field Polymorph integer
--- @field Summoning integer
--- @field PainReflection integer
--- @field Perseverance integer
--- @field Leadership integer
--- @field Telekinesis integer
--- @field Sneaking integer
--- @field Thievery integer
--- @field Loremaster integer
--- @field Repair integer
--- @field Barter integer
--- @field Persuasion integer
--- @field Luck integer
--- @field SightBoost string See Penalty Qualifier enumeration
--- @field HearingBoost string See Penalty Qualifier enumeration
--- @field VitalityBoost integer
--- @field MagicPointsBoost string See Penalty Qualifier enumeration
--- @field ChanceToHitBoost integer
--- @field APMaximum integer
--- @field APStart integer
--- @field APRecovery integer
--- @field AccuracyBoost integer
--- @field DodgeBoost integer
--- @field CriticalChance integer
--- @field ComboCategory string
--- @field Weight integer
--- @field InventoryTab string See InventoryTabs enumeration
--- @field Flags string See AttributeFlags enumeration
--- @field ArmorType string See ArmorType enumeration
--- @field Boosts string
--- @field Skills string
--- @field ItemColor string
--- @field Reflection string
--- @field ItemGroup string
--- @field ObjectCategory string
--- @field MinAmount integer
--- @field MaxAmount integer
--- @field Priority integer
--- @field Unique integer
--- @field MinLevel integer
--- @field MaxLevel integer
--- @field MaxSummons integer
--- @field NeedsIdentification string See YesNo enumeration
--- @field Charges integer
--- @field RuneSlots integer
--- @field RuneSlots_V1 integer
--- @field MaxCharges integer
--- @field Talents string
--- @field Tags string
--- @field ExtraProperties StatProperty[]
local StatEntryArmor = {
    ['Armor Defense Value'] = 0,
    ['Magic Armor Value'] = 0,
    --- @type string See ActPart enumeration
    ['Act part'] = ""
}


--- @class StatEntryShield
--- @field Name string
--- @field Level integer
--- @field Using string
--- @field ArmorBoost integer
--- @field MagicArmorBoost integer
--- @field Movement integer
--- @field Initiative integer
--- @field Requirements StatRequirement[]
--- @field Slot string See Itemslot enumeration
--- @field Durability integer
--- @field DurabilityDegradeSpeed string See Qualifier enumeration
--- @field Value integer
--- @field ModifierType string See ModifierType enumeration
--- @field Act string See Act enumeration
--- @field Fire integer
--- @field Air integer
--- @field Water integer
--- @field Earth integer
--- @field Poison integer
--- @field Piercing integer
--- @field Physical integer
--- @field Blocking integer
--- @field StrengthBoost string See Penalty Qualifier enumeration
--- @field FinesseBoost string See Penalty Qualifier enumeration
--- @field IntelligenceBoost string See Penalty Qualifier enumeration
--- @field ConstitutionBoost string See Penalty Qualifier enumeration
--- @field MemoryBoost string See Penalty Qualifier enumeration
--- @field WitsBoost string See Penalty Qualifier enumeration
--- @field SingleHanded integer
--- @field TwoHanded integer
--- @field Ranged integer
--- @field DualWielding integer
--- @field RogueLore integer
--- @field WarriorLore integer
--- @field RangerLore integer
--- @field FireSpecialist integer
--- @field WaterSpecialist integer
--- @field AirSpecialist integer
--- @field EarthSpecialist integer
--- @field Sourcery integer
--- @field Necromancy integer
--- @field Polymorph integer
--- @field Summoning integer
--- @field Leadership integer
--- @field PainReflection integer
--- @field Perseverance integer
--- @field Telekinesis integer
--- @field Sneaking integer
--- @field Thievery integer
--- @field Loremaster integer
--- @field Repair integer
--- @field Barter integer
--- @field Persuasion integer
--- @field Luck integer
--- @field SightBoost string See Penalty Qualifier enumeration
--- @field HearingBoost string See Penalty Qualifier enumeration
--- @field VitalityBoost integer
--- @field MagicPointsBoost string See Penalty Qualifier enumeration
--- @field ChanceToHitBoost integer
--- @field APMaximum integer
--- @field APStart integer
--- @field APRecovery integer
--- @field AccuracyBoost integer
--- @field DodgeBoost integer
--- @field CriticalChance string See Penalty Qualifier enumeration
--- @field ComboCategory string
--- @field Weight integer
--- @field InventoryTab string See InventoryTabs enumeration
--- @field Flags string See AttributeFlags enumeration
--- @field Skills string
--- @field Reflection string
--- @field ItemGroup string
--- @field ObjectCategory string
--- @field MinAmount integer
--- @field MaxAmount integer
--- @field Priority integer
--- @field Unique integer
--- @field MinLevel integer
--- @field MaxLevel integer
--- @field ItemColor string
--- @field MaxSummons integer
--- @field RuneSlots integer
--- @field RuneSlots_V1 integer
--- @field NeedsIdentification string See YesNo enumeration
--- @field Talents string
--- @field Tags string
--- @field ExtraProperties StatProperty[]
local StatEntryShield = {
    ['Armor Defense Value'] = 0,
    ['Magic Armor Value'] = 0,
    --- @type string See ActPart enumeration
    ['Act part'] = ""
}


--- @class StatEntryPotion
--- @field Name string
--- @field Level integer
--- @field Using string
--- @field ModifierType string See ModifierType enumeration
--- @field VitalityBoost integer
--- @field Strength string See Penalty PreciseQualifier enumeration
--- @field Finesse string See Penalty PreciseQualifier enumeration
--- @field Intelligence string See Penalty PreciseQualifier enumeration
--- @field Constitution string See Penalty PreciseQualifier enumeration
--- @field Memory string See Penalty PreciseQualifier enumeration
--- @field Wits string See Penalty PreciseQualifier enumeration
--- @field SingleHanded integer
--- @field TwoHanded integer
--- @field Ranged integer
--- @field DualWielding integer
--- @field RogueLore integer
--- @field WarriorLore integer
--- @field RangerLore integer
--- @field FireSpecialist integer
--- @field WaterSpecialist integer
--- @field AirSpecialist integer
--- @field EarthSpecialist integer
--- @field Sourcery integer
--- @field Necromancy integer
--- @field Polymorph integer
--- @field Summoning integer
--- @field PainReflection integer
--- @field Perseverance integer
--- @field Leadership integer
--- @field Telekinesis integer
--- @field Sneaking integer
--- @field Thievery integer
--- @field Loremaster integer
--- @field Repair integer
--- @field Barter integer
--- @field Persuasion integer
--- @field Luck integer
--- @field FireResistance integer
--- @field EarthResistance integer
--- @field WaterResistance integer
--- @field AirResistance integer
--- @field PoisonResistance integer
--- @field PhysicalResistance integer
--- @field PiercingResistance integer
--- @field Sight integer
--- @field Hearing string See Penalty Qualifier enumeration
--- @field Initiative integer
--- @field Vitality integer
--- @field VitalityPercentage integer
--- @field MagicPoints integer
--- @field ActionPoints integer
--- @field ChanceToHitBoost integer
--- @field AccuracyBoost integer
--- @field DodgeBoost integer
--- @field DamageBoost integer
--- @field APCostBoost integer
--- @field SPCostBoost integer
--- @field APMaximum integer
--- @field APStart integer
--- @field APRecovery integer
--- @field Movement integer
--- @field MovementSpeedBoost integer
--- @field Gain string See BigQualifier enumeration
--- @field Armor integer
--- @field MagicArmor integer
--- @field ArmorBoost integer
--- @field MagicArmorBoost integer
--- @field CriticalChance integer
--- @field Act string See Act enumeration
--- @field Duration integer
--- @field UseAPCost integer
--- @field ComboCategory string
--- @field StackId string
--- @field BoostConditions string
--- @field Flags string See AttributeFlags enumeration
--- @field StatusMaterial string
--- @field StatusEffect string
--- @field StatusIcon string
--- @field SavingThrow string See SavingThrow enumeration
--- @field Weight integer
--- @field Value integer
--- @field InventoryTab string See InventoryTabs enumeration
--- @field UnknownBeforeConsume string See YesNo enumeration
--- @field Reflection string
--- @field Damage string See Qualifier enumeration
--- @field DamageType string See Damage Type enumeration
--- @field AuraRadius integer
--- @field AuraSelf string
--- @field AuraAllies string
--- @field AuraEnemies string
--- @field AuraNeutrals string
--- @field AuraItems string
--- @field AuraFX string
--- @field RootTemplate string
--- @field ObjectCategory string
--- @field MinAmount integer
--- @field MaxAmount integer
--- @field Priority integer
--- @field Unique integer
--- @field MinLevel integer
--- @field MaxLevel integer
--- @field BloodSurfaceType string
--- @field MaxSummons integer
--- @field AddToBottomBar string See YesNo enumeration
--- @field SummonLifelinkModifier integer
--- @field IgnoredByAI string See YesNo enumeration
--- @field RangeBoost integer
--- @field BonusWeapon string
--- @field AiCalculationStatsOverride string
--- @field RuneEffectWeapon string
--- @field RuneEffectUpperbody string
--- @field RuneEffectAmulet string
--- @field RuneLevel integer
--- @field LifeSteal integer
--- @field IsFood string See YesNo enumeration
--- @field IsConsumable string See YesNo enumeration
--- @field ExtraProperties StatProperty[]
local StatEntryPotion = {
    ['Damage Multiplier'] = 0,
    ['Damage Range'] = 0,
    --- @type string See ActPart enumeration
    ['Act part'] = ""
}


--- @class StatEntryObject
--- @field Name string
--- @field Level integer
--- @field Using string
--- @field ModifierType string See ModifierType enumeration
--- @field Act string See Act enumeration
--- @field UseAPCost integer
--- @field Value integer
--- @field ComboCategory string
--- @field Weight integer
--- @field Strength string See PreciseQualifier enumeration
--- @field Finesse string See PreciseQualifier enumeration
--- @field Intelligence string See PreciseQualifier enumeration
--- @field Constitution string See PreciseQualifier enumeration
--- @field Memory string See PreciseQualifier enumeration
--- @field Wits string See PreciseQualifier enumeration
--- @field Vitality integer
--- @field Armor string See PreciseQualifier enumeration
--- @field FireResistance integer
--- @field EarthResistance integer
--- @field WaterResistance integer
--- @field AirResistance integer
--- @field PoisonResistance integer
--- @field PiercingResistance integer
--- @field PhysicalResistance integer
--- @field ShadowResistance integer
--- @field Flags string See AttributeFlags enumeration
--- @field Requirements StatRequirement[]
--- @field InventoryTab string See InventoryTabs enumeration
--- @field RootTemplate string
--- @field ObjectCategory string
--- @field MinAmount integer
--- @field MaxAmount integer
--- @field Priority integer
--- @field Unique integer
--- @field MinLevel integer
--- @field RuneEffectWeapon string
--- @field RuneEffectUpperbody string
--- @field RuneEffectAmulet string
--- @field RuneLevel integer
--- @field MaxLevel integer
--- @field AddToBottomBar string See YesNo enumeration
--- @field IgnoredByAI string See YesNo enumeration
local StatEntryObject = {
    --- @type string See ActPart enumeration
    ['Act part'] = ""
}


--- @class StatEntryCharacter
--- @field Name string
--- @field Level integer
--- @field Using string
--- @field Strength string See PreciseQualifier enumeration
--- @field Finesse string See PreciseQualifier enumeration
--- @field Intelligence string See PreciseQualifier enumeration
--- @field Constitution string See PreciseQualifier enumeration
--- @field Memory string See PreciseQualifier enumeration
--- @field Wits string See PreciseQualifier enumeration
--- @field SingleHanded integer
--- @field TwoHanded integer
--- @field Ranged integer
--- @field DualWielding integer
--- @field RogueLore integer
--- @field WarriorLore integer
--- @field RangerLore integer
--- @field FireSpecialist integer
--- @field WaterSpecialist integer
--- @field AirSpecialist integer
--- @field EarthSpecialist integer
--- @field Sourcery integer
--- @field Necromancy integer
--- @field Polymorph integer
--- @field Summoning integer
--- @field PainReflection integer
--- @field Leadership integer
--- @field Perseverance integer
--- @field Telekinesis integer
--- @field Sneaking integer
--- @field Thievery integer
--- @field Loremaster integer
--- @field Repair integer
--- @field Barter integer
--- @field Persuasion integer
--- @field Luck integer
--- @field FireResistance integer
--- @field EarthResistance integer
--- @field WaterResistance integer
--- @field AirResistance integer
--- @field PoisonResistance integer
--- @field PiercingResistance integer
--- @field PhysicalResistance integer
--- @field Sight string See Penalty Qualifier enumeration
--- @field Hearing string See Penalty Qualifier enumeration
--- @field FOV integer
--- @field APMaximum integer
--- @field APStart integer
--- @field APRecovery integer
--- @field Initiative integer
--- @field Vitality integer
--- @field MagicPoints integer
--- @field ChanceToHitBoost integer
--- @field Movement integer
--- @field MovementSpeedBoost integer
--- @field CriticalChance string See Penalty Qualifier enumeration
--- @field Gain string See BigQualifier enumeration
--- @field Armor integer
--- @field ArmorBoost integer
--- @field ArmorBoostGrowthPerLevel integer
--- @field MagicArmor integer
--- @field MagicArmorBoost integer
--- @field MagicArmorBoostGrowthPerLevel integer
--- @field Accuracy integer
--- @field Dodge integer
--- @field Act string See Act enumeration
--- @field MaxResistance integer
--- @field Weight integer
--- @field Talents string
--- @field Traits string
--- @field PathInfluence string
--- @field Flags string See AttributeFlags enumeration
--- @field Reflection string
--- @field StepsType string See StepsType enumeration
--- @field MaxSummons integer
--- @field MPStart integer
--- @field DamageBoost integer
--- @field DamageBoostGrowthPerLevel integer
local StatEntryCharacter = {
    --- @type string See ActPart enumeration
    ['Act part'] = "",
    --- @type string See Penalty Qualifier enumeration
    ['Act strength'] = ""
}


--- @class StatEntrySkillData
--- @field Name string
--- @field Level integer
--- @field Using string
--- @field SkillType string
--- @field Ability string See SkillAbility enumeration
--- @field Element string See SkillElement enumeration
--- @field Requirement string See SkillRequirement enumeration
--- @field Requirements StatRequirement[]
--- @field DisplayName string
--- @field DisplayNameRef string
--- @field Description string
--- @field DescriptionRef string
--- @field StatsDescription string
--- @field StatsDescriptionRef string
--- @field StatsDescriptionParams string
--- @field Icon string
--- @field FXScale integer
--- @field PrepareAnimationInit string
--- @field PrepareAnimationLoop string
--- @field PrepareEffect string
--- @field PrepareEffectBone string
--- @field CastAnimation string
--- @field CastTextEvent string
--- @field CastAnimationCheck string See CastCheckType enumeration
--- @field CastEffect string
--- @field CastEffectTextEvent string
--- @field TargetCastEffect string
--- @field TargetHitEffect string
--- @field TargetEffect string
--- @field SourceTargetEffect string
--- @field TargetTargetEffect string
--- @field LandingEffect string
--- @field ImpactEffect string
--- @field MaleImpactEffects string
--- @field FemaleImpactEffects string
--- @field OnHitEffect string
--- @field SelectedCharacterEffect string
--- @field SelectedObjectEffect string
--- @field SelectedPositionEffect string
--- @field DisappearEffect string
--- @field ReappearEffect string
--- @field ReappearEffectTextEvent string
--- @field RainEffect string
--- @field StormEffect string
--- @field FlyEffect string
--- @field SpatterEffect string
--- @field ShieldMaterial string
--- @field ShieldEffect string
--- @field ContinueEffect string
--- @field SkillEffect string
--- @field Template string
--- @field TemplateCheck string See CastCheckType enumeration
--- @field TemplateOverride string
--- @field TemplateAdvanced string
--- @field Totem string See YesNo enumeration
--- @field Template1 string
--- @field Template2 string
--- @field Template3 string
--- @field WeaponBones string
--- @field TeleportSelf string See YesNo enumeration
--- @field CanTargetCharacters string See YesNo enumeration
--- @field CanTargetItems string See YesNo enumeration
--- @field CanTargetTerrain string See YesNo enumeration
--- @field ForceTarget string See YesNo enumeration
--- @field TargetProjectiles string See YesNo enumeration
--- @field UseCharacterStats string See YesNo enumeration
--- @field UseWeaponDamage string See YesNo enumeration
--- @field UseWeaponProperties string See YesNo enumeration
--- @field SingleSource string See YesNo enumeration
--- @field ContinueOnKill string See YesNo enumeration
--- @field Autocast string See YesNo enumeration
--- @field AmountOfTargets integer
--- @field AutoAim string See YesNo enumeration
--- @field AddWeaponRange string See YesNo enumeration
--- @field ActionPoints integer
--- @field Cooldown integer
--- @field CooldownReduction integer
--- @field ChargeDuration integer
--- @field CastDelay integer
--- @field Offset integer
--- @field Lifetime integer
--- @field Duration string See Qualifier enumeration
--- @field TargetRadius integer
--- @field ExplodeRadius integer
--- @field AreaRadius integer
--- @field HitRadius integer
--- @field RadiusMax integer
--- @field Range integer
--- @field MaxDistance integer
--- @field Angle integer
--- @field TravelSpeed integer
--- @field Acceleration integer
--- @field Height integer
--- @field Damage string See DamageSourceType enumeration
--- @field DamageType string See Damage Type enumeration
--- @field DamageMultiplier string See PreciseQualifier enumeration
--- @field DeathType string See Death Type enumeration
--- @field BonusDamage string See Qualifier enumeration
--- @field HitPointsPercent integer
--- @field MinHitsPerTurn integer
--- @field MaxHitsPerTurn integer
--- @field HitDelay integer
--- @field MaxAttacks integer
--- @field NextAttackChance integer
--- @field NextAttackChanceDivider integer
--- @field EndPosRadius integer
--- @field JumpDelay integer
--- @field TeleportDelay integer
--- @field PointsMaxOffset integer
--- @field RandomPoints integer
--- @field ChanceToPierce integer
--- @field MaxPierceCount integer
--- @field MaxForkCount integer
--- @field ForkLevels integer
--- @field ForkChance integer
--- @field HealAmount string See PreciseQualifier enumeration
--- @field StatusClearChance integer
--- @field SurfaceType string See Surface Type enumeration
--- @field SurfaceLifetime integer
--- @field SurfaceStatusChance integer
--- @field SurfaceTileCollision string See SurfaceCollisionFlags enumeration
--- @field SurfaceGrowInterval integer
--- @field SurfaceGrowStep integer
--- @field SurfaceRadius integer
--- @field TotalSurfaceCells integer
--- @field SurfaceMinSpawnRadius integer
--- @field MinSurfaces integer
--- @field MaxSurfaces integer
--- @field MinSurfaceSize integer
--- @field MaxSurfaceSize integer
--- @field GrowSpeed integer
--- @field GrowOnSurface string See SurfaceCollisionFlags enumeration
--- @field GrowTimeout integer
--- @field SkillBoost string
--- @field SkillAttributeFlags string See AttributeFlags enumeration
--- @field SkillProperties StatProperty[]
--- @field CleanseStatuses string
--- @field AoEConditions string See Conditions enumeration
--- @field TargetConditions string See Conditions enumeration
--- @field ForkingConditions string See Conditions enumeration
--- @field CycleConditions string See Conditions enumeration
--- @field ShockWaveDuration integer
--- @field TeleportTextEvent string
--- @field SummonEffect string
--- @field ProjectileCount integer
--- @field ProjectileDelay integer
--- @field StrikeCount integer
--- @field StrikeDelay integer
--- @field PreviewStrikeHits string See YesNo enumeration
--- @field SummonLevel integer
--- @field StartTextEvent string
--- @field StopTextEvent string
--- @field Atmosphere string See AtmosphereType enumeration
--- @field ConsequencesStartTime integer
--- @field ConsequencesDuration integer
--- @field HealthBarColor integer
--- @field Skillbook string
--- @field PreviewImpactEffect string
--- @field IgnoreVisionBlock string See YesNo enumeration
--- @field HealEffectId string
--- @field AddRangeFromAbility string See Ability enumeration
--- @field DivideDamage string See YesNo enumeration
--- @field OverrideMinAP string See YesNo enumeration
--- @field OverrideSkillLevel string See YesNo enumeration
--- @field Tier string See SkillTier enumeration
--- @field GrenadeBone string
--- @field GrenadeProjectile string
--- @field GrenadePath string
--- @field MovingObject string
--- @field SpawnObject string
--- @field SpawnEffect string
--- @field SpawnFXOverridesImpactFX string See YesNo enumeration
--- @field SpawnLifetime integer
--- @field ProjectileTerrainOffset string See YesNo enumeration
--- @field ProjectileType string See ProjectileType enumeration
--- @field HitEffect string
--- @field PushDistance integer
--- @field ForceMove string See YesNo enumeration
--- @field Stealth string See YesNo enumeration
--- @field Distribution string See ProjectileDistribution enumeration
--- @field Shuffle string See YesNo enumeration
--- @field PushPullEffect string
--- @field BackStart integer
--- @field FrontOffset integer
--- @field TargetGroundEffect string
--- @field PositionEffect string
--- @field BeamEffect string
--- @field PreviewEffect string
--- @field CastSelfAnimation string
--- @field IgnoreCursed string See YesNo enumeration
--- @field IsEnemySkill string See YesNo enumeration
--- @field DomeEffect string
--- @field AuraSelf string
--- @field AuraAllies string
--- @field AuraEnemies string
--- @field AuraNeutrals string
--- @field AuraItems string
--- @field AIFlags string See AIFlags enumeration
--- @field Shape string
--- @field Base integer
--- @field AiCalculationSkillOverride string
--- @field TeleportSurface string See YesNo enumeration
--- @field ProjectileSkills string
--- @field SummonCount integer
--- @field LinkTeleports string See YesNo enumeration
--- @field TeleportsUseCount integer
--- @field HeightOffset integer
--- @field ForGameMaster string See YesNo enumeration
--- @field IsMelee string See YesNo enumeration
--- @field MemorizationRequirements StatRequirement[]
--- @field IgnoreSilence string See YesNo enumeration
--- @field IgnoreHeight string See YesNo enumeration
local StatEntrySkillData = {
    ['Memory Cost'] = 0,
    ['Magic Cost'] = 0,
    ['Damage Multiplier'] = 0,
    ['Damage Range'] = 0,
    ['Chance To Hit Multiplier'] = 0,
    ['Damage On Jump'] = 0,
    ['Damage On Landing'] = 0,
    ['Healing Multiplier'] = 0,
    ['Stealth Damage Multiplier'] = 0,
    ['Distance Damage Multiplier'] = 0
}


--- @class StatEntryStatusData
--- @field Name string
--- @field Level integer
--- @field Using string
--- @field StatusType string
--- @field Icon string
--- @field DisplayName string
--- @field DisplayNameRef string
--- @field Description string
--- @field DescriptionRef string
--- @field DescriptionParams string
--- @field OverrideDefaultDescription string See YesNo enumeration
--- @field FormatColor string See FormatStringColor enumeration
--- @field SavingThrow string See SavingThrow enumeration
--- @field IsChanneled string See YesNo enumeration
--- @field Instant string See YesNo enumeration
--- @field StatusEffect string
--- @field StatusEffectOverrideForItems string
--- @field StatusEffectOnTurn string
--- @field MaterialType string See MaterialType enumeration
--- @field Material string
--- @field MaterialApplyBody string See YesNo enumeration
--- @field MaterialApplyArmor string See YesNo enumeration
--- @field MaterialApplyWeapon string See YesNo enumeration
--- @field MaterialApplyNormalMap string See YesNo enumeration
--- @field MaterialFadeAmount integer
--- @field MaterialOverlayOffset integer
--- @field MaterialParameters string
--- @field HealingEvent string See StatusEvent enumeration
--- @field HealStat string See StatusHealType enumeration
--- @field HealType string See HealValueType enumeration
--- @field HealValue integer
--- @field StatsId string
--- @field IsInvulnerable string See YesNo enumeration
--- @field IsDisarmed string See YesNo enumeration
--- @field StackId string
--- @field StackPriority integer
--- @field AuraRadius integer
--- @field AuraSelf string
--- @field AuraAllies string
--- @field AuraEnemies string
--- @field AuraNeutrals string
--- @field AuraItems string
--- @field AuraFX string
--- @field ImmuneFlag string See AttributeFlag enumeration
--- @field CleanseStatuses string
--- @field MaxCleanseCount integer
--- @field ApplyAfterCleanse string See YesNo enumeration
--- @field SoundStart string
--- @field SoundLoop string
--- @field SoundStop string
--- @field DamageEvent string See StatusEvent enumeration
--- @field DamageStats string
--- @field DeathType string See Death Type enumeration
--- @field DamageCharacters string See YesNo enumeration
--- @field DamageItems string See YesNo enumeration
--- @field DamageTorches string See YesNo enumeration
--- @field FreezeTime integer
--- @field SurfaceChange string
--- @field PermanentOnTorch string See YesNo enumeration
--- @field AbsorbSurfaceType string
--- @field AbsorbSurfaceRange integer
--- @field Skills string
--- @field BonusFromAbility string See Ability enumeration
--- @field Items string
--- @field OnlyWhileMoving string See YesNo enumeration
--- @field DescriptionCaster string
--- @field DescriptionTarget string
--- @field WinBoost string See Properties enumeration
--- @field LoseBoost string See Properties enumeration
--- @field WeaponOverride string
--- @field ApplyEffect string
--- @field ForGameMaster string See YesNo enumeration
--- @field ResetCooldowns string
--- @field ResetOncePerCombat string See YesNo enumeration
--- @field PolymorphResult string
--- @field DisableInteractions string See YesNo enumeration
--- @field LoseControl string See YesNo enumeration
--- @field AiCalculationSkillOverride string
--- @field HealEffectId string
--- @field ScaleWithVitality string See YesNo enumeration
--- @field VampirismType string See VampirismType enumeration
--- @field BeamEffect string
--- @field HealMultiplier integer
--- @field InitiateCombat string See YesNo enumeration
--- @field Projectile string
--- @field Radius integer
--- @field Charges integer
--- @field MaxCharges integer
--- @field DefendTargetPosition string See YesNo enumeration
--- @field TargetConditions string See Conditions enumeration
--- @field Toggle string See YesNo enumeration
--- @field LeaveAction string
--- @field DieAction string
--- @field PlayerSameParty string See YesNo enumeration
--- @field PlayerHasTag string
--- @field PeaceOnly string See YesNo enumeration
--- @field Necromantic string See YesNo enumeration
--- @field RetainSkills string
--- @field BringIntoCombat string See YesNo enumeration
--- @field ApplyStatusOnTick string
--- @field IsResistingDeath string See YesNo enumeration
--- @field TargetEffect string
--- @field DamagePercentage integer
--- @field ForceOverhead string See YesNo enumeration
--- @field TickSFX string
--- @field ForceStackOverwrite string See YesNo enumeration
--- @field FreezeCooldowns string See YesNo enumeration
local StatEntryStatusData = {}


--- @class StatEntryCrime
--- @field Name string
--- @field Level integer
--- @field Using string
--- @field CrimeType string
--- @field TensionWeight integer
--- @field Lifetime integer
--- @field Audible string See YesNo enumeration
--- @field DetectionRange integer
--- @field CreatesCrimescene string See YesNo enumeration
--- @field Continuous string See YesNo enumeration
--- @field VictimReaction string
--- @field NoticedAD string
--- @field WarningDialog string
--- @field ArrestDialog string
--- @field InterrogateDialog string
--- @field ReactionCategory_Tag string
--- @field ReactionCategory_Priority integer
--- @field ReactionCategory_PrimaryReaction string
--- @field ReactionCategory_SecondaryReaction string
--- @field MaxInvestigators integer
--- @field ContinuousDelayTimer integer
--- @field SoundEvent string
--- @field AudibleRange integer
--- @field VictimDialog string
--- @field CanRaiseAlarm string See YesNo enumeration
--- @field CanMerge string See YesNo enumeration
local StatEntryCrime = {}


--- @class HitRequest
--- @field Equipment integer
--- @field TotalDamageDone integer
--- @field DamageDealt integer
--- @field DeathType string See DeathType enumeration
--- @field DamageType string See DamageType enumeration
--- @field AttackDirection integer
--- @field ArmorAbsorption integer
--- @field LifeSteal integer
--- @field EffectFlags integer See HitFlags enumeration
--- @field HitWithWeapon boolean
--- @field DamageList DamageList
local HitRequest = {}


--- @class HitContext
--- Context information passed in StatusHitEnter/BeforeCharacterApplyDamage callbacks
--- @field HitId integer Unique hit identifier for hit tracking
--- @field Weapon StatItem
--- @field Hit HitRequest
--- @field HitType string See HitType enumeration
--- @field NoHitRoll boolean
--- @field ProcWindWalker boolean
--- @field ForceReduceDurability boolean
--- @field HighGround string
--- @field CriticalRoll string
--- @field HitStatus EsvStatusHit
local HitContext = {}


--- @class EsvCombatTeam
--- @field TeamId integer A number identifying the team instance
--- @field CombatId integer 	Identifies which combat the team is a participant of
--- @field Initiative integer Computed initiative value of the team
--- @field StillInCombat boolean Can the team currently fight, or is it temporarily out of combat?
--- @field Character EsvCharacter|nil Character object if the team is a character; nil otherwise
--- @field Item EsvItem|nil Item object if the team is an item; nil otherwise
local EsvCombatTeam = {}


--- @class EsvCombat
--- @field CombatId integer A number identifying the combat instance
--- @field LevelName string Level where the combat is taking place
--- @field IsActive boolean
local EsvCombat = {}

--- Retrieves the turn order of the current round.
--- @return EsvCombatTeam[]
function EsvCombat:GetCurrentTurnOrder() end
    
--- Retrieves the turn order of the next round.
--- @return EsvCombatTeam[]
function EsvCombat:GetNextTurnOrder() end

--- Updates the turn order of the current round. 
--- The turnOrder argument should be a reordered version of the table returned by GetCurrentTurnOrder().
--- Notes:
---  It is possible to remove or add characters to the current turn by adding/removing their Team object from the table.
---  It is possible to add a character to the current turn more than once, the character will only appear once in the UI however.
---  The character whose turn is currently active (the very first item) should not be removed or reordered. 
---    This only applies for GetCurrentTurnOrder, the first item can be freely reordered in GetNextTurnOrder.
---  Changed performed using this function are synchronized to the client at the end of the current server tick.
--- @param turnOrder EsvCombatTeam[]
function EsvCombat:UpdateCurrentTurnOrder(turnOrder) end
    
--- Updates the turn order of the next round. 
--- The turnOrder argument should be a reordered version of the table returned by GetNextTurnOrder().
--- Notes:
---  It is possible to remove or add characters to the next turn by adding/removing their Team object from the table.
---  It is possible to add a character to the next turn more than once, the character will only appear once in the UI however.
---  Changed performed using this function are synchronized to the client at the end of the current server tick.
--- @param turnOrder EsvCombatTeam[]
function EsvCombat:UpdateNextTurnOrder(turnOrder) end
    
--- Retrieves all participants of the combat
--- @return EsvCombatTeam[]
function EsvCombat:GetAllTeams() end


--- @class FlashObject
--- Represents an object in Flash.
--- Implements the __index and __newindex metamethods using string keys (i.e. allows table-like behavior):
--- obj.field = 123 -- Can assign values to any object property
--- Ext.Print(obj.field) -- Can read any object property
---
--- Field values are returned using the appropriate Lua type;
--- Flash Boolean/Number/String = Lua boolean/number/string
--- Flash Object = Lua engine class FlashObject
--- Flash array = Lua engine class FlashArray
--- Flash function = Lua engine class FlashFunction
local FlashObject = {}

--- @class FlashArray
--- Represents an array in Flash.
--- Implements the __index, __newindex and __len metamethods using integer keys (i.e. allows table-like behavior):
--- arr[2] = 123 -- Can assign values to any array index
--- Ext.Print(arr[2]) -- Can read any array index
--- Ext.Print(#arr) -- Can query length of array
local FlashArray = {}


--- @class FlashFunction
--- Represents a function or method in Flash.
--- Implements the __call metamethod (i.e. can be called).
--- The passed arguments are forwarded to the Flash method and the return value of the Flash method is returned.
local FlashFunction = {}

---@class FlashEventDispatcher:FlashObject

---Currently unsupported type 12.
---@class FlashDisplayObject:FlashEventDispatcher

---@class FlashInteractiveObject:FlashDisplayObject
---@field doubleClickEnabled boolean Specifies whether the object receives doubleClick events.
---@field mouseEnabled boolean Specifies whether this object receives mouse, or other user input, messages.
---@field tabEnabled boolean Specifies whether this object is in the tab order.
---@field tabIndex integer Specifies the tab ordering of objects in a SWF file.


---@class FlashGraphics:FlashObject
---@field beginBitmapFill fun(bitmap:BitmapData, matrix:FlashMatrix, repeat:boolean, smooth:boolean):void Fills a drawing area with a bitmap image.
---@field beginFill fun(color:integer, alpha:number):void Specifies a simple one-color fill that subsequent calls to other Graphics methods  fun(such as lineTo fun() or drawCircle fun()) use when drawing.
---@field beginGradientFill fun(type:string, colors:FlashArray, alphas:FlashArray, ratios:FlashArray, matrix:FlashMatrix, spreadMethod:string, interpolationMethod:string, focalPointRatio:number):void Specifies a gradient fill used by subsequent calls to other Graphics methods  fun(such as lineTo fun() or drawCircle fun()) for the object.
---@field clear fun():void Clears the graphics that were drawn to this Graphics object, and resets fill and line style settings.
---@field curveTo fun(controlX:number, controlY:number, anchorX:number, anchorY:number):void Draws a quadratic Bezier curve using the current line style from the current drawing position to  fun(anchorX, anchorY) and using the control point that  fun(controlX, controlY) specifies.
---@field drawCircle fun(x:number, y:number, radius:number):void Draws a circle.
---@field drawEllipse fun(x:number, y:number, width:number, height:number):void Draws an ellipse.
---@field drawPath fun(commands:Vector.<int>, data:FlashVector, winding:string):void Submits a series of commands for drawing.
---@field drawRect fun(x:number, y:number, width:number, height:number):void Draws a rectangle.
---@field drawRoundRect fun(x:number, y:number, width:number, height:number, ellipseWidth:number, ellipseHeight:number):void Draws a rounded rectangle.
---@field endFill fun():void Applies a fill to the lines and curves that were added since the last call to the beginFill fun(), beginGradientFill fun(), or beginBitmapFill fun() method.
---@field lineGradientStyle fun(type:string, colors:FlashArray, alphas:FlashArray, ratios:FlashArray, matrix:FlashMatrix, spreadMethod:string, interpolationMethod:string, focalPointRatio:number):void Specifies a gradient to use for the stroke when drawing lines.
---@field lineStyle fun(thickness:number, color:integer, alpha:number, pixelHinting:boolean, scaleMode:string, caps:string, joints:string, miterLimit:number):void Specifies a line style used for subsequent calls to Graphics methods such as the lineTo fun() method or the drawCircle fun() method.
---@field lineTo fun(x:number, y:number):void Draws a line using the current line style from the current drawing position to  fun(x, y); the current drawing position is then set to  fun(x, y).
---@field moveTo fun(x:number, y:number):void Moves the current drawing position to  fun(x, y).

---@class FlashDisplayObjectContainer:FlashInteractiveObject
---@field mouseChildren boolean Determines whether or not the children of the object are mouse, or user input device, enabled.
---@field numChildren integer Returns the number of children of this object. [read-only]
---@field tabChildren boolean Determines whether the children of the object are tab enabled.
---@field textSnapshot FlashTextSnapshot Returns a TextSnapshot object for this DisplayObjectContainer instance. [read-only]
---@field addChild fun(child:FlashDisplayObject):FlashDisplayObject Adds a child DisplayObject instance to this DisplayObjectContainer instance.
---@field addChildAt fun(child:FlashDisplayObject, index:integer):FlashDisplayObject Adds a child DisplayObject instance to this DisplayObjectContainer instance.
---@field areInaccessibleObjectsUnderPoint fun(point:Point):boolean Indicates whether the security restrictions would cause any display objects to be omitted from the list returned by calling the DisplayObjectContainer.getObjectsUnderPoint() method with the specified point point.
---@field contains fun(child:FlashDisplayObject):boolean Determines whether the specified display object is a child of the DisplayObjectContainer instance or the instance itself.
---@field getChildAt fun(index:integer):FlashDisplayObject Returns the child display object instance that exists at the specified index.
---@field getChildByName fun(name:String):FlashDisplayObject Returns the child display object that exists with the specified name.
---@field getChildIndex fun(child:FlashDisplayObject):integer Returns the index position of a child DisplayObject instance.
---@field getObjectsUnderPoint fun(point:Point):Array Returns an array of objects that lie under the specified point and are children (or grandchildren, and so on) of this DisplayObjectContainer instance.
---@field removeChild fun(child:FlashDisplayObject):FlashDisplayObject Removes the specified child DisplayObject instance from the child list of the DisplayObjectContainer instance.
---@field removeChildAt fun(index:integer):FlashDisplayObject Removes a child DisplayObject from the specified index position in the child list of the DisplayObjectContainer.
---@field removeChildren fun(beginIndex:integer, endIndex:integer):void Removes all child DisplayObject instances from the child list of the DisplayObjectContainer instance.
---@field setChildIndex fun(child:FlashDisplayObject, index:integer):void Changes the position of an existing child in the display object container.
---@field swapChildren fun(child1:FlashDisplayObject, child2:FlashDisplayObject):void Swaps the z-order (front-to-back order) of the two specified child objects.
---@field swapChildrenAt fun(index1:integer, index2:integer):void Swaps the z-order (front-to-back order) of the child objects at the two specified index positions in the child list.

---@class FlashSprite:FlashDisplayObjectContainer
---@field buttonMode boolean Specifies the button mode of this sprite.
---@field graphics FlashGraphics Specifies the Graphics object that belongs to this sprite where vector drawing commands can occur. [read-only]
---@field soundTransform number Controls sound within this sprite.
---@field useHandCursor boolean A value that indicates whether the pointing hand (hand cursor) appears when the pointer rolls over a sprite in which the buttonMode property is set to true.


---@class FlashMovieClip:FlashSprite
---@field currentFrame integer Specifies the number of the frame in which the playhead is located in the timeline of the MovieClip instance. [read-only]
---@field currentFrameLabel string The label at the current frame in the timeline of the MovieClip instance. [read-only]
---@field currentLabel string The current label in which the playhead is located in the timeline of the MovieClip instance. [read-only]
---@field currentLabels string[] Returns an array of FrameLabel objects from the current scene. [read-only]
---@field currentScene FlashObject The current scene in which the playhead is located in the timeline of the MovieClip instance. [read-only]
---@field scenes FlashArray[] An array of Scene objects, each listing the name, the number of frames, and the frame labels for a scene in the MovieClip instance. [read-only]
---@field enabled boolean A Boolean value that indicates whether a movie clip is enabled.
---@field framesLoaded integer The number of frames that are loaded from a streaming SWF file. [read-only]
---@field isPlaying boolean A Boolean value that indicates whether a movie clip is curently playing. [read-only]
---@field totalFrames integer The total number of frames in the MovieClip instance. [read-only]
---@field trackAsMenu boolean Indicates whether other display objects that are SimpleButton or MovieClip objects can receive mouse release events or other user input release events.
---@field gotoAndPlay fun(frame:FlashObject, scene:string):void Starts playing the SWF file at the specified frame.
---@field gotoAndStop fun(frame:FlashObject, scene:string):void Brings the playhead to the specified frame of the movie clip and stops it there.
---@field nextFrame fun():void Sends the playhead to the next frame and stops it.
---@field nextScene fun():void Moves the playhead to the next scene of the MovieClip instance.
---@field play fun():void Moves the playhead in the timeline of the movie clip.
---@field prevFrame fun():void Sends the playhead to the previous frame and stops it.
---@field prevScene fun():void Moves the playhead to the previous scene of the MovieClip instance.
---@field stop fun():void Stops the playhead in the movie clip.


---@class FlashMainTimeline:FlashMovieClip
---@field events string[] An array of input keys this UI should listen for, in the form of 'IE Name', such as 'IE UICreationTabPrev'. The engine will invoke onEventDown/onEventUp when these keys are pressed, if they haven't been handled.
---@field onEventDown fun(id:number):boolean Invoked by the engine when a valid input key in this.events is pressed. If true is returned, the key is "handled" and won't send events to other UI objects.
---@field onEventUp fun(id:number):boolean Invoked by the engine when a valid input key in this.events is released. If true is returned, the key is "handled" and won't send events to other UI objects.
---@field onEventResolution fun(width:number, height:number):void Invoked by the engine when the screen is resized.
---@field onEventInit fun():void Invoked by the engine. Typically used to register the anchor id and layout with ExternalInterface.call.


--- @class UIObject
local UIObject = {}

--- @param x integer
--- @param y integer
function UIObject:SetPosition(x, y) end
    
--- @param width integer
--- @param height integer
function UIObject:Resize(width, height) end
    
function UIObject:Show() end
    
function UIObject:Hide() end
    
--- Calls a method on the main timeline object of the UI element. 
--- The first argument(func) is the name of the ActionScript function to call; 
--- all subsequent arguments are passed to the ActionScript function as parameters. 
--- Only string, number and boolean arguments are supported.
--- @param method string Method to call
--- @vararg any
function UIObject:Invoke(method, ...) end
    
--- Simulates an ExternalInterface.call(...) call from Flash, 
--- i.e. it calls an UI handler function in the game engine. 
--- The first argument(func) is the name of the UI function to call; 
--- all subsequent arguments are passed to the engine as parameters. 
--- Only string, number and boolean arguments are supported.
--- @param func string ExternalInterface function to call
--- @vararg any
function UIObject:ExternalInterfaceCall(func, ...) end
    
--- @param frame integer
function UIObject:GotoFrame(frame) end
    
--- Updates the specified public property of the main timeline object.
--- @param property string Property path to set
--- @param value number|boolean|string Value
--- @param arrayIndex integer|nil Pass integer to set property[arrayIndex]
function UIObject:SetValue(property, value, arrayIndex) end
    
--- Retrieves the specified public property of the main timeline object. 
--- "type" contains the type of value to retrieve and must be string, number or boolean.
--- @param property string Property path to set
--- @param type "'number'" | "'boolean'" | "'string'" | nil
--- @param arrayIndex integer|nil Pass integer to set property[arrayIndex]
--- @return number|boolean|string
function UIObject:GetValue(property, type, arrayIndex) end
    
--- Returns the root(MainTimeline) Flash object
--- @return FlashMainTimeline
function UIObject:GetRoot() end
    
--- Returns the engine UI type ID of the UI element
--- @return integer
function UIObject:GetTypeId() end
    
--- @return ObjectHandle
function UIObject:GetHandle() end
    
--- Returns the handle of the player that this UI element is assigned to.
---(Only certain elements have a player assigned, like character sheet, inventory, etc.)
--- @return ObjectHandle|nil
function UIObject:GetPlayerHandle() end
    
function UIObject:Destroy() end

--- @param name string The draw call name for the movieclip. For instance, if the movieclip is iggy_luck, the name would be luck.
--- @param icon string The icon atlas entry to use, such as statIcons_Lucky.
--- @param width number The movieclip width.
--- @param height number The movieclip height.
function UIObject:SetCustomIcon(name, icon, width, height) end

--- Removes the Flash custom draw element override applied by SetCustomIcon().
--- @param element string Name of custom draw object
function UIObject:ClearCustomIcon(element) end

--- @class EclPickingState
--- @field WorldPosition number[] Position of cursor in world coordinates
--- @field WalkablePosition number[] Closest walkable position to cursor in world coordinates
--- @field HoverCharacter ObjectHandle Character hovered by cursor
--- @field HoverCharacter2 ObjectHandle Character hovered by cursor (???)
--- @field HoverCharacterPosition number[] Position of character hovered by cursor
--- @field HoverItem ObjectHandle Item hovered by cursor
--- @field HoverItemPosition number[] Position of item hovered by cursor
--- @field PlaceableEntity ObjectHandle Placeable game object hovered by cursor
--- @field PlaceablePosition number[] Position of placeable game object hovered by cursor


--- @class SurfaceInteractionSet
--- @field TransformType string Surface transform to apply (Bless, Curse, Ignite, ...)
--- @field ActionableSurfaces string[][] Surface types that this transform applies to


--- @alias ExtEngineEvent "'SessionLoading'" | "'SessionLoaded'" | "'ModuleLoading'" | "'ModuleLoadStarted'" | "'ModuleResume'" | "'GameStateChanged'" | "'SkillGetDescriptionParam'" | "'StatusGetDescriptionParam'" | "'GetSkillDamage'" | "'GetSkillAPCost'" | "'ComputeCharacterHit'" | "'CalculateTurnOrder'" | "'GetHitChance'" | "'StatusGetEnterChance'" | '"StatusHitEnter"' | "'BeforeCharacterApplyDamage'" | "'UIInvoke'" | "'UICall'" | "'AfterUIInvoke'" | "'AfterUICall'" | "'BeforeShootProjectile'" | "'ShootProjectile'" | "'ProjectileHit'" | "'GroundHit'" | "'InputEvent'"

--- @alias ExtGameStateChangedCallback fun(fromState: string, toState: string)
--- @alias ExtComputeCharacterHitCallback fun(target: StatCharacter, attacker: StatCharacter, weapon: StatItem, damageList: DamageList, hitType: string, noHitRoll: boolean, forceReduceDurability: boolean, hit: HitRequest, alwaysBackstab: boolean, highGroundFlag: string, criticalRoll: string): HitRequest
--- @alias ExtGetSkillDamageCallback fun(skill: StatEntrySkillData, attacker: StatCharacter, isFromItem: boolean, stealthed: boolean, attackerPos: number[], targetPos: number[], level: number, noRandomization: boolean): DamageList, string
--- @alias ExtCalculateTurnOrderCallback fun(combat: EsvCombat, order: EsvCombatTeam[]): EsvCombatTeam[]
--- @alias ExtGetHitChanceCallback fun(attacker: StatCharacter, target: StatCharacter): number
--- @alias ExtStatusGetEnterChanceCallback fun(status: EsvStatus, isEnterCheck: boolean): number
--- @alias ExtSkillGetDescriptionParamCallback fun(skill: StatEntrySkillData, character: StatCharacter, isFromItem: boolean, ...): string
--- @alias ExtStatusGetDescriptionParamCallback fun(status: EsvStatus, statusSource: EsvGameObject, character: StatCharacter, ...): string
--- @alias ExtGetSkillAPCostCallback fun(skill: StatEntrySkillData, character: StatCharacter, grid: AiGrid, position: number[]|nil, range: number|nil): number, boolean
--- @alias ExtBeforeCharacterApplyDamageCallback fun(target: EsvCharacter, attacker: StatCharacter|StatItem, hit: HitRequest, causeType: string, impactDirection: number[], context: HitContext): HitRequest
--- @alias ExtStatusHitEnterCallback fun(status: EsvStatus, context: HitContext)
--- @alias ExtBeforeShootProjectileCallback fun(projectile: EsvShootProjectileRequest)
--- @alias ExtShootProjectileCallback fun(projectile: EsvProjectile)
--- @alias ExtProjectileHitCallback fun(projectile: EsvProjectile, hitObject: EsvGameObject|nil, position: number[])
--- @alias ExtGroundHitCallback fun(caster: EsvGameObject, position: number[], damageList: DamageList)
--- @alias ExtInputEventCallback fun(event: InputEvent)

--- @class Ext
Ext = {
    ExtraData = {
        FirstItemTypeShift = 9,
        SecondItemTypeShift = 16,
        PickpocketGoldValuePerPoint = 200,
        PickpocketWeightPerPoint = 2000,
        PickpocketExperienceLevelsPerPoint = 4,
        PersuasionAttitudeBonusPerPoint = 5,
        WandUsesMax = 10,
        AttributeBaseValue = 10,
        AttributeCharCreationBonus = 1,
        AttributeLevelGrowth = 2,
        AttributeBoostGrowth = 0.75,
        AttributeGrowthDamp = 0.7,
        AttributeSoftCap = 40,
        WitsGrowthDamp = 0.5,
        VitalityStartingAmount = 21,
        VitalityExponentialGrowth = 1.25,
        VitalityLinearGrowth = 9.091,
        VitalityToDamageRatio = 5,
        VitalityToDamageRatioGrowth = 0.2,
        ExpectedDamageBoostFromAttributePerLevel = 0.065,
        ExpectedDamageBoostFromSkillAbilityPerLevel = 0.015,
        ExpectedDamageBoostFromWeaponAbilityPerLevel = 0.025,
        ExpectedConGrowthForArmorCalculation = 1,
        FirstVitalityLeapLevel = 9,
        FirstVitalityLeapGrowth = 1.25,
        SecondVitalityLeapLevel = 13,
        SecondVitalityLeapGrowth = 1.25,
        ThirdVitalityLeapLevel = 16,
        ThirdVitalityLeapGrowth = 1.25,
        FourthVitalityLeapLevel = 18,
        FourthVitalityLeapGrowth = 1.35,
        DamageBoostFromAttribute = 0.05,
        MonsterDamageBoostPerLevel = 0.02,
        PhysicalArmourBoostFromAttribute = 0,
        MagicArmourBoostFromAttribute = 0,
        VitalityBoostFromAttribute = 0.07,
        DodgingBoostFromAttribute = 0,
        HealToDamageRatio = 1.3,
        ArmorToVitalityRatio = 0.55,
        ArmorRegenTimer = 0.01,
        ArmorRegenConstGrowth = 1,
        ArmorRegenPercentageGrowth = 10,
        ArmorAfterHitCooldown = -7,
        MagicArmorRegenTimer = 0.01,
        MagicArmorRegenConstGrowth = 1,
        MagicArmorRegenPercentageGrowth = 10,
        MagicArmorAfterHitCooldown = -7,
        ArmorHeadPercentage = 0.15,
        ArmorUpperBodyPercentage = 0.3,
        ArmorLowerBodyPercentage = 0.2,
        ArmorShieldPercentage = 0.5,
        ArmorHandsPercentage = 0.15,
        ArmorFeetPercentage = 0.15,
        ArmorBeltPercentage = 0.1,
        ArmorAmuletPercentage = 0.12,
        ArmorRingPercentage = 0.08,
        SkillMemoryCostReductionFromAbility = 0,
        CharacterBaseMemoryCapacity = 3,
        CharacterBaseMemoryCapacityGrowth = 0.5,
        CharacterAttributePointsPerMemoryCapacity = 1,
        LoremasterBonusToMemory = 2,
        AbilityBaseValue = 0,
        NumStartingCombatAbilityPoints = 2,
        CombatAbilityCap = 10,
        CombatAbilityLevelGrowth = 1,
        CombatAbilityNpcGrowth = 0.1,
        CombatAbilityDamageBonus = 5,
        CombatAbilityCritBonus = 1,
        CombatAbilityCritMultiplierBonus = 5,
        CombatAbilityAccuracyBonus = 5,
        CombatAbilityDodgingBonus = 1,
        CombatAbilityReflectionBonus = 5,
        LeadershipRange = 8,
        LeadershipDodgingBonus = 2,
        LeadershipAllResBonus = 3,
        NumStartingCivilAbilityPoints = 2,
        CivilAbilityCap = 5,
        CivilAbilityLevelGrowth = 4,
        CivilPointOffset = 2,
        SavethrowLowChance = 15,
        SavethrowHighChance = 50,
        SavethrowBelowLowPenalty = 5,
        SavethrowPenaltyCap = -30,
        CriticalBonusFromWits = 1,
        InitiativeBonusFromWits = 1,
        WeaponAccuracyPenaltyPerLevel = -20,
        WeaponAccuracyPenaltyCap = -80,
        ShieldAPCost = 0,
        CharacterWeightLight = 40000,
        CharacterWeightMedium = 70000,
        CharacterWeightHeavy = 120000,
        WeaponWeightLight = 1000,
        WeaponWeightMedium = 3000,
        WeaponWeightHeavy = 6000,
        HighGroundThreshold = 2.4,
        HighGroundBaseDamageBonus = 0.2,
        HighGroundMeleeRange = 1,
        HighGroundRangeMultiplier = 2.5,
        LowGroundBaseDamagePenalty = -0.1,
        SneakDefaultAPCost = 4,
        SneakSpeedBoost = -30,
        BlindRangePenalty = 3,
        RangeBoostedGlobalCap = 30,
        SurfaceDurationFromHitFloorReaction = 18,
        SurfaceDurationFireIgniteOverride = 12,
        SurfaceDurationFromCharacterBleeding = -1,
        SurfaceDurationBlessedCursed = -1,
        SurfaceDurationAfterDecay = -1,
        SmokeDurationAfterDecay = 6,
        DualWieldingAPPenalty = 2,
        DualWieldingDamagePenalty = 0.5,
        GhostLifeTime = 3,
        ChanceToSetStatusOnContact = 100,
        AbilityPhysArmorBonusBase = 5,
        AbilityPhysArmorBonusPerPoint = 2,
        AbilityPhysArmorBonusMax = 5,
        AbilityMagicArmorBonusBase = 5,
        AbilityMagicArmorBonusPerPoint = 2,
        AbilityMagicArmorBonusMax = 5,
        AbilityVitalityBonusBase = 3,
        AbilityVitalityBonusPerPoint = 1,
        AbilityVitalityBonusMax = 3,
        SkillAbilityDamageToPhysicalArmorPerPoint = 0,
        SkillAbilityDamageToMagicArmorPerPoint = 0,
        SkillAbilityArmorRestoredPerPoint = 5,
        SkillAbilityVitalityRestoredPerPoint = 5,
        SkillAbilityHighGroundBonusPerPoint = 5,
        SkillAbilityFireDamageBoostPerPoint = 5,
        SkillAbilityPoisonAndEarthDamageBoostPerPoint = 5,
        SkillAbilityAirDamageBoostPerPoint = 5,
        SkillAbilityWaterDamageBoostPerPoint = 5,
        SkillAbilityPhysicalDamageBoostPerPoint = 5,
        SkillAbilityLifeStealPerPoint = 10,
        LifestealFromReflectionModifier = 0,
        SkillAbilityCritMultiplierPerPoint = 5,
        SkillAbilityMovementSpeedPerPoint = 30,
        SkillAbilitySummonsStatsPerPoint = 4,
        SneakingAbilityMovementSpeedPerPoint = 6,
        TalentAttributePointsBonus = 2,
        TalentCombatAbilityPointsBonus = 1,
        TalentCivilAbilityPointsBonus = 1,
        TalentMemoryBonus = 3,
        TalentQuestRootedMemoryBonus = 3,
        TalentRagerPercentage = 70,
        SoftLevelCap = 20,
        PickpocketRequirementDecreaseFromFinesse = 1,
        SkillCombustionRadius = 3,
        TalentPerfectionistAccuracyBonus = 10,
        TalentPerfectionistCriticalChanceBonus = 10,
        TalentExecutionerActionPointBonus = 2,
        TalentPointOffset = 2,
        TalentViolentMagicCriticalChancePercent = 100,
        TalentPointPerLevels = 5,
        TalentQuickStepPartialApBonus = 1,
        SkillHeightRangeMultiplier = 1,
        AbilityPersuasionBonusPerPoint = 4,
        FreeMovementDistanceWhenAttacking = 1,
        TalentSneakingDamageBonus = 40,
        MaximumSummonsInCombat = 4,
        SpiritVisionFallbackRadius = 10,
        AbilityPerseveranceArmorPerPoint = 5,
        AiCoverProjectileTurnMemory = 2,
        CarryWeightBase = 0,
        CarryWeightPerStr = 10000,
        MoveToCarryWeightRatio = 0.75,
        TalentResistDeathVitalityPercentage = 20,
        DeflectProjectileRange = 1,
        SummoningAbilityBonus = 10,
        SurfaceAbsorbBoostPerTilesCount = 10,
        TalentWhatARushThreshold = 50,
        IncarnateSummoningLevel = 10,
        CleaveRangeOverride = 125,
        DamageToThrownWeightRatio = 0.5,
        FleeDistance = 13,
        GlobalGoldValueMultiplier = 1,
        PriceGrowth = 1.12,
        FirstPriceLeapLevel = 9,
        FirstPriceLeapGrowth = 1.75,
        SecondPriceLeapLevel = 13,
        SecondPriceLeapGrowth = 1.15,
        ThirdPriceLeapLevel = 16,
        ThirdPriceLeapGrowth = 1.5,
        FourthPriceLeapLevel = 18,
        FourthPriceLeapGrowth = 1.15,
        PriceModCasualDifficulty = 2.1,
        PriceModClassicDifficulty = 2.5,
        PriceModHardcoreDifficulty = 2.7,
        PriceBarterCoefficient = 0.1,
        PriceAttitudeCoefficient = 0.005,
        PriceRoundToFiveAfterAmount = 100,
        PriceRoundToTenAfterAmount = 1000,
        LevelCap = 35,
        GMCharacterAttributeCap = 100,
        GMCharacterArmorCap = 999999,
        GMCharacterResistanceMin = -1000,
        GMCharacterResistanceMax = 1000,
        GMCharacterAPCap = 100,
        GMCharacterSPCap = 3,
        GMItemLevelCap = 50,
        GMItemAttributeCap = 100,
        GMItemArmorMin = -999999,
        GMItemArmorMax = 999999,
        GMItemResistanceMin = -1000,
        GMItemResistanceMax = 1000,
        LoneWolfMaxAPBonus = 2,
        LoneWolfAPBonus = 2,
        LoneWolfMagicArmorBoostPercentage = 60,
        LoneWolfArmorBoostPercentage = 60,
        LoneWolfVitalityBoostPercentage = 30,
        LivingArmorHealPercentage = 35,
        TorturerDamageStatusTurnIncrease = 1,
        UnstableDamagePercentage = 50,
        UnstableRadius = 3,
        TalentResurrectExtraHealthPercent = 10,
        PoisonedFoodDamage = 1,
        PoisonedFoodDamageRange = 10,
        PoisonedFoodDamageMultiplier = 40,
        TraderDroppedItemsPercentage = 51,
        TraderDroppedItemsCap = 5,
        StatusDefaultDistancePerDamage = 0.75,
        TraderDonationsRequiredAttitude = -45,
        TeleportUnchainDistance = 50,
        TalentHumanCriticalMultiplier = 10,
        TalentHumanCriticalChance = 5,
        TalentSneakingAPCost = 1
    }
}

--- Returns the version number of the Osiris Extender
--- @return integer
function Ext.Version() end

--- Returns the version number of the game
--- @return string
function Ext.GameVersion() end

--- Loads the specified Lua file
--- @param modGuid string GUID of the module containing the Lua file
--- @param fileName string Path of Lua file, relative to Mods/<Mod>/Story/RawFiles/Lua
function Ext.Require(modGuid, fileName) end

--- Registers a function to call when an extender event is thrown
--- @param event ExtEngineEvent Event to listen for
--- @param callback function Lua function to run when the event fires
function Ext.RegisterListener(event, callback) end

--- Registers a function that is called when certain Osiris functions are called.
--- Supports events, built-in queries, DBs, PROCs, QRYs (user queries).
--- @param name string Osiris function/database name
--- @param arity number Number of columns for DBs or the number of parameters (both IN and OUT) for functions
--- @param event string Event type ('before' - triggered before Osiris call; 'after' - after Osiris call; 'beforeDelete'/'afterDelete' - before/after delete from DB)
--- @param handler function Lua function to run when the event fires
function Ext.RegisterOsirisListener(name, arity, event, handler) end

--- Registers a new call in Osiris
--- @param func function Function to register
--- @param funcName string Name of call to register
--- @param arguments string Call argument list
function Ext.NewCall(func, funcName, arguments) end

--- Registers a new query in Osiris
--- @param func function Function to register
--- @param funcName string Name of query to register
--- @param arguments string Query argument list
function Ext.NewQuery(func, funcName, arguments) end

--- Registers a new event in Osiris
--- @param funcName string Name of event to register
--- @param arguments string Event argument list
function Ext.NewEvent(funcName, arguments) end

--- Print to console window and editor messages pane
--- @vararg any
function Ext.Print(...) end

--- Print warning to console window and editor messages pane
--- @vararg any
function Ext.PrintWarning(...) end

--- Print error to console window and editor messages pane
--- @vararg any
function Ext.PrintError(...) end

--- Parse a JSON document into a Lua object
--- @param json string JSON string to parse
--- @return any
function Ext.JsonParse(json) end

--- Converts a Lua value into a JSON document
--- @param val any Value to serialize
--- @return string JSON document
function Ext.JsonStringify(val) end

--- Returns whether the specified mod is loaded
--- @param modGuid string GUID of the module
--- @return boolean
function Ext.IsModLoaded(modGuid) end

--- Returns the list of loaded modules in load order
--- @return string[]
function Ext.GetModLoadOrder() end

--- Returns detailed information about the specified (loaded) module
--- @param modGuid string GUID of the module
--- @return ModInfo
function Ext.GetModInfo(modGuid) end

--- Returns the list of loaded stat entries
--- @param type string|nil Type of stat entry to fetch (StatusData, SkillData, ...)
--- @return string[]
function Ext.GetStatEntries(type) end

--- Returns the list of stat entries that were loaded before the specified mod
--- @param modGuid string Mod GUID to check
--- @param type string|nil Type of stat entry to fetch (StatusData, SkillData, ...)
--- @return string[]
function Ext.GetStatEntriesLoadedBefore(modId, type) end

--- Returns an attribute of the specified stat entry
--- @param stat string Stat entry name
--- @param attribute string Stat attribute name
--- @return any
function Ext.StatGetAttribute(stat, attribute) end

--- Updates an attribute of the specified stat entry
--- @param stat string Stat entry name
--- @param attribute string Stat attribute name
--- @param value any New stat value
function Ext.StatSetAttribute(stat, attribute, value) end

--- Adds a property description to the specified stat entry
--- @param stat string Stat entry name
--- @param attribute string Property list attribute name
--- @param description any Description to add
function Ext.StatAddCustomDescription(stat, attribute, description) end

--- Returns all skills from the specified skill set
--- @param name string Name of skill set entry
--- @return StatSkillSet|nil
function Ext.GetSkillSet(name) end

--- Updates all properties of the specified skill set.
--- The function expects a table in the same format as the one returned by GetSkillSet.
--- @param skillSet StatSkillSet
function Ext.UpdateSkillSet(skillSet) end

--- Returns all equipment from the specified equipment set
--- @param name string Name of equipment set entry
--- @return StatEquipmentSet|nil
function Ext.GetEquipmentSet(name) end

--- Updates all properties of the specified equipment set.
--- The function expects a table in the same format as the one returned by GetEquipmentSet.
--- @param equipmentSet StatEquipmentSet
function Ext.UpdateEquipmentSet(equipmentSet) end

--- Returns the specified DeltaMod or nil on failure
--- @param name string Name of delta mod
--- @param modifierType string Modifier type (Armor/Weapon)
--- @return DeltaMod
function Ext.GetDeltaMod(name, modifierType) end

--- Updates all properties of the specified DeltaMod.
--- The function expects a table in the same format as the one returned by GetDeltaMod.
--- @param deltaMod DeltaMod Name of delta mod
function Ext.UpdateDeltaMod(deltaMod) end

--- Returns the specified crafting item combination or nil on failure
--- @param name string Name of item combo
--- @return ItemCombo|nil
function Ext.GetItemCombo(name) end

--- Updates all properties of the specified item combination.
--- The function expects a table in the same format as the one returned by GetItemCombo.
--- @param itemCombo ItemCombo
function Ext.UpdateItemCombo(itemCombo) end

--- Returns the specified crafting preview data or nil on failure
--- @param name string Name of item combo preview data
--- @return ItemComboPreviewData|nil
function Ext.GetItemComboPreviewData(name) end

--- Updates all properties of the specified item combo preview.
--- The function expects a table in the same format as the one returned by GetItemComboPreviewData.
--- @param previewData ItemComboPreviewData
function Ext.UpdateItemComboPreviewData(previewData) end

--- Returns the specified crafting property or nil on failure
--- @param name string Name of item combo property
--- @return ItemComboProperty|nil
function Ext.GetItemComboProperty(name) end

--- Updates all properties of the specified item combo property.
--- The function expects a table in the same format as the one returned by GetItemComboProperty.
--- @param itemComboProperty ItemComboProperty
function Ext.UpdateItemComboProperty(itemComboProperty) end

--- Returns the specified treasure table or nil on failure
--- @param name string Name of treasure table
--- @return StatTreasureTable|nil
function Ext.GetTreasureTable(name) end

--- Updates all properties of the specified treasure table.
--- The function expects a table in the same format as the one returned by GetTreasureTable.
--- @param treasureTable StatTreasureTable
function Ext.UpdateTreasureTable(treasureTable) end

--- Returns the specified treasure category or nil on failure
--- @param name string Name of treasure category
--- @return StatTreasureCategory|nil
function Ext.GetTreasureCategory(name) end

--- Updates all properties of the specified treasure category.
--- The function expects a table in the same format as the one returned by GetTreasureCategory.
--- @param name string Name of treasure category
--- @param treasureCategory StatTreasureCategory
function Ext.UpdateTreasureCategory(name, treasureCategory) end

--- Returns the specified item progression item group or nil on failure
--- @param name string Name of item group
--- @return ItemGroup|nil
function Ext.GetItemGroup(name) end

--- Returns the specified item progression name group or nil on failure
--- @param name string Name of name group
--- @return ItemNameGroup|nil
function Ext.GetNameGroup(name) end

--- Registers a new skill property that can be triggered via SkillProperties
--- Stat syntax: data "SkillProperties" "EXT:<PROPERTY_NAME>[,<int>,<int>,<string>,<int>,<int>]"
--- The property name must always be preceded by the string "EXT:". 
--- Target contexts (SELF:, TARGET:, ...) and useing multiple actions in the same SkillProperties are supported.
--- Conditions for EXT: properties (i.e. "IF(COND):") are _NOT YET_ supported.
--- @param channel string Skill property name
--- @param defn CustomSkillProperty Event handlers for the skill property
function Ext.RegisterSkillProperty(name, defn) end

--- Replaces level scaling formula for the specified stat
--- @param statType string Stat entry type
--- @param attribute string Stat attribute name
--- @param func function Replacement scaling function
function Ext.StatSetLevelScaling(statType, attribute, func) end

--- Returns the property proxy of the specified stats entry
--- Returns level scaled values if the level parameter is not nil.
--- @param stat string Stat entry name
--- @param level integer|nil Level scaling level
--- @return StatEntryArmor|StatEntryCharacter|StatEntryObject|StatEntryPotion|StatEntryShield|StatEntrySkillData|StatEntryStatusData|StatEntryWeapon
function Ext.GetStat(stat, level) end

--- Creates a new stats entry on the server
--- @param name string Stat entry name
--- @param type string Stat entry type (i.e. SkillData, StatusData, etc.)
--- @param template string|nil When not nil, all properties are copied from the specified stats entry
--- @return StatEntryArmor|StatEntryCharacter|StatEntryObject|StatEntryPotion|StatEntryShield|StatEntrySkillData|StatEntryStatusData|StatEntryWeapon
function Ext.CreateStat(name, type, template) end

--- Synchronizes all modifications of the specified stat to all clients
--- @param name string Stat entry name
--- @param persist boolean|nil Persist stats entry to savegame?
function Ext.SyncStat(name, persist) end

--- Toggles whether the specified stats entry should be persisted to savegames
--- @param name string Stat entry name
--- @param persist boolean Persist stats entry to savegame?
function Ext.StatSetPersistence(name, persist) end

--- Returns the textual label assigned to the specified enumeration value
--- @param enum string Engine enumeration name
--- @param index number Value index to look up
--- @return string|nil
function Ext.EnumIndexToLabel(enum, index) end

--- Returns the numeric index assigned to the specified enumeration label
--- @param enum string Engine enumeration name
--- @param label string Value name to look for
--- @return number|nil
function Ext.EnumLabelToIndex(enum, label) end

-- Execute the SkillProperties of the specified skill on a target character.
--- @param skillId string Stats skill ID
--- @param attacker ObjectHandle|int|string Attacker character handle/NetID/GUID
--- @param target ObjectHandle|int|string Target character handle/NetID/GUID
--- @param position number[]
--- @param propertyContext string Target|AoE|Self|SelfOnHit|SelfOnEquip
--- @param isFromItem boolean
function Ext.ExecuteSkillPropertiesOnTarget(skillId, attacker, target, position, propertyContext, isFromItem) end

-- Execute the SkillProperties of the specified skill on a position.
--- @param skillId string Stats skill ID
--- @param attacker ObjectHandle|int|string Attacker character handle/NetID/GUID
--- @param position number[]
--- @param radius number
--- @param propertyContext string Target|AoE|Self|SelfOnHit|SelfOnEquip
--- @param isFromItem boolean
function Ext.ExecuteSkillPropertiesOnPosition(skillId, attacker, position, radius, propertyContext, isFromItem) end

--- Returns the transformation rules that are applied when two neighbouring surfaces interact.
--- @return SurfaceInteractionSet[][]
function Ext.GetSurfaceTransformRules() end

--- Returns the surface template for the specified surface type
--- @param type string See SurfaceType enumeration
--- @return SurfaceTemplate
function Ext.GetSurfaceTemplate(type) end

--- Updates the transformation rules that are applied when two neighbouring surfaces interact.
--- @param rules SurfaceInteractionSet[][] New rules to apply
function Ext.UpdateSurfaceTransformRules(rules) end

--- Prepares a new surface action for execution
--- @param type string Surface action type
--- @return EsvSurfaceAction
function Ext.CreateSurfaceAction(type) end

--- Executes a surface action
--- @param action EsvSurfaceAction Action to execute
function Ext.ExecuteSurfaceAction(action) end

--- CAncels a surface action
--- @param actionHandle integer Action to cancel
function Ext.CancelSurfaceAction(actionHandle) end

--- Starts creating a new item using template GUID or cloning an existing item.
--- @param from EsvItem|string Template GUID or item to clone
--- @param recursive boolean|nil Copy items in container? (cloning only)
--- @return ItemConstructor
function Ext.CreateItemConstructor(from, recursive) end

--- Begin applying a status on the specified character or item.
--- @param target string|ObjectHandle Target character/item
--- @param statusId string Status ID to apply
--- @param lifeTime number Status lifetime (-1 = infinite, -2 = keep alive)
--- @return EsvStatus|nil
function Ext.PrepareStatus(target, statusId, lifeTime) end

--- Finish applying a status on the specified character or item.
--- @param status EsvStatus Status to apply
function Ext.ApplyStatus(status) end

--- Returns a table containing the UUID of all registered custom stat definitions
--- @return string[]
function Ext.GetAllCustomStats() end

--- Retrieve a custom stat definition by name
--- @param statName string Custom stat name to look for
--- @return CustomStatDefinition|nil
function Ext.GetCustomStatByName(statName) end

--- Retrieve a custom stat definition by id
--- @param statId string Custom stat UUID to look for
--- @return CustomStatDefinition|nil
function Ext.GetCustomStatById(statId) end

--- Create a new custom stat definition
--- @param name string Custom stat name
--- @param description string Custom stat description
--- @return string|nil Custom stat UUID
function Ext.CreateCustomStat(name, description) end

--- Returns the GUID of all characters on the specified level. 
--- Uses the current level if no level name was specified.
--- @param level string|nil Optional level name
--- @return string[]
function Ext.GetAllCharacters(level) end

--- Returns the GUID of all characters within a radius around the specified point.
--- @param x number
--- @param y number
--- @param z number
--- @param distance number
--- @return string[]
function Ext.GetCharactersAroundPosition(x, y, z, distance) end

--- Returns the GUID of all items on the specified level. 
--- Uses the current level if no level name was specified.
--- @param level string|nil Optional level name
--- @return string[]
function Ext.GetAllItems(level) end

--- Returns the GUID of all items within a radius around the specified point.
--- @param x number
--- @param y number
--- @param z number
--- @param distance number
--- @return string[]
function Ext.GetItemsAroundPosition(x, y, z, distance) end

--- Returns the GUID of all triggers on the specified level. 
--- Uses the current level if no level name was specified.
--- @param level string|nil Optional level name
--- @return string[]
function Ext.GetAllTriggers(level) end

--- Returns the property proxy of the specified character
--- @param id string|integer|ObjectHandle Character GUID or handle or NetID
--- @return EsvCharacter|EclCharacter
function Ext.GetCharacter(id) end

--- Returns the property proxy of the specified item
--- @param id string|integer|ObjectHandle Item GUID or handle or NetID
--- @return EsvItem|EclCharacter
function Ext.GetItem(id) end

--- Returns the property proxy of the specified trigger (server only)
--- @param id string|ObjectHandle Trigger GUID or handle
--- @return EsvTrigger
function Ext.GetTrigger(id) end

--- Returns the property proxy of the specified character, item, projectile or trigger
--- @param handle ObjectHandle Game object handle
--- @return EsvGameObject|EclGameObject
function Ext.GetGameObject(handle) end

--- Returns the property proxy of the specified surface
--- @param handle ObjectHandle Surface handle
--- @return EsvSurface
function Ext.GetSurface(handle) end

--- Returns the property proxy of the specified status
--- @param character string|integer|ObjectHandle Character GUID or handle or NetID
--- @param handle integer|StatusHandle Status handle or NetID
--- @return EsvStatus
function Ext.GetStatus(character, handle) end

--- Returns the specified turn-based combat
--- @param combatId integer Combat ID
--- @return EsvCombat
function Ext.GetCombat(combatId) end

--- Returns the AI grid for the currently active level
--- @return AiGrid
function Ext.GetAiGrid() end

--- Returns information about the currently active level
--- @return LevelDesc
function Ext.GetCurrentLevelData() end

--- Creates a new damage list object
--- @return DamageList
function Ext.NewDamageList() end

--- Returns whether Osiris is currently accessible or not.
--- @return boolean
function Ext.OsirisIsCallable() end

--- Returns a random number; equivalent to Lua random
--- @param low integer
--- @param up integer
--- @return integer|number
function Ext.Random(low, up) end

--- Rounds the specified number
--- @param n number
--- @return number
function Ext.Round(n) end

--- Generate Lua IDE helpers for the currently loaded module
--- @param builtin boolean|nil Only export built-in functions and names exported by Lua?
function Ext.GenerateIdeHelpers(builtin) end

--- Returns whether the code is executing in a client context
--- @return boolean
function Ext.IsClient() end

--- Returns whether the code is executing in a server context
--- @return boolean
function Ext.IsServer() end

--- Returns whether the Developer Mode switch is enabled
--- @return boolean
function Ext.IsDeveloperMode() end

--- Returns the current client/server game state machine state.
--- @return string
function Ext.GetGameState() end

--- Broadcast a message to all peers
--- @param channel string Channel that will receive the message
--- @param payload string Message payload
--- @param excludeCharacter string|nil Optional peer to exclude from broadcast
function Ext.BroadcastMessage(channel, payload, excludeCharacter) end

--- Sends a message to the peer that controls the specified character
--- @param characterGuid string Character that will receive the message
--- @param channel string Channel that will receive the message
--- @param payload string Message payload
function Ext.PostMessageToClient(characterGuid, channel, payload) end

--- Sends a message to the specified peer
--- @param userId number User that will receive the message
--- @param channel string Channel that will receive the message
--- @param payload string Message payload
function Ext.PostMessageToUser(userId, channel, payload) end

--- Sends a message to the server
--- @param channel string Channel that will receive the message
--- @param payload string Message payload
function Ext.PostMessageToServer(channel, payload) end

---@alias NetListenerCallback fun(channel:string, payload:string, user:integer|nil):void

--- Registers a listener that is called when a network message is received on the specified channel
--- @param channel string Network channel name
--- @param handler NetListenerCallback Lua handler
function Ext.RegisterNetListener(channel, handler) end

--- Registers a new dialog voice line for the specified speaker.
--- @param speakerGuid string Speaker character UUID
--- @param textKey string Translated string key of text line
--- @param path string Path to audio .WEM
--- @param length number Length of audio in seconds
function Ext.AddVoiceMetaData(speakerGuid, textKey, path, length) end

--- @param handle string Translated string handle
--- @param fallback string Fallback string if the specified handle is not found
--- @return string Translated string
function Ext.GetTranslatedString(handle, fallback) end

--- @param key string Translated string key
--- @return string,string Translated string and handle
function Ext.GetTranslatedStringFromKey(key) end

--- @param key string Translated string key
--- @param handle string Translated string handle
--- @return boolean
function Ext.CreateTranslatedStringKey(key, handle) end

--- @param handle string Translated string handle
--- @param text string Display text
--- @return boolean
function Ext.CreateTranslatedStringHandle(handle, text) end

--- @param key string Translated string key
--- @param text string Display text
--- @return string|nil Created string handle
function Ext.CreateTranslatedString(key, text) end

--- Redirects all file accesses to the specified path to another file.
--- @param path string Original path
--- @param newPath string New (redirected) path
function Ext.AddPathOverride(path, newPath) end

--- Returns whether the specified path is currently redirected to another path.
--- @param path string Original path
--- @return string|nil Overridden path
function Ext.GetPathOverride(path) end

--- Constructs a new Flash UI element
--- @param name string User-defined unique name that identifies the UI element
--- @param path string Path of the SWF file relative to the data directory
--- @param layer integer Stack order of the UI element
--- @return UIObject|nil
function Ext.CreateUI(name, path, layer) end

--- Retrieves an UI element with the specified name
--- @param name string User-defined unique name that identifies the UI element
--- @return UIObject|nil
function Ext.GetUI(name) end

--- Retrieves a built-in UI element at the specified path.
--- If no such element exists, the function returns nil.
--- @param path string UI SWF path relative to Data\
--- @return UIObject|nil
function Ext.GetBuiltinUI(path) end

--- Retrieves an engine UI element with the specified engine type ID.
--- If no such element exists, the function returns nil.
--- @param typeId number Engine UI element type ID
--- @return UIObject|nil
function Ext.GetUIByType(typeId) end

--- Destroys the specified UI element
--- @param name string User-defined unique name that identifies the UI element
function Ext.DestroyUI(name) end

--- Refresh the UI of the specified character
--- @param character ObjectHandle Handle of character
--- @param flags integer UI elements to refresh
function Ext.UISetDirty(character, flags) end

--- Enable/disable debug prints on Flash custom draw callbacks. Useful if you need to see what icon names a UI is handling, for usage with UIObject:SetCustomIcon.
--- @param enable boolean
function Ext.UIEnableCustomDrawCallDebugging(enable) end

---@alias UICallbackHandler fun(ui:UIObject, event:string, vararg any):void

--- Registers a listener that is called when the specified function is called from Flash
--- @param object UIObject UI object returned from Ext.CreateUI, Ext.GetUI or Ext.GetBuiltinUI
--- @param name string ExternalInterface function name
--- @param handler UICallbackHandler Lua handler
--- @param type string|nil Event type - 'Before' or 'After'
function Ext.RegisterUICall(object, name, handler, type) end

--- Registers a listener that is called when the specified function is called from Flash.
--- The event is triggered for every UI element with the specified type ID.
--- @param typeId number Engine UI element type ID
--- @param name string ExternalInterface function name
--- @param handler UICallbackHandler Lua handler
--- @param type string|nil Event type - 'Before' or 'After'
function Ext.RegisterUITypeCall(typeId, name, handler, type) end

--- Registers a listener that is called when the specified function is called from Flash.
--- The event is triggered regardless of which UI element it was called on.
--- (Function call capture must be enabled for every element type that needs to monitored!)
--- @param name string ExternalInterface function name
--- @param handler UICallbackHandler Lua handler
--- @param type string|nil Event type - 'Before' or 'After'
function Ext.RegisterUINameCall(name, handler, type) end

--- Registers a listener that is called when the specified method is called on the main timeline of the Flash object
--- @param object UIObject UI object returned from Ext.CreateUI, Ext.GetUI or Ext.GetBuiltinUI
--- @param method string Flash method name
--- @param handler UICallbackHandler Lua handler
--- @param type string|nil Event type - 'Before' or 'After'
function Ext.RegisterUIInvokeListener(object, name, handler, type) end

--- Registers a listener that is called when the specified method is called on the main timeline of the Flash object
--- The event is triggered for every UI element with the specified type ID.
--- @param typeId number Engine UI element type ID
--- @param method string Flash method name
--- @param handler UICallbackHandler Lua handler
--- @param type string|nil Event type - 'Before' or 'After'
function Ext.RegisterUITypeInvokeListener(typeId, name, handler, type) end

--- Registers a listener that is called when the specified method is called on the main timeline of the Flash object
--- The event is triggered regardless of which UI element it was called on.
--- @param method string Flash method name
--- @param handler UICallbackHandler Lua handler
--- @param type string|nil Event type - 'Before' or 'After'
function Ext.RegisterUINameInvokeListener(name, handler, type) end

--- Registers a listener that is called when a console command is entered in the dev console
--- @param cmd string Console command
--- @param handler function Lua handler
function Ext.RegisterConsoleCommand(cmd, handler) end

--- Write data to an external (persistent) file
--- @param path string File path relative to Documents\Larian Studios\Divinity Original Sin 2 Definitive Edition\Osiris Data
--- @param contents string File contents to write
function Ext.SaveFile(path, contents) end

--- Read data from an external (persistent) file
--- @param path string File path relative to Documents\Larian Studios\Divinity Original Sin 2 Definitive Edition\Osiris Data
--- @param context string|nil Path context (nil or "user" means relative to the Osiris Data directory; "data" means relative to game data path)
--- @return string File contents
function Ext.LoadFile(path, context) end

--- Returns a monotonic value representing the current time in milliseconds.
--- Useful for performance measurements / measuring real world time.
--- (Note: This value is not synchronized between peers and different clients may report different time values!)
--- @return number Time
function Ext.MonotonicTime() end

--- Returns whether the player has a compatible Script Extender version installed
--- @param playerGuid string GUID of player character
--- @return boolean
function Ext.PlayerHasExtender(playerGuid) end

--- Returns information about current mouse position and hovered objects
--- @return EclPickingState
function Ext.GetPickingState() end

--- Triggers a breakpoint in the Lua debugger.
--- If no debugger is connected, the function does nothing.
function Ext.DebugBreak() end

--- Handle to double conversion hack for use in Flash external interface calls
--- (Some of the builtin functions treat handles as double values)
--- @param handle ObjectHandle|StatusHandle Handle to cast
--- @return number Double handle
function Ext.HandleToDouble(handle) end

--- Double to handle conversion hack for use in Flash external interface calls
--- (Some of the builtin functions treat handles as double values)
--- @param handle number Double handle to cast
--- @return ObjectHandle|StatusHandle
function Ext.DoubleToHandle(handle) end

Ext.Audio = {}

--- Set audio state switch
--- @param gameObject ObjectHandle|string|nil Character handle, built-in sound object name or nil
--- @param switchGroup string Switch group name
--- @param state string Switch state
--- @return boolean
function Ext.Audio.SetSwitch(gameObject, switchGroup, state) end

--- Set audio state
--- @param stateGroup string State group name
--- @param state string State
--- @return boolean
function Ext.Audio.SetState(stateGroup, state) end

--- Set real-time parameter (RTPC) value
--- @param gameObject ObjectHandle|string|nil Character handle, built-in sound object name or nil
--- @param rtpc string Parameter name
--- @param value number Parameter value
--- @return boolean
function Ext.Audio.SetRTPC(gameObject, rtpc, value) end

--- Reset real-time parameter (RTPC) value
--- @param gameObject ObjectHandle|string|nil Character handle, built-in sound object name or nil
--- @param rtpc string Parameter Name
--- @return boolean
function Ext.Audio.ResetRTPC(gameObject, rtpc) end

--- Get real-time parameter (RTPC) value
--- @param gameObject ObjectHandle|string|nil Character handle, built-in sound object name or nil
--- @param rtpc string Parameter name
--- @return number
function Ext.Audio.GetRTPC(gameObject, rtpc) end

--- Stop audio
--- @param gameObject ObjectHandle|string|nil Character handle, built-in sound object name or nil
function Ext.Audio.Stop(gameObject) end

--- Pause audio playback
function Ext.Audio.PauseAllSounds() end

--- Resume audio playback
function Ext.Audio.ResumeAllSounds() end

--- Trigger an audio event
--- @param gameObject ObjectHandle|string|nil Character handle, built-in sound object name or nil
--- @param eventName string Event to trigger
--- @param positionSec number|nil Position in audio track
--- @return boolean
function Ext.Audio.PostEvent(gameObject, eventName, positionSec) end

--- Trigger an audio event using an external audio file
--- @param gameObject ObjectHandle|string|nil Character handle, built-in sound object name or nil
--- @param eventName string Event to trigger
--- @param path string Audio file path (relative to data directory)
--- @param codecId integer Codec ID
--- @return boolean
function Ext.Audio.PlayExternalSound(gameObject, eventName, path, codecId) end