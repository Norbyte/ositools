local _DEBUG = Ext.Debug.IsDeveloperMode()

local _format = string.format

local NEWLINE = "\r\n"

---@type {Specific:table<string,string>, Misc:string[]}
local _CustomEntries = Ext.Utils.Include(nil, "builtin://Libs/HelpersGenerator/CustomEntries.lua")

---@type table<string,{Before:string|nil, After:string|nil}>
local _CustomTypeEntries = Ext.Utils.Include(nil, "builtin://Libs/HelpersGenerator/CustomTypeEntries.lua")

---@type table<string,{Before:string|nil, After:string|nil}>

local _CustomFunctionExtras = Ext.Utils.Include(nil, "builtin://Libs/HelpersGenerator/CustomFunctionExtras.lua")

---@type table<string,table<string, string>>
local _Aliases = Ext.Utils.Include(nil, "builtin://Libs/HelpersGenerator/Aliases.lua")

---@type string
local _OsiLines = Ext.Utils.Include(nil, "builtin://Libs/HelpersGenerator/Osi.lua")

---@type {Functions:string, BaseExtraData:string}
local _Deprecated = Ext.Utils.Include(nil, "builtin://Libs/HelpersGenerator/Deprecated.lua")
local _Flash = Ext.Utils.Include(nil, "builtin://Libs/HelpersGenerator/Flash.lua") --[[@as string]]

---@type {Regular:table<string, table<string, {Params:{name:string, arg:string, description:string}, Return:{}}>>, Field:table<string, table<string, string>>}
local _FuncData = Ext.Utils.Include(nil, "builtin://Libs/HelpersGenerator/FunctionFixes.lua")

local _PreventableEvent = {
    EclLuaUICallEventParams = true,
    BeforeStatusDelete = true,
}

local _TypeAliasReplace = {
    ComponentHandle = "userdata",
    EntityHandle = "userdata",
    int16 = "integer",
    int32 = "integer",
    int64 = "integer",
    int8 = "integer",
    uint16 = "integer",
    uint32 = "integer",
    uint64 = "integer",
    uint8 = "integer",
    UserId = "integer",
    NetId = "integer",
    IggyInvokeDataValue = "boolean|number|string",
}

local _TypeReplace = {
    STDString = "string",
    STDWString = "string",
    CString = "string",
    bool = "boolean",
    double = "number",
    float = "number",
}

local _TypeAliasRemove = {
    STDString = true,
    STDWString = true,
    CString = true,
    bool = true,
    double = true,
    float = true,
}

--- @class GenerateIdeHelpersGenerator
local Generator = {}

function Generator.Trim(s)
    return s:gsub("^%s*(.-)%s*$", "%1")
end

function Generator.RTrim(s)
    return s:gsub("^(.-)%s*$", "%1")
end

Generator.ValueKindToLua = {
    Boolean = "boolean",
    Integer = "number",
    Float = "number",
    String = "string",
    Enumeration = "string",
    Any = "any"
}

