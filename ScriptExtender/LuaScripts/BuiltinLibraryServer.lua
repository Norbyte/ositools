local _I = Ext._Internal

_I._PublishedEvents = {
	"ComputeCharacterHit",
	"CalculateTurnOrder",
	"StatusGetEnterChance",
	"BeforeStatusApply",
	"StatusHitEnter",
	"BeforeStatusDelete",
	"StatusDelete",
	"BeforeCharacterApplyDamage",
	"TreasureItemGenerated",
	"BeforeCraftingExecuteCombination",
	"AfterCraftingExecuteCombination",
	"BeforeShootProjectile",
	"ShootProjectile",
	"ProjectileHit",
	"GroundHit",
	"OnExecutePropertyDataOnTarget",
	"OnExecutePropertyDataOnPosition",
	"OnBeforeSortAiActions",
	"OnAfterSortAiActions",
	"OnPeekAiAction"
}

Ext.IsClient = function ()
	return false
end

Ext.IsServer = function ()
	return true
end

Ext._GetModPersistentVars = function (modTable)
	local tab = Mods[modTable]
	if tab ~= nil then
		local persistent = tab.PersistentVars
		if persistent ~= nil then
			return Ext.Json.Stringify(persistent)
		end
	end
end

Ext._RestoreModPersistentVars = function (modTable, vars)
	local tab = Mods[modTable]
	if tab ~= nil then
		tab.PersistentVars = Ext.Json.Parse(vars)
	end
end

_I._RegisterEvents()

Ext.Events.OnExecutePropertyDataOnTarget:Subscribe(function (e)
	local prop = e.Property
	local propType = Ext._SkillPropertyTypes[prop.Action]
	if propType ~= nil and propType.ExecuteOnTarget ~= nil then
		propType.ExecuteOnTarget(e.Property, e.Attacker, e.Target, e.ImpactOrigin, e.IsFromItem, e.Skill, e.Hit)
	end
end)

Ext.Events.OnExecutePropertyDataOnPosition:Subscribe(function (e)
	local prop = e.Property
	local propType = Ext._SkillPropertyTypes[prop.Action]
	if propType ~= nil and propType.ExecuteOnPosition ~= nil then
		propType.ExecuteOnPosition(e.Property, e.Attacker, e.Position, e.AreaRadius, e.IsFromItem, e.Skill, e.Hit)
	end
end)

_C = function ()
	return Ext.Entity.GetCharacter(CharacterGetHostCharacter())
end

_W = function ()
	return Ext.Entity.GetItem(CharacterGetEquippedWeapon(CharacterGetHostCharacter()))
end

-- Backwards compatibility with old Osiris APIs
Ext.RegisterOsirisListener = CallDeprecated(Ext.Osiris.RegisterListener, "RegisterOsirisListener", "Osiris.RegisterListener")
Ext.OsirisIsCallable = CallDeprecated(Ext.Osiris.IsCallable, "OsirisIsCallable", "Osiris.IsCallable")
Ext.NewCall = CallDeprecated(Ext.Osiris.NewCall, "NewCall", "Osiris.NewCall")
Ext.NewQuery = CallDeprecated(Ext.Osiris.NewQuery, "NewQuery", "Osiris.NewQuery")
Ext.NewEvent = CallDeprecated(Ext.Osiris.NewEvent, "NewEvent", "Osiris.NewEvent")

-- Backwards compatibility with old Net APIs
Ext.BroadcastMessage = CallDeprecated(Ext.Net.BroadcastMessage, "BroadcastMessage", "Net.BroadcastMessage")
Ext.PostMessageToClient = CallDeprecated(Ext.Net.PostMessageToClient, "PostMessageToClient", "Net.PostMessageToClient")
Ext.PostMessageToUser = CallDeprecated(Ext.Net.PostMessageToUser, "PostMessageToUser", "Net.PostMessageToUser")
Ext.PlayerHasExtender = CallDeprecated(Ext.Net.PlayerHasExtender, "PlayerHasExtender", "Net.PlayerHasExtender")

