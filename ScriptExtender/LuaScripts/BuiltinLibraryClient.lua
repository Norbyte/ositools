local _I = Ext._Internal

_I._PublishedEvents = {
	"SkillGetDescriptionParam",
	"SkillGetPropertyDescription",
	"StatusGetDescriptionParam",
	"GetSkillDamage",
	"GetSkillAPCost",
	"GetHitChance",
	"UIObjectCreated",
	"UIInvoke",
	"UICall",
	"AfterUIInvoke",
	"AfterUICall",
	"InputEvent"
}

Ext.IsClient = function ()
	return true
end

Ext.IsServer = function ()
	return false
end

local UISubscriptionManager = {
	HandleCallListeners = { Before = {}, After = {} },
	TypeCallListeners = { Before = {}, After = {} },
	NameCallListeners = { Before = {}, After = {} },
	HandleInvokeListeners = { Before = {}, After = {} },
	TypeInvokeListeners = { Before = {}, After = {} },
	NameInvokeListeners = { Before = {}, After = {} }
}

function UISubscriptionManager:RegisterCall(uiObject, call, fn, type)
	type = type or "Before"
	local handle = uiObject:GetHandle()
	if self.HandleCallListeners[type][handle] == nil then
		self.HandleCallListeners[type][handle] = {}
	end
	
	if self.HandleCallListeners[type][handle][call] == nil then
		self.HandleCallListeners[type][handle][call] = {}
	end
	
	table.insert(self.HandleCallListeners[type][handle][call], fn)
	uiObject:CaptureExternalInterfaceCalls()
end

function UISubscriptionManager:RegisterTypeCall(typeId, call, fn, type)
	type = type or "Before"
	if self.TypeCallListeners[type][typeId] == nil then
		self.TypeCallListeners[type][typeId] = {}
	end
	
	if self.TypeCallListeners[type][typeId][call] == nil then
		self.TypeCallListeners[type][typeId][call] = {}
	end
	
	table.insert(self.TypeCallListeners[type][typeId][call], fn)
end

function UISubscriptionManager:RegisterNameCall(call, fn, type)
	type = type or "Before"
	if self.NameCallListeners[type][call] == nil then
		self.NameCallListeners[type][call] = {}
	end
	
	table.insert(self.NameCallListeners[type][call], fn)
end

function UISubscriptionManager:RegisterInvoke(uiObject, method, fn, type)
	type = type or "Before"
	local handle = uiObject:GetHandle()
	if self.HandleInvokeListeners[type][handle] == nil then
		self.HandleInvokeListeners[type][handle] = {}
	end
	
	if self.HandleInvokeListeners[type][handle][method] == nil then
		self.HandleInvokeListeners[type][handle][method] = {}
	end
	
	table.insert(self.HandleInvokeListeners[type][handle][method], fn)
	uiObject:CaptureInvokes()
end

function UISubscriptionManager:RegisterTypeInvoke(typeId, method, fn, type)
	type = type or "Before"
	if self.TypeInvokeListeners[type][typeId] == nil then
		self.TypeInvokeListeners[type][typeId] = {}
	end
	
	if self.TypeInvokeListeners[type][typeId][method] == nil then
		self.TypeInvokeListeners[type][typeId][method] = {}
	end
	
	table.insert(self.TypeInvokeListeners[type][typeId][method], fn)
end

function UISubscriptionManager:RegisterNameInvoke(method, fn, type)
	type = type or "Before"
	if self.NameInvokeListeners[type][method] == nil then
		self.NameInvokeListeners[type][method] = {}
	end
	
	table.insert(self.NameInvokeListeners[type][method], fn)
end

function UISubscriptionManager:OnCall(uiObject, call, type, ...)
    local listeners = self.HandleCallListeners[type][uiObject:GetHandle()]
	if listeners ~= nil and listeners[call] ~= nil then
		for i,callback in pairs(listeners[call]) do
			local status, err = xpcall(callback, debug.traceback, uiObject, call, ...)
			if not status then
				Ext.PrintError("Error in UI ExternalInterface handler for '" .. call .. "': ", err)
			end
		end
	end

    local listeners = self.TypeCallListeners[type][uiObject:GetTypeId()]
	if listeners ~= nil and listeners[call] ~= nil then
		for i,callback in pairs(listeners[call]) do
			local status, err = xpcall(callback, debug.traceback, uiObject, call, ...)
			if not status then
				Ext.PrintError("Error in UI ExternalInterface handler for '" .. call .. "': ", err)
			end
		end
	end

    local listeners = self.NameCallListeners[type][call]
	if listeners ~= nil then
		for i,callback in pairs(listeners) do
			local status, err = xpcall(callback, debug.traceback, uiObject, call, ...)
			if not status then
				Ext.PrintError("Error in UI ExternalInterface handler for '" .. call .. "': ", err)
			end
		end
	end
end

