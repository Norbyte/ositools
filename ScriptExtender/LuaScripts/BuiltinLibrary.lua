local _G = _G

Ext._Internal = {}
local _I = Ext._Internal

Ext._LoadedFiles = {}
-- Table to hold debugger expression evaluation results
Ext._EVAL_ROOTS_ = {}
Mods = {}

_I._PublishedSharedEvents = {
	"ModuleLoadStarted",
	"ModuleLoading",
	"StatsLoaded",
	"ModuleResume",
	"SessionLoading",
	"SessionLoaded",
	"GameStateChanged",
	"ResetCompleted",
	"DoConsoleCommand",
	"Tick",
	"StatsStructureLoaded",
	"GetSkillDamage",
	"GetSkillAPCost",
	"GetAttackAPCost",
	"GetHitChance",
	"GetCharacterWeaponAnimationSetType",
}

_I._WarningShown = {}
_I._DeprecationURLShown = false

Ext._WarnDeprecated56 = function (msg, type)
	type = type or msg
	if _I._WarningShown[type] == nil then
		Ext.PrintWarning(msg)
		if not _I._DeprecationURLShown then
			Ext.PrintWarning("See https://github.com/Norbyte/ositools/blob/master/Docs/ReleaseNotesv56.md for more info.")
			_I._DeprecationURLShown = true
		end
		_I._WarningShown[type] = true
	end
end

Ext.Require = function (mod, path)
	if ModuleUUID == nil then
		Ext.PrintWarning("Calling Ext.Require() after the module was loaded is deprecated!");
	end

	local fullName
	if path == nil then
		fullName = ModuleUUID .. "/" .. mod
	else
		fullName = mod .. "/" .. path
	end

	if Ext._LoadedFiles[fullName] ~= nil then
		return Ext._LoadedFiles[fullName]
	end
	
	local env
	-- LuaJIT workaround
	if getfenv ~= nil then
		env = getfenv(2)
	end

	local loaded
	if path == nil then
		loaded = {Ext.Utils.Include(ModuleUUID, mod, env)}
	else
		loaded = {Ext.Utils.Include(mod, path, env)}
	end

	Ext._LoadedFiles[fullName] = loaded
	return table.unpack(loaded)
end

Ext._LoadBootstrap = function (path, modTable)
	local env = {
		-- Put frequently used items directly into the table for faster access
		type = type,
		tostring = tostring,
		tonumber = tonumber,
		pairs = pairs,
		ipairs = ipairs,
		print = print,
		error = error,
		next = next,

		string = string,
		math = math,
		table = table,

		Ext = Ext,
		Osi = Osi,
		Game = Game,
		Sandboxed = true,

		ModuleUUID = ModuleUUID
	}
	-- The rest are accessed via __index
	setmetatable(env, {__index = _G})
	Mods[modTable] = env
	
	env._G = env
	Ext.Utils.Include(ModuleUUID, path, env)
end

-- Used by the Lua debug adapter to store intermediate evaluation results.
-- Should not be used manually!
Ext.DebugEvaluate = function (retval)
	if type(retval) ~= "table" then
		return retval
	else
		local idx = #Ext._EVAL_ROOTS_ + 1
		Ext._EVAL_ROOTS_[idx] = retval
		return retval, idx
	end
end

-- Custom skill property registration
Ext._SkillPropertyTypes = {}

Ext.RegisterSkillProperty = function (name, proto)
	Ext._SkillPropertyTypes[name] = proto
end

-- Helper for dumping variables in console
Ext.DumpExport = function (val)
	local opts = {
		Beautify = true,
		StringifyInternalTypes = true,
		IterateUserdata = true,
		AvoidRecursion = true
	}
	return Ext.Json.Stringify(val, opts)
end

Ext.Dump = function (val)
	Ext.Utils.Print(Ext.DumpExport(val))
end

Ext.DumpShallow = function (val)
	local opts = {
		Beautify = true,
		StringifyInternalTypes = true,
		IterateUserdata = true,
		AvoidRecursion = true,
		LimitDepth = 1,
		LimitArrayElements = 3
	}
	Ext.Utils.Print(Ext.Json.Stringify(val, opts))
