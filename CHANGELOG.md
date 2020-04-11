
# Script Extender Changelogs

## Changes in v43

### Migrating from v42 to v43

The following changes must be observed when migrating to v43:
- Since it is very easy to accidentally pollute the global table, the global Lua table is changed for mods targeting v43 (i.e. `RequiredExtensionVersion` >= 43). A new config option in `OsiToolsConfig.json`, `ModTable` specifies the name of the mod in the global mod table. Each mod has an isolated mod-specific global table at `Mods[ModTable]`. This table contains all of the symbols from the original global table (`Ext`, `Osi`, ...) and built-in libraries (`math`, `table`, ...), but is otherwise separate. (i.e. declaring `function xyz()` is equivalent to setting `Mods[ModTable].xyz = function () ...`).
- `Ext.Require` is only callable during bootstrap to avoid lag when loading files / doing startup stuff from the server loop
- `Ext.Require` got a one-argument form; i.e. it is sufficient to call `Ext.Require("wtf.lua")` instead of `Ext.Require("SomeUUID_1111-2222-...", "wtf.lua")` (old version will still work) 

### Major Changes
 - Added `Game.Math` Lua library; this is a library of stat/skill/character functions that compute damage/vitality/hit/etc values the same way the game does (currently WIP)
    No docs yet, but contents are here --> https://github.com/Norbyte/ositools/blob/master/OsiInterface/Game.Math.lua
 - Lua mods targeting `v43` are no longer loaded into the global table (see the note above)
 - **Add mod desync detection** (i.e. NetworkFixedStrings synchronization) support; when the `SyncNetworkStrings` config variable in `OsirisExtenderSettings.json` is set to true, the server will sync its fixed string list to all clients to avoid crash/desync. If a a desync is detected, a notification will be shown to the affected client.

### Scripting
 - `Ext.Require` now passes values returned from the loaded file to the caller
 - Lua scripts are only bootstrapped if mod has the `Lua` feature flag
 - Added `UIObject:ExternalInterfaceCall()` function to simulate Flash `ExternalInterface.call` calls to the game engine from Lua
 - `Ext.RegisterUICall()` now supports built-in UI elements as well; previously it only worked for user-defined UIs
 - Added `SessionLoaded` event; builtin UIs are guaranteed to be loaded by the time this event is triggered
 - Add `Ext.AddVoiceMetaData` for programmatically adding speaker voice lines
 - Add `Ext.LoadFile`, `Ext.SaveFile` Lua counterparts of `NRD_LoadFile` and `NRD_SaveFile` (callable on client as well)
 - Lua booleans are now converted to integer 0/1 when passed to Osiris
 - Add `Ext.GetTranslatedString()` call for fetching string values using string handles
 - Added `isFromItem` parameter to `SkillGetDescriptionParam`; new signature is: `function (skill, character, isFromItem, param1, param2, ...)`

### Script objects
 - Added `Ext.GetCharacter`, `Ext.GetItem` and `Ext.GetStatus` on client contexts. These functions work similarly to the server-side ones, but return client-side objects. Because of some game quirks the client cannot reliably use UUID to find characters, so these functions use NetID-s for now.
 - Server-side `Ext.GetCharacter`, `Ext.GetItem` and `Ext.GetStatus` can now look up objects using NetID
 - Character object can now be retrieved from client and server-side stats objects using `stats.Character`
 - Added `NetID` property to characters, items and statuses
 - Added `Position`, `Rotation`, `GetItemBySlot` properties to character stat objects
 - Added `HasTag(N)`, `GetTags()`, `GetStatus(Name)`, `GetStatuses()`, `GetStatusByType(typeName)`, `GetInventoryItems()` methods to character objects
 - Added `NetID`, `OwnerHandle`, `StatusId`, `LifeTime`, `CurrentLifeTime`, `StatusSourceHandle`, `RequestDelete`, `Started`, `KeepAlive`, `HasVisuals` properties to client status objects

### General
 - It is now mandatory to specify `RequiredExtensionVersion` in `OsiToolsConfig.json`
 - Warn when a status handle is used for hits or vice versa
 - Fixed rare crash during Character::Hit when no attacker character is mapped
 - Fixed stat getter for Constitution


## Changes in v42

### Migrating from v41 to v42

One of the aims of the v42 and v43 versions is improving support for client-side features, mainly networking and custom UI support. To achieve this, it was necessary to make significant changes to how Lua is loaded and how the various Lua states behave. Because of this, some Lua listeners, globals and functions were changed. 

The client and server Lua contexts were previously unified, i.e. both the server and the client used the same Lua state. After v42 server and client contexts are separated.

