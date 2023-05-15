local _DEBUG = Ext.Debug.IsDeveloperMode()
local _DoubleToHandle = Ext.UI.DoubleToHandle
local _EnumIndexToLabel = Ext.Stats.EnumIndexToLabel
local _GetAiGrid = Ext.Entity.GetAiGrid
local _GetCharacter = Ext.Entity.GetCharacter
local _GetGameMode = Ext.Utils.GetGameMode
local _GetGameObject = Ext.Entity.GetGameObject
local _GetItem = Ext.Entity.GetItem
local _GetObjectType = Ext.Types.GetObjectType
local _GetPickingState = Ext.UI.GetPickingState
local _GetStat = Ext.Stats.Get
local _GetStatus = Ext.Entity.GetStatus
local _GetTranslatedString = Ext.L10N.GetTranslatedString
local _GetTranslatedStringFromKey = Ext.L10N.GetTranslatedStringFromKey
local _GetUIByType = Ext.UI.GetByType
local _GetUIGetByPath = Ext.UI.GetByPath
local _HandleToDouble = Ext.UI.HandleToDouble
local _MonotonicTime = Ext.Utils.MonotonicTime
local _PrintError = Ext.Utils.PrintError
local _PrintWarning = Ext.Utils.PrintWarning

local _IsNaN = Ext.Math.IsNaN
local _IsValidHandle = Ext.Utils.IsValidHandle
local _UITYPE = Ext.UI.TypeID

local _IsNullOrEmpty = function(str) return str == nil or str == "" end
local _ObjectIsItem = function(obj) return _GetObjectType(obj) == "ecl::Item" end

---@class GameTooltipRequestProcessor
local RequestProcessor = {
	---@type table<string,fun(request:TooltipRequest, ui:UIObject, uiType:integer, event:string, ...:boolean|string|number):TooltipRequest>
	CallbackHandler = {},
	---@type TooltipHooks
	Tooltip = nil,
	ControllerEnabled = false,
}

---@class GameTooltipRequestProcessorInternals
local _INTERNAL = {}
RequestProcessor._Internal = _INTERNAL

RequestProcessor.ControllerEnabled = (_GetUIGetByPath("Public/Game/GUI/msgBox_c.swf") or _GetUIByType(_UITYPE.msgBox_c)) ~= nil

local TooltipCalls = {
	Skill = "showSkillTooltip",
	Status = "showStatusTooltip",
	Item ="showItemTooltip",
	Stat = "showStatTooltip",
	Ability = "showAbilityTooltip",
	Talent = "showTalentTooltip",
	Tag = "showTagTooltip",
	CustomStat = "showCustomStatTooltip",
	Rune = "showRuneTooltip",
	Pyramid = "pyramidOver",
	PlayerPortrait = "showCharTooltip",
	-- World = "updateTooltips",
	-- WorldHover = "setText",
}

local TooltipInvokes = {
	Surface = "displaySurfaceText"
}

local ControllerCharacterCreationCalls = {
	Skill = "requestSkillTooltip",
	Stat = "requestAttributeTooltip",
	Ability = "requestAbilityTooltip",
	Talent = "requestTalentTooltip",
	Tag = "requestTagTooltip",
}

---@param doubleHandle integer
---@param getObjectFunc? (fun(handle:ComponentHandle|number|string):EclCharacter|EclItem|nil)
---@return EclCharacter|EclItem|nil
local function __TryGetObjectFromDouble(doubleHandle, getObjectFunc)
	local handle = _DoubleToHandle(doubleHandle)
	if _IsValidHandle(handle) then
		getObjectFunc = getObjectFunc or _GetGameObject
		return getObjectFunc(handle)
	end
	return nil
end

---@param doubleHandle number
---@param getObjectFunc function|nil
---@return EclCharacter|EclItem|nil
local function _GetObjectFromDouble(doubleHandle, getObjectFunc)
	if doubleHandle == nil or doubleHandle == 0 or _IsNaN(doubleHandle) then
		return nil
	end
	local b,result = pcall(__TryGetObjectFromDouble, doubleHandle, getObjectFunc)
	if b then
		return result
	elseif _DEBUG then
		Ext.Utils.PrintError(result)
	end
	return nil
end

---@param handle ComponentHandle|nil
---@param getObjectFunc function|nil
---@return EclCharacter|EclItem|nil
local function _GetObjectFromHandle(handle, getObjectFunc)
	if _IsValidHandle(handle) then
		if not getObjectFunc then
			getObjectFunc = _GetGameObject
		end
		local b,result = pcall(getObjectFunc, handle)
		if b then
			return result
		elseif _DEBUG then
			Ext.Utils.PrintError(result)
			Ext.Utils.PrintError("_GetObjectFromHandle", handle)
		end
	end
	return nil
end

---Get the GM's target character in GM mode.
---@return EclCharacter
local function _GetGMTargetCharacter()
	local ui = _GetUIByType(_UITYPE.GMPanelHUD)
	if ui then
		local this = ui:GetRoot()
		if this then
			return _GetObjectFromDouble(this.targetHandle, _GetCharacter)
		end
	end
	return nil
end

---Get the current character stored in characterSheet's main timeline.
---@param this {characterHandle:number|nil}|nil
---@return EclCharacter
local function _GetCharacterSheetCharacter(this)
	local character = nil
	if this == nil then
		if not RequestProcessor.ControllerEnabled then
			local ui = _GetUIByType(_UITYPE.characterSheet)
			if ui then
				this = ui:GetRoot()
			end
		else
			local ui = _GetUIByType(_UITYPE.statsPanel_c)
			if ui then
				this = ui:GetRoot()
			end
		end
	end
	if this ~= nil then
		character = _GetObjectFromDouble(this.characterHandle, _GetCharacter)
	end
	return character or RequestProcessor.Utils.GetClientCharacter()
end

local _StatsIdTooltipTypes = {
	Item = true,
	Pyramid = true,
	Rune = true,
}

local _ObjectParamNames = {
	Character = true,
	Item = true,
	RuneItem = true,
	Object = true,
}

