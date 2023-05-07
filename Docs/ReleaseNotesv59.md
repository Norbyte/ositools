
# Changes in v59

The following engine structures are now accessible via Lua:
 - Client status controller state (`EclCharacter.StatusController`)
 - Client movement state (`EclCharacter.MovementMachine`)
 - Client and server skill states (`EclCharacter.SkillManager`, `EsvCharacter.SkillManager`) for the following skill types: `SkillStatePath`, `SkillStateTarget`, `SkillStateTeleportation`, `SkillStateProjectileStrike`, `SkillStateSummon`, `SkillStateRain`, `SkillStateZone`, `SkillStateHeal`, `SkillStateProjectile`, `SkillStateWall`, `SkillStateJump`, `SkillStateQuake`, `SkillStateMultistrike`, `SkillStateStorm`, `SkillStateTornado`, `SkillStateShout`, `SkillStateRush`, `SkillStateDome`
 - Client action state (via `EclCharacter.ActionMachine.Layers[x]`) for the following types: `ASAttack`, `ASDie`, `ASHit`, `ASIdle`, `ASAnimation`, `ASPrepareSkill`, `ASUseSkill`
 - Server character animation state (via `EsvCharacter.CharacterBody`)
 - Server action state (via `EsvCharacter.ActionMachine.Layers[x]`) for the following types: `ASAttack`, `ASDie`, `ASHit`, `ASIdle`, `ASAnimation`, `ASPickUp`, `ASDrop`, `ASMoveItem`, `ASPrepareSkill`, `ASUseSkill`, `ASKnockedDown`, `ASSummoning`, `ASPolymorphing`, `ASUseItem`, `ASCombineItem`, `ASTeleportFall`, `ASSheath`, `ASUnsheath`, `ASIdentify`, `ASRepair`, `ASLockpick`, `ASDisarmTrap`, `ASIncapacitated`, `ASJumpFlight`
 - Server surface subsurfaces and cells

### Misc changes

 - User variable functions were moved from `Ext.Utils` to `Ext.Vars`; the `Ext.Utils` functions are kept for backwards compatibility.
 - Added `Ext.Stats.GetItemBaseStats(statsId, level)` for retrieving a stats object containing the base stats for a given stats ID
 - `Ext.UI.SetDirty` now accepts flag values (eg. `SetDirty(_C().Handle, "CharacterSkillSet")`)
 - `AI:GetCellInfo()` now returns AI flags and ground/cloud surface type when called on the client

### Bugfixes

 - Fixed bug where user variables set on the client would not be synced to other clients in multiplayer
 - Fixed bug where user variables were not cleared on a savegame load if the savegame contained no saved variables
 - Fixed bug where `CombatComponent` structure was mapped incorrectly
 - Fixed bug where dumping `Effect` structures could cause a crash
 - Added additional crash protection for mods changing visual data during the `OnCreateEquipmentVisuals` event
 - Fixed "No component found" message when exiting statuses on an item

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

### Custom Conditions

Custom stats conditions extend the built-in list of conditions that can be used in the stats conditions properties (`TargetConditions`, `AoEConditions`, etc.) as well as in the `IF(...)` part of `ExtraProperties` and `SkillProperties`. Conditions can be registered via the `Ext.Stats.AddCondition(name)` function.

Example:
```lua
local defn = Ext.Stats.AddCondition("CasterSneaking")
defn.Callbacks.EvaluateCallback = function (condition, conditionId, param)
    return condition.Caster == nil or condition.Caster.Stats.Sneaking
end
```

The `Ext.Stats.AddCondition()` function returns a condition descriptor where the condition evaluation function `Callbacks.EvaluateCallback`) can be defined.

Each time the game evaluates a condition, the `EvaluateCallback(condition, conditionId, param)` function is called (both server and client side).
The function takes the following parameters:
 - `condition`: A `stats::ServerConditionCheck` or `stats::ClientConditionCheck` object containing caster and target data
 - `conditionId`: Condition being evaluated
 - `param`: Optional string parameter specified to the condition in the stats entry

The condition object has the following fields:

| Property | Type | Description |
|-|-|-|
| `Caster` | `esv::Character` | Caster character (`nil` when there is no caster) |
| `TargetCharacter` | `esv::Item` | Target character (`nil` when the target is not a character) |
| `TargetItem` | `ecl::Character` | Target item (`nil` when the target is not a item) |
| `Position` | `vec3` | Target position |


Example usage in stats entries:
```lua
data "SkillProperties" "IF(CasterSneaking):EXPLODE,100,1"
```


### Mod variables

Mod variables are the equivalent of user variables for mods; i.e. they store and synchronize a set of variables for each mod. Mod variables are mostly functionally identical to user variables, so only the differences are highlighted here.
For a description of user variables see: [Custom Variable Docs](https://github.com/Norbyte/ositools/blob/master/Docs/ReleaseNotesv58.md#custom-variables)

To use a mod variable, the variable must first be registered with the variable manager:
```lua
Ext.Vars.RegisterModVariable(ModuleUUID, "VariableName", {
    Server = true, Client = true, SyncToClient = true
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

