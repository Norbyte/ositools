# Changes in v58

## WIP!

## Enumerations

Enum values are undergoing a significant technical change in v58. This will not affect most mods, but there are some minor compatibility breaking changes that could affect certain rare use cases.

Enum values returned from functions and enum properties are now returned as `userdata` (lightcppobject) values instead of `string`.

```lua
_D(type(_C().CurrentTemplate.BloodSurfaceType)) -- "userdata"
```

Enum values have `Label`, `Value` and `EnumName` properties that can be queried to fetch the textual name, numeric value and enumeration name respectively.
```lua
local bt = _C().CurrentTemplate.BloodSurfaceType
_D(bt.Label) -- "Blood"
_D(bt.Value) -- 16
_D(bt.EnumName) -- "SurfaceType"
```

Enum values implement `__tostring` for backwards compatibility with old string enums

```lua
print(_C().CurrentTemplate.BloodSurfaceType) -- "Blood"
```

Enum values support comparison with other enum values, enum labels (names) and numeric values:

```lua
local bt = _C().CurrentTemplate.BloodSurfaceType
_D(bt) -- "Blood"
_D(bt == "Blood") -- true
_D(bt == "something else") -- false
_D(bt == 16) -- true
_D(bt == 15) -- false
_D(bt == Ext.Enums.SurfaceType.Blood) -- true
_D(bt == Ext.Enums.SurfaceType.Web) -- false
```

Enum properties support assignment of other enum values, enum labels (names) and numeric values:

```lua
-- assignment by enum label
_C().CurrentTemplate.BloodSurfaceType = "Blood"
-- assignment by enum value
_C().CurrentTemplate.BloodSurfaceType = 16
-- assignment by enum object
_C().CurrentTemplate.BloodSurfaceType = Ext.Enums.SurfaceType.Blood
```

Using enum values as table keys turns them into strings for backwards compatibility reasons:

```lua
local t = { Web = 123 }
_D(t[Ext.Enums.SurfaceType.Web]) -- prints 123
```

JSON serialization turns enum values into their string representation.

```lua
print(Ext.Json.Stringify(Ext.Enums.SurfaceType.Web))
-- "Web"
```

## Bitfields

Bitfields returned from functions and enum properties are now returned as `userdata` (lightcppobject) values instead of `table`.

```lua
_D(type(_C().Stats.AttributeFlags)) -- "userdata"
```

Bitfields have `__Labels`, `__Value` and `__EnumName` properties that can be queried to fetch a table containing all textual names, a numeric value representing all values and the enumeration name respectively.
```lua
local af = _C().Stats.AttributeFlags
_D(af) -- ["SuffocatingImmunity", "BleedingImmunity", "DrunkImmunity"]
_D(af.__Labels) -- ["SuffocatingImmunity", "BleedingImmunity", "DrunkImmunity"]
_D(af.__Value) -- 137440004096
_D(af.__EnumName) -- "StatAttributeFlags"
```

They also support querying the state of each bitfield flag (either by label or by numeric value):
```lua
local af = _C().Stats.AttributeFlags
_D(af.DrunkImmunity) -- true
_D(af.WebImmunity) -- false
```

Bitfields support table-like iteration (i.e. `pairs`/`ipairs`):
```lua
for k,v in pairs(af) do 
   print(k,v) 
end
-- 1       BleedingImmunity
-- 2       DrunkImmunity
-- 3       SuffocatingImmunity
```

Bitfields implement `__tostring` that returns a string containing the enum type and all labels:

```lua
-- "StatAttributeFlags(SuffocatingImmunity,BleedingImmunity,DrunkImmunity)"
print(_C().Stats.AttributeFlags)
```

Bitfields support the `~` (bitwise negate) unary operator and the `|` (bitwise or), `&` (bitwise and) and `~` (bitwise xor) binary operators. All binary operators support bitfields, string bitfield labels, tables of bitfield labels and numeric values as their second operand:

