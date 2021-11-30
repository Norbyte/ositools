local _I = Ext._Internal

_I._PublishedEvents = {
	"GameStateChanged",
	"GetSkillDamage",
	"GetSkillAPCost",
	"ComputeCharacterHit",
	"CalculateTurnOrder", -- FIXME
	"GetHitChance", -- FIXME
	"StatusGetEnterChance",
	"StatusHitEnter",
	"BeforeCharacterApplyDamage",
	"TreasureItemGenerated",
	"BeforeCraftingExecuteCombination",
	"AfterCraftingExecuteCombination",
	"BeforeShootProjectile",
	"ShootProjectile",
	"ProjectileHit",
	"GroundHit",
	"OnExecutePropertyDataOnTarget",
	"OnExecutePropertyDataOnPosition"
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
		propType:ExecuteOnTarget(e.Property, e.Attacker, e.Target, e.ImpactOrigin, e.IsFromItem, e.Skill, e.Hit)
	end
end)

Ext.Events.OnExecutePropertyDataOnPosition:Subscribe(function (e)
	local prop = e.Property
	local propType = Ext._SkillPropertyTypes[prop.Action]
	if propType ~= nil and propType.ExecuteOnPosition ~= nil then
		propType:ExecuteOnPosition(e.Property, e.Attacker, e.Position, e.AreaRadius, e.IsFromItem, e.Skill, e.Hit)
	end
end)