end

-- Global helper aliases for Ext.Dump, Ext.Print
_D = Ext.Dump
_DS = Ext.DumpShallow
_P = Ext.Utils.Print

function CallDeprecated(fun, oldName, newName)
	local shownWarning = false
	return function (...)
		if shownWarning == false then
			Ext._WarnDeprecated56("Calling Ext." .. oldName .. " is deprecated; use Ext." .. newName .. " instead.")
			shownWarning = true
		end

		return fun(...)
	end
end

Ext.EnableExperimentalPropertyWrites = function ()
	Ext.PrintWarning("EnableExperimentalPropertyWrites() is no longer needed in v53!")
end

Ext.OnNextTick = function (fun)
	Ext.Events.Tick:Subscribe(fun, {Once = true})
end

-- Backwards compatibility with old variable APIs
Ext.Utils.RegisterUserVariable = CallDeprecated(Ext.Vars.RegisterUserVariable, "Utils.RegisterUserVariable", "Vars.RegisterUserVariable")
Ext.Utils.SyncUserVariables = CallDeprecated(Ext.Vars.SyncUserVariables, "Utils.SyncUserVariables", "Vars.SyncUserVariables")
Ext.Utils.DirtyUserVariables = CallDeprecated(Ext.Vars.DirtyUserVariables, "Utils.DirtyUserVariables", "Vars.DirtyUserVariables")
Ext.Utils.RegisterModVariable = CallDeprecated(Ext.Vars.RegisterModVariable, "Utils.RegisterModVariable", "Vars.RegisterModVariable")
Ext.Utils.GetModVariables = CallDeprecated(Ext.Vars.GetModVariables, "Utils.GetModVariables", "Vars.GetModVariables")
Ext.Utils.SyncModVariables = CallDeprecated(Ext.Vars.SyncModVariables, "Utils.SyncModVariables", "Vars.SyncModVariables")
Ext.Utils.DirtyModVariables = CallDeprecated(Ext.Vars.DirtyModVariables, "Utils.DirtyModVariables", "Vars.DirtyModVariables")

-- Backwards compatibility with old JSON APIs
Ext.JsonStringify = CallDeprecated(Ext.Json.Stringify, "JsonStringify", "Json.Stringify")
Ext.JsonParse = CallDeprecated(Ext.Json.Parse, "JsonParse", "Json.Parse")

-- Backwards compatibility with old debug APIs
Ext.DebugBreak = CallDeprecated(Ext.Debug.DebugBreak, "DebugBreak", "Debug.DebugBreak")
Ext.IsDeveloperMode = CallDeprecated(Ext.Debug.IsDeveloperMode, "IsDeveloperMode", "Debug.IsDeveloperMode")

-- Backwards compatibility with old utility APIs
Ext.Print = Ext.Utils.Print
Ext.PrintWarning = Ext.Utils.PrintWarning
Ext.PrintError = Ext.Utils.PrintError
Ext.GetGameMode = Ext.Utils.GetGameMode
Ext.GetDifficulty = Ext.Utils.GetDifficulty
Ext.Random = Ext.Utils.Random
Ext.Round = Ext.Utils.Round

Ext.MonotonicTime = CallDeprecated(Ext.Utils.MonotonicTime, "MonotonicTime", "Utils.MonotonicTime")
Ext.Version = CallDeprecated(Ext.Utils.Version, "Version", "Utils.Version")
Ext.GameVersion = CallDeprecated(Ext.Utils.GameVersion, "GameVersion", "Utils.GameVersion")
Ext.GetHandleType = CallDeprecated(Ext.Utils.GetHandleType, "GetHandleType", "Utils.GetHandleType")
Ext.ShowErrorAndExitGame = CallDeprecated(Ext.Utils.ShowErrorAndExitGame, "ShowErrorAndExitGame", "Utils.ShowErrorAndExitGame")

