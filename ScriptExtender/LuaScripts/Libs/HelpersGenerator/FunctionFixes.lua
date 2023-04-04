local _MissingFuncData = {
	Ext_ServerServer = {
		GetGameState = {
			Return = {
				"EsvGameState gameState"
			}
		},
	},
	Ext_ClientEntity = {
		Override = true,
		GetGameObject = {
			Params = {
				{
					name = "handle",
					arg = "ComponentHandle|Guid",
					description = "Game object handle or Guid. NetID will fail since it has no type information (an item and a character could have the same NetID).",
				},
			},
			Return = { "IEoCClientObject object" }
		},
		GetCharacter = {
			Params = {
				{
					name = "handle",
					arg = "ComponentHandle|Guid|NetId",
					description = "Character Guid, Handle, or NetID.",
				},
			},
			Return = { "EclCharacter character" }
		},
		GetItem = {
			Params = {
				{
					name = "handle",
					arg = "ComponentHandle|Guid|NetId",
					description = "Character Guid, Handle, or NetID.",
				},
			},
			Return = { "EclItem item" }
		},
		GetProjectile = {
			Params = {
				{
					name = "handle",
					arg = "ComponentHandle|NetId",
					description = "Projectile Handle or NetID.",
				}
			},
			Return = { "EclProjectile projectile" }
		},
		GetStatus = {
			Params = {
				{
					name = "characterHandle",
					arg = "ComponentHandle|Guid|NetId"
				},
				{
					name = "statusHandle",
					arg = "ComponentHandle|NetId"
				},
			},
			Description = "--- Get a specific, active status from a character."
		},
	},
	Ext_ServerEntity = {
		Override = true,
		GetGameObject = {
			Params = {
				{
					name = "handle",
					arg = "ComponentHandle|Guid",
					description = "Game object handle or Guid. NetID will fail since it has no type information (an item and a character could have the same NetID).",
				}
			},
			Return = { "IEoCServerObject object" }
		},
		GetCharacter = {
			Params = {
				{
					name = "handle",
					arg = "ComponentHandle|Guid|NetId",
					description = "Character Guid, Handle, or NetID.",
				}
			},
			Return = { "EsvCharacter character" }
		},
		GetItem = {
			Params = {
				{
					name = "handle",
					arg = "ComponentHandle|Guid|NetId",
					description = "Character Guid, Handle, or NetID.",
				}
			},
			Return = { "EsvItem item" }
		},
		GetTrigger = {
			Params = {
				{
					name = "handle",
					arg = "ComponentHandle|Guid|NetId",
					description = "Trigger Guid, Handle, or NetID.",
				}
			},
			Return = { "EsvTrigger trigger" }
		},
		GetProjectile = {
			Params = {
				{
					name = "handle",
					arg = "ComponentHandle|NetId",
					description = "Projectile Handle or NetID.",
				}
			},
			Return = { "EsvProjectile projectile" }
		},
		GetStatus = {
			Params = {
				{
					name = "objectHandle",
					arg = "ComponentHandle|NetId",
				},
				{
					name = "statusHandle",
					arg = "ComponentHandle|integer",
				},
			},
			Description = "--- Get a specific, active status from a character or item."
		},
		GetCombat = {
			Params = {{
				name = "combatId",
				arg = "uint32",
				description = "The ID of the combat to retrieve.",
			}},
			Return = { "EsvCombat combat" }
		},
		GetCurrentLevelData = {
			Return = { "LevelDesc levelData" }
		},
	},
	Ext_Json = {
		Parse = {
			Params = {{
				name = "str",
				arg = "string",
				description = "String to convert to a table.",
			}},
			Return = {
				"table parsedTable"
			}
		},
		Stringify = {
			Override = true,
			Params = {
				{
					name = "data",
					arg = "table",
					description = "Table to convert to a string.",
				},
				{
					name = "opts",
					arg = "JsonStringifyOptions|nil",
					description = "Optional settings for the stringify operation",
				},
			},
			Return = {
				"string tableAsString"
			}
		},
	},
	Ext_ClientUI = {
		GetPickingState = {
			Return = {
				"EclPickingState pickingState"
			}
		},
		Create = {
			Override = true,
			Params = {
				{
					name = "name",
					arg = "string",
					description = "A user-defined unique name that identifies the UI element. To avoid name collisions, the name should always be prefixed with the mod name (e.g. `NRD_CraftingUI`)",
				},
				{
					name = "path",
					arg = "string",
					description = "Path of the SWF file relative to the data directory (e.g. `\"Public/ModName/GUI/CraftingUI.swf\"`).**This path must exist or the game will crash!**",
				},
				{
					name = "layer",
					arg = "int32",
					description = "Stack order of the UI element. Overlapping elements with a larger layer value cover those with a smaller one.",
				},
				{
					name = "flags",
					arg = "int32|UIObjectFlags[]|nil",
					description = "Optional UI object flags; default: `{\"Load\"|\"PlayerInput1\"|\"DeleteOnChildDestroy\"}`",
				},
			},
		},
	},
	Ext_ClientVisual = {
		CreateOnCharacter = {
			Override = true,
			Params = {
				{
					name = "position",
					arg = "vec3",
					description = "The position of the visual.",
				},
				{
					name = "target",
					arg = "EclCharacter|nil",
					description = "Optional target.",
				},
				{
					name = "listenForTextKeys",
					arg = "EclCharacter|nil",
					description = "Optional target to sync with animations.",
				}
			},
		},
		CreateOnItem = {
			Override = true,
			Params = {
				{
					name = "position",
					arg = "vec3",
					description = "The position of the visual.",
				},
				{
					name = "target",
					arg = "EclItem|nil",
					description = "Optional target.",
				},
				{
					name = "listenForTextKeys",
					arg = "EclItem|nil",
					description = "Optional target to sync with animations.",
				}
			},
		},
	},
	Ext_Stats = {
		SetLevelScaling = {
			Params = {
				{
					name = "modifierListName",
					arg = 'FixedString|"Character"|"Weapon"|"Armor"|"Shield"',
					description = "Stat type to override (Character)",
				},
				{
					name = "modifierName",
					arg = "FixedString",
					description = "Stat attribute to override (`Strength`, `Constitution`, `Gain`, etc)",
				},
				{
					name = "callback",
					arg = "fun(value, level):integer",
					description = "A function that takes a level, level and return the inter level-scaled value.",
				}
			},
		},
		SetAttribute = {
			Override = true,
			Params = {
				{
					name = "statName",
					arg = "string",
					description = "The stat ID",
				},
				{
					name = "attributeName",
					arg = "FixedString",
					description = "Stat attribute to change",
				},
				{
					name = "value",
					arg = "any"
				}
			},
		},
		NewDamageList = {
			Return = {"StatsDamagePairList damageList"}
		},
		GetStats = {
			Params = {{
				name = "statType",
				arg = "StatType|nil",
				description = "Optional stat type to filter stats by.",
			}},
			Return = {
				"FixedString[]"
			}
		},
	},
	Ext_Utils = {
		Print = {
			Params = {{
				name = "...",
				arg = "any",
			}}
		},
		PrintError = {
			Params = {{
				name = "...",
				arg = "any",
			}}
		},
		PrintWarning = {
			Params = {{
				name = "...",
				arg = "any",
			}}
		},
		Random = {
			Override = true,
			Params = {
				{
					name = "low",
					arg = "integer",
				},
				{
					name = "up",
					arg = "integer",
				}
			},
			Return = {"int64 result"},
			Overload = {"--- @overload fun(num:integer|nil):int64"}
		},
		GetDifficulty = {
			Return = {"GameDifficultyValue difficulty"}
		},
		IsValidHandle = {
			Params = {{
				name = "handle",
				arg = "ComponentHandle",
			}}
		},
		GetHandleType = {
			Params = {{
				name = "handle",
				arg = "ComponentHandle",
			}}
		},
		GetValueType = {
			Params = {{
				name = "value",
				arg = "any",
			}},
			Return = {
				[[string|"nil"|"number"|"string"|"boolean"|"table"|"function"|"thread"|"userdata"|"lightuserdata" valueType # Returns a base type, lightuserdata, or an extender type.]]
			}
		},
		MakeHandleObject = {
			Params = {{
				name = "handle",
				arg = "ComponentHandle",
			}},
			Return = {
				"ComponentHandleProxy handleObject"
			}
		},
		Include = {
			Params = {
				{
					name = "modGUID",
					arg = "string|nil",
					description = "The ModuleUUID value",
				},
				{
					name = "path",
					arg = "string",
					description = "The path to the script, relative to the Lua folder",
				},
				{
					name = "replaceGlobals",
					arg = "table|nil",
					description = "If set, the global environment of the script is replaced with this table",
				},
			},
			Return = {
				"any returnedValue Whatever the script returned, if anything"
			}
		},
		GetGameMode = {
			Return = {
				"\"Campaign\"|\"Arena\"|\"GameMaster\"",
			}
		}
	},
	Ext_Types = {
		GetObjectType = {
			Description = "--- Get the type name for any userdata, lightuserdata, or cppobject.",
			Params = {{
				name = "object",
				arg = "any",
				description = "Any userdata, lightuserdata, or cppobject type."
			}},
			Return = {
				"FixedString|\"userdata\"|\"lightuserdata\" objectType"
			}
		},
		
	},
	Ext_Mod = {
		GetModInfo = {
			Return = {"LegacyModuleInfo info"}
		}
	},
	Ext_ServerPropertyList = {
		ExecuteSkillPropertiesOnPosition = {
			Description = "--- Execute the SkillProperties of a given skill on a position.",
			Params = {
				{
					name = "skillId",
					arg = "FixedString"
				},
				{
					name = "attacker",
					arg = "EsvCharacter"
				},
				{
					name = "position",
					arg = "vec3"
				},
				{
					name = "radius",
					arg = "number"
				},
				{
					name = "propertyContext",
					arg = "StatsPropertyContext|StatsPropertyContext[]",
					description = "The context of the skill properties, such as Self or Target, which controls which actions are applied to whom."
				},
				{
					name = "isFromItem",
					arg = "boolean"
				},
			}
		},
		ExecuteSkillPropertiesOnTarget = {
			Description = "--- Execute the SkillProperties of a given skill on a character.",
			Params = {
				{
					name = "skillId",
					arg = "FixedString"
				},
				{
					name = "attacker",
					arg = "EsvCharacter"
				},
				{
					name = "target",
					arg = "EsvCharacter"
				},
				{
					name = "position",
					arg = "vec3"
				},
				{
					name = "propertyContext",
					arg = "StatsPropertyContext|StatsPropertyContext[]",
					description = "The context of the skill properties, such as Self or Target, which controls which actions are applied to whom."
				},
				{
					name = "isFromItem",
					arg = "boolean"
				},
			}
		},
	},
	Ext_ClientAudio = {
		GetRTPC = {
			Description = "--- Get the value for a 'Real-Time Parameter Controls' WWise variable.",
			Params = {
				{
					name = "gameObject",
					arg = "ObjectHandle|SoundObjectId|nil"
				},
				{
					name = "rtpcName",
					arg = "RTPCName|string"
				}
			}
		},
		ResetRTPC = {
			Description = "--- Reset a 'Real-Time Parameter Controls' WWise variable.",
			Params = {
				{
					name = "gameObject",
					arg = "ObjectHandle|SoundObjectId|nil"
				},
				{
					name = "rtpcName",
					arg = "RTPCName|string"
				}
			}
		},
		SetRTPC = {
			Description = "--- Set a 'Real-Time Parameter Controls' WWise variable.",
			Params = {
				{
					name = "gameObject",
					arg = "ObjectHandle|SoundObjectId|nil"
				},
				{
					name = "rtpcName",
					arg = "RTPCName|string"
				},
				{
					name = "value",
					arg = "number"
				}
			}
		},
		PlayExternalSound = {
			Description = "--- Get a value for a 'Real-Time Parameter Controls' WWise variable.",
			Params = {
				{
					name = "gameObject",
					arg = "ObjectHandle|SoundObjectId|nil"
				},
				{
					name = "eventName",
					arg = "string",
					description = "Event to trigger",
				},
				{
					name = "path",
					arg = "string",
					description = "Audio file path (relative to data directory)",
				},
				{
					name = "codecId",
					arg = "integer",
					description = "Audio file path (relative to data directory)",
				}
			}
		},
		PostEvent = {
			Description = "--- Get a value for a 'Real-Time Parameter Controls' WWise variable.",
			Params = {
				{
					name = "gameObject",
					arg = "ObjectHandle|SoundObjectId|nil"
				},
				{
					name = "eventName",
					arg = "string",
					description = "Event to trigger",
				},
				{
					name = "positionSec",
					arg = "number|nil",
					description = "Optional position to start at in the audio track.",
				}
			}
		},
		SetState = {
			Override=true,
			Description = "--- Set an audio state for a given SyncGroup.  \n--- You can find the the group and state names in this file:  \n--- `Public\\Shared\\Assets\\Sound\\SoundSyncs.lsx`",
			Params = {
				{
					name = "stateGroup",
					arg = "StateGroupName|string"
				},
				{
					name = "state",
					arg = "string",
					description = "A SoundSyncs Switch name within the given SyncGroup, i.e. \"Boss_Theme_01\".",
				}
			}
		},
		SetSwitch = {
			Description = "--- Set an audio state switch, such as making a character sound like a ghoul.  \n--- You can find the the group and switch names in this file:  \n--- `Public\\Shared\\Assets\\Sound\\SoundSyncs.lsx`",
			Params = {
				{
					name = "gameObject",
					arg = "ObjectHandle|SoundObjectId|nil"
				},
				{
					name = "switchGroup",
					arg = "SwitchGroupName|string"
				},
				{
					name = "state",
					arg = "string",
					description = "A SoundSyncs Switch name within the given SyncGroup, i.e. \"Boss_Theme_01\".",
				}
			}
		},
		Stop = {
			Description = "--- Stop audio for an object.",
			Params = {{
				name = "gameObject",
				arg = "ObjectHandle|SoundObjectId|nil"
			}}
		}
	},
	Ext_ClientBehavior = {
		AttachCharacterTask = {
			Override=true,
			Description = "--- Attach a custom task created with `Ext.Behavior.RegisterCharacterTask`.",
			Params = {
				{
					name = "character",
					arg = "EclCharacter"
				},
				{
					name = "taskId",
					arg = "FixedString",
					description = "The ID for the task used in `Ext.Behavior.RegisterCharacterTask`.",
				}
			}
		},
		RegisterCharacterTask = {
			Override=true,
			Description = "--- Create and register a custom scripted task.\n--- Tasks can allow you to define your own actions for specific contexts, such as allowing players to 'fish' over a deep water surface.",
			Params = {
				{
					name = "taskId",
					arg = "FixedString",
					description = "A unique ID for this task.",
				},
				{
					name = "taskClass",
					arg = "UserspaceCharacterTask",
					description = "A table that specifies specific callback functions to invoke when this task is attached to a character.",
				}
			}
		}
	},
}
_MissingFuncData.Ext_ClientClient = {GetGameState = _MissingFuncData.Ext_ServerServer.GetGameState}

_MissingFuncData.Ext_ClientEntity.GetCharacterLegacy = {Params=_MissingFuncData.Ext_ClientEntity.GetCharacter.Params, Return={"EclCharacter legacyCharacter # A character with backwards-compatibility (to avoid lifetime errors)."}}
_MissingFuncData.Ext_ServerEntity.GetCharacterLegacy = {Params=_MissingFuncData.Ext_ServerEntity.GetCharacter.Params, Return={"EsvCharacter legacyCharacter # A character with backwards-compatibility (to avoid lifetime errors)."}}

local _MissingFuncFieldData = {
	Ext_StatsEquipmentSet = {
		GetLegacy = "fun(id:FixedString):StatEquipmentSet",
		Update = "fun(tbl:StatEquipmentSet)",
	},
	Ext_StatsDeltaMod = {
		GetLegacy = "fun(id:FixedString, modifierType:FixedString):DeltaMod",
		Update = "fun(tbl:DeltaMod)",
	},
	Ext_StatsItemCombo = {
		GetLegacy = "fun(id:FixedString):ItemCombo",
		Update = "fun(tbl:ItemCombo)",
	},
	Ext_StatsItemComboPreview = {
		GetLegacy = "fun(id:FixedString):ItemComboPreviewData",
		Update = "fun(tbl:ItemComboPreviewData)",
	},
	Ext_StatsItemGroup = {
		GetLegacy = "fun(id:FixedString):ItemGroup"
	},
	Ext_StatsItemSet = {
		GetLegacy = "fun(id:FixedString):ItemSet",
		Update = "fun(tbl:ItemSet)",
	},
	Ext_StatsNameGroup = {
		GetLegacy = "fun(id:FixedString):ItemNameGroup"
	},
	Ext_StatsSkillSet = {
		GetLegacy = "fun(id:FixedString):StatSkillSet",
		Update = "fun(tbl:StatSkillSet)",
	},
	Ext_StatsTreasureCategory = {
		GetLegacy = "fun(id:FixedString):StatTreasureCategory",
		Update = "fun(id:FixedString, tbl:StatTreasureCategory)",
	},
	Ext_StatsTreasureTable = {
		GetLegacy = "fun(id:FixedString):StatTreasureTable",
		Update = "fun(tbl:StatTreasureTable)",
	},
	Ext_StatsItemColor = {
		Get = "fun(id:FixedString):StatsItemColorDefinition",
		GetAll = "fun():StatsItemColorDefinition[]",
		Update = "fun(tbl:StatsItemColorDefinition)",
	},
	Ext_Stats = {
		AddAttribute = "fun(modifierList:FixedString, modifierName:FixedString, typeName:FixedString):boolean",
		AddDamageType = "fun(damageType:FixedString, overwriteBuiltIn:boolean|nil):CustomDamageTypeDescriptor",
		AddEnumerationValue = "fun(typeName:FixedString, enumLabel:FixedString):int32",
		AddRequirement = "fun(id:FixedString, overwriteBuiltIn:boolean|nil):CustomRequirementDescriptor",
	},
	IEoCServerObject = {
		GetStatusObjects = "fun(self:IEoCClientObject):EclStatus"
	},
	IEoCClientObject = {
		GetStatusObjects = "fun(self:IEoCClientObject):EclStatus"
	},
	UIObject = {
		ExternalInterfaceCall = "fun(self:UIObject, method:string, ...:string|number|boolean)",
		GetRoot = "fun(self:UIObject):FlashMainTimeline|table",
		Invoke = "fun(self:UIObject, method:string, ...:string|number|boolean):boolean",
	},
	EocAiGrid = {
		GetCellInfo = "fun(self:EocAiGrid, x:number, z:number):{Flags:integer, Height:number, Objects:ComponentHandle[]|nil}"
	},
	StatsDamagePairList = {
		ToTable = "fun(self:StatsDamagePairList):{Amount:integer, DamageType:StatsDamageType}[]",
		CopyFrom = "fun(self:StatsDamagePairList, other:StatsDamagePairList)",
		Merge = "fun(self:StatsDamagePairList, other:StatsDamagePairList)",
	},
	EclLuaVisualClientMultiVisual = {
		AddVisual = "fun(self:EclLuaVisualClientMultiVisual, visualId:FixedString, opts:ClientMultiVisualAddVisualOptions|nil):Visual"
	},
	Ext_ServerAction = {
		ExecuteGameAction = "fun(action:EsvGameAction, targetPos:vec3|nil):Visual"
	},
	StatsSkillPrototype = {
		StatsObject = "StatsObject|StatEntryType"
	},
	StatsObject = {
		StatsEntry = "StatsObject|StatEntryType"
	},
	Ext_ClientBehavior = {
		AttachCharacterTask = "fun(character:EclCharacter, taskId:FixedString)",
		RegisterCharacterTask = "fun(taskId:FixedString, taskTableConstructor:(fun():UserspaceCharacterTask))",
	},
	CustomDamageTypeCallbacks = {
		GetDamageBoostCallback = "(fun(target:CDivinityStatsCharacter, damageType:StatsDamageType|string):number)|nil",
		GetResistanceCallback = "(fun(target:CDivinityStatsCharacter, damageType:StatsDamageType|string, isBaseValue:boolean):int32)|nil",
		GetMagicArmorDamageCallback = "(fun(target:CDivinityStatsCharacter, damageType:StatsDamageType|string, int32:damageAmount):int32)|nil Not implemented yet",
		GetArmorDamageCallback = "(fun(target:CDivinityStatsCharacter, damageType:StatsDamageType|string, int32:damageAmount):int32)|nil Not implemented yet",
	},
	Ext_Utils = {
		LoadString = "fun(str:string):UserReturn Similar to lua `loadstring`, with extra safeguards.",
	},
	Ext_Vars = {
		DirtyModVariables = "fun(modGuid?:FixedString, key?:FixedString) Mark a mod's variable, or all variables, as 'dirty', triggering a sync.",
		DirtyUserVariables = "fun(gameObject?:FixedString, userVariableID?:FixedString) Mark a variable, or all variables, as 'dirty', triggering a sync.",
		GetModVariables = "fun(modGuid:FixedString):table",
		RegisterModVariable = "fun(modGuid:FixedString, id:FixedString, opts?:RegisterUserVariableOptions)",
		RegisterUserVariable = "fun(id:FixedString, opts?:RegisterUserVariableOptions) After registration, custom variables can be read/written through the UserVars property on characters and items.<br>See here: https://github.com/Norbyte/ositools/blob/master/Docs/ReleaseNotesv58.md#custom-variables<br><h4>Usage Notes:</h4><ul><li>Since variable prototypes are used for savegame serialization, network syncing, etc., they must be registered before the savegame is loaded and every time the Lua context is reset; performing the registration when `BootstrapServer.lua` or `BootstrapClient.lua` is loaded is recommended.</li><li>Although the variables registered server-side and client-side can differ, it is recommended to register all variables on both sides (even if they're server-only or client-only) for consistency.</li><li>Variable names, much like Osiris DB names are global; it is recommended to prefix them with your mod name to ensure they're unique.</li><li>Variables must be registered with the same settings on both client and server, otherwise various synchronization issues may occur.</ul>",
	},
	StatsNamedElementManager_StatsLevelMap = {
		GetByName = "fun(self:StatsNamedElementManager_StatsLevelMap, id:LevelMapName):StatsLevelMap"
	}
}

--Turned into fields in v58
_MissingFuncFieldData.Ext_ClientEntity = {
	GetCharacterLegacy = "fun(handle:ComponentHandle|Guid|NetId):EclCharacter",
	GetItemLegacy = "fun(handle:ComponentHandle|Guid|NetId):EclItem",
	GetProjectile = "fun(handle:ComponentHandle|NetId):EclProjectile",
	GetStatus = "fun(handle:ComponentHandle|NetId):EclStatus",
}

return {
	Regular = _MissingFuncData,
	Field = _MissingFuncFieldData,
}