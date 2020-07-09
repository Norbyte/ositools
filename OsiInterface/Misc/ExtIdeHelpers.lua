-- Special global value that contains the current mod UUID during load
ModuleUUID = "UUID"

Osi = {}

--- @class DamageItem
--- @field public DamageType string
--- @field public Amount integer
local DamageItem = {
}

--- @class DamageList
local DamageList = {
    --- Returns the amount of damage with the specified type
    --- @param self DamageList
    --- @param damageType string DamageType enumeration
    --- @return integer
    GetByType = function (self, damageType) end,
    
    --- Add damage
    --- @param self DamageList
    --- @param damageType string DamageType enumeration
    --- @param amount integer
    Add = function (self, damageType, amount) end,
    
    --- Clear damage list.
    --- If damageType is specified it only removes damage items with the specified type.
    --- @param self DamageList
    --- @param damageType string|nil DamageType enumeration
    Clear = function (self, damageType) end,
    
    --- Multiply every damage item with the specified value
    --- @param self DamageList
    --- @param multiplier number
    Multiply = function (self, multiplier) end,
    
    --- Merge another DamageList into this list
    --- @param self DamageList
    --- @param list DamageList List to merge
    Merge = function (self, list) end,
    
    --- Converts all damages to the specified type
    --- @param self DamageList
    --- @param damageType string DamageType enumeration
    ConvertDamageType = function (self, damageType) end,
    
    --- Aggregate all damage items with the same type
    --- @param self DamageList
    AggregateSameTypeDamages = function (self) end,
    
    --- Returns all damage items as a table
    --- @param self DamageList
    --- @return DamageItem[]
    ToTable = function (self) end,
}


--- @class EoCGameObjectTemplate
--- @field public Id string
--- @field public Name string
--- @field public TemplateName string
--- @field public IsGlobal boolean
--- @field public IsDeleted boolean
--- @field public LevelName string
--- @field public ModFolder string
--- @field public GroupID string
--- @field public VisualTemplate string
--- @field public PhysicsTemplate string
--- @field public CastShadow boolean
--- @field public ReceiveDecal boolean
--- @field public AllowReceiveDecalWhenAnimated boolean
--- @field public IsReflecting boolean
--- @field public IsShadowProxy boolean
--- @field public RenderChannel number
--- @field public CameraOffset number[]
--- @field public HasParentModRelation boolean
--- @field public HasGameplayValue boolean
--- @field public DevComment string
--- @field public AIBoundsRadius number
--- @field public AIBoundsHeight number
--- @field public DisplayName string
--- @field public Opacity number
--- @field public Fadeable boolean
--- @field public FadeIn boolean
--- @field public SeeThrough boolean
--- @field public FadeGroup string
--- @field public GameMasterSpawnSection string
--- @field public GameMasterSpawnSubSection string
local EoCGameObjectTemplate = {}


--- @class CharacterTemplate : EoCGameObjectTemplate
--- @field public Icon string
--- @field public Stats string
--- @field public SkillSet string
--- @field public Equipment string
--- @field public LightID string
--- @field public HitFX string
--- @field public DefaultDialog string
--- @field public SpeakerGroup string
--- @field public GeneratePortrait string
--- @field public WalkSpeed number
--- @field public RunSpeed number
--- @field public ClimbAttachSpeed number
--- @field public ClimbLoopSpeed number
--- @field public ClimbDetachSpeed number
--- @field public CanShootThrough boolean
--- @field public WalkThrough boolean
--- @field public CanClimbLadders boolean
--- @field public IsPlayer boolean
--- @field public Floating boolean
--- @field public SpotSneakers boolean
--- @field public CanOpenDoors boolean
--- @field public AvoidTraps boolean
--- @field public InfluenceTreasureLevel boolean
--- @field public HardcoreOnly boolean
--- @field public NotHardcore boolean
--- @field public JumpUpLadders boolean
--- @field public NoRotate boolean
--- @field public IsHuge boolean
--- @field public EquipmentClass number
--- @field public ExplodedResourceID string
--- @field public ExplosionFX string
--- @field public VisualSetResourceID string
--- @field public VisualSetIndices number
--- @field public TrophyID string
--- @field public SoundInitEvent string
--- @field public SoundAttachBone string
--- @field public SoundAttenuation number
--- @field public CoverAmount number
--- @field public LevelOverride number
--- @field public ForceUnsheathSkills boolean
--- @field public CanBeTeleported boolean
--- @field public ActivationGroupId string
--- @field public SoftBodyCollisionTemplate string
--- @field public RagdollTemplate string
--- @field public DefaultState number
--- @field public GhostTemplate string
--- @field public IsLootable boolean
--- @field public IsEquipmentLootable boolean
--- @field public InventoryType number
--- @field public IsArenaChampion boolean
--- @field public FootstepWeight string
local CharacterTemplate = {}


--- @class ItemTemplate : EoCGameObjectTemplate
--- @field public Icon string
--- @field public CanBePickedUp boolean
--- @field public CanBeMoved boolean
--- @field public CoverAmount number
--- @field public CanShootThrough boolean
--- @field public CanClickThrough boolean
--- @field public Destroyed boolean
--- @field public WalkThrough boolean
--- @field public WalkOn boolean
--- @field public Wadable boolean
--- @field public IsInteractionDisabled boolean
--- @field public IsPinnedContainer boolean
--- @field public StoryItem boolean
--- @field public FreezeGravity boolean
--- @field public IsKey boolean
--- @field public IsTrap boolean
--- @field public IsSurfaceBlocker boolean
--- @field public IsSurfaceCloudBlocker boolean
--- @field public TreasureOnDestroy boolean
--- @field public IsHuge boolean
--- @field public HardcoreOnly boolean
--- @field public NotHardcore boolean
--- @field public UsePartyLevelForTreasureLevel boolean
--- @field public Unimportant boolean
--- @field public Hostile boolean
--- @field public UseOnDistance boolean
--- @field public UseRemotely boolean
--- @field public IsBlocker boolean
--- @field public IsPointerBlocker boolean
--- @field public UnknownDisplayName boolean
--- @field public Tooltip number
--- @field public Stats string
--- @field public OnUseDescription string
--- @field public DefaultState string
--- @field public Owner string
--- @field public Key string
--- @field public HitFX string
--- @field public LockLevel number 
--- @field public Amount number
--- @field public MaxStackAmount number
--- @field public TreasureLevel number
--- @field public DropSound string
--- @field public PickupSound string
--- @field public UseSound string
--- @field public EquipSound string
--- @field public UnequipSound string
--- @field public InventoryMoveSound string
--- @field public LoopSound string
--- @field public SoundInitEvent string
--- @field public SoundAttachBone string
--- @field public SoundAttenuation number
--- @field public Description string
--- @field public UnknownDescription string
--- @field public Speaker string
--- @field public AltSpeaker string
--- @field public SpeakerGroup string
--- @field public ActivationGroupId string
--- @field public Race number
--- @field public IsWall boolean
--- @field public LevelOverride number
--- @field public Floating boolean
--- @field public IsSourceContainer boolean
--- @field public MeshProxy string
--- @field public IsPublicDomain boolean
--- @field public AllowSummonTeleport boolean
local ItemTemplate = {}


--- @class ProjectileTemplate : EoCGameObjectTemplate
--- @field public LifeTime number
--- @field public Speed number
--- @field public Acceleration number
--- @field public CastBone string
--- @field public ImpactFX string
--- @field public TrailFX string
--- @field public DestroyTrailFXOnImpact boolean
--- @field public BeamFX string
--- @field public PreviewPathMaterial string
--- @field public PreviewPathImpactFX string
--- @field public PreviewPathRadius number
--- @field public ImpactFXSize number
--- @field public RotateImpact boolean
--- @field public IgnoreRoof boolean
--- @field public DetachBeam boolean
--- @field public NeedsArrowImpactSFX boolean
--- @field public ProjectilePath boolean
--- @field public PathShift string
--- @field public PathRadius number
--- @field public PathMinArcDist number
--- @field public PathMaxArcDist number
--- @field public PathRepeat number
local ProjectileTemplate = {}


-- FIXME EclStatus

--- @class EsvStatus
--- @field public StatusType string Engine status type (HIT, DAMAGE, CONSUME, ...)
--- @field public StatusId string
--- @field public CanEnterChance integer
--- @field public StartTimer number
--- @field public LifeTime number
--- @field public CurrentLifeTime number
--- @field public TurnTimer number
--- @field public Strength number
--- @field public StatsMultiplier number
--- @field public DamageSourceType string See CauseType enumeration
--- @field public StatusHandle integer
--- @field public TargetHandle integer Handle of game object this status was applied to
--- @field public StatusSourceHandle integer Handle of game object that caused this status

-- StatusFlags0
--- @field public KeepAlive boolean
--- @field public IsOnSourceSurface boolean
--- @field public IsFromItem boolean
--- @field public Channeled boolean
--- @field public IsLifeTimeSet boolean
--- @field public InitiateCombat boolean
--- @field public Influence boolean
-- StatusFlags1
--- @field public BringIntoCombat boolean
--- @field public IsHostileAct boolean
--- @field public IsInvulnerable boolean
--- @field public IsResistingDeath boolean
-- StatusFlags2
--- @field public ForceStatus boolean
--- @field public ForceFailStatus boolean
--- @field public RequestDelete boolean
--- @field public RequestDeleteAtTurnEnd boolean
--- @field public Started boolean
local EsvStatus = {
}

--- @class EsvStatusHIT : EsvStatus
--- @field public HitByHandle integer
--- @field public HitWithHandle integer
--- @field public WeaponHandle integer
--- @field public HitReason string
--- @field public SkillId string
--- @field public Interruption boolean
--- @field public AllowInterruptAction boolean
--- @field public ForceInterrupt boolean
--- @field public DecDelayDeathCount boolean
--- @field public ImpactPosition number[]
--- @field public ImpactOrigin number[]
--- @field public ImpactDirection number[]
local EsvStatusHIT = {
}

--- @class EsvStatusCONSUME : EsvStatus
-- TODO - Skills, Items, ResetCooldownsSet, StatsIDs?
--- @field public ResetAllCooldowns boolean
--- @field public ResetOncePerCombat boolean
--- @field public ScaleWithVitality boolean
--- @field public LoseControl boolean
--- @field public ApplyStatusOnTick string
--- @field public EffectTime number
--- @field public StatsId string
--- @field public StackId string
--- @field public OriginalWeaponStatsId string
--- @field public OverrideWeaponStatsId string
--- @field public OverrideWeaponHandle integer
--- @field public SavingThrow integer TODO enum + enum prop!
--- @field public SourceDirection number[]
--- @field public Turn integer
--- @field public HealEffectOverride string See HealEffect enumeration
--- @field public Poisoned boolean
local EsvStatusCONSUME = {
}

--- @class EsvStatusHEALING : EsvStatus
--- @field public HealAmount integer
--- @field public TimeElapsed number
--- @field public HealEffect string See HealEffect enumeration
--- @field public HealEffectId string
--- @field public SkipInitialEffect boolean
--- @field public HealingEvent number
--- @field public HealStat string See StatusHealType enumeration
--- @field public AbsorbSurfaceRange integer
local EsvStatusHEALING = {
}

--- @class EsvStatusHEAL : EsvStatus
--- @field public EffectTime number
--- @field public HealAmount integer
--- @field public HealEffect string See HealEffect enumeration
--- @field public HealEffectId string
--- @field public HealType string See StatusHealType enumeration
--- @field public AbsorbSurfaceRange integer
--- @field public TargetDependentHeal boolean
local EsvStatusHEAL = {
}



--- @class StatCharacterDynamic
-- Properties from PropertyMap
--- @field public SummonLifelinkModifier integer
--- @field public Strength integer
--- @field public Memory integer
--- @field public Intelligence integer
--- @field public Movement integer
--- @field public MovementSpeedBoost integer
--- @field public Finesse integer
--- @field public Wits integer
--- @field public Constitution integer
--- @field public FireResistance integer
--- @field public EarthResistance integer
--- @field public WaterResistance integer
--- @field public AirResistance integer
--- @field public PoisonResistance integer
--- @field public ShadowResistance integer
--- @field public Willpower integer
--- @field public Bodybuilding integer
--- @field public PiercingResistance integer
--- @field public PhysicalResistance integer
--- @field public CorrosiveResistance integer
--- @field public MagicResistance integer
--- @field public CustomResistance integer
--- @field public Sight integer
--- @field public Hearing integer
--- @field public FOV integer
--- @field public APMaximum integer
--- @field public APStart integer
--- @field public APRecovery integer
--- @field public CriticalChance integer
--- @field public Initiative integer
--- @field public Vitality integer
--- @field public VitalityBoost integer
--- @field public MagicPoints integer
--- @field public Level integer
--- @field public Gain integer
--- @field public Armor integer
--- @field public MagicArmor integer
--- @field public ArmorBoost integer
--- @field public MagicArmorBoost integer
--- @field public ArmorBoostGrowthPerLevel integer
--- @field public MagicArmorBoostGrowthPerLevel integer
--- @field public DamageBoost integer
--- @field public DamageBoostGrowthPerLevel integer
--- @field public Accuracy integer
--- @field public Dodge integer
--- @field public MaxResistance integer
--- @field public LifeSteal integer
--- @field public Weight integer
--- @field public ChanceToHitBoost integer
--- @field public RangeBoost integer
--- @field public APCostBoost integer
--- @field public SPCostBoost integer
--- @field public MaxSummons integer
--- @field public BonusWeaponDamageMultiplier integer
--- @field public TranslationKey integer
--- @field public BonusWeapon integer
--- @field public StepsType integer
-- TODO Abilities, Talents, RemovedTalents, Traits
local StatCharacterDynamic = {
}


