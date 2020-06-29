
# Script Extender Changelogs

## Changes in v49

### UI

 - Added support for Gift Bag 5 armor set tooltips
 - Added tooltip support for dynamically created UI objects (trade window, etc.)
 - Added support for main/offhand item comparison tooltips
 - Fixed incorrect callback parameters in `UICall` and `UIInvoke`
 - Fixed missing skillbook tooltip icons if tooltip library is used
 - Fixed string truncation issue when reading Flash strings longer than 512 bytes

### Lua

 - Contextual hit information (i.e. parameters when the hit was triggered; hit status; etc.) is now collected and is available in two Lua events:
   - Passed as the last parameter (`context`) to `BeforeCharacterApplyDamage` event; new signature is `function (target, attacker, hit, causeType, impactDirection, context)`
   - In new event `StatusHitEnter` (equivalent to `NRD_OnHit` in Osiris); signature is `function (status, context)`
 - Added `Ext.GetTreasureTable(name)` and `Ext.GetTreasureCategory(name)` for fetching treasure table data
 - Added `GameStateChanged(fromState, toState)` event that tracks game loading progress
 - Added `Ext.GameVersion()` function to fetch game version number
 - Added `character.SetScale()`; it must be called on both the server and the client to take effect!
 - `Sight` stat is now properly readable
 - Fixed saving throw multiplier calculation in `Game.Math`
 - Fixed erroneous return value handling (and possible crash) in `NRD_ModQuery`
 - Several changes aimed at fixing crashes caused by concurrent Lua state use

### General
 
 - Expanded `esv::Character` property list: `TurnTimer`, `TriggerTrapsTimer`, `UserID`, ... (see IDE helpers for a full list)
 - `WalkSpeed`/`RunSpeed` is now changeable on `esv::Character`
 - Internal variables of `UseSkill` and `Attack` action states can now be queried as well
 - Added Extender version number to main menu
 - Fixed crash when client thread is mistakenly identified as a server thread (occurs more frequently with more savegame reloads)
 - Fixed missing blood surfaces on hit; this was caused by the hit handler not copying hit flags properly after `ApplyDamage`
 - `NRD_OnHit`/`NRD_OnHeal` now pass source item GUID properly; previously the GUID for items was always `NULL_00000000-0000-0000-0000-000000000000`


## Changes in v48

 - Added Gift Bag 5 support
 - Added support for reading root templates properties; in Osiris via `NRD_RootTemplateGetXyz`, in Lua via `character.RootTemplate.Xyz`, `item.RootTemplate.Xyz`, `projectile.RootTemplate.xyz`
 - Properties for action states `Attack`, and `UseSkill` are now readable via `NRD_ActionStateGetString`
 - Add 3-4 argument `Min()` and `Max()`; integer `Random()` queries
 - Track savegame version separately from extender version -- this means that a savegame compatibility warning will only be shown when the savegame format changes, not after each extender update
 - Protect builtin Lua object metatables -- previously it was possible to inspect/edit metatables of built-in objects; this is no longer the case
 - Make savegame persistence of synced stats optional -- `Ext.SyncStat()` now has an optional second argument that toggles persistence
 - Add `Ext.StatSetPersistence(stat, persist)` call to toggle persistence of modified stats entries on/off; this means that they will no longer be saved in the savegame and will be lost after reload
 - Fix syncing of `Cone`-type skills


## Changes in v47

### Major Changes

 - Added `Game.Tooltip` tooltip library; this allows mods to customize item, skill, status and stat tooltips (add/replace/remove elements)

### Stats

 - Added `Ext.EnumIndexToLabel` and `Ext.EnumLabelToIndex` for converting `enumerations.xml` indexes to names (and vice versa)
 - Added missing `Projectile` property to weapon stats
 - Fixed `GetLevelScaledWeaponDamage` formula in `Game.Math`
 - Fixed enter chance of statuses triggered by mines in `Game.Math`

