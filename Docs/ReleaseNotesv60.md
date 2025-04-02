This release brings support for creating Custom Skill States, interacting with stats objects, modifying effects, character portraits and much more. 

# Changes and additions in v60

## Custom Skill States
Support for creating custom skill states has been added. Skill states control the `PrepareSkill` and `UseSkill` action states of a character, and on the client also handle visuals for targeting.

Due to their high complexity, custom skill states are implemented by using one of the existing skill types (Projectile, Zone, etc.) and overriding/extending their existing methods.

Custom skill states can be registered on a per-skill, per skill-type, or global basis:

- `Ext.Behaviour.Skill.AddById(skillID, ctor)` registers a custom skill state that only runs for the skill of the specified ID.
- `Ext.Behaviour.Skill.AddByType(skillType, ctor)` registers a custom skill state that runs for skills of specific type.
- `Ext.Behaviour.Skill.AddGlobal(ctor)` registers a custom skill state that runs for all skills.

These methods expect you to pass a function that constructs a table containing your skill state's callbacks. It is not necessary to define all methods. See [this annotations file](https://github.com/Norbyte/ositools/blob/master/ScriptExtender/Misc/CustomSkillStateHelpers.lua) for a list of callbacks.

Skill states are created on the client when preparing a skill, and later created on the server once the skill is cast. Skills used from the server side (ex. by AI or Osiris scripting) will result in only a server skill state being created.

Skill state callbacks run before the original engine method, and receive `CustomSkillEventParams` as their first parameter, and `EclSkillState` or `EsvSkillState` as their second parameter.
- Setting `CustomSkillEventParams.PreventDefault = true` will prevent the engine's original method from being invoked.
	- Return values of the callbacks are only used if the default engine behaviour is prevented. Nonetheless, the callbacks must always return a valid type.
- Setting `CustomSkillEventParams.StopEvent = true` will prevent any subsequent callbacks from being ran for that event, in the case that there are multiple custom skill states running. The engine method will still run unless it was prevented.

The visual effects and targeting logic are handled by `Enter()`, `Update()` and `Exit()`.

## Stats Object Support
Support for fetching and modifying stats objects has been added, via the `Ext.Stats.GetRaw()` function. This allows interacting with them as with other extender objects: fields can be read and even written to modify stats at runtime.