```lua
local af = _C().Stats.AttributeFlags
_D(af) -- ["SuffocatingImmunity", "BleedingImmunity", "DrunkImmunity"]
_D(~af) -- ["Unstorable", "DisarmedImmunity", "PoisonImmunity", "HastedImmunity", ...]
_D(af & {"DrunkImmunity", "BleedingImmunity"}) -- ["BleedingImmunity", "DrunkImmunity"]
_D(af & Ext.Enums.StatAttributeFlags.DrunkImmunity) -- ["DrunkImmunity"]
_D(af | "FreezeImmunity") -- ["FreezeImmunity", "SuffocatingImmunity", "BleedingImmunity", "DrunkImmunity"]
_D(af ~ 0x802) -- ["SuffocatingImmunity", "DrunkImmunity", "BurnImmunity"]
```


Bitfields support comparison with other bitfields, singular bitfield labels, tables of bitfield labels and numeric values:

```lua
local af = _C().Stats.AttributeFlags
_D(af) -- ["SuffocatingImmunity", "BleedingImmunity", "DrunkImmunity"]
_D(af == {"SuffocatingImmunity", "BleedingImmunity", "DrunkImmunity"}) -- true
_D(af == {"SuffocatingImmunity", "BleedingImmunity"}) -- false
_D(af == 137440004096) -- true
_D(af == 1234) -- false
```

Bitfields support assignment of other enum values, enum labels (names) and numeric values:

```lua
-- assignment by enum label
_C().Stats.AttributeFlags = {"SuffocatingImmunity", "BleedingImmunity"}
-- assignment by enum value
_C().Stats.AttributeFlags = 137440004096
-- assignment by enum object
_C().Stats.AttributeFlags = Ext.Enums.StatAttributeFlags.WebImmunity
-- assigning result of bitfield operation
_C().Stats.AttributeFlags = _C().Stats.AttributeFlags | "WebImmunity"
```

JSON serialization turns bitfields into an array of textual labels.

```lua
print(Ext.Json.Stringify(_C().Stats.AttributeFlags))
-- ["SuffocatingImmunity", "BleedingImmunity", "DrunkImmunity"]
```

It should be noted that bitfields are always passed by value, so appending or removing elements from them like a table is not possible:

```lua
local af = _C().Stats.AttributeFlags
-- throws "attempt to index a userdata value (global 'af')"
af.SuffocatingImmunity = false
-- throws "bad argument #1 to 'insert' (table expected, got light C++ object)"
table.insert(af, "WebImmunity")
```


## Custom variables

v58 adds support for attaching custom properties to characters and items. These properties support automatic network synchronization between server and clients as well as savegame persistence.

To use custom variables, the variable name must first be registered with the variable manager:
```lua
Ext.Utils.RegisterUserVariable("NRD_Whatever", {
    Server = true,
    Client = true, 
    SyncToClient = true
})
```

The `RegisterUserVariable` method accepts two parameters, a variable name and an optional list of settings.
The following settings are supported:
| Setting | Default | Meaning |
|-|-|-|
| `Server` | true | Variable is present on server entities |
| `Client` | false | Variable is present on client entities |
| `WriteableOnServer` | true | Variable can be modified on server side |
| `WriteableOnClient` | false | Variable can be modified on client side |
| `Persistent` | true | Variable is written to/restored from savegames |
| `SyncToClient` | false | Server-side changes to the variable are synced to all clients |
| `SyncToServer` | false | Client-side changes to the variable are synced to the server |
| `SyncOnTick` | true | Client-server sync is performed once per game loop tick |
| `SyncOnWrite` | false | Client-server sync is performed immediately when the variable is written. This is disabled by default for performance reasons. |
| `DontCache` | false | Disable Lua caching of variable values (see below) |