---@return TooltipRequest
local function _CreateRequest()
	local request = {
		Type = ""
	}
	--Support lifetime changes by getting the object on the fly
	setmetatable(request, {
		__index = function(tbl,k)
			local tooltipType = rawget(tbl, "Type")
			if _ObjectParamNames[k] then
				local objectHandleDouble = rawget(tbl, "ObjectHandleDouble")
				if objectHandleDouble then
					if k == "Character" then
						return _GetObjectFromDouble(objectHandleDouble, _GetCharacter)
					elseif k == "Item" or k == "RuneItem" then
						return _GetObjectFromDouble(objectHandleDouble, _GetItem)
					elseif "Object" then
						return _GetObjectFromDouble(objectHandleDouble)
					end
				end
			else
				if k == "Owner" and tooltipType == "Item" then
					local objectHandleDouble = rawget(tbl, "OwnerDoubleHandle")
					if objectHandleDouble then
						return _GetObjectFromDouble(objectHandleDouble)
					end
				elseif k == "StatsId" and _StatsIdTooltipTypes[tooltipType] then
					local objectHandleDouble = rawget(tbl, "ObjectHandleDouble")
					if objectHandleDouble then
						local obj = _GetObjectFromDouble(objectHandleDouble)
						if obj and _ObjectIsItem(obj) then
							rawset(tbl, "StatsId", obj.StatsId)
							return obj.StatsId
						end
					end
				elseif k == "Status" or k == "StatusId" then
					local objectHandleDouble = rawget(tbl, "ObjectHandleDouble")
					local statusHandleDouble = rawget(tbl, "StatusHandleDouble")
					if statusHandleDouble and objectHandleDouble then
						local handle = _DoubleToHandle(objectHandleDouble)
						local statusHandle = _DoubleToHandle(request.StatusHandleDouble)
						if _IsValidHandle(handle) and _IsValidHandle(statusHandle) then
							local status = _GetStatus(handle, statusHandle)
							if status then
								if k == "StatusId" then
									rawset(tbl, "StatusId", status.StatusId)
									return status.StatusId
								else
									return status
								end
							end
						end
					end
				elseif k == "Rune" then
					local statsId = rawget(tbl, "StatsId")
					if statsId ~= nil and statsId ~= "" then
						return _GetStat(statsId)
					end
				end
			end
		end
	})
	return request
end

RequestProcessor.CreateRequest = _CreateRequest

RequestProcessor.CallbackHandler[TooltipCalls.Skill] = function(request, ui, uiType, event, id)
	request.Skill = id
	return request
end

RequestProcessor.CallbackHandler[TooltipCalls.Status] = function(request, ui, uiType, event, id)
	request.StatusHandleDouble = id
	local b,status = pcall(_GetStatus, _DoubleToHandle(request.ObjectHandleDouble), _DoubleToHandle(id))
	if b and status then
		request.StatusId = status and status.StatusId or ""
	end
	return request
end

RequestProcessor.CallbackHandler[TooltipCalls.Item] = function (request, ui, uiType, event, ...)
	local params = {...}
	if uiType == _UITYPE.partyInventory_c then
		local this = ui:GetRoot()
		local id = params[1]
		local slot = params[2]
		local handleDouble = params[3]
		local ownerHandle = handleDouble
		if ownerHandle == nil then
			ownerHandle = this.ownerHandle
		end
		if id == nil then
			local inventoryArray = this.inventoryPanel_mc.inventoryList.content_array
			for i=0,#inventoryArray do
				local playerInventory = inventoryArray[i]
				if playerInventory ~= nil then
					if ownerHandle == nil then
						ownerHandle = playerInventory.ownerHandle
					end
					local localInventory = playerInventory.localInventory
					if localInventory._currentIdx >= 0 then
						local currentItem = localInventory._itemArray[localInventory._currentIdx]
						if currentItem ~= nil then
							id = currentItem.itemHandle
							break
						end
					end
				end
			end
		end
		if id then
			request.ObjectHandleDouble = id
		end
		if ownerHandle ~= nil and ownerHandle ~= 0 then
			local inventoryHandle = _DoubleToHandle(ownerHandle)
			if _IsValidHandle(inventoryHandle) then
				request.OwnerDoubleHandle = ownerHandle
			end
		end
	elseif uiType == _UITYPE.uiCraft then
		--Tooltip support for ingredient tooltips
		local id,x,y,width,height,contextParam,side = table.unpack(params)
		if id == 0 then
			--mc.itemHandle is always 0 for ingredients in the recipe UI, which is the first param		
			request.ObjectHandleDouble = nil
			request.Type = "Generic"
			request.X = x
			request.Y = y
			request.Width = width
			request.Height = height
			request.Side = side
			request.AllowDelay = false
		elseif id ~= nil then
			--The Combine tab sends a proper handle
			request.ObjectHandleDouble = id
		else
			_PrintWarning(string.format("[Game.Tooltip.RequestProcessor:%s] Item handle (%s) is nil? UI(%s)", event, id, uiType))
		end
	elseif uiType == _UITYPE.containerInventory.Default or uiType == _UITYPE.containerInventory.Pickpocket then
		local doubleHandle = params[1]
		if not _IsNaN(doubleHandle) and doubleHandle > 0 then
			request.ObjectHandleDouble = doubleHandle
		end
	else
		local id = params[1]
		if _IsNaN(id) then
			_PrintWarning(string.format("[Game.Tooltip.RequestProcessor:%s] Item handle (%s) is nil? UI(%s)", event, id, uiType))
			return request
		end
		request.ObjectHandleDouble = id
	end
	return request
end

--ExternalInterface.call("pyramidOver",param1.id,val2.x,val2.y,param1.width,param1.height,"bottom");
RequestProcessor.CallbackHandler[TooltipCalls.Pyramid] = function(request, ui, uiType, event, id, x, y, width, height, side)
	request.ObjectHandleDouble = id
	return request
end

RequestProcessor.CallbackHandler[TooltipCalls.Stat] = function(request, ui, uiType, event, id)
	if request.Stat == nil then
		local stat = Game.Tooltip.TooltipStatAttributes[id]
		request.Stat = stat or id
	end
	return request
end

RequestProcessor.CallbackHandler[TooltipCalls.CustomStat] = function(request, ui, uiType, event, id, index)
	if request.Stat == nil then
		request.Stat = id or -1
		request.StatIndex = index or -1
	end
	return request
end

RequestProcessor.CallbackHandler[TooltipCalls.Ability] = function(request, ui, uiType, event, id)
	if request.Ability == nil then
		request.Ability = _EnumIndexToLabel("AbilityType", id) or id
	end
	return request
end

RequestProcessor.CallbackHandler[TooltipCalls.Talent] = function(request, ui, uiType, event, id, ...)
	if request.Talent == nil then
		request.Talent = _EnumIndexToLabel("TalentType", id) or id
	end
	return request
end

