---@diagnostic disable

return {
Ext_ClientUI = {
After=[[

--- @class EclPickingState
--- @field WorldPosition number[] Position of cursor in world coordinates
--- @field WalkablePosition number[] Closest walkable position to cursor in world coordinates
--- @field HoverCharacter ComponentHandle Character hovered by cursor
--- @field HoverCharacter2 ComponentHandle Character hovered by cursor (???)
--- @field HoverCharacterPosition number[] Position of character hovered by cursor
--- @field HoverItem ComponentHandle Item hovered by cursor
--- @field HoverItemPosition number[] Position of item hovered by cursor
--- @field PlaceableEntity ComponentHandle Placeable game object hovered by cursor
--- @field PlaceablePosition number[] Position of placeable game object hovered by cursor

--- @alias BuiltinUISWFName "actionProgression" | "addContent" | "addContent_c" | "areaInteract_c" | "arenaResult" | "book" | "bottomBar_c" | "buttonLayout_c" | "calibrationScreen" | "campaignManager" | "characterAssign" | "characterAssign_c" | "characterCreation" | "characterCreation_c" | "characterSheet" | "chatLog" | "combatLog" | "combatLog_c" | "combatTurn" | "connectionMenu" | "connectivity_c" | "containerInventory" | "containerInventoryGM" | "contextMenu" | "contextMenu_c" | "craftPanel_c" | "credits" | "dialog" | "dialog_c" | "dummyOverhead" | "encounterPanel" | "enemyHealthBar" | "engrave" | "equipmentPanel_c" | "examine" | "examine_c" | "feedback_c" | "formation" | "formation_c" | "fullScreenHUD" | "gameMenu" | "gameMenu_c" | "giftBagContent" | "giftBagsMenu" | "gmInventory" | "GMItemSheet" | "GMJournal" | "GMMetadataBox" | "GMMinimap" | "GMMoodPanel" | "GMPanelHUD" | "GMRewardPanel" | "GMSkills" | "hotBar" | "installScreen_c" | "inventorySkillPanel_c" | "itemAction" | "itemGenerator" | "itemSplitter" | "itemSplitter_c" | "journal" | "journal_c" | "journal_csp" | "loadingScreen" | "mainMenu" | "mainMenu_c" | "menuBG" | "minimap" | "minimap_c" | "mods" | "mods_c" | "monstersSelection" | "mouseIcon" | "msgBox" | "msgBox_c" | "notification" | "optionsInput" | "optionsSettings" | "optionsSettings_c" | "overhead" | "overviewMap" | "panelSelect_c" | "partyInventory" | "partyInventory_c" | "partyManagement_c" | "pause" | "peace" | "playerInfo" | "playerInfo_c" | "possessionBar" | "pyramid" | "pyramid_c" | "reputationPanel" | "reward" | "reward_c" | "roll" | "saveLoad" | "saveLoad_c" | "saving" | "serverlist" | "serverlist_c" | "skills" | "skillsSelection" | "sortBy_c" | "startTurnRequest" | "startTurnRequest_c" | "statsPanel_c" | "statusConsole" | "statusPanel" | "stickiesPanel" | "sticky" | "storyElement" | "surfacePainter" | "textDisplay" | "tooltip" | "trade" | "trade_c" | "tutorialBox" | "tutorialBox_c" | "uiCraft" | "uiFade" | "userProfile" | "vignette" | "voiceNotification_c" | "watermark" | "waypoints" | "waypoints_c" | "worldTooltip"

--- @overload fun(string:BuiltinUISWFName):integer
Ext_ClientUI.TypeID = {
	actionProgression = 0,
	addContent = 57,
	addContent_c = 81,
	areaInteract_c = 68,
	arenaResult = 125,
	book = 2,
	bottomBar_c = 59,
	buttonLayout_c = 95,
	calibrationScreen = 98,
	campaignManager = 124,
	characterAssign = 52,
	characterAssign_c = 92,
	characterCreation = 3,
	characterCreation_c = 4,
	characterSheet = 119,
	chatLog = 6,
	combatLog = 7,
	combatLog_c = 65,
	combatTurn = 8,
	connectionMenu = 33,
	connectivity_c = 34,
	containerInventory = { Default = 9, Pickpocket = 37 },
	containerInventoryGM = 143,
	contextMenu = { Default = 10, Object = 11 },
	contextMenu_c = { Default = 12, Object = 96 },
	craftPanel_c = 84,
	credits = 53,
	dialog = 14,
	dialog_c = 66,
	dummyOverhead = 15,
	encounterPanel = 105,
	enemyHealthBar = 42,
	engrave = 69,
	equipmentPanel_c = 64,
	examine = 104,
	examine_c = 67,
	feedback_c = 97,
	formation = 130,
	formation_c = 135,
	fullScreenHUD = 100,
	gameMenu = 19,
	gameMenu_c = 77,
	giftBagContent = 147,
	giftBagsMenu = 146,
	gmInventory = 126,
	GMItemSheet = 107,
	GMJournal = 139,
	GMMetadataBox = 109,
	GMMinimap = 113,
	GMMoodPanel = 108,
	GMPanelHUD = 120,
	GMRewardPanel = 131,
	GMSkills = 123,
	hotBar = 40,
	installScreen_c = 80,
	inventorySkillPanel_c = 62,
	itemAction = 86,
	itemGenerator = 106,
	itemSplitter = 21,
	itemSplitter_c = 85,
	journal = 22,
	journal_c = 70,
	journal_csp = 140,
	loadingScreen = 23,
	mainMenu = 28,
	mainMenu_c = 87, -- Still mainMenu.swf, but this is used for controllers after clicking "Options" in the gameMenu_c
	menuBG = 56,
	minimap = 30,
	minimap_c = 60,
	mods = 49,
	mods_c = 103,
	monstersSelection = 127,
	mouseIcon = 31,
	msgBox = 29,
	msgBox_c = 75,
	notification = 36,
	optionsInput = 13,
	optionsSettings = { Default = 45, Video = 45, Audio = 1, Game = 17 },
	optionsSettings_c = { Default = 91, Video = 91, Audio = 88, Game = 89 },
	overhead = 5,
	overviewMap = 112,
	panelSelect_c = 83,
	partyInventory = 116,
	partyInventory_c = 142,
	partyManagement_c = 82,
	pause = 121,
	peace = 122,
	playerInfo = 38,
	playerInfo_c = 61, --Still playerInfo.swf, but the ID is different.
	possessionBar = 110,
	pyramid = 129,
	pyramid_c = 134,
	reputationPanel = 138,
	reward = 136,
	reward_c = 137,
	roll = 118,
	saveLoad = 39,
	saveLoad_c = 74,
	saving = 99,
	serverlist = 26,
	serverlist_c = 27,
	skills = 41,
	skillsSelection = 54,
	sortBy_c = 79,
	startTurnRequest = 145,
	startTurnRequest_c = 144,
	statsPanel_c = 63,
	statusConsole = 117,
	statusPanel = 128,
	stickiesPanel = 133,
	sticky = 132,
	storyElement = 71,
	surfacePainter = 111,
	textDisplay = 43,
	tooltip = 44,
	trade = 46,
	trade_c = 73,
	tutorialBox = 55,
	tutorialBox_c = 94,
	uiCraft = 102,
	uiFade = 16,
	userProfile = 51,
	vignette = 114,
	voiceNotification_c = 93,
	watermark = 141,
	waypoints = 47,
	waypoints_c = 78,
	worldTooltip = 48,
}]]},
Ext_Types = {
	After = [[
--- @class GenerateIdeHelpersOptions
--- @field AddOsiris boolean Add all Osiris functions to the global Osi table. This is optional, due to the possible performance cost of having so many functions.
--- @field AddDeprecated boolean Add deprecated functions to the helpers file.
--- @field AddAliasEnums boolean Add the enums in alias format, for string comparison. Defaults to true.
--- @field UseBaseExtraData boolean Only include the base ExtraData keys/values in Shared, instead of grabbing whatever the current keys are in the mod environment.
--- @field GenerateExtraDataAsClass boolean Annotate ExtraData as a class, so it only has fields with no fixed/hardcoded values.

--- Generate an ExtIdeHelpers file  
--- @param outputPath string|nil Optional path to save the generated helper file, relative to the `Documents\Larian Studios\Divinity Original Sin 2 Definitive Edition\Osiris Data` folder  
--- @param addOsi boolean|nil If true, all Osiris functions will be included in the Osi global table. This is optional, due to the possible performance cost of having so many functions  
--- @return string fileContents Returns the file contents, for use with Ext.IO.SaveFile
function Ext_Types.GenerateIdeHelpers(outputPath, addOsi) end
]]},
--TODO StatsPropertyData is missing child classes, such as StatPropertySurfaceChange
StatsPropertyData = {
	Before = [[
---@alias AnyStatProperty StatPropertyCustom|StatPropertyStatus|StatPropertySurfaceChange|StatPropertySabotage|StatPropertySummon|StatPropertyForce|StatPropertyGameAction|StatPropertyOsirisTask|StatPropertyExtender

--- @class StatProperty
--- @field Context string[] Target|Self|AoE|SelfOnHit|SelfOnEquip
--- @field Type string Custom|Status|SurfaceChange|GameAction|OsirisTask|Sabotage|Summon|Force
--- @field Condition string|nil

--- @class StatPropertyCustom : StatProperty
--- @field Type string|"Custom"
--- @field Action string

--- @class StatPropertyStatus : StatProperty
--- @field Type string|"Status"
--- @field Action string Status name
--- @field StatusChance number
--- @field Duration number
--- @field StatsId string
--- @field Arg4 integer
--- @field Arg5 integer
--- @field SurfaceBoost boolean
--- @field SurfaceBoosts string[] Labels from "Surface Type" enumeration

--- @class StatPropertySurfaceChange : StatProperty
--- @field Type string|"SurfaceChange"
--- @field Action string Label from "Surface Change" enumeration
--- @field SurfaceChance number
--- @field Lifetime number
--- @field StatusChance number
--- @field Radius number

--- @class StatPropertySabotage : StatProperty
--- @field Type string|"Sabotage"
--- @field Amount integer

--- @class StatPropertySummon : StatProperty
--- @field Type string|"Summon"
--- @field Template string
--- @field Duration number
--- @field IsTotem boolean
--- @field Skill string

--- @class StatPropertyForce : StatProperty
--- @field Type string|"Force"
--- @field Distance integer The force distance in meters.

--- @class StatPropertyGameAction : StatProperty
--- @field Type string|"GameAction"
--- @field Action string Label from "Game Action" enumeration
--- @field Arg1 number
--- @field Arg2 number
--- @field Arg3 string
--- @field Arg4 number
--- @field Arg5 number
--- @field StatusHealType string Label from"StatusHealType"enumeration

--- @class StatPropertyOsirisTask : StatProperty
--- @field Type string|"OsirisTask"
--- @field Action string Label from "Osiris Task" enumeration
--- @field Chance number
--- @field VitalityOnRevive integer

--- @class StatPropertyExtender : StatProperty
--- @field Type string|"Extender"
--- @field Action string Custom SkillProperties action name
--- @field Arg1 number
--- @field Arg2 number
--- @field Arg3 string
--- @field Arg4 integer
--- @field Arg5 integer
]]},
--TODO Ext_Stats.Get has no return value, and all these stat entry types are missing.
Ext_Stats = {
Before = [[
--- @class StatEntryWeapon
--- @field Name string
--- @field Level integer
--- @field Using string
--- @field Damage integer
--- @field Act string See Act enumeration
--- @field Handedness string See Handedness enumeration
--- @field IsTwoHanded YesNo
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
--- @field NeedsIdentification YesNo
--- @field LifeSteal integer
--- @field CleavePercentage integer
--- @field CleaveAngle integer
--- @field Talents string
--- @field IgnoreVisionBlock YesNo
--- @field Tags string
--- @field ExtraProperties AnyStatProperty[]
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
--- @field NeedsIdentification YesNo
--- @field Charges integer
--- @field RuneSlots integer
--- @field RuneSlots_V1 integer
--- @field MaxCharges integer
--- @field Talents string
--- @field Tags string
--- @field ExtraProperties AnyStatProperty[]
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
--- @field NeedsIdentification YesNo
--- @field Talents string
--- @field Tags string
--- @field ExtraProperties AnyStatProperty[]
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
--- @field UnknownBeforeConsume YesNo
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
--- @field AddToBottomBar YesNo
--- @field SummonLifelinkModifier integer
--- @field IgnoredByAI YesNo
--- @field RangeBoost integer
--- @field BonusWeapon string
--- @field AiCalculationStatsOverride string
--- @field RuneEffectWeapon string
--- @field RuneEffectUpperbody string
--- @field RuneEffectAmulet string
--- @field RuneLevel integer
--- @field LifeSteal integer
--- @field IsFood YesNo
--- @field IsConsumable YesNo
--- @field ExtraProperties AnyStatProperty[]
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
--- @field AddToBottomBar YesNo
--- @field IgnoredByAI YesNo
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
--- @field SkillType SkillType
--- @field Ability SkillAbility
--- @field Element SkillElement
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
--- @field Totem YesNo
--- @field Template1 string
--- @field Template2 string
--- @field Template3 string
--- @field WeaponBones string
--- @field TeleportSelf YesNo
--- @field CanTargetCharacters YesNo
--- @field CanTargetItems YesNo
--- @field CanTargetTerrain YesNo
--- @field ForceTarget YesNo
--- @field TargetProjectiles YesNo
--- @field UseCharacterStats YesNo
--- @field UseWeaponDamage YesNo
--- @field UseWeaponProperties YesNo
--- @field SingleSource YesNo
--- @field ContinueOnKill YesNo
--- @field Autocast YesNo
--- @field AmountOfTargets integer
--- @field AutoAim YesNo
--- @field AddWeaponRange YesNo
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
--- @field SkillProperties AnyStatProperty[]
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
--- @field PreviewStrikeHits YesNo
--- @field SummonLevel integer
--- @field StartTextEvent string
--- @field StopTextEvent string
--- @field Atmosphere string See AtmosphereType enumeration
--- @field ConsequencesStartTime integer
--- @field ConsequencesDuration integer
--- @field HealthBarColor integer
--- @field Skillbook string
--- @field PreviewImpactEffect string
--- @field IgnoreVisionBlock YesNo
--- @field HealEffectId string
--- @field AddRangeFromAbility string See Ability enumeration
--- @field DivideDamage YesNo
--- @field OverrideMinAP YesNo
--- @field OverrideSkillLevel YesNo
--- @field Tier string See SkillTier enumeration
--- @field GrenadeBone string
--- @field GrenadeProjectile string
--- @field GrenadePath string
--- @field MovingObject string
--- @field SpawnObject string
--- @field SpawnEffect string
--- @field SpawnFXOverridesImpactFX YesNo
--- @field SpawnLifetime integer
--- @field ProjectileTerrainOffset YesNo
--- @field ProjectileType string See ProjectileType enumeration
--- @field HitEffect string
--- @field PushDistance integer
--- @field ForceMove YesNo
--- @field Stealth YesNo
--- @field Distribution string See ProjectileDistribution enumeration
--- @field Shuffle YesNo
--- @field PushPullEffect string
--- @field BackStart integer
--- @field FrontOffset integer
--- @field TargetGroundEffect string
--- @field PositionEffect string
--- @field BeamEffect string
--- @field PreviewEffect string
--- @field CastSelfAnimation string
--- @field IgnoreCursed YesNo
--- @field IsEnemySkill YesNo
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
--- @field TeleportSurface YesNo
--- @field ProjectileSkills string
--- @field SummonCount integer
--- @field LinkTeleports YesNo
--- @field TeleportsUseCount integer
--- @field HeightOffset integer
--- @field ForGameMaster YesNo
--- @field IsMelee YesNo
--- @field MemorizationRequirements StatRequirement[]
--- @field IgnoreSilence YesNo
--- @field IgnoreHeight YesNo
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
--- @field OverrideDefaultDescription YesNo
--- @field FormatColor string See FormatStringColor enumeration
--- @field SavingThrow string See SavingThrow enumeration
--- @field IsChanneled YesNo
--- @field Instant YesNo
--- @field StatusEffect string
--- @field StatusEffectOverrideForItems string
--- @field StatusEffectOnTurn string
--- @field MaterialType string See MaterialType enumeration
--- @field Material string
--- @field MaterialApplyBody YesNo
--- @field MaterialApplyArmor YesNo
--- @field MaterialApplyWeapon YesNo
--- @field MaterialApplyNormalMap YesNo
--- @field MaterialFadeAmount integer
--- @field MaterialOverlayOffset integer
--- @field MaterialParameters string
--- @field HealingEvent string See StatusEvent enumeration
--- @field HealStat string See StatusHealType enumeration
--- @field HealType string See HealValueType enumeration
--- @field HealValue integer
--- @field StatsId string
--- @field IsInvulnerable YesNo
--- @field IsDisarmed YesNo
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
--- @field ApplyAfterCleanse YesNo
--- @field SoundStart string
--- @field SoundLoop string
--- @field SoundStop string
--- @field DamageEvent string See StatusEvent enumeration
--- @field DamageStats string
--- @field DeathType string See Death Type enumeration
--- @field DamageCharacters YesNo
--- @field DamageItems YesNo
--- @field DamageTorches YesNo
--- @field FreezeTime integer
--- @field SurfaceChange string
--- @field PermanentOnTorch YesNo
--- @field AbsorbSurfaceType string
--- @field AbsorbSurfaceRange integer
--- @field Skills string
--- @field BonusFromAbility string See Ability enumeration
--- @field Items string
--- @field OnlyWhileMoving YesNo
--- @field DescriptionCaster string
--- @field DescriptionTarget string
--- @field WinBoost string See Properties enumeration
--- @field LoseBoost string See Properties enumeration
--- @field WeaponOverride string
--- @field ApplyEffect string
--- @field ForGameMaster YesNo
--- @field ResetCooldowns string
--- @field ResetOncePerCombat YesNo
--- @field PolymorphResult string
--- @field DisableInteractions YesNo
--- @field LoseControl YesNo
--- @field AiCalculationSkillOverride string
--- @field HealEffectId string
--- @field ScaleWithVitality YesNo
--- @field VampirismType string See VampirismType enumeration
--- @field BeamEffect string
--- @field HealMultiplier integer
--- @field InitiateCombat YesNo
--- @field Projectile string
--- @field Radius integer
--- @field Charges integer
--- @field MaxCharges integer
--- @field DefendTargetPosition YesNo
--- @field TargetConditions string See Conditions enumeration
--- @field Toggle YesNo
--- @field LeaveAction string
--- @field DieAction string
--- @field PlayerSameParty YesNo
--- @field PlayerHasTag string
--- @field PeaceOnly YesNo
--- @field Necromantic YesNo
--- @field RetainSkills string
--- @field BringIntoCombat YesNo
--- @field ApplyStatusOnTick string
--- @field IsResistingDeath YesNo
--- @field TargetEffect string
--- @field DamagePercentage integer
--- @field ForceOverhead YesNo
--- @field TickSFX string
--- @field ForceStackOverwrite YesNo
--- @field FreezeCooldowns YesNo

--- @class StatEntryCrime
--- @field Name string
--- @field Level integer
--- @field Using string
--- @field CrimeType string
--- @field TensionWeight integer
--- @field Lifetime integer
--- @field Audible YesNo
--- @field DetectionRange integer
--- @field CreatesCrimescene YesNo
--- @field Continuous YesNo
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
--- @field CanRaiseAlarm YesNo
--- @field CanMerge YesNo

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

---@class StatTreasureTableCategory
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

---@class StatTreasureTableSubTable
---@field Categories StatTreasureTableCategory[]
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
---@field SubTables StatTreasureTableSubTable[]

---@class StatTreasureCategory
---@field Category FixedString
---@field Items StatTreasureCategoryItem[]

---@class StatTreasureCategoryItem
---@field Name FixedString
---@field ActPart integer
---@field MinAmount integer
---@field MaxAmount integer
---@field MinLevel integer
---@field MaxLevel integer
---@field Priority integer
---@field Unique integer

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
]]},
Ext_ServerCustomStat = {
Before = [[--- @class CustomStatDefinition
--- @field Id Guid
--- @field Name string
--- @field Description string
]]},
Ext_Utils = {
Before = [[--- @class ComponentHandleProxy:userdata
--- @field Get (fun():IEoCServerObject|IEoCClientObject)
--- @field TypeId integer
--- @field Type FixedString
--- @field Salt uint32
--- @field Index uint32

--- ## Synchronization 
--- A variable is only eligible for synchronization if: 
--- * Both `Server` and `Client` is true. 
--- * For server to client synchronization, both `WriteableOnServer` and `SyncToClient` is true. 
--- * For client to server synchronization, both `WriteableOnClient` and `SyncToServer` is true. 
--- @class RegisterUserVariableOptions
--- @field Server boolean Variable is present on server entities. Defaults to `true`.
--- @field Client boolean Variable is present on client entities. Defaults to `false`.
--- @field WriteableOnServer boolean Variable can be modified on server side. Defaults to `true`.
--- @field WriteableOnClient boolean Variable can be modified on client side. Defaults to `false`.
--- @field Persistent boolean Variable is written to/restored from savegames. Defaults to `true`.
--- @field SyncToClient boolean Server-side changes to the variable are synced to all clients. Defaults to `false`.
--- @field SyncToServer boolean Client-side changes to the variable are synced to the server. Defaults to `false`.
--- @field SyncOnTick boolean Client-server sync is performed once per game loop tick. Defaults to `true`.
--- @field SyncOnWrite boolean Client-server sync is performed immediately when the variable is written. This is disabled by default for performance reasons.
--- @field DontCache boolean Disable Lua caching of variable values.<br>See here: https://github.com/Norbyte/ositools/blob/master/Docs/ReleaseNotesv58.md#caching-behavior
]]},
Ext_StatsItemSet = {
Before = [[--- @class ItemSet
--- @field TranslatedName TranslatedString
--- @field TranslatedSetEffect TranslatedString
--- @field Name FixedString
--- @field Items FixedString[]
--- @field GrantedSkills FixedString[]
--- @field GrantedStatuses FixedString[]
--- @field ItemsUpgradeLevel integer
]]},
Ext_ServerEntity = {
Before = [[--- @class EsvCombatTeam
--- @field TeamId integer A number identifying the team instance
--- @field CombatId integer 	Identifies which combat the team is a participant of
--- @field Initiative integer Computed initiative value of the team
--- @field StillInCombat boolean Can the team currently fight, or is it temporarily out of combat?
--- @field Character EsvCharacter|nil Character object if the team is a character; nil otherwise
--- @field Item EsvItem|nil Item object if the team is an item; nil otherwise

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
]]},
EclLuaVisualClientMultiVisual = {
Before = [[--- @class ClientMultiVisualAddVisualOptions
--- @field AllowTPose boolean
--- @field ResetScale boolean
--- @field SyncAnimationWithParent boolean
--- @field Bone FixedString
--- @field Color1 vec4
--- @field Color2 vec4
--- @field Color3 vec4
--- @field Color4 vec4
--- @field Color5 vec4
--- @field ExcludeFromBounds boolean
--- @field KeepRot boolean
--- @field KeepScale boolean
--- @field UseLocalTransform boolean
--- @field InheritAnimations boolean
--- @field ParticleSystem boolean
--- @field DoNotUpdate boolean
--- @field Armor boolean
--- @field Weapon boolean
--- @field Wings boolean
--- @field Horns boolean
--- @field Overhead boolean
--- @field WeaponFX boolean
--- @field BonusWeaponFX boolean
--- @field WeaponOverlayFX boolean
--- @field CastShadow boolean
--- @field ReceiveDecal boolean
--- @field Reflecting boolean
--- @field IsShadowProxy boolean
--- @field AllowReceiveDecalWhenAnimated boolean

]]},
ModuleInfo = {
	After = [[

--- Separate from ModuleInfo, this is a table with specific keys.  
--- @class LegacyModuleInfo
--- @field UUID FixedString
--- @field Name string
--- @field Description string
--- @field Directory string
--- @field ModuleType FixedString
--- @field Version integer
--- @field PublishVersion integer
--- @field Dependencies FixedString[]
]]
},
Trigger = {
	After = "--- @field Id Guid The trigger Guid."
},
Ext_Surface = {
	Fields = {"Action Ext_ServerSurfaceAction"}
},
Ext_Json = {
	After = [[
--- @class JsonStringifyOptions
--- @field Beautify boolean Sorts the output table, and indents with tabs. Defaults to true.
--- @field StringifyInternalTypes boolean Defaults to false.
--- @field IterateUserdata boolean Defaults to false.
--- @field AvoidRecursion boolean Defaults to false.
--- @field MaxDepth integer Defaults to 64, the maximum value.
--- @field LimitDepth integer Defaults to -1 (off).
--- @field LimitArrayElements integer Defaults to -1 (off).]]
},

Ext_ClientBehavior = {
	Before = [[
---@class UserspaceCharacterTaskCallbacks
---@field Attached fun(self:UserspaceCharacterTask) Called when the task is attached to a character, or when the task enters the active state (which technically creates a new instance of it within the engine). This will be called again for a character after a lua reset. Use for initialization.
---@field CanEnter fun(self:UserspaceCharacterTask):boolean Returns whether the client can begin previewing the task. If a task wins the priority contest and can be entered, ExitPreview() will be called for the previous previewed task, and EnterPreview() will be called on the newest prioritized task. This method is optional to implement; the default implementation is to return false.
---@field CanExit fun(self:UserspaceCharacterTask):boolean Called every tick while the task is active; purpose unknown, but it's recommended to keep its return value consistent with `Update()`. This method is optional to implement; the default implementation is to return `true`.
---@field CanExit2 fun(self:UserspaceCharacterTask):boolean This method is optional to implement; the default implementation is to return `true`.
---@field ClearAIColliding fun(self:UserspaceCharacterTask) 
---@field Enter fun(self:UserspaceCharacterTask):boolean Called when a task enters the active state. If false is returned, `GetError()` will be called and the task will abort. This method is optional to implement; the default implementation is to return `false`.
---@field EnterPreview fun(self:UserspaceCharacterTask) Called when `CanEnter()` succeeds on the task, if it was previously inactive.
---@field Exit fun(self:UserspaceCharacterTask) Called when `Update()` returns false, marking the end of the active state.
---@field ExitPreview fun(self:UserspaceCharacterTask) Called when an task exits the preview state due to another one having greater priority. Use to remove any visuals related to the preview.
---@field GetActionCost fun(self:UserspaceCharacterTask):integer This method is optional to implement; the default implementation is to return 0.
---@field GetTotalAPCost fun(self:UserspaceCharacterTask):integer This method is optional to implement; the default implementation is to return 0.
---@field GetAPDescription fun(self:UserspaceCharacterTask, description:string):string
---@field GetDescription fun(self:UserspaceCharacterTask, description:string):string
---@field GetAPWarning fun(self:UserspaceCharacterTask):integer This method is optional to implement; the default implementation is to return 0.
---@field GetError fun(self:UserspaceCharacterTask):integer Called if `Enter()` fails. Will show a message on the controller UI. This method is optional to implement; the default implementation is to return 0.
---@field GetExecutePriority fun(self:UserspaceCharacterTask, previousPriority:integer):integer Returns the priority of the task for being executed (becoming active). This is called every tick for the engine for all attached tasks of the active character; the task with the highest priority is the one the client will be able to execute. Character tasks can be queued. In the case that there are multiple tasks that want to execute, they will do so in the order of `GetExecutePriority()`, executing after the previous task exits via `ShouldExit()`. This method is optional to implement; the default implementation is to return 0.
---@field GetPriority fun(self:UserspaceCharacterTask, previousPriority:integer):integer Returns the priority of previewing the task. This is called every tick for the engine for all tasks; the task with the highest priority that also passes its `CanEnter()` check is the one the client will see being previewed. This method is optional to implement; the default implementation is to return 0.
---@field GetSightRange fun(self:UserspaceCharacterTask):number This method is optional to implement; the default implementation is to return 0.0.
---@field ValidateTargetRange  fun(self:UserspaceCharacterTask):integer This method is optional to implement; the default implementation is to return 0.
---@field HandleInputEvent fun(self:UserspaceCharacterTask) Handles an input event. Fired regardless of the task's state, but only for certain input events, like interaction buttons (mouse clicks). You should set `RequestRun` on `task` if you wish for the task to become active next tick, or be queued (if there is already an active task).
---@field SetAIColliding fun(self:UserspaceCharacterTask)
---@field SetCursor fun(self:UserspaceCharacterTask) Called every tick while the task is being previewed, if the cursor is not over a UI.
---@field Start fun(self:UserspaceCharacterTask) Called when the task is first instanced, or when the interact key is pressed (ex. left-click when using keyboard+mouse).
---@field Stop fun(self:UserspaceCharacterTask) Called when the task is being removed by the engine, or when a cancel key is pressed (ex. right-click when using keyboard+mouse).
---@field Update fun(self:UserspaceCharacterTask):boolean Called every tick while the task is in the active state. If false is returned, `Exit()` will be called and the task will become inactive. This method is optional to implement; the default implementation is to return `true`.
---@field UpdatePreview fun(self:UserspaceCharacterTask) Called every tick if the task is in the preview state.

---@alias UserspaceCharacterTask EclCharacterTask|UserspaceCharacterTaskCallbacks
]]
},
}