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
	return Ext.Entity.GetCharacter(handle)
end

-- Debug helper to get character being examined
_E = function ()
	local examine = Ext.UI.GetByType(104)
	if examine == nil then return end
	local handle = examine:GetPlayerHandle()
	if handle == nil then return end
	return Ext.Entity.GetCharacter(handle)
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
Ext.GetPickingState = CallDeprecated(Ext.UI.GetPickingState, "GetPickingState", "UI.GetPickingState")

-- Backwards compatibility with old Entity APIs
Ext.GetCharacter = CallDeprecated(Ext.Entity.GetCharacter, "GetCharacter", "Entity.GetCharacter")
Ext.GetItem = CallDeprecated(Ext.Entity.GetItem, "GetItem", "Entity.GetItem")
Ext.GetStatus = CallDeprecated(Ext.Entity.GetStatus, "GetStatus", "Entity.GetStatus")
Ext.GetGameObject = CallDeprecated(Ext.Entity.GetGameObject, "GetGameObject", "Entity.GetGameObject")
Ext.GetAiGrid = CallDeprecated(Ext.Entity.GetAiGrid, "GetAiGrid", "Entity.GetAiGrid")

Ext.OsirisIsCallable = function ()
	return false
end

-- Backwards compatibility with old Net APIs
Ext.PostMessageToServer = CallDeprecated(Ext.Net.PostMessageToServer, "PostMessageToServer", "Net.PostMessageToServer")

-- Backwards compatibility with old client APIs
Ext.GetGameState = CallDeprecated(Ext.Client.GetGameState, "GetGameState", "Client.GetGameState")
Ext.UpdateShroud = CallDeprecated(Ext.Client.UpdateShroud, "UpdateShroud", "Client.UpdateShroud")

---@overload fun(string:BuiltinUISWFName):integer
Ext.UI.TypeID = {
	actionProgression = 0,
	addContent = 57,
	addContent_c = 81,
	areaInteract_c = 68,
	arenaResult = 125,
	book = 2,
	bottomBar_c = 59,
	buttonLayout_c = 95,
	calibrationScreen = 98,
	campaignManager = 124,
	characterAssign = 52,
	characterAssign_c = 92,
	characterCreation = 3,
	characterCreation_c = 4,
	characterSheet = 119,
	chatLog = 6,
	combatLog = 7,
	combatLog_c = 65,
	combatTurn = 8,
	connectionMenu = 33,
	connectivity_c = 34,
	containerInventory = { Default = 9, Pickpocket = 37 },
	containerInventoryGM = 143,
	contextMenu = { Default = 10, Object = 11 },
	contextMenu_c = { Default = 12, Object = 96 },
	craftPanel_c = 84,
	credits = 53,
	dialog = 14,
	dialog_c = 66,
	dummyOverhead = 15,
	encounterPanel = 105,
	enemyHealthBar = 42,
	engrave = 69,
	equipmentPanel_c = 64,
	examine = 104,
	examine_c = 67,
	feedback_c = 97,
	formation = 130,
	formation_c = 135,
	fullScreenHUD = 100,
	gameMenu = 19,
	gameMenu_c = 77,
	giftBagContent = 147,
	giftBagsMenu = 146,
	gmInventory = 126,
	GMItemSheet = 107,
	GMJournal = 139,
	GMMetadataBox = 109,
	GMMinimap = 113,
	GMMoodPanel = 108,
	GMPanelHUD = 120,
	GMRewardPanel = 131,
	GMSkills = 123,
	hotBar = 40,
	installScreen_c = 80,
	inventorySkillPanel_c = 62,
	itemAction = 86,
	itemGenerator = 106,
	itemSplitter = 21,
	itemSplitter_c = 85,
	journal = 22,
	journal_c = 70,
	journal_csp = 140,
	loadingScreen = 23,
	mainMenu = 28,
	mainMenu_c = 87, -- Still mainMenu.swf, but this is used for controllers after clicking "Options" in the gameMenu_c
	menuBG = 56,
	minimap = 30,
	minimap_c = 60,
	mods = 49,
	mods_c = 103,
	monstersSelection = 127,
	mouseIcon = 31,
	msgBox = 29,
	msgBox_c = 75,
	notification = 36,
	optionsInput = 13,
	optionsSettings = { Default = 45, Video = 45, Audio = 1, Game = 17 },
	optionsSettings_c = { Default = 91, Video = 91, Audio = 88, Game = 89 },
	overhead = 5,
	overviewMap = 112,
	panelSelect_c = 83,
	partyInventory = 116,
	partyInventory_c = 142,
	partyManagement_c = 82,
	pause = 121,
	peace = 122,
	playerInfo = 38,
	playerInfo_c = 61, --Still playerInfo.swf, but the ID is different.
	possessionBar = 110,
	pyramid = 129,
	pyramid_c = 134,
	reputationPanel = 138,
	reward = 136,
	reward_c = 137,
	roll = 118,
	saveLoad = 39,
	saveLoad_c = 74,
	saving = 99,
	serverlist = 26,
	serverlist_c = 27,
	skills = 41,
	skillsSelection = 54,
	sortBy_c = 79,
	startTurnRequest = 145,
	startTurnRequest_c = 144,
	statsPanel_c = 63,
	statusConsole = 117,
	statusPanel = 128,
	stickiesPanel = 133,
	sticky = 132,
	storyElement = 71,
	surfacePainter = 111,
	textDisplay = 43,
	tooltip = 44,
	trade = 46,
	trade_c = 73,
	tutorialBox = 55,
	tutorialBox_c = 94,
	uiCraft = 102,
	uiFade = 16,
	userProfile = 51,
	vignette = 114,
	voiceNotification_c = 93,
	watermark = 141,
	waypoints = 47,
	waypoints_c = 78,
	worldTooltip = 48,
}

local _MultiTypeUI = {
	contextMenu = true,
	contextMenu_c = true,
	containerInventory = true,
	optionsSettings = true,
	optionsSettings_c = true,
}

---@param tbl table
---@param key string
---@return integer
local function _UITypeIDCall(tbl, key)
	local id = tbl[key]
	if _MultiTypeUI[key] then
		local _default = id.Default
		for k,v in pairs(id) do
			if k ~= "Default" then
				local ui = Ext.UI.GetByType(v)
				if ui then
					return v
				end
			end
		end
		return _default
	end
	return id
end

setmetatable(Ext.UI.TypeID, {
	__call = _UITypeIDCall
})