The following changes must be observed when migrating to v42:
 - `Bootstrap.lua` is deprecated. The server will load `BootstrapServer.lua`, the client will try to load `BootstrapClient.lua`. If these files cannot be found, `Bootstrap.lua` will be loaded as a fallback. In addition, care must be taken to ensure that `BootstrapServer.lua` only contains code that should be run on the server and `BootstrapClient.lua` only contains code for the client.
 - `Ext.GetHitChance` and `Ext.StatusGetEnterChance` are now events; instead of assigning a single function to them as before (i.e. `Ext.GetHitChance = func`), a listener should be registered (`Ext.RegisterListener("GetHitChance", func)`). For backwards compatibility, during the deprecation period assigning a function to `Ext.GetHitChance` is equivalent to registering that function as a listener.
 - Server-side scripts now only allow registering server-side listeners and vice versa
     - Listeners allowed on the server: 	`SessionLoading`, `ModuleLoading`, `ModuleResume`, `GetSkillDamage`, `ComputeCharacterHit`, `CalculateTurnOrder`, `GetHitChance`, `StatusGetEnterChance`
     - Listeners allowed on the client: 	`SessionLoading`, `ModuleLoading`, `ModuleResume`, `GetSkillDamage`, `SkillGetDescriptionParam`, `StatusGetDescriptionParam`, `GetHitChance`
 - Calling `Ext.EnableStatOverride` is no longer necessary; any calls to this function should be removed
 - The following functions are deleted in client contexts: `Ext.NewCall`, `Ext.NewQuery`, `Ext.NewEvent`, `Ext.GetCharacter`, `Ext.GetItem`, `Ext.GetStatus`, `Ext.GetCombat`, `Ext.GenerateIdeHelpers`

### Major Features
 - Added support for loading custom UI objects (Flash files). Docs available here: https://github.com/Norbyte/ositools/blob/master/LuaAPIDocs.md#ui
 - Added client/server network communication support. This is achieved using the functions `Ext.BroadcastMessage`, `Ext.PostMessageToClient`, `Ext.PostMessageToServer`


### Scripting
 - Add builtin file override support via `Ext.AddPathOverride`
 - `Ext.GetStatEntries` now supports entry types `SkillSet` and `EquipmentSet`
 - Added `Ext.GetSkillSet(name)` and `Ext.GetEquipmentSet(name)` to retrieve the contents of skill/equipment sets
 - Finalized Lua DB delete API (`Osi.DB:Delete(...)`) - https://github.com/Norbyte/ositools/blob/master/LuaAPIDocs.md#databases
 - Fixed bug where `Osi.DB:Get(...)` didn't return any entries when searching on a GuidString column
 - Added `NRD_ItemGetParent` to fetch the direct parent container of an item (as opposed to `ItemGetOwner` which returns the topmost container)
 - Added `NRD_CharacterEquipItem` which allows equipping items to specific slots (mainly useful for weapons)
 - Added `NRD_ObjectGetInternalFlag` and `NRD_ObjectSetInternalFlag` magic :tm: functions
 - `NRD_LuaReset` now also resets client scripts
 - Added 3-parameter version of `NRD_LuaReset` that allows resetting client/server Lua contexts only
 - Add `Ext.IsClient`, `Ext.IsServer`
 - Add workaround for empty damage lists not triggering StatusHit::Enter
 
### General
 - Add developer mode config flag
    - `DeveloperMode` flag in `OsirisExtenderSettings.json`
    - Added `Ext.IsDeveloperMode`
    - `NRD_LuaReset` is now only callable ingame when `DeveloperMode` is enabled
    - Flash warning/trace messages are now redirected to console when `DeveloperMode` is enabled
 - Removed `Net` feature flag; networking calls are now always available
 - Add block chance computed stat (available as `BlockChance`)

### Fixes
 - Fix `GetHitChance` overrides
 - Fix handling of 0-argument calls/queries
 - Fix placeholder update bug in status/skill description params
 - Fix `Ext.StatSetLevelScaling` crash
 - Fix crash when using networking functions without the appropriate flag set
 - Fix message processing status codes
 - Fix crash when stat getter symbols are not mapped
 - Show native error dialog if in-game dialog doesnt work
 - Don't report crashes from wrapped functions
 - Fix achievements enabler mod compatibility
 - Fix handling of paths with backslashes
 - Re-register network protocols on session load
 - Fix crash when fetching unregistered messages
 - Fix crash when extensions are not enabled during client init
 - Fix crash on failed module initialization
 - Fixed possible race condition during story definition generation

 
 