function UISubscriptionManager:OnInvoke(uiObject, method, type, ...)
    local listenersByHandle = self.HandleInvokeListeners[type][uiObject:GetHandle()]
	if listenersByHandle ~= nil and listenersByHandle[method] ~= nil then
		for i,callback in pairs(listenersByHandle[method]) do
			local status, err = xpcall(callback, debug.traceback, uiObject, method, ...)
			if not status then
				Ext.PrintError("Error in UI Invoke listener for '" .. method .. "': ", err)
			end
		end
	end

    local listenersByType = self.TypeInvokeListeners[type][uiObject:GetTypeId()]
	if listenersByType ~= nil and listenersByType[method] ~= nil then
		for i,callback in pairs(listenersByType[method]) do
			local status, err = xpcall(callback, debug.traceback, uiObject, method, ...)
			if not status then
				Ext.PrintError("Error in UI Invoke listener for '" .. method .. "': ", err)
			end
		end
	end

    local listenersByName = self.NameInvokeListeners[type][method]
	if listenersByName ~= nil then
		for i,callback in pairs(listenersByName) do
			local status, err = xpcall(callback, debug.traceback, uiObject, method, ...)
			if not status then
				Ext.PrintError("Error in UI Invoke listener for '" .. method .. "': ", err)
			end
		end
	end
end

_I.UISubscriptionManager = UISubscriptionManager



Ext.RegisterUICall = function (uiObject, call, fn, type)
	_I.UISubscriptionManager:RegisterCall(uiObject, call, fn, type)
end

Ext.RegisterUITypeCall = function (typeId, call, fn, type)
	_I.UISubscriptionManager:RegisterTypeCall(typeId, call, fn, type)
end

Ext.RegisterUINameCall = function (call, fn, type)
	_I.UISubscriptionManager:RegisterNameCall(call, fn, type)
end

Ext.RegisterUIInvokeListener = function (uiObject, method, fn, type)
	_I.UISubscriptionManager:RegisterInvoke(uiObject, method, fn, type)
end

Ext.RegisterUITypeInvokeListener = function (typeId, method, fn, type)
	_I.UISubscriptionManager:RegisterTypeInvoke(typeId, method, fn, type)
end

Ext.RegisterUINameInvokeListener = function (method, fn, type)
	_I.UISubscriptionManager:RegisterNameInvoke(method, fn, type)
end



-- Update version label with Script Extender version number when the main menu is rendered
Ext.RegisterUITypeInvokeListener(28, "openMenu", function (menu, ...)
    local ver = Ext.GameVersion() .. " (Script Extender v" .. Ext.Version() .. ")"
    menu:Invoke("setDebugText", ver)
end)

_I._RegisterEvents()

-- Support for UI call/invoke listeners
Ext.Events.UIInvoke:Subscribe(function (e)
	_I.UISubscriptionManager:OnInvoke(e.UI, e.Function, e.When, table.unpack(e.Args))
end)

Ext.Events.UICall:Subscribe(function (e)
	_I.UISubscriptionManager:OnCall(e.UI, e.Function, e.When, table.unpack(e.Args))
end)

-- Subscribe to main menu calls
Ext.Events.UIObjectCreated:Subscribe(function (e)
	local ui = e.UI
    if ui:GetTypeId() == 28 then
        ui:CaptureExternalInterfaceCalls()
        ui:CaptureInvokes()
    end
end)

Ext.Events.SkillGetPropertyDescription:Subscribe(function (e)
	local propType = Ext._SkillPropertyTypes[e.Property.Action]
	if propType ~= nil and propType.GetDescription ~= nil then
		local desc = propType.GetDescription(e.Property)
		if desc ~= nil then
			e.Description = desc
			e:StopPropagation()
		end
	end
end)

-- Debug helper to get current player character
_C = function ()
	local statusConsole = Ext.UI.GetByType(117)
	if statusConsole == nil then return end
	local handle = statusConsole:GetPlayerHandle()
	if handle == nil then return end
	return Ext.GetCharacter(handle)
end

-- Debug helper to get character being examined
_E = function ()
	local examine = Ext.UI.GetByType(104)
	if examine == nil then return end
	local handle = examine:GetPlayerHandle()
	if handle == nil then return end
	return Ext.GetCharacter(handle)
end

-- Backwards compatibility with old UI APIs
Ext.CreateUI = CallDeprecated(Ext.UI.Create, "CreateUI", "UI.Create")
Ext.GetUI = CallDeprecated(Ext.UI.GetByName, "GetUI", "UI.GetByName")
Ext.GetUIByType = CallDeprecated(Ext.UI.GetByType, "GetUIByType", "UI.GetByType")
Ext.GetBuiltinUI = CallDeprecated(Ext.UI.GetByPath, "GetBuiltinUI", "UI.GetByPath")
Ext.DestroyUI = CallDeprecated(Ext.UI.Destroy, "DestroyUI", "UI.Destroy")
Ext.UISetDirty = CallDeprecated(Ext.UI.SetDirty, "UISetDirty", "UI.SetDirty")
Ext.UIEnableCustomDrawCallDebugging = CallDeprecated(Ext.UI.EnableCustomDrawCallDebugging, "UIEnableCustomDrawCallDebugging", "UI.EnableCustomDrawCallDebugging")
Ext.HandleToDouble = CallDeprecated(Ext.UI.HandleToDouble, "HandleToDouble", "UI.HandleToDouble")
Ext.DoubleToHandle = CallDeprecated(Ext.UI.DoubleToHandle, "DoubleToHandle", "UI.DoubleToHandle")

-- Backwards compatibility with old Net APIs
Ext.PostMessageToServer = CallDeprecated(Ext.Net.PostMessageToServer, "PostMessageToServer", "Net.PostMessageToServer")
