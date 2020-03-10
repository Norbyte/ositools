Ext._WarnDeprecated = function (msg)
	Ext.PrintError(msg)
	Ext.PrintError("See https://github.com/Norbyte/ositools/blob/master/LuaAPIDocs.md#migrating-from-v41-to-v42 for more info.")
end

Ext._Notify = function (event)
    for i,callback in pairs(Ext._Listeners[event]) do
        local status, err = xpcall(callback, debug.traceback)
        if not status then
            Ext.PrintError("Error during " .. event .. ": ", err)
        end
    end
end

Ext._EngineCallback1 = function (event, ...)
    for i,callback in pairs(Ext._Listeners[event]) do
        local status, result = xpcall(callback, debug.traceback, ...)
        if status then
			if result ~= nil then
				return result
			end
		else
            Ext.PrintError("Error during " .. event .. ": ", result)
        end
    end
end

Ext._Listeners = {
	SessionLoading = {},
	ModuleLoading = {},
	ModuleResume = {},
	GetSkillDamage = {},
	ComputeCharacterHit = {},
	CalculateTurnOrder = {},
	GetHitChance = {},
	StatusGetEnterChance = {}
}

Ext._OnGameSessionLoading = function ()
    Ext._Notify("SessionLoading")
end

Ext._OnModuleLoading = function ()
    Ext._Notify("ModuleLoading")
end

Ext._OnModuleResume = function ()
    Ext._Notify("ModuleResume")
end

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

Ext._GetHitChance = function (...)
    return Ext._EngineCallback1("GetHitChance", ...)
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

-- FIXME - migration notes + migration helper function
Ext.EnableStatOverride = function ()
	Ext._WarnDeprecated("Calling Ext.EnableStatOverride() is no longer neccessary!")
end