--- @class StatBase
--- "Fake" base class to avoid declaring attribute flags/talents/abilities twice

-- StatAttributeFlags
--- @field public FreezeImmunity boolean
--- @field public BurnImmunity boolean
--- @field public StunImmunity boolean
--- @field public PoisonImmunity boolean
--- @field public CharmImmunity boolean
--- @field public FearImmunity boolean
--- @field public KnockdownImmunity boolean
--- @field public MuteImmunity boolean
--- @field public ChilledImmunity boolean
--- @field public WarmImmunity boolean
--- @field public WetImmunity boolean
--- @field public BleedingImmunity boolean
--- @field public CrippledImmunity boolean
--- @field public BlindImmunity boolean
--- @field public CursedImmunity boolean
--- @field public WeakImmunity boolean
--- @field public SlowedImmunity boolean
--- @field public DiseasedImmunity boolean
--- @field public InfectiousDiseasedImmunity boolean
--- @field public PetrifiedImmunity boolean
--- @field public DrunkImmunity boolean
--- @field public SlippingImmunity boolean
--- @field public FreezeContact boolean
--- @field public BurnContact boolean
--- @field public StunContact boolean
--- @field public PoisonContact boolean
--- @field public ChillContact boolean
--- @field public Torch boolean
--- @field public Arrow boolean
--- @field public Unbreakable boolean
--- @field public Unrepairable boolean
--- @field public Unstorable boolean
--- @field public Grounded boolean
--- @field public HastedImmunity boolean
--- @field public TauntedImmunity boolean
--- @field public SleepingImmunity boolean
--- @field public AcidImmunity boolean
--- @field public SuffocatingImmunity boolean
--- @field public RegeneratingImmunity boolean
--- @field public DisarmedImmunity boolean
--- @field public DecayingImmunity boolean
--- @field public ClairvoyantImmunity boolean
--- @field public EnragedImmunity boolean
--- @field public BlessedImmunity boolean
--- @field public ProtectFromSummon boolean
--- @field public Floating boolean
--- @field public DeflectProjectiles boolean
--- @field public IgnoreClouds boolean
--- @field public MadnessImmunity boolean
--- @field public ChickenImmunity boolean
--- @field public IgnoreCursedOil boolean
--- @field public ShockedImmunity boolean
--- @field public WebImmunity boolean
--- @field public LootableWhenEquipped boolean
--- @field public PickpocketableWhenEquipped boolean
--- @field public LoseDurabilityOnCharacterHit boolean
--- @field public EntangledContact boolean
--- @field public ShacklesOfPainImmunity boolean
--- @field public MagicalSulfur boolean
--- @field public ThrownImmunity boolean
--- @field public InvisibilityImmunity boolean

-- Talents (prefix TALENT_)
--- @field public TALENT_None boolean
--- @field public TALENT_ItemMovement boolean
--- @field public TALENT_ItemCreation boolean
--- @field public TALENT_Flanking boolean
--- @field public TALENT_AttackOfOpportunity boolean
--- @field public TALENT_Backstab boolean
--- @field public TALENT_Trade boolean
--- @field public TALENT_Lockpick boolean
--- @field public TALENT_ChanceToHitRanged boolean
--- @field public TALENT_ChanceToHitMelee boolean
--- @field public TALENT_Damage boolean
--- @field public TALENT_ActionPoints boolean
--- @field public TALENT_ActionPoints2 boolean
--- @field public TALENT_Criticals boolean
--- @field public TALENT_IncreasedArmor boolean
--- @field public TALENT_Sight boolean
--- @field public TALENT_ResistFear boolean
--- @field public TALENT_ResistKnockdown boolean
--- @field public TALENT_ResistStun boolean
--- @field public TALENT_ResistPoison boolean
--- @field public TALENT_ResistSilence boolean
--- @field public TALENT_ResistDead boolean
--- @field public TALENT_Carry boolean
--- @field public TALENT_Throwing boolean
--- @field public TALENT_Repair boolean
--- @field public TALENT_ExpGain boolean
--- @field public TALENT_ExtraStatPoints boolean
--- @field public TALENT_ExtraSkillPoints boolean
--- @field public TALENT_Durability boolean
--- @field public TALENT_Awareness boolean
--- @field public TALENT_Vitality boolean
--- @field public TALENT_FireSpells boolean
--- @field public TALENT_WaterSpells boolean
--- @field public TALENT_AirSpells boolean
--- @field public TALENT_EarthSpells boolean
--- @field public TALENT_Charm boolean
--- @field public TALENT_Intimidate boolean
--- @field public TALENT_Reason boolean
--- @field public TALENT_Luck boolean
--- @field public TALENT_Initiative boolean
--- @field public TALENT_InventoryAccess boolean
--- @field public TALENT_AvoidDetection boolean
--- @field public TALENT_AnimalEmpathy boolean
--- @field public TALENT_Escapist boolean
--- @field public TALENT_StandYourGround boolean
--- @field public TALENT_SurpriseAttack boolean
--- @field public TALENT_LightStep boolean
--- @field public TALENT_ResurrectToFullHealth boolean
--- @field public TALENT_Scientist boolean
--- @field public TALENT_Raistlin boolean
--- @field public TALENT_MrKnowItAll boolean
--- @field public TALENT_WhatARush boolean
--- @field public TALENT_FaroutDude boolean
--- @field public TALENT_Leech boolean
--- @field public TALENT_ElementalAffinity boolean
--- @field public TALENT_FiveStarRestaurant boolean
--- @field public TALENT_Bully boolean
--- @field public TALENT_ElementalRanger boolean
--- @field public TALENT_LightningRod boolean
--- @field public TALENT_Politician boolean
--- @field public TALENT_WeatherProof boolean
--- @field public TALENT_LoneWolf boolean
--- @field public TALENT_Zombie boolean
--- @field public TALENT_Demon boolean
--- @field public TALENT_IceKing boolean
--- @field public TALENT_Courageous boolean
--- @field public TALENT_GoldenMage boolean
--- @field public TALENT_WalkItOff boolean
--- @field public TALENT_FolkDancer boolean
--- @field public TALENT_SpillNoBlood boolean
--- @field public TALENT_Stench boolean
--- @field public TALENT_Kickstarter boolean
--- @field public TALENT_WarriorLoreNaturalArmor boolean
--- @field public TALENT_WarriorLoreNaturalHealth boolean
--- @field public TALENT_WarriorLoreNaturalResistance boolean
--- @field public TALENT_RangerLoreArrowRecover boolean
--- @field public TALENT_RangerLoreEvasionBonus boolean
--- @field public TALENT_RangerLoreRangedAPBonus boolean
--- @field public TALENT_RogueLoreDaggerAPBonus boolean
--- @field public TALENT_RogueLoreDaggerBackStab boolean
--- @field public TALENT_RogueLoreMovementBonus boolean
--- @field public TALENT_RogueLoreHoldResistance boolean
--- @field public TALENT_NoAttackOfOpportunity boolean
--- @field public TALENT_WarriorLoreGrenadeRange boolean
--- @field public TALENT_RogueLoreGrenadePrecision boolean
--- @field public TALENT_WandCharge boolean
--- @field public TALENT_DualWieldingDodging boolean
--- @field public TALENT_Human_Inventive boolean
--- @field public TALENT_Human_Civil boolean
--- @field public TALENT_Elf_Lore boolean
--- @field public TALENT_Elf_CorpseEating boolean
--- @field public TALENT_Dwarf_Sturdy boolean
--- @field public TALENT_Dwarf_Sneaking boolean
--- @field public TALENT_Lizard_Resistance boolean
--- @field public TALENT_Lizard_Persuasion boolean
--- @field public TALENT_Perfectionist boolean
--- @field public TALENT_Executioner boolean
--- @field public TALENT_ViolentMagic boolean
--- @field public TALENT_QuickStep boolean
--- @field public TALENT_Quest_SpidersKiss_Str boolean
--- @field public TALENT_Quest_SpidersKiss_Int boolean
--- @field public TALENT_Quest_SpidersKiss_Per boolean
--- @field public TALENT_Quest_SpidersKiss_Null boolean
--- @field public TALENT_Memory boolean
--- @field public TALENT_Quest_TradeSecrets boolean
--- @field public TALENT_Quest_GhostTree boolean
--- @field public TALENT_BeastMaster boolean
--- @field public TALENT_LivingArmor boolean
--- @field public TALENT_Torturer boolean
--- @field public TALENT_Ambidextrous boolean
--- @field public TALENT_Unstable boolean
--- @field public TALENT_ResurrectExtraHealth boolean
--- @field public TALENT_NaturalConductor boolean
--- @field public TALENT_Quest_Rooted boolean
--- @field public TALENT_PainDrinker boolean
--- @field public TALENT_DeathfogResistant boolean
--- @field public TALENT_Sourcerer boolean
--- @field public TALENT_Rager boolean
--- @field public TALENT_Elementalist boolean
--- @field public TALENT_Sadist boolean
--- @field public TALENT_Haymaker boolean
--- @field public TALENT_Gladiator boolean
--- @field public TALENT_Indomitable boolean
--- @field public TALENT_WildMag boolean
--- @field public TALENT_Jitterbug boolean
--- @field public TALENT_Soulcatcher boolean
--- @field public TALENT_MasterThief boolean
--- @field public TALENT_GreedyVessel boolean
--- @field public TALENT_MagicCycles boolean

-- Abilities
--- @field public WarriorLore integer
--- @field public RangerLore integer
--- @field public RogueLore integer
--- @field public SingleHanded integer
--- @field public TwoHanded integer
--- @field public PainReflection integer
--- @field public Ranged integer
--- @field public Shield integer
--- @field public Reflexes integer
--- @field public PhysicalArmorMastery integer
--- @field public MagicArmorMastery integer
--- @field public VitalityMastery integer
--- @field public Sourcery integer
--- @field public FireSpecialist integer
--- @field public WaterSpecialist integer
--- @field public AirSpecialist integer
--- @field public EarthSpecialist integer
--- @field public Necromancy integer
--- @field public Summoning integer
--- @field public Polymorph integer
--- @field public Telekinesis integer
--- @field public Repair integer
--- @field public Sneaking integer
--- @field public Pickpocket integer
--- @field public Thievery integer
--- @field public Loremaster integer
--- @field public Crafting integer
--- @field public Barter integer
--- @field public Charm integer
--- @field public Intimidate integer
--- @field public Reason integer
--- @field public Persuasion integer
--- @field public Leadership integer
--- @field public Luck integer
--- @field public DualWielding integer
--- @field public Wand integer
--- @field public Perseverance integer
--- @field public Runecrafting integer
--- @field public Brewmaster integer
--- @field public Sulfurology integer
local StatBase = {}


--- @class StatCharacter : StatBase
-- Properties from PropertyMap
--- @field public Level integer
--- @field public Name string
--- @field public AIFlags integer
--- @field public CurrentVitality integer
--- @field public CurrentArmor integer
--- @field public CurrentMagicArmor integer
--- @field public ArmorAfterHitCooldownMultiplier integer
--- @field public MagicArmorAfterHitCooldownMultiplier integer
--- @field public MPStart integer
--- @field public CurrentAP integer
--- @field public BonusActionPoints integer
--- @field public Experience integer
--- @field public Reputation integer
--- @field public Flanked integer
--- @field public Karma integer
--- @field public MaxResistance integer
--- @field public HasTwoHandedWeapon integer
--- @field public IsIncapacitatedRefCount integer
--- @field public MaxVitality integer
--- @field public BaseMaxVitality integer
--- @field public MaxArmor integer
--- @field public BaseMaxArmor integer
--- @field public MaxMagicArmor integer
--- @field public BaseMaxMagicArmor integer
--- @field public Sight number
--- @field public BaseSight number
--- @field public MaxSummons integer
--- @field public BaseMaxSummons integer
--- @field public MaxMpOverride integer

-- StatCharacterFlags
--- @field public IsPlayer boolean
--- @field public InParty boolean
--- @field public IsSneaking boolean
--- @field public Invisible boolean
--- @field public Blind boolean
--- @field public DrinkedPotion boolean
--- @field public EquipmentValidated boolean

-- Properties from CDivinityStats_Character::GetStat
--- @field public PhysicalResistance integer
--- @field public PiercingResistance integer
--- @field public CorrosiveResistance integer
--- @field public MagicResistance integer

-- Base properties from CDivinityStats_Character::GetStat
--- @field public BasePhysicalResistance integer
--- @field public BasePiercingResistance integer
--- @field public BaseCorrosiveResistance integer
--- @field public BaseMagicResistance integer

-- Properties from CharacterStatsGetters::GetStat
--- @field public MaxMp integer
--- @field public APStart integer
--- @field public APRecovery integer
--- @field public APMaximum integer
--- @field public Strength integer
--- @field public Finesse integer
--- @field public Intelligence integer
--- @field public Constitution integer
--- @field public Memory integer
--- @field public Wits integer
--- @field public Accuracy integer
--- @field public Dodge integer
--- @field public CriticalChance integer
--- @field public FireResistance integer
--- @field public EarthResistance integer
--- @field public WaterResistance integer
--- @field public AirResistance integer
--- @field public PoisonResistance integer
--- @field public ShadowResistance integer
--- @field public CustomResistance integer
--- @field public LifeSteal integer
--- @field public Hearing integer
--- @field public Movement integer
--- @field public Initiative integer
--- @field public BlockChance integer
--- @field public ChanceToHitBoost integer