RequestProcessor.CallbackHandler[TooltipCalls.Tag] = function(request, ui, uiType, event, id, arg2)
	request.Tag = id
	request.Category = ""
	if uiType == _UITYPE.characterCreation then
		local this = ui:GetRoot()
		for i=0,#this.CCPanel_mc.tags_mc.tagList.content_array-1 do
			local tag = this.CCPanel_mc.tags_mc.tagList.content_array[i]
			if tag and tag.tagID == id then
				request.Category = tag.categoryID
				break
			end
		end
	elseif uiType == _UITYPE.characterCreation_c then
		request.Tag = arg2
		request.Category = id
	end
	return request
end

RequestProcessor.CallbackHandler[TooltipCalls.PlayerPortrait] = function(request, ui, uiType, event, id, x, y, width, height, side)
	request.ObjectHandleDouble = id
	return request
end

RequestProcessor.CallbackHandler[TooltipCalls.Rune] = function(request, ui, uiType, event, slot)
	request.Rune = nil
	request.Slot = slot
	request.StatsId = nil

	local this = ui:GetRoot()
	if this then
		if uiType == _UITYPE.uiCraft then
			local doubleHandle = this.craftPanel_mc.runesPanel_mc.targetHit_mc.itemHandle
			local item = _GetObjectFromDouble(doubleHandle)
			if item then
				request.ObjectHandleDouble = doubleHandle
				local boostEntry = item.Stats.DynamicStats[3+slot]
				if boostEntry then
					request.StatsId = boostEntry.BoostName
				end
			end
		elseif uiType == _UITYPE.craftPanel_c then
			--The tooltip may not be visible yet, but we can still update it
			local runePanel = this.craftPanel_mc.runePanel_mc
			if runePanel then
				request.ObjectHandleDouble = runePanel.runes_mc.runeTargetHandle
				local item = _GetObjectFromDouble(request.ObjectHandleDouble)
				if slot == 0 then
					-- The target item is selected instead of a rune, so this should be an item tooltip
					request.Type = "Item"
					return request
				else
					slot = slot - 1
					request.Slot = slot
					request.ObjectHandleDouble = runePanel.item_array[runePanel.currentHLSlot].itemHandle
					local rune = _GetObjectFromDouble(request.ObjectHandleDouble)
					local statsID = ""
					if rune then
						statsID = rune.StatsId
					elseif item and item.Stats then
						local runeBoost = item.Stats.DynamicStats[3+slot]
						if runeBoost then
							statsID = runeBoost.BoostName
						end
					end

					request.StatsId = statsID
				end
			end
		end
	end

	return request
end

local SurfaceFlags = {
	Ground = {
		Type = {
			Fire = 0x1000000,
			Water = 0x2000000,
			Blood = 0x4000000,
			Poison = 0x8000000,
			Oil = 0x10000000,
			Lava = 0x20000000,
			Source = 0x40000000,
			Web = 0x80000000,
			Deepwater = 0x100000000,
			Sulfurium = 0x200000000,
			--UNUSED = 0x400000000
		},
		State = {
			Blessed = 0x400000000000,
			Cursed = 0x800000000000,
			Purified = 0x1000000000000,
			--??? = 0x2000000000000
		},
		Modifier = {
			Electrified = 0x40000000000000,
			Frozen = 0x80000000000000,
		},
	},
	Cloud = {
		Type = {
			FireCloud = 0x800000000,
			WaterCloud = 0x1000000000,
			BloodCloud = 0x2000000000,
			PoisonCloud = 0x4000000000,
			SmokeCloud = 0x8000000000,
			ExplosionCloud = 0x10000000000,
			FrostCloud = 0x20000000000,
			Deathfog = 0x40000000000,
			ShockwaveCloud = 0x80000000000,
			--UNUSED = 0x100000000000
			--UNUSED = 0x200000000000
		},
		State = {
			Blessed = 0x4000000000000,
			Cursed = 0x8000000000000,
			Purified = 0x10000000000000,
			--UNUSED = 0x20000000000000
		},
		Modifier = {
			Electrified = 0x100000000000000,
			-- ElectrifiedDecay = 0x200000000000000,
			-- SomeDecay = 0x400000000000000,
			--UNUSED = 0x800000000000000
		}
	},
	--AI grid painted flags
	-- Irreplaceable = 0x4000000000000000,
	-- IrreplaceableCloud = 0x800000000000000,
}

---@param flags integer
---@param data {Cell:_GameTooltipGridCell, Ground:string, Cloud:string}
local function SetSurfaceFromFlags(flags, data)
	for k,f in pairs(SurfaceFlags.Ground.Type) do
		if (flags & f) ~= 0 then
			data.Ground = k
		end
	end
	if data.Ground then
		for k,f in pairs(SurfaceFlags.Ground.Modifier) do
			if (flags & f) ~= 0 then
				data.Ground = data.Ground .. k
			end
		end
		for k,f in pairs(SurfaceFlags.Ground.State) do
			if (flags & f) ~= 0 then
				data.Ground = data.Ground .. k
			end
		end
	end
	for k,f in pairs(SurfaceFlags.Cloud.Type) do
		if (flags & f) ~= 0 then
			data.Cloud = k
		end
	end
	if data.Cloud then
		for k,f in pairs(SurfaceFlags.Cloud.Modifier) do
			if (flags & f) ~= 0 then
				data.Cloud = data.Cloud .. k
			end
		end
		for k,f in pairs(SurfaceFlags.Cloud.State) do
			if (flags & f) ~= 0 then
				data.Cloud = data.Cloud .. k
			end
		end
	end
end

---@alias _GameTooltipGridCell {Flags:integer, Height:number, Objects:ComponentHandle[]|nil}

---@param x number
---@param z number
---@param grid EocAiGrid
---@return {Cell:_GameTooltipGridCell, Ground:string|nil, Cloud:string|nil}|nil
local function _GetSurfaces(x, z, grid)
	local cell = grid:GetCellInfo(x, z)
	if cell then
		local data = { Cell=cell }
		if cell.Flags then
			SetSurfaceFromFlags(cell.Flags, data)
		end
		return data
	end
end

RequestProcessor.CallbackHandler[TooltipInvokes.Surface] = function(request, ui, uiType, event, x, y)
	local surfaces = nil
	local cursor = _GetPickingState()
	if cursor and cursor.WalkablePosition then
		request.Position = cursor.WalkablePosition
		local grid = _GetAiGrid()
		if grid then
			surfaces = _GetSurfaces(cursor.WalkablePosition[1], cursor.WalkablePosition[3], grid)
		end
	end
	if surfaces then
		request.Flags = surfaces.Flags
		request.Ground = surfaces.Ground
		request.Cloud = surfaces.Cloud
	end
	return request
end

---The last double handle of the object under the cursor in KB+M mode, when the context menu was opened.
---@type number|nil
local lastCursorObjectDoubleHandle = nil