local startingText = [[--- @meta
--- @diagnostic disable

--- Table that contains every ModTable entry for active mods.
Mods = {}

--- Special global value that contains the current mod UUID during load
--- @type FixedString
ModuleUUID = "UUID"

--- Table that gets stored in the save, unique for each ModTable entry.
--- @type table
PersistentVars = {}

--- @alias OsirisValue number|string

--- Using a DB like a function will allow inserting new values into the database (ex. `Osi.DB_IsPlayer("02a77f1f-872b-49ca-91ab-32098c443beb")`  
--- @overload fun(...:OsirisValue|nil)
--- @class OsiDatabase
local OsiDatabase = {}
--- Databases can be read using the Get method. The method checks its parameters against the database and only returns rows that match the query.  
--- The number of parameters passed to Get must be equivalent to the number of columns in the target database.  
--- Each parameter defines an (optional) filter on the corresponding column.  
--- If the parameter is nil, the column is not filtered (equivalent to passing _ in Osiris). If the parameter is not nil, only rows with matching values will be returned.
--- @vararg OsirisValue|nil
--- @return table<integer,table<integer,OsirisValue>>
function OsiDatabase:Get(...) end
--- The Delete method can be used to delete rows from databases.  
--- The number of parameters passed to Delete must be equivalent to the number of columns in the target database.  
--- Each parameter defines an (optional) filter on the corresponding column.  
--- If the parameter is nil, the column is not filtered (equivalent to passing _ in Osiris). If the parameter is not nil, only rows with matching values will be deleted. 
--- @vararg OsirisValue|nil
function OsiDatabase:Delete(...) end

--- @alias OsiFunction fun(...:OsirisValue):OsirisValue|nil
--- @alias OsiDynamic table<string, OsiFunction|OsiDatabase>

--- @class OsiCommonDatabases
--- @field DB_IsPlayer OsiDatabase|fun(Guid:string) All player characters
--- @field DB_Origins OsiDatabase|fun(Guid:string) All origin characters
--- @field DB_Avatars OsiDatabase|fun(Guid:string) All player characters that were created in character creation, or that have an `AVATAR` tag
--- @field DB_CombatObjects OsiDatabase|fun(Guid:string, combatID:integer) All objects in combat
--- @field DB_CombatCharacters OsiDatabase|fun(Guid:string, combatID:integer) All characters in combat
--- @field DB_Dialogs OsiDatabase|fun(Guid:string, dialog:string)|fun(GUID1:string, GUID2:string, dialog:string)|fun(GUID1:string, GUID2:string, GUID3:string, dialog:string)|fun(GUID1:string, GUID2:string, GUID3:string, GUID4:string, dialog:string) All registered dialogs for objects, the most common being the version with a single character

--- The Osi table contains databases as well as calls, queries, events, and custom PROC / QRY defintions, as long as they are used in a script.  
--- @type OsiCommonDatabases|OsiDynamic
Osi = {}

--- @alias OsirisEventType string|"before"|"after"|"beforeDelete"|"afterDelete"
--- @alias i16vec2 int16[]

--- @alias SkillAbility "None"|"Warrior"|"Ranger"|"Rogue"|"Source"|"Fire"|"Water"|"Air"|"Earth"|"Death"|"Summoning"|"Polymorph"
--- @alias SkillElement SkillAbility
--- @alias YesNo "Yes"|"No"

--- @alias GameDifficultyValue uint32
---|0 # Story
---|1 # Explorer
---|2 # Classic
---|3 # Tactician
---|4 # Honour

--- @alias RTPCName "PlaybackSpeed"|"RTPC_Rumble"|"RTPC_Volume_Ambient"|"RTPC_Volume_Cinematic"|"RTPC_Volume_FX"|"RTPC_Volume_MAIN"|"RTPC_Volume_Music"|"RTPC_Volume_Music_Fight"|"RTPC_Volume_UI"|"RTPC_Volume_VO_Dialog"|"RTPC_Volume_VO_Master"|"RTPC_Volume_VO_Narrator"|"RTPC_Volume_VO_Overhead"
--- @alias SoundObjectId "Global"|"Music"|"Ambient"|"HUD"|"GM"|"Player1"|"Player2"|"Player3"|"Player4"
--- @alias StateGroupName "ARX_Dead"|"ARX_Krakenbattle"|"Amb_ARX_Frozen"|"Amb_Endgame_State"|"Amb_LV_State"|"Amb_Tuto_State"|"COS_OrcTemple"|"CoS_ElfTempleNuked"|"DLC_01_Amb_LV_State"|"DLC_01_Amb_WindBlender"|"GM_Theme"|"Items_Objects_MCH_Laboratory_Machines_Turbine_A"|"Menu_Themes"|"Music_Theme"|"Music_Type"|"Proj_Gren_ClusterBomb_Impact_Multi_VoiceLimitSwitch"|"Skill_NPC_VoidGlide"|"Soundvol_Arx_Sewers_DeathfogMachine_Active"|"State_Dialogue"
--- @alias SwitchGroupName "Armor_Type"|"Bear"|"Boar"|"Burning_Witch"|"Cat"|"Chicken"|"Deer"|"Dog"|"Dragon"|"Drillworm_Hatchlings"|"Elemental_Ooze"|"Items_Material"|"Items_Objects_TOOL_Ladder_Material"|"Items_SurfaceType"|"Items_Weight"|"Movement_FX_Type"|"PlayerType"|"Raanaar_Automaton"|"Race"|"Sex"|"Skill_CharacterType"|"Spider"|"Steps_Speed"|"Steps_Terrain"|"Steps_Type"|"Steps_Weight"|"Tiger"|"Troll"|"Variation"|"Vocal_Combat_Type"|"WarOwl"|"Weapon_Action"|"Weapon_Hit_Armor_Type"|"Weapon_Hit_Bloodtype"|"Weapon_Hit_Material_Type"|"Weapon_Material"|"Weapon_Race"|"Weapon_Type"|"Whoosh_Magic"|"Whoosh_Type"|"Whoosh_Weight"|"Wolf"

--- @alias LevelMapName "Armor ArmorValue"|"Armor ConstitutionBoost"|"Armor FinesseBoost"|"Armor HearingBoost"|"Armor IntelligenceBoost"|"Armor MagicArmorValue"|"Armor MagicPointsBoost"|"Armor MemoryBoost"|"Armor SightBoost"|"Armor StrengthBoost"|"Armor Value"|"Armor VitalityBoost"|"Armor WitsBoost"|"ArmorUsageSkill"|"Character Act Strength"|"Character AirSpecialist"|"Character Armor"|"Character Constitution"|"Character Critical Chance"|"Character DualWielding"|"Character EarthSpecialist"|"Character Finesse"|"Character FireSpecialist"|"Character Gain"|"Character Hearing"|"Character Intelligence"|"Character Leadership"|"Character MagicArmor"|"Character Memory"|"Character Necromancy"|"Character Polymorph"|"Character Ranged"|"Character RangerLore"|"Character RogueLore"|"Character Sight"|"Character SingleHanded"|"Character Sourcery"|"Character Strength"|"Character Summoning"|"Character Telekinesis"|"Character TwoHanded"|"Character WarriorLore"|"Character WaterSpecialist"|"Character Wits"|"EmbellishSkill"|"IdentifyRangeSkill"|"Object Armor"|"Object Constitution"|"Object MagicArmor"|"Object Value"|"ObjectDurabilitySkill"|"Potion Armor"|"Potion Constitution"|"Potion Damage"|"Potion Finesse"|"Potion Gain"|"Potion Hearing"|"Potion Intelligence"|"Potion MagicArmor"|"Potion Memory"|"Potion Strength"|"Potion Value"|"Potion Vitality"|"Potion Wits"|"RepairRangeSkill"|"RewardExperience"|"Shield ArmorValue"|"Shield Blocking"|"Shield ConstitutionBoost"|"Shield FinesseBoost"|"Shield HearingBoost"|"Shield IntelligenceBoost"|"Shield MagicArmorValue"|"Shield MagicPointsBoost"|"Shield MemoryBoost"|"Shield SightBoost"|"Shield StrengthBoost"|"Shield Value"|"Shield VitalityBoost"|"Shield WitsBoost"|"SkillData AreaRadius"|"SkillData BackStart"|"SkillData ChanceToPierce"|"SkillData Duration"|"SkillData EndPosRadius"|"SkillData ExplodeRadius"|"SkillData ForkChance"|"SkillData FrontOffset"|"SkillData GrowSpeed"|"SkillData GrowTimeout"|"SkillData HealAmount"|"SkillData Height"|"SkillData HitPointsPercent"|"SkillData HitRadius"|"SkillData Lifetime"|"SkillData MaxDistance"|"SkillData NextAttackChance"|"SkillData NextAttackChanceDivider"|"SkillData Offset"|"SkillData Radius"|"SkillData Range"|"SkillData StatusChance"|"SkillData StatusClearChance"|"SkillData StatusLifetime"|"SkillData SurfaceRadius"|"SkillData TargetRadius"|"SkillData TravelSpeed"|"StatusData Radius"|"Value"|"Weapon ConstitutionBoost"|"Weapon Damage"|"Weapon FinesseBoost"|"Weapon HearingBoost"|"Weapon IntelligenceBoost"|"Weapon MagicPointsBoost"|"Weapon MemoryBoost"|"Weapon SightBoost"|"Weapon StrengthBoost"|"Weapon Value"|"Weapon VitalityBoost"|"Weapon WitsBoost"|"WisdomSkill"

--- @alias StatsHealValueType "FixedValue"|"Percentage"|"Qualifier"|"Shield"|"TargetDependent"|"DamagePercentage"

]]

--- @return GenerateIdeHelpersGenerator
function Generator:New()
    local o = {}
    setmetatable(o, self)
    o.Intrinsics = {}
    o.Builtins = {}
    o.Enumerations = {}
    o.Classes = {}
    o.Modules = {}
    o.NativeClasses = {}
    o.NativeModules = {}
    o.Text = startingText
    self.__index = self
    return o
end

function Generator:LoadNativeData()
    self.NativeClasses = {}
    self.NativeModules = {}

    local status, res = xpcall(function () 
        local libsJson = Ext.Utils.Include(nil, 'builtin://Libs/IdeHelpersNativeData.lua')
        local libs = Ext.Json.Parse(libsJson)

        return libs
    end, debug.traceback)

    if status == true then
        self.NativeClasses = res.classes
    
        for name,mod in pairs(res.modules) do
            self.NativeModules[name] = mod
        end
    else
        Ext.Utils.PrintWarning("Unable to load native class data; IDE helpers will not include annotations from C++ code")
        Ext.Utils.PrintWarning(res)
    end
end

local _ModuleToClassField = {
    ["Module_Stats.DeltaMod"] = "Module_Stats",
    ["Module_Stats.EquipmentSet"] = "Module_Stats",
    ["Module_Stats.ItemColor"] = "Module_Stats",
    ["Module_Stats.ItemCombo"] = "Module_Stats",
    ["Module_Stats.ItemComboPreview"] = "Module_Stats",
    ["Module_Stats.ItemComboProperty"] = "Module_Stats",
    ["Module_Stats.ItemGroup"] = "Module_Stats",
    ["Module_Stats.ItemSet"] = "Module_Stats",
    ["Module_Stats.NameGroup"] = "Module_Stats",
    ["Module_Stats.SkillSet"] = "Module_Stats",
    ["Module_Stats.TreasureCategory"] = "Module_Stats",
    ["Module_Stats.TreasureTable"] = "Module_Stats",
    ["Module_Stats.Math"] = "Module_Stats",
    ["Module_ServerSurface.Action"] = "Module_ServerSurfaceAction",
}

--- @param opts GenerateIdeHelpersOptions
function Generator:GenerateExtraData(opts)
    self:EmitLine("--#region ExtraData")
    self:EmitEmptyLine()

    local baseKeys = {"AbilityBaseValue", "AbilityMagicArmorBonusBase", "AbilityMagicArmorBonusMax", "AbilityMagicArmorBonusPerPoint", "AbilityPerseveranceArmorPerPoint", "AbilityPersuasionBonusPerPoint", "AbilityPhysArmorBonusBase", "AbilityPhysArmorBonusMax", "AbilityPhysArmorBonusPerPoint", "AbilityVitalityBonusBase", "AbilityVitalityBonusMax", "AbilityVitalityBonusPerPoint", "AiCoverProjectileTurnMemory", "Ally Joins Ally SightRange Multiplier", "ArmorAfterHitCooldown", "ArmorAmuletPercentage", "ArmorBeltPercentage", "ArmorFeetPercentage", "ArmorHandsPercentage", "ArmorHeadPercentage", "ArmorLowerBodyPercentage", "ArmorRegenConstGrowth", "ArmorRegenPercentageGrowth", "ArmorRegenTimer", "ArmorRingPercentage", "ArmorShieldPercentage", "ArmorToVitalityRatio", "ArmorUpperBodyPercentage", "AttributeBaseValue", "AttributeBoostGrowth", "AttributeCharCreationBonus", "AttributeGrowthDamp", "AttributeLevelGrowth", "AttributeSoftCap", "BlindRangePenalty", "Burn Contact Status Duration", "CarryWeightBase", "CarryWeightPerStr", "ChanceToSetStatusOnContact", "CharacterAttributePointsPerMemoryCapacity", "CharacterBaseMemoryCapacity", "CharacterBaseMemoryCapacityGrowth", "CharacterWeightHeavy", "CharacterWeightLight", "CharacterWeightMedium", "Chill Contact Status Duration", "CivilAbilityCap", "CivilAbilityLevelGrowth", "CivilPointOffset", "CleaveRangeOverride", "CombatAbilityAccuracyBonus", "CombatAbilityCap", "CombatAbilityCritBonus", "CombatAbilityCritMultiplierBonus", "CombatAbilityDamageBonus", "CombatAbilityDodgingBonus", "CombatAbilityLevelGrowth", "CombatAbilityNpcGrowth", "CombatAbilityReflectionBonus", "CriticalBonusFromWits", "DamageBoostFromAttribute", "DamageToThrownWeightRatio", "Decaying Touch Damage Modifier", "DeflectProjectileRange", "DodgingBoostFromAttribute", "DualWieldingAPPenalty", "DualWieldingDamagePenalty", "End Of Combat SightRange Multiplier", "EntangledContactStatusDuration", "ExpectedConGrowthForArmorCalculation", "ExpectedDamageBoostFromAttributePerLevel", "ExpectedDamageBoostFromSkillAbilityPerLevel", "ExpectedDamageBoostFromWeaponAbilityPerLevel", "FirstItemTypeShift", "FirstPriceLeapGrowth", "FirstPriceLeapLevel", "FirstVitalityLeapGrowth", "FirstVitalityLeapLevel", "Flanked penalty", "FleeDistance", "FourthPriceLeapGrowth", "FourthPriceLeapLevel", "FourthVitalityLeapGrowth", "FourthVitalityLeapLevel", "FreeMovementDistanceWhenAttacking", "Freeze Contact Status Duration", "GhostLifeTime", "GlobalGoldValueMultiplier", "GMCharacterAPCap", "GMCharacterArmorCap", "GMCharacterAttributeCap", "GMCharacterResistanceMax", "GMCharacterResistanceMin", "GMCharacterSPCap", "GMItemArmorMax", "GMItemArmorMin", "GMItemAttributeCap", "GMItemLevelCap", "GMItemResistanceMax", "GMItemResistanceMin", "Haste Speed Modifier", "HealToDamageRatio", "HighGroundBaseDamageBonus", "HighGroundMeleeRange", "HighGroundRangeMultiplier", "HighGroundThreshold", "HintDuration", "IncarnateSummoningLevel", "Infectious Disease Depth", "Infectious Disease Radius", "InitiativeBonusFromWits", "LeadershipAllResBonus", "LeadershipDodgingBonus", "LeadershipRange", "LevelCap", "LifestealFromReflectionModifier", "LivingArmorHealPercentage", "LoneWolfAPBonus", "LoneWolfArmorBoostPercentage", "LoneWolfMagicArmorBoostPercentage", "LoneWolfMaxAPBonus", "LoneWolfVitalityBoostPercentage", "LoremasterBonusToMemory", "LowGroundBaseDamagePenalty", "MagicArmorAfterHitCooldown", "MagicArmorRegenConstGrowth", "MagicArmorRegenPercentageGrowth", "MagicArmorRegenTimer", "MagicArmourBoostFromAttribute", "Max Throw Distance", "MaximumSummonsInCombat", "Min Throw Distance", "MonsterDamageBoostPerLevel", "MoveToCarryWeightRatio", "NPC max combat turn time", "NumStartingCivilAbilityPoints", "NumStartingCombatAbilityPoints", "Offhand Attack Shield Block", "Oiled Chance to Burn Boost", "Once Per Combat Skill Realtime Cooldown", "Painted surface status chance", "PersuasionAttitudeBonusPerPoint", "PhysicalArmourBoostFromAttribute", "PickpocketExperienceLevelsPerPoint", "PickpocketGoldValuePerPoint", "PickpocketRequirementDecreaseFromFinesse", "PickpocketWeightPerPoint", "Poison Contact Status Duration", "PoisonedFoodDamage", "PoisonedFoodDamageMultiplier", "PoisonedFoodDamageRange", "PriceAttitudeCoefficient", "PriceBarterCoefficient", "PriceGrowth", "PriceModCasualDifficulty", "PriceModClassicDifficulty", "PriceModHardcoreDifficulty", "PriceRoundToFiveAfterAmount", "PriceRoundToTenAfterAmount", "Projectile Terrain Offset", "RangeBoostedGlobalCap", "SavethrowBelowLowPenalty", "SavethrowHighChance", "SavethrowLowChance", "SavethrowPenaltyCap", "SecondItemTypeShift", "SecondPriceLeapGrowth", "SecondPriceLeapLevel", "SecondVitalityLeapGrowth", "SecondVitalityLeapLevel", "Shackles Of Pain Damage Modifier", "ShieldAPCost", "Skill Range Per Ability", "SkillAbilityAirDamageBoostPerPoint", "SkillAbilityArmorRestoredPerPoint", "SkillAbilityCritMultiplierPerPoint", "SkillAbilityDamageToMagicArmorPerPoint", "SkillAbilityDamageToPhysicalArmorPerPoint", "SkillAbilityFireDamageBoostPerPoint", "SkillAbilityHighGroundBonusPerPoint", "SkillAbilityLifeStealPerPoint", "SkillAbilityMovementSpeedPerPoint", "SkillAbilityPhysicalDamageBoostPerPoint", "SkillAbilityPoisonAndEarthDamageBoostPerPoint", "SkillAbilitySummonsStatsPerPoint", "SkillAbilityVitalityRestoredPerPoint", "SkillAbilityWaterDamageBoostPerPoint", "SkillCombustionRadius", "SkillHeightRangeMultiplier", "SkillMemoryCostReductionFromAbility", "Slow Speed Modifier", "SmokeDurationAfterDecay", "Sneak Damage Multiplier", "SneakDefaultAPCost", "SneakingAbilityMovementSpeedPerPoint", "SneakSpeedBoost", "SoftLevelCap", "SpiritVisionFallbackRadius", "StatusDefaultDistancePerDamage", "Stun Contact Status Duration", "Summon Life Link Damage Modifier", "SummoningAbilityBonus", "Surface Clear Owner Time", "Surface Distance Evaluation", "SurfaceAbsorbBoostPerTilesCount", "SurfaceDurationAfterDecay", "SurfaceDurationBlessedCursed", "SurfaceDurationFireIgniteOverride", "SurfaceDurationFromCharacterBleeding", "SurfaceDurationFromHitFloorReaction", "TalentAttributePointsBonus", "TalentCivilAbilityPointsBonus", "TalentCombatAbilityPointsBonus", "TalentExecutionerActionPointBonus", "TalentHumanCriticalChance", "TalentHumanCriticalMultiplier", "TalentMemoryBonus", "TalentPerfectionistAccuracyBonus", "TalentPerfectionistCriticalChanceBonus", "TalentPointOffset", "TalentPointPerLevels", "TalentQuestRootedMemoryBonus", "TalentQuickStepPartialApBonus", "TalentRagerPercentage", "TalentResistDeathVitalityPercentage", "TalentResurrectExtraHealthPercent", "TalentSneakingAPCost", "TalentSneakingDamageBonus", "TalentViolentMagicCriticalChancePercent", "TalentWhatARushThreshold", "TargetCondition LowHP Percentage", "Telekinesis Range", "TeleportUnchainDistance", "ThirdPriceLeapGrowth", "ThirdPriceLeapLevel", "ThirdVitalityLeapGrowth", "ThirdVitalityLeapLevel", "TorturerDamageStatusTurnIncrease", "TraderDonationsRequiredAttitude", "TraderDroppedItemsCap", "TraderDroppedItemsPercentage", "UnstableDamagePercentage", "UnstableRadius", "VitalityBoostFromAttribute", "VitalityExponentialGrowth", "VitalityLinearGrowth", "VitalityStartingAmount", "VitalityToDamageRatio", "VitalityToDamageRatioGrowth", "WandUsesMax", "WeaponAccuracyPenaltyCap", "WeaponAccuracyPenaltyPerLevel", "WeaponWeightHeavy", "WeaponWeightLight", "WeaponWeightMedium", "WitsGrowthDamp"}
    
    local extraData = Ext.Stats.GetStatsManager().ExtraData
    if opts.UseBaseExtraData then
        if not opts.GenerateExtraDataAsClass then
            self:EmitLine(_Deprecated.BaseExtraData)
        else
            self:EmitComment("@class BaseExtraData")

            local keysWithWhitespace = {}

            for _,k in ipairs(baseKeys) do
                if string.find(k, "%s") then
                    keysWithWhitespace[#keysWithWhitespace+1] = k
                else
                    self:EmitFieldComment(k .. " number")
                end
            end
            if #keysWithWhitespace > 0 then
                self:EmitLine("local BaseExtraData = {")
                for _,k in ipairs(keysWithWhitespace) do
                    self:EmitLine("\t[\"" .. k .. "\"] = " .. tostring(extraData[k] or 0) .. ",")
                end
                self:EmitLine("}")
            else
                self:EmitLine("local BaseExtraData = {}")
            end
            self:EmitEmptyLine()
            self:EmitEmptyLine()
            self:EmitComment("@class ExtraData:BaseExtraData")
            self:EmitLine("Ext.ExtraData = {}")
        end
    else
        local baseKeyDict = {}
        for _,v in pairs(baseKeys) do
            baseKeyDict[v] = true
        end
        local newKeys = {}

        for k,v in pairs(extraData) do
            if not baseKeyDict[k] then
                newKeys[#newKeys+1] = k
            end
        end
        table.sort(newKeys)

        if not opts.GenerateExtraDataAsClass then
            self:EmitLine("Ext.ExtraData = {")
            for _,k in ipairs(baseKeys) do
                self:EmitLine(string.format("\t[\"%s\"] = %s,", k, extraData[k]))
            end
    
    
            for _,k in ipairs(newKeys) do
                self:EmitLine(string.format("\t[\"%s\"] = %s,", k, extraData[k]))
            end
    
            self:EmitLine("}")
        else
            self:EmitComment("@class BaseExtraData")

            local keysWithWhitespace = {}

            for _,k in ipairs(baseKeys) do
                if string.find(k, "%s") then
                    keysWithWhitespace[#keysWithWhitespace+1] = k
                else
                    self:EmitFieldComment(k .. " number")
                end
            end
            if #keysWithWhitespace > 0 then
                self:EmitLine("local BaseExtraData = {")
                for _,k in ipairs(keysWithWhitespace) do
                    self:EmitLine("\t[\"" .. k .. "\"] = " .. tostring(extraData[k] or 0) .. ",")
                end
                self:EmitLine("}")
            else
                self:EmitLine("local BaseExtraData = {}")
            end

            self:EmitEmptyLine()
            self:EmitEmptyLine()

            keysWithWhitespace = {}

            self:EmitComment("@class ExtraData:BaseExtraData")
            for _,k in ipairs(newKeys) do
                if string.find(k, "%s") then
                    keysWithWhitespace[#keysWithWhitespace+1] = k
                else
                    self:EmitFieldComment(k .. " number")
                end
            end
            if #keysWithWhitespace > 0 then
                self:EmitLine("Ext.ExtraData = {")
                for _,k in ipairs(keysWithWhitespace) do
                    self:EmitLine("\t[\"" .. k .. "\"] = " .. tostring(extraData[k] or 0) .. ",")
                end
                self:EmitLine("}")
            else
                self:EmitLine("Ext.ExtraData = {}")
            end
            self:EmitEmptyLine()
        end

    end
    self:EmitEmptyLine()
    self:EmitLine("--#endregion")
    self:EmitEmptyLine()
end

--- @param opts GenerateIdeHelpersOptions
function Generator:Build(opts)
    local types = Ext.Types.GetAllTypes()
    local sortedTypes = {}

    for i,typeName in ipairs(types) do
        table.insert(sortedTypes, typeName)
    end

    table.sort(sortedTypes)

    local moduleToClassField = {}
    local removeModuleFromExt = {}

    for i,typeName in ipairs(sortedTypes) do
        local type = Ext.Types.GetTypeInfo(typeName)
        if type.Kind == "Object" then
            table.insert(self.Classes, type)
        elseif type.Kind == "Module" then
            local index = #self.Modules+1
            if _ModuleToClassField[typeName] then
                local targetTypeName = _ModuleToClassField[typeName]
                if moduleToClassField[targetTypeName] == nil then
                    moduleToClassField[targetTypeName] = {}
                end
                removeModuleFromExt[typeName] = index
                table.insert(moduleToClassField[targetTypeName], type)
            end
            self.Modules[index] = type
        elseif type.Kind == "Enumeration" then
            table.insert(self.Enumerations, type)
        elseif type.Kind == "Boolean" or type.Kind == "Integer" or type.Kind == "Float" or type.Kind == "String" or type.Kind == "Any" then
            table.insert(self.Intrinsics, type)
        elseif typeName == "vec2" or typeName == "vec3" or typeName == "vec4" or typeName == "ivec2" or typeName == "ivec3" or typeName == "mat3" or typeName == "mat3x4" or typeName == "mat4x3" or typeName == "mat4" or typeName == "Version" then
            table.insert(self.Builtins, type)
        end
    end

    for i,type in ipairs(self.Intrinsics) do
        self:EmitIntrinsicType(type)
    end

    for i,type in ipairs(self.Builtins) do
        self:EmitBuiltinType(type)
    end

    self:EmitEmptyLine()
    self:EmitEmptyLine()

    if opts.AddAliasEnums then
        for i,type in ipairs(self.Enumerations) do
            self:EmitEnumeration(type)
        end
    end

    self:EmitEmptyLine()
    self:EmitEmptyLine()

    self:EmitLine(_Flash)

    for i,type in ipairs(self.Classes) do
        self:EmitClass(type)
        self:EmitEmptyLine()
        self:EmitEmptyLine()
    end

    for i,type in ipairs(self.Modules) do
        self:EmitModule(type, moduleToClassField[type.TypeName])
        self:EmitEmptyLine()
        self:EmitEmptyLine()
    end

    local modules = {}
    for i,v in pairs(self.Modules) do
        if not removeModuleFromExt[v.TypeName] then
            modules[#modules+1] = v
        end
    end

    self.Modules = modules
    
    self:EmitExt("Client")
    self:EmitEmptyLine()
    self:EmitExt("Server")
    self:EmitEmptyLine()
    self:EmitExt(nil, true)
    if opts.AddOsiris then
        self:EmitLine(_OsiLines)
    end

    if opts.AddDeprecated then
        self:EmitLine(_Deprecated.Functions)
    end

    self:GenerateExtraData(opts)
end

function Generator:MakeTypeName(type)
    -- Replace "_" with capitalization (mainly used for stats class names)
    type = string.gsub(type, "_[%a%d]", function (ns)
        return string.upper(string.sub(ns, 2))
    end)
    -- Replace namespace "::" with capitalization
    type = string.gsub(type, "[%a%d]+::", function (ns)
        return string.upper(string.sub(ns, 1, 1)) .. string.sub(ns, 2, -3)
    end)
    -- Replace template classes "T<V>" with underscore ("T_V")
    type = string.gsub(type, "<[%a%d]+>", function (n)
        return "_" .. string.sub(n, 2, -2)
    end)
    if _TypeReplace[type] then
        return _TypeReplace[type]
    end
    return type
end

local function _TableIsNullOrEmpty(tbl)
    return tbl == nil or tbl[1] == nil
end

function Generator:MakeTypeSignature(cls, type, forceExpand, nativeDefn)
    if _TypeReplace[type.TypeName] then
        return _TypeReplace[type.TypeName]
    end
    if type.IsBuiltin and forceExpand ~= true then
        return type.TypeName
    elseif type.Kind == "Any" then
        return "any"
    elseif type.Kind == "Nullable" then
        return self:MakeTypeSignature(cls, type.ParentType) .. "|nil"
    elseif type.Kind == "Array" then
        if type.ElementType.Kind == "Array" or type.ElementType.Kind == "Map" then
            return self:MakeTypeSignature(nil, type.ElementType) .. "[]"
        else
            return self:MakeTypeName(type.ElementType.TypeName) .. "[]"
        end
    elseif type.Kind == "Map" then
        return "table<" .. self:MakeTypeName(type.KeyType.TypeName) .. ", " .. self:MakeTypeSignature(nil, type.ElementType) .. ">"
    elseif type.Kind == "Function" then
        local args = {}
        local retval = {}

        local clsName = ""
        if cls ~= nil then
            clsName = self:MakeTypeSignature(nil, cls)
            table.insert(args, "self:" .. clsName)
        end

        local missingFuncData = {}
        if _FuncData.Regular[clsName] and _FuncData.Regular[clsName][type.TypeName] then
            missingFuncData = _FuncData.Regular[clsName][type.TypeName]
        end
        
        if _TableIsNullOrEmpty(type.Params) and missingFuncData.Params then
            for i,data in ipairs(missingFuncData.Params) do
                table.insert(args, data.name .. ":" .. data.arg)
            end
        else
            for i,arg in ipairs(type.Params) do
                if nativeDefn ~= nil then
                    table.insert(args, nativeDefn.params[i].name .. ":" .. self:MakeTypeSignature(cls, arg))
                else
                    table.insert(args, "a" .. i .. ":" .. self:MakeTypeSignature(cls, arg))
                end
            end
        end

        if not _TableIsNullOrEmpty(missingFuncData.Return) then
            for i,arg in ipairs(missingFuncData.Return) do
                table.insert(retval, arg)
            end
        else
            for i,arg in ipairs(type.ReturnValues) do
                table.insert(retval, self:MakeTypeSignature(cls, arg))
            end
        end

        local fun = "fun(" .. table.concat(args, ", ") .. ")"
        if #retval > 0 then
            fun = fun .. ":" .. table.concat(retval, ", ")
        end

        return fun
    else
        return self:MakeTypeName(type.TypeName)
    end
end

function Generator:EmitEmptyLine()
    self.Text = self.Text .. NEWLINE
end

function Generator:EmitLine(text, addNewLine)
    self.Text = self.Text .. text .. NEWLINE
    if addNewLine then
        self.Text = self.Text .. NEWLINE
    end
end

function Generator:EmitComment(text)
    self.Text = self.Text .. "--- " .. text .. NEWLINE
end

function Generator:EmitFieldComment(text)
    self.Text = self.Text .. "--- @field " .. text .. NEWLINE
end

function Generator:EmitMultiLineComment(text)
    for line in text:gmatch("([^\n]+)") do
       self:EmitComment(self.RTrim(line))
    end
end

function Generator:EmitAlias(name, definition)
    if not _TypeAliasRemove[name] then
        local def = _TypeAliasReplace[name] or definition
        self:EmitComment("@alias " .. self:MakeTypeName(name) .. " " .. def)
    end
end

function Generator:EmitIntrinsicType(type)
    self:EmitAlias(type.TypeName, self.ValueKindToLua[type.Kind])
end

function Generator:EmitBuiltinType(type)
    self:EmitAlias(type.TypeName, self:MakeTypeSignature(nil, type, true))
end

function Generator:EmitEnumeration(type)
    local decl = "string"
    
    for key,value in pairs(type.EnumValues) do
        decl = _format("%s|\"%s\"", decl, key)
    end
    self:EmitAlias(type.TypeName, decl)
end

function Generator:FindNativeClass(type)
    if type.NativeName ~= nil then
        local name = string.gsub(type.NativeName, "struct ", '')
        name = string.gsub(name, "class ", '')
        return self.NativeClasses[name]
    end

    return nil
end

function Generator:FindNativeMethod(fname, nativeCls)
    local nativeMethod = nil
    if nativeCls ~= nil then
        nativeMethod = nativeCls.methods[fname]
        if nativeMethod == nil then
            nativeMethod = nativeCls.methods["Lua" .. fname]
        end
    end

    return nativeMethod
end

function Generator:FindNativeFunction(fname, nativeNs)
    local nativeMethod = nil
    if nativeNs ~= nil then
        nativeMethod = nativeNs.functions[fname]
    end

    return nativeMethod
end

function Generator:MethodNeedsFullSignature(nativeMethod)
    if nativeMethod == nil then
        return false
    end

    if #self.Trim(nativeMethod.description) > 0 then
        return true
    end

    for i,fun in pairs(nativeMethod.params) do
        if #self.Trim(nativeMethod.description) > 0 then
            return true
        end
    end

    return false
end

function Generator:EmitMethod(type, fname, nativeDefn)
    local nativeMethod = self:FindNativeMethod(fname, nativeDefn)

    if self:MethodNeedsFullSignature(nativeMethod) then
        self:EmitFullMethodSignature(type, fname, type.Methods[fname], nativeMethod)
    else
        local clsName = self:MakeTypeSignature(nil, type)
        local fieldOverride = _FuncData.Field[clsName] and _FuncData.Field[clsName][fname]
        if fieldOverride then
            self:EmitFieldComment(fname .. " " .. fieldOverride)
        else
            self:EmitFieldComment(fname .. " " .. self:MakeTypeSignature(type, type.Methods[fname],  false, nativeMethod))
        end
    end
end

function Generator:EmitModuleFunction(type, fname, nativeDefn, afterText)
    local nativeFunc = self:FindNativeFunction(fname, nativeDefn)

    if nativeFunc == nil then
        local typeSig = ""
        local clsName = self:MakeModuleTypeName(type)
        if _FuncData.Field[clsName] and _FuncData.Field[clsName][fname] then
            typeSig = _FuncData.Field[clsName][fname]
        else
            typeSig = self:MakeTypeSignature(nil, type.Methods[fname])
        end
        self:EmitFieldComment(fname .. " " .. typeSig)
    else
        self:EmitFullMethodSignature(type, fname, type.Methods[fname], nativeFunc, afterText)
    end
end

function Generator:EmitFullMethodSignature(cls, funcName, fun, nativeMethod, afterText)
    local argDescs = {}
    local args = {}
    local overloads = {}

    local clsName
    if cls.Kind == "Module" then
        clsName = self:MakeModuleTypeName(cls)
    else
        clsName = self:MakeTypeName(cls.TypeName)
    end

    local helpersClsName = clsName:gsub("%.", "")

    local missingFuncData = {}
    if _FuncData.Regular[clsName] and _FuncData.Regular[clsName][funcName] then
        missingFuncData = _FuncData.Regular[clsName][funcName]
    end

    if missingFuncData.Params and (_TableIsNullOrEmpty(fun.Params) or missingFuncData.Override) then
        if missingFuncData.Overload then
            overloads = missingFuncData.Overload
        end
        for i,data in ipairs(missingFuncData.Params) do
            if data.name == "..." then
                table.insert(argDescs, "--- @vararg " .. data.arg or "any" .. " " ..  self.Trim(data.description or ""))
            else
                table.insert(argDescs, "--- @param " .. data.name .. " " .. data.arg .. " " ..  self.Trim(data.description or ""))
            end
            table.insert(args, data.name)
        end
    else
        for i,arg in ipairs(fun.Params) do
            table.insert(argDescs, "--- @param " .. nativeMethod.params[i].name .. " " .. self:MakeTypeSignature(cls, arg) .. " " ..  self.Trim(nativeMethod.params[i].description))
            table.insert(args, nativeMethod.params[i].name)
        end
    end

    if not _TableIsNullOrEmpty(missingFuncData.Return) then
        for i,arg in ipairs(missingFuncData.Return) do
            table.insert(argDescs, "--- @return " .. arg)
        end
    else
        for i,arg in ipairs(fun.ReturnValues) do
            table.insert(argDescs, "--- @return " .. self:MakeTypeSignature(cls, arg))
        end
    end

    local fun = "function " .. helpersClsName

    if cls.Kind ~= "Module" then
        fun = fun .. ":"
    else
        fun = fun .. "."
    end

    fun = fun .. funcName .. "(" .. table.concat(args, ", ") .. ") end"
    local desc = table.concat(argDescs, NEWLINE)

    if missingFuncData.Description then
        desc = missingFuncData.Description .. NEWLINE .. desc
    end

    local overloadsLen = #overloads
    if overloadsLen > 0 then
        local text = ""
        for i=1,overloadsLen do
            text = text .. overloads[i]
        end

        -- Put overloads above the params/return annotations
        text = text .. "\n" .. desc
        desc = text
    end

    local funcDesc = self.Trim(nativeMethod.description)
    if nativeMethod.implementation_file ~= nil and #funcDesc > 0 then
        funcDesc = funcDesc .. NEWLINE .. "Location: " .. nativeMethod.implementation_file .. ":" .. nativeMethod.implementation_line
    end

    if #funcDesc > 0 then
        self:EmitMultiLineComment(funcDesc)
    end

    if desc ~= "" then
        self.Text = self.Text .. desc .. NEWLINE .. fun
    else
        self.Text = self.Text .. fun
    end
    if afterText then
        self.Text = self.Text .. NEWLINE .. afterText
    end
    self.Text = self.Text .. NEWLINE .. NEWLINE
end

local serverEventParamsPattern = "EsvLua(%a+)Event"
local clientEventParamsPattern = "EclLua(%a+)Event"
local bothContextEventParamsPattern = "(%a+)Event"

---@type table<integer, {Type:string, Event:string, Context:string|"server"|"client"|"any"}>
local eventTypeGenerationData = {
    ---TODO Still uses the old callback, so Ext.Events.CalculateTurnOrder is never thrown.
    --CalculateTurnOrder = {Type="", Event="CalculateTurnOrder", Context="server"},
    --NetMessageReceived = {Type="LuaNetMessageEvent", Event="NetMessageReceived", Context="any"},
}

---Event name to index in eventTypeGenerationData
---@type table<string, integer>
local eventTypeGenerationDataIndex = {}

local EVENT_NAME_SWAP = {
    GameStateChange = "GameStateChanged",
    LuaTick = "Tick",
    LuaConsole = "DoConsoleCommand",
    AiRequestPeek = "OnPeekAiAction",
    ExecutePropertyDataOnTarget = "OnExecutePropertyDataOnTarget",
    ExecutePropertyDataOnPosition = "OnExecutePropertyDataOnPosition",
    ExecutePropertyDataOnGroundHit = "GroundHit",
    SkillGetDescription = "SkillGetDescriptionParam",
    StatusGetDescription = "StatusGetDescriptionParam",
    Input = "InputEvent",
    LuaNetMessage = "NetMessageReceived",
}

local IGNORE_PARAMS = {
    LuaEmptyEvent = true
}


function Generator:EmitClass(type, moduleToClassField)
    local name = self:MakeTypeName(type.TypeName)
    local nameWithParent = name
    -- if _PreventableEvent[name] then
    --     nameWithParent = name .. ":PreventableSubscribableEventParams"
    -- end
    if type.ParentType ~= nil then
        nameWithParent = nameWithParent .. ":" .. self:MakeTypeName(type.ParentType.TypeName)
    end

    local nativeDefn = self:FindNativeClass(type)

    local customText = _CustomTypeEntries[name]
    --TODO Hacky output fixes [_CustomTypeEntries.Before(EmitClass)]. May not be required later on.
    if customText and customText.Before then
        self:EmitLine(customText.Before)
    end

    self:EmitComment("@class " .. nameWithParent)

    local sortedMembers = {}
    for fname,ftype in pairs(type.Members) do table.insert(sortedMembers, fname) end
    table.sort(sortedMembers)

    for i,fname in ipairs(sortedMembers) do
        local typeSig = self:MakeTypeSignature(type, type.Members[fname])
        local fieldOverride = _FuncData.Field[name] and _FuncData.Field[name][fname]
        if _Aliases[name] then
            if _Aliases[name][fname] then
                typeSig = _Aliases[name][fname]
            end
        elseif fieldOverride then
            typeSig = fieldOverride
        end
        self:EmitFieldComment(fname .. " " .. typeSig)
    end

    if customText and customText.Fields then
        for i,v in ipairs(customText.Fields) do
            self:EmitFieldComment(v)
        end
    end

    local sortedMethods = {}
    for fname,ftype in pairs(type.Methods) do table.insert(sortedMethods, fname) end
    table.sort(sortedMethods)

    local basicMethodSigs = {}
    local extendedMethodSigs = {}

    for i,fname in ipairs(sortedMethods) do
        local nativeMethod = self:FindNativeMethod(fname, nativeDefn)
        if self:MethodNeedsFullSignature(nativeMethod) then
            table.insert(extendedMethodSigs, fname)
        else
            table.insert(basicMethodSigs, fname)
        end
    end
    
    for i,fname in ipairs(basicMethodSigs) do
        self:EmitMethod(type, fname, nativeDefn)
    end

    if #extendedMethodSigs > 0 then
        self:EmitLine('local ' .. name .. ' = {}')
        self:EmitLine("")
        
        for i,fname in ipairs(extendedMethodSigs) do
            self:EmitMethod(type, fname, nativeDefn)
        end
    end
    if not IGNORE_PARAMS[name] and type.ParentType and type.ParentType.TypeName == "lua::EventBase" then
        local context = "any"
        local _,_,eventName = string.find(name, serverEventParamsPattern)
        if not eventName then
            _,_,eventName = string.find(name, clientEventParamsPattern)
            if eventName then
                context = "client"
            else
                _,_,eventName = string.find(name, bothContextEventParamsPattern)
            end
        else
            context = "server"
        end
        if eventName then
            if eventName == "AiRequestSort" then
                local lastIndex = #eventTypeGenerationData+1
                eventTypeGenerationData[lastIndex] = {Type = name, Event = "OnBeforeSortAiActions", Context = context}
                eventTypeGenerationDataIndex["OnBeforeSortAiActions"] = lastIndex
                lastIndex = lastIndex+1
                eventTypeGenerationData[lastIndex] = {Type = name, Event = "OnAfterSortAiActions", Context = context}
                eventTypeGenerationDataIndex["OnAfterSortAiActions"] = lastIndex
            elseif eventName == "StatusDelete" then
                local lastIndex = #eventTypeGenerationData+1
                eventTypeGenerationData[lastIndex] = {Type = name, Event = "StatusDelete", Context = context}
                eventTypeGenerationDataIndex["StatusDelete"] = lastIndex
                lastIndex = lastIndex+1
                eventTypeGenerationData[lastIndex] = {Type = name, Event = "BeforeStatusDelete", Context = context}
                eventTypeGenerationDataIndex["BeforeStatusDelete"] = lastIndex
            elseif eventName == "UICall" then
                local lastIndex = #eventTypeGenerationData+1
                eventTypeGenerationData[lastIndex] = {Type = name, Event = "UICall", Context = context}
                eventTypeGenerationDataIndex["UICall"] = lastIndex
                lastIndex = lastIndex+1
                eventTypeGenerationData[lastIndex] = {Type = name, Event = "UIInvoke", Context = context}
                eventTypeGenerationDataIndex["UIInvoke"] = lastIndex
                lastIndex = lastIndex+1
                eventTypeGenerationData[lastIndex] = {Type = name, Event = "AfterUIInvoke", Context = context}
                eventTypeGenerationDataIndex["AfterUIInvoke"] = lastIndex
                lastIndex = lastIndex+1
                eventTypeGenerationData[lastIndex] = {Type = name, Event = "AfterUICall", Context = context}
                eventTypeGenerationDataIndex["AfterUICall"] = lastIndex
            else
                if EVENT_NAME_SWAP[eventName] then
                    eventName = EVENT_NAME_SWAP[eventName]
                else
                    eventName = eventName:gsub("^Lua", "")
                end
                local lastIndex = eventTypeGenerationDataIndex[eventName]
                if lastIndex == nil then
                    lastIndex = #eventTypeGenerationData+1
                else
                    local lastData = eventTypeGenerationData[lastIndex]
                    name = lastData.Type .. "|" .. name
                    context = "any"
                end
                eventTypeGenerationData[lastIndex] = {Type = name, Event = eventName, Context = context}
                eventTypeGenerationDataIndex[eventName] = lastIndex
            end
        end
    end

     --TODO Hacky output fixes [_CustomTypeEntries.After(EmitClass)]. May not be required later on.
    if customText and customText.After then
        self:EmitLine(customText.After)
    end
end


function Generator:MakeModuleTypeName(type)
    local name = type.NativeName:gsub("%.", "")
    if type.ModuleRole ~= "Both" then
        name = type.ModuleRole .. name
    end

    return "Ext_" .. name
end

---@param moduleToClassField table|nil
function Generator:EmitModule(type, moduleToClassField)
    local helpersModuleName = self:MakeModuleTypeName(type)
    local nativeModuleName = type.NativeName
    if type.ModuleRole ~= "Both" then
        nativeModuleName = type.ModuleRole .. nativeModuleName
    end

    --TODO Hacky output fixes [_CustomTypeEntries.Before(EmitModule)]. May not be required later on.
    local customText = _CustomTypeEntries[helpersModuleName]
    if customText and customText.Before then
        self:EmitLine(customText.Before)
    end

    self:EmitComment("@class " .. helpersModuleName)
    local nativeDefn = self.NativeModules[nativeModuleName]

    if moduleToClassField then
        table.sort(moduleToClassField, function(a,b) return a.TypeName < b.TypeName end)
        for _,vType in pairs(moduleToClassField) do
            local vName = string.gsub(vType.NativeName, type.NativeName .. ".", "")
            local typeSig = self:MakeModuleTypeName(vType)
            local fieldOverride = _FuncData.Field[helpersModuleName] and _FuncData.Field[helpersModuleName][vName]
            if fieldOverride then
                typeSig = fieldOverride
            end
            self:EmitFieldComment(vName .. " " .. typeSig)
        end
    end

    if customText and customText.Fields then
        for i,v in ipairs(customText.Fields) do
            self:EmitFieldComment(v)
        end
    end

    local sortedFuncs = {}
    for fname,ftype in pairs(type.Methods) do table.insert(sortedFuncs, fname) end
    table.sort(sortedFuncs)

    local basicFuncSigs = {}
    local extendedFuncSigs = {}

    for i,fname in ipairs(sortedFuncs) do
        if self:FindNativeFunction(fname, nativeDefn) ~= nil then
            table.insert(extendedFuncSigs, fname)
        else
            table.insert(basicFuncSigs, fname)
        end
    end
    
    for i,fname in ipairs(basicFuncSigs) do
        self:EmitModuleFunction(type, fname, nativeDefn)
    end

    self:EmitLine('local ' .. helpersModuleName .. ' = {}')
    --TODO Hacky output fixes [_CustomTypeEntries.After(EmitModule)]. May not be required later on.
    if customText and customText.After then
        self:EmitLine(customText.After)
    end
    self:EmitEmptyLine()
    
    for i,fname in ipairs(extendedFuncSigs) do
        --TODO Hacky output fixes [_CustomFunctionExtras(EmitModule)]. May not be required later on.
        local replaceText = nil
        local afterText = nil
        if nativeDefn ~= nil then
            local functionAdditions = _CustomFunctionExtras[helpersModuleName.."."..fname]
            if functionAdditions then
                if functionAdditions.Before then
                    self:EmitLine(functionAdditions.Before, true)
                end
                afterText = functionAdditions.After
                replaceText = functionAdditions.Replace
            end
        end
        if replaceText then
            self:EmitLine(replaceText, true)
            if afterText then
                self:EmitLine(afterText, true)
            end
        else
            self:EmitModuleFunction(type, fname, nativeDefn, afterText)
        end
    end
end

local function GenerateSubscriptionEvents(self)
    for _,k in pairs(Ext._Internal._PublishedSharedEvents) do
        if not eventTypeGenerationDataIndex[k] then
            if _DEBUG then
                Ext.Utils.PrintWarning("Found unregistered event", k)
            end
            eventTypeGenerationData[#eventTypeGenerationData+1] = {Type="LuaEmptyEvent", Event = k, Context = "any"}
        end
    end
    table.sort(eventTypeGenerationData, function(a,b) return a.Event < b.Event end)
    for _,v in ipairs(eventTypeGenerationData) do
        if v.Context == "server" then
            self:EmitComment("🔨**Server-Only**🔨  ")
        elseif v.Context == "client" then
            self:EmitComment("🔧**Client-Only**🔧  ")
        else
            self:EmitComment("🔨🔧**Server/Client**🔧🔨  ")
        end
        self:EmitComment(_format("@type SubscribableEvent<%s>  ", v.Type))
        self:EmitLine(_format('Ext.Events.%s = {}', v.Event))
    end
end

local _enumNamePattern = ".-%((.+)%).-"
local _restrictedKeys = {
    ["end"] = true,
}

function Generator:GenerateEnums()
    self:EmitLine("--#region Generated Enums")
    self:EmitEmptyLine()
    self:EmitComment("@class Ext_Enums")
    self:EmitLine("local Ext_Enums = {}", true)

    local enumNames = {}
    for k,enum in pairs(Ext.Enums) do
        enumNames[#enumNames+1] = tostring(k)
    end
    table.sort(enumNames)

    for _,enumName in ipairs(enumNames) do
        self:EmitEmptyLine()
        self:EmitComment("@enum " .. enumName)
        self:EmitLine("Ext_Enums." .. enumName .. " = {")

        local enum = Ext.Enums[enumName]

        local valueToName = {}

        for i,label in ipairs(enum) do
            local name = tostring(label)
            local _,_,actualName = string.find(name, _enumNamePattern)
            if actualName then
                name = actualName
            end

            valueToName[i] = name
            
            if string.find(name, "%s") or _restrictedKeys[name] then
                self:EmitLine(string.format("\t[\"%s\"] = %s,", name, i))
            else
                self:EmitLine(string.format("\t%s = %s,", name, i))
            end
        end

        for i,v in ipairs(valueToName) do
            self:EmitLine(string.format("\t[%s] = \"%s\",", i, v))
        end

        self:EmitLine("}")
    end
    self:EmitEmptyLine()
    self:EmitLine("--#endregion")
    self:EmitEmptyLine()
end

function Generator:EmitExt(role, declareGlobal)
    if declareGlobal then
        self:GenerateEnums()
    end

    self:EmitComment("@class Ext" .. (role or ""))

    local aliases = {}
    for i,mod in ipairs(self.Modules) do
        if role == nil or mod.ModuleRole == "Both" or mod.ModuleRole == role then
            local helpersModuleName = self:MakeModuleTypeName(mod)
            if aliases[mod.NativeName] ~= nil then
                aliases[mod.NativeName] = aliases[mod.NativeName] .. "|" .. helpersModuleName
            else
                aliases[mod.NativeName] = helpersModuleName
            end
        end
    end

    local emitted = {}
    for i,mod in ipairs(self.Modules) do
        if role == nil or mod.ModuleRole == "Both" or mod.ModuleRole == role then
            local helpersModuleName = self:MakeModuleTypeName(mod)
            if emitted[mod.NativeName] == nil then
                self:EmitFieldComment(mod.NativeName .. " " .. (aliases[mod.NativeName] or helpersModuleName))
                emitted[mod.NativeName] = true
            end
            if mod.ModuleRole ~= "Both" then
                self:EmitFieldComment(mod.ModuleRole .. mod.NativeName .. " " .. helpersModuleName)
            end
        end
    end

    if declareGlobal then
        self:EmitFieldComment("Enums Ext_Enums")
        self:EmitLine("Ext = {Events = {}}")
        self:EmitEmptyLine()
        self:EmitEmptyLine()
        if _CustomEntries.Specific then
            for k,v in pairs(_CustomEntries.Specific) do
                self:EmitLine(v)
                if k == "SubscribableEventType" then
                    self:EmitLine("--#region Extender Events")
                    GenerateSubscriptionEvents(self)
                    self:EmitLine("--#endregion")
                end
                self:EmitEmptyLine()
            end
            self:EmitEmptyLine()
        end
        if _CustomEntries.Misc then
            for _,v in ipairs(_CustomEntries.Misc) do
                self:EmitLine(v)
            end
        end
    end
end

--Ext.Types.GenerateIdeHelpers("ExtIdeHelpers_New.lua")
--Ext.Types.GenerateIdeHelpers("ExtIdeHelpers_New.lua", {UseBaseExtraData=true,AddDeprecated=true})

--- @class GenerateIdeHelpersOptions
local _DefaultOpts = {
    AddOsiris = false,
    AddDeprecated = false,
    AddAliasEnums = true,
    UseBaseExtraData = false,
    GenerateExtraDataAsClass = false
}

--- @param outputPath? string
--- @param opts? GenerateIdeHelpersOptions
Ext.Types.GenerateIdeHelpers = function (outputPath, opts)
    local options = {}
    if type(opts) == "table" then
        options = opts
    end
    setmetatable(options, {__index = _DefaultOpts})
    eventTypeGenerationData = {}
    eventTypeGenerationDataIndex = {}
    local gen = Generator:New()
    gen:LoadNativeData()
    gen:Build(options)
    if outputPath then
        Ext.IO.SaveFile(outputPath, gen.Text)
    end

    return gen.Text
end

--TODO Ext.Visual.CreateOnCharacter is missing optional params
--TODO Ext_ServerServer.GetGameState() / Ext_ClientClient.GetGameState() ReturnValues[1] is nil 