-- Base properties from CharacterStatsGetters::GetStat
--- @field public BaseMaxMp integer
--- @field public BaseAPStart integer
--- @field public BaseAPRecovery integer
--- @field public BaseAPMaximum integer
--- @field public BaseStrength integer
--- @field public BaseFinesse integer
--- @field public BaseIntelligence integer
--- @field public BaseConstitution integer
--- @field public BaseMemory integer
--- @field public BaseWits integer
--- @field public BaseAccuracy integer
--- @field public BaseDodge integer
--- @field public BaseCriticalChance integer
--- @field public BaseFireResistance integer
--- @field public BaseEarthResistance integer
--- @field public BaseWaterResistance integer
--- @field public BaseAirResistance integer
--- @field public BasePoisonResistance integer
--- @field public BaseShadowResistance integer
--- @field public BaseCustomResistance integer
--- @field public BaseLifeSteal integer
--- @field public BaseHearing integer
--- @field public BaseMovement integer
--- @field public BaseInitiative integer
--- @field public BaseBlockChance integer
--- @field public BaseChanceToHitBoost integer

-- Properties from CharacterFetchStat
--- @field public DynamicStats StatCharacterDynamic[]
--- @field public MainWeapon StatItem
--- @field public OffHandWeapon StatItem
--- @field public DamageBoost integer
--- @field public Character EsvCharacter
--- @field public Rotation number[]
--- @field public Position number[]
--- @field public MyGuid string
--- @field public NetID integer
local StatCharacter = {
    --- Returns the item equipped in the specified slot
    --- @param self StatCharacter
    --- @param slot string See Itemslot enumeration
    --- @return StatItem|nil
    GetItemBySlot = function (self, slot) end
}


--- @class StatItemDynamic
--- @field public Durability integer
--- @field public DurabilityDegradeSpeed integer
--- @field public StrengthBoost integer
--- @field public FinesseBoost integer
--- @field public IntelligenceBoost integer
--- @field public ConstitutionBoost integer
--- @field public MemoryBoost integer
--- @field public WitsBoost integer
--- @field public SightBoost integer
--- @field public HearingBoost integer
--- @field public VitalityBoost integer
--- @field public SourcePointsBoost integer
--- @field public MaxAP integer
--- @field public StartAP integer
--- @field public APRecovery integer
--- @field public AccuracyBoost integer
--- @field public DodgeBoost integer
--- @field public LifeSteal integer
--- @field public CriticalChance integer
--- @field public ChanceToHitBoost integer
--- @field public MovementSpeedBoost integer
--- @field public RuneSlots integer
--- @field public RuneSlots_V1 integer
--- @field public FireResistance integer
--- @field public AirResistance integer
--- @field public WaterResistance integer
--- @field public EarthResistance integer
--- @field public PoisonResistance integer
--- @field public ShadowResistance integer
--- @field public PiercingResistance integer
--- @field public CorrosiveResistance integer
--- @field public PhysicalResistance integer
--- @field public MagicResistance integer
--- @field public CustomResistance integer
--- @field public Movement integer
--- @field public Initiative integer
--- @field public Willpower integer
--- @field public Bodybuilding integer
--- @field public MaxSummons integer
--- @field public Value integer
--- @field public Weight integer
--- @field public Skills string
--- @field public ItemColor string
--- @field public ModifierType integer
--- @field public ObjectInstanceName string
--- @field public BoostName string
--- @field public StatsType string See EquipmentStatsType enumeration
--- @field public DamageType integer Weapon only! - See DamageType enumeration
--- @field public MinDamage integer Weapon only!
--- @field public MaxDamage integer Weapon only!
--- @field public DamageBoost integer Weapon only!
--- @field public DamageFromBase integer Weapon only!
--- @field public CriticalDamage integer Weapon only!
--- @field public WeaponRange integer Weapon only!
--- @field public CleaveAngle integer Weapon only!
--- @field public CleavePercentage integer Weapon only!
--- @field public AttackAPCost integer Weapon only!
--- @field public ArmorValue integer Shield/Armor only!
--- @field public ArmorBoost integer Shield/Armor only!
--- @field public MagicArmorValue integer Shield/Armor only!
--- @field public MagicArmorBoost integer Shield/Armor only!
--- @field public Blocking integer Shield only!
-- TODO - add attribute flags object support
-- TODO - AbilityModifiers, Talents, Reflection
local StatItemDynamic = {}


--- @class StatItem : StatBase
-- Properties from property map
--- @field public Level integer
--- @field public Name integer
--- @field public InstanceId integer
--- @field public ItemType string See EquipmentStatsType enumeration
--- @field public ItemSlot integer
--- @field public WeaponType string See WeaponType enumeration
--- @field public AnimType integer TODO maybe an enum?
--- @field public WeaponRange integer
--- @field public IsIdentified integer
--- @field public IsTwoHanded boolean
--- @field public ShouldSyncStats boolean
--- @field public HasModifiedSkills boolean
--- @field public Skills string
--- @field public DamageTypeOverwrite string See DamageType enumeration
--- @field public Durability integer
--- @field public DurabilityCounter integer
--- @field public ItemTypeReal string
--- @field public MaxCharges integer
--- @field public Charges integer

-- Properties from ItemFetchStat
--- @field public DynamicStats StatItemDynamic[]

-- Indirect properties from StatEntryWeapon/Armor/Shield
--- @field public Using string
--- @field public Damage integer
--- @field public Act string See Act enumeration
--- @field public Handedness string See Handedness enumeration
--- @field public DamageBoost integer
--- @field public DamageFromBase integer
--- @field public CriticalDamage integer
--- @field public CriticalChance integer
--- @field public Movement integer
--- @field public Initiative integer
--- @field public Requirements StatRequirement[]
--- @field public Slot string See Itemslot enumeration
--- @field public DurabilityDegradeSpeed string See Qualifier enumeration
--- @field public Value integer
--- @field public ModifierType string See ModifierType enumeration
--- @field public Projectile string
--- @field public StrengthBoost string See Penalty Qualifier enumeration
--- @field public FinesseBoost string See Penalty Qualifier enumeration
--- @field public IntelligenceBoost string See Penalty Qualifier enumeration
--- @field public ConstitutionBoost string See Penalty Qualifier enumeration
--- @field public MemoryBoost string See Penalty Qualifier enumeration
--- @field public WitsBoost string See Penalty Qualifier enumeration
--- @field public SingleHanded integer
--- @field public TwoHanded integer
--- @field public Ranged integer
--- @field public DualWielding integer
--- @field public RogueLore integer
--- @field public WarriorLore integer
--- @field public RangerLore integer
--- @field public FireSpecialist integer
--- @field public WaterSpecialist integer
--- @field public AirSpecialist integer
--- @field public EarthSpecialist integer
--- @field public Sourcery integer
--- @field public Necromancy integer
--- @field public Polymorph integer
--- @field public Summoning integer
--- @field public Leadership integer
--- @field public PainReflection integer
--- @field public Perseverance integer
--- @field public Telekinesis integer
--- @field public Sneaking integer
--- @field public Thievery integer
--- @field public Loremaster integer
--- @field public Repair integer
--- @field public Barter integer
--- @field public Persuasion integer
--- @field public Luck integer
--- @field public Fire integer
--- @field public Earth integer
--- @field public Water integer
--- @field public Air integer
--- @field public Poison integer
--- @field public Physical integer
--- @field public Piercing integer
--- @field public SightBoost string See Penalty Qualifier enumeration
--- @field public HearingBoost string See Penalty Qualifier enumeration
--- @field public VitalityBoost integer
--- @field public MagicPointsBoost string See Penalty Qualifier enumeration
--- @field public ChanceToHitBoost integer
--- @field public APMaximum integer
--- @field public APStart integer
--- @field public APRecovery integer
--- @field public AccuracyBoost integer
--- @field public DodgeBoost integer
--- @field public Weight integer
--- @field public AttackAPCost integer
--- @field public ComboCategory string
--- @field public Flags string See AttributeFlags enumeration
--- @field public Boosts string
--- @field public InventoryTab string See InventoryTabs enumeration
--- @field public Reflection string
--- @field public ItemGroup string
--- @field public ObjectCategory string
--- @field public MinAmount integer
--- @field public MaxAmount integer
--- @field public Priority integer
--- @field public Unique integer
--- @field public MinLevel integer
--- @field public MaxLevel integer
--- @field public ItemColor string
--- @field public MaxSummons integer
--- @field public RuneSlots integer
--- @field public RuneSlots_V1 integer
--- @field public NeedsIdentification string See YesNo enumeration
--- @field public LifeSteal integer
--- @field public CleavePercentage integer
--- @field public CleaveAngle integer
--- @field public Talents string
--- @field public IgnoreVisionBlock string See YesNo enumeration
--- @field public Tags string
--- @field public ArmorBoost integer
--- @field public MagicArmorBoost integer
--- @field public Blocking integer
--- @field public ExtraProperties StatProperty[]
local StatItem = {}


--- @class EclGameObject
--- @field public NetID integer
--- @field public MyGuid string
-- TODO - Add more properties via virtual getters?
-- (IsGlobal, GetComponentType, GetFlags, GetTags, T/R/S, Velocity, Height, Level?)
local EclGameObject = {}


--- @class EclItem : EclGameObject
--- @field public RootTemplate ItemTemplate
--- @field public Stats StatItem
--- @field public Handle integer
---
--- @field public WorldPos number[]
--- @field public CurrentLevel string
--- @field public Scale number
--- @field public StatsId string
--- @field public Weight integer
--- @field public KeyName string
--- @field public Level integer
--- @field public ItemType string
--- @field public GoldValueOverride integer
--- @field public BaseWeightOverwrite integer
--- @field public ItemColorOverride integer
local EclItem = {
    --- Returns all delta mods on the item
    --- @param self EclItem
    --- @return string[]
    GetDeltaMods = function (self) end,
    --- Returns whether the item has the specified tag
    --- @param self EclItem
    --- @param tag string
    --- @return boolean
    HasTag = function (self, tag) end,
    --- Returns all tags on the item
    --- @param self EclItem
    --- @return string[]
    GetTags = function (self) end,
    --- Returns the first status with the specified status ID, if one exists.
    --- @param self EclItem
    --- @param statusId string Status ID
    --- @return EclStatus|nil
    GetStatus = function (self, statusId) end,
    --- Returns the first status with the specified engine status type, if one exists.
    --- @param self EclItem
    --- @param type string Status type
    --- @return EclStatus|nil
    GetStatusByType = function (self, type) end,
    --- Returns all statuses on the item
    --- @param self EclItem
    --- @return string[]
    GetStatuses = function (self) end
}


--- @class EclCharacter : EclGameObject
--- @field public RootTemplate CharacterTemplate
--- @field public PlayerCustomData PlayerCustomData
--- @field public Stats StatCharacter
--- @field public Handle integer
---
--- @field public WorldPos number[]
--- @field public CurrentLevel string
--- @field public Scale number
--- @field public AnimationOverride string
--- @field public UserID integer
local EclCharacter = {
    --- Returns whether the character has the specified tag
    --- @param self EclCharacter
    --- @param tag string
    --- @return boolean
    HasTag = function (self, tag) end,
    --- Returns all tags on the character
    --- @param self EclCharacter
    --- @return string[]
    GetTags = function (self) end,
    --- Returns the first status with the specified status ID, if one exists.
    --- @param self EclCharacter
    --- @param statusId string Status ID
    --- @return EclStatus|nil
    GetStatus = function (self, statusId) end,
    --- Returns the first status with the specified engine status type, if one exists.
    --- @param self EclCharacter
    --- @param type string Status type
    --- @return EclStatus|nil
    GetStatusByType = function (self, type) end,
    --- Returns all statuses on the character
    --- @param self EclCharacter
    --- @return string[]
    GetStatuses = function (self) end,
    --- Update model scale of the character.
    --- @param self EclCharacter
    --- @param scale number 
    SetScale = function (self, scale) end
}




--- @class EsvGameObject
--- @field public NetID integer
--- @field public MyGuid string
-- TODO - Add more properties via virtual getters?
-- (IsGlobal, GetComponentType, GetFlags, GetTags, T/R/S, Velocity, Height, Level?)
local EsvGameObject = {}


--- @class EsvItem : EsvGameObject
--- @field public RootTemplate ItemTemplate
--- @field public WorldPos number[]
--- @field public CurrentLevel string
--- @field public Scale number
--- @field public CustomDisplayName string
--- @field public CustomDescription string
--- @field public CustomBookContent string
--- @field public StatsId string
--- @field public InventoryHandle integer
--- @field public ParentInventoryHandle integer
--- @field public Slot integer
--- @field public Amount integer
--- @field public Vitality integer
--- @field public Armor integer
--- @field public InUseByCharacterHandle integer
--- @field public Key string
--- @field public LockLevel integer
--- @field public OwnerHandle integer
--- @field public ItemType string
--- @field public GoldValueOverwrite integer
--- @field public WeightValueOverwrite integer
--- @field public TreasureLevel integer
--- @field public LevelOverride integer
--- @field public ForceSynch boolean
--- @field public Stats StatItem
local EsvItem = {
    --- Returns all delta mods on the item
    --- @param self EsvItem
    --- @return string[]
    GetDeltaMods = function (self) end,
    --- Returns all boosts that were added during treasure generation
    --- @param self EsvItem
    --- @return string[]
    GetGeneratedBoosts = function (self) end,
    --- Returns the GUID of all items within the inventory of the item
    --- @param self EsvItem
    --- @return string[]
    GetInventoryItems = function (self) end,
    --- Returns the GUID of all characters within the specified radius
    --- @param self EsvItem
    --- @return string[]
    GetNearbyCharacters = function (self, radius) end,
    --- Returns whether the item has the specified tag
    --- @param self EsvItem
    --- @param tag string
    --- @return boolean
    HasTag = function (self, tag) end,
    --- Returns all tags on the item
    --- @param self EsvItem
    --- @return string[]
    GetTags = function (self) end,
    --- Returns the first status with the specified status ID, if one exists.
    --- @param self EsvItem
    --- @param statusId string Status ID
    --- @return EsvStatus|nil
    GetStatus = function (self, statusId) end,
    --- Returns the first status with the specified engine status type, if one exists.
    --- @param self EsvItem
    --- @param type string Status type
    --- @return EsvStatus|nil
    GetStatusByType = function (self, type) end,
    --- Returns all statuses on the item
    --- @param self EsvItem
    --- @return string[]
    GetStatuses = function (self) end
}