local function _CaptureCursorObject(ui, event)
	local cursor = _GetPickingState()
	if cursor then
		if _IsValidHandle(cursor.HoverCharacter) then
			lastCursorObjectDoubleHandle = _HandleToDouble(cursor.HoverCharacter)
		elseif _IsValidHandle(cursor.HoverCharacter2) then
			lastCursorObjectDoubleHandle = _HandleToDouble(cursor.HoverCharacter2)
		elseif _IsValidHandle(cursor.HoverItem) then
			lastCursorObjectDoubleHandle = _HandleToDouble(cursor.HoverItem)
		end
	end
end

local function _OnExamineWindowClosed(ui, event)
	lastCursorObjectDoubleHandle = nil
end

Ext.RegisterUITypeInvokeListener(_UITYPE.contextMenu.Object, "open", _CaptureCursorObject)
Ext.RegisterUITypeCall(_UITYPE.examine, "hideUI", _OnExamineWindowClosed)

---@param e EclLuaUICallEvent
---@param ui UIObject
---@param event string
---@param typeIndex integer
---@param id integer
---@vararg any
function RequestProcessor.OnExamineTooltip(e, ui, event, typeIndex, id, ...)
	---@type EclCharacter|EclItem|nil
	local object = nil

	--GetPlayerHandle returns the examined character or item
	object = _GetObjectFromHandle(ui:GetPlayerHandle())

	if not object and lastCursorObjectDoubleHandle then
		object = _GetObjectFromDouble(lastCursorObjectDoubleHandle)
	end

	if not object then
		object = RequestProcessor.Utils.GetClientCharacter()
	end

	local request = _CreateRequest()
	request.UIType = ui.Type

	if object then
		request.ObjectHandleDouble = _HandleToDouble(object.Handle)
	end

	if typeIndex == 1 then
		request.Type = "Stat"
		request.Stat = Game.Tooltip.TooltipStatAttributes[id] or id

		if request.Stat == nil then
			_PrintWarning("Requested tooltip for unknown stat ID " .. id)
		end
	elseif typeIndex == 2 then
		request.Type = "Ability"
		request.Ability = _EnumIndexToLabel("AbilityType", id) or id
	elseif typeIndex == 3 then
		if id == 0 then
			--Tooltip for "This character has no talents" doesn't exist.
			RequestProcessor.Tooltip.Last.Event = event
			RequestProcessor.Tooltip.Last.UIType = ui.Type
			return
		else
			request.Type = "Talent"
			request.Talent = _EnumIndexToLabel("TalentType", id) or id
		end
	elseif typeIndex == 7 then
		request.Type = "Status"
		if not _IsNaN(id) then
			local statusHandle = _DoubleToHandle(id)
			if _IsValidHandle(statusHandle) then
				request.StatusHandleDouble = id
				if object then
					local status = _GetStatus(object.Handle, statusHandle)
					if status then
						request.StatusId = status.StatusId
					end
				end
			end
		end
	else
		local text = typeIndex
		local x = id
		local y, width, height, side, allowDelay = table.unpack({...})
		--text, x, y, width, height, side, allowDelay
		--Generic type
		request.Type = "Generic"
		request.Text = text
		if x then
			request.X = x
			request.Y = y
			request.Width = width
			request.Height = height
			request.Side = side
			request.AllowDelay = allowDelay
		end
	end

	if RequestProcessor.Tooltip.NextRequest ~= nil then
		_PrintWarning("[Game.Tooltip.RequestProcessor.OnExamineTooltip] Previous tooltip request not cleared in render callback?")
	end

	RequestProcessor.Tooltip.NextRequest = request
	RequestProcessor.Tooltip:InvokeRequestListeners(request, "Before", ui, ui.Type, event, typeIndex, id, ...)

	RequestProcessor.Tooltip.Last.Event = event
	RequestProcessor.Tooltip.Last.UIType = request.UIType
	RequestProcessor.Tooltip:InvokeRequestListeners(request, "After", ui, ui.Type, event, typeIndex, id, ...)
end

---@param ui UIObject
---@param event string
---@param id string
---@param objectHandle number|nil
function RequestProcessor.OnControllerExamineTooltip(e, ui, event, id, objectHandle)
	local request = RequestProcessor.CreateRequest()
	local uiTypeId = ui.Type
	request.UIType = uiTypeId

	---@type EclItem|EclCharacter|nil
	local object = nil

	if not _IsNaN(objectHandle) then
		object = _GetObjectFromDouble(objectHandle)
	end

	if not object then
		object = _GetObjectFromHandle(ui:GetPlayerHandle())
	end

	if not object then
		local uiType = ui.Type
		if uiType == _UITYPE.examine_c then
			object = _GetObjectFromDouble(Game.Tooltip.ControllerVars.LastOverhead)
		else
			object = _GetObjectFromHandle(ui:GetPlayerHandle())
			if not object and uiType == _UITYPE.statsPanel_c then
				object = RequestProcessor.Utils.GetClientCharacter()
			end
		end
	end

	if not object then
		object = RequestProcessor.Utils.GetClientCharacter()
	end

	if object then
		request.ObjectHandleDouble = _HandleToDouble(object.Handle)
	end

	if event == "selectStatus" then
		request.Type = "Status"
		if not _IsNaN(id) then
			local statusHandle = _DoubleToHandle(id)
			if _IsValidHandle(statusHandle) then
				request.StatusHandleDouble = id
				if object then
					local status = _GetStatus(object.Handle, statusHandle)
					if status then
						request.StatusId = status.StatusId
					end
				end
			end
		end
	elseif event == "selectAbility" then
		request.Type = "Ability"
		request.Ability = _EnumIndexToLabel("AbilityType", id)
	elseif event == "selectTalent" then
		request.Type = "Talent"
		request.Talent = _EnumIndexToLabel("TalentType", id)
	elseif event == "selectStat" or event == "selectedAttribute" then
		request.Type = "Stat"
		request.Stat = id
		local stat = Game.Tooltip.TooltipStatAttributes[request.Stat]
		if stat ~= nil then
			request.Stat = stat
		else
			_PrintWarning(string.format("[RequestProcessor.OnControllerExamineTooltip] Requested tooltip for unknown stat ID (%s)", request.Stat))
		end
	elseif event == "selectCustomStat" then
		request.Type = "CustomStat"
		request.Stat = id
	elseif event == "selectTag" then
		request.Type = "Tag"
		request.Tag = id
		request.Category = ""
	end

	RequestProcessor.Tooltip.NextRequest = request
	RequestProcessor.Tooltip:InvokeRequestListeners(request, "Before", ui, uiTypeId, event, id, objectHandle)
	if object then
		Game.Tooltip.ControllerVars.LastPlayer = request.ObjectHandleDouble
	end

	RequestProcessor.Tooltip.Last.Event = event
	RequestProcessor.Tooltip.Last.UIType = uiTypeId
	RequestProcessor.Tooltip:InvokeRequestListeners(request, "After", ui, uiTypeId, event, id, objectHandle)
