local _I = Ext._Internal

_I._PublishedEvents = {
	"GameStateChanged",
	"GetSkillDamage",
	"GetSkillAPCost",
	"ComputeCharacterHit",
	"CalculateTurnOrder",
	"GetHitChance",
	"StatusGetEnterChance",
	"StatusHitEnter",
	"BeforeCharacterApplyDamage",
	"TreasureItemGenerated",
	"BeforeCraftingExecuteCombination",
	"AfterCraftingExecuteCombination",
	"BeforeShootProjectile",
	"ShootProjectile",
	"ProjectileHit",
	"GroundHit"
}

-- FIXME - migrate these events!
Ext._GetSkillDamage = function (...)
    for i,callback in pairs(Ext._Listeners.GetSkillDamage) do
        local status, damageList, deathType = xpcall(callback, debug.traceback, ...)
        if status then
			if damageList ~= nil then
				return damageList, deathType
			end
		else
            Ext.PrintError("Error during GetSkillDamage: ", damageList)
        end
    end
end

Ext._ComputeCharacterHit = function (...)
    return Ext._EngineCallback1("ComputeCharacterHit", ...)
end

Ext._CalculateTurnOrder = function (...)
    return Ext._EngineCallback1("CalculateTurnOrder", ...)
end

Ext._StatusGetEnterChance = function (...)
    return Ext._EngineCallback1("StatusGetEnterChance", ...)
end

Ext.RegisterListener = function (type, fn)
	if Ext._Listeners[type] ~= nil then
		table.insert(Ext._Listeners[type], fn)
	else
		Ext.PrintError("Unknown listener type: " .. type)
	end
end

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

Ext._BeforeCharacterApplyDamage = function (target, attacker, hit, causeType, impactDirection, context)
    for i,callback in pairs(Ext._Listeners.BeforeCharacterApplyDamage) do
        local status, result = xpcall(callback, debug.traceback, target, attacker, hit, causeType, impactDirection, context)
        if not status then
            Ext.PrintError("Error during BeforeCharacterApplyDamage: ", result)
        end
    end

	return hit
end

Ext._TreasureItemGenerated = function (...)
    return Ext._EngineCallback1("TreasureItemGenerated", ...)
end

Ext._BeforeCraftingExecuteCombination = function (...)
    for i,callback in pairs(Ext._Listeners["BeforeCraftingExecuteCombination"]) do
        local status, result = xpcall(callback, debug.traceback, ...)
        if status then
			if result == true then
				return result
			end
		else
            Ext.PrintError("Error during BeforeCraftingExecuteCombination: ", result)
        end
    end
end

Ext._StatusHitEnter = function (...)
    return Ext._Notify("StatusHitEnter", ...)
end

Ext._AfterCraftingExecuteCombination = function (...)
    return Ext._Notify("AfterCraftingExecuteCombination", ...)
end

Ext._OnBeforeShootProjectile = function (...)
    return Ext._Notify("BeforeShootProjectile", ...)
end

Ext._OnShootProjectile = function (...)
    return Ext._Notify("ShootProjectile", ...)
end

Ext._OnProjectileHit = function (...)
    return Ext._Notify("ProjectileHit", ...)
end

Ext._OnGroundHit = function (...)
    return Ext._Notify("GroundHit", ...)
end

Ext._ExecutePropertyDataOnPosition = function (prop, ...)
	local propType = Ext._SkillPropertyTypes[prop.Action]
	if propType ~= nil and propType.ExecuteOnPosition ~= nil then
		propType.ExecuteOnPosition(prop, ...)
	end
end

Ext._ExecutePropertyDataOnTarget = function (prop, ...)
	local propType = Ext._SkillPropertyTypes[prop.Action]
	if propType ~= nil and propType.ExecuteOnTarget ~= nil then
		propType.ExecuteOnTarget(prop, ...)
	end
end

_I._RegisterEvents()
