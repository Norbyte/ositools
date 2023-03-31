---@diagnostic disable

return {
Functions=[[
--#region Deprecated Functions (moved to Ext modules)

--- @alias ExtEngineEvent string|"SessionLoading"|"SessionLoaded"|"ModuleLoading"|"ModuleLoadStarted"|"ModuleResume"|"StatsLoaded"|"GameStateChanged"|"SkillGetDescriptionParam"|"StatusGetDescriptionParam"|"GetSkillDamage"|"GetSkillAPCost"|"ComputeCharacterHit"|"CalculateTurnOrder"|"GetHitChance"|"StatusGetEnterChance"|"StatusHitEnter"|"BeforeCharacterApplyDamage"|"UIInvoke"|"UICall"|"AfterUIInvoke"|"AfterUICall"|"BeforeShootProjectile"|"ShootProjectile"|"ProjectileHit"|"GroundHit"|"InputEvent"|"RawInput"|"TreasureItemGenerated"

--- @alias StatType string|"SkillData"|"Potion"|"StatusData"|"Weapon"|"Armor"|"Shield"|"DeltaMod"|"Object"|"Character"
--- @alias StatEntryType StatEntryArmor|StatEntryCharacter|StatEntryObject|StatEntryPotion|StatEntryShield|StatEntrySkillData|StatEntryStatusData|StatEntryWeapon|StatEntryCrime

--- @deprecated
--- Returns the version number of the Osiris Extender
--- @return integer
function Ext.Version() end

--- @deprecated
--- Returns the version number of the game
--- @return string
function Ext.GameVersion() end

--- @deprecated
--- Registers a function to call when an extender event is thrown
--- @param event ExtEngineEvent Event to listen for
--- @param callback function Lua function to run when the event fires
function Ext.RegisterListener(event, callback) end

--- @deprecated
--- Registers a function that is called when certain Osiris functions are called
--- Supports events, built-in queries, DBs, PROCs, QRYs (user queries)
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
--- @return LegacyModuleInfo
function Ext.GetModInfo(modGuid) end

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
--- Updates all properties of the specified skill set
--- The function expects a table in the same format as the one returned by GetSkillSet
--- @param skillSet StatSkillSet
function Ext.UpdateSkillSet(skillSet) end

--- @deprecated
--- Returns all equipment from the specified equipment set
--- @param name string Name of equipment set entry
--- @return StatEquipmentSet|nil
function Ext.GetEquipmentSet(name) end

--- @deprecated
--- Updates all properties of the specified equipment set
--- The function expects a table in the same format as the one returned by GetEquipmentSet
--- @param equipmentSet StatEquipmentSet
function Ext.UpdateEquipmentSet(equipmentSet) end

--- @deprecated
--- Returns the specified DeltaMod or nil on failure
--- @param name string Name of delta mod
--- @param modifierType string Modifier type (Armor/Weapon)
--- @return DeltaMod
function Ext.GetDeltaMod(name, modifierType) end

--- @deprecated
--- Updates all properties of the specified DeltaMod
--- The function expects a table in the same format as the one returned by GetDeltaMod
--- @param deltaMod DeltaMod Name of delta mod
function Ext.UpdateDeltaMod(deltaMod) end

--- @deprecated
--- Returns the specified crafting item combination or nil on failure
--- @param name string Name of item combo
--- @return ItemCombo|nil
function Ext.GetItemCombo(name) end

--- @deprecated
--- Updates all properties of the specified item combination
--- The function expects a table in the same format as the one returned by GetItemCombo
--- @param itemCombo ItemCombo
function Ext.UpdateItemCombo(itemCombo) end

--- @deprecated
--- Returns the specified crafting preview data or nil on failure
--- @param name string Name of item combo preview data
--- @return ItemComboPreviewData|nil
function Ext.GetItemComboPreviewData(name) end

--- Updates all properties of the specified item combo preview
--- The function expects a table in the same format as the one returned by GetItemComboPreviewData
--- @param previewData ItemComboPreviewData
function Ext.UpdateItemComboPreviewData(previewData) end

--- @deprecated
--- Returns the specified crafting property or nil on failure
--- @param name string Name of item combo property
--- @return ItemComboProperty|nil
function Ext.GetItemComboProperty(name) end

--- @deprecated
--- Updates all properties of the specified item combo property
--- The function expects a table in the same format as the one returned by GetItemComboProperty
--- @param itemComboProperty ItemComboProperty
function Ext.UpdateItemComboProperty(itemComboProperty) end

--- @deprecated
--- Returns the specified treasure table or nil on failure
--- @param name string Name of treasure table
--- @return StatTreasureTable|nil
function Ext.GetTreasureTable(name) end

--- @deprecated
--- Updates all properties of the specified treasure table
--- The function expects a table in the same format as the one returned by GetTreasureTable
--- @param treasureTable StatTreasureTable
function Ext.UpdateTreasureTable(treasureTable) end

--- @deprecated
--- Returns the specified treasure category or nil on failure
--- @param name string Name of treasure category
--- @return StatTreasureCategory|nil
function Ext.GetTreasureCategory(name) end

--- @deprecated
--- Updates all properties of the specified treasure category
--- The function expects a table in the same format as the one returned by GetTreasureCategory
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

--- @deprecated
--- Replaces level scaling formula for the specified stat
--- @param statType string Stat entry type
--- @param attribute string Stat attribute name
--- @param func function Replacement scaling function
function Ext.StatSetLevelScaling(statType, attribute, func) end

--- @deprecated
--- Returns the property proxy of the specified stats entry
--- Returns level scaled values if the level parameter is not nil
--- @param stat string Stat entry name
--- @param level integer|nil Level scaling level
--- @return StatEntryType
function Ext.GetStat(stat, level) end

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
--- Execute the SkillProperties of the specified skill on a target character
--- @param skillId string Stats skill ID
--- @param attacker ComponentHandle|number|string Attacker character handle/NetID/UUID
--- @param target ComponentHandle|number|string Target character handle/NetID/UUID
--- @param position number[]
--- @param propertyContext string Target|AoE|Self|SelfOnHit|SelfOnEquip
--- @param isFromItem boolean
function Ext.ExecuteSkillPropertiesOnTarget(skillId, attacker, target, position, propertyContext, isFromItem) end

--- @deprecated
--- Execute the SkillProperties of the specified skill on a position
--- @param skillId string Stats skill ID
--- @param attacker ComponentHandle|number|string Attacker character handle/NetID/UUID
--- @param position number[]
--- @param radius number
--- @param propertyContext string Target|AoE|Self|SelfOnHit|SelfOnEquip
--- @param isFromItem boolean
function Ext.ExecuteSkillPropertiesOnPosition(skillId, attacker, position, radius, propertyContext, isFromItem) end

--- @class SurfaceInteractionSet
--- @field TransformType string Surface transform to apply (Bless, Curse, Ignite, ...)
--- @field ActionableSurfaces string[][] Surface types that this transform applies to

--- @deprecated
--- Returns the transformation rules that are applied when two neighbouring surfaces interact
--- @return table<integer, SurfaceInteractionSet[]>
function Ext.GetSurfaceTransformRules() end

--- @deprecated
--- Returns the surface template for the specified surface type
--- @param type string See SurfaceType enumeration
--- @return SurfaceTemplate
function Ext.GetSurfaceTemplate(type) end

--- @deprecated
--- Updates the transformation rules that are applied when two neighbouring surfaces interact
--- @param rules table<integer, SurfaceInteractionSet[]> New rules to apply
function Ext.UpdateSurfaceTransformRules(rules) end

--- @deprecated
--- Returns a table containing the UUID of all registered custom stat definitions
--- @return string[]
--- @see Ext_ServerCustomStat#GetAll
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
--- Uses the current level if no level name was specified
--- @param level string|nil Optional level name
--- @return string[]
function Ext.GetAllCharacters(level) end

--- @deprecated
--- Returns the UUID of all characters within a radius around the specified point
--- @param x number
--- @param y number
--- @param z number
--- @param distance number
--- @return string[]
function Ext.GetCharactersAroundPosition(x, y, z, distance) end

--- @deprecated
--- Returns the UUID of all items on the specified level. 
--- Uses the current level if no level name was specified
--- @param level string|nil Optional level name
--- @return string[]
function Ext.GetAllItems(level) end

--- @deprecated
--- Returns the UUID of all items within a radius around the specified point
--- @param x number
--- @param y number
--- @param z number
--- @param distance number
--- @return string[]
function Ext.GetItemsAroundPosition(x, y, z, distance) end

--- @deprecated
--- Returns the UUID of all triggers on the specified level. 
--- Uses the current level if no level name was specified
--- @param level string|nil Optional level name
--- @return string[]
function Ext.GetAllTriggers(level) end

--- @deprecated
--- Returns the property proxy of the specified character
--- @param id string|integer|ComponentHandle Character UUID or handle or NetID
--- @return EsvCharacter|EclCharacter
function Ext.GetCharacter(id) end

--- @deprecated
--- Returns the property proxy of the specified item
--- @param id string|integer|ComponentHandle Item UUID or handle or NetID
--- @return EsvItem|EclItem
function Ext.GetItem(id) end

--- @deprecated
--- Returns the property proxy of the specified trigger (server only)
--- @param id string|ComponentHandle Trigger UUID or handle
--- @return Trigger
function Ext.GetTrigger(id) end

--- @deprecated
--- Returns the property proxy of the specified character, item, projectile or trigger
--- @param handle ComponentHandle|string Game object handle or UUID. NetID will fail since it has no type information (an item and a character could have the same NetID)
--- @return IEoCServerObject|IEoCClientObject
function Ext.GetGameObject(handle) end

--- @deprecated
--- Returns the property proxy of the specified surface
--- @param handle ComponentHandle Surface handle
--- @return EsvSurface
function Ext.GetSurface(handle) end

--- @deprecated
--- Returns the property proxy of the specified status
--- @param character Guid|ComponentHandle Character Guid or handle
--- @param handle integer|ComponentHandle Status handle or NetID
--- @return EsvStatus
function Ext.GetStatus(character, handle) end

--- @deprecated
--- Returns the specified turn-based combat
--- @param combatId integer Combat ID
--- @return EsvCombat
function Ext.GetCombat(combatId) end

--- @deprecated
--- Returns the AI grid for the currently active level
--- @return EocAiGrid
function Ext.GetAiGrid() end

--- @deprecated
--- Returns information about the currently active level
--- @return LevelDesc
function Ext.GetCurrentLevelData() end

--- @deprecated
--- Creates a new damage list object
--- @return StatsDamagePairList
function Ext.NewDamageList() end

--- @deprecated
--- Returns whether Osiris is currently accessible or not
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
--- Returns whether the Developer Mode switch is enabled
--- @return boolean
function Ext.IsDeveloperMode() end

--- @deprecated
--- Returns the current gamemode
--- @return string|"Campaign"|"GameMaster"|"Arena"
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

--- @deprecated
--- Registers a new dialog voice line for the specified speaker
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
--- Redirects all file accesses to the specified path to another file
--- @param path string Original path
--- @param newPath string New (redirected) path
function Ext.AddPathOverride(path, newPath) end

--- @deprecated
--- Returns whether the specified path is currently redirected to another path
--- @param path string Original path
--- @return string|nil Overridden path
function Ext.GetPathOverride(path) end

--- @deprecated
--- Constructs a new Flash UI element
--- @param name string User-defined unique name that identifies the UI element
--- @param path string Path of the SWF file relative to the data directory
--- @param layer integer Stack order of the UI element
--- @param flags integer Optional UI flags to test (experimental)
--- @return UIObject|nil
function Ext.CreateUI(name, path, layer, flags) end

--- @deprecated
--- Retrieves an UI element with the specified name
--- @param name string User-defined unique name that identifies the UI element
--- @return UIObject|nil
function Ext.GetUI(name) end

--- @deprecated
--- Retrieves a built-in UI element at the specified path
--- If no such element exists, the function returns nil
--- @param path string UI SWF path relative to Data\
--- @return UIObject|nil
function Ext.GetBuiltinUI(path) end

--- @deprecated
--- Retrieves an engine UI element with the specified engine type ID
--- If no such element exists, the function returns nil
--- @param typeId number Engine UI element type ID
--- @return UIObject|nil
function Ext.GetUIByType(typeId) end

--- @deprecated
--- Destroys the specified UI element
--- @param name string User-defined unique name that identifies the UI element
function Ext.DestroyUI(name) end

--- @deprecated
--- Refresh the UI of the specified character
--- @param character ComponentHandle Handle of character
--- @param flags integer UI elements to refresh
function Ext.UISetDirty(character, flags) end

--- @deprecated
--- Enable/disable debug prints on Flash custom draw callbacks. Useful if you need to see what icon names a UI is handling, for usage with UIObject:SetCustomIcon
--- @param enable boolean
function Ext.UIEnableCustomDrawCallDebugging(enable) end

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
--- Returns a monotonic value representing the current time in milliseconds
--- Useful for performance measurements / measuring real world time
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
--- Use Ext.Debug.DebugBreak instead.  
--- Triggers a breakpoint in the Lua debugger
--- If no debugger is connected, the function does nothing
--- @see Ext_Debug#DebugBreak
function Ext.DebugBreak() end

--- @deprecated
--- Use Ext.UI.HandleToDouble instead.  
--- Handle to double conversion hack for use in Flash external interface calls
--- (Some of the builtin functions treat handles as double values)
--- @param handle ComponentHandle|ComponentHandle Handle to cast
--- @return number Double handle
--- @see Ext_ClientUI#HandleToDouble
function Ext.HandleToDouble(handle) end

--- @deprecated
--- Use Ext.UI.DoubleToHandle instead.  
--- Double to handle conversion hack for use in Flash external interface calls
--- (Some of the builtin functions treat handles as double values)
--- @param handle number Double handle to cast
--- @return ComponentHandle|ComponentHandle
--- @see Ext_ClientUI#DoubleToHandle
function Ext.DoubleToHandle(handle) end

--- @deprecated
--- Use Ext.Client.UpdateShroud instead.  
--- Modifies the shroud value at a given cell
--- @param x number
--- @param y number
--- @param layer ShroudType
--- @param value integer The shroud cell value, 0-255
--- @see Ext_ClientClient#UpdateShroud
function Ext.UpdateShroud(x, y, layer, value) end

--- @deprecated
--- No longer needed
function Ext.EnableExperimentalPropertyWrites() end

--#endregion

--#region Deprecated Module Functions

--- @deprecated
--- Use `Ext.Vars.RegisterModVariable` instead.
Ext_Utils.RegisterModVariable = Ext.Vars.RegisterModVariable

--- @deprecated
--- Use `Ext.Vars.RegisterUserVariable` instead.
Ext_Utils.RegisterUserVariable = Ext.Vars.RegisterUserVariable

--- @deprecated
--- Use `Ext.Vars.GetModVariables` instead.
Ext_Utils.GetModVariables = Ext.Vars.GetModVariables

--- @deprecated
--- Use `Ext.Vars.SyncUserVariables` instead.
Ext_Utils.SyncUserVariables = Ext.Vars.SyncUserVariables

--- @deprecated
--- Use `Ext.Vars.SyncModVariables` instead.
Ext_Utils.SyncModVariables = Ext.Vars.SyncModVariables

--- @deprecated
--- Use `Ext.Vars.DirtyModVariables` instead.
Ext_Utils.DirtyUserVariables = Ext.Vars.DirtyUserVariables

--- @deprecated
--- Use `Ext.Vars.DirtyModVariables` instead.
Ext_Utils.DirtyModVariables = Ext.Vars.DirtyUserVariables

--#endregion
]],
BaseExtraData = [[
Ext.ExtraData = {
    FirstItemTypeShift = 9,
    SecondItemTypeShift = 16,
    PickpocketGoldValuePerPoint = 200,
    PickpocketWeightPerPoint = 2000,
    PickpocketExperienceLevelsPerPoint = 4,
    PersuasionAttitudeBonusPerPoint = 5,
    WandUsesMax = 10,
    AttributeBaseValue = 10,
    AttributeCharCreationBonus = 1,
    AttributeLevelGrowth = 2,
    AttributeBoostGrowth = 0.75,
    AttributeGrowthDamp = 0.7,
    AttributeSoftCap = 40,
    WitsGrowthDamp = 0.5,
    VitalityStartingAmount = 21,
    VitalityExponentialGrowth = 1.25,
    VitalityLinearGrowth = 9.091,
    VitalityToDamageRatio = 5,
    VitalityToDamageRatioGrowth = 0.2,
    ExpectedDamageBoostFromAttributePerLevel = 0.065,
    ExpectedDamageBoostFromSkillAbilityPerLevel = 0.015,
    ExpectedDamageBoostFromWeaponAbilityPerLevel = 0.025,
    ExpectedConGrowthForArmorCalculation = 1,
    FirstVitalityLeapLevel = 9,
    FirstVitalityLeapGrowth = 1.25,
    SecondVitalityLeapLevel = 13,
    SecondVitalityLeapGrowth = 1.25,
    ThirdVitalityLeapLevel = 16,
    ThirdVitalityLeapGrowth = 1.25,
    FourthVitalityLeapLevel = 18,
    FourthVitalityLeapGrowth = 1.35,
    DamageBoostFromAttribute = 0.05,
    MonsterDamageBoostPerLevel = 0.02,
    PhysicalArmourBoostFromAttribute = 0,
    MagicArmourBoostFromAttribute = 0,
    VitalityBoostFromAttribute = 0.07,
    DodgingBoostFromAttribute = 0,
    HealToDamageRatio = 1.3,
    ArmorToVitalityRatio = 0.55,
    ArmorRegenTimer = 0.01,
    ArmorRegenConstGrowth = 1,
    ArmorRegenPercentageGrowth = 10,
    ArmorAfterHitCooldown = -7,
    MagicArmorRegenTimer = 0.01,
    MagicArmorRegenConstGrowth = 1,
    MagicArmorRegenPercentageGrowth = 10,
    MagicArmorAfterHitCooldown = -7,
    ArmorHeadPercentage = 0.15,
    ArmorUpperBodyPercentage = 0.3,
    ArmorLowerBodyPercentage = 0.2,
    ArmorShieldPercentage = 0.5,
    ArmorHandsPercentage = 0.15,
    ArmorFeetPercentage = 0.15,
    ArmorBeltPercentage = 0.1,
    ArmorAmuletPercentage = 0.12,
    ArmorRingPercentage = 0.08,
    SkillMemoryCostReductionFromAbility = 0,
    CharacterBaseMemoryCapacity = 3,
    CharacterBaseMemoryCapacityGrowth = 0.5,
    CharacterAttributePointsPerMemoryCapacity = 1,
    LoremasterBonusToMemory = 2,
    AbilityBaseValue = 0,
    NumStartingCombatAbilityPoints = 2,
    CombatAbilityCap = 10,
    CombatAbilityLevelGrowth = 1,
    CombatAbilityNpcGrowth = 0.1,
    CombatAbilityDamageBonus = 5,
    CombatAbilityCritBonus = 1,
    CombatAbilityCritMultiplierBonus = 5,
    CombatAbilityAccuracyBonus = 5,
    CombatAbilityDodgingBonus = 1,
    CombatAbilityReflectionBonus = 5,
    LeadershipRange = 8,
    LeadershipDodgingBonus = 2,
    LeadershipAllResBonus = 3,
    NumStartingCivilAbilityPoints = 2,
    CivilAbilityCap = 5,
    CivilAbilityLevelGrowth = 4,
    CivilPointOffset = 2,
    SavethrowLowChance = 15,
    SavethrowHighChance = 50,
    SavethrowBelowLowPenalty = 5,
    SavethrowPenaltyCap = -30,
    CriticalBonusFromWits = 1,
    InitiativeBonusFromWits = 1,
    WeaponAccuracyPenaltyPerLevel = -20,
    WeaponAccuracyPenaltyCap = -80,
    ShieldAPCost = 0,
    CharacterWeightLight = 40000,
    CharacterWeightMedium = 70000,
    CharacterWeightHeavy = 120000,
    WeaponWeightLight = 1000,
    WeaponWeightMedium = 3000,
    WeaponWeightHeavy = 6000,
    HighGroundThreshold = 2.4,
    HighGroundBaseDamageBonus = 0.2,
    HighGroundMeleeRange = 1,
    HighGroundRangeMultiplier = 2.5,
    LowGroundBaseDamagePenalty = -0.1,
    SneakDefaultAPCost = 4,
    SneakSpeedBoost = -30,
    BlindRangePenalty = 3,
    RangeBoostedGlobalCap = 30,
    SurfaceDurationFromHitFloorReaction = 18,
    SurfaceDurationFireIgniteOverride = 12,
    SurfaceDurationFromCharacterBleeding = -1,
    SurfaceDurationBlessedCursed = -1,
    SurfaceDurationAfterDecay = -1,
    SmokeDurationAfterDecay = 6,
    DualWieldingAPPenalty = 2,
    DualWieldingDamagePenalty = 0.5,
    GhostLifeTime = 3,
    ChanceToSetStatusOnContact = 100,
    AbilityPhysArmorBonusBase = 5,
    AbilityPhysArmorBonusPerPoint = 2,
    AbilityPhysArmorBonusMax = 5,
    AbilityMagicArmorBonusBase = 5,
    AbilityMagicArmorBonusPerPoint = 2,
    AbilityMagicArmorBonusMax = 5,
    AbilityVitalityBonusBase = 3,
    AbilityVitalityBonusPerPoint = 1,
    AbilityVitalityBonusMax = 3,
    SkillAbilityDamageToPhysicalArmorPerPoint = 0,
    SkillAbilityDamageToMagicArmorPerPoint = 0,
    SkillAbilityArmorRestoredPerPoint = 5,
    SkillAbilityVitalityRestoredPerPoint = 5,
    SkillAbilityHighGroundBonusPerPoint = 5,
    SkillAbilityFireDamageBoostPerPoint = 5,
    SkillAbilityPoisonAndEarthDamageBoostPerPoint = 5,
    SkillAbilityAirDamageBoostPerPoint = 5,
    SkillAbilityWaterDamageBoostPerPoint = 5,
    SkillAbilityPhysicalDamageBoostPerPoint = 5,
    SkillAbilityLifeStealPerPoint = 10,
    LifestealFromReflectionModifier = 0,
    SkillAbilityCritMultiplierPerPoint = 5,
    SkillAbilityMovementSpeedPerPoint = 30,
    SkillAbilitySummonsStatsPerPoint = 4,
    SneakingAbilityMovementSpeedPerPoint = 6,
    TalentAttributePointsBonus = 2,
    TalentCombatAbilityPointsBonus = 1,
    TalentCivilAbilityPointsBonus = 1,
    TalentMemoryBonus = 3,
    TalentQuestRootedMemoryBonus = 3,
    TalentRagerPercentage = 70,
    SoftLevelCap = 20,
    PickpocketRequirementDecreaseFromFinesse = 1,
    SkillCombustionRadius = 3,
    TalentPerfectionistAccuracyBonus = 10,
    TalentPerfectionistCriticalChanceBonus = 10,
    TalentExecutionerActionPointBonus = 2,
    TalentPointOffset = 2,
    TalentViolentMagicCriticalChancePercent = 100,
    TalentPointPerLevels = 5,
    TalentQuickStepPartialApBonus = 1,
    SkillHeightRangeMultiplier = 1,
    AbilityPersuasionBonusPerPoint = 4,
    FreeMovementDistanceWhenAttacking = 1,
    TalentSneakingDamageBonus = 40,
    MaximumSummonsInCombat = 4,
    SpiritVisionFallbackRadius = 10,
    AbilityPerseveranceArmorPerPoint = 5,
    AiCoverProjectileTurnMemory = 2,
    CarryWeightBase = 0,
    CarryWeightPerStr = 10000,
    MoveToCarryWeightRatio = 0.75,
    TalentResistDeathVitalityPercentage = 20,
    DeflectProjectileRange = 1,
    SummoningAbilityBonus = 10,
    SurfaceAbsorbBoostPerTilesCount = 10,
    TalentWhatARushThreshold = 50,
    IncarnateSummoningLevel = 10,
    CleaveRangeOverride = 125,
    DamageToThrownWeightRatio = 0.5,
    FleeDistance = 13,
    GlobalGoldValueMultiplier = 1,
    PriceGrowth = 1.12,
    FirstPriceLeapLevel = 9,
    FirstPriceLeapGrowth = 1.75,
    SecondPriceLeapLevel = 13,
    SecondPriceLeapGrowth = 1.15,
    ThirdPriceLeapLevel = 16,
    ThirdPriceLeapGrowth = 1.5,
    FourthPriceLeapLevel = 18,
    FourthPriceLeapGrowth = 1.15,
    PriceModCasualDifficulty = 2.1,
    PriceModClassicDifficulty = 2.5,
    PriceModHardcoreDifficulty = 2.7,
    PriceBarterCoefficient = 0.1,
    PriceAttitudeCoefficient = 0.005,
    PriceRoundToFiveAfterAmount = 100,
    PriceRoundToTenAfterAmount = 1000,
    LevelCap = 35,
    GMCharacterAttributeCap = 100,
    GMCharacterArmorCap = 999999,
    GMCharacterResistanceMin = -1000,
    GMCharacterResistanceMax = 1000,
    GMCharacterAPCap = 100,
    GMCharacterSPCap = 3,
    GMItemLevelCap = 50,
    GMItemAttributeCap = 100,
    GMItemArmorMin = -999999,
    GMItemArmorMax = 999999,
    GMItemResistanceMin = -1000,
    GMItemResistanceMax = 1000,
    LoneWolfMaxAPBonus = 2,
    LoneWolfAPBonus = 2,
    LoneWolfMagicArmorBoostPercentage = 60,
    LoneWolfArmorBoostPercentage = 60,
    LoneWolfVitalityBoostPercentage = 30,
    LivingArmorHealPercentage = 35,
    TorturerDamageStatusTurnIncrease = 1,
    UnstableDamagePercentage = 50,
    UnstableRadius = 3,
    TalentResurrectExtraHealthPercent = 10,
    PoisonedFoodDamage = 1,
    PoisonedFoodDamageRange = 10,
    PoisonedFoodDamageMultiplier = 40,
    TraderDroppedItemsPercentage = 51,
    TraderDroppedItemsCap = 5,
    StatusDefaultDistancePerDamage = 0.75,
    TraderDonationsRequiredAttitude = -45,
    TeleportUnchainDistance = 50,
    TalentHumanCriticalMultiplier = 10,
    TalentHumanCriticalChance = 5,
    TalentSneakingAPCost = 1
}
]]
}