end

---@param e EclLuaUICallEvent
---@param ui UIObject
---@param event string
---@param id number
---@vararg any
function RequestProcessor.OnGMStatusTooltip(e, ui, event, id, ...)
	---@type EclCharacter|EclItem|nil
	local object = nil

	object = _GetObjectFromHandle(ui:GetPlayerHandle())

	if not object then
		object = _GetGMTargetCharacter() or RequestProcessor.Utils.GetClientCharacter()
	end

	local request = _CreateRequest()

	if object then
		request.ObjectHandleDouble = _HandleToDouble(object.Handle)
	end

	request.Type = "Status"
	if not _IsNaN(id) then
		local statusHandle = _DoubleToHandle(id)
		if _IsValidHandle(statusHandle) then
			request.StatusHandleDouble = id
			if object then
				local status = _GetStatus(object.Handle, statusHandle)
				if status then
					request.StatusId = status.StatusId
				end
			end
		end
	end

	RequestProcessor.Tooltip.NextRequest = request
	RequestProcessor.Tooltip:InvokeRequestListeners(request, "Before", ui, ui.Type, event, id, ...)

	RequestProcessor.Tooltip.Last.Event = event
	RequestProcessor.Tooltip.Last.UIType = ui.Type
	RequestProcessor.Tooltip:InvokeRequestListeners(request, "After", ui, ui.Type, event, id, ...)
end

---@param e EclLuaUICallEvent
---@param ui UIObject
---@param event string
---@param text string
---@param x number
---@param y number
---@param width number
---@param height number
---@param side string
---@param allowDelay boolean
function RequestProcessor.OnGenericTooltip(e, ui, event, text, x, y, width, height, side, allowDelay)
	if RequestProcessor.Tooltip.NextRequest == nil and text ~= nil then
		---@type TooltipGenericRequest
		local request = _CreateRequest()
		request.Type = "Generic"
		request.Text = text
		request.UIType = ui.Type

		local hasExtraData = x ~= nil and side ~= nil

		if hasExtraData then
			request.X = x
			request.Y = y
			request.Width = width
			request.Height = height
			request.Side = side
			request.AllowDelay = allowDelay
		end

		RequestProcessor.Tooltip.NextRequest = request
		RequestProcessor.Tooltip:InvokeRequestListeners(request, "Before", ui, request.UIType, event, text, x, y, width, height, side, allowDelay)

		RequestProcessor.Tooltip.Last.Event = event
		RequestProcessor.Tooltip.Last.UIType = request.UIType
		RequestProcessor.Tooltip:InvokeRequestListeners(request, "After", ui, request.UIType, event, text, x, y, width, height, side, allowDelay)

		if not _IsNullOrEmpty(request.Text) then
			e.Args[1] = request.Text
		else
			_PrintError(string.format("[RequestProcessor.OnGenericTooltip:%s] request.Text is nil?\nRequest:\n%s\nArgs:\n%s", event, Ext.DumpExport(request), Ext.DumpExport(e.Args)))
			RequestProcessor.Tooltip.NextRequest = nil
			-- e.Args[1] = ""
			-- request.Text = ""
			return false
		end

		if hasExtraData then
			e.Args[2] = request.X or x
			e.Args[3] = request.Y or y
			e.Args[4] = request.Width or width
			e.Args[5] = request.Height or height
			e.Args[6] = request.Side or side
			if request.AllowDelay == nil then
				request.AllowDelay = allowDelay == true
			end
			e.Args[7] = request.AllowDelay
		end
	end
end

function RequestProcessor.SetWorldTooltipRequest(request, ui, uiType, event, text, x, y, isItem, item)
	request.Type = "World"
	request.Text = text
	request.IsFromItem = false
	if isItem and item then
		request.ObjectHandleDouble = _HandleToDouble(item.Handle)
		request.IsFromItem = true
	end
	return request
end

---@param request TooltipRequest
---@param ui UIObject
---@param uiType integer
---@param event string
---@param text string
---@param levelText string
---@param shortenWidth boolean
---@param item EclItem
---@param objectHandleDouble number
function RequestProcessor.SetWorldHoverTooltipRequest(request, ui, uiType, event, text, levelText, shortenWidth, item, objectHandleDouble)
	request.Type = "WorldHover"
	request.Text = text
	request.IsFromItem = item ~= nil
	request.ObjectHandleDouble = objectHandleDouble
	return request
end

---@param e EclLuaUICallEvent
---@param requestType string
---@param ui UIObject
---@param uiType integer
---@param event string Call or method.
---@param idOrDoubleHandle any
---@param statOrWidth any
---@vararg any
function RequestProcessor.HandleCallback(e, requestType, ui, uiType, event, idOrDoubleHandle, statOrWidth, ...)
	---@type {characterHandle:number|nil}
	local this = ui:GetRoot()

	local character = _GetObjectFromHandle(ui:GetPlayerHandle())
	local id = idOrDoubleHandle

	if (event == "showSkillTooltip" or event == "showStatusTooltip") then
		id = statOrWidth
		if not character then
			character = _GetObjectFromDouble(idOrDoubleHandle)
		end
	end

	if not character and event == "showSkillTooltip" and _GetGameMode() == "GameMaster" then
		character = _GetGMTargetCharacter()
	end

	if not character and this and this.characterHandle then
		character = _GetObjectFromDouble(this.characterHandle)
	end

	if not character then
		if (uiType == _UITYPE.characterSheet or uiType == _UITYPE.statsPanel_c) then
			character = _GetCharacterSheetCharacter(this)
		elseif (uiType == _UITYPE.playerInfo or uiType == _UITYPE.playerInfo_c) then
			--[[ Help! This shouldn't ever happen because the character handle is passed into the external call.
			We have no idea which row of statuses/whatever is being looked at for a character otherwise.--]]
		end
	end

	if not character then
		character = RequestProcessor.Utils.GetClientCharacter()
	end

	if uiType == _UITYPE.characterCreation then
		id = statOrWidth
	end

	local request = _CreateRequest()
	request.Type = requestType
	if character then
		request.ObjectHandleDouble = _HandleToDouble(character.Handle)
	end
	request.UIType = uiType

	RequestProcessor.Tooltip.NextRequest = request

	RequestProcessor.Tooltip:InvokeRequestListeners(request, "Before", ui, uiType, event, id, statOrWidth, ...)
	if RequestProcessor.CallbackHandler[event] then
		local b,r = xpcall(RequestProcessor.CallbackHandler[event], debug.traceback, request, ui, uiType, event, id, statOrWidth, ...)
		if b then
			RequestProcessor.Tooltip.NextRequest = r
			request = r
		else
			_PrintError(string.format("[Game.Tooltips.RequestProcessor] Error invoking tooltip handler for event (%s):\n%s", event, r))
		end
	end

	if RequestProcessor.ControllerEnabled and character then
		Game.Tooltip.ControllerVars.LastPlayer = _HandleToDouble(character.Handle)
	end
	RequestProcessor.Tooltip.Last.Event = event
	RequestProcessor.Tooltip.Last.UIType = uiType

	RequestProcessor.Tooltip:InvokeRequestListeners(request, "After", ui, uiType, event, id, statOrWidth, ...)
