# Changes in v56

## General Lua

### New Lifetime Rules

Previously, a `userdata` (game object passed from the extender to Lua, i.e. `Character`, `Status`, etc.) returned from an API call or passed via a parameter was valid for an infinite duration. This meant that the object could be accessed anytime, potentially well after the actual object in the engine was destroyed, leading to strange crashes. The extender before v56 tried to work around this by storing object handles instead of the actual object in the `userdata`, which meant that each time a property was accessed on an object, it had to resolve the handle, which is a very slow operation. Example:

```lua
s = Ext.GetCharacter(...).Stats
-- These 2 reads cause the character and stats component to be fetched via their handle twice. 
-- If you do anything thats is stats-intensive, it may cause up to 100+ entity fetches
local wf = s.WarriorLore
local lr = s.RangerLore
```

Example of possible crash:
```lua
local hit
Ext.RegisterListener("StatusHitEnter", function (status, ...) 
    hit = status
end)

Ext.RegisterListener("ProjectileHit", function (...) 
    -- Status might get deleted beforehand
    -- POSSIBLE CRASH!
    local dmg = hit.Hit.TotalDamageDealt
end)
```

To fix these issues, most `userdata` types are now bound to their enclosing *extender scope*. Since the engine always deletes game objects at the end of the game loop, it is guaranteed that eg. a Status or Character won't disappear during a Lua call, but they may be gone afterwards. To rectify this, "smuggling" objects outside of listeners is no longer allowed. Example:

```lua
local hit

Ext.Events.StatusHitEnter:Subscribe(function (event) 
    hit = event.Hit
end)

Ext.Events.ProjectileHit:Subscribe(function (event) 
    -- Throws "Attempted to read object of type 'esv::StatusHit' whose lifetime has expired"
    local dmg = hit.Hit.TotalDamageDealt
end)
```

This rule also applies to objects you fetch manually during a listener:

```lua
local ch

Ext.Events.StatusHitEnter:Subscribe(function (event) 
    ch = Ext.GetCharacter(event.Hit.OwnerHandle)
end)

Ext.Events.ProjectileHit:Subscribe(function (event) 
    -- Throws "Attempted to read object of type 'esv::Character' whose lifetime has expired"
    local name = ch.DisplayName
end)
```

Subproperties inherit the lifetime of their parent object, eg. if you keep a copy of the permament boosts (`local boosts = character.Stats.DynamicStats[2]`), its lifetime will expire at the same time as the characters'.

This ensures that no potentially deleted objects are not accessed in risky contexts.

### Parameter handling changes

 - Numeric enum values and numeric bitmask values passed to API calls are now validated.
   Previously, passing invalid numeric values for enum parameters (eg. 9999 for `DamageType`) was allowed because of the lack of validation; in v56 an error is thrown if an invalid enum value is passed to a function.
   
 - Bitmask parameters (eg. `PropertyContext`) where multiple values are allowed had to be calculated manually in v55, since only one value was allowed to be passed in string form. All bitmask parameters now support passing tables to specify multiple flag values, i.e. the allowed ways to pass bitmasks are now:
    - Integer (i.e. `3` means "Target and AoE" for `PropertyContext`)
    - String (i.e. `"Target"`) - note that this only supports passing a single value!
    - Table (i.e. `{"Target", "AoE"}`)


### Object behavior changes

The Lua game object mapping layer was rewritten and stricter rules apply to object property access. 
Attempting to read or write properties that don't exist on an object class now lead to a Lua error:
```lua
local ch = Ext.GetCharacter(...)
-- Object of type 'esv::Character' has no property named 'Shield'
Ext.Print(ch.Shield)
```

The properties and methods of all engine objects can now be read using iteration (metatables now support `__pairs`):
```lua
local ch = Ext.GetCharacter(...)
for property, value in pairs(ch) do
    Ext.Print(property, value)
end
```

Stringifying an engine object now returns its class and instance ID (i.e. metatables now support `__tostring`):
```lua
-- Prints "esv::Character (00007FF43B4F6600)"
Ext.Print(tostring(Ext.GetCharacter(CharacterGetHostCharacter())))
```

Equality checks on engine objects return whether the two references point to the _same_ object:
```lua
-- Prints whether the current player character is the red prince
Ext.Print(Ext.GetCharacter(CharacterGetHostCharacter()) == Ext.GetCharacter("a26a1efb-cdc8-4cf3-a7b2-b2f9544add6f"))
```

Array-like engine objects now support iteration via `ipairs()` and their length can be read using the `#` operator (i.e. the `__len` and `__pairs` metamethods are now supported):
```lua
local tags = _C().Tags
Ext.Print("Number of tags: ", #tags)
for i, tag in ipairs(tags) do
    Ext.Print(i, tag)
end
```

### New event system

The `Ext.RegisterListener()` API is deprecated; event subscription should be done through the `Ext.Events` table instead.

Example:
```lua
-- OLD event listener
Ext.RegisterListener("GameStateChanged", function (from, to)
    Ext.Print("State change from " .. from .. " to " .. to)
end)

-- NEW event listener
Ext.Events.GameStateChanged:Subscribe(function (e)
    Ext.Print("State change from " .. e.FromState .. " to " .. e.ToState)
end)
```

The `Subscribe()` method accepts an optional options table that contains additional settings:
```lua
Ext.Events.GameStateChanged:Subscribe(handler, {
    Priority = 50,
    Once = true
})
```

The `Priority` setting determines the order in which subscribers are called; subscribers with lower priority are called first. The default priority is 100.
If the `Once` flag is set, the event is only triggered once and the handler is automatically unsubscribed after the first call.

The `Subscribe()` method returns a handler index that can be used to cancel the subscription later on:
```lua
local handlerId = Ext.Events.GameStateChanged:Subscribe(handler)
...
Ext.Events.GameStateChanged:Unsubscribe(handlerId)
```

### Garbage collection

v56 fixes jitter caused by infrequent garbage collection (GC). The Lua GC counters are somewhat unreliable due to the large number of userdata objects being created in DOS2 which causes the automatic GC to be triggered too infrequently (sometimes once every several minutes), resulting in large GC workloads. A minor GC collection step was added to the main client/server loop to reduce the impact of this issue.

### Console

 - Console commands are now thread-safe; previously they were executed from the console thread and had concurrency issues (i.e. could cause crashes)
 - Console commands are now disallowed during game states where the game is not yet loaded
 - Added client/server context indicator to console prompt


## New Features

