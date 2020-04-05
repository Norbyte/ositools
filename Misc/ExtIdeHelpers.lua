-- Special global value that contains the current mod UUID during load
ModuleUUID = "UUID"

Osi = {}

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
    GetExtensionVersion = function () end,

    --- Loads the specified Lua file
    --- @param modGuid string GUID of the module containing the Lua file
    --- @param fileName string Path of Lua file, relative to Mods/<Mod>/Story/RawFiles/Lua
    Require = function (modGuid, fileName) end,

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
    --- @return table
    GetModLoadOrder = function () end,

    --- Returns detailed information about the specified (loaded) module
    --- @param modGuid string GUID of the module
    --- @return table
    GetModInfo = function (modGuid) end,

    --- Returns the list of loaded stat entries
    --- @param type string|nil Type of stat entry to fetch (StatusData, SkillData, ...)
    --- @return table
    GetStatEntries = function (type) end,

    --- Returns all skills from the specified skill set
    --- @param name string Name of skill set entry
    --- @return table
    GetSkillSet = function (name) end,

    --- Returns all equipment from the specified equipment set
    --- @param name string Name of equipment set entry
    --- @return table
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

    --- Replaces level scaling formula for the specified stat
    --- @param statType string Stat entry type
    --- @param attribute string Stat attribute name
    --- @param func function Replacement scaling function
    StatSetLevelScaling = function (statType, attribute, func) end,

    --- Returns the property proxy of the specified stats entry
    --- Returns level scaled values if the level parameter is not nil.
    --- @param stat string Stat entry name
    --- @param level integer|nil Level scaling level
    --- @return table
    GetStat = function (stat, level) end,

    --- Returns the property proxy of the specified character
    --- @param characterGuid string Character GUID or handle
    --- @return table
    GetCharacter = function (characterGuid) end,

    --- Returns the property proxy of the specified item
    --- @param itemGuid string Item GUID or handle
    --- @return table
    GetItem = function (itemGuid) end,

    --- Returns the property proxy of the specified status
    --- @param characterGuid string Character GUID or handle
    --- @param handle integer Status handle
    --- @return table
    GetStatus = function (characterGuid, handle) end,

    --- Returns the specified turn-based combat
    --- @param combatId integer Combat ID
    --- @return table
    GetCombat = function (combatId) end,

    --- Creates a new damage list object
    --- @return table
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
    GenerateIdeHelpers = function () end,

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

    --- Redirects all file accesses to the specified path to another file.
    --- @param path string Original path
    --- @param newPath string New (redirected) path
    AddPathOverride = function (path, newPath) end,

    --- Constructs a new Flash UI element
    --- @param name string User-defined unique name that identifies the UI element
    --- @param path string Path of the SWF file relative to the data directory
    --- @param layer integer Stack order of the UI element
    --- @return table|nil
    CreateUI = function (name, path, layer) end,

    --- Retrieves an UI element with the specified name
    --- @param name string User-defined unique name that identifies the UI element
    --- @return table|nil
    GetUI = function (name) end,

    --- Destroys the specified UI element
    --- @param name string User-defined unique name that identifies the UI element
    DestroyUI = function (name) end,

    --- Registers a listener that is called when the specified function is called from Flash
    --- @param object table UI object returned from Ext.CreateUI or Ext.GetUI
    --- @param name string ExternalInterface function name
    --- @param handler function Lua handler
    RegisterUICall = function (object, name, handler) end,
}