--- @class PlayerCustomData
--- @field public CustomLookEnabled boolean
--- @field public Name string
--- @field public ClassType string
--- @field public SkinColor integer
--- @field public HairColor integer
--- @field public ClothColor1 integer
--- @field public ClothColor2 integer
--- @field public ClothColor3 integer
--- @field public IsMale boolean
--- @field public Race string
--- @field public OriginName string
--- @field public Icon string
--- @field public MusicInstrument string
--- @field public OwnerProfileID string
--- @field public ReservedProfileID string
--- @field public AiPersonality string
--- @field public Speaker string
local PlayerCustomData = {
}


--- @class EsvSkillInfo
--- @field public ActiveCooldown number
--- @field public IsActivated boolean
--- @field public IsLearned boolean
--- @field public ZeroMemory boolean
--- @field public OncePerCombat boolean
--- @field public NumCharges number
local EsvSkillInfo = {}


--- @class EsvCharacter : EsvGameObject
--- @field public RootTemplate CharacterTemplate
--- @field public PlayerCustomData PlayerCustomData
--- @field public Stats StatCharacter
---
--- @field public NetID integer
--- @field public MyGuid string
--- @field public WorldPos number[]
--- @field public CurrentLevel string
--- @field public Scale number
--- @field public AnimationOverride string
--- @field public WalkSpeedOverride number
--- @field public RunSpeedOverride number
--- @field public NeedsUpdateCount integer
--- @field public ScriptForceUpdateCount integer
--- @field public ForceSynchCount integer
--- @field public InventoryHandle integer Not useful yet as you can't use inventory handles for any API
--- @field public SkillBeingPrepared string Set while preparing a skill, cleared afterwards
--- @field public LifeTime number Used for summons to indicate remaining lifetime
--- @field public TurnTimer number
--- @field public TriggerTrapsTimer number
--- @field public UserID integer
--- @field public OwnerHandle integer Handle of owner character (in case of summons/followers)
--- @field public FollowCharacterHandle integer
--- @field public SpiritCharacterHandle integer
--- @field public CorpseCharacterHandle integer
--- @field public PartialAP number Movement AP
--- @field public AnimType integer
--- @field public DelayDeathCount integer
--- @field public AnimationSetOverride string
--- @field public OriginalTransformDisplayName string
--- @field public PartyHandle integer
--- @field public CustomTradeTreasure string
--- @field public IsAlarmed boolean
--- @field public CrimeWarningsEnabled boolean
--- @field public CrimeInterrogationEnabled boolean
--- @field public MovingCasterHandle integer
--- @field public Archetype string
--- @field public EquipmentColor string
--- @field public ProjectileTemplate string
--- @field public ReadyCheckBlocked boolean
--- @field public CorpseLootable boolean
--- @field public CustomBloodSurface string
--- @field public PreviousLevel string

-- CharacterFlags 0
--- @field public IsPlayer boolean
--- @field public Multiplayer boolean
--- @field public InParty boolean
--- @field public HostControl boolean
--- @field public OffStage boolean
--- @field public Dead boolean
--- @field public HasOwner boolean
--- @field public InDialog boolean
--- @field public Summon boolean
--- @field public CharacterControl boolean
--- @field public Loaded boolean
--- @field public InArena boolean
--- @field public CharacterCreationFinished boolean
--- @field public Floating boolean
--- @field public SpotSneakers boolean
--- @field public WalkThrough boolean
--- @field public CoverAmount boolean
--- @field public CanShootThrough boolean
--- @field public PartyFollower boolean
--- @field public Totem boolean
--- @field public NoRotate boolean
--- @field public IsHuge boolean

-- CharacterFlags 2
--- @field public Global boolean
--- @field public HasOsirisDialog boolean
--- @field public HasDefaultDialog boolean
--- @field public TreasureGeneratedForTrader boolean
--- @field public Trader boolean
--- @field public Resurrected boolean

-- CharacterFlags 3
--- @field public IsPet boolean
--- @field public IsSpectating boolean
--- @field public NoReptuationEffects boolean
--- @field public HasWalkSpeedOverride boolean
--- @field public HasRunSpeedOverride boolean
--- @field public IsGameMaster boolean
--- @field public IsPossessed boolean
local EsvCharacter = {
    --- Returns the GUID of all items in the characters inventory
    --- @param self EsvCharacter
    --- @return string[]
    GetInventoryItems = function (self) end,
    --- Returns detailed information about the specified skill
    --- @param self EsvCharacter
    --- @param skillId string
    --- @return EsvSkillInfo
    GetSkillInfo = function (self, skillId) end,
    --- Returns the name of all skills available to the character
    --- @param self EsvCharacter
    --- @return string[]
    GetSkills = function (self) end,
    --- Returns the GUID of all characters within the specified radius
    --- @param self EsvCharacter
    --- @return string[]
    GetNearbyCharacters = function (self, radius) end,
    --- Returns whether the character has the specified tag
    --- @param self EsvCharacter
    --- @param tag string
    --- @return boolean
    HasTag = function (self, tag) end,
    --- Returns all tags on the character
    --- @param self EsvCharacter
    --- @return string[]
    GetTags = function (self) end,
    --- Returns the first status with the specified status ID, if one exists.
    --- @param self EsvCharacter
    --- @param statusId string Status ID
    --- @return EsvStatus|nil
    GetStatus = function (self, statusId) end,
    --- Returns the first status with the specified engine status type, if one exists.
    --- @param self EsvCharacter
    --- @param type string Status type
    --- @return EsvStatus|nil
    GetStatusByType = function (self, type) end,
    --- Returns all statuses on the character
    --- @param self EsvCharacter
    --- @return string[]
    GetStatuses = function (self) end,
    --- Update model scale of the character.
    --- NOTE: This change must be manually synchronized to the client!
    --- @param self EsvCharacter
    --- @param scale number 
    SetScale = function (self, scale) end
}


--- @class EsvProjectile : EsvGameObject
--- @field public RootTemplate ProjectileTemplate
--- @field public NetID integer
--- @field public MyGuid string
--- @field public CasterHandle integer
--- @field public SourceHandle integer
--- @field public TargetObjectHandle integer
--- @field public HitObjectHandle integer
--- @field public SourcePosition number[]
--- @field public TargetPosition number[]
--- @field public DamageType string
--- @field public DamageSourceType string
--- @field public LifeTime number
--- @field public HitInterpolation integer
--- @field public ExplodeRadius0 number
--- @field public ExplodeRadius1 number
--- @field public DeathType string
--- @field public SkillId string
--- @field public WeaponHandle integer
--- @field public MovingEffectHandle integer
--- @field public SpawnEffect string
--- @field public SpawnFXOverridesImpactFX boolean
--- @field public EffectHandle string
--- @field public RequestDelete boolean
--- @field public Launched boolean
--- @field public IsTrap boolean
--- @field public UseCharacterStats boolean
--- @field public ReduceDurability boolean
--- @field public AlwaysDamage boolean
--- @field public ForceTarget boolean
--- @field public IsFromItem boolean
--- @field public DivideDamage boolean
--- @field public IgnoreRoof boolean
--- @field public CanDeflect boolean
--- @field public IgnoreObjects boolean
--- @field public CleanseStatuses string
--- @field public StatusClearChance integer
--- @field public Position number[]
--- @field public PrevPosition number[]
--- @field public Velocity number[]
--- @field public Scale number
--- @field public CurrentLevel string
local EsvProjectile = {}

--- @class ModInfo
--- @field public UUID string
--- @field public Name string
--- @field public Version integer
--- @field public PublishVersion integer
--- @field public Directory string
--- @field public Author string
--- @field public Description string
--- @field public ModuleType string
--- @field public Dependencies string[] Dependency mod UUID-s
local ModInfo = {}

--- @class DeltaModBoost
--- @field public Boost string
--- @field public Count integer
local DeltaModBoost = {}

--- @class DeltaMod
--- @field public ModifierType string
--- @field public SlotType string
--- @field public WeaponType string
--- @field public ArmorType string
--- @field public Handedness string
--- @field public Name string
--- @field public BoostType string
--- @field public MinLevel integer
--- @field public MaxLevel integer
--- @field public Frequency integer
--- @field public Boosts DeltaModBoost[]
local DeltaMod = {}


--- @class StatRequirement
--- @field public Requirement string
--- @field public Param string|integer
--- @field public Not boolean
local StatRequirement = {}


--- @class StatEntryWeapon
--- @field public Name string
--- @field public Level integer
--- @field public Using string
--- @field public Damage integer
--- @field public Act string See Act enumeration
--- @field public Handedness string See Handedness enumeration
--- @field public IsTwoHanded string See YesNo enumeration
--- @field public DamageBoost integer
--- @field public DamageFromBase integer
--- @field public CriticalDamage integer
--- @field public CriticalChance integer
--- @field public Movement integer
--- @field public Initiative integer
--- @field public Requirements StatRequirement[]
--- @field public Slot string See Itemslot enumeration
--- @field public Durability integer
--- @field public DurabilityDegradeSpeed string See Qualifier enumeration
--- @field public Value integer
--- @field public WeaponType string See WeaponType enumeration
--- @field public AnimType string See AnimType enumeration
--- @field public WeaponRange integer
--- @field public ModifierType string See ModifierType enumeration
--- @field public Projectile string
--- @field public StrengthBoost string See Penalty Qualifier enumeration
--- @field public FinesseBoost string See Penalty Qualifier enumeration
--- @field public IntelligenceBoost string See Penalty Qualifier enumeration
--- @field public ConstitutionBoost string See Penalty Qualifier enumeration
--- @field public MemoryBoost string See Penalty Qualifier enumeration
--- @field public WitsBoost string See Penalty Qualifier enumeration
--- @field public SingleHanded integer
--- @field public TwoHanded integer
--- @field public Ranged integer
--- @field public DualWielding integer
--- @field public RogueLore integer
--- @field public WarriorLore integer
--- @field public RangerLore integer
--- @field public FireSpecialist integer
--- @field public WaterSpecialist integer
--- @field public AirSpecialist integer
--- @field public EarthSpecialist integer
--- @field public Sourcery integer
--- @field public Necromancy integer
--- @field public Polymorph integer
--- @field public Summoning integer
--- @field public Leadership integer
--- @field public PainReflection integer
--- @field public Perseverance integer
--- @field public Telekinesis integer
--- @field public Sneaking integer
--- @field public Thievery integer
--- @field public Loremaster integer
--- @field public Repair integer
--- @field public Barter integer
--- @field public Persuasion integer
--- @field public Luck integer
--- @field public Fire integer
--- @field public Earth integer
--- @field public Water integer
--- @field public Air integer
--- @field public Poison integer
--- @field public Physical integer
--- @field public Piercing integer
--- @field public SightBoost string See Penalty Qualifier enumeration
--- @field public HearingBoost string See Penalty Qualifier enumeration
--- @field public VitalityBoost integer
--- @field public MagicPointsBoost string See Penalty Qualifier enumeration
--- @field public ChanceToHitBoost integer
--- @field public APMaximum integer
--- @field public APStart integer
--- @field public APRecovery integer
--- @field public AccuracyBoost integer
--- @field public DodgeBoost integer
--- @field public Weight integer
--- @field public AttackAPCost integer
--- @field public ComboCategory string
--- @field public Flags string See AttributeFlags enumeration
--- @field public Boosts string
--- @field public InventoryTab string See InventoryTabs enumeration
--- @field public Charges integer
--- @field public MaxCharges integer
--- @field public Skills string
--- @field public Reflection string
--- @field public ItemGroup string
--- @field public ObjectCategory string
--- @field public MinAmount integer
--- @field public MaxAmount integer
--- @field public Priority integer
--- @field public Unique integer
--- @field public MinLevel integer
--- @field public MaxLevel integer
--- @field public ItemColor string
--- @field public MaxSummons integer
--- @field public RuneSlots integer
--- @field public RuneSlots_V1 integer
--- @field public NeedsIdentification string See YesNo enumeration
--- @field public LifeSteal integer
--- @field public CleavePercentage integer
--- @field public CleaveAngle integer
--- @field public Talents string
--- @field public IgnoreVisionBlock string See YesNo enumeration
--- @field public Tags string
--- @field public ExtraProperties StatProperty[]
local StatEntryWeapon = {
    --- @type string See Damage Type enumeration
    ['Damage Type'] = "",
    --- @type integer
    ['Damage Range'] = 0,
    --- @type string See ActPart enumeration
    ['Act part'] = ""
}