-- Backwards compatibility with old IO APIs
Ext.LoadFile = CallDeprecated(Ext.IO.LoadFile, "LoadFile", "IO.LoadFile")
Ext.SaveFile = CallDeprecated(Ext.IO.SaveFile, "SaveFile", "IO.SaveFile")
Ext.AddPathOverride = CallDeprecated(Ext.IO.AddPathOverride, "AddPathOverride", "IO.AddPathOverride")
Ext.GetPathOverride = CallDeprecated(Ext.IO.GetPathOverride, "GetPathOverride", "IO.GetPathOverride")

-- Backwards compatibility with old Mod APIs
Ext.IsModLoaded = CallDeprecated(Ext.Mod.IsModLoaded, "IsModLoaded", "Mod.IsModLoaded")
Ext.GetModLoadOrder = CallDeprecated(Ext.Mod.GetLoadOrder, "GetModLoadOrder", "Mod.GetLoadOrder")
Ext.GetModInfo = CallDeprecated(Ext.Mod.GetModInfo, "GetModInfo", "Mod.GetModInfo")

-- Backwards compatibility with old localization APIs
Ext.GetTranslatedString = CallDeprecated(Ext.L10N.GetTranslatedString, "GetTranslatedString", "L10N.GetTranslatedString")
Ext.GetTranslatedStringFromKey = CallDeprecated(Ext.L10N.GetTranslatedStringFromKey, "GetTranslatedStringFromKey", "L10N.GetTranslatedStringFromKey")
Ext.CreateTranslatedString = CallDeprecated(Ext.L10N.CreateTranslatedString, "CreateTranslatedString", "L10N.CreateTranslatedString")
Ext.CreateTranslatedStringKey = CallDeprecated(Ext.L10N.CreateTranslatedStringKey, "CreateTranslatedStringKey", "L10N.CreateTranslatedStringKey")
Ext.CreateTranslatedStringHandle = CallDeprecated(Ext.L10N.CreateTranslatedStringHandle, "CreateTranslatedStringHandle", "L10N.CreateTranslatedStringHandle")

-- Backwards compatibility with old stat APIs
Ext.GetStatEntries = CallDeprecated(Ext.Stats.GetStats, "GetStatEntries", "Stats.GetStats")
Ext.GetStatEntriesLoadedBefore = CallDeprecated(Ext.Stats.GetStatsLoadedBefore, "GetStatEntriesLoadedBefore", "Stats.GetStatsLoadedBefore")
Ext.GetStat = CallDeprecated(Ext.Stats.Get, "GetStat", "Stats.Get")
Ext.CreateStat = CallDeprecated(Ext.Stats.Create, "CreateStat", "Stats.Create")
Ext.SyncStat = CallDeprecated(Ext.Stats.Sync, "SyncStat", "Stats.Sync")
Ext.StatSetPersistence = CallDeprecated(Ext.Stats.SetPersistence, "StatSetPersistence", "Stats.SetPersistence")
Ext.StatGetAttribute = Ext.Stats.GetAttribute
Ext.StatSetAttribute = Ext.Stats.SetAttribute
Ext.StatAddCustomDescription = CallDeprecated(Ext.Stats.AddCustomDescription, "StatAddCustomDescription", "Stats.AddCustomDescription")
Ext.StatSetLevelScaling = CallDeprecated(Ext.Stats.SetLevelScaling, "StatSetLevelScaling", "Stats.SetLevelScaling")
Ext.EnumIndexToLabel = CallDeprecated(Ext.Stats.EnumIndexToLabel, "EnumIndexToLabel", "Stats.EnumIndexToLabel")
Ext.EnumLabelToIndex = CallDeprecated(Ext.Stats.EnumLabelToIndex, "EnumLabelToIndex", "Stats.EnumLabelToIndex")
Ext.NewDamageList = CallDeprecated(Ext.Stats.NewDamageList, "NewDamageList", "Stats.NewDamageList")
Ext.AddVoiceMetaData = CallDeprecated(Ext.Stats.AddVoiceMetaData, "AddVoiceMetaData", "Stats.AddVoiceMetaData")

