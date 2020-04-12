Ext._Listeners = {
	SessionLoading = {},
	SessionLoaded = {},
	ModuleLoading = {},
	ModuleResume = {},
	GetSkillDamage = {},
	ComputeCharacterHit = {},
	CalculateTurnOrder = {},
	GetHitChance = {},
	StatusGetEnterChance = {},
	ConsoleCommand = {}
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
