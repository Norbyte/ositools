# Changes in v58

## WIP!

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
