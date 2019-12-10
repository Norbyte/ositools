### Lua API v35 Documentation

### Table of Contents  

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
 - [JSON Support](#json-support)


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
| `table` | - | Osiris only supports scalar values, so table cannot be passed to/from Osiris functions. |



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

It is possible to insert new tuples to Osiris databases by calling the DB like a function.

```lua
Osi.DB_CharacterAllCrimesDisabled(player)
```

Deleting from DBs and iterating DBs is currently not implemented.


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
 - Stat overrides, status chance overrides, Restricted contexts, Client-side behavior
 - Lua state lifetime, Globals behavior (not saved)
 - Ext.Require, Print
 - Osi special Lua functions
 - Bootstrap phase
 - Reloading Lua and changing exports
 - Reloading Story
