--#region "Imports"

local Ext = Ext
local _DEBUG = Ext.Debug.IsDeveloperMode()
local _DoubleToHandle = Ext.UI.DoubleToHandle
local _Dump = Ext.Dump
local _DumpExport = Ext.DumpExport
local _GetCharacter = Ext.Entity.GetCharacter
local _GetGameState = Ext.Client.GetGameState
local _GetItem = Ext.Entity.GetItem
local _GetObjectType = Ext.Types.GetObjectType
local _GetStat = Ext.Stats.Get
local _GetUIByPath = Ext.UI.GetByPath
local _GetUIByType = Ext.UI.GetByType
local _HandleToDouble = Ext.UI.HandleToDouble
local _Print = Ext.Utils.Print
local _PrintError = Ext.Utils.PrintError
local _PrintWarning = Ext.Utils.PrintWarning
local _Require = Ext.Utils.Include
local assert = assert
local debug = debug
local pairs = pairs
local setmetatable = setmetatable
local string = string
local table = table
local tostring = tostring
local type = type
local xpcall = xpcall

local _IsNaN = Ext.Math.IsNaN
local _IsValidHandle = Ext.Utils.IsValidHandle
local _Stringify = Ext.Json.Stringify
local _UITYPE = Ext.UI.TypeID

--#end-region

if Game == nil then
	Game = {}
end

---@class ExtenderTooltipAPI
local _TT = {}

Game.Tooltip = _TT

---@param prioritizeSecondPlayer? boolean Priotize getting the second player in splitscreen mode. If there is no second player, the first player is returned anyway.
---@return EclCharacter|nil
local function _GetClientCharacter(prioritizeSecondPlayer)
	local playerManager = Ext.Entity.GetPlayerManager()
	if playerManager then
		if prioritizeSecondPlayer then
			local player2Data = playerManager.ClientPlayerData[2]
			if player2Data then
				local client = Ext.Entity.GetCharacter(player2Data.CharacterNetId)
				if client then
					return client
				end
			end
		end
		for id,data in pairs(playerManager.ClientPlayerData) do
			local client = Ext.Entity.GetCharacter(data.CharacterNetId)
			if client then
				return client
			end
		end
	end
	return nil
end

---@type GameTooltipRequestProcessor
local RequestProcessor = _Require(nil, "builtin://Libs/Game.Tooltip.RequestProcessor.lua")
_TT.RequestProcessor = RequestProcessor
RequestProcessor.Utils.GetClientCharacter = _GetClientCharacter

local game = Game
_ENV = _TT
_ENV.Game = game
---@diagnostic disable deprecated
if setfenv ~= nil then
	setfenv(1, _TT)
end
---@diagnostic enable

---@class GameTooltipControllerVars
---@field LastPlayer integer The NetID of the last player character.
---@field LastOverhead number The double handle of the overhead object.
local ControllerVars = {}
_TT.ControllerVars = ControllerVars

local tooltipCustomIcons = {}

