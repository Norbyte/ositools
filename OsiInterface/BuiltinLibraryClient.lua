Ext._Listeners = {
	ModuleLoadStarted = {},
	ModuleLoading = {},
	StatsLoaded = {},
	ModuleResume = {},
	SessionLoading = {},
	SessionLoaded = {},
	SkillGetDescriptionParam = {},
	StatusGetDescriptionParam = {},
	GetSkillDamage = {},
	GetHitChance = {},
	UIInvoke = {},
	UICall = {}
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

Ext._UIExternalInterfaceHandleListeners = {}
Ext._UIExternalInterfaceTypeListeners = {}
Ext._UIExternalInterfaceNameListeners = {}

Ext.RegisterUICall = function (object, call, fn)
	local handle = object:GetHandle()
	if Ext._UIExternalInterfaceHandleListeners[handle] == nil then
		Ext._UIExternalInterfaceHandleListeners[handle] = {}
	end
	
	if Ext._UIExternalInterfaceHandleListeners[handle][call] == nil then
		Ext._UIExternalInterfaceHandleListeners[handle][call] = {}
	end
	
	table.insert(Ext._UIExternalInterfaceHandleListeners[handle][call], fn)
	object:CaptureExternalInterfaceCalls()
end

Ext.RegisterUITypeCall = function (typeId, call, fn)
	if Ext._UIExternalInterfaceTypeListeners[typeId] == nil then
		Ext._UIExternalInterfaceTypeListeners[typeId] = {}
	end
	
	if Ext._UIExternalInterfaceTypeListeners[typeId][call] == nil then
		Ext._UIExternalInterfaceTypeListeners[typeId][call] = {}
	end
	
	table.insert(Ext._UIExternalInterfaceTypeListeners[typeId][call], fn)
end

Ext.RegisterUINameCall = function (call, fn)
	if Ext._UIExternalInterfaceNameListeners[call] == nil then
		Ext._UIExternalInterfaceNameListeners[call] = {}
	end
	
	table.insert(Ext._UIExternalInterfaceNameListeners[call], fn)
end

Ext._UICall = function (object, call, ...)
    local listeners = Ext._UIExternalInterfaceHandleListeners[object:GetHandle()]
	if listeners ~= nil and listeners[call] ~= nil then
		for i,callback in pairs(listeners[call]) do
			local status, err = xpcall(callback, debug.traceback, object, call, ...)
			if not status then
				Ext.PrintError("Error in UI ExternalInterface handler for '" .. call .. "': ", err)
			end
		end
	end

    local listeners = Ext._UIExternalInterfaceTypeListeners[object:GetTypeId()]
	if listeners ~= nil and listeners[call] ~= nil then
		for i,callback in pairs(listeners[call]) do
			local status, err = xpcall(callback, debug.traceback, object, call, ...)
			if not status then
				Ext.PrintError("Error in UI ExternalInterface handler for '" .. call .. "': ", err)
			end
		end
	end

    local listeners = Ext._UIExternalInterfaceNameListeners[call]
	if listeners ~= nil then
		for i,callback in pairs(listeners) do
			local status, err = xpcall(callback, debug.traceback, object, call, ...)
			if not status then
				Ext.PrintError("Error in UI ExternalInterface handler for '" .. call .. "': ", err)
			end
		end
	end

	Ext._Notify("UICall", ...)
end

Ext._UIInvokeHandleListeners = {}
Ext._UIInvokeTypeListeners = {}
Ext._UIInvokeNameListeners = {}

Ext.RegisterUIInvokeListener = function (object, method, fn)
	local handle = object:GetHandle()
	if Ext._UIInvokeHandleListeners[handle] == nil then
		Ext._UIInvokeHandleListeners[handle] = {}
	end
	
	if Ext._UIInvokeHandleListeners[handle][method] == nil then
		Ext._UIInvokeHandleListeners[handle][method] = {}
	end
	
	table.insert(Ext._UIInvokeHandleListeners[handle][method], fn)
	object:CaptureInvokes()
end

Ext.RegisterUITypeInvokeListener = function (typeId, method, fn)
	if Ext._UIInvokeTypeListeners[typeId] == nil then
		Ext._UIInvokeTypeListeners[typeId] = {}
	end
	
	if Ext._UIInvokeTypeListeners[typeId][method] == nil then
		Ext._UIInvokeTypeListeners[typeId][method] = {}
	end
	
	table.insert(Ext._UIInvokeTypeListeners[typeId][method], fn)
end

Ext.RegisterUINameInvokeListener = function (method, fn)
	if Ext._UIInvokeNameListeners[method] == nil then
		Ext._UIInvokeNameListeners[method] = {}
	end
	
	table.insert(Ext._UIInvokeNameListeners[method], fn)
end

Ext._UIInvoke = function (object, method, ...)
    local listenersByHandle = Ext._UIInvokeHandleListeners[object:GetHandle()]
	if listenersByHandle ~= nil and listenersByHandle[method] ~= nil then
		for i,callback in pairs(listenersByHandle[method]) do
			local status, err = xpcall(callback, debug.traceback, object, method, ...)
			if not status then
				Ext.PrintError("Error in UI Invoke listener for '" .. method .. "': ", err)
			end
		end
	end

    local listenersByType = Ext._UIInvokeTypeListeners[object:GetTypeId()]
	if listenersByType ~= nil and listenersByType[method] ~= nil then
		for i,callback in pairs(listenersByType[method]) do
			local status, err = xpcall(callback, debug.traceback, object, method, ...)
			if not status then
				Ext.PrintError("Error in UI Invoke listener for '" .. method .. "': ", err)
			end
		end
	end

    local listenersByName = Ext._UIInvokeNameListeners[method]
	if listenersByName ~= nil then
		for i,callback in pairs(listenersByName) do
			local status, err = xpcall(callback, debug.traceback, object, method, ...)
			if not status then
				Ext.PrintError("Error in UI Invoke listener for '" .. method .. "': ", err)
			end
		end
	end
	
	Ext._Notify("UIInvoke", ...)
end
