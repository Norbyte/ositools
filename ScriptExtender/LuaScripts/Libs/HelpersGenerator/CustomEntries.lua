---@diagnostic disable

local SubscribableEventType = [[--- @class SubscribableEvent<T>:{ (Subscribe:fun(self:SubscribableEvent, callback:fun(e:T|SubscribableEventParams), opts:{Priority:integer, Once:boolean}|nil):integer), (Unsubscribe:fun(self:SubscribableEvent, index:integer))}

--- @class SubscribableEventParams
--- @field StopPropagation fun(self:SubscribableEventParams) Stop the event from continuing on to other registered listeners.]]

return {
	Specific = {SubscribableEventType = SubscribableEventType},
	Misc = {
[[
--#region Deprecated Functions (moved to Ext modules)

--- @deprecated
--- Returns the version number of the Osiris Extender
--- @return integer
function Ext.Version() end

--- @deprecated
--- Returns the version number of the game
--- @return string
function Ext.GameVersion() end

--- @deprecated
--- Loads the specified Lua file
--- @param fileName string|nil Path of Lua file, relative to Mods/<Mod>/Story/RawFiles/Lua
--- @see Ext_Utils#Include
function Ext.Require(fileName) end

--- @alias ExtEngineEvent string|"SessionLoading"|"SessionLoaded"|"ModuleLoading"|"ModuleLoadStarted"|"ModuleResume"|"StatsLoaded"|"GameStateChanged"|"SkillGetDescriptionParam"|"StatusGetDescriptionParam"|"GetSkillDamage"|"GetSkillAPCost"|"ComputeCharacterHit"|"CalculateTurnOrder"|"GetHitChance"|"StatusGetEnterChance"|"StatusHitEnter"|"BeforeCharacterApplyDamage"|"UIInvoke"|"UICall"|"AfterUIInvoke"|"AfterUICall"|"BeforeShootProjectile"|"ShootProjectile"|"ProjectileHit"|"GroundHit"|"InputEvent"|"TreasureItemGenerated"

--- @deprecated
--- Registers a function to call when an extender event is thrown
--- @param event ExtEngineEvent Event to listen for
--- @param callback function Lua function to run when the event fires
function Ext.RegisterListener(event, callback) end

--- @alias OsirisEventType string|"before"|"after"|"beforeDelete"|"afterDelete"

--- @deprecated
--- Registers a function that is called when certain Osiris functions are called.
--- Supports events, built-in queries, DBs, PROCs, QRYs (user queries).
--- @param name string Osiris function/database name
--- @param arity number Number of columns for DBs or the number of parameters (both IN and OUT) for functions
--- @param event OsirisEventType Event type ('before' - triggered before Osiris call; 'after' - after Osiris call; 'beforeDelete'/'afterDelete' - before/after delete from DB)
--- @param handler function Lua function to run when the event fires
function Ext.RegisterOsirisListener(name, arity, event, handler) end

--- @deprecated
--- Registers a new call in Osiris
--- @param func function Function to register
--- @param funcName string Name of call to register
--- @param arguments string Call argument list
function Ext.NewCall(func, funcName, arguments) end

--- @deprecated
--- Registers a new query in Osiris
--- @param func function Function to register
--- @param funcName string Name of query to register
--- @param arguments string Query argument list
function Ext.NewQuery(func, funcName, arguments) end

--- @deprecated
--- Registers a new event in Osiris
--- @param funcName string Name of event to register
--- @param arguments string Event argument list
function Ext.NewEvent(funcName, arguments) end

--- @deprecated
--- Print to console window and editor messages pane
--- @vararg any
function Ext.Print(...) end

--- @deprecated
--- Print warning to console window and editor messages pane
--- @vararg any
function Ext.PrintWarning(...) end

--- @deprecated
--- Print error to console window and editor messages pane
--- @vararg any
function Ext.PrintError(...) end

--- @deprecated
--- Parse a JSON document into a Lua object
--- @param json string JSON string to parse
--- @return any
function Ext.JsonParse(json) end

--- @deprecated
--- Converts a Lua value into a JSON document
--- @param val any Value to serialize
--- @return string JSON document
function Ext.JsonStringify(val) end

--- @deprecated
--- Returns whether the specified mod is loaded
--- @param modGuid string UUID of the module
--- @return boolean
function Ext.IsModLoaded(modGuid) end

--- @deprecated
--- Returns the list of loaded modules in load order
--- @return string[]
function Ext.GetModLoadOrder() end

--- @deprecated
--- Returns detailed information about the specified (loaded) module
--- @param modGuid string UUID of the module
--- @return ModInfo
function Ext.GetModInfo(modGuid) end

--- @alias StatType string|"SkillData"|"Potion"|"StatusData"|"Weapon"|"Armor"|"Shield"|"DeltaMod"|"Object"

--- @deprecated
--- Returns the list of loaded stat entries
--- @param type string|nil Type of stat entry to fetch (StatusData, SkillData, ...)
--- @return string[]
function Ext.GetStatEntries(type) end

--- @deprecated
--- Returns the list of stat entries that were loaded before the specified mod
--- @param modId string Mod UUID to check
--- @param type string|nil Type of stat entry to fetch (StatusData, SkillData, ...)
--- @return string[]
function Ext.GetStatEntriesLoadedBefore(modId, type) end

--- @deprecated
--- Returns an attribute of the specified stat entry
--- @param stat string Stat entry name
--- @param attribute string Stat attribute name
--- @return any
function Ext.StatGetAttribute(stat, attribute) end

--- @deprecated
--- Updates an attribute of the specified stat entry
--- @param stat string Stat entry name
--- @param attribute string Stat attribute name
--- @param value any New stat value
function Ext.StatSetAttribute(stat, attribute, value) end

--- @deprecated
--- Adds a property description to the specified stat entry
--- @param stat string Stat entry name
--- @param attribute string Property list attribute name
--- @param description any Description to add
function Ext.StatAddCustomDescription(stat, attribute, description) end

--- @deprecated
--- Returns all skills from the specified skill set
--- @param name string Name of skill set entry
--- @return StatSkillSet|nil
function Ext.GetSkillSet(name) end

--- @deprecated
--- Updates all properties of the specified skill set.
--- The function expects a table in the same format as the one returned by GetSkillSet.
--- @param skillSet StatSkillSet
function Ext.UpdateSkillSet(skillSet) end

--- @deprecated
--- Returns all equipment from the specified equipment set
--- @param name string Name of equipment set entry
--- @return StatEquipmentSet|nil
function Ext.GetEquipmentSet(name) end

--- @deprecated
--- Updates all properties of the specified equipment set.
--- The function expects a table in the same format as the one returned by GetEquipmentSet.
--- @param equipmentSet StatEquipmentSet
function Ext.UpdateEquipmentSet(equipmentSet) end

--- @deprecated
--- Returns the specified DeltaMod or nil on failure
--- @param name string Name of delta mod
--- @param modifierType string Modifier type (Armor/Weapon)
--- @return DeltaMod
function Ext.GetDeltaMod(name, modifierType) end

--- @deprecated
--- Updates all properties of the specified DeltaMod.
--- The function expects a table in the same format as the one returned by GetDeltaMod.
--- @param deltaMod DeltaMod Name of delta mod
function Ext.UpdateDeltaMod(deltaMod) end

--- @deprecated
--- Returns the specified crafting item combination or nil on failure
--- @param name string Name of item combo
--- @return ItemCombo|nil
function Ext.GetItemCombo(name) end

--- @deprecated
--- Updates all properties of the specified item combination.
--- The function expects a table in the same format as the one returned by GetItemCombo.
--- @param itemCombo ItemCombo
function Ext.UpdateItemCombo(itemCombo) end

--- @deprecated
--- Returns the specified crafting preview data or nil on failure
--- @param name string Name of item combo preview data
--- @return ItemComboPreviewData|nil
function Ext.GetItemComboPreviewData(name) end

--- Updates all properties of the specified item combo preview.
--- The function expects a table in the same format as the one returned by GetItemComboPreviewData.
--- @param previewData ItemComboPreviewData
function Ext.UpdateItemComboPreviewData(previewData) end

--- @deprecated
--- Returns the specified crafting property or nil on failure
--- @param name string Name of item combo property
--- @return ItemComboProperty|nil
function Ext.GetItemComboProperty(name) end

--- @deprecated
--- Updates all properties of the specified item combo property.
--- The function expects a table in the same format as the one returned by GetItemComboProperty.
--- @param itemComboProperty ItemComboProperty
function Ext.UpdateItemComboProperty(itemComboProperty) end

--- @deprecated
--- Returns the specified treasure table or nil on failure
--- @param name string Name of treasure table
--- @return StatTreasureTable|nil
function Ext.GetTreasureTable(name) end

--- @deprecated
--- Updates all properties of the specified treasure table.
--- The function expects a table in the same format as the one returned by GetTreasureTable.
--- @param treasureTable StatTreasureTable
function Ext.UpdateTreasureTable(treasureTable) end

--- @deprecated
--- Returns the specified treasure category or nil on failure
--- @param name string Name of treasure category
--- @return StatTreasureCategory|nil
function Ext.GetTreasureCategory(name) end

--- @deprecated
--- Updates all properties of the specified treasure category.
--- The function expects a table in the same format as the one returned by GetTreasureCategory.
--- @param name string Name of treasure category
--- @param treasureCategory StatTreasureCategory
function Ext.UpdateTreasureCategory(name, treasureCategory) end

--- @deprecated
--- Returns the specified item progression item group or nil on failure
--- @param name string Name of item group
--- @return ItemGroup|nil
function Ext.GetItemGroup(name) end

--- @deprecated
--- Returns the specified item progression name group or nil on failure
--- @param name string Name of name group
--- @return ItemNameGroup|nil
function Ext.GetNameGroup(name) end

--- @class CustomSkillProperty
--- @field GetDescription fun(property:StatsPropertyExtender):string|nil
--- @field ExecuteOnPosition fun(property:StatsPropertyExtender, attacker: EsvCharacter|EsvItem, position: vec3, areaRadius: number, isFromItem: boolean, skill: StatEntrySkillData|nil, hit: StatsHitDamageInfo|nil)
--- @field ExecuteOnTarget fun(property:StatsPropertyExtender, attacker: EsvCharacter|EsvItem, target: EsvCharacter|EsvItem, position: vec3, isFromItem: boolean, skill: StatEntrySkillData|nil, hit: StatsHitDamageInfo|nil)

--- @deprecated
--- Registers a new skill property that can be triggered via SkillProperties
--- Stat syntax: data"SkillProperties""EXT:<PROPERTY_NAME>[,<int>,<int>,<string>,<int>,<int>]"
--- The property name must always be preceded by the string "EXT:". 
--- Target contexts (SELF:, TARGET:, ...) and useing multiple actions in the same SkillProperties are supported.
--- Conditions for EXT: properties (i.e. "IF(COND):") are _NOT YET_ supported.
--- @param name string Skill property name
--- @param defn CustomSkillProperty Event handlers for the skill property
function Ext.RegisterSkillProperty(name, defn) end

--- @deprecated
--- Replaces level scaling formula for the specified stat
--- @param statType string Stat entry type
--- @param attribute string Stat attribute name
--- @param func function Replacement scaling function
function Ext.StatSetLevelScaling(statType, attribute, func) end

--- @deprecated
--- Returns the property proxy of the specified stats entry
--- Returns level scaled values if the level parameter is not nil.
--- @param stat string Stat entry name
--- @param level integer|nil Level scaling level
--- @return StatEntryArmor|StatEntryCharacter|StatEntryObject|StatEntryPotion|StatEntryShield|StatEntrySkillData|StatEntryStatusData|StatEntryWeapon
function Ext.GetStat(stat, level) end

--- @alias StatEntryType StatEntryArmor|StatEntryCharacter|StatEntryObject|StatEntryPotion|StatEntryShield|StatEntrySkillData|StatEntryStatusData|StatEntryWeapon

--- @deprecated
--- Creates a new stats entry on the server
--- @param name string Stat entry name
--- @param type string Stat entry type (i.e. SkillData, StatusData, etc.)
--- @param template string|nil When not nil, all properties are copied from the specified stats entry
--- @return StatEntryType
function Ext.CreateStat(name, type, template) end

--- @deprecated
--- Synchronizes all modifications of the specified stat to all clients
--- @param name string Stat entry name
--- @param persist boolean|nil Persist stats entry to savegame?
function Ext.SyncStat(name, persist) end

--- @deprecated
--- Toggles whether the specified stats entry should be persisted to savegames
--- @param name string Stat entry name
--- @param persist boolean Persist stats entry to savegame?
function Ext.StatSetPersistence(name, persist) end

--- @deprecated
--- Returns the textual label assigned to the specified enumeration value
--- @param enum string Engine enumeration name
--- @param index number Value index to look up
--- @return string|nil
function Ext.EnumIndexToLabel(enum, index) end

--- @deprecated
--- Returns the numeric index assigned to the specified enumeration label
--- @param enum string Engine enumeration name
--- @param label string Value name to look for
--- @return number|nil
function Ext.EnumLabelToIndex(enum, label) end

--- @deprecated
--- Execute the SkillProperties of the specified skill on a target character.
--- @param skillId string Stats skill ID
--- @param attacker ObjectHandle|number|string Attacker character handle/NetID/UUID
--- @param target ObjectHandle|number|string Target character handle/NetID/UUID
--- @param position number[]
--- @param propertyContext string Target|AoE|Self|SelfOnHit|SelfOnEquip
--- @param isFromItem boolean
function Ext.ExecuteSkillPropertiesOnTarget(skillId, attacker, target, position, propertyContext, isFromItem) end

--- @deprecated
--- Execute the SkillProperties of the specified skill on a position.
--- @param skillId string Stats skill ID
--- @param attacker ObjectHandle|number|string Attacker character handle/NetID/UUID
--- @param position number[]
--- @param radius number
--- @param propertyContext string Target|AoE|Self|SelfOnHit|SelfOnEquip
--- @param isFromItem boolean
function Ext.ExecuteSkillPropertiesOnPosition(skillId, attacker, position, radius, propertyContext, isFromItem) end

--- @deprecated
--- Returns the transformation rules that are applied when two neighbouring surfaces interact.
--- @return SurfaceInteractionSet[][]
function Ext.GetSurfaceTransformRules() end

--- @deprecated
--- Returns the surface template for the specified surface type
--- @param type string See SurfaceType enumeration
--- @return SurfaceTemplate
function Ext.GetSurfaceTemplate(type) end

--- @deprecated
--- Updates the transformation rules that are applied when two neighbouring surfaces interact.
--- @param rules SurfaceInteractionSet[][] New rules to apply
function Ext.UpdateSurfaceTransformRules(rules) end

--- @deprecated
--- Prepares a new surface action for execution
--- @param type string Surface action type
--- @return EsvSurfaceAction
function Ext.CreateSurfaceAction(type) end

--- @deprecated
--- Executes a surface action
--- @param action EsvSurfaceAction Action to execute
function Ext.ExecuteSurfaceAction(action) end

--- @deprecated
--- Cancels a surface action
--- @param actionHandle integer Action to cancel
function Ext.CancelSurfaceAction(actionHandle) end

--- Starts creating a new item using template UUID or cloning an existing item.
--- @param from EsvItem|string Template UUID or item to clone
--- @param recursive boolean|nil Copy items in container? (cloning only)
--- @return ItemConstructor
function Ext.CreateItemConstructor(from, recursive) end

--- Begin applying a status on the specified character or item.
--- @param target string|ObjectHandle Target character/item
--- @param statusId string Status ID to apply
--- @param lifeTime number Status lifetime (-1 = infinite, -2 = keep alive)
--- @return EsvStatus|nil
function Ext.PrepareStatus(target, statusId, lifeTime) end

--- Finish applying a status on the specified character or item.
--- @param status EsvStatus Status to apply
function Ext.ApplyStatus(status) end

--- @see Ext_ServerCustomStat#GetAll
--- @deprecated
--- Returns a table containing the UUID of all registered custom stat definitions
--- @return string[]
function Ext.GetAllCustomStats() end

--- @deprecated
--- Retrieve a custom stat definition by name
--- @param statName string Custom stat name to look for
--- @return CustomStatDefinition|nil
function Ext.GetCustomStatByName(statName) end

--- @deprecated
--- Retrieve a custom stat definition by id
--- @param statId string Custom stat UUID to look for
--- @return CustomStatDefinition|nil
function Ext.GetCustomStatById(statId) end

--- @deprecated
--- Create a new custom stat definition
--- @param name string Custom stat name
--- @param description string Custom stat description
--- @return string|nil Custom stat UUID
function Ext.CreateCustomStat(name, description) end

--- @deprecated
--- Returns the UUID of all characters on the specified level. 
--- Uses the current level if no level name was specified.
--- @param level string|nil Optional level name
--- @return string[]
function Ext.GetAllCharacters(level) end

--- @deprecated
--- Returns the UUID of all characters within a radius around the specified point.
--- @param x number
--- @param y number
--- @param z number
--- @param distance number
--- @return string[]
function Ext.GetCharactersAroundPosition(x, y, z, distance) end

--- @deprecated
--- Returns the UUID of all items on the specified level. 
--- Uses the current level if no level name was specified.
--- @param level string|nil Optional level name
--- @return string[]
function Ext.GetAllItems(level) end

--- @deprecated
--- Returns the UUID of all items within a radius around the specified point.
--- @param x number
--- @param y number
--- @param z number
--- @param distance number
--- @return string[]
function Ext.GetItemsAroundPosition(x, y, z, distance) end

--- @deprecated
--- Returns the UUID of all triggers on the specified level. 
--- Uses the current level if no level name was specified.
--- @param level string|nil Optional level name
--- @return string[]
function Ext.GetAllTriggers(level) end

--- @deprecated
--- Returns the property proxy of the specified character
--- @param id string|integer|ObjectHandle Character UUID or handle or NetID
--- @return EsvCharacter|EclCharacter
function Ext.GetCharacter(id) end

--- @deprecated
--- Returns the property proxy of the specified item
--- @param id string|integer|ObjectHandle Item UUID or handle or NetID
--- @return EsvItem|EclItem
function Ext.GetItem(id) end

--- @deprecated
--- Returns the property proxy of the specified trigger (server only)
--- @param id string|ObjectHandle Trigger UUID or handle
--- @return EsvTrigger
function Ext.GetTrigger(id) end

--- @deprecated
--- Returns the property proxy of the specified character, item, projectile or trigger
--- @param handle ObjectHandle|string Game object handle or UUID. NetID will fail since it has no type information (an item and a character could have the same NetID).
--- @return EsvGameObject|EclGameObject
function Ext.GetGameObject(handle) end

--- @deprecated
--- Returns the property proxy of the specified surface
--- @param handle ObjectHandle Surface handle
--- @return EsvSurface
function Ext.GetSurface(handle) end

--- @deprecated
--- Returns the property proxy of the specified status
--- @param character string|integer|ObjectHandle Character UUID or handle or NetID
--- @param handle integer|StatusHandle Status handle or NetID
--- @return EsvStatus
function Ext.GetStatus(character, handle) end

--- @deprecated
--- Returns the specified turn-based combat
--- @param combatId integer Combat ID
--- @return EsvCombat
function Ext.GetCombat(combatId) end

--- @deprecated
--- Returns the AI grid for the currently active level
--- @return AiGrid
function Ext.GetAiGrid() end

--- @deprecated
--- Returns information about the currently active level
--- @return LevelDesc
function Ext.GetCurrentLevelData() end

--- @deprecated
--- Creates a new damage list object
--- @return DamageList
function Ext.NewDamageList() end

--- @deprecated
--- Returns whether Osiris is currently accessible or not.
--- @return boolean
function Ext.OsirisIsCallable() end

--- @deprecated
--- Returns a random number; equivalent to Lua random
--- @param low integer
--- @param up integer
--- @return integer|number
function Ext.Random(low, up) end

--- @deprecated
--- Rounds the specified number
--- @param n number
--- @return number
function Ext.Round(n) end

--- @deprecated
--- Generate Lua IDE helpers for the currently loaded module
--- @param builtin boolean|nil Only export built-in functions and names exported by Lua?
function Ext.GenerateIdeHelpers(builtin) end

--- @deprecated
--- Returns whether the code is executing in a client context
--- @return boolean
function Ext.IsClient() end

--- @deprecated
--- Returns whether the code is executing in a server context
--- @return boolean
function Ext.IsServer() end

--- @deprecated
--- Returns whether the Developer Mode switch is enabled
--- @return boolean
function Ext.IsDeveloperMode() end

--- @deprecated
--- Returns the current client/server game state machine state.
--- @return string
function Ext.GetGameState() end

--- @alias GameMode string|"Campaign"|"GameMaster"|"Arena"

--- @deprecated
--- Returns the current gamemode.
--- @return GameMode
function Ext.GetGameMode() end

--- @deprecated
--- Returns the current difficulty (0-3). 0 = Story, 1 = Explorer, 2 = Classic, 3 = Tactician, 4 = Honour
--- @return integer
function Ext.GetDifficulty() end

--- @deprecated
--- Broadcast a message to all peers
--- @param channel string Channel that will receive the message
--- @param payload string Message payload
--- @param excludeCharacter string|nil Optional peer to exclude from broadcast
function Ext.BroadcastMessage(channel, payload, excludeCharacter) end

--- @deprecated
--- Sends a message to the peer that controls the specified character
--- @param characterGuid string Character that will receive the message
--- @param channel string Channel that will receive the message
--- @param payload string Message payload
function Ext.PostMessageToClient(characterGuid, channel, payload) end

--- @deprecated
--- Sends a message to the specified peer
--- @param userId number User that will receive the message
--- @param channel string Channel that will receive the message
--- @param payload string Message payload
function Ext.PostMessageToUser(userId, channel, payload) end

--- @deprecated
--- Sends a message to the server
--- @param channel string Channel that will receive the message
--- @param payload string Message payload
function Ext.PostMessageToServer(channel, payload) end

--- @alias NetListenerCallback fun(channel:string, payload:string, user:integer|nil)

--- Registers a listener that is called when a network message is received on the specified channel
--- @param channel string Network channel name
--- @param handler NetListenerCallback Lua handler
function Ext.RegisterNetListener(channel, handler) end

--- @deprecated
--- Registers a new dialog voice line for the specified speaker.
--- @param speakerGuid string Speaker character UUID
--- @param textKey string Translated string key of text line
--- @param path string Path to audio .WEM
--- @param length number Length of audio in seconds
function Ext.AddVoiceMetaData(speakerGuid, textKey, path, length) end

--- @deprecated
--- @param handle string Translated string handle
--- @param fallback string Fallback string if the specified handle is not found
--- @return string Translated string
function Ext.GetTranslatedString(handle, fallback) end

--- @deprecated
--- @param key string Translated string key
--- @return string,string Translated string and handle
function Ext.GetTranslatedStringFromKey(key) end

--- @deprecated
--- @param key string Translated string key
--- @param handle string Translated string handle
--- @return boolean
function Ext.CreateTranslatedStringKey(key, handle) end

--- @deprecated
--- @param handle string Translated string handle
--- @param text string Display text
--- @return boolean
function Ext.CreateTranslatedStringHandle(handle, text) end

--- @deprecated
--- @param key string Translated string key
--- @param text string Display text
--- @return string|nil Created string handle
function Ext.CreateTranslatedString(key, text) end

--- @deprecated
--- Redirects all file accesses to the specified path to another file.
--- @param path string Original path
--- @param newPath string New (redirected) path
function Ext.AddPathOverride(path, newPath) end

--- @deprecated
--- Returns whether the specified path is currently redirected to another path.
--- @param path string Original path
--- @return string|nil Overridden path
function Ext.GetPathOverride(path) end

--- @deprecated
--- Constructs a new Flash UI element
--- @param name string User-defined unique name that identifies the UI element
--- @param path string Path of the SWF file relative to the data directory
--- @param layer integer Stack order of the UI element
--- @param flags integer Optional UI flags to test (experimental).
--- @return UIObject|nil
function Ext.CreateUI(name, path, layer, flags) end

--- @deprecated
--- Retrieves an UI element with the specified name
--- @param name string User-defined unique name that identifies the UI element
--- @return UIObject|nil
function Ext.GetUI(name) end

--- @deprecated
--- Retrieves a built-in UI element at the specified path.
--- If no such element exists, the function returns nil.
--- @param path string UI SWF path relative to Data\
--- @return UIObject|nil
function Ext.GetBuiltinUI(path) end

--- @deprecated
--- Retrieves an engine UI element with the specified engine type ID.
--- If no such element exists, the function returns nil.
--- @param typeId number Engine UI element type ID
--- @return UIObject|nil
function Ext.GetUIByType(typeId) end

--- @deprecated
--- Destroys the specified UI element
--- @param name string User-defined unique name that identifies the UI element
function Ext.DestroyUI(name) end

--- @deprecated
--- Refresh the UI of the specified character
--- @param character ObjectHandle Handle of character
--- @param flags integer UI elements to refresh
function Ext.UISetDirty(character, flags) end

--- @deprecated
--- Enable/disable debug prints on Flash custom draw callbacks. Useful if you need to see what icon names a UI is handling, for usage with UIObject:SetCustomIcon.
--- @param enable boolean
function Ext.UIEnableCustomDrawCallDebugging(enable) end

--- @alias UICallbackHandler fun(ui:UIObject, event:string, ...:string|boolean|number)
--- @alias UICallbackEventType string|"Before"|"After"

--- Registers a listener that is called when the specified function is called from Flash
--- @param object UIObject UI object returned from Ext.CreateUI, Ext.GetUI or Ext.GetBuiltinUI
--- @param name string ExternalInterface function name
--- @param handler UICallbackHandler Lua handler
--- @param type UICallbackEventType|nil Event type - 'Before' or 'After'
function Ext.RegisterUICall(object, name, handler, type) end

--- Registers a listener that is called when the specified function is called from Flash.
--- The event is triggered for every UI element with the specified type ID.
--- @param typeId number Engine UI element type ID
--- @param name string ExternalInterface function name
--- @param handler UICallbackHandler Lua handler
--- @param type UICallbackEventType|nil Event type - 'Before' or 'After'
function Ext.RegisterUITypeCall(typeId, name, handler, type) end

--- Registers a listener that is called when the specified function is called from Flash.
--- The event is triggered regardless of which UI element it was called on.
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
--- The event is triggered for every UI element with the specified type ID.
--- @param typeId number Engine UI element type ID
--- @param name string Flash method name
--- @param handler UICallbackHandler Lua handler
--- @param type UICallbackEventType|nil Event type - 'Before' or 'After'
function Ext.RegisterUITypeInvokeListener(typeId, name, handler, type) end

--- Registers a listener that is called when the specified method is called on the main timeline of the Flash object
--- The event is triggered regardless of which UI element it was called on.
--- @param name string Flash method name
--- @param handler UICallbackHandler Lua handler
--- @param type UICallbackEventType|nil Event type - 'Before' or 'After'
function Ext.RegisterUINameInvokeListener(name, handler, type) end

--- Registers a listener that is called when a console command is entered in the dev console
--- @param cmd string Console command
--- @param handler fun(cmd:string, ...:string)
function Ext.RegisterConsoleCommand(cmd, handler) end

--- @deprecated
--- Write data to an external (persistent) file
--- @param path string File path relative to Documents\Larian Studios\Divinity Original Sin 2 Definitive Edition\Osiris Data
--- @param contents string File contents to write
function Ext.SaveFile(path, contents) end

--- @deprecated
--- Read data from an external (persistent) file
--- @param path string File path relative to Documents\Larian Studios\Divinity Original Sin 2 Definitive Edition\Osiris Data
--- @param context string|nil Path context (nil or"user"means relative to the Osiris Data directory;"data"means relative to game data path)
--- @return string File contents
function Ext.LoadFile(path, context) end

--- @deprecated
--- Returns a monotonic value representing the current time in milliseconds.
--- Useful for performance measurements / measuring real world time.
--- (Note: This value is not synchronized between peers and different clients may report different time values!)
--- @return number Time
function Ext.MonotonicTime() end

--- @deprecated
--- Returns whether the player has a compatible Script Extender version installed
--- @param playerGuid string UUID of player character
--- @return boolean
function Ext.PlayerHasExtender(playerGuid) end

--- @deprecated
--- Returns information about current mouse position and hovered objects
--- @return EclPickingState
function Ext.GetPickingState() end

--- @deprecated
--- Triggers a breakpoint in the Lua debugger.
--- If no debugger is connected, the function does nothing.
function Ext.DebugBreak() end

--- @deprecated
--- Handle to double conversion hack for use in Flash external interface calls
--- (Some of the builtin functions treat handles as double values)
--- @param handle ObjectHandle|StatusHandle Handle to cast
--- @return number Double handle
function Ext.HandleToDouble(handle) end

--- @deprecated
--- Double to handle conversion hack for use in Flash external interface calls
--- (Some of the builtin functions treat handles as double values)
--- @param handle number Double handle to cast
--- @return ObjectHandle|StatusHandle
function Ext.DoubleToHandle(handle) end

--#endregion
]],
Flash = {[[
--#region Flash Types

--- @class FlashObject
--- Represents an object in Flash.
--- Implements the __index and __newindex metamethods using string keys (i.e. allows table-like behavior):
--- obj.field = 123 -- Can assign values to any object property
--- Ext.Print(obj.field) -- Can read any object property
---
--- Field values are returned using the appropriate Lua type;
--- Flash Boolean/Number/string = Lua boolean/number/string
--- Flash Object = Lua engine class FlashObject
--- Flash array = Lua engine class FlashArray
--- Flash function = Lua engine class FlashFunction
local FlashObject = {}

--- Represents an array in Flash.
--- Implements the __index, __newindex and __len metamethods using integer keys (i.e. allows table-like behavior):
--- arr[2] = 123 -- Can assign values to any array index
--- Ext.Print(arr[2]) -- Can read any array index
--- Ext.Print(#arr) -- Can query length of array
--- @class FlashArray<T>: { [integer]: T }

--- @class FlashFunction
--- Represents a function or method in Flash.
--- Implements the __call metamethod (i.e. can be called).
--- The passed arguments are forwarded to the Flash method and the return value of the Flash method is returned.
local FlashFunction = {}

---@class FlashEventDispatcher:FlashObject

---Currently unsupported type 12.
---@class FlashDisplayObject:FlashEventDispatcher

---@class FlashInteractiveObject:FlashDisplayObject
---@field doubleClickEnabled boolean Specifies whether the object receives doubleClick events.
---@field mouseEnabled boolean Specifies whether this object receives mouse, or other user input, messages.
---@field tabEnabled boolean Specifies whether this object is in the tab order.
---@field tabIndex integer Specifies the tab ordering of objects in a SWF file.

---@class FlashBitmapData
---@class FlashMatrix
---@class FlashVector
---@class FlashTextSnapshot
---@class FlashPoint
---@class FlashEvent
---@class FlashMouseEvent

---@class FlashGraphics:FlashObject
---@field beginBitmapFill fun(bitmap:FlashBitmapData, matrix:FlashMatrix, repeat:boolean, smooth:boolean) Fills a drawing area with a bitmap image.
---@field beginFill fun(color:integer, alpha:number) Specifies a simple one-color fill that subsequent calls to other Graphics methods (such as lineTo or drawCircle) use when drawing.
---@field beginGradientFill fun(type:string, colors:FlashArray, alphas:FlashArray, ratios:FlashArray, matrix:FlashMatrix, spreadMethod:string, interpolationMethod:string, focalPointRatio:number) Specifies a gradient fill used by subsequent calls to other Graphics methods (such as lineTo or drawCircle) for the object.
---@field clear function Clears the graphics that were drawn to this Graphics object, and resets fill and line style settings.
---@field curveTo fun(controlX:number, controlY:number, anchorX:number, anchorY:number) Draws a quadratic Bezier curve using the current line style from the current drawing position to the control point specified.
---@field drawCircle fun(x:number, y:number, radius:number) Draws a circle.
---@field drawEllipse fun(x:number, y:number, width:number, height:number) Draws an ellipse.
---@field drawPath fun(commands:FlashVector, data:FlashVector, winding:string) Submits a series of commands for drawing.
---@field drawRect fun(x:number, y:number, width:number, height:number) Draws a rectangle.
---@field drawRoundRect fun(x:number, y:number, width:number, height:number, ellipseWidth:number, ellipseHeight:number) Draws a rounded rectangle.
---@field endFill function Applies a fill to the lines and curves that were added since the last call to beginFill, beginGradientFill, or beginBitmapFill methods.
---@field lineGradientStyle fun(type:string, colors:FlashArray, alphas:FlashArray, ratios:FlashArray, matrix:FlashMatrix, spreadMethod:string, interpolationMethod:string, focalPointRatio:number) Specifies a gradient to use for the stroke when drawing lines.
---@field lineStyle fun(thickness:number, color:integer, alpha:number, pixelHinting:boolean, scaleMode:string, caps:string, joints:string, miterLimit:number) Specifies a line style used for subsequent calls to Graphics methods such as the lineTo method or the drawCircle method.
---@field lineTo fun(x:number, y:number) Draws a line using the current line style from the current drawing position.
---@field moveTo fun(x:number, y:number) Moves the current drawing position,

---@class FlashDisplayObjectContainer:FlashInteractiveObject
---@field mouseChildren boolean Determines whether or not the children of the object are mouse, or user input device, enabled.
---@field numChildren integer Returns the number of children of this object. [read-only]
---@field tabChildren boolean Determines whether the children of the object are tab enabled.
---@field textSnapshot FlashTextSnapshot Returns a TextSnapshot object for this DisplayObjectContainer instance. [read-only]
---@field addChild fun(child:FlashDisplayObject):FlashDisplayObject Adds a child DisplayObject instance to this DisplayObjectContainer instance.
---@field addChildAt fun(child:FlashDisplayObject, index:integer):FlashDisplayObject Adds a child DisplayObject instance to this DisplayObjectContainer instance.
---@field areInaccessibleObjectsUnderPoint fun(point:FlashPoint):boolean Indicates whether the security restrictions would cause any display objects to be omitted from the list returned by calling the DisplayObjectContainer.getObjectsUnderPoint() method with the specified point point.
---@field contains fun(child:FlashDisplayObject):boolean Determines whether the specified display object is a child of the DisplayObjectContainer instance or the instance itself.
---@field getChildAt fun(index:integer):FlashDisplayObject Returns the child display object instance that exists at the specified index.
---@field getChildByName fun(name:string):FlashDisplayObject Returns the child display object that exists with the specified name.
---@field getChildIndex fun(child:FlashDisplayObject):integer Returns the index position of a child DisplayObject instance.
---@field getObjectsUnderPoint fun(point:FlashPoint):table Returns an array of objects that lie under the specified point and are children (or grandchildren, and so on) of this DisplayObjectContainer instance.
---@field removeChild fun(child:FlashDisplayObject):FlashDisplayObject Removes the specified child DisplayObject instance from the child list of the DisplayObjectContainer instance.
---@field removeChildAt fun(index:integer):FlashDisplayObject Removes a child DisplayObject from the specified index position in the child list of the DisplayObjectContainer.
---@field removeChildren fun(beginIndex:integer, endIndex:integer) Removes all child DisplayObject instances from the child list of the DisplayObjectContainer instance.
---@field setChildIndex fun(child:FlashDisplayObject, index:integer) Changes the position of an existing child in the display object container.
---@field swapChildren fun(child1:FlashDisplayObject, child2:FlashDisplayObject) Swaps the z-order (front-to-back order) of the two specified child objects.
---@field swapChildrenAt fun(index1:integer, index2:integer) Swaps the z-order (front-to-back order) of the child objects at the two specified index positions in the child list.

---@class FlashSprite:FlashDisplayObjectContainer
---@field buttonMode boolean Specifies the button mode of this sprite.
---@field graphics FlashGraphics Specifies the Graphics object that belongs to this sprite where vector drawing commands can occur. [read-only]
---@field soundTransform number Controls sound within this sprite.
---@field useHandCursor boolean A value that indicates whether the pointing hand (hand cursor) appears when the pointer rolls over a sprite in which the buttonMode property is set to true.


---@class FlashMovieClip:FlashSprite
---@field currentFrame integer Specifies the number of the frame in which the playhead is located in the timeline of the MovieClip instance. [read-only]
---@field currentFrameLabel string The label at the current frame in the timeline of the MovieClip instance. [read-only]
---@field currentLabel string The current label in which the playhead is located in the timeline of the MovieClip instance. [read-only]
---@field currentLabels string[] Returns an array of FrameLabel objects from the current scene. [read-only]
---@field currentScene FlashObject The current scene in which the playhead is located in the timeline of the MovieClip instance. [read-only]
---@field scenes FlashArray[] An array of Scene objects, each listing the name, the number of frames, and the frame labels for a scene in the MovieClip instance. [read-only]
---@field enabled boolean A Boolean value that indicates whether a movie clip is enabled.
---@field framesLoaded integer The number of frames that are loaded from a streaming SWF file. [read-only]
---@field isPlaying boolean A Boolean value that indicates whether a movie clip is curently playing. [read-only]
---@field totalFrames integer The total number of frames in the MovieClip instance. [read-only]
---@field trackAsMenu boolean Indicates whether other display objects that are SimpleButton or MovieClip objects can receive mouse release events or other user input release events.
---@field gotoAndPlay fun(frame:string|integer, scene:string|nil) Starts playing the SWF file at the specified frame.
---@field gotoAndStop fun(frame:string|integer, scene:string|nil) Brings the playhead to the specified frame of the movie clip and stops it there.
---@field nextFrame function Sends the playhead to the next frame and stops it.
---@field nextScene function Moves the playhead to the next scene of the MovieClip instance.
---@field play function Moves the playhead in the timeline of the movie clip.
---@field prevFrame function Sends the playhead to the previous frame and stops it.
---@field prevScene function Moves the playhead to the previous scene of the MovieClip instance.
---@field stop function Stops the playhead in the movie clip.
---@field hitTest fun(x:number, y:number, shapeFlag:boolean|nil):boolean


---@class FlashMainTimeline:FlashMovieClip
---@field events string[] An array of input keys this UI should listen for, in the form of 'IE Name', such as 'IE UICreationTabPrev'. The engine will invoke onEventDown/onEventUp when these keys are pressed, if they haven't been handled.
---@field onEventDown fun(id:number):boolean Invoked by the engine when a valid input key in this.events is pressed. If true is returned, the key is"handled"and won't send events to other UI objects.
---@field onEventUp fun(id:number):boolean Invoked by the engine when a valid input key in this.events is released. If true is returned, the key is"handled"and won't send events to other UI objects.
---@field onEventResolution fun(width:number, height:number) Invoked by the engine when the screen is resized.
---@field onEventInit function Invoked by the engine. Typically used to register the anchor id and layout with ExternalInterface.call.

--#endregion
]]}
	},
}