---Add a custom icon to the tooltip UI.  
---Use this with tooltip elements that have an Icon string property, such as WandSkill.
---@param id string
---@param icon string
---@param w integer
---@param h integer
function _TT.AddCustomIconToTooltip(id, icon, w, h)
	local ui = _GetUIByType(_UITYPE.tooltip)
	if ui then
		ui:SetCustomIcon(id, icon, w, h)
		tooltipCustomIcons[#tooltipCustomIcons+1] = id
	end
end

TooltipItemIds = {
	"ItemName","ItemWeight","ItemGoldValue","ItemLevel","ItemDescription","ItemRarity","ItemUseAPCost","ItemAttackAPCost","StatBoost",
	"ResistanceBoost","AbilityBoost","OtherStatBoost","VitalityBoost","ChanceToHitBoost","DamageBoost","APCostBoost","APMaximumBoost",
	"APStartBoost","APRecoveryBoost","CritChanceBoost","ArmorBoost","ConsumableDuration","ConsumablePermanentDuration","ConsumableEffect",
	"ConsumableDamage","ExtraProperties","Flags","ItemRequirement","WeaponDamage","WeaponDamagePenalty","WeaponCritMultiplier","WeaponCritChance",
	"WeaponRange","Durability","CanBackstab","AccuracyBoost","DodgeBoost","EquipmentUnlockedSkill","WandSkill","WandCharges","ArmorValue",
	"ArmorSlotType","Blocking","NeedsIdentifyLevel","IsQuestItem","PriceToIdentify","PriceToRepair","PickpocketInfo","Engraving",
	"ContainerIsLocked","SkillName","SkillIcon","SkillSchool","SkillTier","SkillRequiredEquipment","SkillAPCost","SkillCooldown",
	"SkillDescription","SkillProperties","SkillDamage","SkillRange","SkillExplodeRadius","SkillCanPierce","SkillCanFork","SkillStrikeCount",
	"SkillProjectileCount","SkillCleansesStatus","SkillMultiStrikeAttacks","SkillWallDistance","SkillPathSurface","SkillPathDistance",
	"SkillHealAmount","SkillDuration","ConsumableEffectUknown","Reflection","SkillAlreadyLearned","SkillOnCooldown","SkillAlreadyUsed",
	"AbilityTitle","AbilityDescription","TalentTitle","TalentDescription","SkillMPCost","MagicArmorValue","WarningText","RuneSlot",
	"RuneEffect","Equipped","ShowSkillIcon","SkillbookSkill","Tags","EmptyRuneSlot","StatName","StatsDescription","StatsDescriptionBoost",
	"StatSTRWeight","StatMEMSlot","StatsPointValue","StatsTalentsBoost","StatsTalentsMalus","StatsBaseValue","StatsPercentageBoost",
	"StatsPercentageMalus","StatsPercentageTotal","StatsGearBoostNormal","StatsATKAPCost","StatsCriticalInfos","StatsAPTitle","StatsAPDesc",
	"StatsAPBase","StatsAPBonus","StatsAPMalus","StatsTotalDamage","TagDescription","StatusImmunity","StatusBonus","StatusMalus","StatusDescription",
	"Title","SurfaceDescription","Duration","Fire","Water","Earth","Air","Poison","Physical","Sulfur","Heal","Splitter","ArmorSet"
}
TooltipItemTypes = {
	ItemName = 1,
	ItemWeight = 2,
	ItemGoldValue = 3,
	ItemLevel = 4,
	ItemDescription = 5,
	ItemRarity = 6,
	ItemUseAPCost = 7,
	ItemAttackAPCost = 8,
	StatBoost = 9,
	ResistanceBoost = 10,
	AbilityBoost = 11,
	OtherStatBoost = 12,
	VitalityBoost = 13,
	ChanceToHitBoost = 14,
	DamageBoost = 15,
	APCostBoost = 16,
	APMaximumBoost = 17,
	APStartBoost = 18,
	APRecoveryBoost = 19,
	CritChanceBoost = 20,
	ArmorBoost = 21,
	ConsumableDuration = 22,
	ConsumablePermanentDuration = 23,
	ConsumableEffect = 24,
	ConsumableDamage = 25,
	ExtraProperties = 26,
	Flags = 27,
	ItemRequirement = 28,
	WeaponDamage = 29,
	WeaponDamagePenalty = 30,
	WeaponCritMultiplier = 31,
	WeaponCritChance = 32,
	WeaponRange = 33,
	Durability = 34,
	CanBackstab = 35,
	AccuracyBoost = 36,
	DodgeBoost = 37,
	EquipmentUnlockedSkill = 38,
	WandSkill = 39,
	WandCharges = 40,
	ArmorValue = 41,
	ArmorSlotType = 42,
	Blocking = 43,
	NeedsIdentifyLevel = 44,
	IsQuestItem = 45,
	PriceToIdentify = 46,
	PriceToRepair = 47,
	PickpocketInfo = 48,
	Engraving = 49,
	ContainerIsLocked = 50,
	SkillName = 51,
	SkillIcon = 52,
	SkillSchool = 53,
	SkillTier = 54,
	SkillRequiredEquipment = 55,
	SkillAPCost = 56,
	SkillCooldown = 57,
	SkillDescription = 58,
	SkillProperties = 59,
	SkillDamage = 60,
	SkillRange = 61,
	SkillExplodeRadius = 62,
	SkillCanPierce = 63,
	SkillCanFork = 64,
	SkillStrikeCount = 65,
	SkillProjectileCount = 66,
	SkillCleansesStatus = 67,
	SkillMultiStrikeAttacks = 68,
	SkillWallDistance = 69,
	SkillPathSurface = 70,
	SkillPathDistance = 71,
	SkillHealAmount = 72,
	SkillDuration = 73,
	ConsumableEffectUknown = 74,
	Reflection = 75,
	SkillAlreadyLearned = 76,
	SkillOnCooldown = 77,
	SkillAlreadyUsed = 78,
	AbilityTitle = 79,
	AbilityDescription = 80,
	TalentTitle = 81,
	TalentDescription = 82,
	SkillMPCost = 83,
	MagicArmorValue = 84,
	WarningText = 85,
	RuneSlot = 86,
	RuneEffect = 87,
	Equipped = 88,
	ShowSkillIcon = 89,
	SkillbookSkill = 90,
	Tags = 91,
	EmptyRuneSlot = 92,
	StatName = 93,
	StatsDescription = 94,
	StatsDescriptionBoost = 95,
	StatSTRWeight = 96,
	StatMEMSlot = 97,
	StatsPointValue = 98,
	StatsTalentsBoost = 99,
	StatsTalentsMalus = 100,
	StatsBaseValue = 101,
	StatsPercentageBoost = 102,
	StatsPercentageMalus = 103,
	StatsPercentageTotal = 104,
	StatsGearBoostNormal = 105,
	StatsATKAPCost = 106,
	StatsCriticalInfos = 107,
	StatsAPTitle = 108,
	StatsAPDesc = 109,
	StatsAPBase = 110,
	StatsAPBonus = 111,
	StatsAPMalus = 112,
	StatsTotalDamage = 113,
	TagDescription = 114,
	StatusImmunity = 115,
	StatusBonus = 116,
	StatusMalus = 117,
	StatusDescription = 118,
	Title = 119,
	SurfaceDescription = 120,
	Duration = 121,
	Fire = 122,
	Water = 123,
	Earth = 124,
	Air = 125,
	Poison = 126,
	Physical = 127,
	Sulfur = 128,
	Heal = 129,
	Splitter = 130,
	ArmorSet = 131,
}

_TT.TooltipItemTypes = TooltipItemTypes

local _Label = {"Label", "string"}
local _Value = {"Value", "string"}
local _NumValue = {"Value", "number"}
local _Icon = {"Icon", "string"}
local _Warning = {"Warning", "string"}
local _Unused = {nil, nil}
local BoostSpec = {_Label, _NumValue, _Unused}
TooltipSpecs = {
	ItemName = {_Label},
	ItemWeight = {_Label, _Unused},
	ItemGoldValue = {_Label},
	ItemLevel = {_Label, _NumValue, _Unused},
	ItemDescription = {_Label},
	ItemRarity = {_Label},
	ItemUseAPCost = {_Label, _NumValue, {"RequirementMet", "boolean"}},
	ItemAttackAPCost = {_Label, _NumValue, _Warning, {"RequirementMet", "boolean"}},
	StatBoost = BoostSpec,
	ResistanceBoost = BoostSpec,
	AbilityBoost = BoostSpec,
	OtherStatBoost = {_Label, _Value, _Unused, _Unused},
	VitalityBoost = BoostSpec,
	ChanceToHitBoost = BoostSpec,
	DamageBoost = BoostSpec,
	APCostBoost = BoostSpec,
	APMaximumBoost = BoostSpec,
	APStartBoost = BoostSpec,
	APRecoveryBoost = BoostSpec,
	CritChanceBoost = BoostSpec,
	ArmorBoost = BoostSpec,
	ConsumableDuration = {_Label, _Unused, _Unused, _Value},
	ConsumablePermanentDuration = {_Label, _Value},
	ConsumableEffect = {_Label, _Unused, _Value, _Unused},
	ConsumableDamage = {_Unused, {"MinDamage", "number"}, {"MaxDamage", "number"}, {"DamageType", "number"}, _Label},
	ExtraProperties = {_Label, _Unused, _Unused, _Unused, _Unused},
	Flags = {_Label, _Unused, _Unused},
	ItemRequirement = {_Label, _Unused, {"RequirementMet", "boolean"}},
	WeaponDamage = {{"MinDamage", "number"}, {"MaxDamage", "number"}, _Label, {"DamageType", "number"}, _Unused},
	WeaponDamagePenalty = {_Label},
	WeaponCritMultiplier = {_Label, _Unused, _Unused, _Unused, _Value},
	WeaponCritChance = {_Label, _Value, _Unused, _Unused},
	WeaponRange = {_Label, _Unused, _Value, _Unused},
	Durability = {_Label, _NumValue, {"Max", "number"}, _Unused, _Unused},
	CanBackstab = {_Label, _Unused},
	AccuracyBoost = {_Label, _NumValue, _Unused},
	DodgeBoost = {_Label, _NumValue, _Unused},
	EquipmentUnlockedSkill = {_Label, _Value, {"Icon", "number"}},
	WandSkill = {_Label, _Value, _Icon, _Warning},
	WandCharges = {_Label, {"Value", "number"}, {"MaxValue", "number"}, _Unused, _Unused},
	ArmorValue = {_Label, _NumValue, _Unused, _Unused},
	ArmorSlotType = {_Label, _Unused, _Unused},
	Blocking = {_Label, _NumValue, _Unused, _Unused},
	NeedsIdentifyLevel = {_Label, _Unused, _Unused},
	IsQuestItem = {},
	PriceToIdentify = {_Label, _Value, _Unused},
	PriceToRepair = {_Label, _Value, _Unused},
	PickpocketInfo = {_Label, _Unused},
	Engraving = {_Label, _Unused},
	ContainerIsLocked = {_Label, _Unused},
	Tags = {_Label, _Value, _Warning},
	SkillName = {_Label},
	SkillIcon = {_Label},
	SkillSchool = {_Label, {"Icon", "number"}},
	SkillTier = {_Label, _Unused},
	SkillRequiredEquipment = {_Label, {"RequirementMet", "boolean"}},
	SkillAPCost = {_Label, _NumValue, _Warning, {"RequirementMet", "boolean"}},
	SkillCooldown = {_Label, _NumValue, _Warning, _Unused, {"ValueText", "string"}},
	SkillDescription = {_Label},
	SkillDamage = {_Label, {"MinValue", "number"}, {"MaxValue", "number"}, {"DamageType", "number"}},
	SkillRange = {_Value, _Unused, _Label},
	SkillExplodeRadius = {_Label, _Unused, _Value},
	SkillCanPierce = {_Label, _Value},
	SkillCanFork = {_Label, _Value, _Unused, _Unused, _Unused},
	SkillStrikeCount = {_Label, _Value, _Unused},
	SkillProjectileCount = {_Label, _Value, _Unused},
	SkillCleansesStatus = {_Label, _Value, _Unused},
	SkillMultiStrikeAttacks = {_Label, _Value, _Unused, _Unused},
	SkillWallDistance = {_Label, _Value, _Unused},
	SkillPathSurface = {_Label, _Value, _Unused},
	SkillPathDistance = {_Label, _Value, _Unused},
	SkillHealAmount = {_Label, _Unused, _Unused, _Value},
	SkillDuration = {_Label, _NumValue, _Unused, _Warning},
	ConsumableEffectUknown = {_Label, _Unused},
	Reflection = {_Label},
	SkillAlreadyLearned = {_Label},
	SkillOnCooldown = {_Label},
	SkillAlreadyUsed = {_Label},

	AbilityTitle = {_Label},
	AbilityDescription = {{"AbilityId", "number"}, {"Description", "string"}, {"Description2", "string"}, {"CurrentLevelEffect", "string"}, {"NextLevelEffect", "string"}},
	
	TalentTitle = {_Label},
	TalentDescription = {{"TalentId", "number"}, {"Description", "string"}, {"Requirement", "string"}, {"IncompatibleWith", "string"}, {"Selectable", "boolean"}, {"Unknown", "boolean"}},

	SkillMPCost = {_Label, _NumValue, {"RequirementMet", "boolean"}},
	MagicArmorValue = {_Label, _NumValue, _Unused, {"RequirementMet", "boolean"}},
	WarningText = {_Label},
	RuneSlot = {_Label, _Value, _Unused},
	RuneEffect = {{"Unknown1", "number"}, {"Rune1", "string"}, {"Rune2", "string"}, {"Rune3", "string"}, _Label, {"Label2", "string"}},
	Equipped = {{"EquippedBy", "string"}, _Label, {"Slot", "string"}},
	ShowSkillIcon = {_Unused},
	SkillbookSkill = {_Label, _Value, {"Icon", "number"}},
	EmptyRuneSlot = {_Label, _Value, _Unused},

	StatName = {_Label},
	StatsDescription = {_Label},
	StatsDescriptionBoost = {_Label, _NumValue},

	StatSTRWeight = {_Label},
	StatMEMSlot = {_Label},
	StatsPointValue = {_Label},
	StatsTalentsBoost = {_Label},
	StatsTalentsMalus = {_Label},
	StatsBaseValue = {_Label},
	StatsPercentageBoost = {_Label},
	StatsPercentageMalus = {_Label},
	StatsPercentageTotal = {_Label, _NumValue},
	StatsGearBoostNormal = {_Label},
	StatsATKAPCost = {_Label},
	StatsCriticalInfos = {_Label},
	StatsAPTitle = {_Label},
	StatsAPDesc = {_Label},
	StatsAPBase = {_Label},
	StatsAPBonus = {_Label},
	StatsAPMalus = {_Label},
	StatsTotalDamage = {_Label},

	TagDescription = {_Label, {"Image", "number"}},

	StatusImmunity = {_Label},
	StatusBonus = {_Label},
	StatusMalus = {_Label},
	StatusDescription = {_Label},

	--Unused / throw errors
	
	Title = {_Label},
	SurfaceDescription = {_Label},
	Duration = {_Label},

	Fire = {_Label},
	Water = {_Label},
	Earth = {_Label},
	Air = {_Label},
	Poison = {_Label},
	Physical = {_Label},
	Sulfur = {_Label},
	Heal = {_Label},

	Splitter = {}
}

TooltipStatAttributes = {
	[0x0] = "Strength",
	[0x1] = "Finesse",
	[0x2] = "Intelligence",
	[0x3] = "Constitution",
	[0x4] = "Memory",
	[0x5] = "Wits",
	[0x6] = "Damage",
	[0x7] = "Armor",
	[0x8] = "MagicArmor",
	[0x9] = "CriticalChance",
	[0xA] = "Accuracy",
	[0xB] = "Dodge",
	[0xC] = "Vitality",
	[0xD] = "APRecovery",
	[0xE] = "Source",
	[0x11] = "Sight",
	[0x12] = "Hearing",
	[0x14] = "Movement",
	[0x15] = "Initiative",
	[0x17] = "PiercingResistance",
	[0x18] = "PhysicalResistance",
	[0x19] = "CorrosiveResistance",
	[0x1A] = "MagicResistance",
	[0x1B] = "ShadowResistance",
	[0x1C] = "FireResistance",
	[0x1D] = "WaterResistance",
	[0x1E] = "EarthResistance",
	[0x1F] = "AirResistance",
	[0x20] = "PoisonResistance",
	[0x21] = "CustomResistance",
	[0x24] = "Experience",
	[0x25] = "NextLevelExperience",
	[0x26] = "MaxAP",
	[0x27] = "StartAP",
	[0x28] = "APRecovery2",
	[0x2A] = "MinDamage",
	[0x2B] = "MaxDamage",
	[0x2C] = "LifeSteal",
	[0x2D] = "Gain",
}

_TT.TooltipStatAttributes = TooltipStatAttributes

--- @param ui UIObject
--- @param name string MainTimeline property name to fetch
--- @return table
function TableFromFlash(ui, name, this)
	local value
	local idx = 0
	local tbl = {}

	local this = this or ui:GetRoot()
	local arr = this[name]
	if not arr then
		_PrintError("No array with name", name, "in UI", ui.Path)
		return tbl
	end

	repeat
		value = arr[idx]
		idx = idx + 1
		if value ~= nil then
			table.insert(tbl, value)
		end
	until value == nil

	return tbl
end

--- @param ui UIObject
--- @param name string MainTimeline property name to write
--- @param tbl table Table to convert to Flash
--- @param this FlashMainTimeline|nil
function TableToFlash(ui, name, tbl, this)
	local this = this or ui:GetRoot()
	local arr = this[name]
	if arr ~= nil then
		for i=1,#tbl do
			arr[i-1] = tbl[i]
		end
	end
end

--- @param ui UIObject Tooltip UI object
--- @param propertyName string Flash property name (tooltip_array, tooltipCompare_array, etc.)
--- @param tooltipArray table Tooltip array
--- @param originalTooltipArray table Unmodified tooltip array
function ReplaceTooltipArray(ui, propertyName, tooltipArray, originalTooltipArray)
	local this = ui:GetRoot()
	TableToFlash(ui, propertyName, tooltipArray, this)
	if #tooltipArray < #originalTooltipArray then
		-- Pad out the tooltip array with dummy values
		for i=#tooltipArray,#originalTooltipArray do
			this[propertyName][i] = TooltipItemTypes.IsQuestItem
		end
	end
end

function ParseTooltipElement(tt, index, spec, typeName)
	if #tt - index + 1 < #spec then
		_PrintError("Not enough fields to parse spec @" .. index)
		return
	end

	local element = {Type = typeName}
	for i,field in pairs(spec) do
		local val = tt[index + i - 1]
		if field[1] ~= nil then
			element[field[1]] = val
		end
		if _DEBUG and (field[2] ~= nil and type(val) ~= field[2]) then
			_PrintWarning("Type of field " .. typeName .. "." .. field[1] .. " differs: " .. type(val) .. " vs " .. field[2] .. ":", val)
		end
	end

	return index + #spec, element
end

function ParseTooltipSkillProperties(tt, index)
	local element = {
		Type = "SkillProperties",
		Properties = {},
		Resistances = {}
	}

	local numProps = tt[index + 1]
	index = index + 2

	for i=1,numProps do
		local prop = {
			Label = tt[index],
			Warning = tt[index + 1]
		}
		index = index + 2
		table.insert(element.Properties, prop)
	end
	
	local numResistances = tt[index]
	index = index + 1

	for i=1,numResistances do
		local resist = {
			Label = tt[index],
			Value = tt[index + 1]
		}
		index = index + 2
		table.insert(element.Resistances, resist)
	end

	return index, element
end

function ParseTooltipArmorSet(tt, index)
	local element = {
		Type = "ArmorSet",
		GrantedStatuses = {},
		GrantedStatuses2 = {}
	}

	element.SetName = tt[index]
	element.FoundPieces = tt[index + 1]
	element.TotalPieces = tt[index + 2]
	element.SetDescription = tt[index + 3]
	local numStatuses = tt[index + 4]
	index = index + 5

	for i=1,numStatuses do
		local prop = {
			Label = tt[index],
			IconIndex = tt[index + 1]
		}
		index = index + 2
		table.insert(element.GrantedStatuses, prop)
	end
	
	local numStatuses2 = tt[index]
	index = index + 1

	for i=1,numStatuses2 do
		local resist = {
			Label = tt[index],
			IconIndex = tt[index + 1]
		}
		index = index + 2
		table.insert(element.GrantedStatuses2, resist)
	end

	return index, element
end

--- @param tt table Flash tooltip array
--- @return TooltipElement[]
function ParseTooltipArray(tt)
	local index = 1
	local element
	local elements = {}

	while index <= #tt do
		local id = tt[index]
		index = index + 1

		if TooltipItemIds[id] == nil then
			if _DEBUG then
				Ext.Dump(tt)
				_PrintError("[Game.Tooltip] Encountered unknown tooltip item type: ", index, id)
			end
			return elements
		end

		local typeName = TooltipItemIds[id]
		if typeName == "SkillProperties" then
			index, element = ParseTooltipSkillProperties(tt, index)
		elseif typeName == "ArmorSet" then
			index, element = ParseTooltipArmorSet(tt, index)
		else
			local spec = TooltipSpecs[typeName]
			if spec == nil then
				_PrintError("No spec available for tooltip item type: ", typeName)
				return elements
			end

			index, element = ParseTooltipElement(tt, index, spec, typeName)
			if element == nil then
				return elements
			end
		end

		table.insert(elements, element)
	end

	return elements
end

function EncodeTooltipElement(tt, spec, element)
	for i,field in pairs(spec) do
		local name = field[1]
		local fieldType = field[2]
		local val = element[name]
		if name == nil then
			table.insert(tt, "")
		else
			if fieldType ~= nil and type(val) ~= fieldType then
				if _DEBUG then
					_PrintWarning("Type of field " .. element.Type .. "." .. name .. " differs: " .. type(val) .. " vs " .. fieldType .. ":", val)
				end
				val = nil
			end

			if val == nil then
				if fieldType == "boolean" then
					val = false
				elseif fieldType == "number" then
					val = 0
				else
					val = ""
				end
			end

			table.insert(tt, val)
		end
	end
end

function EncodeTooltipSkillProperties(tt, element)
	local properties = element.Properties or {}
	table.insert(tt, "")
	table.insert(tt, #properties)
	for i,prop in pairs(properties) do
		table.insert(tt, prop.Label or "")
		table.insert(tt, prop.Warning or "")
	end

	local resistances = element.Resistances or {}
	table.insert(tt, #resistances)
	for i,prop in pairs(resistances) do
		table.insert(tt, prop.Label or "")
		table.insert(tt, prop.Value or "")
	end
end

function EncodeTooltipArmorSet(tt, element)
	local statuses = element.GrantedStatuses or {}
	local statuses2 = element.GrantedStatuses2 or {}

	table.insert(tt, element.SetName or "")
	table.insert(tt, element.FoundPieces or 0)
	table.insert(tt, element.TotalPieces or 0)
	table.insert(tt, element.SetDescription or "")

	table.insert(tt, #statuses)
	for i,status in pairs(statuses) do
		table.insert(tt, status.Label or "")
		table.insert(tt, status.IconIndex or "")
	end

	table.insert(tt, #statuses2)
	for i,status in pairs(statuses2) do
		table.insert(tt, status.Label or "")
		table.insert(tt, status.IconIndex or "")
	end
end

--- @param elements table Flash tooltip array
--- @return table
function EncodeTooltipArray(elements)
	local tt = {}
	for i=1,#elements do
		local element = elements[i]
		if element then
			local type = TooltipItemTypes[element.Type]
			if type == nil then
				if _DEBUG then
					_PrintError("Couldn't encode tooltip element with unknown type:", element.Type)
					_Dump(element)
				end
			else
				if element.Type == "SkillProperties" then
					table.insert(tt, type)
					EncodeTooltipSkillProperties(tt, element)
				elseif element.Type == "ArmorSet" then
					table.insert(tt, type)
					EncodeTooltipArmorSet(tt, element)
				else
					local spec = TooltipSpecs[element.Type]
					if spec == nil then
						if _DEBUG then
							_PrintError("No encoder found for tooltip element type:", element.Type)
							_Dump(element)
						end
					else
						table.insert(tt, type)
						EncodeTooltipElement(tt, spec, element)
					end
				end
			end
		end
	end
	return tt
end

function DebugTooltipEncoding(ui)
	local tooltipArray = TableFromFlash(ui, "tooltip_array")
	local tooltipArray2 = {}

	for i,s in pairs(tooltipArray) do
		if s ~= nil and type(s) == "number" and TooltipItemIds[s] ~= nil then
			s = "TYPE: " .. TooltipItemIds[s]
		end

		tooltipArray2[i] = s
	end

	_Print("tooltip_array: " .. _Stringify(tooltipArray2))
	local parsed = ParseTooltipArray(tooltipArray)
	_Print("Parsed: " .. _Stringify(parsed))
	local encoded = EncodeTooltipArray(parsed)
	local parsed2 = ParseTooltipArray(encoded)
	_Print("Encoding matches: ", _Stringify(parsed2) == _Stringify(parsed))
end

---@class TooltipRequest:table
---@field Type TooltipRequestType
---@field UIType integer The UI type ID for the UI that initially called for a tooltip.
---@field TooltipUIType integer The UI type ID for the tooltip UI.
---@field ObjectHandleDouble number|nil

---@class TooltipItemRequest:TooltipRequest
---@field Item EclItem
---@field Character EclCharacter

---@class TooltipPyramidRequest:TooltipRequest
---@field Item EclItem

---@class TooltipRuneRequest:TooltipRequest
---@field Item EclItem
---@field Character EclCharacter
---@field Rune StatEntryObject The rune stat entry.
---@field Slot integer
---@field StatsId string The rune stat id.

---@class TooltipSkillRequest:TooltipRequest
---@field Character EclCharacter
---@field Skill string

---@class TooltipStatusRequest:TooltipRequest
---@field Character EclCharacter
---@field Status EclStatus
---@field StatusId string

---@class TooltipStatRequest:TooltipRequest
---@field Character EclCharacter
---@field Stat string

---@class TooltipAbilityRequest:TooltipRequest
---@field Character EclCharacter
---@field Ability string

---@class TooltipTalentRequest:TooltipRequest
---@field Character EclCharacter
---@field Talent string

---@class TooltipTagRequest:TooltipRequest
---@field Character EclCharacter
---@field Tag string
---@field Category string

---@class TooltipCustomStatRequest:TooltipRequest
---@field Character EclCharacter
---@field Stat number The stat handle.
---@field StatIndex integer The stat index in the characterSheet array.
---@field StatData {ID:string, __tostring:fun(tbl:table):string}|nil Custom stats data that returns a string ID via the metamethod __tostring. Must be implemented by a mod, otherwise this is nil.

---@class TooltipSurfaceRequest:TooltipRequest
---@field Character EclCharacter
---@field Ground string|nil
---@field Cloud string|nil

---@class TooltipGenericRequest:TooltipRequest
---@field Text string
---@field X number|nil
---@field Y number|nil
---@field Width number|nil
---@field Height number|nil
---@field Side string|nil
---@field AllowDelay boolean|nil
---@field AnchorEnum integer|nil
---@field BackgroundType integer|nil
---@field IsCharacterTooltip boolean|nil

---@class TooltipWorldRequest:TooltipRequest
---@field Text string Set this to change the resulting world tooltip text.
---@field X number
---@field Y number
---@field IsFromItem boolean
---@field Item EclItem|nil

---@class TooltipPlayerPortraitRequest:TooltipRequest
---@field Text string Set this to change the resulting tooltip text.
---@field X number
---@field Y number
---@field Width number|nil
---@field Height number|nil
---@field Side string|nil
---@field Character EclCharacter

local previousListeners = {}

---@class TooltipHooks
local _ttHooks = {
	---@type TooltipRequest
	NextRequest = nil,
	ActiveType = "",
	Last = {
		---@type TooltipRequest
		Request = nil,
		Event = "",
		Type = "",
		---@type integer
		UIType = -1,
	},
	IsOpen = false,
	SessionLoaded = false,
	InitializationRequested = false,
	Initialized = false,
	GlobalListeners = {},
	TypeListeners = {},
	ObjectListeners = {},
	RequestListeners = {
		All = {
			Before = {},
			After = {},
		}
	},
	BeforeNotifyListeners = {
		All = {
			Before = {},
			After = {},
		},
	},
}

---@type TooltipHooks
TooltipHooks = {}

setmetatable(TooltipHooks, {
	__index = function(_,k)
		return _ttHooks[k]
	end,
	__newindex = function(_,k,v)
		local t = type(_ttHooks[k])
		if t == "nil" or t ~= "function" then
			_ttHooks[k] = v
		end
	end
})

RequestProcessor.Tooltip = _ttHooks

if previousListeners.GlobalListeners then
	for _,v in pairs(previousListeners.GlobalListeners) do
		_ttHooks.GlobalListeners[#_ttHooks.GlobalListeners+1] = v
	end
end

if previousListeners.TypeListeners then
	for t,v in pairs(previousListeners.TypeListeners) do
		if _ttHooks.TypeListeners[t] == nil then
			_ttHooks.TypeListeners[t] = v
		else
			for _,v2 in pairs(v) do
				table.insert(_ttHooks.TypeListeners[t], v2)
			end
		end
	end
end

if previousListeners.ObjectListeners then
	for t,v in pairs(previousListeners.ObjectListeners) do
		if _ttHooks.ObjectListeners[t] == nil then
			_ttHooks.ObjectListeners[t] = v
		else
			for k,v2 in pairs(v) do
				_ttHooks.ObjectListeners[t][k] = v2
			end
		end
	end
end

--Auto-completion
_TT.TooltipHooks = TooltipHooks

---@class TooltipArrayData
---@field Main string
---@field CompareMain string|nil
---@field CompareOff string|nil

local TooltipArrayNames = {
	---@type TooltipArrayData
	Default = {
		Main = "tooltip_array",
		CompareMain = "tooltipCompare_array",
		CompareOff = "tooltipOffHand_array",
	},
	Surface = {Main = "tooltipArray" },
	Console = {
		CharacterCreation = {
			Main = "tooltipArray",
		},
		ContainerInventory = {
			Main = "tooltip_array",
		},
		PartyInventory = {
			Main = "tooltip_array",
			CompareMain = "compareTooltip_array",
			CompareOff = "offhandTooltip_array"
		},
		BottomBar = {
			Main = "tooltip_array"
		},
		Examine = {
			Main = "tooltipArray"
		},
		Trade = {
			Main = "tooltip_array",
			CompareMain = "tooltipCompare_array",
			CompareOff = "equipOffhandTooltip_array"
		},
		Reward = {
			Main = "tooltip_array",
		},
		EquipmentPanel = {
			Main = "equipTooltip_array",
			CompareMain = "tooltip_array",
		},
		StatsPanel = {
			Main = "tooltipArray",
		},
		CraftPanel = {
			Main = "tooltip_array",
		}
	}
}
_TT.TooltipArrayNames = TooltipArrayNames

---@type table<string, {Callback:function, TypeIds:table<integer,boolean>|nil, When:string|nil}>
local CallHandlers = {}
local InvokeHandlers = {}

---@param event string|string[]
---@param callback fun(e:EclLuaUICallEvent, ui:UIObject, event:string, ...:any)
---@param typeIds table<integer,boolean>|integer|nil
---@param when "Before"|"After"|nil
local function _CallHandler(event, callback, typeIds, when)
	local idType = type(typeIds)
	if idType == "number" then
		local t = typeIds
		typeIds = {[t] = true}
	elseif idType ~= "nil" and idType ~= "table" then
		_PrintError("[Game.Tooltip._CallHandler] Bad typeID(%s) type(%s)", typeIds, idType)
	end
	if type(event) == "table" then
		for i=1,#event do
			_CallHandler(event[i], callback, typeIds, when)
		end
	else
		CallHandlers[event] = {
			Callback = callback,
			TypeIds = typeIds,
			When = when
		}
	end
end

---@param event string|string[]
---@param callback fun(e:EclLuaUICallEvent, ui:UIObject, event:string, ...:any)
---@param typeIds table<integer,boolean>|integer|nil
---@param when "Before"|"After"|nil
local function _InvokeHandler(event, callback, typeIds, when)
	local idType = type(typeIds)
	if idType == "number" then
		local t = typeIds
		typeIds = {[t] = true}
	elseif idType ~= "nil" and idType ~= "table" then
		_PrintError("[Game.Tooltip._InvokeHandler] Bad typeID(%s) type(%s)", typeIds, idType)
	end
	if type(event) == "table" then
		for i=1,#event do
			_CallHandler(event[i], callback, typeIds, when)
		end
	else
		InvokeHandlers[event] = {
			Callback = callback,
			TypeIds = typeIds,
			When = when
		}
	end
end

---@param id integer
---@return {Main:string, CompareMain:string|nil, CompareOff:string|nil}|nil
local _UITypeToArray = function(id)
	if id == _UITYPE.craftPanel_c then
		return TooltipArrayNames.Console.CraftPanel
	elseif id == _UITYPE.equipmentPanel_c then
		return TooltipArrayNames.Console.EquipmentPanel
	elseif id == _UITYPE.bottomBar_c then
		return TooltipArrayNames.Console.BottomBar
	elseif id == _UITYPE.containerInventory.Default or id == _UITYPE.containerInventory.Pickpocket then
		return TooltipArrayNames.Console.ContainerInventory
	elseif id == _UITYPE.statsPanel_c then
		return TooltipArrayNames.Console.StatsPanel
	elseif id == _UITYPE.examine_c then
		return TooltipArrayNames.Console.Examine
	elseif id == _UITYPE.partyInventory_c then
		return TooltipArrayNames.Console.PartyInventory
	elseif id == _UITYPE.reward_c then
		return TooltipArrayNames.Console.Reward
	elseif id == _UITYPE.characterCreation_c then
		return TooltipArrayNames.Console.CharacterCreation
	elseif id == _UITYPE.trade_c then
		return TooltipArrayNames.Console.Trade
	end
	return nil
end

local _ControllerTooltipInvokes = {
	[_UITYPE.equipmentPanel_c] = {"updateTooltip", "updateEquipTooltip"},
	[_UITYPE.craftPanel_c] = "updateTooltip",
	[_UITYPE.containerInventory.Default] = "updateTooltip",
	[_UITYPE.containerInventory.Pickpocket] = "updateTooltip",
	[_UITYPE.examine_c] = "showFormattedTooltip",
	[_UITYPE.statsPanel_c] = "showTooltip",
	[_UITYPE.bottomBar_c] = "updateTooltip",
	[_UITYPE.partyInventory_c] = "updateTooltip",
	[_UITYPE.reward_c] = "updateTooltipData",
	[_UITYPE.characterCreation_c] = "showTooltip",
	[_UITYPE.characterCreation_c] = "showTooltip",
}

function _ttHooks:RegisterControllerHooks()
	local function _RenderControllerTooltip(e, ui, event, ...)
		local arrayNames = _UITypeToArray(ui.Type)
		if arrayNames then
			self:OnRenderTooltip(arrayNames, ui, event, ...)
		end
	end

	for typeId,event in pairs(_ControllerTooltipInvokes) do
		_InvokeHandler(event, _RenderControllerTooltip, typeId)
	end

	-- This allows examine_c to have a character reference
	_InvokeHandler("updateOHs", function (e, ui, event, ...)
		if RequestProcessor.ControllerEnabled then
			---@type {selectionInfo_array:FlashArray<number>}
			local main = ui:GetRoot()
			if main then
				for i=0,#main.selectionInfo_array,21 do
					local id = main.selectionInfo_array[i]
					if id and not _IsNaN(id) then
						ControllerVars.LastOverhead = id
						break
					end
				end
			end
		end
	end, _UITYPE.overhead)

	_InvokeHandler("setPlayerHandle", function (e, ui, event, doubleHandle)
		if doubleHandle ~= nil and doubleHandle ~= 0 then
			local handle = _DoubleToHandle(doubleHandle)
			if _IsValidHandle(handle) then
				local character = _GetCharacter(handle)
				if character then
					ControllerVars.LastPlayer = character.NetID
				end
			end
		end
	end, _UITYPE.bottomBar_c)

	---@param self TooltipHooks
	---@return EclCharacter
	self.GetLastPlayer = function(self)
		if RequestProcessor.ControllerEnabled then
			if ControllerVars.LastPlayer then
				local character = _GetCharacter(ControllerVars.LastPlayer)
				if character then
					return character
				end
			end
			local ui = _GetUIByType(_UITYPE.bottomBar_c)
			if ui then
				---@type {characterHandle:number}
				local this = ui:GetRoot()
				if this and not _IsNaN(this.characterHandle) then
					local character = RequestProcessor.Utils.GetObjectFromDouble(this.characterHandle)
					if character then
						ControllerVars.LastPlayer = character.NetID
						return character
					end
				end
			end
		end
		return nil
	end
end

function _ttHooks:Init()
	if self.Initialized then
		return
	end

	RequestProcessor:Init(_ttHooks)

	Ext.Events.UIInvoke:Subscribe(function (e)
		local handler = InvokeHandlers[e.Function]
		if handler and ((not handler.When and e.When == "After") or e.When == handler.When) then
			if handler.TypeIds then
				if handler.TypeIds[e.UI.Type] == true then
					handler.Callback(e, e.UI, e.Function, table.unpack(e.Args))
				end
			else
				handler.Callback(e, e.UI, e.Function, table.unpack(e.Args))
			end
		end
	end, {Priority=2})
	
	Ext.Events.UICall:Subscribe(function (e)
		local handler = CallHandlers[e.Function]
		if handler and ((not handler.When and e.When == "After") or e.When == handler.When) then
			if handler.TypeIds then
				if handler.TypeIds[e.UI.Type] == true then
					handler.Callback(e, e.UI, e.Function, table.unpack(e.Args))
				end
			else
				handler.Callback(e, e.UI, e.Function, table.unpack(e.Args))
			end
		end
	end, {Priority=2})

	_InvokeHandler("addFormattedTooltip", function (e, ui, call, x, y, b, ...)
		_ttHooks.Last.Position = {x,y}
		self:OnRenderTooltip(TooltipArrayNames.Default, ui, call, x, y, b, ...)
	end)

	_InvokeHandler("addStatusTooltip", function (e, ui, call, x, y, b, ...)
		_ttHooks.Last.Position = {x,y}
		self:OnRenderTooltip(TooltipArrayNames.Default, ui, call, x, y, b, ...)
	end)

	_InvokeHandler("displaySurfaceText", function (e, ...)
		self:OnRenderTooltip(TooltipArrayNames.Surface, ...)
	end)
	
	--public function addTooltip(text:String, widthOverride:Number = 0, heightOverride:Number = 18, allowDelay:Boolean = true, stickToMouse:Number = 0, bgType:uint = 0)
	_InvokeHandler("addTooltip", function (e, ui, event, text, widthOverride, heightOverride, allowDelay, stickToMouse, bgType)
		self:OnRenderGenericTooltip(e, ui, event, text, widthOverride, heightOverride, allowDelay, stickToMouse, bgType)
	end, nil, "Before")

	_CallHandler("hideTooltip", function (e, ui, event, ...)
		self.IsOpen = false
		self.ActiveType = ""
		if self.NextRequest and self.NextRequest.Type == "Generic" then
			self.Last.Request = self.NextRequest
			self.NextRequest = nil
		end
		local tt = _GetUIByType(_UITYPE.tooltip)
		if tt then
			if #tooltipCustomIcons > 0 then
				for _,v in pairs(tooltipCustomIcons) do
					tt:ClearCustomIcon(v)
				end
				tooltipCustomIcons = {}
			end
		end
	end)

	self:RegisterControllerHooks()

	self.Initialized = true
end

local _GenericTooltipTypes = {
	Generic = true,
	WorldHover = true,
	PlayerPortrait = true,
}

local function _GetCurrentCharacter()
	return _GetClientCharacter() or {}
end

local function _GetRequestCharacter(req)
	return req.Character or _GetCurrentCharacter()
end

---@param e EclLuaUICallEvent
---@param ui UIObject
---@param method string
function _ttHooks:OnRenderGenericTooltip(e, ui, method, text, widthOverride, heightOverride, allowDelay, anchorEnum, backgroundType)
	---@type TooltipGenericRequest|TooltipPlayerPortraitRequest|TooltipWorldRequest
	local req = self.NextRequest
	if not req then
		return
	end
	if _GenericTooltipTypes[req.Type] then
		if req.Type == "WorldHover" then
			req.Type = "World"
		end

		if req.IsCharacterTooltip then
			req.Text = text
		end
	
		self.IsOpen = true
	
		req.AllowDelay = allowDelay
		req.AnchorEnum = anchorEnum
		req.BackgroundType = backgroundType
	
		local tooltipData = TooltipData:Create({{
			Type = "GenericDescription",
			Label = text,
			X = 0,
			Y = 0,
			Width = widthOverride,
			Height = heightOverride,
			AllowDelay = req.AllowDelay,
			AnchorEnum = req.AnchorEnum,
			BackgroundType = req.BackgroundType,
			OverrideSize = false,
		}}, ui.Type, req.UIType)

		self.ActiveType = req.Type
		self.Last.Type = req.Type

		if req.Type == "World" then
			local item = req.Item
			if item then
				self:NotifyListeners("World", item.StatsId, req, tooltipData, item)
			else
				self:NotifyListeners("World", nil, req, tooltipData, nil)
			end
		elseif req.Type == "PlayerPortrait" then
			local character = _GetRequestCharacter(req)
			self:NotifyListeners("PlayerPortrait", nil, req, tooltipData, character)
		else
			self:NotifyListeners("Generic", nil, req, tooltipData)
		end
	
		local desc = tooltipData:GetDescriptionElement()
		if desc then
			e.Args[1] = desc.Label or ""
			e.Args[2] = desc.Width or req.Width
			e.Args[3] = desc.Height or req.Height
			e.Args[4] = desc.AllowDelay or req.AllowDelay
			e.Args[5] = desc.AnchorEnum or req.AnchorEnum
			e.Args[6] = desc.BackgroundType or req.BackgroundType
			if desc.OverrideSize then

			end
		else
			desc = {}
			e.Args[1] = ""
		end

		local sizeChanged = false

		local this = ui:GetRoot()
		if this then
			local target = this.defaultTooltip
			if target then
				if desc.OverrideSize then
					local w = desc.Width or req.Width
					local h = desc.Height or req.Height
					if w > 0 then
						target.widthOverride = w
						sizeChanged = true
					end
					if h > 0 then
						target.heightOverride = h
						sizeChanged = true
					end
				else
					target.widthOverride = -1
					target.heightOverride = -1
				end
			end
		end

		local requestedText = e.Args[1] or ""
		local textChanged = requestedText ~= "" and requestedText ~= text
		local finalText = text:gsub("<bp>","<img src=\'Icon_BulletPoint\'>"):gsub("<line>","<img src=\'Icon_Line\'>"):gsub("<shortLine>","<img src=\'Icon_Line\' width=\'85%\'>")
		local bgType = e.Args[6] or 0
		local t = ui.Type
		--TODO PlayerPortrait tooltips get continually set with addTooltip invokes, despite no more calls happening, possibly due to ticking statuses (they display in the text)
		Ext.OnNextTick(function (e)
			local ui = _GetUIByType(t)
			if ui then
				local this = ui:GetRoot()
				if this and this.tf then
					local tt = this.tf
					if sizeChanged then
						tt.widthOverride = -1
						tt.heightOverride = -1
					end
					if textChanged and tt.setText then
						tt.shortDesc = finalText
						tt.setText(finalText, bgType)
					end
				end
			end
		end)
	
		self.Last.Request = self.NextRequest
		self.NextRequest = nil
	end
end

---@param ui UIObject
---@param item EclItem
---@return EclCharacter
function _ttHooks:GetCompareOwner(ui, item)
	local owner = ui:GetPlayerHandle()

	if owner ~= nil then
		local char = _GetCharacter(owner)
		if char.Stats.IsPlayer then
			return char
		end
	end

	local handle = nil
	if not RequestProcessor.ControllerEnabled then
		local hotbar = _GetUIByType(_UITYPE.hotBar)
		if hotbar ~= nil then
			---@type {hotbar_mc:{characterHandle:number}}
			local main = hotbar:GetRoot()
			if main ~= nil then
				handle = _DoubleToHandle(main.hotbar_mc.characterHandle)
			end
		end
	else
		local hotbar = _GetUIByType(_UITYPE.bottomBar_c)
		if hotbar ~= nil then
			---@type {characterHandle:number}
			local main = hotbar:GetRoot()
			if main ~= nil then
				handle = _DoubleToHandle(main.characterHandle)
			end
		end
	end

	if handle ~= nil then
		local char = _GetCharacter(handle)
		if char then
			return char
		end
	end

	local character = RequestProcessor.Utils.GetClientCharacter()

	if character == nil then
		--Fallback to the item's owner last, since it may not be the active character.
		local itemOwner = item:GetOwnerCharacter()
		if itemOwner ~= nil then
			local ownerCharacter = _GetCharacter(itemOwner)
			if ownerCharacter ~= nil and ownerCharacter.Stats.IsPlayer then
				return ownerCharacter
			end
		end
	else
		return character
	end
	
	return nil
end

--- @param ui UIObject
--- @param item EclItem
--- @param offHand boolean
--- @return EclItem|nil
function _ttHooks:GetCompareItem(ui, item, offHand)
	local char = self:GetCompareOwner(ui, item)

	if char == nil then
		_PrintWarning("Tooltip compare render failed: Couldn't find owner of item", item.StatsId)
		return nil
	end

	local statSlot = nil

	if item.Stats and item.Stats.ItemSlot then
		statSlot = item.Stats.ItemSlot
	elseif item.StatsFromName and item.StatsFromName.ModifierListIndex < 3 then -- 0 is Weapon, 1 is Armor, 2 is Shield, 3 is Potion, 4 is Object
		statSlot = item.StatsFromName.StatsEntry.Slot
	end

	if statSlot == nil then
		_PrintWarning("[Game.Tooltip:GetCompareItem] Trying to compare an item with no Stats?", item.StatsId)
		return nil
	end

	if statSlot == "Weapon" then
		if offHand then
			return char:GetItemObjectBySlot("Shield")
		else
			return char:GetItemObjectBySlot("Weapon")
		end
	elseif statSlot == "Ring" or statSlot == "Ring2" then
		if offHand then
			return char:GetItemObjectBySlot("Ring2")
		else
			return char:GetItemObjectBySlot("Ring")
		end
	else
		return char:GetItemObjectBySlot(statSlot)
	end
end

---@param arrayData TooltipArrayData
---@param ui UIObject
function _ttHooks:OnRenderTooltip(arrayData, ui, method, ...)
	if self.NextRequest == nil then
		if _DEBUG then
			_PrintWarning(string.format("[Game.Tooltip] Got tooltip render request, but did not find original tooltip info! method(%s)", method))
		end
		return
	end

	self.IsOpen = true
	
	---@type TooltipItemRequest
	local req = self.NextRequest
	self.ActiveType = req.Type
	self.Last.Type = req.Type
	self.Last.ArrayData = arrayData

	local arrayId = arrayData.Main
	local compareMain = arrayData.CompareMain
	local compareOff = arrayData.CompareOff
	local checkCompare = req.Type == "Item"

	if ui:GetTypeId() == _UITYPE.equipmentPanel_c then
		checkCompare = false	
		if method == "updateTooltip" then
			arrayId = TooltipArrayNames.Console.EquipmentPanel.CompareMain
		end
	end

	self:OnRenderSubTooltip(ui, arrayId, req, method, ...)
	
	--equipmentPanel_c updates each array in separate invokes, so we don't need to setup comparison tooltips
	if checkCompare then
		local this = ui:GetRoot()

		local reqItem = req.Item
		local mainArray = compareMain and this[arrayData.CompareMain]
		local compareArray = compareOff and this[arrayData.CompareOff]

		if mainArray and mainArray[0] ~= nil then
			local compareItem = self:GetCompareItem(ui, reqItem, false)
			if compareItem ~= nil then
				if _IsValidHandle(compareItem.Handle) then
					local lastObjectHandle = req.ObjectHandleDouble
					local lastStatsId = req.StatsId
					req.ObjectHandleDouble = _HandleToDouble(compareItem.Handle)
					req.StatsId = compareItem.StatsId
					self:OnRenderSubTooltip(ui, arrayData.CompareMain, req, method, ...)
					req.ObjectHandleDouble = lastObjectHandle
					req.StatsId = lastStatsId
				elseif _DEBUG then
					_PrintError("compareItem.Handle is nil?", _DumpExport(compareItem))
				end
			else
				_PrintError("Tooltip compare render failed: Couldn't find item to compare", method, compareMain)
			end
		end

		if compareArray and compareArray[0] ~= nil then
			local compareItem = self:GetCompareItem(ui, reqItem, true)
			if compareItem ~= nil then
				if _IsValidHandle(compareItem.Handle) then
					local lastObjectHandle = req.ObjectHandleDouble
					local lastStatsId = req.StatsId
					req.ObjectHandleDouble = _HandleToDouble(compareItem.Handle)
					req.StatsId = compareItem.StatsId
					self:OnRenderSubTooltip(ui, arrayData.CompareOff, req, method, ...)		
					req.ObjectHandleDouble = lastObjectHandle
					req.StatsId = lastStatsId
				elseif _DEBUG then
					_PrintError("compareItem.Handle is nil?", _DumpExport(compareItem))
				end
			else
				_PrintError("Tooltip compare render failed: Couldn't find off-hand item to compare", method, compareOff)
			end
		end
	end

	self.Last.Request = self.NextRequest
	self.NextRequest = nil
end

local function _RunNotifyListeners(self, req, ui, method, tooltip, ...)
	self:InvokeBeforeNotifyListeners(req, ui, method, tooltip, ...)
	if req.Type == "Stat" then
		local stat = req.Stat or ""
		self:NotifyListeners("Stat", stat, req, tooltip, _GetRequestCharacter(req), stat)
	elseif req.Type == "CustomStat" then
		if req.StatData ~= nil then
			self:NotifyListeners("CustomStat", tostring(req.StatData), req, tooltip, _GetRequestCharacter(req), req.StatData)
		else
			self:NotifyListeners("CustomStat", nil, req, tooltip, _GetRequestCharacter(req), {ID=req.Stat})
		end
	elseif req.Type == "Skill" then
		local skill = req.Skill or ""
		self:NotifyListeners("Skill", skill, req, tooltip, _GetRequestCharacter(req), skill)
	elseif req.Type == "Ability" then
		local ability = req.Ability or ""
		self:NotifyListeners("Ability", ability, req, tooltip, _GetRequestCharacter(req), ability)
	elseif req.Type == "Talent" then
		local talent = req.Talent or ""
		self:NotifyListeners("Talent", talent, req, tooltip, _GetRequestCharacter(req), talent)
	elseif req.Type == "Status" then
		local status = req.Status
		if not status then
			_PrintError("[Game.Tooltip:OnRenderSubTooltip] Failed to fetch status for upcoming status tooltip:", _DumpExport(req))
			--Try to preserve parameter order
			status = {}
		end
		self:NotifyListeners("Status", req.StatusId, req, tooltip, _GetRequestCharacter(req), status)
	elseif req.Type == "Item" then
		self:NotifyListeners("Item", req.StatsId, req, tooltip, req.Item or {Type="nil"})
	elseif req.Type == "Pyramid" then
		self:NotifyListeners("Pyramid", req.StatsId, req, tooltip, req.Item or {Type="nil"})
	elseif req.Type == "Rune" then
		self:NotifyListeners("Rune", req.StatsId, req, tooltip, req.Item or {Type="nil"}, req.Rune or {}, req.Slot or -1)
	elseif req.Type == "Tag" then
		self:NotifyListeners("Tag", req.Category, req, tooltip, req.Tag)
	elseif req.Type == "Surface" then
		if req.Ground then
			self:NotifyListeners("Surface", req.Ground, req, tooltip, _GetRequestCharacter(req), req.Ground)
		end
		if req.Cloud then
			self:NotifyListeners("Surface", req.Cloud, req, tooltip, _GetRequestCharacter(req), req.Cloud)
		end
		if not req.Cloud and not req.Ground then
			self:NotifyListeners("Surface", "Unknown", req, tooltip, _GetRequestCharacter(req), "Unknown")
		end
	elseif req.Type == "World" then
		-- Manually invoked in RequestProcessor, so the text array can be updated
		---@see GameTooltipRequestProcessorInternals#CreateWorldTooltipRequest
	elseif req.Type == "PlayerPortrait" then
		---@see TooltipHooks#OnRenderGenericTooltip
	elseif req.Type == "Generic" then
		---@see TooltipHooks#OnRenderGenericTooltip
	else
		_PrintError("Unknown tooltip type? ", req.Type)
	end
end

TooltipHooks._RunNotifyListeners = _RunNotifyListeners

---@param ui UIObject
---@param arrayId string
---@param req AnyTooltipRequest
---@param method string
function _ttHooks:OnRenderSubTooltip(ui, arrayId, req, method, ...)
	local tt = TableFromFlash(ui, arrayId)
	local params = ParseTooltipArray(tt)
	if params ~= nil then
		local tooltip = TooltipData:Create(params, ui:GetTypeId(), req.UIType)
		_RunNotifyListeners(self, req, ui, method, tooltip, ...)
		local newTooltip = EncodeTooltipArray(tooltip.Data)
		if newTooltip ~= nil then
			ReplaceTooltipArray(ui, arrayId, newTooltip, tt)
		end
	end
end

local function InvokeListenerTable(tbl, ...)
	if tbl then
		for i=1,#tbl do
			local b,err = xpcall(tbl[i], debug.traceback, ...)
			if not b then
				_PrintError(err)
			end
		end
	end
end

---@param requestType string
---@param listener fun(req:TooltipRequest)
function _ttHooks:RegisterBeforeNotifyListener(requestType, listener)
	if requestType == nil or requestType == "all" then
		requestType = "All"
	end
	if self.BeforeNotifyListeners[requestType] == nil then
		self.BeforeNotifyListeners[requestType] = {}
	end
	if self.BeforeNotifyListeners[requestType] == nil then
		self.BeforeNotifyListeners[requestType] = {}
	end
	table.insert(self.BeforeNotifyListeners[requestType], listener)
end

---@param request TooltipRequest
---@vararg string|boolean|number|IEoCClientObject
function _ttHooks:InvokeBeforeNotifyListeners(request, ...)
	local rTypeTable = self.BeforeNotifyListeners[request.Type]
	if rTypeTable then
		InvokeListenerTable(rTypeTable, request, ...)
	end
	InvokeListenerTable(self.BeforeNotifyListeners.All, request, ...)
end

---@param requestType string
---@param name string
---@param request TooltipRequest
---@param tooltip TooltipData
function _ttHooks:NotifyListeners(requestType, name, request, tooltip, ...)
	local args = {...}
	table.insert(args, tooltip)
	self:NotifyAll(self.TypeListeners[requestType], table.unpack(args))
	if name ~= nil and self.ObjectListeners[requestType] ~= nil then
		self:NotifyAll(self.ObjectListeners[requestType][name], table.unpack(args))
	end
	self:NotifyAll(self.GlobalListeners, request, tooltip, ...)
end

function _ttHooks:NotifyAll(listeners, ...)
	if not listeners then
		return
	end
	for i,callback in pairs(listeners) do
		local status, err = xpcall(callback, debug.traceback, ...)
		if not status then
			_PrintError("Error during tooltip callback: ", err)
		end
	end
end

---@param tooltipType string|nil
---@param tooltipID string|nil
---@param listener function
function _ttHooks:RegisterListener(tooltipType, tooltipID, listener)
	if not self.Initialized then
		self:Init()
	end

	if tooltipType == nil then
		table.insert(self.GlobalListeners, listener)
	elseif tooltipID == nil then
		if self.TypeListeners[tooltipType] == nil then
			self.TypeListeners[tooltipType] = {listener}
		else
			table.insert(self.TypeListeners[tooltipType], listener)
		end
	else
		local listeners = self.ObjectListeners[tooltipType]
		if listeners == nil then
			self.ObjectListeners[tooltipType] = {[tooltipID] = {listener}}
		else
			if listeners[tooltipID] == nil then
				listeners[tooltipID] = {listener}
			else
				table.insert(listeners[tooltipID], listener)
			end
		end
	end
end

---@param requestType string
---@param listener fun(req:TooltipRequest)
---@param state string
function _ttHooks:RegisterRequestListener(requestType, listener, state)
	if type(listener) ~= "function" then
		error(string.format("listener param must be a function (%s)", listener), 2)
	end
	if requestType == nil or requestType == "all" then
		requestType = "All"
	end
	if self.RequestListeners[requestType] == nil then
		self.RequestListeners[requestType] = {}
	end

	if state and type(state) == "string" then
		state = string.lower(state)
		state = state == "before" and "Before" or "After"
	else
		state = "After"
	end
	if self.RequestListeners[requestType][state] == nil then
		self.RequestListeners[requestType][state] = {}
	end
	table.insert(self.RequestListeners[requestType][state], listener)
end

function _ttHooks:InvokeRequestListeners(request, state, ...)
	local rTypeTable = self.RequestListeners[request.Type]
	if rTypeTable then
		InvokeListenerTable(rTypeTable[state], request, ...)
	end
	InvokeListenerTable(self.RequestListeners.All[state], request, ...)
end

---@class TooltipData
---@field Data TooltipElement[]
---@field ControllerEnabled boolean
---@field TooltipUIType integer
---@field UIType integer
---@field Instance UIObject
---@field Root FlashMainTimeline
local TooltipData = {}

_TT.TooltipData = TooltipData

---@param data TooltipElement[]
---@param tooltipUIType integer
---@param requestingUIType integer
---@return TooltipData
function TooltipData:Create(data, tooltipUIType, requestingUIType)
	local tt = {
		Data = data,
		ControllerEnabled = RequestProcessor.ControllerEnabled or false,
		TooltipUIType = tooltipUIType,
		UIType = requestingUIType
	}
	setmetatable(tt, {
		__index = function(tbl, k)
			if k == "Instance" then
				return _GetUIByType(tooltipUIType)
			elseif k == "Root" then
				local ui = _GetUIByType(tooltipUIType)
				if ui then
					return ui:GetRoot()
				end
			end
			return TooltipData[k]
		end
	})
	return tt
end

local DescriptionElements = {
	AbilityDescription = true,
	GenericDescription = true, -- World/Generic Tooltips
	Description = true,
	ItemDescription = true,
	SkillDescription = true,
	StatsDescription = true,
	StatusDescription = true,
	SurfaceDescription = true,
	TagDescription = true,
	TalentDescription = true,
}

---Gets whichever element is the description.
---@param fallback AnyTooltipDescriptionElement|nil If a description isn't found, add this element.
---@return AnyTooltipDescriptionElement
function TooltipData:GetDescriptionElement(fallback)
	---@type {Type:TooltipElementType, Label:string|nil}[]
	local elements = self.Data
	for _,element in pairs(elements) do
		if DescriptionElements[element.Type] then
			return element
		end
	end
	if fallback then
		self:AppendElement(fallback)
	end
	return fallback
end

local _CustomTooltipTypes = {
	Description = true,
	GenericDescription = true,
}

_TT.CustomTooltipTypes = _CustomTooltipTypes

local function _IsTooltipElement(ele)
	return type(ele) == "table" and (TooltipItemTypes[ele.Type] ~= nil or _CustomTooltipTypes[ele.Type])
end

local function _ElementTypeMatch(e,t,isTable)
	if isTable then
		for i=1,#t do
			if t[i] == e then
				return true
			end
		end
	elseif e == t then
		return true
	end
	return false
end

---@generic T:TooltipElement|TooltipElementType
---@param t `T`|`T`[] The tooltip element type, or an array of element types.
---@param fallback TooltipElement|nil If an element of the desired type isn't found, append and return this fallback element.
---@return T elementOrFallback
function TooltipData:GetElement(t, fallback)
	local isTable = type(t) == "table"
	for i=1,#self.Data do
		local element = self.Data[i]
		if element and _ElementTypeMatch(element.Type, t, isTable) then
			return element
		end
	end
	--If this element wasn't found, and fallback is set, append it.
	if _IsTooltipElement(fallback) then
		self:AppendElement(fallback)
		return fallback
	end
	return nil
end

---Get the last element in the tooltip data of the given type.
---@overload fun(self:TooltipData, t:TooltipElementType, fallback:TooltipElement|nil)
---@generic T:TooltipElement|TooltipElementType
---@param t `T`|`T`[] The tooltip element type, or an array of element types.
---@param fallback TooltipElement|nil If an element of the desired type isn't found, append and return this fallback element.
---@return T|nil lastElementOrFallback
function TooltipData:GetLastElement(t, fallback)
	local isTable = type(t) == "table"
	for i=#self.Data,1,-1 do
		local element = self.Data[i]
		if element and _ElementTypeMatch(element.Type, t, isTable) then
			return element
		end
	end
	if _IsTooltipElement(fallback) then
		self:AppendElement(fallback)
		return fallback
	end
	return nil
end

---@overload fun(self:TooltipData, t:TooltipElementType)
---@generic T:TooltipElement|TooltipElementType
---@param t `T`|`T`[] The tooltip element type, or an array of element types.
---@return T[] elements An array of elements, or an empty table.
function TooltipData:GetElements(t)
	local isTable = type(t) == "table"
	local elements = {}
	for i=1,#self.Data do
		local element = self.Data[i]
		if element and _ElementTypeMatch(element.Type, t, isTable) then
			elements[#elements+1] = element
		end
	end
	return elements
end

---Remove all elements matching the given tooltip element type(s).
---@param t TooltipElementType|TooltipElementType[] The tooltip element type, or an array of element types.
---@return boolean success Whether any elements matching the given types were removed.
function TooltipData:RemoveElements(t)
	local isTable = type(t) == "table"
	local success = false
	local j = 1
	local n = #self.Data
	--Alternative table.remove optimization
	--https://stackoverflow.com/a/53038524/2290477
	for i=1,n do
		if not _ElementTypeMatch(self.Data[i].Type, t, isTable) then
			if (i ~= j) then
				self.Data[j] = self.Data[i]
				self.Data[i] = nil
			end
			j = j + 1
		else
			self.Data[i] = nil
		end
	end
	return success
end

---Remove the provided tooltip element.
---@param ele TooltipElement
---@return boolean success Whether the element was removed.
function TooltipData:RemoveElement(ele)
	for i,element in pairs(self.Data) do
		if element == ele then
			table.remove(self.Data, i)
			return true
		end
	end
	return false
end

---Append a tooltip element to the end of the tooltip data.
---@param ele TooltipElement
---@return TooltipElement
function TooltipData:AppendElement(ele)
	if _IsTooltipElement(ele) then
		self.Data[#self.Data+1] = ele
	else
		_PrintError(string.format("[Game.Tooltip.TooltipData:AppendElement] Invalid tooltip element parameter: (%s)", _DumpExport(ele)))
	end
	return ele
end

---Append a table of elements to the end of the tooltip data.
---@param tbl TooltipElement[]
function TooltipData:AppendElements(tbl)
	for i=1,#tbl do
		local ele = tbl[i]
		if _IsTooltipElement(ele) then
			self.Data[#self.Data+1] = ele
		else
			_PrintError(string.format("[Game.Tooltip.TooltipData:AppendElements] Invalid tooltip element parameter: (%s)", _DumpExport(ele)))
		end
	end
end

---@param ele TooltipElement
---@param appendAfter TooltipElement
---@return TooltipElement
function TooltipData:AppendElementAfter(ele, appendAfter)
	if _IsTooltipElement(ele) then
		if _IsTooltipElement(appendAfter) then
			for i=1,#self.Data do
				local compareEle = self.Data[i]
				if compareEle == appendAfter then
					table.insert(self.Data, i+1, ele)
					return ele
				end
			end
		end
		self.Data[#self.Data+1] = ele
	else
		_PrintError(string.format("[Game.Tooltip.TooltipData:AppendElementAfter] Invalid tooltip element parameter: (%s)", _DumpExport(ele)))
	end
	return ele
end

---@param ele TooltipElement
---@param appendBefore TooltipElement
---@return TooltipElement
function TooltipData:AppendElementBefore(ele, appendBefore)
	if _IsTooltipElement(ele) then
		if _IsTooltipElement(appendBefore) then
			for i=1,#self.Data do
				local compareEle = self.Data[i]
				if compareEle == appendBefore then
					table.insert(self.Data, i-1, ele)
					return ele
				end
			end
		end
		self.Data[#self.Data+1] = ele
	else
		_PrintError(string.format("[Game.Tooltip.TooltipData:AppendElementBefore] Invalid tooltip element parameter: (%s)", _DumpExport(ele)))
	end
	return ele
end

---Append an element after a specific element type.
---@param ele TooltipElement
---@param elementType TooltipElementType|table<TooltipElementType,boolean> Either an TooltipElementType (string), or a table where the key is a TooltipElementType (i.e. `enableTypes = { SkillDescription = true, ItemDescription = true}`)
---@return TooltipElement
function TooltipData:AppendElementAfterType(ele, elementType)
	if _IsTooltipElement(ele) then
		local t = type(elementType)
		for i=1,#self.Data do
			local element = self.Data[i]
			if (t == "string" and element.Type == elementType) or (t == "table" and elementType[element.Type] == true) then
				table.insert(self.Data, i+1, ele)
				return ele
			end
		end
		self.Data[#self.Data+1] = ele
	else
		_PrintError(string.format("[Game.Tooltip.TooltipData:AppendElementAfterType] Invalid tooltip element parameter: (%s)", _DumpExport(ele)))
	end

	return ele
end

---Append an element before a specific element type.
---@param ele TooltipElement
---@param elementType TooltipElementType|table<TooltipElementType,boolean> Either an TooltipElementType (string), or a table where the key is a TooltipElementType (i.e. `enableTypes = { SkillDescription = true, ItemDescription = true}`)
---@return TooltipElement
function TooltipData:AppendElementBeforeType(ele, elementType)
	if _IsTooltipElement(ele) then
		local t = type(elementType)
		for i=1,#self.Data do
			local element = self.Data[i]
			if (t == "string" and element.Type == elementType) or (t == "table" and elementType[element.Type] == true) then
				table.insert(self.Data, i-1, ele)
				return ele
			end
		end
		self.Data[#self.Data+1] = ele
	else
		_PrintError(string.format("[Game.Tooltip.TooltipData:AppendElementAfterType] Invalid tooltip element parameter: (%s)", _DumpExport(ele)))
	end

	return ele
end

_TT.Register = {
	---@param callback fun(request:AnyTooltipRequest, tooltip:TooltipData, ...:any)
	Global = function(callback)
		_ttHooks:RegisterListener(nil, nil, callback)
	end,

	---@param callback fun(character:EclCharacter, ability:StatsAbilityType|string, tooltip:TooltipData)
	---@param ability StatsAbilityType|nil Optional ability to filter by.
	Ability = function(callback, ability)
		_ttHooks:RegisterListener("Ability", ability, callback)
	end,

	---@param callback fun(character:EclCharacter, statData:{ID:string}, tooltip:TooltipData)
	---@param id string|nil Optional CustomStat ID to filter by.
	CustomStat = function(callback, id)
		_ttHooks:RegisterListener("CustomStat", id, callback)
	end,
	
	---@param callback fun(tooltip:TooltipData)
	Generic = function(callback)
		_ttHooks:RegisterListener("Generic", nil, callback)
	end,

	---@param callback fun(item:EclItem, tooltip:TooltipData)
	---@param statsId string|nil Optional Rune StatsId to filter by.
	Item = function(callback, statsId)
		_ttHooks:RegisterListener("Item", statsId, callback)
	end,

	---Called when a tooltip is created when hovering over a player portrait.
	---@param callback fun(character:EclCharacter|nil, tooltip:TooltipData)
	PlayerPortrait = function(callback)
		_ttHooks:RegisterListener("PlayerPortrait", nil, callback)
	end,

	---@param callback fun(item:EclItem, tooltip:TooltipData)
	---@param statsId string|nil Optional Rune StatsId to filter by.
	Pyramid = function(callback, statsId)
		_ttHooks:RegisterListener("Pyramid", statsId, callback)
	end,

	---@param callback fun(item:EclItem, rune:StatEntryObject, slot:integer, tooltip:TooltipData)
	---@param statsId string|nil Optional Rune StatsId to filter by.
	Rune = function(callback, statsId)
		_ttHooks:RegisterListener("Rune", statsId, callback)
	end,

	---@param callback fun(character:EclCharacter, skill:string, tooltip:TooltipData)
	---@param skillId string|nil Optional Skill ID to filter by.
	Skill = function(callback, skillId)
		_ttHooks:RegisterListener("Skill", skillId, callback)
	end,

	---Register a callback for stat tooltips in the character sheet, such as attributes and resistances.
	---@param callback fun(character:EclCharacter, stat:StatsCharacterStatGetterType|string, tooltip:TooltipData)
	---@param id StatsCharacterStatGetterType|string|nil Optional Stat ID to filter by, such as "Damage".
	Stat = function(callback, id)
		_ttHooks:RegisterListener("Stat", id, callback)
	end,

	---@param callback fun(character:EclCharacter, status:EclStatus, tooltip:TooltipData)
	---@param statusId string|nil Optional Status ID to filter by.
	Status = function(callback, statusId)
		_ttHooks:RegisterListener("Status", statusId, callback)
	end,

	---Register a callback for when cloud and ground surface tooltip text is shown.
	---@param callback fun(character:EclCharacter, surface:string, tooltip:TooltipData)
	---@param surfaceId SurfaceType|nil Optional Surface ID to filter by.
	Surface = function(callback, surfaceId)
		_ttHooks:RegisterListener("Surface", surfaceId, callback)
	end,

	---@param callback fun(character:EclCharacter, tag:string, tooltip:TooltipData)
	---@param tag string|nil Optional Tag ID to filter by.
	Tag = function(callback, tag)
		_ttHooks:RegisterListener("Tag", tag, callback)
	end,

	---@param callback fun(character:EclCharacter, talent:StatsTalentType|string, tooltip:TooltipData)
	---@param talentId StatsTalentType|nil Optional Talent ID to filter by.
	Talent = function(callback, talentId)
		_ttHooks:RegisterListener("Talent", talentId, callback)
	end,

	---Called for both mouse-hovered items, and item names displayed when pressing "Show World Tooltips".
	---@param callback fun(item:EclItem|nil, tooltip:TooltipData)
	---@param statsId string|nil Optional item StatsId to filter by.
	World = function(callback, statsId)
		_ttHooks:RegisterListener("World", statsId, callback)
	end,
}

---Register a function to call when a tooltip occurs.
---Examples:
---_TT.RegisterListener("Skill", nil, myFunction) - Register a function for skill type tooltips.
---_TT.RegisterListener("Status", "HASTED", myFunction) - Register a function for a HASTED status tooltip.
---_TT.RegisterListener(myFunction) - Register a function for every kind of tooltip.
---@param tooltipTypeOrCallback TooltipRequestType|function The tooltip type, such as "Skill".
---@param idOrNil string|function|nil The tooltip ID, such as "Projectile_Fireball".
---@param callbackOrNil function|nil If the first two parameters are set, this is the function to invoke.
function _TT.RegisterListener(tooltipTypeOrCallback, idOrNil, callbackOrNil)
	if type(callbackOrNil) == "function" then
		--assert(type(tooltipTypeOrCallback) == "string", "If the third parameter is a function, the first parameter must be a string (TooltipType).")
		--assert(type(tooltipID) == "string", "If the third parameter is a function, the second parameter must be a string.")
		_ttHooks:RegisterListener(tooltipTypeOrCallback, idOrNil, callbackOrNil)
	elseif type(idOrNil) == "function" then
		assert(type(tooltipTypeOrCallback) == "string", "If the second parameter is a function, the first parameter must be a string (TooltipType).")
		_ttHooks:RegisterListener(tooltipTypeOrCallback, nil, idOrNil)
	elseif type(tooltipTypeOrCallback) == "function" then
		_ttHooks:RegisterListener(nil, nil, tooltipTypeOrCallback)
	else
		local t1 = type(tooltipTypeOrCallback)
		local t2 = type(idOrNil)
		local t3 = type(callbackOrNil)
		_PrintError(string.format("[_TT.RegisterListener] Invalid arguments - 1: [%s](%s), 2: [%s](%s), 3: [%s](%s)", tooltipTypeOrCallback, t1, idOrNil, t2, callbackOrNil, t3))
	end
end

---@param typeOrCallback string|GameTooltipRequestListener
---@param callbackOrNil GameTooltipRequestListener
---@param state string The function state, either "before" or "after".
function _TT.RegisterRequestListener(typeOrCallback, callbackOrNil, state)
	state = state or "after"
	local t = type(typeOrCallback)
	if t == "string" or typeOrCallback == nil then
		assert(type(callbackOrNil) == "function", "Second parameter must be a function.")
		_ttHooks:RegisterRequestListener(typeOrCallback, callbackOrNil, state)
	elseif t == "function" then
		_ttHooks:RegisterRequestListener(nil, typeOrCallback, state)
	end
end

---@param typeOrCallback string|GameTooltipBeforeNotifyListener Request type or the callback to register.
---@param callbackOrNil GameTooltipBeforeNotifyListener|nil The callback to register if the first parameter is a string.
function _TT.RegisterBeforeNotifyListener(typeOrCallback, callbackOrNil)
	local t = type(typeOrCallback)
	if t == "string" then
		assert(type(callbackOrNil) == "function", "Second parameter must be a function.")
		_ttHooks:RegisterBeforeNotifyListener(typeOrCallback, callbackOrNil)
	elseif t == "function" then
		_ttHooks:RegisterBeforeNotifyListener("All", typeOrCallback)
	end
end

---Check if the current tooltip request type matches the given type.
---@param t TooltipRequestType
---@return boolean
function _TT.RequestTypeEquals(t)
	if _ttHooks.ActiveType == t or (_ttHooks.NextRequest and _ttHooks.NextRequest.Type == t) then
		return true
	end
	return false
end

---Check if the last tooltip request type matches the given type.
---@param t TooltipRequestType
---@return boolean
function _TT.LastRequestTypeEquals(t)
	if _ttHooks.Last.Type == t or (_ttHooks.NextRequest and _ttHooks.NextRequest.Type == t) then
		return true
	end
	return false
end

---Get the current or last request table and type.
---@return AnyTooltipRequest request
---@return TooltipRequestType requestType
function _TT.GetCurrentOrLastRequest()
	if _ttHooks.NextRequest then
		return _ttHooks.NextRequest,_ttHooks.ActiveType
	end
	if _ttHooks.Last then
		return _ttHooks.Last.Request,_ttHooks.Last.Type
	end
	return nil,""
end

---Returns true if a tooltip is currently open.
---@return boolean
function _TT.IsOpen()
	return _ttHooks.IsOpen
end

local function CaptureBuiltInUIs()
	for i = 1,150 do
		local ui = _GetUIByType(i)
		if ui ~= nil then
			ui:CaptureExternalInterfaceCalls()
			ui:CaptureInvokes()
		end
	end
end

local function EnableHooks()
	RequestProcessor.ControllerEnabled = (_GetUIByPath("Public/Game/GUI/msgBox_c.swf") or _GetUIByType(_UITYPE.msgBox_c)) ~= nil

	if _ttHooks.InitializationRequested then
		_ttHooks:Init()
	end

	CaptureBuiltInUIs()
end

local _highPriority = {Priority = 999}

Ext.Events.GameStateChanged:Subscribe(function (e)
	if e.ToState == "Menu" then
		EnableHooks()
	end
end, _highPriority)

Ext.Events.SessionLoaded:Subscribe(function (e)
	_ttHooks.SessionLoaded = true
	EnableHooks()
end, _highPriority)

Ext.Events.UIObjectCreated:Subscribe(function (e)
	---@type UIObject
	local ui = e.UI
	ui:CaptureExternalInterfaceCalls()
	-- Has the 'no flash player' warning if the root is nil
	if ui:GetRoot() ~= nil then
		ui:CaptureInvokes()
	elseif _GetGameState() == "Running" then
		Ext.OnNextTick(function (e)
			CaptureBuiltInUIs()
		end)
	end
end, _highPriority)

--#region Annotations

---@alias TooltipElementType string|"AbilityBoost"|"AbilityDescription"|"AbilityTitle"|"AccuracyBoost"|"APCostBoost"|"APMaximumBoost"|"APRecoveryBoost"|"APStartBoost"|"ArmorBoost"|"ArmorSet"|"ArmorSlotType"|"ArmorValue"|"Blocking"|"CanBackstab"|"ChanceToHitBoost"|"ConsumableDamage"|"ConsumableDuration"|"ConsumableEffect"|"ConsumableEffectUknown"|"ConsumablePermanentDuration"|"ContainerIsLocked"|"CritChanceBoost"|"DamageBoost"|"DodgeBoost"|"Durability"|"EmptyRuneSlot"|"Engraving"|"EquipmentUnlockedSkill"|"Equipped"|"ExtraProperties"|"Flags"|"IsQuestItem"|"ItemAttackAPCost"|"ItemDescription"|"ItemGoldValue"|"ItemLevel"|"ItemName"|"ItemRarity"|"ItemRequirement"|"ItemUseAPCost"|"ItemWeight"|"MagicArmorValue"|"NeedsIdentifyLevel"|"OtherStatBoost"|"PickpocketInfo"|"PriceToIdentify"|"PriceToRepair"|"Reflection"|"ResistanceBoost"|"RuneEffect"|"RuneSlot"|"ShowSkillIcon"|"SkillAlreadyLearned"|"SkillAlreadyUsed"|"SkillAPCost"|"SkillbookSkill"|"SkillCanFork"|"SkillCanPierce"|"SkillCleansesStatus"|"SkillCooldown"|"SkillDamage"|"SkillDescription"|"SkillDuration"|"SkillExplodeRadius"|"SkillHealAmount"|"SkillIcon"|"SkillMPCost"|"SkillMultiStrikeAttacks"|"SkillName"|"SkillOnCooldown"|"SkillPathDistance"|"SkillPathSurface"|"SkillProjectileCount"|"SkillProperties"|"SkillRange"|"SkillRequiredEquipment"|"SkillSchool"|"SkillStrikeCount"|"SkillTier"|"SkillWallDistance"|"StatBoost"|"StatMEMSlot"|"StatName"|"StatsAPBase"|"StatsAPBonus"|"StatsAPDesc"|"StatsAPMalus"|"StatsAPTitle"|"StatsATKAPCost"|"StatsBaseValue"|"StatsCriticalInfos"|"StatsDescription"|"StatsDescriptionBoost"|"StatsGearBoostNormal"|"StatsPercentageBoost"|"StatsPercentageMalus"|"StatsPercentageTotal"|"StatsPointValue"|"StatsTalentsBoost"|"StatsTalentsMalus"|"StatsTotalDamage"|"StatSTRWeight"|"StatusBonus"|"StatusDescription"|"StatusImmunity"|"StatusMalus"|"TagDescription"|"Tags"|"TalentDescription"|"TalentTitle"|"VitalityBoost"|"WandCharges"|"WandSkill"|"WarningText"|"WeaponCritChance"|"WeaponCritMultiplier"|"WeaponDamage"|"WeaponDamagePenalty"|"WeaponRange"|
---@alias TooltipRequestType "Ability"|"CustomStat"|"Generic"|"Item"|"PlayerPortrait"|"Pyramid"|"Rune"|"Skill"|"Stat"|"Status"|"Surface"|"Tag"|"Talent"|"World"
---@alias AnyTooltipRequest TooltipItemRequest|TooltipRuneRequest|TooltipSkillRequest|TooltipStatusRequest|TooltipAbilityRequest|TooltipTalentRequest|TooltipStatRequest|TooltipSurfaceRequest|TooltipPyramidRequest|TooltipTagRequest|TooltipCustomStatRequest|TooltipGenericRequest|TooltipWorldRequest

---@class TooltipElement
---@field Type TooltipElementType

---@class TooltipLabelElement
---@field Label string

---@class TooltipLabelDamageElement
---@field Label string
---@field DamageType integer
---@field MinDamage integer
---@field MaxDamage integer

---@class TooltipLabelNumValueElement
---@field Label string
---@field Value number

---@class TooltipLabelStringValueElement
---@field Label string
---@field Value string

---The tooltip element for Generic, PlayerPortrait, and World tooltips.  
---The Type is just `"GenericDescription"`.
---@class GenericDescription:TooltipLabelElement
---@field Type "GenericDescription"
---@field X number
---@field Y number
---@field Width number
---@field Height number
---@field OverrideSize boolean|nil If true, the resulting tooltip will be adjusted by the Width/Height.
---@field AllowDelay boolean|nil
---@field AnchorEnum integer|nil
---@field BackgroundType integer|nil

---The tooltip element for Surface tooltips.
---@class SurfaceDescription:TooltipLabelElement

---@class BoostSpec:TooltipElement
---@field Value number

---@class TooltipSkillPropertiesProperty
---@field Label string
---@field Warning string

---@class TooltipSkillPropertiesResistance
---@field Label string
---@field Value number

---@class SkillProperties:TooltipElement
---@field Properties TooltipSkillPropertiesProperty[]
---@field Resistances TooltipSkillPropertiesResistance[]

---@class ItemName:TooltipLabelElement
---@class ItemWeight:TooltipLabelElement
---@class ItemGoldValue:TooltipLabelElement
---@class ItemLevel:TooltipLabelNumValueElement
---@class ItemDescription:TooltipLabelElement
---@class ItemRarity:TooltipLabelElement

---@class ItemUseAPCost:TooltipLabelNumValueElement
---@field RequirementMet boolean

---@class ItemAttackAPCost:TooltipLabelNumValueElement
---@field Warning string
---@field RequirementMet boolean

---@class StatBoost:BoostSpec
---@class ResistanceBoost:BoostSpec
---@class AbilityBoost:BoostSpec
---@class OtherStatBoost:TooltipLabelStringValueElement
---@class VitalityBoost:BoostSpec
---@class ChanceToHitBoost:BoostSpec
---@class DamageBoost:BoostSpec
---@class APCostBoost:BoostSpec
---@class APMaximumBoost:BoostSpec
---@class APStartBoost:BoostSpec
---@class APRecoveryBoost:BoostSpec
---@class CritChanceBoost:BoostSpec
---@class ArmorBoost:BoostSpec
---@class ConsumableDuration:TooltipLabelStringValueElement
---@class ConsumablePermanentDuration:TooltipLabelStringValueElement
---@class ConsumableEffect:TooltipLabelStringValueElement

---@class ConsumableDamage:TooltipLabelDamageElement

---@class ExtraProperties:TooltipLabelElement
---@class Flags:TooltipLabelElement

---@class ItemRequirement:TooltipLabelElement
---@field RequirementMet boolean

---@class WeaponDamage:TooltipLabelDamageElement

---@class WeaponDamagePenalty:TooltipLabelElement
---@class WeaponCritMultiplier:TooltipLabelStringValueElement
---@class WeaponCritChance:TooltipLabelStringValueElement
---@class WeaponRange:TooltipLabelStringValueElement

---@class Durability:TooltipLabelNumValueElement
---@field Max number

---@class CanBackstab:TooltipLabelElement
---@class AccuracyBoost:TooltipLabelNumValueElement
---@class DodgeBoost:TooltipLabelNumValueElement

---@class EquipmentUnlockedSkill:TooltipLabelStringValueElement
---@field Icon number

---@class WandSkill:TooltipLabelStringValueElement
---@field Icon string
---@field Warning string

---@class WandCharges:TooltipLabelNumValueElement
---@field MaxValue number

---@class ArmorValue:TooltipLabelNumValueElement
---@class ArmorSlotType:TooltipLabelElement
---@class Blocking:TooltipLabelNumValueElement
---@class NeedsIdentifyLevel:TooltipLabelElement
---@class IsQuestItem:TooltipElement
---@class PriceToIdentify:TooltipLabelStringValueElement
---@class PriceToRepair:TooltipLabelStringValueElement
---@class PickpocketInfo:TooltipLabelElement
---@class Engraving:TooltipLabelElement
---@class ContainerIsLocked:TooltipLabelElement

---@class Tags:TooltipLabelStringValueElement
---@field Warning string

---@class SkillName:TooltipLabelElement
---@class SkillIcon:TooltipLabelElement

---@class SkillSchool:TooltipLabelElement
---@field Icon number

---@class SkillTier:TooltipLabelElement

---@class SkillRequiredEquipment:TooltipLabelElement
---@field RequirementMet boolean

---@class SkillAPCost:TooltipLabelNumValueElement
---@field Warning string
---@field RequirementMet boolean

---@class SkillCooldown:TooltipLabelNumValueElement
---@field Warning string
---@field ValueText string

---@class SkillDescription:TooltipLabelElement
---@class SkillDamage:TooltipLabelDamageElement
---@class SkillRange:TooltipLabelStringValueElement
---@class SkillExplodeRadius:TooltipLabelStringValueElement
---@class SkillCanPierce:TooltipLabelStringValueElement
---@class SkillCanFork:TooltipLabelStringValueElement
---@class SkillStrikeCount:TooltipLabelStringValueElement
---@class SkillProjectileCount:TooltipLabelStringValueElement
---@class SkillCleansesStatus:TooltipLabelStringValueElement
---@class SkillMultiStrikeAttacks:TooltipLabelStringValueElement
---@class SkillWallDistance:TooltipLabelStringValueElement
---@class SkillPathSurface:TooltipLabelStringValueElement
---@class SkillPathDistance:TooltipLabelStringValueElement
---@class SkillHealAmount:TooltipLabelStringValueElement

---@class SkillDuration:TooltipLabelNumValueElement
---@field Warning string

---@class ConsumableEffectUknown:TooltipLabelElement
---@class Reflection:TooltipLabelElement
---@class SkillAlreadyLearned:TooltipLabelElement
---@class SkillOnCooldown:TooltipLabelElement
---@class SkillAlreadyUsed:TooltipLabelElement
---@class AbilityTitle:TooltipLabelElement

---@class AbilityDescription:TooltipElement
---@field AbilityId number
---@field Description string
---@field Description2 string
---@field CurrentLevelEffect string
---@field NextLevelEffect string

---@class TalentTitle:TooltipLabelElement

---@class TalentDescription:TooltipElement
---@field TalentId number
---@field Description string
---@field Requirement string
---@field IncompatibleWith string
---@field Selectable boolean
---@field Unknown boolean

---@class SkillMPCost:TooltipLabelNumValueElement
---@field RequirementMet boolean

---@class MagicArmorValue:TooltipLabelNumValueElement
---@field RequirementMet boolean

---@class WarningText:TooltipLabelElement
---@class RuneSlot:TooltipLabelStringValueElement

---@class RuneEffect:TooltipElement
---@field Unknown1 number
---@field Rune1 string
---@field Rune2 string
---@field Rune3 string
---@field Label string
---@field Label2 string

---@class Equipped:TooltipLabelElement
---@field EquippedBy string
---@field Slot string

---@class ShowSkillIcon:TooltipElement
---@class SkillbookSkill:TooltipLabelStringValueElement
---@field Icon number

---@class EmptyRuneSlot:TooltipLabelStringValueElement
---@class StatName:TooltipLabelElement
---@class StatsDescription:TooltipLabelElement
---@class StatsDescriptionBoost:TooltipLabelNumValueElement
---@class StatSTRWeight:TooltipLabelElement
---@class StatMEMSlot:TooltipLabelElement
---@class StatsPointValue:TooltipLabelElement
---@class StatsTalentsBoost:TooltipLabelElement
---@class StatsTalentsMalus:TooltipLabelElement
---@class StatsBaseValue:TooltipLabelElement
---@class StatsPercentageBoost:TooltipLabelElement
---@class StatsPercentageMalus:TooltipLabelElement
---@class StatsPercentageTotal:TooltipLabelNumValueElement
---@class StatsGearBoostNormal:TooltipLabelElement
---@class StatsATKAPCost:TooltipLabelElement
---@class StatsCriticalInfos:TooltipLabelElement
---@class StatsAPTitle:TooltipLabelElement
---@class StatsAPDesc:TooltipLabelElement
---@class StatsAPBase:TooltipLabelElement
---@class StatsAPBonus:TooltipLabelElement
---@class StatsAPMalus:TooltipLabelElement
---@class StatsTotalDamage:TooltipLabelElement

---@class TagDescription:TooltipLabelElement
---@field Image number

---@class StatusImmunity:TooltipLabelElement
---@class StatusBonus:TooltipLabelElement
---@class StatusMalus:TooltipLabelElement
---@class StatusDescription:TooltipLabelElement

---@alias AnyTooltipDescriptionElement AbilityDescription|GenericDescription|ItemDescription|SkillDescription|StatsDescription|StatusDescription|SurfaceDescription|TagDescription|TalentDescription

---@alias GameTooltipRequestListener fun(request:AnyTooltipRequest, ui:UIObject, uiType:integer, event:string, id:string|number|boolean|nil, ...:string|number|boolean|nil)

---@alias GameTooltipBeforeNotifyListener fun(request:AnyTooltipRequest, ui:UIObject, method:string, tooltip:TooltipData)

--#endregion