-- Backwards compatibility with old Entity APIs
Ext.GetAllCharacters = CallDeprecated(Ext.Entity.GetAllCharacterGuids, "GetAllCharacters", "Entity.GetAllCharacterGuids")
Ext.GetCharactersAroundPosition = CallDeprecated(Ext.Entity.GetCharacterGuidsAroundPosition, "GetCharactersAroundPosition", "Entity.GetCharacterGuidsAroundPosition")
Ext.GetAllItems = CallDeprecated(Ext.Entity.GetAllItemGuids, "GetAllItems", "Entity.GetAllItemGuids")
Ext.GetItemsAroundPosition = CallDeprecated(Ext.Entity.GetItemGuidsAroundPosition, "GetItemsAroundPosition", "Entity.GetItemGuidsAroundPosition")
Ext.GetAllTriggers = CallDeprecated(Ext.Entity.GetAllTriggerGuids, "GetAllTriggers", "Entity.GetAllTriggerGuids")
-- NOTE: we call GetCharacterLegacy internally since they have different lifetime behavior
-- for compatibility with pre-v56 lifetimes
Ext.GetCharacter = CallDeprecated(Ext.Entity.GetCharacterLegacy, "GetCharacter", "Entity.GetCharacter")
Ext.GetItem = CallDeprecated(Ext.Entity.GetItem, "GetItem", "Entity.GetItem")
Ext.GetTrigger = CallDeprecated(Ext.Entity.GetTrigger, "GetTrigger", "Entity.GetTrigger")
Ext.GetGameObject = CallDeprecated(Ext.Entity.GetGameObject, "GetGameObject", "Entity.GetGameObject")
Ext.GetStatus = CallDeprecated(Ext.Entity.GetStatus, "GetStatus", "Entity.GetStatus")
Ext.GetCombat = CallDeprecated(Ext.Entity.GetCombat, "GetCombat", "Entity.GetCombat")
Ext.GetSurface = CallDeprecated(Ext.Entity.GetSurface, "GetSurface", "Entity.GetSurface")
Ext.GetAiGrid = CallDeprecated(Ext.Entity.GetAiGrid, "GetAiGrid", "Entity.GetAiGrid")
Ext.GetCurrentLevelData = CallDeprecated(Ext.Entity.GetCurrentLevelData, "GetCurrentLevelData", "Entity.GetCurrentLevelData")

-- Backwards compatibility with old custom stat APIs
Ext.CreateCustomStat = CallDeprecated(Ext.CustomStat.Create, "CreateCustomStat", "CustomStat.Create")
Ext.GetCustomStatById = CallDeprecated(Ext.CustomStat.GetById, "GetCustomStatById", "CustomStat.GetById")
Ext.GetCustomStatByName = CallDeprecated(Ext.CustomStat.GetByName, "GetCustomStatByName", "CustomStat.GetByName")
Ext.GetAllCustomStats = CallDeprecated(Ext.CustomStat.GetAll, "GetAllCustomStats", "CustomStat.GetAll")

-- Backwards compatibility with old surface APIs
Ext.CreateSurfaceAction = CallDeprecated(Ext.Surface.Action.Create, "CreateSurfaceAction", "Surface.Action.Create")
Ext.ExecuteSurfaceAction = CallDeprecated(Ext.Surface.Action.Execute, "ExecuteSurfaceAction", "Surface.Action.Execute")
Ext.CancelSurfaceAction = CallDeprecated(Ext.Surface.Action.Cancel, "CancelSurfaceAction", "Surface.Action.Cancel")

-- Backwards compatibility with old server APIs
Ext.GetGameState = CallDeprecated(Ext.Server.GetGameState, "GetGameState", "Server.GetGameState")

-- Backwards compatibility with old property list APIs
Ext.ExecuteSkillPropertiesOnTarget = CallDeprecated(Ext.PropertyList.ExecuteSkillPropertiesOnTarget, "ExecuteSkillPropertiesOnTarget", "PropertyList.ExecuteSkillPropertiesOnTarget")
Ext.ExecuteSkillPropertiesOnPosition = CallDeprecated(Ext.PropertyList.ExecuteSkillPropertiesOnPosition, "ExecuteSkillPropertiesOnPosition", "PropertyList.ExecuteSkillPropertiesOnPosition")