--- @class StatEntryArmor
--- @field public Name string
--- @field public Level integer
--- @field public Using string
--- @field public ArmorBoost integer
--- @field public MagicArmorBoost integer
--- @field public Movement integer
--- @field public Initiative integer
--- @field public Requirements StatRequirement[]
--- @field public Slot string See Itemslot enumeration
--- @field public Durability integer
--- @field public DurabilityDegradeSpeed string See Qualifier enumeration
--- @field public Value integer
--- @field public ModifierType string See ModifierType enumeration
--- @field public Act string See Act enumeration
--- @field public Fire integer
--- @field public Air integer
--- @field public Water integer
--- @field public Earth integer
--- @field public Poison integer
--- @field public Piercing integer
--- @field public Physical integer
--- @field public StrengthBoost string See Penalty Qualifier enumeration
--- @field public FinesseBoost string See Penalty Qualifier enumeration
--- @field public IntelligenceBoost string See Penalty Qualifier enumeration
--- @field public ConstitutionBoost string See Penalty Qualifier enumeration
--- @field public MemoryBoost string See Penalty Qualifier enumeration
--- @field public WitsBoost string See Penalty Qualifier enumeration
--- @field public SingleHanded integer
--- @field public TwoHanded integer
--- @field public Ranged integer
--- @field public DualWielding integer
--- @field public RogueLore integer
--- @field public WarriorLore integer
--- @field public RangerLore integer
--- @field public FireSpecialist integer
--- @field public WaterSpecialist integer
--- @field public AirSpecialist integer
--- @field public EarthSpecialist integer
--- @field public Sourcery integer
--- @field public Necromancy integer
--- @field public Polymorph integer
--- @field public Summoning integer
--- @field public PainReflection integer
--- @field public Perseverance integer
--- @field public Leadership integer
--- @field public Telekinesis integer
--- @field public Sneaking integer
--- @field public Thievery integer
--- @field public Loremaster integer
--- @field public Repair integer
--- @field public Barter integer
--- @field public Persuasion integer
--- @field public Luck integer
--- @field public SightBoost string See Penalty Qualifier enumeration
--- @field public HearingBoost string See Penalty Qualifier enumeration
--- @field public VitalityBoost integer
--- @field public MagicPointsBoost string See Penalty Qualifier enumeration
--- @field public ChanceToHitBoost integer
--- @field public APMaximum integer
--- @field public APStart integer
--- @field public APRecovery integer
--- @field public AccuracyBoost integer
--- @field public DodgeBoost integer
--- @field public CriticalChance integer
--- @field public ComboCategory string
--- @field public Weight integer
--- @field public InventoryTab string See InventoryTabs enumeration
--- @field public Flags string See AttributeFlags enumeration
--- @field public ArmorType string See ArmorType enumeration
--- @field public Boosts string
--- @field public Skills string
--- @field public ItemColor string
--- @field public Reflection string
--- @field public ItemGroup string
--- @field public ObjectCategory string
--- @field public MinAmount integer
--- @field public MaxAmount integer
--- @field public Priority integer
--- @field public Unique integer
--- @field public MinLevel integer
--- @field public MaxLevel integer
--- @field public MaxSummons integer
--- @field public NeedsIdentification string See YesNo enumeration
--- @field public Charges integer
--- @field public RuneSlots integer
--- @field public RuneSlots_V1 integer
--- @field public MaxCharges integer
--- @field public Talents string
--- @field public Tags string
--- @field public ExtraProperties StatProperty[]
local StatEntryArmor = {
    ['Armor Defense Value'] = 0,
    ['Magic Armor Value'] = 0,
    --- @type string See ActPart enumeration
    ['Act part'] = ""
}


--- @class StatEntryShield
--- @field public Name string
--- @field public Level integer
--- @field public Using string
--- @field public ArmorBoost integer
--- @field public MagicArmorBoost integer
--- @field public Movement integer
--- @field public Initiative integer
--- @field public Requirements StatRequirement[]
--- @field public Slot string See Itemslot enumeration
--- @field public Durability integer
--- @field public DurabilityDegradeSpeed string See Qualifier enumeration
--- @field public Value integer
--- @field public ModifierType string See ModifierType enumeration
--- @field public Act string See Act enumeration
--- @field public Fire integer
--- @field public Air integer
--- @field public Water integer
--- @field public Earth integer
--- @field public Poison integer
--- @field public Piercing integer
--- @field public Physical integer
--- @field public Blocking integer
--- @field public StrengthBoost string See Penalty Qualifier enumeration
--- @field public FinesseBoost string See Penalty Qualifier enumeration
--- @field public IntelligenceBoost string See Penalty Qualifier enumeration
--- @field public ConstitutionBoost string See Penalty Qualifier enumeration
--- @field public MemoryBoost string See Penalty Qualifier enumeration
--- @field public WitsBoost string See Penalty Qualifier enumeration
--- @field public SingleHanded integer
--- @field public TwoHanded integer
--- @field public Ranged integer
--- @field public DualWielding integer
--- @field public RogueLore integer
--- @field public WarriorLore integer
--- @field public RangerLore integer
--- @field public FireSpecialist integer
--- @field public WaterSpecialist integer
--- @field public AirSpecialist integer
--- @field public EarthSpecialist integer
--- @field public Sourcery integer
--- @field public Necromancy integer
--- @field public Polymorph integer
--- @field public Summoning integer
--- @field public Leadership integer
--- @field public PainReflection integer
--- @field public Perseverance integer
--- @field public Telekinesis integer
--- @field public Sneaking integer
--- @field public Thievery integer
--- @field public Loremaster integer
--- @field public Repair integer
--- @field public Barter integer
--- @field public Persuasion integer
--- @field public Luck integer
--- @field public SightBoost string See Penalty Qualifier enumeration
--- @field public HearingBoost string See Penalty Qualifier enumeration
--- @field public VitalityBoost integer
--- @field public MagicPointsBoost string See Penalty Qualifier enumeration
--- @field public ChanceToHitBoost integer
--- @field public APMaximum integer
--- @field public APStart integer
--- @field public APRecovery integer
--- @field public AccuracyBoost integer
--- @field public DodgeBoost integer
--- @field public CriticalChance string See Penalty Qualifier enumeration
--- @field public ComboCategory string
--- @field public Weight integer
--- @field public InventoryTab string See InventoryTabs enumeration
--- @field public Flags string See AttributeFlags enumeration
--- @field public Skills string
--- @field public Reflection string
--- @field public ItemGroup string
--- @field public ObjectCategory string
--- @field public MinAmount integer
--- @field public MaxAmount integer
--- @field public Priority integer
--- @field public Unique integer
--- @field public MinLevel integer
--- @field public MaxLevel integer
--- @field public ItemColor string
--- @field public MaxSummons integer
--- @field public RuneSlots integer
--- @field public RuneSlots_V1 integer
--- @field public NeedsIdentification string See YesNo enumeration
--- @field public Talents string
--- @field public Tags string
--- @field public ExtraProperties StatProperty[]
local StatEntryShield = {
    ['Armor Defense Value'] = 0,
    ['Magic Armor Value'] = 0,
    --- @type string See ActPart enumeration
    ['Act part'] = ""
}


--- @class StatEntryPotion
--- @field public Name string
--- @field public Level integer
--- @field public Using string
--- @field public ModifierType string See ModifierType enumeration
--- @field public VitalityBoost integer
--- @field public Strength string See Penalty PreciseQualifier enumeration
--- @field public Finesse string See Penalty PreciseQualifier enumeration
--- @field public Intelligence string See Penalty PreciseQualifier enumeration
--- @field public Constitution string See Penalty PreciseQualifier enumeration
--- @field public Memory string See Penalty PreciseQualifier enumeration
--- @field public Wits string See Penalty PreciseQualifier enumeration
--- @field public SingleHanded integer
--- @field public TwoHanded integer
--- @field public Ranged integer
--- @field public DualWielding integer
--- @field public RogueLore integer
--- @field public WarriorLore integer
--- @field public RangerLore integer
--- @field public FireSpecialist integer
--- @field public WaterSpecialist integer
--- @field public AirSpecialist integer
--- @field public EarthSpecialist integer
--- @field public Sourcery integer
--- @field public Necromancy integer
--- @field public Polymorph integer
--- @field public Summoning integer
--- @field public PainReflection integer
--- @field public Perseverance integer
--- @field public Leadership integer
--- @field public Telekinesis integer
--- @field public Sneaking integer
--- @field public Thievery integer
--- @field public Loremaster integer
--- @field public Repair integer
--- @field public Barter integer
--- @field public Persuasion integer
--- @field public Luck integer
--- @field public FireResistance integer
--- @field public EarthResistance integer
--- @field public WaterResistance integer
--- @field public AirResistance integer
--- @field public PoisonResistance integer
--- @field public PhysicalResistance integer
--- @field public PiercingResistance integer
--- @field public Sight integer
--- @field public Hearing string See Penalty Qualifier enumeration
--- @field public Initiative integer
--- @field public Vitality integer
--- @field public VitalityPercentage integer
--- @field public MagicPoints integer
--- @field public ActionPoints integer
--- @field public ChanceToHitBoost integer
--- @field public AccuracyBoost integer
--- @field public DodgeBoost integer
--- @field public DamageBoost integer
--- @field public APCostBoost integer
--- @field public SPCostBoost integer
--- @field public APMaximum integer
--- @field public APStart integer
--- @field public APRecovery integer
--- @field public Movement integer
--- @field public MovementSpeedBoost integer
--- @field public Gain string See BigQualifier enumeration
--- @field public Armor integer
--- @field public MagicArmor integer
--- @field public ArmorBoost integer
--- @field public MagicArmorBoost integer
--- @field public CriticalChance integer
--- @field public Act string See Act enumeration
--- @field public Duration integer
--- @field public UseAPCost integer
--- @field public ComboCategory string
--- @field public StackId string
--- @field public BoostConditions string
--- @field public Flags string See AttributeFlags enumeration
--- @field public StatusMaterial string
--- @field public StatusEffect string
--- @field public StatusIcon string
--- @field public SavingThrow string See SavingThrow enumeration
--- @field public Weight integer
--- @field public Value integer
--- @field public InventoryTab string See InventoryTabs enumeration
--- @field public UnknownBeforeConsume string See YesNo enumeration
--- @field public Reflection string
--- @field public Damage string See Qualifier enumeration
--- @field public DamageType string See Damage Type enumeration
--- @field public AuraRadius integer
--- @field public AuraSelf string
--- @field public AuraAllies string
--- @field public AuraEnemies string
--- @field public AuraNeutrals string
--- @field public AuraItems string
--- @field public AuraFX string
--- @field public RootTemplate string
--- @field public ObjectCategory string
--- @field public MinAmount integer
--- @field public MaxAmount integer
--- @field public Priority integer
--- @field public Unique integer
--- @field public MinLevel integer
--- @field public MaxLevel integer
--- @field public BloodSurfaceType string
--- @field public MaxSummons integer
--- @field public AddToBottomBar string See YesNo enumeration
--- @field public SummonLifelinkModifier integer
--- @field public IgnoredByAI string See YesNo enumeration
--- @field public RangeBoost integer
--- @field public BonusWeapon string
--- @field public AiCalculationStatsOverride string
--- @field public RuneEffectWeapon string
--- @field public RuneEffectUpperbody string
--- @field public RuneEffectAmulet string
--- @field public RuneLevel integer
--- @field public LifeSteal integer
--- @field public IsFood string See YesNo enumeration
--- @field public IsConsumable string See YesNo enumeration
--- @field public ExtraProperties StatProperty[]
local StatEntryPotion = {
    ['Damage Multiplier'] = 0,
    ['Damage Range'] = 0,
    --- @type string See ActPart enumeration
    ['Act part'] = ""
}


--- @class StatEntryObject
--- @field public Name string
--- @field public Level integer
--- @field public Using string
--- @field public ModifierType string See ModifierType enumeration
--- @field public Act string See Act enumeration
--- @field public UseAPCost integer
--- @field public Value integer
--- @field public ComboCategory string
--- @field public Weight integer
--- @field public Strength string See PreciseQualifier enumeration
--- @field public Finesse string See PreciseQualifier enumeration
--- @field public Intelligence string See PreciseQualifier enumeration
--- @field public Constitution string See PreciseQualifier enumeration
--- @field public Memory string See PreciseQualifier enumeration
--- @field public Wits string See PreciseQualifier enumeration
--- @field public Vitality integer
--- @field public Armor string See PreciseQualifier enumeration
--- @field public FireResistance integer
--- @field public EarthResistance integer
--- @field public WaterResistance integer
--- @field public AirResistance integer
--- @field public PoisonResistance integer
--- @field public PiercingResistance integer
--- @field public PhysicalResistance integer
--- @field public ShadowResistance integer
--- @field public Flags string See AttributeFlags enumeration
--- @field public Requirements StatRequirement[]
--- @field public InventoryTab string See InventoryTabs enumeration
--- @field public RootTemplate string
--- @field public ObjectCategory string
--- @field public MinAmount integer
--- @field public MaxAmount integer
--- @field public Priority integer
--- @field public Unique integer
--- @field public MinLevel integer
--- @field public RuneEffectWeapon string
--- @field public RuneEffectUpperbody string
--- @field public RuneEffectAmulet string
--- @field public RuneLevel integer
--- @field public MaxLevel integer
--- @field public AddToBottomBar string See YesNo enumeration
--- @field public IgnoredByAI string See YesNo enumeration
local StatEntryObject = {
    --- @type string See ActPart enumeration
    ['Act part'] = ""
}


