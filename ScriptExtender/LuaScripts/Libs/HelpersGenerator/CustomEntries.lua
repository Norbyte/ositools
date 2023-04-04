---@diagnostic disable

local SubscribableEventType = [[--- @class SubscribableEvent<T>:{ (Subscribe:fun(self:SubscribableEvent, callback:fun(e:T|LuaEventBase), opts:{Priority:integer, Once:boolean}|nil):integer), (Unsubscribe:fun(self:SubscribableEvent, index:integer))}

--- Developer functions for the SubscribableEvent type. 
--- Throw can be used to manually throw the event, but special care may be needed to ensure the table used for the event data is valid.  
--- @class SubscribableEventDev<T>:{ (Throw:fun(self:SubscribableEvent, e:T|LuaEventBase))}
]]
return {
	Specific = {SubscribableEventType = SubscribableEventType},
	Misc = {
[[--#region Extender Functions / Globals

--- @alias NetListenerCallback fun(channel:string, payload:string, user:UserId|nil)

--- Registers a listener that is called when a network message is received on the specified channel
--- @param channel string Network channel name
--- @param handler NetListenerCallback Lua handler
function Ext.RegisterNetListener(channel, handler) end

--- Loads the specified Lua file
--- @param fileName string|nil Path of Lua file, relative to Mods/<Mod>/Story/RawFiles/Lua
--- @see Ext_Utils#Include
--- @return any
function Ext.Require(fileName) end

--- Returns whether the code is executing in a client context
--- @return boolean
function Ext.IsClient() end

--- Returns whether the code is executing in a server context
--- @return boolean
function Ext.IsServer() end

--- Console window shortcut for Ext.Dump
_D = Ext.Dump

--- Console window shortcut for Ext.Utils.Print
_P = Ext.Utils.Print

--- Console window helper to get current player character 
--- This is the host on the server, or the hotbar character on the client  
--- @return EsvCharacter|EclCharacter
_C = function() end

--- Console window helper to get character being examined on the client-side  
--- @return EclCharacter
_E = function() end

--- Console window helper to get the host's equipped weapon on the server-side  
--- @return EsvItem
_W = function() end

--- Helper for dumping variables to the console  
--- This is essentially `Ext.Utils.Print(Ext.DumpExport(val))`  
--- @param val any
function Ext.Dump(val) end

--- Helper for dumping variables to a string
--- @param val any
--- @return string
function Ext.DumpExport(val) end

--- Register a callback that runs on the next tick, and is then unregistered afterwards  
--- @param callback fun(e:LuaTickEventParams)
function Ext.OnNextTick(callback) end

--- @class CustomSkillProperty
--- @field GetDescription fun(property:StatsPropertyExtender):string|nil
--- @field ExecuteOnPosition fun(property:StatsPropertyExtender, attacker: EsvCharacter|EsvItem, position: vec3, areaRadius: number, isFromItem: boolean, skill: StatsSkillPrototype|nil, hit: StatsHitDamageInfo|nil)
--- @field ExecuteOnTarget fun(property:StatsPropertyExtender, attacker: EsvCharacter|EsvItem, target: EsvCharacter|EsvItem, position: vec3, isFromItem: boolean, skill: StatsSkillPrototype|nil, hit: StatsHitDamageInfo|nil)

--- Registers a new skill property that can be triggered via SkillProperties
--- Stat syntax: data"SkillProperties""EXT:<PROPERTY_NAME>[,<int>,<int>,<string>,<int>,<int>]"
--- The property name must always be preceded by the string "EXT:". 
--- Target contexts (SELF:, TARGET:, ...) and useing multiple actions in the same SkillProperties are supported
--- Conditions for EXT: properties (i.e. "IF(COND):") are _NOT YET_ supported
--- @param name string Skill property name
--- @param defn CustomSkillProperty Event handlers for the skill property
function Ext.RegisterSkillProperty(name, defn) end

--- @alias UICallbackHandler fun(ui:UIObject, event:string, ...:string|boolean|number)
--- @alias UICallbackEventType "Before"|"After"

--- Registers a listener that is called when the specified function is called from Flash
--- @param object UIObject UI object returned from Ext.CreateUI, Ext.GetUI or Ext.GetBuiltinUI
--- @param name string ExternalInterface function name
--- @param handler UICallbackHandler Lua handler
--- @param type UICallbackEventType|nil Event type - 'Before' or 'After'
function Ext.RegisterUICall(object, name, handler, type) end

--- Registers a listener that is called when the specified function is called from Flash
--- The event is triggered for every UI element with the specified type ID
--- @param typeId number Engine UI element type ID
--- @param name string ExternalInterface function name
--- @param handler UICallbackHandler Lua handler
--- @param type UICallbackEventType|nil Event type - 'Before' or 'After'
function Ext.RegisterUITypeCall(typeId, name, handler, type) end

--- Registers a listener that is called when the specified function is called from Flash
--- The event is triggered regardless of which UI element it was called on
--- (Function call capture must be enabled for every element type that needs to monitored!)
--- @param name string ExternalInterface function name
--- @param handler UICallbackHandler Lua handler
--- @param type UICallbackEventType|nil Event type - 'Before' or 'After'
function Ext.RegisterUINameCall(name, handler, type) end

--- Registers a listener that is called when the specified method is called on the main timeline of the Flash object
--- @param object UIObject UI object returned from Ext.CreateUI, Ext.GetUI or Ext.GetBuiltinUI
--- @param name string Flash method name
--- @param handler UICallbackHandler Lua handler
--- @param type UICallbackEventType|nil Event type - 'Before' or 'After'
function Ext.RegisterUIInvokeListener(object, name, handler, type) end

--- Registers a listener that is called when the specified method is called on the main timeline of the Flash object
--- The event is triggered for every UI element with the specified type ID
--- @param typeId number Engine UI element type ID
--- @param name string Flash method name
--- @param handler UICallbackHandler Lua handler
--- @param type UICallbackEventType|nil Event type - 'Before' or 'After'
function Ext.RegisterUITypeInvokeListener(typeId, name, handler, type) end

--- Registers a listener that is called when the specified method is called on the main timeline of the Flash object
--- The event is triggered regardless of which UI element it was called on
--- @param name string Flash method name
--- @param handler UICallbackHandler Lua handler
--- @param type UICallbackEventType|nil Event type - 'Before' or 'After'
function Ext.RegisterUINameInvokeListener(name, handler, type) end

--- Registers a listener that is called when a console command is entered in the dev console
--- @param cmd string Console command
--- @param handler fun(cmd:string, ...:string)
function Ext.RegisterConsoleCommand(cmd, handler) end

--- Prepares a new surface action for execution
--- @param type string Surface action type
--- @return EsvSurfaceAction
function Ext.CreateSurfaceAction(type) end

--- Executes a surface action
--- @param action EsvSurfaceAction Action to execute
function Ext.ExecuteSurfaceAction(action) end

--- @deprecated
--- Cancels a surface action
--- @param actionHandle integer Action to cancel
function Ext.CancelSurfaceAction(actionHandle) end

--- @class ItemDefinition
--- @field RootTemplate FixedString
--- @field OriginalRootTemplate FixedString
--- @field Slot integer
--- @field Amount integer
--- @field GoldValueOverwrite integer
--- @field WeightValueOverwrite integer
--- @field DamageTypeOverwrite StatsDamageType
--- @field ItemType ItemDataRarity
--- @field CustomDisplayName string
--- @field CustomDescription string
--- @field CustomBookContent string
--- @field GenerationStatsId FixedString
--- @field GenerationItemType ItemDataRarity
--- @field GenerationRandom integer
--- @field GenerationLevel integer
--- @field StatsLevel integer
--- @field Key string
--- @field LockLevel integer
--- @field EquipmentStatsType integer
--- @field HasModifiedSkills boolean
--- @field Skills string
--- @field HasGeneratedStats boolean
--- @field IsIdentified boolean
--- @field GMFolding boolean
--- @field CanUseRemotely boolean
--- @field GenerationBoosts FixedString[]
--- @field RuneBoosts FixedString[]
--- @field DeltaMods FixedString[]
local ItemDefinition = {}

--- Clears item progression data (name group, level group, etc.)
function ItemDefinition:ResetProgression() end

--- @alias ItemConstructorDefinitionAccessor ItemDefinition[]

--- Creates an item based on a parsed item or newly created item definition.
--- Should be initialized using Ext.CreateItemConstructor(item/template) first.
--- Item definitions can be accessed using c[1], c[2], etc.
--- For non-recursive item cloning (i.e. creating a single item), there is only one item (c[1]).
--- For container cloning, the contained items are accessible using c[2], c[3], etc.
--- @class ItemConstructorBase
local ItemConstructorBase = {}

--- Constructs an instance of the item contained in the constructor definition.
--- The definition is cleared after the item is created.
--- @return EsvItem|nil
function ItemConstructorBase:Construct() end

--- @alias ItemConstructor ItemConstructorBase|ItemConstructorDefinitionAccessor

--- Starts creating a new item using template UUID or cloning an existing item
--- @param from EsvItem|string Template UUID or item to clone
--- @param recursive boolean|nil Copy items in container? (cloning only)
--- @return ItemConstructor
function Ext.CreateItemConstructor(from, recursive) end

--- Begin applying a status on the specified character or item
--- @param target string|ComponentHandle Target character/item
--- @param statusId string Status ID to apply
--- @param lifeTime number Status lifetime (-1 = infinite, -2 = keep alive)
--- @return EsvStatus|nil
function Ext.PrepareStatus(target, statusId, lifeTime) end

--- Finish applying a status on the specified character or item
--- @param status EsvStatus Status to apply
function Ext.ApplyStatus(status) end

--- Returns the current client/server game state machine state
--- @return EsvGameState
function Ext.GetGameState() end

--#endregion
]],
},

}