### Notable new object properties

 - Root templates can now be accessed by their GUID using `Ext.Template.GetTemplate(guid)`
 - Actions performed when using an item are now accessible using the `OnUsePeaceActions` root template property
 - Character inventory (`ItemList`), treasure tables (`Treasures`) and skills (`SkillList`) were added to character templates
 - Most root template fields are now writeable
 - Translated string properties (e.g. `DisplayName` in root templates) now contain both the handle and the translated string value
 - Resources can now be accessed by their GUID using `Ext.Resource.Get(guid)`
 - Conditions from script check blocks (i.e. `TargetConditions`) and `IF(...)` blocks from `ExtraProperties` are now readable
 - Character and root template combat component properties are now accessible

For a full list of properties that were added in v56, check the tables below.

### New low-level objects

Several new accessor functions were added to allow reading game data that was previously unavailable:

 - `Ext.Utils.GetGlobalSwitches()` returns the current game settings as well as many internal game switches
 - `Ext.Utils.GetGraphicSettings()` returns the current graphics settings
 - `Ext.Entity.GetInventory(handle)` (server-only) fetches character/item inventories using their handle
 - `Ext.Entity.GetCombat(id)` (server-only) returns combat objects using the combatId
 - `Ext.Entity.GetSurface(handle)` (server-only) fetches surface objects their handle
 - `Ext.Entity.GetAiGrid()` returns the AI grid for the currently active level
 - `Ext.Entity.GetCurrentLevel()` returns the currently active level, including all of its currently available internals

### Alignments

 - `Ext.Entity.GetAlignmentManager()` (server-only) returns the alignment manager object, which can be used to query and modify relation values and fetch/modify alignments
 - Custom alignment support added: The way `alignments.lsx` is loaded by the game was adjusted; previously `alignments.lsx` was only loaded from the base module (i.e. Origins) and was ignored from add-ons; this was adjusted to load alignments from all add-ons as well.

### UI

 - More UI capabilities via drag + drop, 
 - the newer UI properties like Flags, 
 - specific UI data mapped out like character creation 

#### Editing UI calls

When subscribing to the new `UIInvoke` and `UICall` events, it is possible to change call parameters or or cancel the call entirely before the call is performed. (This only works in the `Before` event handler!)

```lua
Ext.Events.UIInvoke:Subscribe(function (e)
    if e.Function == "setDebugText" and e.When == "Before" then
        -- Change parameter
        e.Args[1] = "TEST WHEEEEEE"
        -- Cancel UI call
        e:PreventAction()
    end
end)
```

### JSON API

 - The API for `Ext.Json.Stringify` has changed; the new signature is `Stringify(value, [options])`. `options` is a table that supports the following keys:
   - `Beautify` (bool) - Generate human-readable JSON (i.e. add indents and linebreaks to the output)
   - `StringifyInternalTypes` (bool) - Save engine types (handles, coroutines, etc.) as strings instead of throwing an error
   - `IterateUserdata` (bool) - Dump engine objects similarly to tables instead of throwing an error
      - NOTE: Due to the nature of these objects, neither internal types nor userdata types can be unserialized from a JSON; parsing a JSON with userdata objects will return them as normal tables
    - `AvoidRecursion` (bool) - If an userdata or table is seen multiple times, further instances will be serialized as `"*RECURSION*"`; this is helpful when dumping objects
    - `MaxDepth` (int) - Maximum iteration depth
 - The old `Stringify(value, beautify, stringifyInternalTypes)` API is retained for backwards compatibility


### Math library

A new math library, `Ext.Math` was added with the following functions:

##### Add(a: any, b: any)

Adds the two operands. All math types (number/vec3/vec4/mat3x3/mat4x4) are supported. Mixing different operand types works in if a reasonable implementation is available (eg. `number + vec3`).

##### Sub(a: any, b: any)

Subtracts the two operands. All math types (number/vec3/vec4/mat3x3/mat4x4) are supported. Mixing different operand types works in if a reasonable implementation is available (eg. `vec3 - number`).

##### Mul(a: any, b: any)

Multiplies the two operands. All math types (number/vec3/vec4/mat3x3/mat4x4) are supported. Mixing different operand types works in if a reasonable implementation is available (eg. `mat3x3 * vec3`).

##### Div(a: any, b: any)

Divides the two operands. All math types (number/vec3/vec4/mat3x3/mat4x4) are supported.

##### vec3|vec4 Reflect(I: vec3|vec4, N: vec3|vec4)

For the incident vector `I` and surface orientation `N`, returns the reflection direction: `result = I - 2.0 * dot(N, I) * N`.

##### float Angle(a: vec3|vec4, b: vec3|vec4)

Returns the absolute angle between two vectors. Parameters need to be normalized.

##### vec3 Cross(x: vec3, y: vec3)

Returns the cross product of x and y.

##### float Distance(p0: vec3, p1: vec3)

Returns the distance between p0 and p1, i.e., `length(p0 - p1)`.

##### float Dot(x: vec3, y: vec3)

Returns the dot product of x and y.

##### float Length(x: vec3|vec4)

Returns the length of x, i.e., `sqrt(x * x)`.

##### vec3|vec4 Determinant(x: vec3|vec4)

Returns a vector in the same direction as x but with length of 1.

##### float Length(x: mat3|mat4)

Return the determinant of a matrix.

##### mat3|mat4 Inverse(x: mat3|mat4)

Return the inverse of a matrix.

##### mat3|mat4 Transpose(x: mat3|mat4)

Returns the transposed matrix of `x`.

##### mat3|mat4 OuterProduct(c: vec3|vec4, r: vec3|vec4)

Treats the first parameter `c` as a column vector and the second parameter `r` as a row vector and does a linear algebraic matrix multiply `c * r`.

##### void Rotate(m: mat3|mat4, angle: float, axis: vec4)

Builds a rotation matrix created from an axis of 3 scalars and an angle expressed in radians.

##### void Translate(m: mat4, translation: vec3)

Transforms a matrix with a translation 4 * 4 matrix created from a vector of 3 components.

##### void Scale(m: mat4, translation: vec3)

Transforms a matrix with a scale 4 * 4 matrix created from a vector of 3 components.

##### mat4 BuildRotation4(v: vec3, angle: float)

Builds a rotation 4 * 4 matrix created from an axis of 3 scalars and an angle expressed in radians.

##### mat3 BuildRotation3(v: vec3, angle: float)

Builds a rotation 3 * 3 matrix created from an axis of 3 scalars and an angle expressed in radians.

##### mat4 BuildTranslation(v: vec3)

Builds a translation 4 * 4 matrix created from a vector of 3 components.

##### mat4 BuildScale(v: vec3)

Builds a scale 4 * 4 matrix created from 3 scalars.

##### vec3 ExtractEulerAngles(m: mat3|mat4)

Extracts the `(X * Y * Z)` Euler angles from the rotation matrix M.