### UI

 - Added `Ext.DoubleToHandle` function for converting Flash handle values to game handles
 - Added `ui:GetTypeId`, `ui:GetPlayerHandle`, `Ext.GetUIByType`
 - Added extended UI event subscriptions `Ext.RegisterUITypeInvokeListener`, `Ext.RegisterUINameCall` 
 - Allow passing a nil type to `ui:GetValue()` (previously 3 calls had to be made to `GetValue` with each type to fetch values with an unknown type)

### General

 - Added experimental functions `Ext.GetSurface` and `Ext.GetCellInfo` for fetching surface data and inspecting surfaces on a given map coordinate
 - Added support for fetching client statuses via handle
 - Improved verbosity of property map error messages
 - Handle unassigned characters properly in `Ext.PlayerHasExtender`
 - Fixed incorrect peer info reset when unloading module
 - Fixed crash when client entered an unmapped state ID
 - Fixed alignment bug in `ecl::Item`
 - Fixed parameter off by one in `ModQuery()`
 - Fixed bug where custom function initialization could be skipped if the update check took too long

## Changes in v45

### Migrating from v44 to v45

It was discovered that there are situations when `ModuleLoading` is not triggered even though the stats were reloaded (this occurs during certain client-server sync situations and when stats were reloaded from the editor console); this causes stats to revert to their original values. A new event (`StatsLoaded`) was added to fix these shortcomings. The `StatsLoaded` event is meant to replace the use of `ModuleLoading` for stat override purposes, i.e. all stats should be edited when `StatsLoaded` is triggered. (The `ModuleLoading` event will be kept for backwards compatibility.)
```lua
local function statsLoaded()
    -- Edit stats here!
end

Ext.RegisterListener("StatsLoaded", statsLoaded)
``` 

### Major Changes

 - Lua variables can now be stored inside savegames; for more information see https://github.com/Norbyte/ositools/blob/master/LuaAPIDocs.md#persistent-vars

### Stats
 - Add `StatsLoaded` event for stat overrides
 - Add `Ext.GetStatEntriesLoadedBefore(modId, [statType])` to fetch stat entries loaded before a specified mod. This returns a stat name list just like `Ext.GetStatEntries(...)`
 - Added `Ext.GetDeltaMod`, `Ext.UpdateDeltaMod`; these functions allow DeltaMod stats entry manipulation
 - Added read/write support for `SkillProperties` and `ExtraProperties`
 - Added write support for `*Conditions` fields (`TargetConditions`, `AoEConditions`, etc.)
 - Added read/write support for stats `ObjectCategory`
 - The module that declared the stats entry can now be discovered using `stats.ModId`
 - Stats entries can be created on the fly using `Ext.CreateStat(statId, type, [copyFrom])` and `Ext.SyncStat(statId)`
 - Stat entries can be edited dynamically (`Ext.SyncStat(statId)` must be called after editing)
Stat construction example:
```lua
    local stat = Ext.CreateStat("NRD_Dynamic_Skill", "SkillData", "Rain_Water")
    stat.RainEffect = "RS3_FX_Environment_Rain_Fire_01"
    stat.SurfaceType = "Fire"
    Ext.SyncStat("NRD_Dynamic_Skill")

    CharacterAddSkill(CharacterGetHostCharacter(), "NRD_Dynamic_Skill")
```

Stat editing example:
```lua
    local stat = Ext.GetStat("FROST_AURA")
    stat.HealValue = 100
    Ext.SyncStat("FROST_AURA")
```

 - Dynamically created/updated stat entries are stored in the savegame