--- @class StatEntryCharacter
--- @field public Name string
--- @field public Level integer
--- @field public Using string
--- @field public Strength string See PreciseQualifier enumeration
--- @field public Finesse string See PreciseQualifier enumeration
--- @field public Intelligence string See PreciseQualifier enumeration
--- @field public Constitution string See PreciseQualifier enumeration
--- @field public Memory string See PreciseQualifier enumeration
--- @field public Wits string See PreciseQualifier enumeration
--- @field public SingleHanded integer
--- @field public TwoHanded integer
--- @field public Ranged integer
--- @field public DualWielding integer
--- @field public RogueLore integer
--- @field public WarriorLore integer
--- @field public RangerLore integer
--- @field public FireSpecialist integer
--- @field public WaterSpecialist integer
--- @field public AirSpecialist integer
--- @field public EarthSpecialist integer
--- @field public Sourcery integer
--- @field public Necromancy integer
--- @field public Polymorph integer
--- @field public Summoning integer
--- @field public PainReflection integer
--- @field public Leadership integer
--- @field public Perseverance integer
--- @field public Telekinesis integer
--- @field public Sneaking integer
--- @field public Thievery integer
--- @field public Loremaster integer
--- @field public Repair integer
--- @field public Barter integer
--- @field public Persuasion integer
--- @field public Luck integer
--- @field public FireResistance integer
--- @field public EarthResistance integer
--- @field public WaterResistance integer
--- @field public AirResistance integer
--- @field public PoisonResistance integer
--- @field public PiercingResistance integer
--- @field public PhysicalResistance integer
--- @field public Sight string See Penalty Qualifier enumeration
--- @field public Hearing string See Penalty Qualifier enumeration
--- @field public FOV integer
--- @field public APMaximum integer
--- @field public APStart integer
--- @field public APRecovery integer
--- @field public Initiative integer
--- @field public Vitality integer
--- @field public MagicPoints integer
--- @field public ChanceToHitBoost integer
--- @field public Movement integer
--- @field public MovementSpeedBoost integer
--- @field public CriticalChance string See Penalty Qualifier enumeration
--- @field public Gain string See BigQualifier enumeration
--- @field public Armor integer
--- @field public ArmorBoost integer
--- @field public ArmorBoostGrowthPerLevel integer
--- @field public MagicArmor integer
--- @field public MagicArmorBoost integer
--- @field public MagicArmorBoostGrowthPerLevel integer
--- @field public Accuracy integer
--- @field public Dodge integer
--- @field public Act string See Act enumeration
--- @field public MaxResistance integer
--- @field public Weight integer
--- @field public Talents string
--- @field public Traits string
--- @field public PathInfluence string
--- @field public Flags string See AttributeFlags enumeration
--- @field public Reflection string
--- @field public StepsType string See StepsType enumeration
--- @field public MaxSummons integer
--- @field public MPStart integer
--- @field public DamageBoost integer
--- @field public DamageBoostGrowthPerLevel integer
local StatEntryCharacter = {
    --- @type string See ActPart enumeration
    ['Act part'] = "",
    --- @type string See Penalty Qualifier enumeration
    ['Act strength'] = ""
}


--- @class StatEntrySkillData
--- @field public Name string
--- @field public Level integer
--- @field public Using string
--- @field public SkillType string
--- @field public Ability string See SkillAbility enumeration
--- @field public Element string See SkillElement enumeration
--- @field public Requirement string See SkillRequirement enumeration
--- @field public Requirements StatRequirement[]
--- @field public DisplayName string
--- @field public DisplayNameRef string
--- @field public Description string
--- @field public DescriptionRef string
--- @field public StatsDescription string
--- @field public StatsDescriptionRef string
--- @field public StatsDescriptionParams string
--- @field public Icon string
--- @field public FXScale integer
--- @field public PrepareAnimationInit string
--- @field public PrepareAnimationLoop string
--- @field public PrepareEffect string
--- @field public PrepareEffectBone string
--- @field public CastAnimation string
--- @field public CastTextEvent string
--- @field public CastAnimationCheck string See CastCheckType enumeration
--- @field public CastEffect string
--- @field public CastEffectTextEvent string
--- @field public TargetCastEffect string
--- @field public TargetHitEffect string
--- @field public TargetEffect string
--- @field public SourceTargetEffect string
--- @field public TargetTargetEffect string
--- @field public LandingEffect string
--- @field public ImpactEffect string
--- @field public MaleImpactEffects string
--- @field public FemaleImpactEffects string
--- @field public OnHitEffect string
--- @field public SelectedCharacterEffect string
--- @field public SelectedObjectEffect string
--- @field public SelectedPositionEffect string
--- @field public DisappearEffect string
--- @field public ReappearEffect string
--- @field public ReappearEffectTextEvent string
--- @field public RainEffect string
--- @field public StormEffect string
--- @field public FlyEffect string
--- @field public SpatterEffect string
--- @field public ShieldMaterial string
--- @field public ShieldEffect string
--- @field public ContinueEffect string
--- @field public SkillEffect string
--- @field public Template string
--- @field public TemplateCheck string See CastCheckType enumeration
--- @field public TemplateOverride string
--- @field public TemplateAdvanced string
--- @field public Totem string See YesNo enumeration
--- @field public Template1 string
--- @field public Template2 string
--- @field public Template3 string
--- @field public WeaponBones string
--- @field public TeleportSelf string See YesNo enumeration
--- @field public CanTargetCharacters string See YesNo enumeration
--- @field public CanTargetItems string See YesNo enumeration
--- @field public CanTargetTerrain string See YesNo enumeration
--- @field public ForceTarget string See YesNo enumeration
--- @field public TargetProjectiles string See YesNo enumeration
--- @field public UseCharacterStats string See YesNo enumeration
--- @field public UseWeaponDamage string See YesNo enumeration
--- @field public UseWeaponProperties string See YesNo enumeration
--- @field public SingleSource string See YesNo enumeration
--- @field public ContinueOnKill string See YesNo enumeration
--- @field public Autocast string See YesNo enumeration
--- @field public AmountOfTargets integer
--- @field public AutoAim string See YesNo enumeration
--- @field public AddWeaponRange string See YesNo enumeration
--- @field public ActionPoints integer
--- @field public Cooldown integer
--- @field public CooldownReduction integer
--- @field public ChargeDuration integer
--- @field public CastDelay integer
--- @field public Offset integer
--- @field public Lifetime integer
--- @field public Duration string See Qualifier enumeration
--- @field public TargetRadius integer
--- @field public ExplodeRadius integer
--- @field public AreaRadius integer
--- @field public HitRadius integer
--- @field public RadiusMax integer
--- @field public Range integer
--- @field public MaxDistance integer
--- @field public Angle integer
--- @field public TravelSpeed integer
--- @field public Acceleration integer
--- @field public Height integer
--- @field public Damage string See DamageSourceType enumeration
--- @field public DamageType string See Damage Type enumeration
--- @field public DamageMultiplier string See PreciseQualifier enumeration
--- @field public DeathType string See Death Type enumeration
--- @field public BonusDamage string See Qualifier enumeration
--- @field public HitPointsPercent integer
--- @field public MinHitsPerTurn integer
--- @field public MaxHitsPerTurn integer
--- @field public HitDelay integer
--- @field public MaxAttacks integer
--- @field public NextAttackChance integer
--- @field public NextAttackChanceDivider integer
--- @field public EndPosRadius integer
--- @field public JumpDelay integer
--- @field public TeleportDelay integer
--- @field public PointsMaxOffset integer
--- @field public RandomPoints integer
--- @field public ChanceToPierce integer
--- @field public MaxPierceCount integer
--- @field public MaxForkCount integer
--- @field public ForkLevels integer
--- @field public ForkChance integer
--- @field public HealAmount string See PreciseQualifier enumeration
--- @field public StatusClearChance integer
--- @field public SurfaceType string See Surface Type enumeration
--- @field public SurfaceLifetime integer
--- @field public SurfaceStatusChance integer
--- @field public SurfaceTileCollision string See SurfaceCollisionFlags enumeration
--- @field public SurfaceGrowInterval integer
--- @field public SurfaceGrowStep integer
--- @field public SurfaceRadius integer
--- @field public TotalSurfaceCells integer
--- @field public SurfaceMinSpawnRadius integer
--- @field public MinSurfaces integer
--- @field public MaxSurfaces integer
--- @field public MinSurfaceSize integer
--- @field public MaxSurfaceSize integer
--- @field public GrowSpeed integer
--- @field public GrowOnSurface string See SurfaceCollisionFlags enumeration
--- @field public GrowTimeout integer
--- @field public SkillBoost string
--- @field public SkillAttributeFlags string See AttributeFlags enumeration
--- @field public SkillProperties StatProperty[]
--- @field public CleanseStatuses string
--- @field public AoEConditions string See Conditions enumeration
--- @field public TargetConditions string See Conditions enumeration
--- @field public ForkingConditions string See Conditions enumeration
--- @field public CycleConditions string See Conditions enumeration
--- @field public ShockWaveDuration integer
--- @field public TeleportTextEvent string
--- @field public SummonEffect string
--- @field public ProjectileCount integer
--- @field public ProjectileDelay integer
--- @field public StrikeCount integer
--- @field public StrikeDelay integer
--- @field public PreviewStrikeHits string See YesNo enumeration
--- @field public SummonLevel integer
--- @field public StartTextEvent string
--- @field public StopTextEvent string
--- @field public Atmosphere string See AtmosphereType enumeration
--- @field public ConsequencesStartTime integer
--- @field public ConsequencesDuration integer
--- @field public HealthBarColor integer
--- @field public Skillbook string
--- @field public PreviewImpactEffect string
--- @field public IgnoreVisionBlock string See YesNo enumeration
--- @field public HealEffectId string
--- @field public AddRangeFromAbility string See Ability enumeration
--- @field public DivideDamage string See YesNo enumeration
--- @field public OverrideMinAP string See YesNo enumeration
--- @field public OverrideSkillLevel string See YesNo enumeration
--- @field public Tier string See SkillTier enumeration
--- @field public GrenadeBone string
--- @field public GrenadeProjectile string
--- @field public GrenadePath string
--- @field public MovingObject string
--- @field public SpawnObject string
--- @field public SpawnEffect string
--- @field public SpawnFXOverridesImpactFX string See YesNo enumeration
--- @field public SpawnLifetime integer
--- @field public ProjectileTerrainOffset string See YesNo enumeration
--- @field public ProjectileType string See ProjectileType enumeration
--- @field public HitEffect string
--- @field public PushDistance integer
--- @field public ForceMove string See YesNo enumeration
--- @field public Stealth string See YesNo enumeration
--- @field public Distribution string See ProjectileDistribution enumeration
--- @field public Shuffle string See YesNo enumeration
--- @field public PushPullEffect string
--- @field public BackStart integer
--- @field public FrontOffset integer
--- @field public TargetGroundEffect string
--- @field public PositionEffect string
--- @field public BeamEffect string
--- @field public PreviewEffect string
--- @field public CastSelfAnimation string
--- @field public IgnoreCursed string See YesNo enumeration
--- @field public IsEnemySkill string See YesNo enumeration
--- @field public DomeEffect string
--- @field public AuraSelf string
--- @field public AuraAllies string
--- @field public AuraEnemies string
--- @field public AuraNeutrals string
--- @field public AuraItems string
--- @field public AIFlags string See AIFlags enumeration
--- @field public Shape string
--- @field public Base integer
--- @field public AiCalculationSkillOverride string
--- @field public TeleportSurface string See YesNo enumeration
--- @field public ProjectileSkills string
--- @field public SummonCount integer
--- @field public LinkTeleports string See YesNo enumeration
--- @field public TeleportsUseCount integer
--- @field public HeightOffset integer
--- @field public ForGameMaster string See YesNo enumeration
--- @field public IsMelee string See YesNo enumeration
--- @field public MemorizationRequirements StatRequirement[]
--- @field public IgnoreSilence string See YesNo enumeration
--- @field public IgnoreHeight string See YesNo enumeration
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
--- @field public Name string
--- @field public Level integer
--- @field public Using string
--- @field public StatusType string
--- @field public Icon string
--- @field public DisplayName string
--- @field public DisplayNameRef string
--- @field public Description string
--- @field public DescriptionRef string
--- @field public DescriptionParams string
--- @field public OverrideDefaultDescription string See YesNo enumeration
--- @field public FormatColor string See FormatStringColor enumeration
--- @field public SavingThrow string See SavingThrow enumeration
--- @field public IsChanneled string See YesNo enumeration
--- @field public Instant string See YesNo enumeration
--- @field public StatusEffect string
--- @field public StatusEffectOverrideForItems string
--- @field public StatusEffectOnTurn string
--- @field public MaterialType string See MaterialType enumeration
--- @field public Material string
--- @field public MaterialApplyBody string See YesNo enumeration
--- @field public MaterialApplyArmor string See YesNo enumeration
--- @field public MaterialApplyWeapon string See YesNo enumeration
--- @field public MaterialApplyNormalMap string See YesNo enumeration
--- @field public MaterialFadeAmount integer
--- @field public MaterialOverlayOffset integer
--- @field public MaterialParameters string
--- @field public HealingEvent string See StatusEvent enumeration
--- @field public HealStat string See StatusHealType enumeration
--- @field public HealType string See HealValueType enumeration
--- @field public HealValue integer
--- @field public StatsId string
--- @field public IsInvulnerable string See YesNo enumeration
--- @field public IsDisarmed string See YesNo enumeration
--- @field public StackId string
--- @field public StackPriority integer
--- @field public AuraRadius integer
--- @field public AuraSelf string
--- @field public AuraAllies string
--- @field public AuraEnemies string
--- @field public AuraNeutrals string
--- @field public AuraItems string
--- @field public AuraFX string
--- @field public ImmuneFlag string See AttributeFlag enumeration
--- @field public CleanseStatuses string
--- @field public MaxCleanseCount integer
--- @field public ApplyAfterCleanse string See YesNo enumeration
--- @field public SoundStart string
--- @field public SoundLoop string
--- @field public SoundStop string
--- @field public DamageEvent string See StatusEvent enumeration
--- @field public DamageStats string
--- @field public DeathType string See Death Type enumeration
--- @field public DamageCharacters string See YesNo enumeration
--- @field public DamageItems string See YesNo enumeration
--- @field public DamageTorches string See YesNo enumeration
--- @field public FreezeTime integer
--- @field public SurfaceChange string
--- @field public PermanentOnTorch string See YesNo enumeration
--- @field public AbsorbSurfaceType string
--- @field public AbsorbSurfaceRange integer
--- @field public Skills string
--- @field public BonusFromAbility string See Ability enumeration
--- @field public Items string
--- @field public OnlyWhileMoving string See YesNo enumeration
--- @field public DescriptionCaster string
--- @field public DescriptionTarget string
--- @field public WinBoost string See Properties enumeration
--- @field public LoseBoost string See Properties enumeration
--- @field public WeaponOverride string
--- @field public ApplyEffect string
--- @field public ForGameMaster string See YesNo enumeration
--- @field public ResetCooldowns string
--- @field public ResetOncePerCombat string See YesNo enumeration
--- @field public PolymorphResult string
--- @field public DisableInteractions string See YesNo enumeration
--- @field public LoseControl string See YesNo enumeration
--- @field public AiCalculationSkillOverride string
--- @field public HealEffectId string
--- @field public ScaleWithVitality string See YesNo enumeration
--- @field public VampirismType string See VampirismType enumeration
--- @field public BeamEffect string
--- @field public HealMultiplier integer
--- @field public InitiateCombat string See YesNo enumeration
--- @field public Projectile string
--- @field public Radius integer
--- @field public Charges integer
--- @field public MaxCharges integer
--- @field public DefendTargetPosition string See YesNo enumeration
--- @field public TargetConditions string See Conditions enumeration
--- @field public Toggle string See YesNo enumeration
--- @field public LeaveAction string
--- @field public DieAction string
--- @field public PlayerSameParty string See YesNo enumeration
--- @field public PlayerHasTag string
--- @field public PeaceOnly string See YesNo enumeration
--- @field public Necromantic string See YesNo enumeration
--- @field public RetainSkills string
--- @field public BringIntoCombat string See YesNo enumeration
--- @field public ApplyStatusOnTick string
--- @field public IsResistingDeath string See YesNo enumeration
--- @field public TargetEffect string
--- @field public DamagePercentage integer
--- @field public ForceOverhead string See YesNo enumeration
--- @field public TickSFX string
--- @field public ForceStackOverwrite string See YesNo enumeration
--- @field public FreezeCooldowns string See YesNo enumeration
local StatEntryStatusData = {}