##### mat4 BuildFromEulerAngles4(angles: vec3)

Creates a 3D 4 * 4 homogeneous rotation matrix from euler angles `(X * Y * Z)`.

##### mat3 BuildFromEulerAngles3(angles: vec3)

Creates a 3D 3 * 3 homogeneous rotation matrix from euler angles `(X * Y * Z)`.

##### void Decompose(m: mat4, scale: vec3, yawPitchRoll: vec3, translation: vec3)

Decomposes a model matrix to translations, rotation and scale components.

##### float ExtractAxisAngle(m: mat3|mat4, axis: vec3)

Decomposes a model matrix to translations, rotation and scale components.

##### mat3 BuildFromAxisAngle3(axis: vec3, angle: float)
##### mat4 BuildFromAxisAngle4(axis: vec3, angle: float)

Build a matrix from axis and angle.

##### vec3|vec4 Perpendicular(x: vec3|vec4, normal: vec3|vec4)

Projects `x` on a perpendicular axis of `normal`.

##### vec3|vec4 Project(x: vec3|vec4, normal: vec3|vec4)

Projects `x` on `normal`.

##### float Fract(x: float)

Return `x - floor(x).`

##### float Trunc(x: float)

Returns a value equal to the nearest integer to x whose absolute value is not larger than the absolute value of x.

##### float Sign(x: float)

Returns 1.0 if `x > 0`, 0.0 if `x == 0`, or -1.0 if `x < 0`.

##### float Clamp(val: float, minVal: float, maxVal: float)

Returns `min(max(x, minVal), maxVal)` for each component in x using the floating-point values minVal and maxVal.

##### float Lerp(x: float, y: float, a: float)

Returns `x * (1.0 - a) + y * a`, i.e., the linear blend of x and y using the floating-point value a.

##### float Acos(x: float)

Arc cosine. Returns an angle whose sine is x.

##### float Asin(x: float)

Arc sine. Returns an angle whose sine is x.

##### float Atan(y_over_x: float)

 Arc tangent. Returns an angle whose tangent is `y_over_x`.

##### float Atan2(x: float, y: float)

Arc tangent. Returns an angle whose tangent is `y / x`. The signs of x and y are used to determine what quadrant the angle is in.


### New Events

 - `BeforeStatusApply`: thrown before a status is applied on the server; similarly to the Osiris `NRD_OnStatusAttempt` event, this can be used to prevent a status from being applied.
 - `BeforeStatusDelete`: thrown before a status is removed on the server; this event be used to prevent a status from being removed. 
    - Note: certain engine mechanisms delete statuses directly and don't trigger a `BeforeStatusDelete` event.
 - Ext table reorganization, name deprecations **[[ TODO TABLE ]]**
 - `OnResetCompleted`: Thrown when a console `reset` command or an `NRD_LuaReset` Osiris call completes.
 - `Tick`: Thrown on each game loop tick on both the client and the server. Normally game logic runs at ~30hz, so this event is thrown roughly every 33ms.
    -  The `Ext.OnNextTick(fun)` helper registers a handler that is only called on the next tick and is unregistered afterwards


### Helper functions

Some helper functions were added to aid in development. (Please note that using them in mod code is not recommended, they are designed for developer use only.)

 - `_C()`: Returns the currently active character object (`esv::Character` on server, `ecl::Character` on client)
 - `_E()`: Returns the character or item being inspected in the examine UI (client only)
 - `_W()`: Returns the weapon equipped by the active character
 - `_D()`: Equivalent to `Ext.Utils.Dump()`, an utility function for dumping an expression to console; supports hierarchical dumping of tables and userdata (engine) objects
 - `_P()`: Equivalent to `Ext.Print()`

### Inventory sync speedup

An inefficient inventory sync issue was fixed in the base game where syncing inventory from the server exhibited exponential behavior. This means that on maps/savegames with a large number of items the client spent a significant amount of time syncing items before the game was loaded. (Time savings of ~1 minute were observed in very large saves.)