end

local function OnControllerSlotOver(e, ui, event, id, ...)
	if id ~= 0 then
		RequestProcessor.HandleCallback(e, "Item", ui, ui.Type, TooltipCalls.Item, id, ...)
	end
end

local function RedirectControllerTooltip(e, tooltipType, ui, event, ...)
	--Redirect to standard events, so the regular handlers work
	local event = TooltipCalls[tooltipType] or event
	RequestProcessor.HandleCallback(e, tooltipType, ui, ui.Type, event, ...)
end

---@type table<string, {Callback:function, TypeIds:table<integer,boolean>|nil}>
local CallHandlers = {}
local InvokeHandlers = {}

---@param event string
---@param callback fun(e:EclLuaUICallEvent, ui:UIObject, event:string, ...:any)
---@param typeIds table<integer,boolean>|integer|nil
local function _CallHandler(event, callback, typeIds)
	if type(typeIds) == "number" then
		local t = typeIds
		typeIds = {[t] = true}
	end
	CallHandlers[event] = {
		Callback = callback,
		TypeIds = typeIds
	}
end

---@param event string
---@param callback fun(e:EclLuaUICallEvent, ui:UIObject, event:string, ...:any)
---@param typeIds table<integer,boolean>|integer|nil
local function _InvokeHandler(event, callback, typeIds)
	if type(typeIds) == "number" then
		local t = typeIds
		typeIds = {[t] = true}
	end
	InvokeHandlers[event] = {
		Callback = callback,
		TypeIds = typeIds
	}
end

Ext.Events.UIInvoke:Subscribe(function (e)
	if e.When == "Before" then
		local handler = InvokeHandlers[e.Function]
		if handler then
			if handler.TypeIds then
				if handler.TypeIds[e.UI.Type] == true then
					handler.Callback(e, e.UI, e.Function, table.unpack(e.Args))
				end
			else
				handler.Callback(e, e.UI, e.Function, table.unpack(e.Args))
			end
		end
	end
end, {Priority=1})

local _lastShowStatusTooltip = -1

Ext.Events.UICall:Subscribe(function (e)
	if e.When == "Before" then
		local typeID = e.UI.Type
		if e.Function == "showStatusTooltip" and typeID == 38 then -- Data.UIType.playerInfo
			--Fix for ticking statuses causing the last element in a tooltip array to be duplicated, if a new value was added in a tooltip listener
			if _lastShowStatusTooltip >= 0 then
				if _MonotonicTime() - _lastShowStatusTooltip <= 33 then
					e:StopPropagation()
					e:PreventAction() -- The necessary part
					return
				end
			end
			_lastShowStatusTooltip = _MonotonicTime()
		end
		local handler = CallHandlers[e.Function]
		if handler then
			if handler.TypeIds then
				if handler.TypeIds[typeID] == true then
					handler.Callback(e, e.UI, e.Function, table.unpack(e.Args))
				end
			else
				handler.Callback(e, e.UI, e.Function, table.unpack(e.Args))
			end
		end
	end
end, {Priority=1})

local function RegisterControllerHandlers()
	local _ccControllerTypeIds = {[_UITYPE.characterCreation_c]=true}
	for t,v in pairs(ControllerCharacterCreationCalls) do
		_CallHandler(v, function (e, ...)
			RedirectControllerTooltip(e, t, ...)
		end, _ccControllerTypeIds)
	end
	--Custom controller tooltip calls.
	_CallHandler("SlotHover", function (e, ui, event, slotNum)
		---@type {bottombar_mc:{slotsHolder_mc:{tooltipSlotType:integer, tooltipSlot:number, tooltipStr:string, slot_array:FlashArray}}}
		local this = ui:GetRoot()
		if this then
			local slotsHolder_mc = this.bottombar_mc.slotsHolder_mc
			local slotType = slotsHolder_mc.tooltipSlotType
			local slotHandle = slotsHolder_mc.tooltipSlot
	
			local requestType = "Skill"
			local id = nil
			-- 4 is for non-skills like Flee, Sheathe etc
			if slotType == 1 or slotType == 4 then
				event = TooltipCalls.Skill
				RequestProcessor.HandleCallback(e, requestType, ui, ui.Type, event, nil, slotsHolder_mc.tooltipStr)
			elseif slotType == 2 then
				-- Sometimes tooltipSlot will be set to the tooltip index instead of the slot's handle value
				if slotNum == slotHandle then
					local slot = slotsHolder_mc.slot_array[slotNum]
					if slot then
						slotHandle = slot.handle
					end
				end
				if slotHandle and slotNum ~= slotHandle then
					local handle = _DoubleToHandle(slotHandle)
					if _IsValidHandle(handle) then
						requestType = "Item"
						event = TooltipCalls.Item
						RequestProcessor.HandleCallback(e, requestType, ui, ui.Type, event, slotHandle)
					end
				end
			end
		end
	end, _UITYPE.bottomBar_c)

	local slotOverTypeIds = {
		[_UITYPE.equipmentPanel_c] = true,
		[_UITYPE.craftPanel_c] = true,
		[_UITYPE.partyInventory_c] = true,
		[_UITYPE.containerInventory.Default] = true,
		[_UITYPE.containerInventory.Pickpocket] = true,
	}

	-- slotOver is called when selecting any slot, item or not
	_CallHandler("slotOver", OnControllerSlotOver, slotOverTypeIds)

	_CallHandler("itemDollOver", OnControllerSlotOver, _UITYPE.equipmentPanel_c)

	_CallHandler("runeSlotOver", function (e, ui, event, id, ...)
		if id ~= -1 then
			RequestProcessor.HandleCallback(e, "Rune", ui, ui.Type, TooltipCalls.Rune, id, ...)
		end
	end, _UITYPE.craftPanel_c)

	_CallHandler("setTooltipVisible", function (e, ui, event, id, ...)
		if id ~= -1 then
			RequestProcessor.HandleCallback(e, "Rune", ui, ui.Type, TooltipCalls.Rune, id, ...)
		end
	end, _UITYPE.craftPanel_c)

	_CallHandler("setTooltipVisible", function (e, ui, event, visible, ...)
		if visible == true then
			RequestProcessor.HandleCallback(e, "Item", ui, ui.Type, TooltipCalls.Item)
		end
	end, _UITYPE.partyInventory_c)

	_CallHandler("overItem", function (e, ui, event, itemHandleDouble, ...)
		RequestProcessor.HandleCallback(e, "Item", ui, ui.Type, TooltipCalls.Item, itemHandleDouble)
	end, _UITYPE.trade_c)

	_CallHandler("refreshTooltip", function (e, ui, event, itemHandleDouble, ...)
		RequestProcessor.HandleCallback(e, "Item", ui, ui.Type, TooltipCalls.Item, itemHandleDouble)
	end, _UITYPE.reward_c)

	_CallHandler("selectedAttribute", function (e, ui, event, id, ...)
		RequestProcessor.OnControllerExamineTooltip(e, ui, event, id)
	end, _UITYPE.statsPanel_c)

	_CallHandler("selectCustomStat", function (e, ui, event, id, ...)
		RequestProcessor.OnControllerExamineTooltip(e, ui, event, id)
	end, _UITYPE.statsPanel_c)

	local selectEvents = {
		"selectStat",
		"selectAbility",
		"selectAbility",
		"selectTalent",
		"selectStatus",
		"selectTitle",
	}

	local selectTypeIds = {
		[_UITYPE.statsPanel_c] = true,
		[_UITYPE.examine_c] = true,
	}

	for i=1,#selectEvents do
		local v = selectEvents[i]
		_CallHandler(v, function (e, ui, ...)
			RequestProcessor.OnControllerExamineTooltip(e, ui, ...)
		end, selectTypeIds)
	end
