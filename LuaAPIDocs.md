### Lua API v43 Documentation

### Table of Contents  

 - [Migrating from v41 to v42](#migrating-from-v41-to-v42)
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
    * [Mod Info](#mod-info)
    * [Combat](#combat)
    * [Damage lists](#damage-lists)
    * [Overwriting Native Functions](#overwriting-native-functions)
        * [Hit Chance](#31o_hit_chance)
    * [JSON Support](#json-support)


## Migrating from v41 to v42

The client and server Lua contexts were previously unified, i.e. both the server and the client used the same Lua state. After v42 server and client contexts are separated.

The following changes must be observed when migrating to v42:
 - `Bootstrap.lua` is deprecated. The server will load `BootstrapServer.lua`, the client will try to load `BootstrapClient.lua`. If these files cannot be found, `Bootstrap.lua` will be loaded as a fallback. In addition, care must be taken to ensure that `BootstrapServer.lua` only contains code that should be run on the server and `BootstrapClient.lua` only contains code for the client.
 - `Ext.GetHitChance` and `Ext.StatusGetEnterChance` are now events; instead of assigning a single function to them as before (i.e. `Ext.GetHitChance = func`), a listener should be registered (`Ext.RegisterListener("GetHitChance", func)`). For backwards compatibility, during the deprecation period assigning a function to `Ext.GetHitChance` is equivalent to registering that function as a listener.
 - Server-side scripts now only allow registering server-side listeners and vice versa
     - Listeners allowed on the server: 	`SessionLoading`, `ModuleLoading`, `ModuleResume`, `GetSkillDamage`, `ComputeCharacterHit`, `CalculateTurnOrder`, `GetHitChance`, `StatusGetEnterChance`
     - Listeners allowed on the client: 	`SessionLoading`, `ModuleLoading`, `ModuleResume`, `GetSkillDamage`, `SkillGetDescriptionParam`, `StatusGetDescriptionParam`, `GetHitChance`
 - Calling `Ext.EnableStatOverride` is no longer necessary; any calls to this function should be removed
 - The following functions are deleted in client contexts: `Ext.NewCall`, `Ext.NewQuery`, `Ext.NewEvent`, `Ext.GetCharacter`, `Ext.GetItem`, `Ext.GetStatus`, `Ext.GetCombat`, `Ext.GenerateIdeHelpers`


## Calling Lua from Osiris

By default, functions defined in Lua are not visible to Osiris. 
During the Lua bootstrap process, it is possible to declare new functions (calls/queries/events) that will be accessible to Osiris during compilation and execution.

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

It is possible to call Lua functions by name, without exporting them to the Osiris story header. For this purpose two polymorphic functions are provided, `NRD_LuaCall` and `NRD_LuaQuery*`.

`NRD_LuaCall` is a call (i.e. usable from the `THEN` part of the rule) and returns no results. Its first parameter is the function name to call, and it accepts an arbitrary number of arguments to pass to the Lua function. Only functions in the global table can be called this way.

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
// Zero argument call
NRD_LuaCall("TestFunc")

// Two argument call
[...]
AND
IntegerSum(1, 2, _Sum)
THEN
NRD_LuaCall("TestFunc2", "string arg", (STRING)_Sum)
```

`NRD_LuaQuery*` is a query (i.e. usable from the `IF` part of the rule). Its first parameter is the function name to call, and it accepts an arbitrary number of arguments to pass to the Lua function as well as an arbitrary number of results. The last character of the function name indicates the number of IN parameters (i.e. `NRD_LuaQuery2` for a query with 2 input parameters). Only functions in the global table can be called this way.

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

## Calling Osiris from Lua

Lua ha a special global table called `Osi` that contains every Osiris symbol. In addition, built-in functions (calls, queries, events), functions added by the Osiris extender and functions registered from Lua via `Ext.NewCall`, `Ext.NewQuery` and `Ext.NewEvent` are also added to the global table.

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
<a name="o2l_procs"></a>

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

#### Ext.CreateUI(name, path, layer)

Creates a new UI element. Returns the UI object on success and `nil` on failure.
 - `name` is a user-defined unique name that identifies the UI element. To avoid name collisions, the name should always be prefixed with the mod name (e.g. `NRD_CraftingUI`)
 - `path` is the path of the SWF file relative to the data directory (e.g. `"Public/ModName/GUI/CraftingUI.swf"`)
 - `layer` specifies the stack order of the UI element. Overlapping elements with a larger layer value cover those with a smaller one.

#### Ext.GetUI(name)

Retrieves a UI element with the specified name. If no such element exists, the function returns `nil`.

#### Ext.GetBuiltinUI(path)

Retrieves a built-in UI element at the specified path. If no such element exists, the function returns `nil`.

#### Ext.DestroyUI(name)

Destroys the specified UI element.


### Interacting with the UI Element

#### UIObject:Invoke(func, ...)

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

#### UIObject:ExternalInterfaceCall(func, ...)

The `ExternalInterfaceCall` method simulates an `ExternalInterface.call(...)` call from Flash, i.e. it calls an UI handler function in the game engine. The first argument (`func`) is the name of the UI function to call; all subsequent arguments are passed to the engine as parameters.
Only `string`, `number` and `boolean` arguments are supported.

Example:
```lua
local ui = Ext.GetBuiltinUI("Public/Game/GUI/characterSheet.swf")
ui:ExternalInterfaceCall("show")
```

#### Ext.RegisterUICall(object, name, handler)

The `Ext.RegisterUICall` function registers a listener that is called when the `ExternalInterface.call` function is invoked from ActionScript. ([ExternalInterface docs](https://help.adobe.com/en_US/FlashPlatform/reference/actionscript/3/flash/external/ExternalInterface.html))
 - `object` is the UI object that is returned from `Ext.CreateUI` or `Ext.GetUI`
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

#### UIObject:SetValue(name, value, [arrayIndex])

The `SetValue` method updates the specified public property of the main timeline object. 


#### UIObject:GetValue(name, type, [arrayIndex])

The `GetValue` method retrieves the specified public property of the main timeline object. 
`type` contains the type of value to retrieve and must be `string`, `number` or `boolean`.


#### UIObject:Show()

Displays the UI element.

#### UIObject:Hide()

Hides the UI element.

#### UIObject:SetPosition(x, y)

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
This function can only be called during the `ModuleLoading` event; calling it elsewhere throws a Lua error.

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
This function can only be called during the `ModuleLoading` event.

Example:
```lua
Ext.StatAddCustomDescription("Dome_CircleOfProtection", "SkillProperties", "Custom desc one")
```
### StatSetLevelScaling(statType, attribute, func) <sup>R</sup>

Replaces level scaling formula for the specified stat.
This function can only be called during the `ModuleLoading` event.

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

Each time the game calculates hit chance, the Lua function `Ext.GetHitChance` gets called. If the function is overridden by a Lua script, the game will use the return value of the custom function as the hit chance. If the function is not overridden or the function call fails, the game's own hit chance calculation is used.

The following example makes it so the overall Hit Chance is the Attacker's Accuracy minus the Target's Dodge, no multiplicative operations involved:
```lua
local function YourHitChanceFunction(attacker, target)
    local hitChance = attacker.Accuracy - target.Dodge
    -- Make sure that we return a value in the range (0% .. 100%)
    hitChance = math.max(math.min(hitChance, 100), 0)
    return hitChance
end

Ext.GetHitChance = YourHitChanceFunction
Ext.EnableStatOverride("HitChance")
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

## Combat

Each combat in-game is represented by a Combat object in Lua. 

Properties:

| Name | Type | Notes |
|--|--|--|
| CombatId | integer | A number identifying the combat instance |
| LevelName | string | Level where the combat is taking place |
| IsActive | boolean |  |


Methods:

#### GetAllTeams()
Retrieves all participants of the combat. The return value is a table of `Team` objects.

#### GetCurrentTurnOrder()
Retrieves the turn order of the current round. The return value is a table of `Team` objects.

#### GetNextTurnOrder()
Retrieves the turn order of the next round. The return value is a table of `Team` objects.

#### UpdateCurrentTurnOrder(turnOrder)
Updates the turn order of the current round. The `turnOrder` argument should be a reordered version of the table returned by `GetCurrentTurnOrder()`. 

Notes:
 - It is possible to remove or add characters to the current turn by adding/removing their `Team` object from the table. 
 - It is possible to add a character to the current turn more than once, the character will only appear once in the UI however.
 - The character whose turn is currently active (the very first item) should not be removed or reordered. This only applies for `GetCurrentTurnOrder`, the first item can be freely reordered in `GetNextTurnOrder`.
 - Changed performed using this function are synchronized to the client at the end of the current server tick.

#### UpdateNextTurnOrder(turnOrder)
Updates the turn order of the next round. The `turnOrder` argument should be a reordered version of the table returned by `GetNextTurnOrder()`. 

Notes:
 - It is possible to remove or add characters to the next turn by adding/removing their `Team` object from the table. 
 - It is possible to add a character to the next turn more than once, the character will only appear once in the UI however.
 - Changed performed using this function are synchronized to the client at the end of the current server tick.

### CalculateTurnOrder Event <sup>R</sup>

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

### Team

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
It is currently only used by the `GetSkillDamage` event to fetch damage information.

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
 - Status chance overrides, Damage calc override, Skill/Status tooltip callbacks, Restricted contexts, Client-side behavior
 - Lua state lifetime, Globals behavior (not saved)
 - Ext.Require, Print
 - File IO
 - Osi special Lua functions
 - Bootstrap phase
 - Reloading Lua and changing exports
 - Reloading Story

