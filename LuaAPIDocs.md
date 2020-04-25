### Lua API v45 Documentation

### Table of Contents  

 - [Migrating from v42 to v43](#migrating-from-v42-to-v43)
 - [Migrating from v41 to v42](#migrating-from-v41-to-v42)
 - [Client / Server States](#client-server)
 - [Console](#console)
 - [Calling Lua from Osiris](#calling-lua-from-osiris)
    * [Calls](#l2o_calls)
    * [Queries](#l2o_queries)
    * [Events](#l2o_events)
    * [Custom Calls](#l2o_custom_calls)
 - [Calling Osiris from Lua](#calling-osiris-from-lua)
    * [Calls](#o2l_calls)
    * [Queries](#o2l_queries)
    * [Events](#o2l_events)
    * [PROCs](#o2l_procs)
    * [User Queries](#o2l_qrys)
    * [Databases](#o2l_dbs)
 - [The Ext Library](#the-ext-library)
    * [UI](#ui)
    * [Stats](#stats)
    * [Skill/Status Overrides](#skillstatus-overrides)
        * [Hit Chance](#31o_hit_chance)
    * [Mod Info](#mod-info)
    * [Server Characters](#server-characters)
    * [Player Custom Data](#player-custom-data)
    * [Character Stats](#character-stats)
    * [Character Dynamic Stats](#character-dynamic-stats)
    * [Server Items](#server-items)
    * [Item Stats](#item-stats)
    * [Item Dynamic Stats](#item-dynamic-stats)
    * [Server Projectiles](#server-projectiles)
    * [Server Statuses](#server-statuses)
    * [Combat](#combat)
    * [Damage lists](#damage-lists)
    * [Utility functions](#ext-utility)
    * [JSON Support](#json-support)



## Upgrading

### Migrating from v42 to v43

The following changes must be observed when migrating to v43:
- Since it is very easy to accidentally pollute the global table, the global Lua table is changed for mods targeting v43 (i.e. `RequiredExtensionVersion` >= 43). A new config option in `OsiToolsConfig.json`, `ModTable` specifies the name of the mod in the global mod table. Each mod has an isolated mod-specific global table at `Mods[ModTable]`. This table contains all of the symbols from the original global table (`Ext`, `Osi`, ...) and built-in libraries (`math`, `table`, ...), but is otherwise separate. (i.e. declaring `function xyz()` is equivalent to setting `Mods[ModTable].xyz = function () ...`).
- `Ext.Require` is only callable during bootstrap to avoid lag when loading files / doing startup stuff from the server loop
- `Ext.Require` got a one-argument form; i.e. it is sufficient to call `Ext.Require("wtf.lua")` instead of `Ext.Require("SomeUUID_1111-2222-...", "wtf.lua")` (old version will still work) 


### Migrating from v41 to v42

The client and server Lua contexts were previously unified, i.e. both the server and the client used the same Lua state. After v42 server and client contexts are separated.

The following changes must be observed when migrating to v42:
 - `Bootstrap.lua` is deprecated. The server will load `BootstrapServer.lua`, the client will try to load `BootstrapClient.lua`. If these files cannot be found, `Bootstrap.lua` will be loaded as a fallback. In addition, care must be taken to ensure that `BootstrapServer.lua` only contains code that should be run on the server and `BootstrapClient.lua` only contains code for the client.
 - `Ext.GetHitChance` and `Ext.StatusGetEnterChance` are now events; instead of assigning a single function to them as before (i.e. `Ext.GetHitChance = func`), a listener should be registered (`Ext.RegisterListener("GetHitChance", func)`). For backwards compatibility, during the deprecation period assigning a function to `Ext.GetHitChance` is equivalent to registering that function as a listener.
 - Server-side scripts now only allow registering server-side listeners and vice versa
     - Listeners allowed on the server: 	`SessionLoading`, `ModuleLoading`, `ModuleResume`, `GetSkillDamage`, `ComputeCharacterHit`, `CalculateTurnOrder`, `GetHitChance`, `StatusGetEnterChance`
     - Listeners allowed on the client: 	`SessionLoading`, `ModuleLoading`, `ModuleResume`, `GetSkillDamage`, `SkillGetDescriptionParam`, `StatusGetDescriptionParam`, `GetHitChance`
 - Calling `Ext.EnableStatOverride` is no longer necessary; any calls to this function should be removed
 - The following functions are deleted in client contexts: `Ext.NewCall`, `Ext.NewQuery`, `Ext.NewEvent`, `Ext.GetCharacter`, `Ext.GetItem`, `Ext.GetStatus`, `Ext.GetCombat`, `Ext.GenerateIdeHelpers`


## Client / Server States
<a id="client-server"></a>

Internally the game is split into two components, a client and a server component. When a new game is started/loaded, a new server is created and client connect to this server. The server component is only created on the host; client components are created on both the host and all peers. Because of this, the game technically always runs in multiplayer. Single player is just a special form of multiplayer where only one local peer is connected to the server.

Osiris and behavior scripts (gamescripts) always run on the server. Since Lua has access to features that require client-side code (UI modification, level scaling formulas, status chances, skill damage calculation, etc.) the extender keeps multiple Lua states: one state for the server and one for each client (including the single player "fake client"). These states are completely separated from each other and cannot access the internal state of each other (Lua globals, functions, variables).

Because they run in different environments, server and client states can access a different set of features. Functions/classes in this document are annotated with the following letters, which indicate where they are available:
 - **C** - The function is only available on the client
 - **S** - The function is only available on the server
 - **R** - Restricted; the function is only callable in special contexts/locations


## Console

Extender versions v44 and above allow commands to be entered to the console window.

Press `<enter>` to enter console mode; in this mode the normal log output is disabled to avoid log spam while typing commands.

Client/server context can be selected by typing `client` or `server`. This selects in which Lua environment the console commands will execute. By default the console uses the server context.

Typing `exit` returns to log mode.

Commands prefixed by a `!` will trigger the `ConsoleCommand` Lua event.
Example:
```lua
local function consoleCmd(cmd, a1, a2, ...)
    Ext.Print("Cmd: " .. cmd .. ", args: ", a1, ", ", a2);
end
Ext.RegisterListener("ConsoleCommand", consoleCmd);
```
The command `!test 123 456` will call `consoleCmd("test", 123, 456)` and prints `Cmd: test, args: 123, 456`.

Anything else typed in the console will be executed as Lua code in the current context. (eg. typing `Ext.Print(1234)` will print `123`). 
The console has full access to the underlying Lua state, i.e. server console commands can also call builtin/custom Osiris functions, so Osiris calls like `CharacterGiveReward(CharacterGetHostCharacter(), "CheatShieldsAllRarities", 1)` are possible using the console.
Variables can be used just like in Lua, i.e. variable in one command can later on be used in another console command. Be careful, console code runs in global context, so make sure console variable names don't conflict with globals (i.e. `Mods`, `Ext`, etc.)! Don't use `local` for console variables, since the lifetime of the local will be one console command. (Each console command is technically a separate chunk).


## Calling Lua from Osiris <sup>S</sup>
<a id="calling-lua-from-osiris"></a>

By default, functions defined in Lua are not visible to Osiris. 
During the Lua server bootstrap process, it is possible to declare new functions (calls/queries/events) that will be accessible to Osiris during compilation and execution. Since Osiris only runs on the server, Osiris functions are inaccessible in Lua client states.

Lua functions are registered through the story header (`story_header.div`). This means that each time a function is added, changed or removed, the story header must be regenerated in the editor. (The game regenerates its own story header, so it is always up to date.)

### Types
<a id="l2o_types"></a>

The following table describes how Lua values are converted to Osiris values and vice versa.

| Lua Type | Osiris Type | Notes |
|--|--|--|
| `nil` | - | `nil` is not convertible to an Osiris value, however it has a special meaning when calling/returning from queries (see [Queries](#l2o_queries)). |
| `boolean` | `INTEGER` | Osiris has no true boolean type, it uses the integer value 1 to represent `true` and 0 to represent `false`. |
| `number` (integer) | `INTEGER` | Although Lua only has one `number` type, its internal representation can vary depending on whether it stores an integer or floating point number. |
| `number` (float) | `REAL` | |
| `string` | `STRING`, `GUIDSTRING` | Any `GUIDSTRING` alias (eg. `CHARACTERGUID`, `ITEMGUID`, ...) is also convertible to string. |
| `table` | - | Osiris only supports scalar values, so tables cannot be passed to/from Osiris functions. |



### Calls
<a id="l2o_calls"></a>

Calls can be registered using the `Ext.NewCall(name, parameters)` function. The first parameter is the name of the call to create. The second parameter is the argument list of the Osiris call; it should follow the same syntax that the Osiris story header uses.

It is strongly recommended to follow the Osiris naming scheme, i.e. the name of calls should start with the name prefix of your mod.

Examples:
```lua
-- Call with a single argument
local function TestLog(msg)
    print(msg)
end
Ext.NewCall(TestLog, "NRD_EXT_TestLog", "(STRING)_Msg");

-- Call with multiple arguments
local function Multiply(a, b)
    print(a * b)
end
Ext.NewCall(Multiply, "NRD_EXT_Multiply", "(REAL)_A, (REAL)_B");
```

Functions exported from Lua can be called in Osiris just like any other call:
```c
IF
[...]
THEN
NRD_EXT_TestLog("Test");
NRD_EXT_Multiply(10, 5);
```

### Queries
<a id="l2o_queries"></a>

Unlike `QRY`s defined in Osiris code, Lua queries can return values just like the built-in queries.
Queries can be registered using the `Ext.NewQuery(name, parameters)` function. The first parameter is the name of the query. The second parameter is the argument list of the Osiris query; it should follow the same syntax that the Osiris story header uses.

Queries have two outcomes: they can either succeed or fail. A successful query returns a value for all of its `out` arguments; a failed query doesn't return any values.
To indicate whether a query succeeded or failed, Lua uses the following mechanism:
 - For 0 `out` parameters (i.e. when the query returns no values) the function should return `true` when it succeeded and `false` when it failed.
 - For N (1 or more) `out` parameters the function should return N non-`nil` values when it succeeded and N `nil` values when it failed. It is not permitted to return a mixture of `nil` and non-`nil` values.

The following table summarizes the expected return values:

| Number of params | Result | Return values |
|--|--|--|
| 0 | Successful | `true` |
| 0 | Failed | `false` |
| 1 | Successful | non-`nil` return value |
| 1 | Failed | `nil` |
| 3 | Successful | 3 non-`nil` return values - (v1, v2, v3) |
| 3 | Failed | `nil, nil, nil` |

Example:
```lua
local function Divide(a, b)
    if b == 0 then
        return nil
    end
    return a / b
end
Ext.NewQuery(Divide, "NRD_EXT_Divide", "[in](REAL)_A, [in](REAL)_B, [out](REAL)_Result");
```

Functions exported from Lua can be called in Osiris just like any other call:
```c
IF
[...]
AND
NRD_EXT_Divide(100, 5, _Result)
THEN
[...]
```

### Events
<a id="l2o_events"></a>

New Osiris events can be created by calling `Ext.NewEvent(name, parameters)`. The first parameter is the name of the event. The second parameter is the argument list of the Osiris event; it should follow the same syntax that the Osiris story header uses.

```lua
Ext.NewEvent("NRD_EXT_TestEvent", "(STRING)_Name");
```

Custom events can be thrown by calling them like a function:
```LUA
NRD_EXT_TestEvent("Whatever");
```

### Custom calls/queries
<a id="l2o_custom_calls"></a>

It is possible to call Lua functions by name, without exporting them to the Osiris story header. For this purpose multiple polymorphic functions are provided, `NRD_LuaCall*`, `NRD_ModCall*`, `NRD_LuaQuery*` and `NRD_ModQuery*`.

`NRD_LuaCall(Func, ...)` is a call (i.e. usable from the `THEN` part of the rule) and returns no results. Its first parameter is the function name to call, and it accepts an arbitrary number of arguments to pass to the Lua function. Only functions in the global table can be called this way, i.e. it is meant to be used for mods targeting v42 or below.
`NRD_ModCall(Mod, Func, ...)` is similar to `NRD_LuaCall`, but its first two parameters are the mod name in the global table (i.e. the `ModTable` setting from `OsiToolsConfig.json`) and the function name to call. It calls the function `Mods[Mod].Func(...)` in Lua, i.e. it is meant to be used for mods targeting v43 or above.

```lua
function TestFunc()
    Ext.Print("Woohoo!")
end

-- 2-arg version
function TestFunc2(str, int)
    Ext.Print(str .. ", " .. int)
end
```

```c
// Zero argument call - before v42
NRD_LuaCall("TestFunc");
// v43+
NRD_ModCall("YourMod", "TestFunc");

// Two argument call
[...]
AND
IntegerSum(1, 2, _Sum)
THEN
NRD_LuaCall("TestFunc2", "string arg", (STRING)_Sum);
// v43+
NRD_ModCall("YourMod", "TestFunc2", "string arg", (STRING)_Sum);
```

`NRD_LuaQuery*(Func, ...)` is a query (i.e. usable from the `IF` part of the rule). Its first parameter is the function name to call, and it accepts an arbitrary number of arguments to pass to the Lua function as well as an arbitrary number of results. The last character of the function name indicates the number of IN parameters (i.e. `NRD_LuaQuery2` for a query with 2 input parameters). Only functions in the global table can be called this way, i.e. it is meant to be used for mods targeting v42 or below.
`NRD_ModQuery*(Mod, Func, ...)` is a version of `NRD_LuaQuery` that calls functions from mod-local tables, i.e. it should be used for mods targeting version 43+.

```lua
-- 0 input, 0 output
function TestFunc()
    return "test"
end

-- 2 inputs, 2 outputs
function TestFunc2(str1, str2)
    return str1 .. str2, "something else"
end
```

Using `LuaQuery` (v42):
```c
[...]
AND
// Zero argument, zero return value query
NRD_LuaQuery0("TestFunc")
THEN
[...]

[...]
AND
// Two argument, two return value query
NRD_LuaQuery2("TestFunc2", "asdf", "ghjk", _Out1, _Out2)
THEN
DebugBreak(_Out1);
```

Using `ModQuery` (v43+):
```c
[...]
AND
// Zero argument, zero return value query
NRD_ModQuery0("YourMod", "TestFunc")
THEN
[...]

[...]
AND
// Two argument, two return value query
NRD_ModQuery2("YourMod", "TestFunc2", "asdf", "ghjk", _Out1, _Out2)
THEN
DebugBreak(_Out1);
```


## Calling Osiris from Lua <sup>S</sup>

Lua server contexts have a special global table called `Osi` that contains every Osiris symbol. In addition, built-in functions (calls, queries, events), functions added by the Osiris extender and functions registered from Lua via `Ext.NewCall`, `Ext.NewQuery` and `Ext.NewEvent` are also added to the global table.

### Calls
<a id="o2l_calls"></a>

Simply call the method from Lua with the same parameters:
```lua
-- Built-in functions are in the global table (_G)
CharacterResetCooldowns(player)
-- Equivalent to the above
Osi.CharacterResetCooldowns(player)
```

Implementation detail: Technically, name resolution is only performed when the function is called, since Osiris allows multiple overloads of the same name and the function to call is resolved based on the number of arguments. Because of this, getting any key from the `Osi` table will return an object, even if no function with that name exists. Therefore, `Osi.Something ~= nil` and similar checks cannot be used to determine whether a given Osiris symbol exists.

### Queries
<a id="o2l_queries"></a>

The query behavior is a mirror of the one described in the [Exporting Lua functions to Osiris](#exporting-lua-functions-to-osiris) chapter.

For queries with zero OUT arguments, the function will return a boolean indicating whether the query succeeded (true) or failed (false).
```lua
local succeeded = SysIsCompleted("TestGoal")
```

Queries with OUT arguments will have a number of return values corresponding to the number of OUT arguments. 
```lua
-- Single return value
local player = CharacterGetHostCharacter()
-- Multiple return values
local x, y, z = GetPosition(player)
```

### Events
<a id="o2l_events"></a>

Osiris events can be triggered by calling them like a function. Events are not buffered and the event is triggered synchronously, i.e. the function call returns when every Osiris rule that handles the event has finished.
```lua
StoryEvent(player, "event name")
```

### PROCs
<a id="o2l_procs"></a>

Calling PROCs is equivalent to built-in calls, however they are not added to the global table.

```lua
Osi.Proc_CharacterFullRestore(player)
```

### User Queries
<a id="o2l_qrys"></a>

User queries (`QRY`) behave just like built-in queries do. Since they can't have OUT arguments (i.e. can't return values), the function will just return a boolean indicating whether the query succeeded or not. User queries are not added to the global table.

```lua
local succeeded = Osi.Qry_IsHealingStatus("DAMAGE")
```

### Databases
<a id="o2l_dbs"></a>

Databases can be read using the `Get` method. The method checks its parameters against the database and only returns rows that match the query.

The number of parameters passed to `Get` must be equivalent to the number of columns in the target database. Each parameter defines an (optional) filter on the corresponding column; if the parameter is `nil`, the column is not filtered (equivalent to passing `_` in Osiris). If the parameter is not `nil`, only rows with matching values will be returned.

Example:
```lua
-- Fetch all rows from DB_GiveTemplateFromNpcToPlayerDialogEvent
local rows = Osi.DB_GiveTemplateFromNpcToPlayerDialogEvent:Get(nil, nil, nil)

-- Fetch rows where the first column is CON_Drink_Cup_A_Tea_080d0e93-12e0-481f-9a71-f0e84ac4d5a9
local rows = Osi.DB_GiveTemplateFromNpcToPlayerDialogEvent:Get("CON_Drink_Cup_A_Tea_080d0e93-12e0-481f-9a71-f0e84ac4d5a9", nil, nil)
```

It is possible to insert new tuples to Osiris databases by calling the DB like a function.

```lua
Osi.DB_CharacterAllCrimesDisabled(player)
```

The `Delete` method can be used to delete rows from databases. The number of parameters passed to `Delete` must be equivalent to the number of columns in the target database.
Each parameter defines an (optional) filter on the corresponding column; if the parameter is `nil`, the column is not filtered (equivalent to passing `_` in Osiris). If the parameter is not `nil`, only rows with matching values will be deleted.
Example:
```lua
-- Delete all rows from DB_GiveTemplateFromNpcToPlayerDialogEvent
Osi.DB_GiveTemplateFromNpcToPlayerDialogEvent:Delete(nil, nil, nil)

-- Delete rows where the first column is CON_Drink_Cup_A_Tea_080d0e93-12e0-481f-9a71-f0e84ac4d5a9
Osi.DB_GiveTemplateFromNpcToPlayerDialogEvent:Delete("CON_Drink_Cup_A_Tea_080d0e93-12e0-481f-9a71-f0e84ac4d5a9", nil, nil)
```


# The `Ext` library

## UI

#### Ext.CreateUI(name, path, layer) <sup>C</sup>

Creates a new UI element. Returns the UI object on success and `nil` on failure.
 - `name` is a user-defined unique name that identifies the UI element. To avoid name collisions, the name should always be prefixed with the mod name (e.g. `NRD_CraftingUI`)
 - `path` is the path of the SWF file relative to the data directory (e.g. `"Public/ModName/GUI/CraftingUI.swf"`)
 - `layer` specifies the stack order of the UI element. Overlapping elements with a larger layer value cover those with a smaller one.

#### Ext.GetUI(name) <sup>C</sup>

Retrieves a UI element with the specified name. If no such element exists, the function returns `nil`.

#### Ext.GetBuiltinUI(path) <sup>C</sup>

Retrieves a built-in UI element at the specified path. If no such element exists, the function returns `nil`.

#### Ext.DestroyUI(name) <sup>C</sup>

Destroys the specified UI element.


### Interacting with the UI Element

#### UIObject:Invoke(func, ...) <sup>C</sup>

The `Invoke` method calls a method on the main timeline object of the UI element. The first argument (`func`) is the name of the ActionScript function to call; all subsequent arguments are passed to the ActionScript function as parameters.
Only `string`, `number` and `boolean` arguments are supported.

Example:
```lua
local ui = Ext.GetUI(...)
ui:Invoke("exposedMethod", "test")
```

ActionScript:
```actionscript
function exposedMethod(val: String): * {
	this.testLabel.text = val;
}
```

#### UIObject:ExternalInterfaceCall(func, ...) <sup>C</sup>

The `ExternalInterfaceCall` method simulates an `ExternalInterface.call(...)` call from Flash, i.e. it calls an UI handler function in the game engine. The first argument (`func`) is the name of the UI function to call; all subsequent arguments are passed to the engine as parameters.
Only `string`, `number` and `boolean` arguments are supported.

Example:
```lua
local ui = Ext.GetBuiltinUI("Public/Game/GUI/characterSheet.swf")
ui:ExternalInterfaceCall("show")
```

#### Ext.RegisterUICall(object, name, handler) <sup>C</sup>

The `Ext.RegisterUICall` function registers a listener that is called when the `ExternalInterface.call` function is invoked from ActionScript. ([ExternalInterface docs](https://help.adobe.com/en_US/FlashPlatform/reference/actionscript/3/flash/external/ExternalInterface.html))
 - `object` is the UI object that is returned from `Ext.CreateUI`, `Ext.GetUI` or `Ext.GetBuiltinUI`
 - `name` is the ExternalInterface function name
 - `handler` is a Lua function that is called when the call is fired from Flash. The function receives the UI object and the function name as parameters followed by the arguments passed to the `ExternalInterface.call` call.

Support for capturing ExternalInterface calls on builtin UI elements is available in version v43 (and above).

Example:
```lua
local function handleTextEvent(ui, call, arg1, arg2)
    ...
end

local ui = Ext.GetUI(...)
Ext.RegisterUICall(ui, "sendTextEvent", handleTextEvent)
```

```actionscript
ExternalInterface.call("sendTextEvent", "argument 1", "argument 2");
```

#### Ext.RegisterUIInvokeListener(object, name, handler) <sup>C</sup>

The `Ext.RegisterUIInvokeListener` function registers a listener that is called when the engine invokes a method on the Flash main timeline object.
 - `object` is the UI object that is returned from `Ext.CreateUI`, `Ext.GetUI` or `Ext.GetBuiltinUI`
 - `name` is the Flash method name
 - `handler` is a Lua function that is called when the call is fired. The function receives the UI object and the method name as parameters followed by the arguments passed to the Flash method.

Example:
```lua
local function onHelmetOptionChanged(ui, call, state)
    ...
end

local ui = Ext.GetBuiltinUI(...)
Ext.RegisterUICall(ui, "setHelmetOptionState", onHelmetOptionChanged)
```


#### UIObject:SetValue(name, value, [arrayIndex]) <sup>C</sup>

The `SetValue` method updates the specified public property of the main timeline object. 


#### UIObject:GetValue(name, type, [arrayIndex]) <sup>C</sup>

The `GetValue` method retrieves the specified public property of the main timeline object. 
`type` contains the type of value to retrieve and must be `string`, `number` or `boolean`.


#### UIObject:Show() <sup>C</sup>

Displays the UI element.

#### UIObject:Hide() <sup>C</sup>

Hides the UI element.

#### UIObject:SetPosition(x, y) <sup>C</sup>

Updates the position of the UI element.

Example:
```lua
local ui = Ext.GetUI(...)
UI:SetPosition(100, 100)
```


## Stats

### GetStatEntries(type)

Returns a table with the names of all loaded stat entries.
When the optional parameter `type` is specified, it'll only return stats with the specified type. (The type of a stat entry is specified in the stat .txt file itself (eg. `type "StatusData"`).

### StatGetAttribute(stat, attribute)

Returns the specified `attribute` of the stat entry.
If the stat entry does not exist, the stat entry doesn't have an attribute named `attribute`, or the attribute is not supported, the function returns `nil`.

**Notes:**
 - For enumerations, the function will return the enumeration label (eg. `Corrosive`). See `ModifierLists.txt` or `Enumerations.xml` for a list of enumerations and enumeration labels.
 - The following fields are not supported: `AoEConditions`, `TargetConditions`, `ForkingConditions`, `CycleConditions`, `SkillProperties`, `WinBoost`, `LoseBoost`

`Requirements` and `MemorizationRequirements` are returned in the following format:
```js
[
    {
        "Not" : false, // Negated condition?
        "Param" : 1, // Parameter; number for ability/attribute level, string for Tag
        "Requirement" : "FireSpecialist" // Requirement name
    },
    {
        "Not" : false,
        "Param" : 1,
        "Requirement" : "Necromancy"
    }
]
```

### StatSetAttribute(stat, attribute, value) <sup>R</sup>

Updates the specified `attribute` of the stat entry. This essentially allows on-the-fly patching of stat .txt files from script without having to override the while stat entry.
This function can only be called from a `ModuleLoading` listener; calling it elsewhere throws a Lua error.

**Notes:**
 - For enumerations, the function accepts both the enumeration label (a string value, eg. `Corrosive`) and the enumeration index (an integer value, eg, `7`). See `ModifierLists.txt` or `Enumerations.xml` for a list of enumerations and enumeration labels.
 - Be aware that a lot of number-like attributes are in fact enums. eg. the `Strength`, `Finesse`, `Intelligence`, etc. attributes of `Potion` are enumerations and setting them by passing an integer value to this function will yield unexpected results. For example, calling `StatSetAttribute("SomePotion", "Strength", 5)` will set the `Strength` value to `-9.6`! The proper way to set these values is by passing the enumeration label as string, eg. `StatSetAttribute("SomePotion", "Strength", "5")`
 - The following fields are not supported: `AoEConditions`, `TargetConditions`, `ForkingConditions`, `CycleConditions`, `SkillProperties`, `WinBoost`, `LoseBoost`

Example:
```lua
-- Swap DamageType from Poison to Air on all skills
for i,name in pairs(Ext.GetStatEntries("SkillData")) do
    local damageType = Ext.StatGetAttribute(name, "DamageType")
    if damageType == "Poison" then
        Ext.StatSetAttribute(name, "DamageType", "Air")
    end
end
```

### GetStat(stat, [level])

Returns the specified stats entry as an object for easier manipulation.
If the `level` argument is not specified or is `nil`, the table will contain stat values as specified in the stat entry.
If the `level` argument is not `nil`, the table will contain level-scaled values for the specified level. A `level` value of `-1` will use the level specified in the stat entry.

The behavior of getting a table entry is identical to that of `StatGetAttribute` and setting a table entry is identical to `StatSetAttribute`.

The `StatSetAttribute` example rewritten using `GetStat`:
```lua
-- Swap DamageType from Poison to Air on all skills
for i,name in pairs(Ext.GetStatEntries("SkillData")) do
    local stat = Ext.GetStat(name)
    if stat.DamageType == "Poison" then
        stat.DamageType = "Air"
    end
end
```

### StatAddCustomDescription(stat, attribute, description) <sup>R</sup>

Adds a custom property description to the specified stat entry. (The blue text in the skill description tooltip).
This function can only be called from a `ModuleLoading` listener.

Example:
```lua
Ext.StatAddCustomDescription("Dome_CircleOfProtection", "SkillProperties", "Custom desc one")
```
### StatSetLevelScaling(statType, attribute, func) <sup>R</sup>

Replaces level scaling formula for the specified stat.
This function can only be called from a `ModuleLoading` listener.

`statType` is the type of stat to override (`Character`, `SkillData`, `Potion`, etc). `attribute` is the stat attribute to override (`Strength`, `Constitution`, ...).

`func` must satisfy the following requirements:
 - Must be a Lua function that receives two arguments `(attributeValue, level)` and returns the integer level scaled value.
 - Must have no side effects (i.e. can't set external variables, call external functions, etc)
 - Must always returns the same result when given the same argument values
 - Since the function is called very frequently (up to 50,000 calls during a level load), it should execute as quickly as possible

### GetSkillSet(name)

Returns a table with the names of skills contained within the specified SkillSet.

### GetEquipmentSet(name)

Returns a table with the names of equipment entries contained within the specified EquipmentSet.

### ExtraData

`Ext.ExtraData` is an object containing all entries from `Data.txt`.

*Note*: It is possible to add custom `ExtraData` keys by adding a new `Data.txt` to the mod and then retrieve them using Lua.

Example:
```lua
Ext.Print(Ext.ExtraData.DamageBoostFromAttribute)
```

## Skill/Status Overrides

Using the extender it is possible to replace/override hardcoded game math and behavior. The following hooks are supported:

### Hit Chance

Each time the game calculates hit chance, the Lua event `GetHitChance` is triggered. If a Lua script listens to this event and returns a non-`nil` value from the listener function, the game will use the return value of the custom function as the hit chance. If the function returns `nil` or the function call fails, the game's own hit chance calculation is used.

The following example makes it so the overall Hit Chance is the Attacker's Accuracy minus the Target's Dodge, no multiplicative operations involved:
```lua
local function YourHitChanceFunction(attacker, target)
    local hitChance = attacker.Accuracy - target.Dodge
    -- Make sure that we return a value in the range (0% .. 100%)
    hitChance = math.max(math.min(hitChance, 100), 0)
    return hitChance
end

Ext.RegisterListener("GetHitChance", YourHitChanceFunction)
```

Be aware that the Hit Chance Calculation considers a lot of variables, including checking if the target is incapacitated. To better approximate vanilla behavior, it is recommended to replicate the majority of the features present on the vanilla's code, changing only what you want to change. The complete code is available at: https://gist.github.com/Norbyte/e49cbff75e985f4558f0dbd6969d715c



## Mod Info

### IsModLoaded(modGuid)

Returns whether the module with the specified GUID is loaded.
This is equivalent to Osiris `NRD_IsModLoaded`, but is callable when the Osiris scripting runtime is not yet available (i.e. `ModuleLoading˙, etc events).

Example:
```lua
if (Ext.IsModLoaded("5cc23efe-f451-c414-117d-b68fbc53d32d"))
    Ext.Print("Mod loaded")
end
```

### GetModLoadOrder()

Returns the list of loaded module UUIDs in the order they're loaded in.

### GetModInfo(modGuid)

Returns detailed information about the specified (loaded) module.
Example:
```lua
local loadOrder = Ext.GetModLoadOrder()
for k,uuid in pairs(loadOrder) do
    local mod = Ext.GetModInfo(uuid)
    Ext.Print(Ext.JsonStringify(mod))
end
```

Output:
```json
{
    "Author" : "Larian Studios",
    "Dependencies" :
    [
        "2bd9bdbe-22ae-4aa2-9c93-205880fc6564",
        "eedf7638-36ff-4f26-a50a-076b87d53ba0"
    ],
    "Description" : "",
    "Directory" : "DivinityOrigins_1301db3d-1f54-4e98-9be5-5094030916e4",
    "ModuleType" : "Adventure",
    "Name" : "Divinity: Original Sin 2",
    "PublishVersion" : 905969667,
    "UUID" : "1301db3d-1f54-4e98-9be5-5094030916e4",
    "Version" : 372645092
}
```

## Server Characters <sup>S</sup>
<a id="server-characters"></a>

Characters in server contexts can be retrieved using the `Ext.GetCharacter(ref)` call. The function accepts a character GUID, a NetID or an ObjectHandle. If the character cannot be found, the return value is `nil`; otherwise a Character object is returned.

Player objects have the following properties:

| Name | Type | Notes |
|--|--|--|
| Stats | userdata | See [CharacterStats](#character-stats) |
| PlayerCustomData | userdata | See [PlayerCustomData](#player-custom-data) |
| NetID | integer | Network ID of the character |
| MyGuid | string | GUID of the character |
| WorldPos | number[3] | Position of the character |
| CurrentLevel | String | Name of level (map) the character is currently on |
| Scale | number |  |
| AnimationOverride | string |  |
| WalkSpeedOverride | integer |  |
| RunSpeedOverride | integer |  |
| NeedsUpdateCount | integer |  |
| ScriptForceUpdateCount | integer |  |
| ForceSynchCount | integer |  |
| SkillBeingPrepared | string |  |
| LifeTime | number | Used for summons to indicate lifetime |
| PartialAP | number | Movement AP |
| AnimType | integer |  |
| DelayDeathCount | integer |  |
| AnimationSetOverride | string |  |
| CustomTradeTreasure | string |  |
| Archetype | string |  |
| EquipmentColor | string |  |
| IsPlayer | boolean |  |
| Multiplayer | boolean |  |
| InParty | boolean |  |
| HostControl | boolean |  |
| OffStage | boolean |  |
| Dead | boolean |  |
| HasOwner | boolean  |  |
| InDialog | boolean |  |
| Summon | boolean  |  |
| CharacterControl | boolean |  |
| Loaded | boolean  |  |
| InArena | boolean |  |
| CharacterCreationFinished | boolean |  |
| Floating | boolean |  |
| SpotSneakers | boolean |  |
| WalkThrough | boolean |  |
| CoverAmount | boolean |  |
| CanShootThrough | boolean |  |
| PartyFollower | boolean |  |
| Totem | boolean  |  |
| NoRotate | boolean  |  |
| IsHuge | boolean  |  |
| Global | boolean |  |
| HasOsirisDialog | boolean |  |
| HasDefaultDialog | boolean |  |
| TreasureGeneratedForTrader | boolean |  |
| Trader | boolean |  |
| Resurrected | boolean |  |
| IsPet | boolean |  |
| IsSpectating | boolean |  |
| NoReptuationEffects | boolean |  |
| HasWalkSpeedOverride | boolean |  |
| HasRunSpeedOverride | boolean |  |
| IsGameMaster | boolean  |  |
| IsPossessed | boolean |  |


## Player Custom Data
<a id="player-custom-data"></a>

Contains player customization info. Properties:

| Name | Type | 
|--|--|
| CustomLookEnabled | boolean |
| Name | string |
| ClassType | string |
| SkinColor | integer |
| HairColor | integer |
| ClothColor1 | integer |
| ClothColor2 | integer |
| ClothColor3 | integer |
| IsMale | boolean |
| Race | string |
| OriginName | string |
| Icon | string |
| MusicInstrument | string |
| OwnerProfileID | string |
| ReservedProfileID | string |
| AiPersonality | string |
| Speaker | string |


## Character Stats
<a id="character-stats"></a>

Represents all stats of a character (both players and non-players). Unlike Character objects which are different on the server and the client, the same Character Stats objects are present on both ends.

| Name | Type | Notes |
|--|--|--|
| DynamicStats | table | A table containing all dynamic stat entries for the character. See [CharacterDynamicStats](#character-dynamic-stats) for details on what these are. |
| MainWeapon | userdata | Currently equipped main hand weapon (or `nil` if none is equipped). See [ItemStats](#item-stats) |
| OffHandWeapon | userdata | Currently equipped off-hand weapon (or `nil` if none is equipped). See [ItemStats](#item-stats) |

To access items in other slots, use the `character:GetItemBySlot(slot)` method. The `slot` name must be one of `Helmet`, `Breast`, `Leggings`, `Weapon`, `Shield`, `Ring`, `Belt`, `Boots`, `Gloves`, `Amulet`, `Ring2` `Wings`, `Horns`, `Overhead`.

It is possible to fetch the base and computed values of the following stats. To get the base value (returns base points + permanent boosts + talent bonuses), add a `Base` prefix to the field (i.e. `BasePhysicalResistance` instead of `PhysicalResistance`); to get the computed value, just use the name (i.e. `PhysicalResistance`).

| Name | Type | Notes |
|--|--|--|
| MaxMp | integer | Max Source points |
| APStart | integer | |
| APRecovery | integer | |
| APMaximum | integer | |
| Strength | integer | |
| Finesse | integer | |
| Intelligence | integer | |
| Constitution | integer | |
| Memory | integer | |
| Wits | integer | |
| Accuracy | integer | |
| Dodge | integer | |
| CriticalChance | integer | |
| FireResistance | integer | |
| EarthResistance | integer | |
| WaterResistance | integer | |
| AirResistance | integer | |
| PoisonResistance | integer | |
| ShadowResistance | integer | |
| CustomResistance | integer | |
| PhysicalResistance | integer | |
| PiercingResistance | integer | |
| CorrosiveResistance | integer | |
| MagicResistance | integer | |
| LifeSteal | integer | |
| Sight | integer | |
| Hearing | integer | |
| Movement | integer | |
| Initiative | integer | |
| BlockChance | integer | |
| ChanceToHitBoost | integer | |
| DamageBoost | integer | |

Talents can be queried using the field name `TALENT_` + the talent name (e.g. `character.TALENT_Bully`). For a list of talents, see [https://docs.larian.game/Talent_list](https://docs.larian.game/Talent_list)

Abilities can be queried using their name (e.g. `character.WarriorLore`). For a list of ability names see the `Ability` enumeration in `Enumerations.xml`.


## Character Dynamic Stats
<a id="character-dynamic-stats"></a>

Character stats are calculated from multiple different sources (base stats, potions, statuses, etc.). Each of these sources is stored as a dynamic stat.

Dynamic stat index `1` always contains character base stats, index `2` contains permanent boosts.

| Name | Type | Notes |
|--|--|--|
| SummonLifelinkModifier | integer | |
| Strength | integer | |
| Memory | integer | |
| Intelligence | integer | |
| Movement | integer | |
| MovementSpeedBoost | integer | |
| Finesse | integer | |
| Wits | integer | |
| Constitution | integer | |
| Willpower | integer | |
| Bodybuilding | integer | |
| FireResistance  | integer | |
| EarthResistance | integer | |
| WaterResistance | integer | |
| AirResistance | integer | |
| PoisonResistance | integer | |
| ShadowResistance | integer | |
| PiercingResistance | integer | |
| PhysicalResistance | integer | |
| CorrosiveResistance | integer | |
| MagicResistance | integer | |
| CustomResistance | integer | |
| Sight | integer | |
| Hearing | integer | |
| FOV | integer | |
| APMaximum | integer | |
| APStart | integer | |
| APRecovery | integer | |
| CriticalChance | integer | |
| Initiative | integer | |
| CriticalChance | integer | |
| Vitality | integer | |
| VitalityBoost | integer | |
| MagicPoints | integer | |
| Level | integer | |
| Gain | integer | |
| Armor | integer | |
| MagicArmor | integer | |
| ArmorBoost | integer | |
| MagicArmorBoost | integer | |
| ArmorBoostGrowthPerLevel | integer | |
| MagicArmorBoostGrowthPerLevel | integer | |
| DamageBoost | integer | |
| DamageBoostGrowthPerLevel | integer | |
| Accuracy | integer | |
| Dodge | integer | |
| MaxResistance | integer | |
| LifeSteal | integer | |
| Weight | integer | |
| ChanceToHitBoost | integer | |
| RangeBoost | integer | |
| APCostBoost | integer | |
| SPCostBoost | integer | |
| MaxSummons | integer | |
| BonusWeaponDamageMultiplier | integer | |
| TranslationKey | string | |
| BonusWeapon | string | |


## Server Items <sup>S</sup>
<a id="server-items"></a>

Items on the server can be retrieved using the `Ext.GetItem(ref)` call. The function accepts an item GUID or an ObjectHandle. If the item cannot be found, the return value is `nil`; otherwise an Item object is returned.

Items have the following properties:

| Name | Type | Notes |
|--|--|--|
| Stats | userdata | See [ItemStats](#item-stats) |
| PlayerCustomData | userdata | See [PlayerCustomData](#player-custom-data) |
| NetID | integer | Network ID of the item |
| MyGuid | string | GUID of the item |
| WorldPos | vec3 | Position of the item |
| CurrentLevel | String | Name of level (map) the item is currently on |
| Scale | number |  |
| CustomDisplayName | string | |
| CustomDescription | string | |
| CustomBookContent | string | |
| StatsId | string | Stats entry (eg. `WPN_Dagger`) |
| Slot | integer | |
| Amount | integer | |
| Vitality | integer | |
| Armor | integer | |
| InUseByCharacterHandle | integer | Character currently using the item |
| Key | string | Key used to open the container |
| LockLevel | integer | |
| OwnerHandle | integer | ObjectHandle to the owner of this item |
| ComputedVitality | integer | |
| ItemType | integer | |
| GoldValueOverwrite | integer | |
| WeightValueOverwrite | integer | |
| TreasureLevel | integer | |
| LevelOverride | integer | |
| ForceSynch | boolean | |


## Item Stats
<a id="item-stats"></a>

Represents all stats of an item. Unlike Item objects which are different on the server and the client, the same Item Stats objects are present on both ends.

| Name | Type | Notes |
|--|--|--|
| DynamicStats | table | A table containing all dynamic stat entries for the item. See [ItemDynamicStats](#item-dynamic-stats) for details on what these are. |
| ItemType | string | `EquipmentStatType` value |
| ItemSlot | integer | |
| WeaponType | string | `WeaponType` value |
| AnimType | integer | |
| WeaponRange | number | |
| IsIdentified | boolean | |
| IsTwoHanded | boolean | |
| ShouldSyncStats | boolean | |
| HasModifiedSkills | boolean | |
| Skills | string | |
| DamageTypeOverwrite | string | `DamageType` value |
| Durability | integer | |
| DurabilityCounter | integer | |
| ItemTypeReal | string | |
| MaxCharges | integer | |
| Charges | integer | |

Immunity/attribute flags from the [AttributeFlags enumeration](#attributeflags) can be retrieved using their name (i.e. `stats.KnockdownImmunity`).


## Item Dynamic Stats
<a id="item-dynamic-stats"></a>

Item stats are calculated from multiple different sources (base stats, permanent boosts, runes, deltamods, etc.). Each of these sources is stored as a dynamic stat.

Dynamic stat index `1` always contains item base stats, index `2` contains permanent boosts, indices `3` to `5` are rune slots.

| Name | Type | Notes |
|--|--|--|
| Durability| integer | |
| DurabilityDegradeSpeed | integer | |
| StrengthBoost | integer | |
| FinesseBoost | integer | |
| IntelligenceBoost | integer | |
| ConstitutionBoost | integer | |
| MemoryBoost | integer | |
| WitsBoost | integer | |
| SightBoost | integer | |
| HearingBoost | integer | |
| VitalityBoost | integer | |
| SourcePointsBoost | integer | |
| MaxAP | integer | |
| StartAP | integer | |
| APRecovery | integer | |
| AccuracyBoost | integer | |
| DodgeBoost | integer | |
| LifeSteal | integer | |
| CriticalChance | integer | |
| ChanceToHitBoost | integer | |
| MovementSpeedBoost | integer | |
| RuneSlots | integer | |
| RuneSlots_V1 | integer | |
| FireResistance  | integer | |
| EarthResistance | integer | |
| WaterResistance | integer | |
| AirResistance | integer | |
| PoisonResistance | integer | |
| ShadowResistance | integer | |
| PiercingResistance | integer | |
| PhysicalResistance | integer | |
| CorrosiveResistance | integer | |
| MagicResistance | integer | |
| CustomResistance | integer | |
| Movement | integer | |
| Initiative | integer | |
| Willpower | integer | |
| Bodybuilding | integer | |
| MaxSummons | integer | |
| Value | integer | |
| Weight | integer | |
| Skills | string | |
| ItemColor | string | |
| ObjectInstanceName | string | |
| BoostName | string | |
| StatsType | string | (See `EquipmentStatsType` enumeration) 

Weapon-only properties:
| Name | Type | Notes |
|--|--|--|
| DamageType | string | (See `DamageType` enumeration) |
| MinDamage | integer | |
| MaxDamage | integer | |
| DamageBoost | integer | |
| DamageFromBase | integer | |
| CriticalDamage | integer | |
| WeaponRange | integer | |
| CleaveAngle | integer | |
| CleavePercentage | integer | |
| AttackAPCost | integer | |

Shield-only properties:
| Name | Type | Notes |
|--|--|--|
| ArmorValue | integer | |
| ArmorBoost | integer | |
| MagicArmorValue | integer | |
| MagicArmorBoost | integer | |
| Blocking | integer | |

Armor-only properties:
| Name | Type | Notes |
|--|--|--|
| ArmorValue | integer | |
| ArmorBoost | integer | |
| MagicArmorValue | integer | |
| MagicArmorBoost | integer | |


## Projectiles <sup>S</sup>
<a id="server-projectiles"></a>

Currently projectiles are only available when passed as parameters to event listeners (`GetSkillDamage`, `ComputeCharacterHit`, etc.), and are not retrievable otherwise.

They have the following properties:

| Name | Type | Notes |
|--|--|--|
| NetID | integer | Network ID of the projectile |
| MyGuid | string | GUID of the projectile |
| CasterHandle | integer | |
| SourceHandle | integer | |
| TargetObjectHandle | integer | |
| HitObjectHandle | integer | |
| SourcePosition | vec3 | |
| TargetPosition | vec3 | |
| DamageType | string | (See `DamageType` enumeration) |
| DamageSourceType | string | (See `CauseType` enumeration) |
| LifeTime | number |  |
| HitInterpolation | number |  |
| ExplodeRadius0 | number |  |
| ExplodeRadius1 | number |  |
| DeathType | string | (See `DeathType` enumeration) |
| SkillId | string |  |
| WeaponHandle | integer | |
| MovingEffectHandle | integer |  |
| SpawnEffect | string |  |
| SpawnFXOverridesImpactFX | boolean |  |
| EffectHandle | integer |  |
| RequestDelete | boolean |  |
| Launched | boolean |  |
| IsTrap | boolean |  |
| UseCharacterStats | boolean |  |
| ReduceDurability | boolean |  |
| AlwaysDamage | boolean |  |
| ForceTarget | boolean  |  |
| IsFromItem | boolean |  |
| DivideDamage | boolean  |  |
| IgnoreRoof | boolean |  |
| CanDeflect | boolean  |  |
| IgnoreObjects | boolean |  |
| CleanseStatuses | string |  |
| StatusClearChance | number |  |
| Position | vec3 |  |
| PrevPosition | vec3 |  |
| Velocity | vec3 |  |
| Scale | number|  |
| CurrentLevel | string |  |


## Server Statuses <sup>S</sup>
<a id="server-statuses"></a>

Properties available on all statuses:

| Property | Type | Notes |
|--|--|--|--|
| NetID | integer | Network ID of the status. Since status have no GUID, only the NetID can be used for sending status references between the client and the server. |
| StatusId | string | Name of the associated stat entry |
| StatusHandle | integer | Handle of this status |
| TargetHandle | integer | Character or item that the status was applied to |
| StatusSourceHandle | integer | Character or item that caused the status |
| StartTimer | number |  |
| LifeTime | number | Total lifetime of the status, in seconds. -1 if the status does not expire. |
| CurrentLifeTime | number | Remaining lifetime of the status, in seconds. |
| TurnTimer | number | Elapsed time in the current turn (0..6) |
| Strength | number |  |
| StatsMultiplier | number |  |
| CanEnterChance | integer | Chance of entering status (between 0 and 100) |
| DamageSourceType | string | Cause of status (See `DamageSourceType` enum) |
| KeepAlive | boolean |  |
| IsOnSourceSurface | boolean |  |
| IsFromItem | boolean |  |
| Channeled | boolean |  |
| IsLifeTimeSet | boolean | Does the status have a lifetime or is it infinite? |
| InitiateCombat | boolean |  |
| Influence | boolean |  |
| BringIntoCombat | boolean |  |
| IsHostileAct | boolean |  |
| IsInvulnerable | boolean | The status turns the character invulnerable |
| IsResistingDeath | boolean | The character can't die until the status expires |
| ForceStatus | boolean | Bypass immunity and status enter chance checks. |
| ForceFailStatus | boolean | Forces prerequisite checks to fail. |
| RequestDelete | boolean | The status is being deleted (i.e. it's not active anymore) |
| RequestDeleteAtTurnEnd | boolean | The status will be deleted at the end of the current turn |
| Started | boolean |  |

### `CONSUME` status properties

| Property | Type | Notes |
|--|--|--|--|
| ResetAllCooldowns | boolean |  |
| ResetOncePerCombat | boolean |  |
| ScaleWithVitality | boolean |  |
| LoseControl | boolean |  |
| ApplyStatusOnTick | boolean |  |
| EffectTime | number |  |
| StatsId | string |  |
| StackId | string |  |
| OriginalWeaponStatsId | string |  |
| OverrideWeaponStatsId | string |  |
| OverrideWeaponHandle | integer |  |
| SavingThrow | string |  |
| SourceDirection | vec3 |  |
| Turn | integer | |
| HealEffectOverride | string | See `HealEffect` enumeration |

### `HIT` status properties

| Property | Type | Notes |
|--|--|--|--|
| SkillId | string | Stats ID of the skill (`SkillData`) that was used for the attack |
| HitByHandle | integer |  |
| HitWithHandle | integer |  |
| WeaponHandle | integer |  |
| HitReason | integer |  |
| Interruption | boolean |  |
| AllowInterruptAction | boolean |  |
| ForceInterrupt | boolean |  |
| DecDelayDeathCount | boolean |  |
| ImpactPosition | vec3 |  |
| ImpactOrigin | vec3 |  |
| ImpactDirection | vec3 |  |
| Equipment | integer | |
| TotalDamage | integer | Sum of all damages |
| DamageDealt | integer | Damage dealt after ApplyDamage |
| DeathType | string | See `Death Type` enumeration |
| DamageType | string | See `Damage Type` enumeration |
| AttackDirection | string | See `AttackDirection` enumeration. |
| ArmorAbsorption | integer | Armor points consumed during attack |
| LifeSteal | integer |  |
| HitWithWeapon | boolean |  |
| Hit | boolean | The attack hit |
| Blocked | boolean | The attack was blocked |
| Dodged | boolean | The attack was dodged |
| Missed | boolean | The attack missed |
| CriticalHit | boolean |  |
| AlwaysBackstab | boolean | Equivalent to the `AlwaysBackstab` skill property |
| FromSetHP | boolean | Indicates that the hit was called from `CharacterSetHitpointsPercentage` (or similar) |
| DontCreateBloodSurface | boolean | Avoids creating a blood surface when the character is hit |
| Reflection | boolean |  |
| NoDamageOnOwner | boolean |  |
| FromShacklesOfPain | boolean |  |
| DamagedMagicArmor | boolean | Indicates that the hit damaged magic armor |
| DamagedPhysicalArmor | boolean | Indicates that the hit damaged physical armor |
| DamagedVitality | boolean | Indicates that the hit damaged the characters vitality |
| Flanking | boolean | |
| PropagatedFromOwner | boolean |  |
| Surface | boolean | The hit is from a surface (`HitType` was `Surface`) |
| DoT | boolean | The hit is from a DoT attack (`HitType` was `DoT`) |
| ProcWindWalker | boolean | Hit should proc the Wind Walker talent |
| CounterAttack | boolean | Counterattack triggered by Gladiator talent |
| Poisoned | boolean | Character was poisoned when hit |
| Bleeding | boolean | Character was bleeding when hit |
| Burning | boolean | Character was burning when hit |
| NoEvents | boolean | Don't throw `OnHit`/`OnPrepareHit` events for this hit |

### `DAMAGE` status properties

| Property | Type | Notes |
|--|--|--|--|
| DamageEvent | integer |  |
| HitTimer | number |  |
| TimeElapsed | number | |
| DamageLevel | integer | |
| DamageStats | string |  |
| SpawnBlood | boolean | |

### `DAMAGE_ON_MOVE` status properties

| Property | Type | Notes |
|--|--|--|--|
| DistancePerDamage | number | |
| DistanceTraveled | number | |

### `HEAL` status properties

| Property | Type | Notes |
|--|--|--|--|
| EffectTime | number |  |
| HealAmount | integer |  |
| HealEffect | string |  |
| HealEffectId | string | Default `RS3_FX_GP_ScriptedEvent_Regenerate_01` |
| HealType | string | See `StatusHealType` enumeration |
| AbsorbSurfaceRange | integer |  |
| TargetDependentHeal | boolean |  |


### `HEALING` status properties

| Property | Type | Notes |
|--|--|--|--|
| HealAmount | integer |  |
| TimeElapsed | number |  |
| HealEffect | string | See `HealEffect` enumeration |
| HealEffectId | string | Default `RS3_FX_GP_ScriptedEvent_Regenerate_01` |
| SkipInitialEffect | boolean | |
| HealingEvent | integer |  |
| HealStat | string | See `HealType` enumeration |
| AbsorbSurfaceRange | integer |  |


## Combat <sup>S</sup>
<a id="combat"></a>

Each combat in-game is represented by a Combat object in Lua. 

Properties:

| Name | Type | Notes |
|--|--|--|
| CombatId | integer | A number identifying the combat instance |
| LevelName | string | Level where the combat is taking place |
| IsActive | boolean |  |


Methods:

#### GetAllTeams() <sup>S</sup>
Retrieves all participants of the combat. The return value is a table of `Team` objects.

#### GetCurrentTurnOrder() <sup>S</sup>
Retrieves the turn order of the current round. The return value is a table of `Team` objects.

#### GetNextTurnOrder() <sup>S</sup>
Retrieves the turn order of the next round. The return value is a table of `Team` objects.

#### UpdateCurrentTurnOrder(turnOrder) <sup>S</sup>
Updates the turn order of the current round. The `turnOrder` argument should be a reordered version of the table returned by `GetCurrentTurnOrder()`. 

Notes:
 - It is possible to remove or add characters to the current turn by adding/removing their `Team` object from the table. 
 - It is possible to add a character to the current turn more than once, the character will only appear once in the UI however.
 - The character whose turn is currently active (the very first item) should not be removed or reordered. This only applies for `GetCurrentTurnOrder`, the first item can be freely reordered in `GetNextTurnOrder`.
 - Changed performed using this function are synchronized to the client at the end of the current server tick.

#### UpdateNextTurnOrder(turnOrder) <sup>S</sup>
Updates the turn order of the next round. The `turnOrder` argument should be a reordered version of the table returned by `GetNextTurnOrder()`. 

Notes:
 - It is possible to remove or add characters to the next turn by adding/removing their `Team` object from the table. 
 - It is possible to add a character to the next turn more than once, the character will only appear once in the UI however.
 - Changed performed using this function are synchronized to the client at the end of the current server tick.

### CalculateTurnOrder Event <sup>S R</sup>

When the turn order of the next round of a combat is being updated for some reason (new round, character entered combat, etc.) the `CalculateTurnOrder` Ext event is thrown. 
The event receives two parameters:
 - `combat`: the Combat object
 - `order`: the turn order determined by the game (equivalent to calling `combat:GetNextTurnOrder()`)

To change the turn order, reorder the `order` table and return it from the event handler. To leave the turn order untouched, return `nil` (or nothing).

```lua
-- Example for calculating an initiative-based turn order
local function CalcInitiativeTurnOrder(combat, order)
    table.sort(order, function (a, b)
        return a.Initiative > b.Initiative
    end)
    return order
end

Ext.RegisterListener("CalculateTurnOrder", CalcInitiativeTurnOrder)
```

### Team <sup>S</sup>

A `Team` is a combat participant (either a character or an item).

Properties:

| Name | Type | Notes |
|--|--|--|
| TeamId | integer | A number identifying the team instance |
| CombatId | integer | Identifies which combat the team is a participant of |
| Initiative | integer | Computed initiative value of the team |
| StillInCombat | boolean | Can the team currently fight, or is it temporarily out of combat? |
| Character | esv::Character | Character object if the team is a character; `nil` otherwise |
| Item | esv::Item | Item object if the team is an item; `nil` otherwise |


## Damage Lists

A damage list is an object that stores the amount of damage dealt for each damage type (`Physical`, `Poison`, etc.).
It is currently used by the `GetSkillDamage` and `ComputeCharacterHit` events to fetch damage information.

Damage lists can be created using the `Ext.NewDamageList()` function.

Methods:

#### Add(damageType, amount)
Increase/decrease the amount of damage in the list. Positive values add, negative values remove damage.
```lua
list:Add("Physical", 10) -- Add 10 points of physical damage
list:Add("Physical", -5) -- Subtract 5 points of physical damage
```

#### Clear([damageType])
Clears the damage list. If `damageType` is specified, only damages with the specified type will be removed.
```lua
list:Clear() -- Remove all
list:Clear("Poison") -- Remove only poison damage
```

#### Multiply(amount)
Multiplies the amount of damage with the specified value.
```lua
list:Add("Physical", 10)
list:Multiply(2.5) -- Physical damage amount is now 25
```

#### Merge(list)
Merge the damage values in the second list into the first one.
```lua
list:Add("Physical", 15)
local list2 = Ext.NewDamageList()
list2:Add("Physical", 25)
list:Merge(list2) -- Physical damage amount is now 40
```

#### ConvertDamageType(damageType)
Converts the damage type of every item in the damage list to the specified value.
```lua
list:Add("Physical", 15)
list:Add("Piercing", 15)
list:ConvertDamageType("Poison") -- Poison damage amount is now 30
```

#### ToTable()
Returns a table containing every item in the list.
```lua
for i,damage in pairs(list:ToTable()) do
    Ext.Print(damage.DamageType .. ": " .. damage.Amount)
end
```

## Utility functions
<a id="ext-utility"></a>

#### Ext.Require(path) <sup>R</sup>

The `Ext.Require` function is the extender's version of the Lua built-in `require` function. 
The function checks if the file at `Mods/<ModuleUUID>/Story/RawFiles/Lua/<path>` was already loaded; if not, it'll load the file, store the return value of the main chunk and return it to the caller. If the file was already loaded, it'll return the stored return value.
**Note:** `Ext.Require` should only be called during module startup (i.e. when loading `BootstrapClient.lua` or `BoostrapServer.lua`). Loading Lua files after module startup is deprecated.

#### Ext.Print(...)

Prints the specified value(s) to the debug console. Works similarly to the built-in Lua `print()`, except that it also logs the printed messages to the editor messages pane.

#### Ext.GetTranslatedString(key[, fallback])

Returns the text associated with the specified translated string key. If the key doesn't exist, the value of `fallback` is returned. If no fallback value is specified, an empty string (`""`) is returned.

```lua
local str = Ext.GetTranslatedString("h17edbbb2g9444g4c79g9409gdb8eb5731c7c", "[1] cast [2] on the ground")
```

#### Ext.AddPathOverride(originalPath, newPath)

Redirects file access from `originalPath` to `newPath`. This is useful for overriding built-in files or resources that are otherwise not moddable, eg. UI Flash files.
Make sure that the override is added as early as possible (preferably in `ModuleLoading`), as adding path overrides after the game has already loaded the resource has no effect.

Example:
```lua
Ext.AddPathOverride("Public/Game/GUI/enemyHealthBar.swf", "Public/YourMod/GUI/enemyHealthBar.swf")
```


## JSON Support

Two functions are provided for parsing and building JSON documents, `Ext.JsonParse` and `Ext.JsonStringify`.

Lua types are encoded in JSON (and vice versa) using the following table:

| Lua Type | JS Type |
|--|--|
| `nil` | `null` |
| `boolean` | `boolean` |
| `number` (integer) | `number` |
| `number` (float) | `number` |
| `string` | `string` |
| `table` (sequential keys) | `array` |
| `table` (non-sequential) | `object` |

It is not possible to stringify/parse `lightuserdata`, `userdata`, `function` and `thread` values.

Since JSON only supports string object keys, Lua `number` (integer/float) keys are saved as `string`.

Usage example:
```lua
local tab = {
    asd = 1234,
    arr = {
        "ab", "bc", 44
    }
}

local json = Ext.JsonStringify(tab)
Ext.Print(json)

local decoded = Ext.JsonParse(json)
Ext.Print(decoded.arr[1])
```

Expected output:
```
{
    "arr": [
        "ab",
        "bc",
        44
    ],
    "asd" : 1234
}

ab
```



### TODO
 - Status chance overrides, Damage calc override, Skill/Status tooltip callbacks
 - GetCharacter, GetItem, GetProjectile, GetGameObject, GetStatus (client/server) + updated property maps
 - Lua state lifetime, Globals behavior (not saved)
 - File IO
 - Bootstrap phase
 - Reloading Lua and changing exports
 - Reloading Story
