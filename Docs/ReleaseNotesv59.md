
# Changes in v59

## WIP!

The following engine functions are now accessible via Lua:
 - Client status controller state (`EclCharacter.StatusController`)
 - Client movement state (`EclCharacter.MovementMachine`)
 - Client and server skill states (`EclCharacter.SkillManager`, `EsvCharacter.SkillManager`) for the following skill types: `SkillStatePath`, `SkillStateTarget`, `SkillStateTeleportation`, `SkillStateProjectileStrike`, `SkillStateSummon`, `SkillStateRain`, `SkillStateZone`, `SkillStateHeal`, `SkillStateProjectile`, `SkillStateWall`, `SkillStateJump`, `SkillStateQuake`, `SkillStateMultistrike`, `SkillStateStorm`, `SkillStateTornado`, `SkillStateShout`, `SkillStateRush`, `SkillStateDome`
 - Client action state (via `EclCharacter.ActionMachine.Layers[x]`) for the following types: `ASAttack`, `ASDie`, `ASHit`, `ASIdle`, `ASAnimation`, `ASPrepareSkill`, `ASUseSkill`
 - Server character animation state (via `EsvCharacter.CharacterBody`)
 - Server action state (via `EsvCharacter.ActionMachine.Layers[x]`) for the following types: `ASAttack`, `ASDie`, `ASHit`, `ASIdle`, `ASAnimation`, `ASPickUp`, `ASDrop`, `ASMoveItem`, `ASPrepareSkill`, `ASUseSkill`

### Misc changes

 - User variable functions were moved from `Ext.Utils` to `Ext.Vars`
 - Added `Ext.Stats.GetItemBaseStats(statsId, level)` for retrieving a stats object containing the base stats for a given stats ID
 - `Ext.UI.SetDirty` now accepts flag values (eg. `SetDirty(_C().Handle, "CharacterSkillSet")`)

### Bugfixes

 - Fixed bug where user variables set on the client would not be synced to other clients in multiplayer
 - Fixed bug where user variables were not cleared on a savegame load if the savegame contained no saved variables

### Raycasting

The raycast functions can be used to check line of sight between two points or between a game object and a point.

These methods are part of the `VisionGrid` object that can be retrieved via `Ext.Entity.GetVisionGrid()`, `EclLevel.VisionGrid`, `EsvLevel.VisionGrid` and `EocAi.VisionGrid`.

#### VisionGrid:RaycastToPosition(fromPos, toPos, checkFlags) : VisionFlags

Performs a raycast between `fromPos` and `toPos`; stops when a cell matching any of the flags specified in `checkFlags` is found. The method returns the vision flags of the cell where the search stopped.

Example:
```lua
local flags = Ext.Enums.VisionFlags.VisionBlock
_D(Ext.Entity.GetVisionGrid():RaycastToPosition(_C().WorldPos, [150, 0, 100], flags))
-- prints ["VisionBlock"]
```
#### VisionGrid:RaycastToObject(fromPos, ai, checkFlags[, bboxBias]) : VisionFlags

Performs a raycast between `fromPos` and `ai` (the AI grid component of a game object, i.e. character/item); stops when a cell matching any of the flags specified in `checkFlags` is found. The method returns the vision flags of the cell where the search stopped.

### Custom Requirements

Custom skill and item requirements can be registered via the `Ext.Stats.AddRequirement(name)` function.
Requirements registered this way can be used in the `Requirements` and `MemorizationRequirements` stats entry fields.

Example:
```lua
local defn = Ext.Stats.AddRequirement("CustomFireSpecialist")
defn.Description = "text goes here"
defn.Callbacks.EvaluateCallback = function (req, ctx, char) 
    return char.FireSpecialist - char.WaterSpecialist > req.Param
end
```

The `Ext.Stats.AddRequirement()` function returns a requirement descriptor object where the name (`Description`, `DescriptionHandle`) as well as the custom requirement evaluation function `Callbacks.EvaluateCallback`) can be defined.

Each time the game evaluates a requirement, the `EvaluateCallback(requirement, context, character)` function is called (both server and client side).
The function takes the following parameters:
 - `requirement`: A `stats::Requirement` object containing requirement ID and parameter
 - `context`: Additional data about the context in which the function is called (see below)
 - `character`: The stats block (`stats::Character`) of the character being evaluated

The context object has the following fields:

| Property | Type | Description |
|-|-|-|
| `ServerCharacter` | `esv::Character` | Character being evaluated (server-side) |
| `ServerItem` | `esv::Item` | Set when checking requirements on an item (server-side) |
| `ClientCharacter` | `ecl::Character` | Character being evaluated (client-side) |
| `ClientItem` | `ecl::Item` | Set when checking requirements on an item (client-side)  |
| `ItemStats` | `stats::Item` | Set when checking requirements on an item |
| `SkillId` | `string` | Set when checking requirements for a skill |
| `CharacterStats` | `stats::Character` | The stats block of the character being evaluated |
| `IsInCombat` | `boolean` | |
| `IsImmobile` | `boolean` | |
| `HasCharges` | `boolean` | |
| `Tags` | `string[]` | |
| `Requirement` | `stats::Requirement` | |
| `ExcludeBoosts` | `boolean` | Evaluate base character stats or stats including boosts (potions, etc) |


Custom requirements are specified in stats entries just like normal requirement types:
```lua
data "Requirements" "CustomFireSpecialist 1"
```


### Mod variables

Mod variables are the equivalent of user variables for mods; i.e. they store and synchronize a set of variables for each mod. Mod variables are mostly functionally identical to user variables, so only the differences are highlighted here.
For a description of user variables see: [Custom Variable Docs](https://github.com/Norbyte/ositools/blob/master/Docs/ReleaseNotesv58.md#custom-variables)

To use a mod variable, the variable must first be registered with the variable manager:
```lua
Ext.Vars.RegisterModVariable(ModuleUUID, "VariableName", {
    Server = true,Client = true, SyncToClient = true
})
```

Mod variable registrations are kept separate for each mod UUID, so there is no need to use unique prefixes for variables since a mod registering a variable in its own table will have no effect on other mods.

The variables for a mod can be accessed by calling `Ext.Vars.GetModVariables(ModuleUUID)`:
```lua
local vars = Ext.Vars.GetModVariables(ModuleUUID)
Ext.Print(vars.VariableName)
vars.VariableName = 123
```

`Ext.Vars.SyncModVariables([moduleUuid])` can be called to perform an immediate synchronization of all mod variable changes.