![Stats object support.](https://i.imgur.com/IPwlBC2.png)

It is also possible to determine the type of the stat via additional fields like `ModifierListIndex`.

## Effects
Support for accessing AllSpark effects on the client has been added, allowing components and their properties to be accessed and modified at runtime while the effect plays.

Effects can be fetched using `Ext.Entity.GetEffect(handle)`. Effect handles can be obtained from various game classes, including skill states.

See the `Effect`, `AspkEffect` and `FxEffect` classes for a list of properties. AllSpark components can be accessed through `AspkEffect.Components`. In addition, some methods such as `Stop()`, `Play()` and `UpdateParametersFromResource()` are available.
## Console
- A `clear` command has been added, which clears the console window. (PR #127)
- New console-related settings have been added to `OsirisExtenderSettings.json`:
	- `ClearOnReset` (boolean, defaults to `false`): automatically clears the console after a Lua reset, as per the `clear` command. (PR #127)
	- `DefaultToClientConsole` (boolean, defaults to `false`): makes the Lua console use the client context by default. (PR #121)

## Templates
The following `GameObjectTemplate` types are now mapped: `DecalTemplate`, `LightTemplate`, `LightProbeTemplate`, `PrefabTemplate`, `TerrainTemplate`, and `WallConstructionTemplate` (including its sub-components).

New functions have been added to fetch templates:
- `Ext.Template.GetAllRootTemplates()`: returns all root templates - this refers to templates created within the editor.
- `Ext.Template.GetAllLocalTemplates()`: returns all templates local to the level - these are template instances placed from the editor for the current level.
- `Ext.Template.GetAllLocalCacheTemplates()`: returns all local cache templates -  these are template instances that are specific to the level and kept in the savegame.
- `Ext.Template.GetAllCacheTemplates()`: returns all cache templates - these are template instances that are kept in the savegame, but are not specific to any level. This will include templates for items and characters that have been moved from their original level.

## Game.Math
A transcript of the `ApplyDamage` function has been added, as well as various related functions, such as healing logic. Not all functionality of these methods is currently present in the extender; these have been marked as `FIXME`.

A new function `Ext.Stats.Math.GetSkillAPCost(stats: CDivinityStats_Character, skillID: string, worldPos: vec3, boundsSize: number): integer, boolean` has been added, which respects lua hooks and returns the AP cost and whether elemental affinity was applied.

## Ext.IO
Added new functions which allow traversing the `Osiris Data` directory (PR #130):

```lua
---Returns the files and directories at a path.
---@param path string
---@return string[]
function Ext.IO.Enumerate(path) end

---Returns whether the item at the path is a directory.
---@param path string
---@return boolean
function Ext.IO.IsDirectory(path) end

---Returns whether the item at the path is a file.
---@param path string
---@return boolean
function Ext.IO.IsFile(path) end
```

`Ext.IO.SaveFile()` now supports an optional `size` param (integer) to allow it to save strings that contain the character with code `0`.

Additionally, an issue was fixed where files could not be saved if it would require creating more than one directory along the way.

## Ext.Entity
Added new functions related to character visuals, outlines and portraits:

```lua
---Sets the outline shader to use for an entity.
---**Client-only**.
---@param playerID uint8
---@param entityHandle ComponentHandle
---@param highlight integer
---@param unknownBool boolean
function Ext.Entity.SetHighlight(playerID, entityHandle, highlight, unknownBool) end

---Sets the custom portrait of a character.
---@param entityHandle ComponentHandle Must have custom portrait enabled in CustomData.
---@param width integer
---@param height integer
---@param ddsStream string Expected to be a BC1 DDS stream, including header.
function Ext.Entity.SetPortrait(entityHandle, width, height, ddsStream) end

---Returns the custom portrait of a character as a DDS file.
---**Client-only**.
---@param entityHandle ComponentHandle Must have custom portrait enabled in CustomData.
---@return string -- DDS file stream, including header.
function Ext.Entity.GetPortrait(entityHandle) end

---Refreshes a character's equipment visuals.
---Will also cause the `CreateEquipmentVisuals` event to be fired again.
---**Client-only**.
---@param characterHandle ComponentHandle
function Ext.Entity.RefreshEquipmentVisuals(characterHandle) end
```

For `SetHighlight`, the following `highlight` IDs are accepted:

```lua
HIGHLIGHT_TYPES = {
	NONE = 0,
	SELECTED = 1, -- White.
	ENEMY = 2,
	ATTITUDE_BASED = 3,
	NONE_2 = 4, -- Purpose unknown.
	YELLOW = 5,
}
```

## Naming Consistency
Some fields that have the same usage in both client and server classes previously had inconsistent names across the different contexts. New aliases have been created for some of these fields for consistency; they remain accessible under their old names for backwards compatibility.

- `EsvCharacter.OwnerHandle` is aliased to `OwnerCharacterHandle`
- `EsvItem.OwnerHandle` is aliased to `OwnerCharacterHandle`
- `EclItem.InventoryParentHandle` is aliased to `ParentInventoryHandle`
- `EclItem.CurrentSlot` is aliased to `Slot`
- `EsvTurnManager` may now also be fetched using `Ext.Entity.GetTurnManager()`
- `EclStatusCombat.CombatTeamId` is now `OwnerTeamId` (no backwards compatibility)

## Miscellaneous
- Added the `CharacterGetWeaponAnimationSetType` event, which allows changing the weapon animations for characters
- Added `UIObject:SetCustomPortraitIcon(iconName: string, characterHandle: ComponentHandle, width: integer, height: integer, materialGUID: string?,  zeroSaturation: boolean?)`, which allows rendering portraits of characters (including custom ones) as Iggy icons
- Added `Ext.Utils.GetCommandLineParams()`
- Attributes unmapped in `Game.Tooltip.TooltipStatAttributes` are now passed to tooltip listeners (PR #126)
- Fixed altering treasure tables causing them to drop their items on the next roll regardless of the weights set (PR #124)
- Various annotation typing fixes have been made to `Game.Math`, and deprecated method calls have been replaced with v56+ ones
- `BeforeCharacterApplyDamage` event now has `EnterCombat` and `AttackerEntity` properties
- Added `InUse` flag to `EsvItem`
- Added support for throwing deferred Osiris events using `Osi.SomeEvent:Defer(...)`, which are processed in the next tick. These are used for example by `ApplyDamage`.
- `DragDropManager:StartDraggingObject()` now takes a third boolean parameter that determines if the drag disallows items to be dropped to the ground
- Fixed a bug that prevented custom UIs from being properly re-created under the same name/ID
- Added `Ext.Combat.TryEnterCombat()` on server
- Added `EsvLevel.ShroudManager` as well as new properties to client shroud manager
- Various properties in visual classes such as `Actor`, `MeshBinding`, `VisualAttachment`, `Visual`, and `Light` are no longer read-only
- `MoveableObject`, `LightProbe`, `RenderableObject`, and certain other visual-related classes are now mapped
- Fixed sign issues with `StatsLevelMap:GetScaledValue()`
- GUIDs are now synchronized to the client in cases where they were not set (ex. characters/items created dynamically), allowing uservars to be used safely in such cases
- `UIObjectManagerPlayerState`'s `ActiveUIObjectHandle` and `UIUnderMouseCursor` fields are no longer read-only
- Fixed the console saying "Enter server Lua console" even when the console was previously set to client context (PR #136)
- Added a warning when attempting to run an unregistered console command, so it no longer just fails silently
- Removed an unnecessary "Synching user vars to unknown game object" warning message, as valid scenarios of this exist (ex. if the object is on another level)
- Added `Ext.Client.GetGameControl(): EclGameControl`, though this structure currently only contains the `CanSendToLadyVengeance` field
- Updated `OsirisExtenderSettings.json` documentation