### Lua
 - Add `ModuleLoadStarted` event to allow filesystem overrides for early files (localization, etc.)
 - Status engine type is now available via `status.StatusType`
 - Added `Ext.GetGameObject` call for fetching arbitrary objects
 - Added support for capturing UI object invokes (calls from the engine to Flash) via `Ext.RegisterUIInvokeListener`
 - Add `Ext.PlayerHasExtender(characterGuid)` call
 - Add `Ext.MonotonicTime()` for timing purposes (returns time in milliseconds)
 - Add `Ext.PrintWarning(...)`
 - Add `BeforeCharacterApplyDamage` callback for editing raw damage values before server-side apply
 - Add `HasTag`, `GetTags`, `GetStatus`, `GetStatusByType`, `GetStatuses` methods to client-side character and item objects
 - Add Flash object handle helpers (`Ext.HandleToDouble(handle)`, `character.Handle`)
 
### General
 - Added definitions of all internal objects to Lua IDE helper: https://github.com/Norbyte/ositools/blob/master/OsiInterface/Misc/ExtIdeHelpers.lua
 - Added type annotations in `Game-Math`
 - Added `StatusGetEnterChance` implementation in `Game.Math`
 - Added Lua projectile property maps
 - Major internal restructuring with the aim of converting all static properties to FixedStrings
 - Relaxed handle restrictions on server-side callbacks; `StatusGetEnterChance`, `OnUpdateTurnOrder` and `ComputeCharacterHit` callbacks can now look up objects via `Ext.GetXyz`
 - Fixed `StatusGetEnterChance` signature (param 2 is `isEnterCheck`, not `useCharacterStats`)
 - Fixed reference counting behavior for fixed strings. This fixes many random crashes that might have occurred when handling FixedString values
 - Fixed sneaking flags again :)
 - Fixed weapon requirements calculation in `Game.Math`
 - Fixed crash during `CharacterIterateSkills` when the skill list was modified during iteration
 - Fixed FixedString reference counter underflow crash when deleting from stat maps
 - Various typing corrections for stats entries
 - Fix incorrect type for `AIFlags`
 - Fix armor absorption in hit reset functions
 

## Changes in v44

### Console

The debug window (toggled with the `CreateConsole` config var) now supports console commands.
Usage:
 - Press <enter> to enter console mode; in this mode the normal log output is disabled to avoid log spam while typing commands.
 - You can select client/server context by typing "client" or "server". This selects where the Lua code and console commands will execute.
 - "exit" returns to log mode
 - Commands prefixed by a `!` will trigger the `ConsoleCommand` Lua event. Example:
```lua
local function consoleCmd(cmd, a1, a2, ...)
    Ext.Print("Cmd: " .. cmd .. ", args: ", a1, ", ", a2);
end
Ext.RegisterListener("ConsoleCommand", consoleCmd);
```
The command `!test 123 456` will print `Cmd: test, args: 123, 456`.

Anything else you type in the console will be executed as Lua code in the current context. i.e. you can do `Ext.Print(1234)`. 
Server console commands can also call builtin/custom Osiris functions, so calls like `CharacterGiveReward(CharacterGetHostCharacter(), "CheatShieldsAllRarities", 1)` are possible.
You can use variables just like in Lua, i.e. if you assign a value to a variable in one command you'll be able to use that variable in another command. Be careful, console code runs in global context, so make sure your console variable names don't conflict with globals!
Don't use `local` for console variables, since the lifetime of the local will be one console command. (Each console command is technically a separate chunk that is exited when the command is executed)

### Fixes

 - Re-added GUID and handle support in client `Ext.GetCharacter` (using NetID is not neccessary now)
 - Add `NRD_ModCall` and `NRD_ModQuery`
 - Fixed incorrect parameter order in `StatusGetDescriptionParam` callback; previously `statusSource` and `target` were swapped, the correct signature is `function (status, statusSource, target, param)`
 - Fixed bug where editor did not recognize local peer as extender-capable
 - Fixed bug where extender server messages would not be sent to peers
 - Fixed crash when using extender-enabled clients with a non-extender host
 - Fix missing type/salt checks during handle resolution
 - Fix global table restore after an include call
 - Fix strange NetworkFixedString corruption


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

 
 
