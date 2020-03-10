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
	SkillGetDescriptionParam = {},
	StatusGetDescriptionParam = {},
	GetSkillDamage = {},
	GetHitChance = {}
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

Ext._SkillGetDescriptionParam = function (...)
    return Ext._EngineCallback1("SkillGetDescriptionParam", ...)
end

Ext._StatusGetDescriptionParam = function (...)
    return Ext._EngineCallback1("StatusGetDescriptionParam", ...)
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

Ext._GetHitChance = function (...)
    return Ext._EngineCallback1("GetHitChance", ...)
end

Ext._StatusGetEnterChance = function (...)
    return Ext._EngineCallback1("StatusGetEnterChance", ...)
end

Ext.RegisterListener = function (type, fn)
	if Ext._Listeners[type] ~= nil then
		table.insert(Ext._Listeners[type], fn)
	elseif type == "CalculateTurnOrder" or type == "ComputeCharacterHit" or type == "StatusGetEnterChance" then
		Ext._WarnDeprecated("Cannot register listeners for event '" .. type .. "' from client!")
	else
		Ext.PrintError("Unknown listener type: " .. type)
	end
end

Ext.NewCall = function ()
	Ext._WarnDeprecated("Calling Ext.NewCall() from a client context is deprecated!")
end

Ext.NewQuery = function ()
	Ext._WarnDeprecated("Calling Ext.NewQuery() from a client context is deprecated!")
end

Ext.NewEvent = function ()
	Ext._WarnDeprecated("Calling Ext.NewEvent() from a client context is deprecated!")
end

Ext.GetCharacter = function ()
	Ext._WarnDeprecated("Calling Ext.GetCharacter() from a client context is deprecated!")
end

Ext.GetItem = function ()
	Ext._WarnDeprecated("Calling Ext.GetItem() from a client context is deprecated!")
end

Ext.GetStatus = function ()
	Ext._WarnDeprecated("Calling Ext.GetStatus() from a client context is deprecated!")
end

Ext.GetCombat = function ()
	Ext._WarnDeprecated("Calling Ext.GetCombat() from a client context is deprecated!")
end

Ext.GenerateIdeHelpers = function ()
	Ext._WarnDeprecated("Calling Ext.GenerateIdeHelpers() from a client context is deprecated!")
end

Ext.EnableStatOverride = function ()
	Ext._WarnDeprecated("Calling Ext.EnableStatOverride() is no longer neccessary!")
end