Usage notes:
 - Since variable prototypes are used for savegame serialization, network syncing, etc., they must be registered before the savegame is loaded and every time the Lua context is reset; performing the registration when `BootstrapServer.lua` or `BootstrapClient.lua` is loaded is recommended
 - Although the variables registered server-side and client-side can differ, it is recommended to register all variables on both sides (even if they're server-only or client-only) for consistency
 - Variable names, much like Osiris DB names are global; it is recommended to prefix them with your mod name to ensure they're unique
 - Variables must be registered with the same settings on both client and server, otherwise various synchronization issues may occur.


After registration, custom variables can be read/written through the `UserVars` property on characters and items:
```lua
_C().UserVars.NRD_Whatever = 123
Ext.Print(_C().UserVars.NRD_Whatever)
```

### Synchronization

A variable is only eligible for synchronization if:
 - Both `Server` and `Client` flags are set
 - For server to client synchronization, both `WriteableOnServer` and `SyncToClient` flags are set
 - For client to server synchronization, both `WriteableOnClient` and `SyncToServer` flags are set

For a variable to be synchronized, it must be *dirtied* first. The most straightforward way to perform this is by doing a direct write to the variable:
```lua
_C().UserVars.NRD_Whatever = "asd"
```

Note: Writes to subproperties of complex types (i.e. tables etc) will not trigger this mechanism! Example:
```lua
_C().UserVars.NRD_Whatever.SomeProperty = 123
```
Since the `__newindex` metamethod of the `UserVars` object is not called, the variable manager does not detect that a change was performed. A simple fix is to reassign the property after modifications were made:
```lua
local v = _C().UserVars.NRD_Whatever
v.SomeProperty = 123
_C().UserVars.NRD_Whatever = v
```

On each tick of the game loop, variables that were changed during the current tick are collected and sent to the client/server in a batch. Unless configured otherwise (i.e. the `SyncOnTick` setting is disabled), this is the default synchronization method.

If a change to a user variable must be visible by the peer before the end of the current tick:
 - The `SyncOnWrite` flag can be enabled which ensures that the write is immediately sent to client/server without additional wait time. 
 - `Ext.Utils.SyncUserVariables()` can be called, which synchronizes all user variable changes that were done up to that point


### Caching behavior

The variable manager keeps a Lua copy of table variables for performance reasons. This means that instead of unserializing the table from JSON each time the property is accessed, the cached Lua version is returned after the first access. This means that subsequent accesses to the property will return the same reference and writes to the property.

Example:
```lua
local t1 = _C().UserVars.NRD_Whatever
local t2 = _C().UserVars.NRD_Whatever
t1.Name = "test"
_D(t2.Name) -- prints "test"
```

Cached variables are serialized to JSON when they are first sent to the client/server or when a savegame is created. This means that all changes to a dirtied variable up to the next synchronization point will be visible to peers despite no explicit write being performed to `UserVars`. Example:
```lua
local v = _C().UserVars.NRD_Whatever
v.SomeProperty = 123
-- variable is dirtied here
_C().UserVars.NRD_Whatever = v
v.SomeProperty = 456
-- client will receive 456
Ext.Utils.SyncUserVariables()

-- client will NOT receive this change since the NRD_Whatever variable is no longer dirtied after sync;
-- another explicit write to UserVars.NRD_Whatever must be performed
v.SomeProperty = 789
```

Variable caching can be disabled by passing the `DontCache` flag to `RegisterUserVariable`. Uncached variables are unserialized from JSON each time the property is accessed, so each access returns a different copy:

```lua
local t1 = _C().UserVars.NRD_Whatever
local t2 = _C().UserVars.NRD_Whatever
t1.Name = "test"
_D(t2.Name) -- prints nil
```

Variables are immediately serialized to JSON when a `UserVars` write occurs; this means that changes to the original reference have no effect after assignment.

```lua
local t1 = { Name = "t1" }
_C().UserVars.NRD_Whatever = t1
t1.Name = "t2"
_D(_C().UserVars.NRD_Whatever.Name) -- prints "t1"
```

This also means that changing the value returned from a `UserVars` fetch will not affect the stored value:

```lua
local t1 = _C().UserVars.NRD_Whatever
t1.Name = "t1"
_D(_C().UserVars.NRD_Whatever.Name) -- prints "t1"
```

## Custom character activities

Support for custom CharacterTasks has been added. A CharacterTask defines and controls interactions that the client character can do within the world, such as moving, picking up items, casting skills, etc. CharacterTasks also manage their associated visuals and cursor icons.

CharacterTasks are not global; a new instance of them is attached to each controllable character that can perform them. Characters do not necessarily need to have all possible character tasks attached to them.

Each task defines a conditional priority for itself; the task with the highest priority is the one the client character sees or uses. For example, hovering over an interactable item causes the item use (higher priority) task to take priority over others like "move to" (default task, lowest priority).

Tasks themselves can be in 3 states:
- Inactive (default)
- Previewing: the task's priority won over all others in the game tick and its effects are being "previewed" - in this state the task is allowed to change the cursor, show temporary effects in the world, etc.
- Active/executing: the task is being carried out. Once finished, the task will be deleted and the preview task selection process starts again.

Custom CharacterTasks can be registered by calling:

```lua
Ext.Behavior.RegisterCharacterTask("Prefix_MyTask", MyTaskCreator)
```
where `MyTaskCreator` is a constructor function that returns an instance of your custom pseudo-class table.
Since `Prefix_MyTask` is a global name, it is recommended to prefix it with the acronym of the mod (similarly to stats, Osiris databases, etc.)

The tables that act as CharacterTask classes should implement the following interface methods:

### Attached
```lua
function MyTask:Attached()
```

Called when the task is attached to a character, or when the task enters the active state (which technically creates a new instance of it within the engine). This will be called again for a character after a lua reset. Use for initialization.

### Start
```lua
function MyTask:Start()
```

Called when the task is first instanced, or when the interact key is pressed (ex. left-click when using keyboard+mouse).

### Stop
```lua
function MyTask:Stop()
```

Called when the task is being removed by the engine, or when a cancel key is pressed (ex. right-click when using keyboard+mouse).

### CanEnter
```lua
---@return boolean
function MyTask:CanEnter()
```

Returns whether the client can begin previewing the task.

If a task wins the priority contest and can be entered, `ExitPreview()` will be called for the previous previewed task, and `EnterPreview()` will be called on the newest prioritized task. 

This method is optional to implement; the default implementation is to return `false`.

### GetPriority
```lua
---@param previousPriority int32
---@return int32
function MyTask:GetPriority(previousPriority)
```

Returns the priority of previewing the task. This is called every tick for the engine for all tasks; the task with the highest priority that also passes its `CanEnter()` check is the one the client will see being previewed.

This method is optional to implement; the default implementation is to return `0`.

### GetExecutePriority
```lua
---@param previousPriority int32
---@return int32
function MyTask:GetExecutePriority(previousPriority)
```

Returns the priority of the task for being executed (becoming active). This is called every tick for the engine for all attached tasks of the active character; the task with the highest priority is the one the client will be able to execute.

Character tasks can be queued. In the case that there are multiple tasks that want to execute, they will do so in the order of `GetExecutePriority()`, executing after the previous task exits via `ShouldExit()`.

This method is optional to implement; the default implementation is to return `0`.

### GetActionCost
```lua
---@return int32
function MyTask:GetActionCost()
```

Purpose unknown.

This method is optional to implement; the default implementation is to return `0`.

### GetTotalAPActionCost
```lua
---@return int32
function MyTask:GetTotalAPActionCost()
```

Purpose unknown.

This method is optional to implement; the default implementation is to return `0`.

### GetAPWarning
```lua
---@return int64
function MyTask:GetAPWarning()
```

Purpose unknown.

This method is optional to implement; the default implementation is to return `0`.

### GetError
```lua
---@return int64
function MyTask:GetError()
```

Called if `Enter()` fails. Will show a message on the controller UI.

This method is optional to implement; the default implementation is to return `0`.

### SetCursor
```lua
function MyTask:SetCursor()
```

Called every tick while the task is being previewed, if the cursor is not over a UI.

### HandleInputEvent
```lua
---@param event InputEvent
---@param task EclCharacterTask
function MyTask:HandleInputEvent(event, task)
```

Handles an input event. Fired regardless of the task's state, but only for certain input events, like interaction buttons (mouse clicks). You should set `RequestRun` on `task` if you wish for the task to become active next tick, or be queued (if there is already an active task).

### EnterPreview
```lua
function MyTask:EnterPreview()
```

Called when `CanEnter()` succeeds on the task, if it was previously inactive.

### UpdatePreview
```lua
function MyTask:UpdatePreview()
```

Called every tick if the task is in the preview state.

### ExitPreview
```lua
function MyTask:ExitPreview()
```

Called when an task exits the preview state due to another one having greater priority. Use to remove any visuals related to the preview.

### GetSightRange
```lua
---@return float
function MyTask:GetSightRange()
```

Purpose unknown.

This method is optional to implement; the default implementation is to return `0.0`.

### ClearAIColliding
```lua
function MyTask:ClearAIColliding()
```

Purpose unknown.

### ValidateTargetRange
```lua
---@return int32
function MyTask:ValidateTargetRange()
```

Purpose unknown.

This method is optional to implement; the default implementation is to return `0`.

### Enter
```lua
---@return boolean
function MyTask:Enter()
```

Called when a task enters the active state. If `false` is returned, `GetError()` will be called and the task will abort.

This method is optional to implement; the default implementation is to return `false`.

### Update
```lua
---@return boolean
function MyTask:Update()
```

Called every tick while the task is in the active state. If `false` is returned, `Exit()` will be called and the task will become inactive.

This method is optional to implement; the default implementation is to return `true`.

### Exit
```lua
function MyTask:Exit()
```

Called when `Update()` returns `false`, marking the end of the active state.

### CanExit
```lua
---@return boolean
function MyTask:CanExit()
```

Called every tick while the task is active; purpose unknown, but it's recommended to keep its return value consistent with `Update()`.

This method is optional to implement; the default implementation is to return `true`.

### CanExit2
```lua
---@return boolean
function MyTask:CanExit2()
```

Purpose unknown.

This method is optional to implement; the default implementation is to return `true`.

### GetDescription
```lua
---@return string
function MyTask:GetDescription()
```

Purpose unknown.

### GetAPDescription
```lua
---@return string
function MyTask:GetAPDescription()
```

Purpose unknown.

## v58 crash status

Some information on game crashes and what's planned ahead of the v58 release.

In total, 5553 crash dumps were collected by the extender error reporting service between 2022 Nov and 2023 January. 
Out of these, 3983 are vanilla game crashes and 1570 are likely to be extender-related crashes.
(Note: this is only an approximate number as extender code may still be present in the stack trace even for a vanilla bug and vice-versa)

In total there were 752 different signatures (i.e. types of crash). Most of these are caused by memory corruption of various once-off issues that are essentially impossible to debug (552 out of the 752 crash types occurred only once). As such, only crashes with more than 5 occurrences were checked.

These are the most significant findings:
 - A. 1127 occurrences - `MADNESS` is applied on an item or a character with an item summon
 - B. 548 occurrences - calling Lua `GetCellInfo()` while the map is not fully loaded
 - C. 520 occurrences - `MADNESS` is removed from an item or a character with an item summon
 - D: 230 occurrences - applying visuals on a character where the visual GUID doesn't reference a valid VisualResource
 - E. 189 occurrences - memory corruption while applying damage during item hit
 - F. 140 occurrences - opening inventory when the client has not yet finished syncing
 - G. 138 occurrences - Osiris `ShowNotification()` on a non-player character
 - H. 130 occurrences - shooting a projectile from a caster or origin that is dead/deleted
 - I. 128 occurrences - performing stealth check while a character is being deleted
 - J. 114 occurrences - deleting a status while it is being applied (mostly from behavior code)
 - K. 108 occurrences - crash while updating character animation type after unequipping a weapon
 - L. 52 occurrences - crash while retrieving an UI object
 - M. 40 occurrences - calculating AI score of a nonexistent status
 - N. 40 occurrences - crash while firing `CharacterStatusApplied` event
 - O. 38 occurrences - crash while cleaning up after an Osiris event
 - P. 28 occurrences - evaluating `ProtectFromSummon` flag with no attacker character set
 - Q. 27 occurrences - instantiating Flash object with invalid path

Status of these issues in v58:
 - A,B,C,D,G,H,I,Q. - fixed or a workaround is applied
 - E. (item hit issue) - it is not known what the exact cause of this bug is, so only a speculative fix was applied which may or may not fix this issue
 - J,N. (deleting a pending status) - only a partial workaround is applied. Because of the way this status deletion is performed (Behavior code deleting a status from an even thrown inside the status apply code), it is not possible to completely fix the issue. It is generally not recommended to apply or remove any statuses from Behavior status events, since those fire immediately during the status apply/remove process, unlike Osiris events that are safe because they fire after the apply/remove was performed.
 - K,L. - it was not possible to determine the cause of these crashes
 - M,P. - no fix is provided at this time because of their relatively minor importance and the fact that these are caused by mods having invalid data in their stats entries
 - O. - Osiris events are not reentrant (i.e. an event cannot be thrown while already dispatching an event); if a double event dispatch happens, there is a chance that the Osiris runtime will crash. Because of the complexity of the underlying code, no workaround will be provided


## Vanilla game fixes

 - Fix `MADNESS` crash caused by items - `esv::StatusConsume::Enter` incorrectly assumes that `MADNESS` can only be applied on characters and will crash when it encounters an item
 - Fix crash caused by item summons - `esv::StatusConsumeBase::Enter` and `esv::StatusConsumeBase::Exit` incorrectly handle non-character summons
 - Fix `ShowNotification()` crash - calling `ShowNotification` on a non-player character could result in a crash
 - Fix crash during stealth checks; `ecl::StatusMachine::ExitStatus` performs a stealth check during deletion of the StatusMachine when the character no longer has any stats
 - Fix crash when trying to apply a nonexistent visual - `CreateEquipmentVisuals()` only checks for the existence of the resource UUID parameter, but doesn't check if it points to a valid resource
 - Fix crash when the caster of a projectile was destroyed - The projectile logic will try to fetch the stats of the caster without checking if the caster object is NULL, causing a crash
 - Fix crash when a status is removed while it is being applied (this was mainly observed in behavior scripts) (Note: while this prevents the vast majority of crashes, there are still cases where a crash may occur)


## Bug fixes

 - Fix issue where custom client visuals created via Lua would not be removed correctly
 - Fix read-only properties being writeable
 - Fix flag setters not accepting integer values
 - Fix setting/unsetting flags on `UIObject` not triggering the appropriate update mechanisms
 - Fix `EnumLabelToIndex` returning incorrect value for certain types
 - Fix damage list type mismatch
This could have caused crashes when damage lists were modified in some circumstances
 - Fix bogus `__eq` logic in objects - previously equality checks on lightcppobjects would return incorrect results
 - Fix potential crash when statuses are applied during game unload - ThrowEvent() during UnloadSession
 - Fix weapon type check oversight in `Game.Math` hit calculation code - this could have caused incorrect hit chance calculations
 - Fix crash when `AiGrid::GetCellInfo` is called while the map is not fully loaded
 - Fix crash when reading certain client projectile properties
 - Fix Ext.Audio calls not working when called on characters/items

## Minor changes

 - Added `Ext.Entity.GetCombatComponent(handle)` to get the combat component of an entity
 - Add `Ext.Entity.GetProjectile(handle)` to fetch projectiles via their handle
 - Expand visual deactivate masks (EquipmentSlotMask and VisualSetSlotMask in EquipmentVisualsComponent)
 - Expanded list of writeable UIObject properties
 - Add support for rebuilding shroud manager heightmap via the `ShroudManager:RebuildHeightMapTexture()` method; this is used to ensure the client draws decals and surfaces at the correct height after changing AI grid heights
 - Re-added a restricted version of Lua `load()` and `loadstring()` -- these were previously removed for security reasons
 - Allow disabling updates via the `DisableUpdates` key in `ScriptExtenderUpdaterConfig.json`
 - Add Lua backtrace to crash reporter window