Ext.GetDeltaMod = CallDeprecated(Ext.Stats.DeltaMod.GetLegacy, "GetDeltaMod", "Stats.DeltaMod.GetLegacy")
Ext.UpdateDeltaMod = CallDeprecated(Ext.Stats.DeltaMod.Update, "UpdateDeltaMod", "Stats.DeltaMod.Update")
Ext.GetSkillSet = CallDeprecated(Ext.Stats.SkillSet.GetLegacy, "GetSkillSet", "Stats.SkillSet.GetLegacy")
Ext.UpdateSkillSet = CallDeprecated(Ext.Stats.SkillSet.Update, "UpdateSkillSet", "Stats.SkillSet.Update")
Ext.GetEquipmentSet = CallDeprecated(Ext.Stats.EquipmentSet.GetLegacy, "GetEquipmentSet", "Stats.EquipmentSet.GetLegacy")
Ext.UpdateEquipmentSet = CallDeprecated(Ext.Stats.EquipmentSet.Update, "UpdateEquipmentSet", "Stats.EquipmentSet.Update")
Ext.GetTreasureTable = CallDeprecated(Ext.Stats.TreasureTable.GetLegacy, "GetTreasureTable", "Stats.TreasureTable.GetLegacy")
Ext.UpdateTreasureTable = CallDeprecated(Ext.Stats.TreasureTable.Update, "UpdateTreasureTable", "Stats.TreasureTable.Update")
Ext.GetTreasureCategory = CallDeprecated(Ext.Stats.TreasureCategory.GetLegacy, "GetTreasureCategory", "Stats.TreasureCategory.GetLegacy")
Ext.UpdateTreasureCategory = CallDeprecated(Ext.Stats.TreasureCategory.Update, "UpdateTreasureCategory", "Stats.TreasureCategory.Update")
Ext.GetItemCombo = CallDeprecated(Ext.Stats.ItemCombo.GetLegacy, "GetItemCombo", "Stats.ItemCombo.GetLegacy")
Ext.UpdateItemCombo = CallDeprecated(Ext.Stats.ItemCombo.Update, "UpdateItemCombo", "Stats.ItemCombo.Update")
Ext.GetItemComboPreviewData = CallDeprecated(Ext.Stats.ItemComboPreview.GetLegacy, "GetItemComboPreviewData", "Stats.ItemComboPreview.GetLegacy")
Ext.UpdateItemComboPreviewData = CallDeprecated(Ext.Stats.ItemComboPreview.Update, "UpdateItemComboPreviewData", "Stats.ItemComboPreview.Update")
Ext.GetItemComboProperty = CallDeprecated(Ext.Stats.ItemComboPreview.GetLegacy, "GetItemComboProperty", "Stats.ItemComboPreview.GetLegacy")
Ext.UpdateItemComboProperty = CallDeprecated(Ext.Stats.ItemComboPreview.Update, "UpdateItemComboProperty", "Stats.ItemComboPreview.Update")
Ext.GetItemGroup = CallDeprecated(Ext.Stats.ItemGroup.GetLegacy, "GetItemGroup", "Stats.ItemGroup.GetLegacy")
Ext.GetNameGroup = CallDeprecated(Ext.Stats.NameGroup.Update, "GetNameGroup", "Stats.NameGroup.Update")

-- Backwards compatibility with old surface APIs
Ext.GetSurfaceTransformRules = CallDeprecated(Ext.Surface.GetTransformRules, "GetSurfaceTransformRules", "Surface.GetTransformRules")
Ext.UpdateSurfaceTransformRules = CallDeprecated(Ext.Surface.UpdateTransformRules, "UpdateSurfaceTransformRules", "Surface.UpdateTransformRules")
Ext.GetSurfaceTemplate = CallDeprecated(Ext.Surface.GetTemplate, "GetSurfaceTemplate", "Surface.GetTemplate")


-- New getmetatable() to support fetching type information for proxy objects
local base_getmetatable = getmetatable
getmetatable = function (obj)
	return Ext.Types.GetObjectType(obj) or base_getmetatable(obj)
end