--- @class StatEntryCrime
--- @field public Name string
--- @field public Level integer
--- @field public Using string
--- @field public CrimeType string
--- @field public TensionWeight integer
--- @field public Lifetime integer
--- @field public Audible string See YesNo enumeration
--- @field public DetectionRange integer
--- @field public CreatesCrimescene string See YesNo enumeration
--- @field public Continuous string See YesNo enumeration
--- @field public VictimReaction string
--- @field public NoticedAD string
--- @field public WarningDialog string
--- @field public ArrestDialog string
--- @field public InterrogateDialog string
--- @field public ReactionCategory_Tag string
--- @field public ReactionCategory_Priority integer
--- @field public ReactionCategory_PrimaryReaction string
--- @field public ReactionCategory_SecondaryReaction string
--- @field public MaxInvestigators integer
--- @field public ContinuousDelayTimer integer
--- @field public SoundEvent string
--- @field public AudibleRange integer
--- @field public VictimDialog string
--- @field public CanRaiseAlarm string See YesNo enumeration
--- @field public CanMerge string See YesNo enumeration
local StatEntryCrime = {}


--- @class HitRequest
--- @field public Equipment integer
--- @field public TotalDamageDone integer
--- @field public DamageDealt integer
--- @field public DeathType string See DeathType enumeration
--- @field public DamageType string See DamageType enumeration
--- @field public AttackDirection integer
--- @field public ArmorAbsorption integer
--- @field public LifeSteal integer
--- @field public EffectFlags integer See HitFlags enumeration
--- @field public HitWithWeapon boolean
--- @field public DamageList DamageList
local HitRequest = {}


--- @class HitContext
--- Context information passed in StatusHitEnter/BeforeCharacterApplyDamage callbacks
--- @field public HitId integer Unique hit identifier for hit tracking
--- @field public Weapon StatItem
--- @field public Hit HitRequest
--- @field public HitType string See HitType enumeration
--- @field public NoHitRoll boolean
--- @field public ProcWindWalker boolean
--- @field public ForceReduceDurability boolean
--- @field public HighGround string
--- @field public CriticalRoll string
--- @field public HitStatus EsvStatus
local HitContext = {}


--- @class EsvCombatTeam
--- @field public TeamId integer A number identifying the team instance
--- @field public CombatId integer 	Identifies which combat the team is a participant of
--- @field public Initiative integer Computed initiative value of the team
--- @field public StillInCombat boolean Can the team currently fight, or is it temporarily out of combat?
--- @field public Character EsvCharacter|nil Character object if the team is a character; nil otherwise
--- @field public Item EsvItem|nil Item object if the team is an item; nil otherwise
local EsvCombatTeam = {}


--- @class EsvCombat
--- @field public CombatId integer A number identifying the combat instance
--- @field public LevelName string Level where the combat is taking place
--- @field public IsActive boolean
local EsvCombat = {
    --- Retrieves the turn order of the current round.
    --- @param self EsvCombat
    --- @return EsvCombatTeam[]
    GetCurrentTurnOrder = function (self) end,
    
    --- Retrieves the turn order of the next round.
    --- @param self EsvCombat
    --- @return EsvCombatTeam[]
    GetNextTurnOrder = function (self) end,
    
    --- Updates the turn order of the current round. 
    --- The turnOrder argument should be a reordered version of the table returned by GetCurrentTurnOrder().
    --- Notes:
    ---  It is possible to remove or add characters to the current turn by adding/removing their Team object from the table.
    ---  It is possible to add a character to the current turn more than once, the character will only appear once in the UI however.
    ---  The character whose turn is currently active (the very first item) should not be removed or reordered. 
    ---    This only applies for GetCurrentTurnOrder, the first item can be freely reordered in GetNextTurnOrder.
    ---  Changed performed using this function are synchronized to the client at the end of the current server tick.
    --- @param self EsvCombat
    --- @param turnOrder EsvCombatTeam[]
    UpdateCurrentTurnOrder = function (self, turnOrder) end,
    
    --- Updates the turn order of the next round. 
    --- The turnOrder argument should be a reordered version of the table returned by GetNextTurnOrder().
    --- Notes:
    ---  It is possible to remove or add characters to the next turn by adding/removing their Team object from the table.
    ---  It is possible to add a character to the next turn more than once, the character will only appear once in the UI however.
    ---  Changed performed using this function are synchronized to the client at the end of the current server tick.
    --- @param self EsvCombat
    --- @param turnOrder EsvCombatTeam[]
    UpdateNextTurnOrder = function (self, turnOrder) end,
    
    --- Retrieves all participants of the combat
    --- @param self EsvCombat
    --- @return EsvCombatTeam[]
    GetAllTeams = function (self) end
}

--- @class UIObject
local UIObject = {
    --- @param self UIObject
    --- @param x integer
    --- @param y integer
    SetPosition = function (self, x, y) end,
    
    --- @param self UIObject
    --- @param width integer
    --- @param height integer
    Resize = function (self, width, height) end,
    
    --- @param self UIObject
    Show = function (self) end,
    
    --- @param self UIObject
    Hide = function (self) end,
    
    --- Calls a method on the main timeline object of the UI element. 
    --- The first argument (func) is the name of the ActionScript function to call; 
    --- all subsequent arguments are passed to the ActionScript function as parameters. 
    --- Only string, number and boolean arguments are supported.
    --- @param self UIObject
    --- @param method string Method to call
    --- @vararg any
    Invoke = function (self, method, ...) end,
    
    --- Simulates an ExternalInterface.call(...) call from Flash, 
    --- i.e. it calls an UI handler function in the game engine. 
    --- The first argument (func) is the name of the UI function to call; 
    --- all subsequent arguments are passed to the engine as parameters. 
    --- Only string, number and boolean arguments are supported.
    --- @param self UIObject
    --- @param func string ExternalInterface function to call
    --- @vararg any
    ExternalInterfaceCall = function (self, func, ...) end,
    
    --- @param self UIObject
    --- @param frame integer
    GotoFrame = function (self, frame) end,
    
    --- Updates the specified public property of the main timeline object.
    --- @param self UIObject
    --- @param property string Property path to set
    --- @param value number|boolean|string Value
    --- @param arrayIndex integer|nil Pass integer to set property[arrayIndex]
    SetValue = function (self, property, value, arrayIndex) end,
    
    --- Retrieves the specified public property of the main timeline object. 
    --- "type" contains the type of value to retrieve and must be string, number or boolean.
    --- @param self UIObject
    --- @param property string Property path to set
    --- @param type "'number'" | "'boolean'" | "'string'" | nil
    --- @param arrayIndex integer|nil Pass integer to set property[arrayIndex]
    --- @return number|boolean|string
    GetValue = function (self, property, type, arrayIndex) end,
    
    --- Returns the engine UI type ID of the UI element
    --- @param self UIObject
    --- @return integer
    GetTypeId = function (self) end,
    
    --- @param self UIObject
    --- @return integer
    GetHandle = function (self) end,
    
    --- Returns the handle of the player that this UI element is assigned to.
    --- (Only certain elements have a player assigned, like character sheet, inventory, etc.)
    --- @param self UIObject
    --- @return integer|nil
    GetPlayerHandle = function (self) end,
    
    --- @param self UIObject
    Destroy = function (self) end,
}

--- @alias ExtEngineEvent "'SessionLoading'" | "'SessionLoaded'" | "'ModuleLoading'" | "'ModuleLoadStarted'" | "'ModuleResume'" | "'GameStateChanged'" | "'SkillGetDescriptionParam'" | "'StatusGetDescriptionParam'" | "'GetSkillDamage'" | "'ComputeCharacterHit'" | "'CalculateTurnOrder'" | "'GetHitChance'" | "'StatusGetEnterChance'" | '"StatusHitEnter"' | "'BeforeCharacterApplyDamage'" | "'UIInvoke'" | "'UICall'"