end

---@param tooltip TooltipHooks
function RequestProcessor:Init(tooltip)
	self.Tooltip = tooltip
	for t,v in pairs(TooltipCalls) do
		_CallHandler(v, function (e, ui, ...) RequestProcessor.HandleCallback(e, t, ui, ui.Type, ...) end)
	end
	for t,v in pairs(TooltipInvokes) do
		_InvokeHandler(v, function (e, ui, ...) RequestProcessor.HandleCallback(e, t, ui, ui.Type, ...) end)
	end

	_CallHandler("showTooltip", function (e, ui, ...)
		if ui.Type == _UITYPE.examine then
			RequestProcessor.OnExamineTooltip(e, ui, ...)
		elseif ui.Type == _UITYPE.statusPanel then -- GM
			RequestProcessor.OnGMStatusTooltip(e, ui, ...)
		else
			RequestProcessor.OnGenericTooltip(e, ui, ...)
		end
	end)

	if RequestProcessor.ControllerEnabled then
		RegisterControllerHandlers()
	end

	-- Disabled for now since this function doesn't include any ID for the tag.
	-- Ext.RegisterUICall(statsPanel, "selectTag", function(e, ui, method, emptyWorthlessTagTooltip)
	-- 	print(method, emptyWorthlessTagTooltip)
	-- 	local main = ui:GetRoot()
	-- 	local tags_mc = main.mainpanel_mc.stats_mc.tags_mc
	-- 	local selectedTag = tags_mc.statList.m_CurrentSelection
	-- 	if selectedTag ~= nil then
	-- 		local tagNameText = selectedTag.label_txt.htmlText
	-- 		self:OnRequestConsoleExamineTooltip(ui, method, tagNameText)
	-- 	end
	-- end)
end

local UNSET_HANDLE = "ls::TranslatedStringRepository::s_HandleUnknown"

local function _GetTranslatedStringValue(ts)
	local refString = ts.Handle and ts.Handle.ReferenceString or ""
	if refString == "" and ts.ArgumentString then
		refString = ts.ArgumentString.ReferenceString
	end
	if ts.Handle and ts.Handle.Handle ~= UNSET_HANDLE then
		return _GetTranslatedString(ts.Handle.Handle, ts.Handle.ReferenceString)
	end
	return refString
end

---@param item EclItem
local function _GetItemDisplayName(item)
	local statsId = nil
	if item.StatsId ~= "" and item.StatsId ~= nil and not Ext.Enums.ItemDataRarity[item.StatsId] then
		statsId = item.StatsId
	end
	if string.find(item.DisplayName, "|") or item.CurrentTemplate.DisplayName.Handle == nil or item.DisplayName == statsId then
		if statsId then
			local name = _GetTranslatedStringFromKey(item.StatsId)
			if name ~= nil and name ~= "" then
				return name
			end
		end
		local translatedName = _GetTranslatedStringValue(item.CurrentTemplate.DisplayName)
		if translatedName ~= nil and translatedName ~= "" then
			return translatedName
		end
	end
	return item.DisplayName
end

local _SlotNames = {
	Helmet = {"hd4b98ff5g33a8g44e0ga6a9gdb1ab7d70bf3", "Helmet"},
	Breast = {"hb5c52d20g6855g4929ga78ege3fe776a1f2e", "Chest Armour"},
	Leggings = {"he7042b52g54d7g4f46g8f69g509460dfe595", "Leggings"},
	Weapon = {"h102d1ef8g3757g4ff3g8ef2gd68007c6268d", "Weapon"},
	Shield = {"h77557ac7g4f6fg49bdga76cg404de43d92f5", "Shield"},
	Ring = {"h970199f8ge650g4fa3ga0deg5995696569b6", "Ring"},
	Belt = {"h2a76a9ecg2982g4c7bgb66fgbe707db0ac9e", "Belt"},
	Boots = {"h9b65aab2gf4c4g4b81g96e6g1dcf7ffa8306", "Boots"},
	Gloves = {"h185545eagdaf0g4286ga411gd50cbdcabc8b", "Gloves"},
	Amulet = {"hb9d79ca5g59afg4255g9cdbgf614b894be68", "Amulet"},
	Ring2 = {"h970199f8ge650g4fa3ga0deg5995696569b6", "Ring"},
	Wings = {"hd716a074gd36ag4dfcgbf79g53bd390dd202", "Wings"},
	Horns = {"ha35fc503g56dbg4adag963dga359d961e0c8", "Horns"},
	Overhead = {"hda749a3fg52c0g48d5gae3bgd522dd34f65c", "Overhead"},
	Offhand = {"h50110389gc98ag49dbgb58fgae2fd227dff4", "Offhand"},
}

