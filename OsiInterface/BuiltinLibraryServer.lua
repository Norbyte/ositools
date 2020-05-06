Ext._Listeners = {
	ModuleLoadStarted = {},
	ModuleLoading = {},
	StatsLoaded = {},
	ModuleResume = {},
	SessionLoading = {},
	SessionLoaded = {},
	GetSkillDamage = {},
	ComputeCharacterHit = {},
	CalculateTurnOrder = {},
	GetHitChance = {},
	StatusGetEnterChance = {},
	BeforeCharacterApplyDamage = {}
}

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
	elseif type == "SkillGetDescriptionParam" or type == "StatusGetDescriptionParam" then
		Ext._WarnDeprecated("Cannot register listeners for event '" .. type .. "' from server!")
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

Ext.EnableStatOverride = function ()
	Ext._WarnDeprecated("Calling Ext.EnableStatOverride() is no longer neccessary!")
end

Ext.StatSetLevelScaling = function ()
	Ext._WarnDeprecated("Calling Ext.StatSetLevelScaling() from a server context is deprecated!")
end

Ext._GetModPersistentVars = function (modTable)
	local tab = Mods[modTable]
	if tab ~= nil then
		local persistent = tab.PersistentVars
		if persistent ~= nil then
			return Ext.JsonStringify(persistent)
		end
	end
end

Ext._RestoreModPersistentVars = function (modTable, vars)
	local tab = Mods[modTable]
	if tab ~= nil then
		tab.PersistentVars = Ext.JsonParse(vars)
	end
end

Ext._BeforeCharacterApplyDamage = function (target, attacker, hit, causeType, impactDirection)
    for i,callback in pairs(Ext._Listeners.BeforeCharacterApplyDamage) do
        local status, result = xpcall(callback, debug.traceback, target, attacker, hit, causeType, impactDirection)
        if not status then
            Ext.PrintError("Error during BeforeCharacterApplyDamage: ", result)
        end
    end

	return hit
end