--- @alias ExtGameStateChangedCallback fun(fromState: string, toState: string)
--- @alias ExtComputeCharacterHitCallback fun(target: StatCharacter, attacker: StatCharacter, weapon: StatItem, damageList: DamageList, hitType: string, noHitRoll: boolean, forceReduceDurability: boolean, hit: HitRequest, alwaysBackstab: boolean, highGroundFlag: string, criticalRoll: string): HitRequest
--- @alias ExtGetSkillDamageCallback fun(skill: StatEntrySkillData, attacker: StatCharacter, isFromItem: boolean, stealthed: boolean, attackerPos: number[], targetPos: number[], level: number, noRandomization: boolean): DamageList, string
--- @alias ExtCalculateTurnOrderCallback fun(combat: EsvCombat, order: EsvCombatTeam[]): EsvCombatTeam[]
--- @alias ExtGetHitChanceCallback fun(attacker: StatCharacter, target: StatCharacter): number
--- @alias ExtStatusGetEnterChanceCallback fun(status: EsvStatus, isEnterCheck: boolean): number
--- @alias ExtSkillGetDescriptionParamCallback fun(skill: StatEntrySkillData, character: StatCharacter, isFromItem: boolean, ...): string
--- @alias ExtStatusGetDescriptionParamCallback fun(status: EsvStatus, statusSource: EsvGameObject, character: StatCharacter, ...): string
--- @alias ExtBeforeCharacterApplyDamageCallback fun(target: EsvCharacter, attacker: StatCharacter|StatItem, hit: HitRequest, causeType: string, impactDirection: number[], context: HitContext): HitRequest
--- @alias ExtStatusHitEnterCallback fun(status: EsvStatus, context: HitContext)

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
    },

    --- Returns the version number of the Osiris Extender
    --- @return integer
    Version = function () end,

    --- Returns the version number of the game
    --- @return string
    GameVersion = function () end,

    --- Loads the specified Lua file
    --- @param modGuid string GUID of the module containing the Lua file
    --- @param fileName string Path of Lua file, relative to Mods/<Mod>/Story/RawFiles/Lua
    Require = function (modGuid, fileName) end,

    --- Registers a function to call when an extender event is thrown
    --- @param event ExtEngineEvent Event to listen for
    --- @param callback function Lua function to run when the event fires
    RegisterListener = function (event, callback) end,

    --- Registers a function that is called when certain Osiris functions are called.
    --- Supports events, built-in queries, DBs, PROCs, QRYs (user queries).
    --- @param name string Osiris function/database name
    --- @param arity number Number of columns for DBs or the number of parameters (both IN and OUT) for functions
    --- @param event string Event type ('before' - triggered before Osiris call; 'after' - after Osiris call; 'beforeDelete'/'afterDelete' - before/after delete from DB)
    --- @param handler function Lua function to run when the event fires
    RegisterOsirisListener = function (name, arity, event, handler) end,

    --- Registers a new call in Osiris
    --- @param func function Function to register
    --- @param funcName string Name of call to register
    --- @param arguments string Call argument list
    NewCall = function (func, funcName, arguments) end,

    --- Registers a new query in Osiris
    --- @param func function Function to register
    --- @param funcName string Name of query to register
    --- @param arguments string Query argument list
    NewQuery = function (func, funcName, arguments) end,

    --- Registers a new event in Osiris
    --- @param funcName string Name of event to register
    --- @param arguments string Event argument list
    NewEvent = function (funcName, arguments) end,

    --- Print to console window and editor messages pane
    Print = function (...) end,

    --- Print warning to console window and editor messages pane
    PrintWarning = function (...) end,

    --- Print error to console window and editor messages pane
    PrintError = function (...) end,

    --- Parse a JSON document into a Lua object
    --- @param json string JSON string to parse
    --- @return any
    JsonParse = function (json) end,

    --- Converts a Lua value into a JSON document
    --- @param val any Value to serialize
    --- @return string JSON document
    JsonStringify = function (val) end,

    --- Returns whether the specified mod is loaded
    --- @param modGuid string GUID of the module
    --- @return boolean
    IsModLoaded = function (modGuid) end,

    --- Returns the list of loaded modules in load order
    --- @return string[]
    GetModLoadOrder = function () end,

    --- Returns detailed information about the specified (loaded) module
    --- @param modGuid string GUID of the module
    --- @return ModInfo
    GetModInfo = function (modGuid) end,

    --- Returns the list of loaded stat entries
    --- @param type string|nil Type of stat entry to fetch (StatusData, SkillData, ...)
    --- @return string[]
    GetStatEntries = function (type) end,

    --- Returns the list of stat entries that were loaded before the specified mod
    --- @param modGuid string Mod GUID to check
    --- @param type string|nil Type of stat entry to fetch (StatusData, SkillData, ...)
    --- @return string[]
    GetStatEntriesLoadedBefore = function (modId, type) end,

    --- Returns all skills from the specified skill set
    --- @param name string Name of skill set entry
    --- @return string[]
    GetSkillSet = function (name) end,

    --- Returns all equipment from the specified equipment set
    --- @param name string Name of equipment set entry
    --- @return string[]
    GetEquipmentSet = function (name) end,

    --- Returns an attribute of the specified stat entry
    --- @param stat string Stat entry name
    --- @param attribute string Stat attribute name
    --- @return any
    StatGetAttribute = function (stat, attribute) end,

    --- Updates an attribute of the specified stat entry
    --- @param stat string Stat entry name
    --- @param attribute string Stat attribute name
    --- @param value any New stat value
    StatSetAttribute = function (stat, attribute, value) end,

    --- Adds a property description to the specified stat entry
    --- @param stat string Stat entry name
    --- @param attribute string Property list attribute name
    --- @param description any Description to add
    StatAddCustomDescription = function (stat, attribute, description) end,

    --- Returns the specified DeltaMod or nil on failure
    --- @param name string Name of delta mod
    --- @return DeltaMod
    GetDeltaMod = function (name) end,

    --- Updates all properties of the specified DeltaMod.
    --- The function expects a table in the same format as the one returned by GetDeltaMod.
    --- @param deltaMod DeltaMod Name of delta mod
    UpdateDeltaMod = function (deltaMod) end,

    --- Replaces level scaling formula for the specified stat
    --- @param statType string Stat entry type
    --- @param attribute string Stat attribute name
    --- @param func function Replacement scaling function
    StatSetLevelScaling = function (statType, attribute, func) end,

    --- Returns the property proxy of the specified stats entry
    --- Returns level scaled values if the level parameter is not nil.
    --- @param stat string Stat entry name
    --- @param level integer|nil Level scaling level
    --- @return StatEntryArmor|StatEntryCharacter|StatEntryObject|StatEntryPotion|StatEntryShield|StatEntrySkillData|StatEntryStatusData|StatEntryWeapon
    GetStat = function (stat, level) end,

    --- Creates a new stats entry on the server
    --- @param name string Stat entry name
    --- @param type string Stat entry type (i.e. SkillData, StatusData, etc.)
    --- @param template string|nil When not nil, all properties are copied from the specified stats entry
    --- @return StatEntryArmor|StatEntryCharacter|StatEntryObject|StatEntryPotion|StatEntryShield|StatEntrySkillData|StatEntryStatusData|StatEntryWeapon
    CreateStat = function (name, type, template) end,

    --- Synchronizes all modifications of the specified stat to all clients
    --- @param name string Stat entry name
    --- @param persist boolean|nil Persist stats entry to savegame?
    SyncStat = function (name, persist) end,

    --- Toggles whether the specified stats entry should be persisted to savegames
    --- @param name string Stat entry name
    --- @param persist boolean Persist stats entry to savegame?
    StatSetPersistence = function (name, persist) end,

    --- Returns the textual label assigned to the specified enumeration value
    --- @param enum string Engine enumeration name
    --- @param index number Value index to look up
    --- @return string|nil
    EnumIndexToLabel = function (enum, index) end,

    --- Returns the numeric index assigned to the specified enumeration label
    --- @param enum string Engine enumeration name
    --- @param label string Value name to look for
    --- @return number|nil
    EnumLabelToIndex = function (enum, label) end,

    --- Returns the GUID of all characters on the specified level. 
    --- Uses the current level if no level name was specified.
    --- @param level string|nil Optional level name
    --- @return string[]
    GetAllCharacters = function (level) end,

    --- Returns the GUID of all characters within a radius around the specified point.
    --- @param x number
    --- @param y number
    --- @param z number
    --- @param distance number
    --- @return string[]
    GetCharactersAroundPosition = function (x, y, z, distance) end,

    --- Returns the GUID of all items on the specified level. 
    --- Uses the current level if no level name was specified.
    --- @param level string|nil Optional level name
    --- @return string[]
    GetAllItems = function (level) end,

    --- Returns the GUID of all items within a radius around the specified point.
    --- @param x number
    --- @param y number
    --- @param z number
    --- @param distance number
    --- @return string[]
    GetItemsAroundPosition = function (x, y, z, distance) end,

    --- Returns the property proxy of the specified character
    --- @param id string|integer Character GUID or handle or NetID
    --- @return EsvCharacter
    GetCharacter = function (id) end,

    --- Returns the property proxy of the specified item
    --- @param id string|integer Item GUID or handle or NetID
    --- @return EsvItem
    GetItem = function (id) end,

    --- Returns the property proxy of the specified item
    --- @param handle integer Game object handle
    --- @return EsvGameObject
    GetGameObject = function (handle) end,

    --- Returns the property proxy of the specified status
    --- @param characterGuid string Character GUID or handle
    --- @param handle integer Status handle or NetID
    --- @return EsvStatus
    GetStatus = function (characterGuid, handle) end,

    --- Returns the specified turn-based combat
    --- @param combatId integer Combat ID
    --- @return EsvCombat
    GetCombat = function (combatId) end,

    --- Creates a new damage list object
    --- @return DamageList
    NewDamageList = function () end,

    --- Returns whether Osiris is currently accessible or not.
    --- @return boolean
    OsirisIsCallable = function () end,

    --- Returns a random number; equivalent to Lua random
    --- @param low integer
    --- @param up integer
    --- @return integer|number
    Random = function (low, up) end,

    --- Rounds the specified number
    --- @param n number
    --- @return number
    Round = function (n) end,

    --- Generate Lua IDE helpers for the currently loaded module
    --- @param builtin boolean|nil Only export built-in functions and names exported by Lua?
    GenerateIdeHelpers = function (builtin) end,

    --- Returns whether the code is executing in a client context
    --- @return boolean
    IsClient = function () end,

    --- Returns whether the code is executing in a server context
    --- @return boolean
    IsServer = function () end,

    --- Returns whether the Developer Mode switch is enabled
    --- @return boolean
    IsDeveloperMode = function () end,

    --- Broadcast a message to all peers
    --- @param channel string Channel that will receive the message
    --- @param payload string Message payload
    --- @param excludeCharacter string|nil Optional peer to exclude from broadcast
    BroadcastMessage = function (channel, payload, excludeCharacter) end,

    --- Sends a message to the specified peer
    --- @param characterGuid string Peer that will receive the message
    --- @param channel string Channel that will receive the message
    --- @param payload string Message payload
    PostMessageToClient = function (characterGuid, channel, payload) end,

    --- Sends a message to the server
    --- @param channel string Channel that will receive the message
    --- @param payload string Message payload
    PostMessageToServer = function (channel, payload) end,

    --- Registers a listener that is called when a network message is received on the specified channel
    --- @param channel string Network channel name
    --- @param handler function Lua handler
    RegisterNetListener = function (channel, handler) end,

    --- Registers a new dialog voice line for the specified speaker.
    --- @param speakerGuid string Original path
    --- @param textKey string Translated string key of text line
    --- @param path string Path to audio .WEM
    --- @param length number Length of audio in seconds
    AddVoiceMetaData = function (speakerGuid, textKey, path, length) end,

    --- @param handle string Translated string handle
    --- @param fallback string Fallback string if the specified handle is not found
    --- @return string Translated string
    GetTranslatedString = function (handle, fallback) end,

    --- @param key string Translated string key
    --- @return string,string Translated string and handle
    GetTranslatedStringFromKey = function (key) end,

    --- Redirects all file accesses to the specified path to another file.
    --- @param path string Original path
    --- @param newPath string New (redirected) path
    AddPathOverride = function (path, newPath) end,

    --- Constructs a new Flash UI element
    --- @param name string User-defined unique name that identifies the UI element
    --- @param path string Path of the SWF file relative to the data directory
    --- @param layer integer Stack order of the UI element
    --- @return UIObject|nil
    CreateUI = function (name, path, layer) end,

    --- Retrieves an UI element with the specified name
    --- @param name string User-defined unique name that identifies the UI element
    --- @return UIObject|nil
    GetUI = function (name) end,

    --- Retrieves a built-in UI element at the specified path.
    --- If no such element exists, the function returns nil.
    --- @param path string UI SWF path relative to Data\
    --- @return UIObject|nil
    GetBuiltinUI = function (path) end,

    --- Retrieves an engine UI element with the specified engine type ID.
    --- If no such element exists, the function returns nil.
    --- @param typeId number Engine UI element type ID
    --- @return UIObject|nil
    GetUIByType = function (typeId) end,

    --- Destroys the specified UI element
    --- @param name string User-defined unique name that identifies the UI element
    DestroyUI = function (name) end,

    --- Registers a listener that is called when the specified function is called from Flash
    --- @param object UIObject UI object returned from Ext.CreateUI, Ext.GetUI or Ext.GetBuiltinUI
    --- @param name string ExternalInterface function name
    --- @param handler function Lua handler
    RegisterUICall = function (object, name, handler) end,

    --- Registers a listener that is called when the specified function is called from Flash.
    --- The event is triggered for every UI element with the specified type ID.
    --- @param typeId number Engine UI element type ID
    --- @param name string ExternalInterface function name
    --- @param handler function Lua handler
    RegisterUITypeCall = function (typeId, name, handler) end,

    --- Registers a listener that is called when the specified function is called from Flash.
    --- The event is triggered regardless of which UI element it was called on.
    --- (Function call capture must be enabled for every element type that needs to monitored!)
    --- @param name string ExternalInterface function name
    --- @param handler function Lua handler
    RegisterUINameCall = function (name, handler) end,

    --- Registers a listener that is called when the specified method is called on the main timeline of the Flash object
    --- @param object UIObject UI object returned from Ext.CreateUI, Ext.GetUI or Ext.GetBuiltinUI
    --- @param method string Flash method name
    --- @param handler function Lua handler
    RegisterUIInvokeListener = function (object, name, handler) end,

    --- Registers a listener that is called when the specified method is called on the main timeline of the Flash object
    --- The event is triggered for every UI element with the specified type ID.
    --- @param typeId number Engine UI element type ID
    --- @param method string Flash method name
    --- @param handler function Lua handler
    RegisterUITypeInvokeListener = function (typeId, name, handler) end,

    --- Registers a listener that is called when the specified method is called on the main timeline of the Flash object
    --- The event is triggered regardless of which UI element it was called on.
    --- @param method string Flash method name
    --- @param handler function Lua handler
    RegisterUINameInvokeListener = function (name, handler) end,

    --- Registers a listener that is called when a console command is entered in the dev console
    --- @param cmd string Console command
    --- @param handler function Lua handler
    RegisterConsoleCommand = function (cmd, handler) end,

    --- Write data to an external (persistent) file
    --- @param path string File path relative to Documents\Larian Studios\Divinity Original Sin 2 Definitive Edition\Osiris Data
    --- @param contents string File contents to write
    SaveFile = function (path, contents) end,

    --- Read data from an external (persistent) file
    --- @param path string File path relative to Documents\Larian Studios\Divinity Original Sin 2 Definitive Edition\Osiris Data
    --- @return string File contents
    LoadFile = function (path) end,

    --- Returns a monotonic value representing the current time in milliseconds.
    --- Useful for performance measurements / measuring real world time.
    --- (Note: This value is not synchronized between peers and different clients may report different time values!)
    --- @return number Time
    MonotonicTime = function () end,

    -- Returns whether the player has a compatible Script Extender version installed
    --- @param playerGuid string GUID of player character
    --- @return boolean
    PlayerHasExtender = function (playerGuid) end,

    -- Handle to double conversion hack for use in Flash external interface calls
    -- (Some of the builtin functions treat handles as double values)
    --- @param handle number Integer handle to cast
    --- @return number Double handle
    HandleToDouble = function (handle) end,

    -- Double to handle conversion hack for use in Flash external interface calls
    -- (Some of the builtin functions treat handles as double values)
    --- @param handle number Double handle to cast
    --- @return number Integer handle
    DoubleToHandle = function (handle) end
}
