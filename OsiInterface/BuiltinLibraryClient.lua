Ext._Listeners = {
	SessionLoading = {},
	SessionLoaded = {},
	ModuleLoading = {},
	ModuleResume = {},
	SkillGetDescriptionParam = {},
	StatusGetDescriptionParam = {},
	GetSkillDamage = {},
	GetHitChance = {}
}

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

Ext.RegisterListener = function (type, fn)
	if Ext._Listeners[type] ~= nil then
		table.insert(Ext._Listeners[type], fn)
	elseif type == "CalculateTurnOrder" or type == "ComputeCharacterHit" or type == "StatusGetEnterChance" then
		Ext._WarnDeprecated("Cannot register listeners for event '" .. type .. "' from client!")
	else
		Ext.PrintError("Unknown listener type: " .. type)
	end
end

Ext.IsClient = function ()
	return true
end

Ext.IsServer = function ()
	return false
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

Ext.GetCombat = function ()
	Ext._WarnDeprecated("Calling Ext.GetCombat() from a client context is deprecated!")
end

Ext.GenerateIdeHelpers = function ()
	Ext._WarnDeprecated("Calling Ext.GenerateIdeHelpers() from a client context is deprecated!")
end

Ext.EnableStatOverride = function ()
	Ext._WarnDeprecated("Calling Ext.EnableStatOverride() is no longer neccessary!")
end

Ext._UIListeners = {}

Ext.RegisterUICall = function (object, call, fn)
	local handle = object:GetHandle()
	if Ext._UIListeners[handle] == nil then
		Ext._UIListeners[handle] = {}
	end
	
	if Ext._UIListeners[handle][call] == nil then
		Ext._UIListeners[handle][call] = {}
	end
	
	table.insert(Ext._UIListeners[handle][call], fn)
	object:CaptureExternalInterfaceCalls()
end

Ext._UICall = function (object, call, ...)
    local listeners = Ext._UIListeners[object:GetHandle()]
	if listeners ~= nil and listeners[call] ~= nil then
		for i,callback in pairs(listeners[call]) do
			local status, err = xpcall(callback, debug.traceback, object, call, ...)
			if not status then
				Ext.PrintError("Error in UI ExternalInterface handler for '" .. call .. "': ", err)
			end
		end
	end
end