---@param item EclItem
local function _GetItemSlotName(item)
	if item.StatsFromName then
		if item.StatsFromName.ModifierListIndex < 3 then
			---@type StatEntryArmor|StatEntryShield|StatEntryWeapon
			local stat = item.StatsFromName.StatsEntry
			local tsData = _SlotNames[stat.Slot]
			if tsData then
				return _GetTranslatedString(tsData[1], tsData[2])
			end
		end
	elseif not _IsNullOrEmpty(item.StatsId) and not Ext.Enums.ItemDataRarity[item.StatsId] then
		local stat = _GetStat(item.StatsId)
		if stat and stat.Slot then
			local tsData = _SlotNames[stat.Slot]
			if tsData then
				return _GetTranslatedString(tsData[1], tsData[2])
			end
		end
	end
	return nil
end

local _equipmentPattern = "<font color=\"#ffffff\">%s</font><font size=\"15\"><br>%s</font>"

--Called before a world hover tooltip is shown. text may be "" if it's an item without health.
--[enemyHealthBar(42)][invoke] setText("<font color="#ffffff">Barrel</font>", "Level 1", false)
--TODO Figure out if there's an equivalent for controllers.
_InvokeHandler("setText", function(e, ui, event, text, levelText, shortenWidth)
	local cursor = _GetPickingState()
	if cursor and _IsValidHandle(cursor.HoverItem) then
		local item = _GetItem(cursor.HoverItem)
		---@cast item EclItem
		if item and item.CurrentTemplate and item.CurrentTemplate.Tooltip > 0 then
			local objectHandleDouble = _HandleToDouble(cursor.HoverItem)
			local request = _CreateRequest()
			if text == nil or text == "" then
				local name = _GetItemDisplayName(item)
				local slotName = _GetItemSlotName(item)
				if slotName then
					text = _equipmentPattern:format(name, slotName)
				else
					text = name
				end
			end
			RequestProcessor.Tooltip.NextRequest = RequestProcessor.SetWorldHoverTooltipRequest(request, ui, _UITYPE.enemyHealthBar, event, text, levelText, shortenWidth, item, objectHandleDouble)
		end
	end
end, _UITYPE.enemyHealthBar)

---@return string
local function _CreateWorldTooltipRequest(ui, event, text, x, y, isItem, item)
	local uiType = ui.Type
	---@type TooltipWorldRequest
	local request = _CreateRequest()
	RequestProcessor.Tooltip:InvokeRequestListeners(request, "Before", ui, uiType, event, text, x, y, isItem, item)
	local b,r = xpcall(RequestProcessor.SetWorldTooltipRequest, debug.traceback, request, ui, uiType, event, text, x, y, isItem, item)
	if b then
		request = r
		RequestProcessor.Tooltip.NextRequest = request
	else
		_PrintError(string.format("[Game.Tooltip.RequestProcessor:_CreateWorldTooltipRequest] Error invoking tooltip handler for event (%s):\n%s", event, r))
	end

	RequestProcessor.Tooltip.Last.Event = event
	RequestProcessor.Tooltip.Last.UIType = uiType
	RequestProcessor.Tooltip.Last.Request = request
	
	RequestProcessor.Tooltip:InvokeRequestListeners(request, "After", ui, uiType, event, text, x, y, isItem, item)
	
	local tooltipData = Game.Tooltip.TooltipData:Create({{
		Type = "GenericDescription",
		Label = text,
		X = x,
		Y = y,
		AllowDelay = false,
		AnchorEnum = 0,
		BackgroundType = 0,
	}}, uiType, uiType)
	
	RequestProcessor.Tooltip.ActiveType = request.Type
	RequestProcessor.Tooltip:NotifyListeners("World", nil, request, tooltipData, request.Item)

	local desc = tooltipData:GetDescriptionElement()
	return desc and desc.Label or nil
end

_INTERNAL.CreateWorldTooltipRequest = _CreateWorldTooltipRequest

_InvokeHandler("updateTooltips", function(e, ui, event)
	---@type {worldTooltip_array:table<integer,number|string|boolean>}
	local this = ui:GetRoot()
	if this then
		--public function setTooltip(param1:uint, param2:Number, param3:Number, param4:Number, param5:String, param6:Number, param7:Boolean, param8:uint = 16777215, param9:uint = 0
		--this.setTooltip(val2,val3,val4,val5,val6,this.worldTooltip_array[val2++],this.worldTooltip_array[val2++]);
		for i=0,#this.worldTooltip_array-1,6 do
			---@type number
			local doubleHandle = this.worldTooltip_array[i]
			if doubleHandle then
				local x = this.worldTooltip_array[i+1]
				local y = this.worldTooltip_array[i+2]
				local text = this.worldTooltip_array[i+3]
				--local sortHelper = main.worldTooltip_array[i+4]
				local isItem = this.worldTooltip_array[i+5]
				if isItem then
					local item = _GetObjectFromDouble(doubleHandle)
					if item then
						local textReplacement = _CreateWorldTooltipRequest(ui, "updateTooltips", text, x, y, true, item)
						if textReplacement and textReplacement ~= text then
							this.worldTooltip_array[i+3] = textReplacement
						end
					end
				else
					local textReplacement = _CreateWorldTooltipRequest(ui, "updateTooltips", text, x, y, false, nil)
					if textReplacement and textReplacement ~= text then
						this.worldTooltip_array[i+3] = textReplacement
					end
				end
			end
		end
	end
end, _UITYPE.worldTooltip)

--Hack to clear the last tooltip being "World"
_InvokeHandler("removeTooltip", function(e, ui, ...)
	local lastRequest = RequestProcessor.Tooltip.Last.Request
	if lastRequest and lastRequest.Type == "World" then
		Game.Tooltip.TooltipHooks.Last.Request = nil
	end
end)

---@class GameTooltipRequestProcessorUtils
---@field GetClientCharacter fun():EclCharacter|nil
RequestProcessor.Utils = {
	GetObjectFromDouble = _GetObjectFromDouble,
	GetObjectFromHandle = _GetObjectFromHandle,
	GetGMTargetCharacter = _GetGMTargetCharacter,
	GetCharacterSheetCharacter = _GetCharacterSheetCharacter,
	GetSurfaces = _GetSurfaces
}

return RequestProcessor