### Bug fixes

 - Fix client state not being reinitialized after returning to menu
 - Fix hashtable corruption when resizing to the same size
 - Fix Invoke events being triggered from UI objects that are not completely initialized (don't miss early-init events)
 - Fix use-after-free in flash invoke table (name memory reuse before invoke is executed)
 - Fix UIObject::Resize()
 - Fix crash when setting custom stat without a custom stats component
 - Fix status fetch issue when trying to apply multiple statuses with the same handle
- Fix tooltip hooks not working properly when inspecting an item
- Fix AI grid fetch crash with over 32k objects
- Fix incorrectly mapped signed fields in some status mappings
- Fix crash while syncing stats with null FixedStrings
- Add GB4 talents to requirement enum
- Fixed memory leak when fetching Flash objects
- Fix ComputeCharacterHit not inheriting hit values from caller (infinite reflection loop, etc.)
- Fix integer overflow in `Ext.MonotonicTime()`
- Fix Osiris callbacks not working when debugger is enabled

### IDE helpers

 - Added type information API (`Ext.Types`) that can be used to query object structure and properties.
 - IDE helpers are now generated directly from Lua:
    - `Ext.IO.SaveFile("helpers.lua", Ext.Types.GenerateIdeHelpers())`


## TODOS

 - Effect and visual creation allows for doing things not possible previously (though I'd argue the visual stuff needs an API to manage it in its current state, due to how easily visuals are destroyed, like sheathing/unsheathing) -- Ext.Visual
     - Add support for overriding material parameters ***[[TODO]]***
    
 - Add server effect APIs
- Add AI accessor functions


## Ext. name changes


### Shared context

| Old name | New name |
|--|--|
| `JsonStringify` | `Json.Stringify` |
| `JsonParse` | `Json.Parse` |
| `DebugBreak` | `Debug.DebugBreak` |
| `IsDeveloperMode` | `Debug.IsDeveloperMode` |
| `MonotonicTime` | `Utils.MonotonicTime` |
| `Version` | `Utils.Version` |
| `GameVersion` | `Utils.GameVersion` |
| `GetHandleType` | `Utils.GetHandleType` |
| `ShowErrorAndExitGame` | `Utils.ShowErrorAndExitGame` |
| `LoadFile` | `IO.LoadFile` |
| `SaveFile` | `IO.SaveFile` |
| `AddPathOverride` | `IO.AddPathOverride` |
| `GetPathOverride` | `IO.GetPathOverride` |
| `IsModLoaded` | `Mod.IsModLoaded` |
| `GetModLoadOrder` | `Mod.GetLoadOrder` |
| `GetModInfo` | `Mod.GetModInfo` |
| `GetTranslatedString` | `L10N.GetTranslatedString` |
| `GetTranslatedStringFromKey` | `L10N.GetTranslatedStringFromKey` |
| `CreateTranslatedString` | `L10N.CreateTranslatedString` |
| `CreateTranslatedStringKey` | `L10N.CreateTranslatedStringKey` |
| `CreateTranslatedStringHandle` | `L10N.CreateTranslatedStringHandle` |
| `GetStatEntries` | `Stats.GetStats` |
| `GetStatEntriesLoadedBefore` | `Stats.GetStatsLoadedBefore` |
| `GetStat` | `Stats.Get` |
| `CreateStat` | `Stats.Create` |
| `SyncStat` | `Stats.Sync` |
| `StatSetPersistence` | `Stats.SetPersistence` |
| `StatGetAttribute` | `Stats.GetAttribute` |
| `StatSetAttribute` | `Stats.SetAttribute` |
| `StatAddCustomDescription` | `Stats.AddCustomDescription` |
| `StatSetLevelScaling` | `Stats.SetLevelScaling` |
| `EnumIndexToLabel` | `Stats.EnumIndexToLabel` |
| `EnumLabelToIndex` | `Stats.EnumLabelToIndex` |
| `NewDamageList` | `Stats.NewDamageList` |
| `AddVoiceMetaData` | `Stats.AddVoiceMetaData` |
| `GetDeltaMod` | `Stats.DeltaMod.GetLegacy` |
| `UpdateDeltaMod` | `Stats.DeltaMod.Update` |
| `GetSkillSet` | `Stats.SkillSet.GetLegacy` |
| `UpdateSkillSet` | `Stats.SkillSet.Update` |
| `GetEquipmentSet` | `Stats.EquipmentSet.GetLegacy` |
| `UpdateEquipmentSet` | `Stats.EquipmentSet.Update` |
| `GetTreasureTable` | `Stats.TreasureTable.GetLegacy` |
| `UpdateTreasureTable` | `Stats.TreasureTable.Update` |
| `GetTreasureCategory` | `Stats.TreasureCategory.GetLegacy` |
| `UpdateTreasureCategory` | `Stats.TreasureCategory.Update` |
| `GetItemCombo` | `Stats.ItemCombo.GetLegacy` |
| `UpdateItemCombo` | `Stats.ItemCombo.Update` |
| `GetItemComboPreviewData` | `Stats.ItemComboPreview.GetLegacy` |
| `UpdateItemComboPreviewData` | `Stats.ItemComboPreview.Update` |
| `GetItemComboProperty` | `Stats.ItemComboPreview.GetLegacy` |
| `UpdateItemComboProperty` | `Stats.ItemComboPreview.Update` |
| `GetItemGroup` | `Stats.ItemGroup.GetLegacy` |
| `GetNameGroup` | `Stats.NameGroup.Update` |
| `GetSurfaceTransformRules` | `Surface.GetTransformRules` |
| `UpdateSurfaceTransformRules` | `Surface.UpdateTransformRules` |
| `GetSurfaceTemplate` | `Surface.GetTemplate` |

### Client context

| Old name | New name |
|--|--|
| `CreateUI` | `UI.Create` |
| `GetUI` | `UI.GetByName` |
| `GetUIByType` | `UI.GetByType` |
| `GetBuiltinUI` | `UI.GetByPath` |
| `DestroyUI` | `UI.Destroy` |
| `UISetDirty` | `UI.SetDirty` |
| `UIEnableCustomDrawCallDebugging` | `UI.EnableCustomDrawCallDebugging` |
| `HandleToDouble` | `UI.HandleToDouble` |
| `DoubleToHandle` | `UI.DoubleToHandle` |
| `GetPickingState` | `UI.GetPickingState` |
| `GetCharacter` | `Entity.GetCharacter` |
| `GetItem` | `Entity.GetItem` |
| `GetStatus` | `Entity.GetStatus` |
| `GetGameObject` | `Entity.GetGameObject` |
| `GetAiGrid` | `Entity.GetAiGrid` |
| `PostMessageToServer` | `Net.PostMessageToServer` |
| `GetGameState` | `Client.GetGameState` |
| `UpdateShroud` | `Client.UpdateShroud` |

### Server context

| Old name | New name |
|--|--|
| `RegisterOsirisListener` | `Osiris.RegisterListener` |
| `OsirisIsCallable` | `Osiris.IsCallable` |
| `NewCall` | `Osiris.NewCall` |
| `NewQuery` | `Osiris.NewQuery` |
| `NewEvent` | `Osiris.NewEvent` |
| `BroadcastMessage` | `Net.BroadcastMessage` |
| `PostMessageToClient` | `Net.PostMessageToClient` |
| `PostMessageToUser` | `Net.PostMessageToUser` |
| `PlayerHasExtender` | `Net.PlayerHasExtender` |
| `GetAllCharacters` | `Entity.GetAllCharacterGuids` |
| `GetCharactersAroundPosition` | `Entity.GetCharacterGuidsAroundPosition` |
| `GetAllItems` | `Entity.GetAllItemGuids` |
| `GetItemsAroundPosition` | `Entity.GetItemGuidsAroundPosition` |
| `GetAllTriggers` | `Entity.GetAllTriggerGuids` |
| `GetCharacter` | `Entity.GetCharacter` |
| `GetItem` | `Entity.GetItem` |
| `GetTrigger` | `Entity.GetTrigger` |
| `GetGameObject` | `Entity.GetGameObject` |
| `GetStatus` | `Entity.GetStatus` |
| `GetCombat` | `Entity.GetCombat` |
| `GetSurface` | `Entity.GetSurface` |
| `GetAiGrid` | `Entity.GetAiGrid` |
| `GetCurrentLevelData` | `Entity.GetCurrentLevelData` |
| `CreateCustomStat` | `CustomStat.Create` |
| `GetCustomStatById` | `CustomStat.GetById` |
| `GetCustomStatByName` | `CustomStat.GetByName` |
| `GetAllCustomStats` | `CustomStat.GetAll` |
| `CreateSurfaceAction` | `Surface.Action.Create` |
| `ExecuteSurfaceAction` | `Surface.Action.Execute` |
| `CancelSurfaceAction` | `Surface.Action.Cancel` |
| `GetGameState` | `Server.GetGameState` |
| `ExecuteSkillPropertiesOnTarget` | `PropertyList.ExecuteSkillPropertiesOnTarget` |
| `ExecuteSkillPropertiesOnPosition` | `PropertyList.ExecuteSkillPropertiesOnPosition` |


## New Ext. module functions

### Shared context

| Module | Function |
|--|--|
| `Mod` | `GetMod(uuid)` |
| `Mod` | `GetBaseMod()` |
| `Resource` | `Get(uuid)` |
| `Stats.ItemColor` | `Get(colorName)` |
| `Stats.ItemColor` | `Update(color)` |
| `Stats.ItemColor` | `GetAll()` |
| `Surface` | `GetTemplate(type)` |
| `Types` | `GetObjectType(object)` |
| `Types` | `GetTypeInfo(typeName)` |
| `Types` | `GetAllTypes()` |
| `Utils` | `GetValueType(value)` |
| `Utils` | `IsValidHandle(handle)` |
| `Utils` | `GetHandleType(handle)` |
| `Utils` | `MakeHandleObject(handle)` |
| `Utils` | `HandleToInteger(handle)` |
| `Utils` | `IntegerToHandle(integerHandle)` |
| `Utils` | `GetGlobalSwitches()` |
| `Utils` | `GetGraphicSettings()` |


### Client context

| Module | Function |
|--|--|
| `Client` | `GetModManager()` |
| `Entity` | `NullHandle()` |
| `Entity` | `GetInventory(handle)` |
| `Entity` | `GetGameObject(id)` |
| `Entity` | `GetAiGrid()` |
| `Entity` | `GetCurrentLevel()` |
| `Template` | `GetRootTemplate(uuid)` |
| `Template` | `GetLocalTemplate(uuid)` |
| `Template` | `GetCacheTemplate(uuid)` |
| `Template` | `GetLocalCacheTemplate(uuid)` |
| `Template` | `GetTemplate(uuid)` |
| `UI` | `GetCharacterCreationWizard()` |
| `UI` | `GetDragDrop()` |
| `UI` | `LoadFlashLibrary(moduleName, path)` |
| `UI` | `GetViewportSize()` |
| `Visual` | `GetVisualObject(handle)` |
| `Visual` | `Get(handle)` |
| `Visual` | `Create(position)` |
| `Visual` | `CreateOnCharacter(position, target, listenForTextKeys)` |
| `Visual` | `CreateOnItem(position, target, listenForTextKeys)` |

### Server context

| Module | Function |
|--|--|
| `Server` | `GetModManager()` |
| `Ai` | `GetArchetypes()` |
| `Ai` | `GetAiHelpers()` |
| `Effect` | `GetAllEffectHandles()` |
| `Effect` | `GetEffect(handle)` |
| `Effect` | `CreateEffect(effectName, sourceHandle, castBone)` |
| `Entity` | `NullHandle()` |
| `Entity` | `GetGameObject(id)` |
| `Entity` | `GetInventory(handle)` |
| `Entity` | `GetCombat(id)` |
| `Entity` | `GetSurface(handle)` |
| `Entity` | `GetAiGrid()` |
| `Entity` | `GetCurrentLevelData()` |
| `Entity` | `GetCurrentLevel()` |
| `Entity` | `GetAlignmentManager()` |
| `Entity` | `GetAllCharacterGuids()` |
| `Entity` | `GetCharacterGuidsAroundPosition(x, y, z, radius)` |
| `Entity` | `GetAllItemGuids()` |
| `Entity` | `GetItemGuidsAroundPosition(x, y, z, radius)` |
| `Entity` | `GetAllTriggerGuids()` |
| `PropertyList` | `ExecuteExtraPropertiesOnTarget(statsEntryName, propertyName, attacker, target, position, propertyContext, isFromItem, skillId)` |
| `PropertyList` | `ExecuteExtraPropertiesOnPosition(statsEntryName, propertyName, attacker, position, radius, propertyContext, isFromItem, skillId)` |
| `Template` | `GetRootTemplate(uuid)` |
| `Template` | `GetLocalTemplate(uuid)` |
| `Template` | `GetCacheTemplate(uuid)` |
| `Template` | `GetLocalCacheTemplate(uuid)` |
| `Template` | `GetTemplate(uuid)` |
| `Template` | `CreateCacheTemplate(baseTemplateUuid)` |

## New class properties

### EoCGameObjectTemplate
| Property | Type |
|-|-|
| `AIBoundsAIType` | `number` |
| `AIBoundsMax` | `vec3` |
| `AIBoundsMin` | `vec3` |
| `FileName` | `Path` |
| `Flags` | `GameObjectTemplateFlags` |
| `Handle` | `number` |
| `NonUniformScale` | `boolean` |
| `RootTemplate` | `string` |
| `Tags` | `string[]` |
| `Transform` | `Transform` |
| `Type` | `TemplateType` |


### CharacterTemplate
| Property | Type |
|-|-|
| `BloodSurfaceType` | `SurfaceType` |
| `CombatComponent` | `CombatComponentTemplate` |
| `EmptyVisualSet` | `boolean` |
| `ItemList` | `InventoryItemData[]` |
| `OnDeathActions` | `IActionData[]` |
| `PickingPhysicsTemplates` | `table<string, string>` |
| `SkillList` | `CharacterSkillData[]` |
| `TradeTreasures` | `string[]` |
| `Treasures` | `string[]` |
| `VisualSet` | `VisualSet` |


### ItemTemplate
| Property | Type |
|-|-|
| `BloodSurfaceType` | `number` |
| `OnDestroyActions` | `IActionData[]` |
| `OnUsePeaceActions` | `IActionData[]` |
| `PinnedContainerTags` | `string[]` |
| `Treasures` | `string[]` |


### SurfaceTemplate
| Property | Type |
|-|-|
| `FX` | `SurfaceTemplateVisualData[]` |
| `InstanceVisual` | `SurfaceTemplateVisualData[]` |
| `IntroFX` | `SurfaceTemplateVisualData[]` |


### EclStatus
| Property | Type |
|-|-|
| `CurrentLifeTime` | `number` |
| `Flags` | `EclStatusFlags` |
| `LifeTime` | `number` |
| `NetID` | `NetId` |
| `OwnerHandle` | `ComponentHandle` |
| `StatsMultiplier` | `number` |
| `StatusSourceHandle` | `ComponentHandle` |
| `StatusType` | `string` |



### EsvStatus
| Property | Type |
|-|-|
| `BringIntoCombat` | `boolean` |
| `Channeled` | `boolean` |
| `CleansedByHandle` | `ComponentHandle` |
| `DamageSourceType` | `CauseType` |
| `Flags0` | `EsvStatusFlags0` |
| `Flags1` | `EsvStatusFlags1` |
| `Flags2` | `EsvStatusFlags2` |
| `ForceFailStatus` | `boolean` |
| `ForceStatus` | `boolean` |
| `Influence` | `boolean` |
| `InitiateCombat` | `boolean` |
| `IsFromItem` | `boolean` |
| `IsHostileAct` | `boolean` |
| `IsInvulnerable` | `boolean` |
| `IsLifeTimeSet` | `boolean` |
| `IsOnSourceSurface` | `boolean` |
| `IsResistingDeath` | `boolean` |
| `KeepAlive` | `boolean` |
| `NetID` | `NetId` |
| `OwnerHandle` | `ComponentHandle` |
| `RequestClientSync` | `boolean` |
| `RequestClientSync2` | `boolean` |
| `RequestDelete` | `boolean` |
| `RequestDeleteAtTurnEnd` | `boolean` |
| `Started` | `boolean` |
| `StatusType` | `string` |


### EsvStatusHit
| Property | Type |
|-|-|
| `PropertyList` | `StatsPropertyList` |


### EsvStatusConsumeBase
| Property | Type |
|-|-|
| `ItemHandles` | `ComponentHandle[]` |
| `Items` | `string[]` |
| `ResetCooldownsAbilities` | `number[]` |
| `Skill` | `string[]` |
| `StatsIds` | `EsvStatusConsumeBaseStatsData[]` |
| `SurfaceChanges` | `SurfaceTransformActionType[]` |


### EsvStatusHeal : EsvStatus
| Property | Type |
|-|-|
| `AbsorbSurfaceTypes` | `SurfaceType[]` |
| `TargetDependentHealAmount` | `number[]` |
| `TargetDependentValue` | `number[]` |


### EsvStatusAoO
| Property | Type |
|-|-|
| `AoOTargetHandle` | `ComponentHandle` |


### EsvStatusClimbing : EsvStatus
| Property | Type |
|-|-|
| `JumpUpLadders` | `boolean` |


### EsvStatusInSurface : EsvStatus
| Property | Type |
|-|-|
| `Layers` | `ESurfaceFlag` |



### CDivinityStatsCharacter
| Property | Type |
|-|-|
| `AttributeFlags` | `StatAttributeFlags` |
| `AttributeFlagsUpdated` | `boolean` |
| `Character` | `IGameObject` |
| `Flags` | `StatsCharacterFlags` |
| `ItemBoostedAttributeFlags` | `StatAttributeFlags` |
| `MainWeapon` | `CDivinityStatsItem` |
| `OffHandWeapon` | `CDivinityStatsItem` |
| `StatsFromStatsEntry` | `StatsCharacterDynamicStat` |
| `SurfacePathInfluences` | `SurfacePathInfluence[]` |
| `TraitOrder` | `number[]` |
| `GetItemBySlot` | ``fun(self: CDivinityStatsCharacter, slot: StatsItemSlot, mustBeEquipped: boolean\|nil):CDivinityStatsItem`` |



### CDivinityStatsItem
| Property | Type |
|-|-|
| `AttributeFlags` | `StatAttributeFlags` |
| `DamageTypeOverwrite` | `StatsDamageType` |
| `DeltaMods` | `string[]` |
| `EquipmentType` | `StatsEquipmentStatsType` |
| `ItemType` | `StatsEquipmentStatsType` |
| `WeaponType` | `StatsWeaponType` |


### IEoCClientObject
| Property | Type |
|-|-|
| `Base` | `BaseComponent` |
| `Handle` | `ComponentHandle` |
| `Height` | `number` |
| `Scale` | `number` |
| `Translate` | `vec3` |
| `Velocity` | `vec3` |
| `Visual` | `Visual` |
| `DisplayName` | `string|nil` |



### EclItem
| Property | Type |
|-|-|
| `AIBoundSize` | `number` |
| `Activated` | `boolean` |
| `Amount` | `number` |
| `Awake` | `boolean` |
| `CachedItemDescription` | `string` |
| `CanBeMoved` | `boolean` |
| `CanBePickedUp` | `boolean` |
| `CanShootThrough` | `boolean` |
| `CanUse` | `boolean` |
| `CanUseRemotely` | `boolean` |
| `CanWalkThrough` | `boolean` |
| `Consumable` | `boolean` |
| `CoverAmount` | `boolean` |
| `CurrentSlot` | `number` |
| `CurrentTemplate` | `ItemTemplate` |
| `CustomBookContent` | `string` |
| `CustomDescription` | `string` |
| `CustomDisplayName` | `string` |
| `Destroyed` | `boolean` |
| `DisableGravity` | `boolean` |
| `EnableHighlights` | `boolean` |
| `FallTimer` | `number` |
| `Flags` | `EclItemFlags` |
| `Flags2` | `EclItemFlags2` |
| `Floating` | `boolean` |
| `FoldDynamicStats` | `boolean` |
| `Global` | `boolean` |
| `GravityTimer` | `number` |
| `Hostile` | `boolean` |
| `InUseByCharacterHandle` | `ComponentHandle` |
| `InUseByUserId` | `number` |
| `InteractionDisabled` | `boolean` |
| `InventoryHandle` | `ComponentHandle` |
| `InventoryParentHandle` | `ComponentHandle` |
| `Invisible` | `boolean` |
| `Invulnerable` | `boolean` |
| `IsCraftingIngredient` | `boolean` |
| `IsInventory_M` | `boolean` |
| `IsKey` | `boolean` |
| `IsLadder` | `boolean` |
| `IsSecretDoor` | `boolean` |
| `IsSourceContainer` | `boolean` |
| `Known` | `boolean` |
| `LockLevel` | `number` |
| `OwnerCharacterHandle` | `ComponentHandle` |
| `PhysicsFlags` | `EclItemPhysicsFlags` |
| `PinnedContainer` | `boolean` |
| `PositionUpdatePending` | `boolean` |
| `StatsFromName` | `StatsObject` |
| `StatusMachine` | `EclStatusMachine` |
| `Sticky` | `boolean` |
| `Stolen` | `boolean` |
| `Tags` | `string[]` |
| `TeleportOnUse` | `boolean` |
| `TickPhysics` | `boolean` |
| `UnEquipLocked` | `boolean` |
| `Unimportant` | `boolean` |
| `UnknownTimer` | `number` |
| `UseSoundsLoaded` | `boolean` |
| `Vitality` | `number` |
| `Wadable` | `boolean` |
| `WakeFlag1` | `boolean` |
| `WakeNeighbours` | `boolean` |
| `WakePosition` | `vec3` |
| `Walkable` | `boolean` |
| `WorldPos` | `vec3` |


### EclCharacter
| Property | Type |
|-|-|
| `AnimationSpeed` | `number` |
| `CorpseCharacterHandle` | `ComponentHandle` |
| `CurrentTemplate` | `CharacterTemplate` |
| `DisplayNameOverride` | `string` |
| `Flags` | `uint64` |
| `FollowCharacterHandle` | `ComponentHandle` |
| `InventoryHandle` | `ComponentHandle` |
| `ItemTags` | `string[]` |
| `OriginalTemplate` | `CharacterTemplate` |
| `OwnerCharacterHandle` | `ComponentHandle` |
| `PlayerCustomData` | `EocPlayerCustomData` |
| `PlayerData` | `EclPlayerData` |
| `PlayerUpgrade` | `EocPlayerUpgrade` |
| `SkillManager` | `EclSkillManager` |
| `StatusMachine` | `EclStatusMachine` |
| `SurfacePathInfluences` | `SurfacePathInfluence[]` |
| `Tags` | `string[]` |
| `TalkingIconEffect` | `ComponentHandle` |
| `UserID` | `UserId` |
| `WorldPos` | `vec3` |


	
### EsvItem
| Property | Type |
|-|-|
| `ComputedVitality` | `number` |
| `Flags` | `EsvItemFlags` |
| `Flags2` | `EsvItemFlags2` |
| `Generation` | `EsvItemGeneration` |
| `Invisible` | `boolean` |
| `IsSecretDoor` | `boolean` |
| `Known` | `boolean` |
| `OriginalOwnerCharacter` | `ComponentHandle` |
| `OriginalTemplateType` | `uint64` |
| `PreviousLevel` | `string` |
| `Rarity` | `string` |
| `StatsFromName` | `StatsObject` |
| `StatusMachine` | `EsvStatusMachine` |
| `Tags` | `string[]` |
| `TeleportTargetOverride` | `uint64` |
| `TeleportUseCount` | `number` |
| `UserId` | `number` |
| `VisualResourceID` | `string` |


### EocItemDefinition
| Property | Type |
|-|-|
| `Active` | `boolean` |
| `CanBeMoved` | `boolean` |
| `CanBePickedUp` | `boolean` |
| `CanBeUsed` | `boolean` |
| `CustomRequirements` | `boolean` |
| `DamageTypeOverwrite` | `StatsDamageType` |
| `Floating` | `boolean` |
| `HP` | `number` |
| `InventoryNetID` | `NetId` |
| `InventorySubContainerNetID` | `NetId` |
| `Invisible` | `boolean` |
| `IsGlobal` | `boolean` |
| `IsPinnedContainer` | `boolean` |
| `ItemNetId` | `NetId` |
| `Known` | `boolean` |
| `LevelGroupIndex` | `number` |
| `NameCool` | `number` |
| `NameIndex` | `number` |
| `NetID` | `NetId` |
| `OriginalRootTemplateType` | `number` |
| `PinnedContainerTags` | `string[]` |
| `RootGroupIndex` | `number` |
| `RootTemplateType` | `number` |
| `StatsEntryName` | `string` |
| `Tags` | `string[]` |
| `UUID` | `string` |
| `Version` | `number	` |


### PlayerCustomData
| Property | Type |
|-|-|
| `NameTranslated` | `string` |


### EsvCharacter
| Property | Type |
|-|-|
| `AI` | `EocAi` |
| `CannotMove` | `boolean` |
| `Color` | `number` |
| `CreatedTemplateItems` | `string[]` |
| `CrimeHandle` | `ComponentHandle` |
| `CrimeState` | `number` |
| `CurrentTemplate` | `CharacterTemplate` |
| `CustomBloodSurface` | `SurfaceType` |
| `DamageCounter` | `uint64` |
| `DelayedDyingStatus` | `EsvStatus` |
| `Dialog` | `number` |
| `DisabledCrime` | `string[]` |
| `EnemyCharacterHandle` | `ComponentHandle` |
| `EnemyHandles` | `ComponentHandle[]` |
| `Flags` | `EsvCharacterFlags` |
| `Flags2` | `EsvCharacterFlags2` |
| `Flags3` | `EsvCharacterFlags3` |
| `FlagsEx` | `number` |
| `HealCounter` | `uint64` |
| `InvestigationTimer` | `number` |
| `IsDialogAiControlled` | `boolean` |
| `KillCounter` | `uint64` |
| `LifeTime` | `number` |
| `NoiseTimer` | `number` |
| `NumConsumables` | `number` |
| `ObjectHandle6` | `ComponentHandle` |
| `OriginalTemplate` | `CharacterTemplate` |
| `PartialAP` | `number` |
| `PlayerData` | `EsvPlayerData` |
| `PlayerUpgrade` | `EocPlayerUpgrade` |
| `PreferredAiTarget` | `string[]` |
| `PreviousCrimeHandle` | `ComponentHandle` |
| `PreviousCrimeState` | `number` |
| `RegisteredTriggerHandles` | `ComponentHandle[]` |
| `RegisteredTriggers` | `string[]` |
| `ServerControlRefCount` | `number` |
| `SkillBeingPrepared` | `string` |
| `SkillManager` | `EsvSkillManager` |
| `StatusMachine` | `EsvStatusMachine` |
| `StatusesFromItems` | `table<ComponentHandle,` | StatsPropertyStatus[]>
| `SummonHandles` | `ComponentHandle[]` |
| `SurfacePathInfluences` | `SurfacePathInfluence[]` |
| `Tags` | `string[]` |
| `TagsFromItems` | `string[]` |
| `Team` | `number` |
| `TemplateUsedForSkills` | `CharacterTemplate` |
| `TimeElapsed` | `number` |
| `Treasures` | `string[]` |
| `UserID` | `UserId` |
| `VoiceSet` | `string[]` |
| `WorldRot` | `mat3` |


### EsvSurface
| Property | Type |
|-|-|
| `NeedsSplitEvaluation` | `boolean` |
| `NetID` | `NetId` |



### EsvShootProjectileHelper
| Property | Type |
|-|-|
| `DamageList` | `StatsDamagePairList` |
| `HitObject` | `EsvShootProjectileHelperHitObject` |


### EsvShootProjectileHelperHitObject
| Property | Type |
|-|-|
| `HitInterpolation` | `number` |
| `Position` | `vec3` |
| `Target` | `ComponentHandle` |



### EsvProjectile
| Property | Type |
|-|-|
| `BoostConditions` | `number` |
| `DamageList` | `StatsDamagePairList` |
| `OnHitAction` | `EsvDefaultProjectileHit` |
| `Position` | `vec3` |
| `PrevPosition` | `vec3` |
| `PropertyList` | `StatsPropertyList` |


### ModuleInfo
| Property | Type |
|-|-|
| `DisplayDescription` | `string` |
| `DisplayName` | `string` |
| `GMTemplate` | `string` |
| `Hash` | `string` |
| `LobbyLevel` | `string` |
| `MenuLevel` | `string` |
| `ModVersion` | `Version` |
| `ModuleUUID` | `string` |
| `NumPlayers` | `number` |
| `PhotoBoothLevel` | `string` |
| `StartLevel` | `string` |
| `Tags` | `string[]` |
| `TargetModes` | `string[]` |



### UIObject
| Property | Type |
|-|-|
| `AnchorId` | `STDString` |
| `AnchorObjectName` | `FixedString` |
| `AnchorPos` | `STDString` |
| `AnchorTPos` | `STDString` |
| `AnchorTarget` | `STDString` |
| `ChildUIHandle` | `ComponentHandle` |
| `CustomScale` | `float` |
| `Flags` | `UIObjectFlags` |
| `FlashMovieSize` | `vec2` |
| `FlashSize` | `vec2` |
| `HasAnchorPos` | `bool` |
| `InputFocused` | `bool` |
| `IsDragging` | `bool` |
| `IsDragging2` | `bool` |
| `IsMoving2` | `bool` |
| `IsUIMoving` | `bool` |
| `Layer` | `int32` |
| `Left` | `float` |
| `MinSize` | `vec2` |
| `MovieLayout` | `int32` |
| `ParentUIHandle` | `ComponentHandle` |
| `Path` | `Path` |
| `PlayerId` | `int16` |
| `RenderDataPrepared` | `bool` |
| `RenderOrder` | `int32` |
| `Right` | `float` |
| `SysPanelPosition` | `ivec2` |
| `SysPanelSize` | `vec2` |
| `Top` | `float` |
| `Type` | `int32` |
| `UIObjectHandle` | `ComponentHandle` |
| `UIScale` | `float` |
| `UIScaling` | `bool` |
| `ClearCustomIcon` | `fun(self: UIObject, element: STDWString)` |
| `EnableCustomDraw` | `fun(self: UIObject)` |
| `GetUIScaleMultiplier` | `fun(self: UIObject):float` |
| `SetCustomIcon` | ``fun(self: UIObject, element: STDWString, icon: STDString, width: int32, height: int32, materialGuid: STDString\|nil)`` |


## New classes

### General

 - BaseComponent
 - Bound
 - GameTime
 - GlobalSwitches
 - GlobalSwitchesSoundSetting
 - GraphicSettings
 - IGameObject
 - IEoCClientObject
 - IEoCServerObject
 - Level
 - ModManager
 - Module
 - ModuleInfo
 - ModuleSettings
 - ModuleShortDesc
 - RuntimeStringHandle
 - Transform
 - TranslatedString
 - TypeInformation

### AI

 - EocAi
 - EsvAiAction
 - EsvAiCombos
 - EsvAiCombosCombo
 - EsvAiHelpers
 - EsvAiHelpersModifiers
 - EsvAiItemData
 - EsvAiKnownState
 - EsvAiKnownStateSet
 - EsvAiKnownStateSetStatusInteraction
 - EsvAiKnownStateSetSurfaceInteraction
 - EsvAiKnownStateSetSurfaceOnObjectAction
 - EsvAiModifiers
 - EsvAiRequest
 - EsvAiScore
 - EsvAiScoreImplDamageAmount
 - EsvAiSkillData
 - EsvAiTranslateOverride

### Alignment system

 - EsvAlignment
 - EsvAlignmentContainer
 - EsvAlignmentData
 - EsvAlignmentEntity
 - EsvHasRelationsObject

### Server objects

 - EsvInventory
 - EsvInventoryView
 - EsvLevel
 - EsvLevelManager
 - EsvSkillManager
 - EsvSkillState
 - EsvEffect

### Server environmental statuses

 - EsvEnvironmentalInfluences
 - EsvEnvironmentalInfluencesPermanentInfluence
 - EsvEnvironmentalInfluencesStatus
 - EsvEnvironmentalInfluencesTemporaryStatus
 - EsvEnvironmentalStatusManager


### Action states

 - EsvASAttack
 - EsvASPrepareSkill


### Template actions
 - IActionData
 - BookActionData
 - ConstrainActionData
 - ConsumeActionData
 - CraftActionData
 - CreatePuddleActionData
 - CreateSurfaceActionData
 - DestroyParametersData
 - DisarmTrapActionData
 - DoorActionData
 - IdentifyActionData
 - LockpickActionData
 - LyingActionData
 - PlaySoundActionData
 - RecipeActionData
 - RepairActionData
 - ShowStoryElementUIActionData
 - SitActionData
 - SkillBookActionData
 - SoundVolumeTriggerData
 - SpawnCharacterActionData
 - TeleportActionData
 - UseSkillActionData

### Templates
 - CharacterSkillData
 - InventoryItemData
 - LevelTemplate
 - SkillAIConditions
 - SkillAIParams
 - SurfacePathInfluence
 - SurfaceTemplateVisualData

### Resource system
 - Resource
 - DeferredLoadableResource
 - Material
 - MaterialParameter
 - Trigger
 - VisualResource
 - VisualResourceAttachment
 - VisualResourceBonePosRot
 - VisualResourceClothParam
 - VisualResourceObjectDesc
 - VisualSet
 - AnimationSet
 - AnimationSetAnimationDescriptor
 - Skeleton

### Character creation
 
 - CharacterCreationAbilityChange
 - CharacterCreationAiPersonalityDesc
 - CharacterCreationAttributeChange
 - CharacterCreationCharacterCreationManager
 - CharacterCreationClassDesc
 - CharacterCreationColorDesc
 - CharacterCreationEquipmentProperty
 - CharacterCreationOriginDesc
 - CharacterCreationRaceDesc
 - CharacterCreationVoiceDesc

### Client objects

 - EclInventory
 - EclInventoryView
 - EclLevel
 - EclLevelManager
 - EclSkill
 - EclSkillManager
 - EclStatusMachine

### Client effects

 - EclMultiEffectHandler
 - EclMultiEffectHandlerEffectInfo
 - EclMultiEffectHandlerMultiEffectVisual
 - EclMultiEffectHandlerWeaponAttachmentInfo

### Client character creation

 - EclCharacterCreationManager
 - EclCharacterCreationCustomization
 - EclCharacterCreationCustomizationState
 - EclUICharacterCreationWizard
 - EclUICharacterCreationWizardPoints

### Client rendering

 - Actor
 - AnimatableObject
 - MoveableObject
 - Light
 - PhysicsRagdoll
 - PhysicsShape
 - RenderableObject
 - AppliedMaterial
 - MaterialParameters
 - MaterialSamplerState
 - MaterialTexture2D
 - MaterialVector3
 - MaterialVector4
 - MeshBinding
 - ShaderDesc
 - Visual
 - VisualAttachment
 - VisualObjectDesc

### UI

 - DragDropManager
 - DragDropManagerPlayerDragInfo

