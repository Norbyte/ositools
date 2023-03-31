--- @diagnostic disable

--- Table that contains every ModTable entry for active mods.
Mods = {}

--- Special global value that contains the current mod UUID during load
--- @type FixedString
ModuleUUID = "UUID"

--- Table that gets stored in the save, unique for each ModTable entry.
--- @type table
PersistentVars = {}

--- @alias OsirisValue number|string

--- Using a DB like a function will allow inserting new values into the database (ex. `Osi.DB_IsPlayer("02a77f1f-872b-49ca-91ab-32098c443beb")`  
--- @overload fun(...:OsirisValue|nil)
--- @class OsiDatabase
local OsiDatabase = {}
--- Databases can be read using the Get method. The method checks its parameters against the database and only returns rows that match the query.  
--- The number of parameters passed to Get must be equivalent to the number of columns in the target database.  
--- Each parameter defines an (optional) filter on the corresponding column.  
--- If the parameter is nil, the column is not filtered (equivalent to passing _ in Osiris). If the parameter is not nil, only rows with matching values will be returned.
--- @vararg OsirisValue|nil
--- @return table<integer,table<integer,OsirisValue>>
function OsiDatabase:Get(...) end
--- The Delete method can be used to delete rows from databases.  
--- The number of parameters passed to Delete must be equivalent to the number of columns in the target database.  
--- Each parameter defines an (optional) filter on the corresponding column.  
--- If the parameter is nil, the column is not filtered (equivalent to passing _ in Osiris). If the parameter is not nil, only rows with matching values will be deleted. 
--- @vararg OsirisValue|nil
function OsiDatabase:Delete(...) end

--- @alias OsiFunction fun(...:OsirisValue):OsirisValue|nil
--- @alias OsiDynamic table<string, OsiFunction|OsiDatabase>

--- @class OsiCommonDatabases
--- @field DB_IsPlayer OsiDatabase|fun(Guid:string) All player characters
--- @field DB_Origins OsiDatabase|fun(Guid:string) All origin characters
--- @field DB_Avatars OsiDatabase|fun(Guid:string) All player characters that were created in character creation, or that have an `AVATAR` tag
--- @field DB_CombatObjects OsiDatabase|fun(Guid:string, combatID:integer) All objects in combat
--- @field DB_CombatCharacters OsiDatabase|fun(Guid:string, combatID:integer) All characters in combat
--- @field DB_Dialogs OsiDatabase|fun(Guid:string, dialog:string)|fun(GUID1:string, GUID2:string, dialog:string)|fun(GUID1:string, GUID2:string, GUID3:string, dialog:string)|fun(GUID1:string, GUID2:string, GUID3:string, GUID4:string, dialog:string) All registered dialogs for objects, the most common being the version with a single character

--- The Osi table contains databases as well as calls, queries, events, and custom PROC / QRY defintions, as long as they are used in a script.  
--- @type OsiCommonDatabases|OsiDynamic
Osi = {}

--- @alias i16vec2 int16[]

--- @alias SkillAbility "None"|"Warrior"|"Ranger"|"Rogue"|"Source"|"Fire"|"Water"|"Air"|"Earth"|"Death"|"Summoning"|"Polymorph"
--- @alias SkillElement SkillAbility

--- @alias GameDifficultyValue uint32
---|0 # Story
---|1 # Explorer
---|2 # Classic
---|3 # Tactician
---|4 # Honour

--- @alias RTPCName "PlaybackSpeed"|"RTPC_Rumble"|"RTPC_Volume_Ambient"|"RTPC_Volume_Cinematic"|"RTPC_Volume_FX"|"RTPC_Volume_MAIN"|"RTPC_Volume_Music"|"RTPC_Volume_Music_Fight"|"RTPC_Volume_UI"|"RTPC_Volume_VO_Dialog"|"RTPC_Volume_VO_Master"|"RTPC_Volume_VO_Narrator"|"RTPC_Volume_VO_Overhead"

--- @alias SoundObjectId "Global"|"Music"|"Ambient"|"HUD"|"GM"|"Player1"|"Player2"|"Player3"|"Player4"

--- @alias StateGroupName "ARX_Dead"|"ARX_Krakenbattle"|"Amb_ARX_Frozen"|"Amb_Endgame_State"|"Amb_LV_State"|"Amb_Tuto_State"|"COS_OrcTemple"|"CoS_ElfTempleNuked"|"DLC_01_Amb_LV_State"|"DLC_01_Amb_WindBlender"|"GM_Theme"|"Items_Objects_MCH_Laboratory_Machines_Turbine_A"|"Menu_Themes"|"Music_Theme"|"Music_Type"|"Proj_Gren_ClusterBomb_Impact_Multi_VoiceLimitSwitch"|"Skill_NPC_VoidGlide"|"Soundvol_Arx_Sewers_DeathfogMachine_Active"|"State_Dialogue"

--- @alias SwitchGroupName "Armor_Type"|"Bear"|"Boar"|"Burning_Witch"|"Cat"|"Chicken"|"Deer"|"Dog"|"Dragon"|"Drillworm_Hatchlings"|"Elemental_Ooze"|"Items_Material"|"Items_Objects_TOOL_Ladder_Material"|"Items_SurfaceType"|"Items_Weight"|"Movement_FX_Type"|"PlayerType"|"Raanaar_Automaton"|"Race"|"Sex"|"Skill_CharacterType"|"Spider"|"Steps_Speed"|"Steps_Terrain"|"Steps_Type"|"Steps_Weight"|"Tiger"|"Troll"|"Variation"|"Vocal_Combat_Type"|"WarOwl"|"Weapon_Action"|"Weapon_Hit_Armor_Type"|"Weapon_Hit_Bloodtype"|"Weapon_Hit_Material_Type"|"Weapon_Material"|"Weapon_Race"|"Weapon_Type"|"Whoosh_Magic"|"Whoosh_Type"|"Whoosh_Weight"|"Wolf"

--- @alias ComponentHandle userdata
--- @alias EntityHandle userdata
--- @alias FixedString string
--- @alias Guid string
--- @alias IggyInvokeDataValue boolean|number|string
--- @alias NetId integer
--- @alias Path string
--- @alias PersistentRef any
--- @alias PersistentRegistryEntry any
--- @alias Ref any
--- @alias RegistryEntry any
--- @alias TemplateHandle number
--- @alias UserId integer
--- @alias UserReturn any
--- @alias int16 integer
--- @alias int32 integer
--- @alias int64 integer
--- @alias int8 integer
--- @alias uint16 integer
--- @alias uint32 integer
--- @alias uint64 integer
--- @alias uint8 integer
--- @alias Version int32[]
--- @alias ivec2 int32[]
--- @alias mat3 number[]
--- @alias mat4 number[]
--- @alias vec2 number[]
--- @alias vec3 number[]
--- @alias vec4 number[]


--- @alias AIFlags string|"IgnoreBuff"|"IgnoreControl"|"StatusIsSecondary"|"CanNotUse"|"IgnoreSelf"|"IgnoreDebuff"|"CanNotTargetFrozen"
--- @alias ActionDataType string|"Destroy"|"OpenClose"|"Teleport"|"Sticky"|"CreateSurface"|"Lockpick"|"Craft"|"Unknown"|"StoryUse"|"Lying"|"Door"|"CreatePuddle"|"UseSkill"|"Pyramid"|"StoryUseInInventoryOnly"|"SkillBook"|"Identify"|"KickstarterMessageInABottle"|"DisarmTrap"|"Recipe"|"DestroyParameters"|"Equip"|"SpawnCharacter"|"Constrain"|"Consume"|"Repair"|"Book"|"Sit"|"StoryUseInInventory"|"Ladder"|"PlaySound"|"ShowStoryElementUI"
--- @alias AiActionStep string|"CalculateItems"|"SortActions"|"ScoreActions"|"CalculateStandardAttack"|"ScoreActionsFallback"|"CalculatePositionScores"|"Init"|"ReevaluateActions"|"ScoreActionsAPSaving"|"CollectPossibleActions"|"CalculateFutureScores"|"CalculateSkills"|"ScoreActionsBehavior"
--- @alias AiActionType string|"Skill"|"Consume"|"None"|"StandardAttack"|"FallbackCommand"
--- @alias AiModifier string|"MULTIPLIER_DOT_ENEMY_POS"|"MULTIPLIER_CONTROL_NEUTRAL_POS"|"MULTIPLIER_TARGET_MY_HOSTILE"|"MULTIPLIER_TARGET_UNPREFERRED"|"SURFACE_DAMAGE_MAX_TURNS"|"MULTIPLIER_REMOVE_ARMOR"|"MULTIPLIER_BOOST_SELF_NEG"|"MULTIPLIER_ARMOR_SELF_POS"|"ENDPOS_NEARBY_DISTANCE"|"MOVESKILL_AP_DIFF_REQUIREMENT"|"MULTIPLIER_ADD_MAGIC_ARMOR"|"MULTIPLIER_AP_MAX"|"MULTIPLIER_CONTACT_BOOST"|"MULTIPLIER_FIRST_ACTION_BUFF"|"MULTIPLIER_ARMOR_NEUTRAL_NEG"|"MULTIPLIER_TARGET_MY_ENEMY"|"MULTIPLIER_ENDPOS_HEIGHT_DIFFERENCE"|"MULTIPLIER_ARMORBOOST"|"MULTIPLIER_MAGICAL_SULFUR_CURRENTLY_DAM"|"UNSTABLE_BOMB_NEARBY"|"MULTIPLIER_DOT_NEUTRAL_POS"|"MULTIPLIER_MOVEMENT_COST_MULTPLIER"|"MULTIPLIER_TARGET_AGGRO_MARKED"|"MULTIPLIER_ENDPOS_STENCH"|"MOVESKILL_ITEM_AP_DIFF_REQUIREMENT"|"MULTIPLIER_STATUS_FAILED"|"MULTIPLIER_BLIND"|"MULTIPLIER_DOT_SELF_POS"|"MULTIPLIER_BOOST_ENEMY_NEG"|"MULTIPLIER_ARMOR_SELF_NEG"|"MULTIPLIER_DEATH_RESIST"|"MULTIPLIER_SURFACE_REMOVE"|"MULTIPLIER_DESTROY_INTERESTING_ITEM"|"TURNS_REPLACEMENT_INFINITE"|"MULTIPLIER_HEAL_SELF_NEG"|"MULTIPLIER_HOT_ALLY_POS"|"MULTIPLIER_CONTROL_ENEMY_POS"|"MULTIPLIER_RESISTANCE"|"MULTIPLIER_POSITION_LEAVE"|"MULTIPLIER_DAMAGE_SELF_NEG"|"MULTIPLIER_HOT_ENEMY_POS"|"MULTIPLIER_ENDPOS_NOT_IN_AIHINT"|"MAX_SCORE_ON_NEUTRAL"|"MULTIPLIER_SHACKLES_OF_PAIN"|"MULTIPLIER_MAIN_ATTRIB"|"MULTIPLIER_ARMOR_ENEMY_NEG"|"MULTIPLIER_INCAPACITATE"|"MULTIPLIER_HEAL_ENEMY_NEG"|"MULTIPLIER_COOLDOWN_MULTIPLIER"|"MULTIPLIER_TARGET_IN_SIGHT"|"MULTIPLIER_ENDPOS_ENEMIES_NEARBY"|"MAX_HEAL_SELF_MULTIPLIER"|"MULTIPLIER_DISARMED"|"ENABLE_ACTIVE_DEFENSE_OFFENSIVE_USE"|"ENABLE_SAVING_ACTION_POINTS"|"SCORE_MOD"|"MULTIPLIER_DOT_ALLY_NEG"|"MULTIPLIER_HOT_NEUTRAL_POS"|"MULTIPLIER_ACTION_COST_MULTIPLIER"|"MULTIPLIER_GUARDIAN_ANGEL"|"MULTIPLIER_CRITICAL"|"DANGEROUS_ITEM_NEARBY"|"MULTIPLIER_SCORE_ON_NEUTRAL"|"MULTIPLIER_STATUS_CANCEL_SLEEPING"|"MULTIPLIER_FEAR"|"MULTIPLIER_AP_COSTBOOST"|"MULTIPLIER_HEAL_ALLY_NEG"|"MULTIPLIER_CONTROL_ALLY_NEG"|"MULTIPLIER_HIGH_ITEM_AMOUNT_MULTIPLIER"|"MULTIPLIER_TARGET_KNOCKED_DOWN"|"MULTIPLIER_BONUS_WEAPON_BOOST"|"MULTIPLIER_KILL_ENEMY"|"UNSTABLE_BOMB_RADIUS"|"MULTIPLIER_CONTROL_SELF_POS"|"MULTIPLIER_INVISIBLE"|"MULTIPLIER_DAMAGE_ON_MOVE"|"MULTIPLIER_DAMAGE_NEUTRAL_POS"|"MULTIPLIER_DOT_SELF_NEG"|"MULTIPLIER_RESURRECT"|"MULTIPLIER_DAMAGE_ENEMY_NEG"|"MULTIPLIER_HEAL_NEUTRAL_NEG"|"MULTIPLIER_HOT_ALLY_NEG"|"MULTIPLIER_TARGET_HOSTILE_COUNT_TWO_OR_"|"MULTIPLIER_TARGET_PREFERRED"|"MULTIPLIER_KILL_ALLY_SUMMON"|"MULTIPLIER_AP_RECOVERY"|"MULTIPLIER_POS_SECONDARY_SURFACE"|"MULTIPLIER_BOOST_NEUTRAL_NEG"|"FALLBACK_WANTED_ENEMY_DISTANCE"|"MAX_HEAL_MULTIPLIER"|"SKILL_TELEPORT_MINIMUM_DISTANCE"|"MULTIPLIER_GROUNDED"|"MULTIPLIER_ARMOR_NEUTRAL_POS"|"MULTIPLIER_STATUS_CANCEL_INVISIBILITY"|"MULTIPLIER_KNOCKDOWN"|"MULTIPLIER_DOT_ENEMY_NEG"|"MULTIPLIER_LOW_ITEM_AMOUNT_MULTIPLIER"|"MULTIPLIER_TARGET_SUMMON"|"MULTIPLIER_ENDPOS_ALLIES_NEARBY"|"MULTIPLIER_STATUS_REMOVE"|"MULTIPLIER_MUTE"|"MULTIPLIER_WINDWALKER"|"MULTIPLIER_HOT_SELF_POS"|"MULTIPLIER_BOOST_ENEMY_POS"|"MULTIPLIER_REMOVE_MAGIC_ARMOR"|"MULTIPLIER_ACC_BOOST"|"MIN_TURNS_SCORE_EXISTING_STATUS"|"MULTIPLIER_HEAL_SELF_POS"|"FALLBACK_ALLIES_NEARBY"|"MULTIPLIER_SOURCE_POINT"|"MULTIPLIER_PUDDLE_RADIUS"|"MULTIPLIER_DAMAGE_SELF_POS"|"MULTIPLIER_DAMAGE_ALLY_POS"|"MULTIPLIER_DOT_NEUTRAL_NEG"|"MULTIPLIER_CONTROL_NEUTRAL_NEG"|"MULTIPLIER_TARGET_HOSTILE_COUNT_ONE"|"MULTIPLIER_ENDPOS_FLANKED"|"MULTIPLIER_KILL_ALLY"|"MULTIPLIER_STATUS_OVERWRITE"|"MULTIPLIER_SUMMON_PATH_INFLUENCES"|"TARGET_WEAK_ALLY"|"MULTIPLIER_BOOST_ALLY_POS"|"MULTIPLIER_ARMOR_ENEMY_POS"|"MULTIPLIER_ENDPOS_NOT_IN_DANGEROUS_SURF"|"MULTIPLIER_SOURCE_MUTE"|"MULTIPLIER_MOVEMENT_BOOST"|"BUFF_DIST_MAX"|"MULTIPLIER_HEAL_ENEMY_POS"|"MULTIPLIER_DOT_ALLY_POS"|"MULTIPLIER_CONTROL_ENEMY_NEG"|"MULTIPLIER_SOURCE_COST_MULTIPLIER"|"MULTIPLIER_SCORE_OUT_OF_COMBAT"|"MULTIPLIER_SURFACE_STATUS_ON_MOVE"|"MULTIPLIER_ADD_ARMOR"|"MULTIPLIER_DEFLECT_PROJECTILES"|"BUFF_DIST_MIN"|"MULTIPLIER_HOT_ENEMY_NEG"|"MULTIPLIER_FREE_ACTION"|"MULTIPLIER_ENDPOS_NOT_IN_SMOKE"|"MULTIPLIER_DECAYING_TOUCH"|"MULTIPLIER_AP_BOOST"|"MULTIPLIER_COMBO_SCORE_POSITIONING"|"MULTIPLIER_DAMAGE_ALLY_NEG"|"MULTIPLIER_BOOST_ALLY_NEG"|"MULTIPLIER_ARMOR_ALLY_POS"|"MULTIPLIER_DAMAGEBOOST"|"MULTIPLIER_SHIELD_BLOCK"|"MULTIPLIER_HEAL_ALLY_POS"|"MULTIPLIER_CONTROL_ALLY_POS"|"MULTIPLIER_TARGET_INCAPACITATED"|"MULTIPLIER_VITALITYBOOST"|"MULTIPLIER_CHARMED"|"MULTIPLIER_REFLECT_DAMAGE"|"MULTIPLIER_HOT_NEUTRAL_NEG"|"MULTIPLIER_CANNOT_EXECUTE_THIS_TURN"|"MULTIPLIER_ACTIVE_DEFENSE"|"MULTIPLIER_SPARK"|"MULTIPLIER_HOT_SELF_NEG"|"MULTIPLIER_ENDPOS_TURNED_INVISIBLE"|"MULTIPLIER_SCORE_ON_ALLY"|"MULTIPLIER_DODGE_BOOST"|"CHARMED_MAX_CONSUMABLES_PER_TURN"|"AVENGE_ME_VITALITY_LEVEL"|"MULTIPLIER_DAMAGE_ENEMY_POS"|"MULTIPLIER_HEAL_NEUTRAL_POS"|"MULTIPLIER_INVISIBLE_MOVEMENT_COST_MULT"|"FALLBACK_ENEMIES_NEARBY"|"MULTIPLIER_KILL_ENEMY_SUMMON"|"MULTIPLIER_EXPLOSION_DISTANCE_MIN"|"MULTIPLIER_SP_COSTBOOST"|"MULTIPLIER_MAGICAL_SULFUR"|"MULTIPLIER_CONTROL_SELF_NEG"|"MULTIPLIER_BOOST_SELF_POS"|"MULTIPLIER_BOOST_NEUTRAL_POS"|"MULTIPLIER_LOSE_CONTROL"|"SKILL_JUMP_MINIMUM_DISTANCE"|"MULTIPLIER_SECONDARY_ATTRIB"|"MULTIPLIER_COMBO_SCORE_INTERACTION"|"AVENGE_ME_RADIUS"|"MULTIPLIER_DAMAGE_NEUTRAL_NEG"|"MULTIPLIER_ARMOR_ALLY_NEG"|"MULTIPLIER_HEAL_SHARING"
--- @alias AiScoreReasonFlags string|"RemoveMadnessSelf"|"BreakInvisibility"|"TooFar"|"CannotTargetFrozen"|"TargetBlocked"|"ResurrectByCharmedPlayer"|"KillSelf"|"StupidInvisibility"|"ScoreTooLow"|"ResurrectOutOfCombat"|"TooComplex"|"NoMovement"|"BreakInvisibilityForNoEnemies"|"MustStayInAiHint"|"MoveSkillCannotExecute"
--- @alias CauseType string|"StatusEnter"|"StatusTick"|"None"|"GM"|"Attack"|"Offhand"|"SurfaceMove"|"SurfaceCreate"|"SurfaceStatus"
--- @alias CraftingStationType string|"Anvil"|"Cauldron"|"Beehive"|"Misc1"|"Misc3"|"None"|"Oven"|"Wetstone"|"Misc2"|"Well"|"BoilingPot"|"SpinningWheel"|"Misc4"
--- @alias ESurfaceFlag string|"SurfaceExclude"|"ShockwaveCloud"|"IrreplaceableCloud"|"Sulfurium"|"Oil"|"Deepwater"|"CloudBlessed"|"GroundSurfaceBlock"|"CloudElectrified"|"Frozen"|"Door"|"WalkThroughBlockCharacter"|"MovementBlock"|"Dead"|"CloudSurfaceBlock"|"Cursed"|"Lava"|"Portal"|"Source"|"CloudCursed"|"CloudPurified"|"Water"|"FireCloud"|"SomeDecay"|"WaterCloud"|"Blessed"|"Portal2"|"SurfaceShapeFlood"|"BloodCloud"|"PoisonCloud"|"ElectrifiedDecay"|"Irreplaceable"|"Fire"|"SmokeCloud"|"ExplosionCloud"|"ShootThroughBlock"|"FrostCloud"|"ProjectileBlock"|"ShootThroughBlockCharacter"|"DoorNoShootThrough"|"Blood"|"Purified"|"Web"|"Electrified"|"WalkThroughBlock"|"Poison"|"Deathfog"
--- @alias EquipmentClass string|"UndeadHumanMale"|"UndeadDwarfMale"|"UndeadElfMale"|"UndeadElfFemale"|"UndeadLizardMale"|"UndeadLizardFemale"|"ElfMale"|"None"|"UndeadHumanFemale"|"HumanMale"|"HumanFemale"|"DwarfMale"|"DwarfFemale"|"ElfFemale"|"UndeadDwarfFemale"|"LizardMale"|"LizardFemale"
--- @alias ExtComponentType string|"ServerProjectile"|"ClientCharacter"|"ClientItem"|"ServerCustomStatDefinition"|"ServerCharacter"|"Max"|"ServerItem"|"Combat"
--- @alias GameActionType string|"RainAction"|"StormAction"|"TornadoAction"|"PathAction"|"GameObjectMoveAction"|"StatusDomeAction"|"WallAction"
--- @alias GameObjectTemplateFlags string|"IsCustom"
--- @alias HealEffect string|"NegativeDamage"|"Unknown9"|"HealSharing"|"Sitting"|"HealSharingReflected"|"Heal"|"Script"|"None"|"Surface"|"Necromantic"|"ResistDeath"|"Behavior"|"Unknown4"|"Lifesteal"
--- @alias IngredientTransformType string|"Boost"|"Transform"|"Consume"|"None"|"Poison"
--- @alias IngredientType string|"Property"|"Category"|"None"|"Object"
--- @alias InputModifier string|"Ctrl"|"Gui"|"Alt"|"Shift"
--- @alias InputRawType string|"scrolllock"|"slash"|"a"|"f6"|"touch_press"|"end"|"l"|"f17"|"kp_2"|"ralt"|"wheel_ypos"|"dpad_up"|"pause"|"num1"|"w"|"right2"|"wheel_yneg"|"item6"|"c"|"f8"|"leftstick_xneg"|"n"|"f19"|"kp_4"|"leftstick_ypos"|"home"|"num3"|"y"|"kp_plus"|"x2"|"leftstick_xpos"|"item8"|"tilde"|"e"|"f10"|"leftstick_yneg"|"p"|"f21"|"left"|"kp_6"|"rightstick_xneg"|"num5"|"f1"|"rightstick_ypos"|"item10"|"delete_key"|"g"|"f12"|"numlock"|"lshift"|"rightstick_xpos"|"back"|"escape"|"backspace"|"pagedown"|"equals"|"r"|"f23"|"kp_8"|"rightstick_yneg"|"item1"|"comma"|"semicolon"|"num7"|"f3"|"lefttrigger"|"leftbracket"|"i"|"f14"|"lgui"|"righttrigger"|"start"|"backslash"|"t"|"kp_0"|"mode"|"controller_a"|"item3"|"dot"|"num9"|"f5"|"up"|"controller_b"|"k"|"f16"|"kp_1"|"rshift"|"controller_x"|"num0"|"v"|"middle"|"controller_y"|"item5"|"b"|"f7"|"rightbracket"|"m"|"f18"|"kp_3"|"leftstick"|"insert"|"apostrophe"|"num2"|"x"|"x1"|"rightstick"|"item7"|"tab"|"d"|"f9"|"kp_divide"|"leftshoulder"|"o"|"f20"|"right"|"kp_multiply"|"kp_5"|"rightshoulder"|"pageup"|"num4"|"z"|"kp_minus"|"motion"|"dpad_down"|"item9"|"f"|"f11"|"kp_enter"|"lctrl"|"dpad_left"|"enter"|"q"|"f22"|"kp_7"|"kp_period"|"dpad_right"|"num6"|"f2"|"motion_xneg"|"touch_tap"|"item11"|"h"|"f13"|"lalt"|"motion_ypos"|"guide"|"touch_hold"|"s"|"f24"|"kp_9"|"rgui"|"motion_xpos"|"touch_pinch_in"|"item2"|"dash"|"num8"|"f4"|"motion_yneg"|"touch_pinch_out"|"capslock"|"j"|"f15"|"rctrl"|"wheel_xpos"|"touch_rotate"|"space"|"printscreen"|"u"|"down"|"left2"|"wheel_xneg"|"touch_flick"|"item4"
--- @alias InputState string|"Pressed"|"Released"
--- @alias InputType string|"Hold"|"Press"|"Repeat"|"ValueChange"|"ClearOnRelease"|"AcceleratedRepeat"|"Release"
--- @alias ItemDataRarity string|"Divine"|"Sentinel"|"Uncommon"|"Legendary"|"Epic"|"Rare"|"Unique"|"Common"
--- @alias LuaTypeId string|"Integer"|"Module"|"Boolean"|"Enumeration"|"Unknown"|"Set"|"Nullable"|"Any"|"Tuple"|"Float"|"Void"|"Array"|"Function"|"String"|"Map"|"Object"
--- @alias MaterialInfoFlags string|"Force"|"ApplyCurrentNormalMap"|"IsOverlay"|"IsActive"
--- @alias MultiEffectHandlerFlags string|"FaceSource"|"Beam"|"FollowScale"|"EffectAttached"|"Detach"|"KeepRot"
--- @alias NetMessage string|"NETMSG_CHARACTER_STEERING"|"NETMSG_CHARACTER_CORPSE_LOOTABLE"|"NETMSG_PARTYORDER"|"NETMSG_INVENTORY_DESTROY"|"NETMSG_EFFECT_CREATE"|"NETMSG_NOTIFICATION"|"NETMSG_DIALOG_ACTORLEAVES_MESSAGE"|"NETMSG_GM_REQUEST_CAMPAIGN_DATA"|"NETMSG_SHOW_TUTORIAL_MESSAGE"|"NETMSG_LOBBY_CHARACTER_SELECT"|"NETMSG_SCRIPT_EXTENDER"|"NETMSG_CLIENT_ACCEPT"|"NETMSG_CHARACTER_OFFSTAGE"|"NETMSG_ITEM_DEACTIVATE"|"NETMSG_TURNBASED_SETTEAM"|"NETMSG_CHARACTER_ERROR"|"NETMSG_DIALOG_ANSWER_MESSAGE"|"NETMSG_GM_TOGGLE_PAUSE"|"NETMSG_GM_SET_INTERESTED_CHARACTER"|"NETMSG_GM_SET_STATUS"|"NETMSG_GM_INVENTORY_OPERATION"|"NETMSG_CAMERA_TARGET"|"NETMSG_SESSION_LOAD"|"NETMSG_LEVEL_SWAP_READY"|"NETMSG_CHARACTER_CHANGE_OWNERSHIP"|"NETMSG_VOICEDATA"|"NETMSG_UPDATE_COMBAT_GROUP_INFO"|"NETMSG_ITEM_MOVE_UUID"|"NETMSG_SCREEN_FADE"|"NETMSG_GM_TELEPORT"|"NETMSG_GM_SYNC_OVERVIEW_MAPS"|"NETMSG_GM_SYNC_NOTES"|"NETMSG_GM_REQUEST_CHARACTERS_REROLL"|"NETMSG_PARTY_NPC_DATA"|"NETMSG_DIFFICULTY_CHANGED"|"NETMSG_LOCK_WAYPOINT"|"NETMSG_CHARACTER_ACTION"|"NETMSG_CHARACTER_CONTROL"|"NETMSG_INVENTORY_VIEW_DESTROY"|"NETMSG_TURNBASED_ORDER"|"NETMSG_TROPHY_UPDATE"|"NETMSG_GM_SET_ATMOSPHERE"|"NETMSG_LOBBY_DATAUPDATE"|"NETMSG_CHARACTER_LOOT_CORPSE"|"NETMSG_PARTY_CREATE"|"NETMSG_NET_ENTITY_DESTROY"|"NETMSG_SKILL_UNLEARN"|"NETMSG_OPEN_KICKSTARTER_BOOK_UI_MESSAGE"|"NETMSG_MYSTERY_UPDATE"|"NETMSG_GM_TOGGLE_OVERVIEWMAP"|"NETMSG_ATMOSPHERE_OVERRIDE"|"NETMSG_SESSION_LOADED"|"NETMSG_AITEST_UPDATE"|"NETMSG_STORY_VERSION"|"NETMSG_HOST_REFUSE"|"NETMSG_PLAYER_DISCONNECT"|"NETMSG_SECRET_REGION_UNLOCK"|"NETMSG_ITEM_CREATE"|"NETMSG_PLAYSOUND"|"NETMSG_GM_DRAW_SURFACE"|"NETMSG_GM_TOGGLE_PEACE"|"NETMSG_PLAYER_ACCEPT"|"NETMSG_CHARACTER_SKILLBAR"|"NETMSG_CHARACTER_SELECTEDSKILLSET"|"NETMSG_ITEM_STATUS"|"NETMSG_CACHETEMPLATE"|"NETMSG_GM_TICK_ROLLS"|"NETMSG_GM_CHANGE_SCENE_NAME"|"NETMSG_GAMETIME_SYNC"|"NETMSG_ITEM_ENGRAVE"|"NETMSG_CUSTOM_STATS_CREATE"|"NETMSG_GAMEACTION"|"NETMSG_TURNBASED_START"|"NETMSG_GAMECONTROL_UPDATE_C2S"|"NETMSG_QUEST_UPDATE"|"NETMSG_GM_REMOVE_EXPORTED"|"NETMSG_GM_ASSETS_PENDING_SYNCS_INFO"|"NETMSG_GM_LOAD_CAMPAIGN"|"NETMSG_SAVEGAME_LOAD_FAIL"|"NETMSG_DIALOG_LISTEN"|"NETMSG_READYCHECK"|"NETMSG_CHARACTER_CONFIRMATION"|"NETMSG_CHARACTER_USE_AP"|"NETMSG_ITEM_MOVE_TO_WORLD"|"NETMSG_SKILL_DESTROY"|"NETMSG_OPEN_CUSTOM_BOOK_UI_MESSAGE"|"NETMSG_QUEST_POSTPONE"|"NETMSG_REGISTER_WAYPOINT"|"NETMSG_TELEPORT_WAYPOINT"|"NETMSG_LOBBY_SURRENDER"|"NETMSG_PING_BEACON"|"NETMSG_CHARACTER_STATS_UPDATE"|"NETMSG_PARTYUPDATE"|"NETMSG_INVENTORY_LOCKSTATE_SYNC"|"NETMSG_EFFECT_FORGET"|"NETMSG_FLAG_UPDATE"|"NETMSG_GM_EDIT_CHARACTER"|"NETMSG_LOAD_GAME_WITH_ADDONS"|"NETMSG_CLIENT_JOINED"|"NETMSG_ITEM_DESTINATION"|"NETMSG_ALIGNMENT_CREATE"|"NETMSG_ALIGNMENT_RELATION"|"NETMSG_CLOSE_CUSTOM_BOOK_UI_MESSAGE"|"NETMSG_GM_REQUEST_ROLL"|"NETMSG_GM_VIGNETTE_ANSWER"|"NETMSG_GM_REMOVE_STATUS"|"NETMSG_CAMERA_MODE"|"NETMSG_LEVEL_INSTANTIATE_SWAP"|"NETMSG_LEVEL_START"|"NETMSG_OPEN_CRAFT_UI_MESSAGE"|"NETMSG_CAMERA_SPLINE"|"NETMSG_SKIPMOVIE_RESULT"|"NETMSG_CHARACTER_DEACTIVATE"|"NETMSG_ITEM_STATUS_LIFETIME"|"NETMSG_INVENTORY_CREATE"|"NETMSG_SCREEN_FADE_DONE"|"NETMSG_CLOSED_MESSAGE_BOX_MESSAGE"|"NETMSG_PARTY_UNLOCKED_RECIPE"|"NETMSG_GM_CAMPAIGN_SAVE"|"NETMSG_GM_REORDER_ELEMENTS"|"NETMSG_GM_MAKE_FOLLOWER"|"NETMSG_REQUESTAUTOSAVE"|"NETMSG_CRAFT_RESULT"|"NETMSG_LOBBY_SPECTATORUPDATE"|"NETMSG_MULTIPLE_TARGET_OPERATION"|"NETMSG_CHARACTER_STATUS"|"NETMSG_CHARACTER_STATUS_LIFETIME"|"NETMSG_CHARACTER_TRANSFORM"|"NETMSG_PEER_ACTIVATE"|"NETMSG_MARKER_UI_CREATE"|"NETMSG_MODULE_LOAD"|"NETMSG_CHARACTERCREATION_NOT_READY"|"NETMSG_LOBBY_USERUPDATE"|"NETMSG_TRIGGER_CREATE"|"NETMSG_COMBAT_TURN_TIMER"|"NETMSG_PARTY_DESTROY"|"NETMSG_INVENTORY_VIEW_UPDATE_PARENTS"|"NETMSG_SKILL_ACTIVATE"|"NETMSG_OPEN_MESSAGE_BOX_MESSAGE"|"NETMSG_PARTYFORMATION"|"NETMSG_LEVEL_LOAD"|"NETMSG_CHARACTERCREATION_DONE"|"NETMSG_DLC_UPDATE"|"NETMSG_HOST_LEFT"|"NETMSG_CHARACTER_POSITION_SYNC"|"NETMSG_ITEM_DESTROY"|"NETMSG_ITEM_MOVED_INFORM"|"NETMSG_PLAYMOVIE"|"NETMSG_PARTY_MERGE_NOTIFICATION"|"NETMSG_GM_DAMAGE"|"NETMSG_GM_CONFIGURE_CAMPAIGN"|"NETMSG_GM_MAKE_TRADER"|"NETMSG_MODULE_LOADED"|"NETMSG_LEVEL_LOADED"|"NETMSG_PLAYER_JOINED"|"NETMSG_CHARACTER_PICKPOCKET"|"NETMSG_ITEM_TRANSFORM"|"NETMSG_TURNBASED_SUMMONS"|"NETMSG_OVERHEADTEXT"|"NETMSG_DIALOG_NODE_MESSAGE"|"NETMSG_GM_CHANGE_LEVEL"|"NETMSG_GM_UI_OPEN_STICKY"|"NETMSG_STORY_ELEMENT_UI"|"NETMSG_ACHIEVEMENT_PROGRESS_MESSAGE"|"NETMSG_HACK_TELL_BUILDNAME"|"NETMSG_CHARACTER_CREATE"|"NETMSG_TURNBASED_STOP"|"NETMSG_QUEST_CATEGORY_UPDATE"|"NETMSG_QUEST_STATE"|"NETMSG_GM_STOP_TRAVELING"|"NETMSG_GM_POSSESS"|"NETMSG_CHANGE_COMBAT_FORMATION"|"NETMSG_MUSIC_STATE"|"NETMSG_DIPLOMACY"|"NETMSG_CUSTOM_STATS_DEFINITION_UPDATE"|"NETMSG_HOST_REFUSEPLAYER"|"NETMSG_CHARACTER_ACTION_DATA"|"NETMSG_CHARACTER_AOO"|"NETMSG_INVENTORY_VIEW_CREATE"|"NETMSG_SKILL_UPDATE"|"NETMSG_UI_QUESTSELECTED"|"NETMSG_GM_CLEAR_STATUSES"|"NETMSG_LOBBY_RETURN"|"NETMSG_UPDATE_CHARACTER_TAGS"|"NETMSG_GIVE_REWARD"|"NETMSG_CHARACTER_ACTION_REQUEST_RESULT"|"NETMSG_CHARACTER_LOCK_ABILITY"|"NETMSG_PARTYUSER"|"NETMSG_NET_ENTITY_CREATE"|"NETMSG_EFFECT_DESTROY"|"NETMSG_DIALOG_HISTORY_MESSAGE"|"NETMSG_DIALOG_ACTORJOINS_MESSAGE"|"NETMSG_GM_DELETE"|"NETMSG_GM_DUPLICATE"|"NETMSG_GM_SET_REPUTATION"|"NETMSG_CHARACTER_ANIMATION_SET_UPDATE"|"NETMSG_CLIENT_LEFT"|"NETMSG_SHROUD_FRUSTUM_UPDATE"|"NETMSG_ITEM_UPDATE"|"NETMSG_INVENTORY_VIEW_UPDATE_ITEMS"|"NETMSG_ALIGNMENT_SET"|"NETMSG_GAMEOVER"|"NETMSG_GM_PASS_ROLL"|"NETMSG_GM_SYNC_VIGNETTES"|"NETMSG_GM_CREATE_ITEM"|"NETMSG_RUNECRAFT"|"NETMSG_JOURNALDIALOGLOG_UPDATE"|"NETMSG_SURFACE_CREATE"|"NETMSG_CHARACTER_SET_STORY_NAME"|"NETMSG_INVENTORY_VIEW_SORT"|"NETMSG_EGG_CREATE"|"NETMSG_TURNBASED_FLEECOMBATRESULT"|"NETMSG_CLOSE_UI_MESSAGE"|"NETMSG_GM_SPAWN"|"NETMSG_GM_SYNC_SCENES"|"NETMSG_GM_SOUND_PLAYBACK"|"NETMSG_GM_DEACTIVATE"|"NETMSG_UNLOCK_ITEM"|"NETMSG_SET_CHARACTER_ARCHETYPE"|"NETMSG_CHARACTER_UPDATE"|"NETMSG_CHARACTER_ITEM_USED"|"NETMSG_PEER_DEACTIVATE"|"NETMSG_GAMECONTROL_UPDATE_S2C"|"NETMSG_MARKER_UI_UPDATE"|"NETMSG_GM_TOGGLE_VIGNETTE"|"NETMSG_GM_HEAL"|"NETMSG_SAVEGAMEHANDSHAKE"|"NETMSG_NOTIFY_COMBINE_FAILED_MESSAGE"|"NETMSG_LOBBY_COMMAND"|"NETMSG_TRIGGER_DESTROY"|"NETMSG_CHARACTER_DESTROY"|"NETMSG_PARTYGROUP"|"NETMSG_ITEM_USE_REMOTELY"|"NETMSG_SKILL_REMOVED"|"NETMSG_JOURNALRECIPE_UPDATE"|"NETMSG_STOP_FOLLOW"|"NETMSG_GM_ROLL"|"NETMSG_CUSTOM_STATS_DEFINITION_REMOVE"|"NETMSG_MUTATORS_ENABLED"|"NETMSG_CLIENT_CONNECT"|"NETMSG_CHARACTER_UPGRADE"|"NETMSG_ITEM_ACTIVATE"|"NETMSG_INVENTORY_ITEM_UPDATE"|"NETMSG_PLAY_HUD_SOUND"|"NETMSG_GM_EXPORT"|"NETMSG_GM_HOST"|"NETMSG_GM_GIVE_REWARD"|"NETMSG_GM_CHANGE_SCENE_PATH"|"NETMSG_CAMERA_ROTATE"|"NETMSG_MODULES_DOWNLOAD"|"NETMSG_LOAD_START"|"NETMSG_CUSTOM_STATS_UPDATE"|"NETMSG_LOAD_GAME_WITH_ADDONS_FAIL"|"NETMSG_PLAYER_LEFT"|"NETMSG_ITEM_OFFSTAGE"|"NETMSG_GAMECONTROL_PRICETAG"|"NETMSG_COMBATLOG"|"NETMSG_DIALOG_ANSWER_HIGHLIGHT_MESSAGE"|"NETMSG_GM_ADD_EXPERIENCE"|"NETMSG_GM_POSITION_SYNC"|"NETMSG_GM_EDIT_ITEM"|"NETMSG_CHAT"|"NETMSG_SERVER_NOTIFICATION"|"NETMSG_UNLOCK_WAYPOINT"|"NETMSG_TELEPORT_ACK"|"NETMSG_CHARACTER_ACTIVATE"|"NETMSG_CHARACTER_ASSIGN"|"NETMSG_ITEM_MOVE_TO_INVENTORY"|"NETMSG_INVENTORY_CREATE_AND_OPEN"|"NETMSG_TURNBASED_ROUND"|"NETMSG_SECRET_UPDATE"|"NETMSG_PARTY_CONSUMED_ITEMS"|"NETMSG_GM_CHANGE_NAME"|"NETMSG_SAVEGAME"|"NETMSG_REALTIME_MULTIPLAY"|"NETMSG_MUSIC_EVENT"|"NETMSG_CHARACTER_BOOST"|"NETMSG_CHARACTER_CUSTOMIZATION"|"NETMSG_PROJECTILE_CREATE"|"NETMSG_SKILL_LEARN"|"NETMSG_MYSTERY_STATE"|"NETMSG_UPDATE_ITEM_TAGS"|"NETMSG_ARENA_RESULTS"|"NETMSG_PAUSE"|"NETMSG_SHOW_ERROR"|"NETMSG_CLIENT_GAME_SETTINGS"|"NETMSG_HOST_WELCOME"|"NETMSG_CHARACTER_IN_DIALOG"|"NETMSG_SHROUD_UPDATE"|"NETMSG_FORCE_SHEATH"|"NETMSG_COMBATLOGITEMINTERACTION"|"NETMSG_GM_ITEM_CHANGE"|"NETMSG_GM_TRAVEL_TO_DESTINATION"|"NETMSG_GM_ITEM_USE"|"NETMSG_CAMERA_ACTIVATE"|"NETMSG_LEVEL_SWAP_COMPLETE"|"NETMSG_ACHIEVEMENT_UNLOCKED_MESSAGE"|"NETMSG_DISCOVERED_PORTALS"|"NETMSG_UNPAUSE"|"NETMSG_PLAYER_CONNECT"|"NETMSG_CHARACTER_POSITION"|"NETMSG_ITEM_ACTION"|"NETMSG_ITEM_CONFIRMATION"|"NETMSG_ALIGNMENT_CLEAR"|"NETMSG_PARTY_SPLIT_NOTIFICATION"|"NETMSG_GM_REMOVE_ROLL"|"NETMSG_GM_SET_START_POINT"|"NETMSG_SERVER_COMMAND"|"NETMSG_CUSTOM_STATS_DEFINITION_CREATE"|"NETMSG_SURFACE_META"|"NETMSG_CHARACTER_TELEPORT"|"NETMSG_PROJECTILE_EXPLOSION"|"NETMSG_EGG_DESTROY"|"NETMSG_JOURNAL_RESET"|"NETMSG_DIALOG_STATE_MESSAGE"|"NETMSG_GM_SYNC_ASSETS"|"NETMSG_TRADE_ACTION"|"NETMSG_TELEPORT_PYRAMID"|"NETMSG_CHARACTER_COMPANION_CUSTOMIZATION"|"NETMSG_GM_JOURNAL_UPDATE"|"NETMSG_CHARACTER_DIALOG"|"NETMSG_COMBAT_COMPONENT_SYNC"|"NETMSG_SKILL_CREATE"|"NETMSG_OPEN_WAYPOINT_UI_MESSAGE"|"NETMSG_SHOW_ENTER_REGION_UI_MESSAGE"|"NETMSG_QUEST_TRACK"|"NETMSG_CHARACTERCREATION_READY"|"NETMSG_LOBBY_STARTGAME"|"NETMSG_PARTYCREATEGROUP"|"NETMSG_TRIGGER_UPDATE"
--- @alias ObjectHandleType string|"Scene"|"ClientDummyGameObject"|"BlendState"|"ServerRegionTrigger"|"DepthState"|"UIObject"|"CustomStatsComponent"|"ServerSurfaceAction"|"RasterizerState"|"TerrainObject"|"ClientGameAction"|"Constant"|"GrannyFile"|"ClientPointSoundTriggerDummy"|"ClientSpectatorTrigger"|"StructuredBuffer"|"ConstantBuffer"|"GMJournalNode"|"ServerProjectile"|"CompiledShader"|"ExtenderClientVisual"|"Unknown"|"ClientEgg"|"TexturedFont"|"ClientCharacter"|"ClientWallIntersection"|"ServerMusicVolumeTrigger"|"ClientItem"|"ClientCameraLockTrigger"|"Effect"|"ContainerElementComponent"|"ClientScenery"|"ClientWallBase"|"ServerStartTrigger"|"ContainerComponent"|"ClientInventory"|"ServerEocPointTrigger"|"ServerCrimeAreaTrigger"|"ClientAlignment"|"ClientOverviewMap"|"SRV"|"ClientSkill"|"ClientAtmosphereTrigger"|"ServerExplorationTrigger"|"Shader"|"Dummy"|"ClientStatus"|"ServerInventoryView"|"Texture"|"ClientParty"|"Visual"|"ClientInventoryView"|"ClientVignette"|"ClientNote"|"ClientRegionTrigger"|"ClientSurface"|"Light"|"ServerEgg"|"ServerTeleportTrigger"|"ClientProjectile"|"ServerCharacter"|"Reference"|"ClientPointTrigger"|"ServerCrimeRegionTrigger"|"ServerSoundVolumeTrigger"|"SoundVolumeTrigger"|"ClientAiSeederTrigger"|"ServerItem"|"ServerInventory"|"ServerStatsAreaTrigger"|"ServerCustomStatDefinitionComponent"|"ServerParty"|"ServerOverviewMap"|"ClientAlignmentData"|"ServerVignette"|"Decal"|"Trigger"|"ClientSecretRegionTrigger"|"ClientPointSoundTrigger"|"ServerNote"|"TextureRemoveData"|"CombatComponent"|"ServerEocAreaTrigger"|"ServerAtmosphereTrigger"|"ServerAIHintAreaTrigger"|"IndexBuffer"|"MeshProxy"|"ServerEventTrigger"|"VertexBuffer"|"ClientWallConstruction"|"ClientSoundVolumeTrigger"|"ServerSecretRegionTrigger"|"VertexFormat"|"Text3D"|"ClientCustomStatDefinitionComponent"|"ClientCullTrigger"|"SamplerState"
--- @alias PathRootType string|"Root"|"Public"|"MyDocuments"|"GameStorage"|"Data"
--- @alias PlayerUpgradeAttribute string|"Finesse"|"Strength"|"Intelligence"|"Constitution"|"Wits"|"Memory"
--- @alias RecipeCategory string|"Arrows"|"Runes"|"Armour"|"Potions"|"Weapons"|"Grenades"|"Objects"|"Grimoire"|"Food"|"Common"
--- @alias ResourceType string|"Material"|"Sound"|"VisualSet"|"AnimationBlueprint"|"Atmosphere"|"Physics"|"Effect"|"Texture"|"Script"|"Visual"|"MaterialSet"|"Animation"|"MeshProxy"|"AnimationSet"
--- @alias ScriptCheckType string|"Variable"|"Operator"
--- @alias ScriptOperatorType string|"Or"|"None"|"And"|"Not"
--- @alias ShroudType string|"Sight"|"Sneak"|"RegionMask"|"Shroud"
--- @alias SkillType string|"Dome"|"Path"|"Zone"|"SkillHeal"|"Wall"|"ProjectileStrike"|"Quake"|"Jump"|"Shout"|"Tornado"|"Rush"|"MultiStrike"|"Teleportation"|"Target"|"Summon"|"Projectile"|"Storm"|"Rain"
--- @alias SoundComponentFlags string|"IsTemporary"|"IsTemporary2"|"AlwaysInactive"|"AlwaysActive"|"Positional"|"AlwaysInActiveVolume"
--- @alias StatAttributeFlags string|"MuteImmunity"|"EntangledContact"|"ChilledImmunity"|"DisarmedImmunity"|"WarmImmunity"|"KnockdownImmunity"|"WetImmunity"|"BlindImmunity"|"SuffocatingImmunity"|"CursedImmunity"|"WeakImmunity"|"SlowedImmunity"|"PetrifiedImmunity"|"DrunkImmunity"|"FreezeContact"|"ShacklesOfPainImmunity"|"BurnContact"|"DecayingImmunity"|"StunImmunity"|"StunContact"|"PoisonImmunity"|"BleedingImmunity"|"PoisonContact"|"Arrow"|"ProtectFromSummon"|"InfectiousDiseasedImmunity"|"ChillContact"|"Unbreakable"|"FreezeImmunity"|"Unrepairable"|"Floating"|"BurnImmunity"|"Torch"|"Unstorable"|"LoseDurabilityOnCharacterHit"|"SlippingImmunity"|"Grounded"|"HastedImmunity"|"InvisibilityImmunity"|"TauntedImmunity"|"ClairvoyantImmunity"|"AcidImmunity"|"CrippledImmunity"|"EnragedImmunity"|"DeflectProjectiles"|"BlessedImmunity"|"RegeneratingImmunity"|"IgnoreClouds"|"MadnessImmunity"|"ChickenImmunity"|"SleepingImmunity"|"IgnoreCursedOil"|"ShockedImmunity"|"CharmImmunity"|"WebImmunity"|"MagicalSulfur"|"FearImmunity"|"DiseasedImmunity"|"LootableWhenEquipped"|"PickpocketableWhenEquipped"|"ThrownImmunity"
--- @alias StatusHealType string|"Vitality"|"MagicArmor"|"AllArmor"|"None"|"Source"|"All"|"PhysicalArmor"
--- @alias StatusMaterialApplyFlags string|"ApplyOnBody"|"ApplyOnArmor"|"ApplyOnWeapon"|"ApplyOnWings"|"ApplyOnHorns"|"ApplyOnOverhead"
--- @alias StatusType string|"INVISIBLE"|"ROTATE"|"INFECTIOUS_DISEASED"|"ENCUMBERED"|"FEAR"|"MATERIAL"|"MUTED"|"IDENTIFY"|"LEADERSHIP"|"FLANKED"|"ADRENALINE"|"LYING"|"WIND_WALKER"|"DARK_AVENGER"|"SOURCE_MUTED"|"HEAL_SHARING_CASTER"|"HEAL"|"KNOCKED_DOWN"|"CLEAN"|"SUMMONING"|"FLOATING"|"STORY_FROZEN"|"UNLOCK"|"EXTRA_TURN"|"SHACKLES_OF_PAIN"|"ACTIVE_DEFENSE"|"SNEAKING"|"CHALLENGE"|"SITTING"|"DECAYING_TOUCH"|"DAMAGE_ON_MOVE"|"SPARK"|"THROWN"|"UNHEALABLE"|"SPIRIT"|"DEMONIC_BARGAIN"|"DYING"|"SMELLY"|"CHANNELING"|"GUARDIAN_ANGEL"|"SPIRIT_VISION"|"COMBAT"|"UNSHEATHED"|"FORCE_MOVE"|"REMORSE"|"CLIMBING"|"DISARMED"|"INCAPACITATED"|"PLAY_DEAD"|"STANCE"|"SHACKLES_OF_PAIN_CASTER"|"INSURFACE"|"CONSTRAINED"|"HEALING"|"INFUSED"|"HIT"|"BLIND"|"DEACTIVATED"|"EFFECT"|"TUTORIAL_BED"|"TELEPORT_FALLING"|"CONSUME"|"OVERPOWER"|"EXPLODE"|"COMBUSTION"|"REPAIR"|"POLYMORPHED"|"BOOST"|"HEAL_SHARING"|"CHARMED"|"DRAIN"|"DAMAGE"|"LINGERING_WOUNDS"|"AOO"
--- @alias SurfaceActionType string|"ZoneAction"|"ExtinguishFireAction"|"PolygonSurfaceAction"|"TransformSurfaceAction"|"CreateSurfaceAction"|"SwapSurfaceAction"|"ChangeSurfaceOnPathAction"|"RectangleSurfaceAction"|"CreatePuddleAction"
--- @alias SurfaceLayer string|"Ground"|"None"|"Cloud"
--- @alias SurfaceTransformActionType string|"Curse"|"Shatter"|"Purify"|"Oilify"|"Bless"|"Electrify"|"Condense"|"None"|"Vaporize"|"Freeze"|"Bloodify"|"Contaminate"|"Ignite"|"Melt"
--- @alias SurfaceType string|"ShockwaveCloud"|"FireBlessed"|"WaterElectrifiedCursed"|"BloodCloudPurified"|"Sentinel"|"FireCursed"|"BloodFrozenCursed"|"Oil"|"Deepwater"|"FirePurified"|"BloodCloudElectrifiedPurified"|"WaterFrozen"|"WaterElectrified"|"WaterBlessed"|"WaterCursed"|"BloodElectrifiedBlessed"|"WaterPurified"|"WaterCloudBlessed"|"BloodFrozen"|"FireCloudPurified"|"WaterCloudElectrifiedBlessed"|"BloodCloudElectrified"|"BloodBlessed"|"WaterFrozenCursed"|"BloodCursed"|"PoisonCloudBlessed"|"BloodPurified"|"BloodFrozenPurified"|"WaterCloudElectrifiedPurified"|"PoisonBlessed"|"PoisonCursed"|"WaterElectrifiedBlessed"|"PoisonPurified"|"BloodCloudElectrifiedBlessed"|"BloodFrozenBlessed"|"OilBlessed"|"Lava"|"OilCursed"|"Source"|"WaterCloudCursed"|"OilPurified"|"BloodCloudElectrifiedCursed"|"WebBlessed"|"Water"|"WaterFrozenPurified"|"WebCursed"|"FireCloud"|"PoisonCloudCursed"|"WebPurified"|"WaterCloud"|"SmokeCloudBlessed"|"SmokeCloudCursed"|"BloodCloud"|"CustomBlessed"|"BloodCloudBlessed"|"BloodCloudCursed"|"PoisonCloud"|"Fire"|"WaterFrozenBlessed"|"BloodElectrifiedPurified"|"CustomCursed"|"SmokeCloud"|"BloodElectrifiedCursed"|"CustomPurified"|"ExplosionCloud"|"WaterElectrifiedPurified"|"WaterCloudElectrifiedCursed"|"SmokeCloudPurified"|"FrostCloud"|"Blood"|"Web"|"FireCloudCursed"|"BloodElectrified"|"Poison"|"PoisonCloudPurified"|"Custom"|"FireCloudBlessed"|"WaterCloudElectrified"|"WaterCloudPurified"|"Deathfog"
--- @alias TemplateType string|"RootTemplate"|"LocalTemplate"|"LevelCacheTemplate"|"GlobalCacheTemplate"|"GlobalTemplate"
--- @alias TriggerTypeId string|"CrimeRegionTrigger"|"StartTrigger"|"TeleportTrigger"|"AtmosphereTrigger"|"PointSoundTrigger"|"StatsAreaTrigger"|"CameraLockTrigger"|"SecretRegionTrigger"|"ExplorationTrigger"|"MusicVolumeTrigger"|"SoundVolumeTrigger"|"CrimeAreaTrigger"|"PointTrigger"|"RegionTrigger"|"AISeederTrigger"|"PointSoundTriggerDummy"|"AreaTrigger"|"SpectatorTrigger"|"CullTrigger"|"AIHintAreaTrigger"|"EventTrigger"
--- @alias UIObjectFlags string|"OF_PlayerModal1"|"OF_PlayerModal2"|"OF_PlayerTextInput4"|"OF_PreventCameraMove"|"OF_Load"|"OF_PlayerModal3"|"OF_PlayerModal4"|"OF_KeepInScreen"|"OF_PlayerTextInput1"|"OF_PauseRequest"|"OF_PrecacheUIData"|"OF_SortOnAdd"|"OF_FullScreen"|"OF_RequestDelete"|"OF_DontHideOnDelete"|"OF_PlayerTextInput2"|"OF_KeepCustomInScreen"|"OF_Loaded"|"OF_Visible"|"OF_PlayerTextInput3"|"OF_Activated"|"OF_PlayerInput1"|"OF_DeleteOnChildDestroy"|"OF_PlayerInput2"|"OF_PlayerInput3"|"OF_PlayerInput4"
--- @alias VisualAttachmentFlags string|"UseLocalTransform"|"Weapon"|"KeepScale"|"InheritAnimations"|"DoNotUpdate"|"Horns"|"Overhead"|"WeaponFX"|"BonusWeaponFX"|"WeaponOverlayFX"|"DestroyWithParent"|"ParticleSystem"|"ExcludeFromBounds"|"Armor"|"Wings"|"KeepRot"
--- @alias VisualComponentFlags string|"ForceUseAnimationBlueprint"|"VisualSetLoaded"
--- @alias VisualFlags string|"CastShadow"|"ReceiveDecal"|"IsShadowProxy"|"Reflecting"|"AllowReceiveDecalWhenAnimated"
--- @alias VisualTemplateColorIndex string|"Hair"|"Cloth"|"Skin"
--- @alias VisualTemplateVisualIndex string|"Visual8"|"Head"|"Beard"|"Visual9"|"Torso"|"HairHelmet"|"Trousers"|"Boots"|"Arms"
--- @alias EclCharacterFlags string|"VisionGridInvisible2"|"DisableSneaking"|"MovementUpdated"|"GMControl"|"HasCustomVisualIndices"|"InArena"|"NoRotate"|"HasSkillTargetEffect"|"IsPlayer"|"Activated"|"Dead"|"CharCreationInProgress"|"OffStage"|"Multiplayer"|"CanShootThrough"|"CharacterCreationFinished"|"StoryNPC"|"HasOwner"|"InDialog"|"Floating"|"CannotMove"|"WalkThrough"|"Global"|"SpotSneakers"|"Invisible"|"PartyFollower"|"UseOverlayMaterials"|"IsTrading"|"IsHuge"|"InParty"|"NoCover"|"Summon"|"CannotFight"|"Detached"|"InCombat"|"VisualsUpdated"|"IsRunning"|"ReservedForDialog"|"WeaponSheathed"|"HasInventory"
--- @alias EclCharacterFlags2 string|"HasWalkSpeedOverride"|"IsTrader"|"TransformMoveAttachedEffects"|"HasRunSpeedOverride"|"IsGameMaster"|"IsPossessed"|"NoReputationEffects"
--- @alias EclCharacterFlags3 string|"LootCorpseRequested"|"Pickpocketing"|"Spectating"|"HasCustomVisualSet"|"UnknownDialogFlag"|"IsArenaParticipant"|"RefreshVisualsRequested"|"PickpocketRequested"
--- @alias EclCharacterSoundObjectType string|"General"|"MainHand"|"OffHand"
--- @alias EclCharacterTaskFlags string|"IgnorePreviewCancel"|"OverrideCursorOnHover"|"NeedsPreviewToActivate"|"CannotActivateSameType"|"NeedsTaskToActivate"|"IsExecuting"|"IgnoreAP"|"CannotPreviewSameType"|"SupportsPicking"|"CombatTargeting"
--- @alias EclCursorType string|"CursorBackstab_Warning"|"CursorItemPickup_Warning"|"Sentinel"|"CursorLock_Warning"|"CursorShovel_Warning"|"CursorPickPocket_Warning"|"CursorBow_Ground"|"CursorItemMove_Warning"|"CursorOpenContainer_New"|"CursorPickPocket"|"CursorCast_Warning"|"CursorMelee_Warning"|"CursorSystem"|"CursorTalk_Warning"|"CursorArrow"|"CursorBow"|"CursorWand_Warning"|"CursorWand"|"CursorItemUse_Warning"|"CursorBackstab"|"CursorOpenDoor_Warning"|"CursorCast"|"CursorListen_Warning"|"CursorArrow_Warning"|"CursorCombine"|"CursorOpenContainer_Warning"|"CursorCombine_Warning"|"CursorIdentify"|"CursorMelee_Ground"|"CursorItemMove"|"CursorWalk_Warning"|"CursorItemUse"|"CursorWand_Ground"|"CursorLock"|"CursorItemPickup"|"CursorMelee"|"CursorOpenDoor"|"CursorRepair_Warning"|"CursorRepair"|"CursorBow_Warning"|"CursorShovel"|"CursorIdentify_Warning"|"CursorTalk"|"CursorOpenContainer"|"CursorWalk"|"CursorCameraRotation"|"CursorListen"
--- @alias EclDeactivateEquipmentSlotMask string|"DeactivateGloves"|"DeactivateRing"|"DeactivateBelt"|"DeactivateBoots"|"DeactivateLeggings"|"DeactivateRing2"|"DeactivateWings"|"DeactivateHorns"|"DeactivateAmulet"|"DeactivateWeapon"|"DeactivateHelmet"|"DeactivateOverhead"|"DeactivateShield"|"DeactivateBreast"
--- @alias EclDeactivateVisualSetSlotMask string|"DeactivateBootsVisual"|"DeactivateHair"|"DeactivateHead"|"DeactivateTorso"|"DeactivateArms"|"DeactivateBeard"|"DeactivateTrousers"
--- @alias EclEntityComponentIndex string|"Sound"|"AnimationBlueprint"|"CustomStats"|"Character"|"AtmosphereTrigger"|"GMJournalNode"|"PointSoundTrigger"|"Scenery"|"GameMaster"|"CameraLockTrigger"|"Effect"|"EquipmentVisualsComponent"|"Egg"|"LightProbe"|"SecretRegionTrigger"|"ParentEntity"|"AiSeederTrigger"|"Visual"|"OverviewMap"|"PingBeacon"|"Vignette"|"Net"|"Spline"|"Light"|"ContainerElement"|"Container"|"SoundVolumeTrigger"|"PointTrigger"|"Note"|"CustomStatDefinition"|"Item"|"Decal"|"RegionTrigger"|"Combat"|"Projectile"|"PointSoundTriggerDummy"|"SpectatorTrigger"|"PublishingRequest"|"CullTrigger"
--- @alias EclEntitySystemIndex string|"LightManager"|"DecalManager"|"PublishingSystem"|"SoundSystem"|"PingBeaconManager"|"GrannySystem"|"GameActionManager"|"GameMasterManager"|"TurnManager"|"SceneryManager"|"TriggerManager"|"SeeThroughManager"|"GameMasterCampaignManager"|"ItemManager"|"EggManager"|"ContainerElementComponent"|"PhysXScene"|"PickingHelperManager"|"SurfaceManager"|"LEDSystem"|"EquipmentVisualsSystem"|"GMJournalSystem"|"CharacterManager"|"CameraSplineSystem"|"AnimationBlueprintSystem"|"MusicManager"|"CustomStatsSystem"|"ContainerComponentSystem"|"LightProbeManager"|"EncounterManager"|"ProjectileManager"|"AtmosphereManager"|"VisualSystem"
--- @alias EclGameState string|"Idle"|"Lobby"|"Running"|"Movie"|"Unknown"|"InitMenu"|"Save"|"InitNetwork"|"InitConnection"|"Menu"|"LoadMenu"|"SwapLevel"|"LoadLevel"|"LoadModule"|"LoadSession"|"Init"|"LoadGMCampaign"|"UnloadLevel"|"UnloadModule"|"UnloadSession"|"PrepareRunning"|"Disconnect"|"Join"|"StartLoading"|"StopLoading"|"StartServer"|"Installation"|"GameMasterPause"|"Exit"|"ModReceiving"|"Paused"|"BuildStory"|"LoadLoca"
--- @alias EclItemFlags string|"CanUse"|"MovementUpdated"|"IsCraftingIngredient"|"Wadable"|"Sticky"|"IsDoor"|"CanBePickedUp"|"InteractionDisabled"|"Known"|"CanBeMoved"|"Activated"|"EnableHighlights"|"CoverAmount"|"Invulnerable"|"CanShootThrough"|"Walkable"|"CanWalkThrough"|"IsLadder"|"DontAddToBottomBar"|"WasOpened"|"PhysicsFlag1"|"Floating"|"Global"|"Invisible"|"PhysicsFlag2"|"FreezeGravity"|"StoryItem"|"JoinedDialog"|"IsSecretDoor"|"IsGrenade"|"IsSourceContainer"|"Registered"|"FoldDynamicStats"|"PhysicsFlag3"|"TeleportOnUse"|"PinnedContainer"|"HasPendingNetUpdate"|"Destroyed"|"Fade"|"Hostile"|"Unimportant"
--- @alias EclItemFlags2 string|"IsKey"|"UnEquipLocked"|"UseSoundsLoaded"|"Stolen"|"Consumable"|"CanUseRemotely"
--- @alias EclItemPhysicsFlags string|"RequestWakeNeighbours"|"PhysicsDisabled"|"RequestRaycast"
--- @alias EclSceneryFlags string|"Wadable"|"CanShootThrough"|"Walkable"|"Fadeable"|"SeeThrough"|"Invisible"|"IsWall"|"Destroyed"|"Fade"|"UnknownFlag80"|"UnknownFlag100"
--- @alias EclSceneryRenderFlags string|"CastShadow"|"IsReflecting"|"IsShadowProxy"|"UnknownRenderFlag20"|"ForceUseAnimationBlueprint"|"ReceiveDecals"|"AllowReceiveDecalWhenAnimated"|"UnknownRenderFlag1"
--- @alias EclStatusFlags string|"Started"|"RequestRefreshLifeTime"|"HasVisuals"|"Ticking"|"KeepAlive"|"RequestDelete"
--- @alias EclTaskErrorFlags string|"CantSee"|"Entangled"|"ZeroSpeed"|"NotEnoughAP"|"InvalidTarget"|"TooHeavy"|"OutOfSight"|"BlockedByShapeshift"|"TooFar"|"LadderBlocked"|"Disarmed"|"NotEnoughStrength"|"NotEnoughSpace"|"Webbed"|"TargetBlocked"|"CantReach"|"CantReach2"|"CancelTask"|"OutOfCharges"|"Encumbered"|"TooSteep"|"InvalidPosition"|"TooHigh"|"Crippled"
--- @alias EclVisualSystemSetFlags string|"CastShadow"|"ReceiveDecal"|"IsShadowProxy"|"Reflecting"|"HasHair"|"ClearEffects"|"AllowTPose"|"SomeWeaponFlag"|"ApplyColors"
--- @alias EocCombatComponentFlags string|"IsTicking"|"RequestEndTurn"|"IsBoss"|"CanForceEndTurn"|"RequestTakeExtraTurn"|"FleeOnEndTurn"|"InArena"|"Guarded"|"GuardOnEndTurn"|"CanFight"|"TurnEnded"|"TookExtraTurn"|"CanJoinCombat"|"DelayDeathCount"|"CounterAttacked"|"CanGuard"|"IsInspector"|"CanUseResistDeadTalent"|"RequestEnterCombat"
--- @alias EocSkillBarItemType string|"Skill"|"None"|"ItemTemplate"|"Action"|"Item"
--- @alias EsvActionStateType string|"Idle"|"Summoning"|"Sheath"|"Hit"|"Lockpick"|"UseSkill"|"Drop"|"Identify"|"DisarmTrap"|"Die"|"Repair"|"PrepareSkill"|"Attack"|"MoveItem"|"KnockedDown"|"Polymorphing"|"CombineItem"|"TeleportFall"|"Unsheath"|"PickUp"|"Incapacitated"|"JumpFlight"|"Animation"|"UseItem"
--- @alias EsvCharacterFlags string|"CannotAttachToGroups"|"DeferredRemoveEscapist"|"Temporary"|"InArena"|"DisableWaypointUsage"|"NoRotate"|"IsPlayer"|"Activated"|"Dead"|"CharCreationInProgress"|"OffStage"|"Invulnerable"|"CharacterControl"|"CoverAmount"|"LevelTransitionPending"|"Multiplayer"|"CannotDie"|"CanShootThrough"|"HostControl"|"StoryNPC"|"CharacterCreationFinished"|"HasOwner"|"FightMode"|"InDialog"|"Floating"|"CannotMove"|"WalkThrough"|"NeedsMakePlayerUpdate"|"SpotSneakers"|"CannotRun"|"Deactivated"|"PartyFollower"|"IgnoresTriggers"|"Totem"|"CustomLookEnabled"|"DontCacheTemplate"|"IsTrading"|"IsCompanion_M"|"IsHuge"|"InParty"|"DisableCulling"|"ForceNonzeroSpeed"|"DoNotFaceFlag"|"Loaded"|"GMReroll"|"RequestStartTurn"|"Summon"|"Detached"|"FindValidPositionOnActivate"|"ReservedForDialog"|"RegisteredForAutomatedDialog"
--- @alias EsvCharacterFlags2 string|"HasDefaultDialog"|"Global"|"TreasureGeneratedForTrader"|"HasOsirisDialog"|"Trader"|"Resurrected"
--- @alias EsvCharacterFlags3 string|"IsSpectating"|"HasWalkSpeedOverride"|"HasRunSpeedOverride"|"IsPet"|"IsGameMaster"|"IsPossessed"|"NoReptuationEffects"|"ManuallyLeveled"
--- @alias EsvCharacterTransformFlags string|"ReplaceTags"|"ReplaceScale"|"ReleasePlayerData"|"ReplaceSkills"|"ReplaceVoiceSet"|"SaveOriginalDisplayName"|"ImmediateSync"|"MoveAttachedEffects"|"ReplaceEquipment"|"ReplaceCustomLooks"|"DiscardOriginalDisplayName"|"ReplaceOriginalTemplate"|"DontCheckRootTemplate"|"ReplaceInventory"|"ReplaceCustomNameIcon"|"ReplaceCurrentTemplate"|"ReplaceScripts"|"ReplaceStats"|"Immediate"|"DontReplaceCombatState"
--- @alias EsvCharacterTransformType string|"TransformToTemplate"|"TransformToCharacter"
--- @alias EsvEntityComponentIndex string|"StartTrigger"|"CrimeRegionTrigger"|"TeleportTrigger"|"AnimationBlueprint"|"CustomStats"|"Character"|"AtmosphereTrigger"|"GMJournalNode"|"StatsAreaTrigger"|"GameMaster"|"Effect"|"Egg"|"SecretRegionTrigger"|"ExplorationTrigger"|"OverviewMap"|"EoCPointTrigger"|"Vignette"|"EoCAreaTrigger"|"Net"|"MusicVolumeTrigger"|"Spline"|"ContainerElement"|"Container"|"CrimeAreaTrigger"|"SoundVolumeTrigger"|"Note"|"CustomStatDefinition"|"Item"|"RegionTrigger"|"Combat"|"Projectile"|"EventTrigger"|"AIHintAreaTrigger"
--- @alias EsvEntitySystemIndex string|"EnvironmentalStatusManager"|"SightManager"|"GameActionManager"|"GameMasterManager"|"TurnManager"|"TriggerManager"|"GameMasterCampaignManager"|"ItemManager"|"EggManager"|"ContainerElementComponent"|"NetEntityManager"|"SurfaceManager"|"GMJournalSystem"|"CharacterManager"|"EffectManager"|"RewardManager"|"CameraSplineSystem"|"AnimationBlueprintSystem"|"CharacterSplineSystem"|"CustomStatsSystem"|"ContainerComponentSystem"|"ShroudManager"|"ProjectileManager"
--- @alias EsvGameState string|"Idle"|"Sync"|"Running"|"Unknown"|"Save"|"Uninitialized"|"LoadLevel"|"ReloadStory"|"LoadModule"|"LoadSession"|"Init"|"LoadGMCampaign"|"UnloadLevel"|"UnloadModule"|"UnloadSession"|"Disconnect"|"Installation"|"GameMasterPause"|"Exit"|"Paused"|"BuildStory"
--- @alias EsvItemFlags string|"CanUse"|"WakePhysics"|"Destroy"|"DisableInventoryView80"|"Sticky"|"LoadedTemplate"|"IsDoor"|"Frozen"|"CanBePickedUp"|"InteractionDisabled"|"ClientSync1"|"Known"|"CanBeMoved"|"IsContainer"|"LuckyFind"|"Activated"|"ForceSync"|"OffStage"|"WalkOn"|"Invisible2"|"Invulnerable"|"LevelTransitionPending"|"CanShootThrough"|"HideHP"|"DisableSync"|"IsLadder"|"DontAddToHotbar"|"WalkThrough"|"PositionChanged"|"Floating"|"TransformChanged"|"Invisible"|"FreezeGravity"|"GMFolding"|"IsSurfaceBlocker"|"StoryItem"|"IsSecretDoor"|"InPartyInventory"|"Totem"|"Invulnerable2"|"ForceClientSync"|"SourceContainer"|"NoCover"|"TeleportOnUse"|"PinnedContainer"|"IsMoving"|"Destroyed"|"CanOnlyBeUsedByOwner"|"Summon"|"IsSurfaceCloudBlocker"|"InAutomatedDialog"|"ReservedForDialog"
--- @alias EsvItemFlags2 string|"TreasureGenerated"|"IsKey"|"UnsoldGenerated"|"UnEquipLocked"|"Global"|"CanConsume"|"UseRemotely"
--- @alias EsvItemTransformFlags string|"ReplaceScripts"|"ReplaceStats"|"Immediate"
--- @alias EsvMovementStateType string|"MoveTo"|"Movement"
--- @alias EsvStatusControllerFlags string|"Summoning"|"PlayingDeathAnimation"|"HitAnimationFlag2b"|"TeleportFalling"|"CombatReady"|"SheathWeapon"|"CancelCurrentAction"|"UnsheathWeapon"|"SteerToEnemy"|"CancelSkill"|"Dying"|"PlayHitAnimation"|"CancelCurrentAction2"|"EndTurn"|"HitAnimationFlag1"|"KnockedDown"|"Polymorphing"|"PlayHitStillAnimation"|"Incapacitated"|"JumpFlight"|"HitAnimationFlag2"|"HitAnimationFlag3"
--- @alias EsvStatusFlags0 string|"IsFromItem"|"InitiateCombat"|"IsLifeTimeSet"|"Channeled"|"KeepAlive"|"Influence"|"IsOnSourceSurface"
--- @alias EsvStatusFlags1 string|"IsHostileAct"|"BringIntoCombat"|"IsInvulnerable"|"IsResistingDeath"
--- @alias EsvStatusFlags2 string|"ForceStatus"|"ForceFailStatus"|"Started"|"RequestDeleteAtTurnEnd"|"RequestClientSync2"|"RequestClientSync"|"RequestDelete"
--- @alias EsvTaskType string|"FollowNPC"|"PlayAnimation"|"PickupItem"|"Steer"|"Resurrect"|"MoveToAndTalk"|"UseSkill"|"Drop"|"Flee"|"Appear"|"Wander"|"MoveItem"|"Attack"|"TeleportToLocation"|"MoveToLocation"|"MoveToObject"|"MoveInRange"|"Disappear"|"UseItem"
--- @alias StatsAbilityType string|"MagicArmorMastery"|"FireSpecialist"|"Charm"|"Runecrafting"|"Necromancy"|"Sentinel"|"Summoning"|"Pickpocket"|"Ranged"|"Polymorph"|"PainReflection"|"Telekinesis"|"Sulfurology"|"WaterSpecialist"|"Barter"|"Sourcery"|"Crafting"|"Thievery"|"Wand"|"Loremaster"|"Reflexes"|"Repair"|"Sneaking"|"RangerLore"|"RogueLore"|"Reason"|"Persuasion"|"Intimidate"|"Leadership"|"DualWielding"|"Perseverance"|"PhysicalArmorMastery"|"VitalityMastery"|"Shield"|"Luck"|"SingleHanded"|"TwoHanded"|"WarriorLore"|"AirSpecialist"|"EarthSpecialist"|"Brewmaster"
--- @alias StatsArmorType string|"Sentinel"|"Mail"|"Robe"|"None"|"Cloth"|"Leather"|"Plate"
--- @alias StatsCharacterFlags string|"IsPlayer"|"IsSneaking"|"Invisible"|"DrinkedPotion"|"InParty"|"Blind"|"EquipmentValidated"
--- @alias StatsCharacterStatGetterType string|"Dodge"|"Hearing"|"Finesse"|"Strength"|"APStart"|"Intelligence"|"Constitution"|"CorrosiveResistance"|"Sight"|"PoisonResistance"|"LifeSteal"|"ChanceToHitBoost"|"Initiative"|"APMaximum"|"APRecovery"|"ShadowResistance"|"Wits"|"Accuracy"|"Movement"|"PiercingResistance"|"FireResistance"|"EarthResistance"|"WaterResistance"|"CustomResistance"|"MaxMp"|"Memory"|"AirResistance"|"MagicResistance"|"DamageBoost"|"CriticalChance"|"BlockChance"|"PhysicalResistance"
--- @alias StatsCriticalRoll string|"Critical"|"NotCritical"|"Roll"
--- @alias StatsDamageType string|"Air"|"Sentinel"|"Piercing"|"Physical"|"Corrosive"|"Shadow"|"Earth"|"Magic"|"Chaos"|"None"|"Water"|"Sulfuric"|"Fire"|"Poison"
--- @alias StatsDeathType string|"PetrifiedShatter"|"Sentinel"|"Explode"|"Piercing"|"Physical"|"Acid"|"Incinerate"|"Arrow"|"Hang"|"Electrocution"|"FrozenShatter"|"Sulfur"|"None"|"Surrender"|"KnockedDown"|"DoT"|"Lifetime"
--- @alias StatsEquipmentStatsType string|"Weapon"|"Shield"|"Armor"
--- @alias StatsHandednessType string|"Any"|"One"|"Two"
--- @alias StatsHighGroundBonus string|"Unknown"|"HighGround"|"EvenGround"|"LowGround"
--- @alias StatsHitFlag string|"PropagatedFromOwner"|"Flanking"|"Backstab"|"FromShacklesOfPain"|"Hit"|"ProcWindWalker"|"Reflection"|"Dodged"|"DamagedMagicArmor"|"CriticalHit"|"FromSetHP"|"Burning"|"NoDamageOnOwner"|"Blocked"|"DamagedPhysicalArmor"|"DamagedVitality"|"CounterAttack"|"Surface"|"Poisoned"|"Bleeding"|"DontCreateBloodSurface"|"NoEvents"|"DoT"|"Missed"
--- @alias StatsHitType string|"Ranged"|"Magic"|"WeaponDamage"|"Reflected"|"Surface"|"DoT"|"Melee"
--- @alias StatsItemSlot string|"Amulet"|"Weapon"|"Gloves"|"Sentinel"|"Ring"|"Horns"|"Overhead"|"Breast"|"Ring2"|"Shield"|"Leggings"|"Boots"|"Wings"|"Belt"|"Helmet"
--- @alias StatsItemSlot32 string|"Amulet"|"Weapon"|"Gloves"|"Sentinel"|"Ring"|"Horns"|"Overhead"|"Breast"|"Ring2"|"Shield"|"Leggings"|"Boots"|"Wings"|"Belt"|"Helmet"
--- @alias StatsPropertyContext string|"Self"|"SelfOnHit"|"SelfOnEquip"|"AoE"|"Target"
--- @alias StatsPropertyType string|"Status"|"Force"|"Sabotage"|"GameAction"|"SurfaceChange"|"Summon"|"OsirisTask"|"CustomDescription"|"Custom"|"Extender"
--- @alias StatsRequirementType string|"MagicArmorMastery"|"Vitality"|"FireSpecialist"|"Charm"|"TALENT_SurpriseAttack"|"TALENT_WarriorLoreNaturalResistance"|"TALENT_NoAttackOfOpportunity"|"TALENT_QuickStep"|"MaxKarma"|"Necromancy"|"TALENT_LightningRod"|"TALENT_Stench"|"TALENT_RogueLoreDaggerBackStab"|"TRAIT_Egotistical"|"Immobile"|"TALENT_MasterThief"|"Finesse"|"Summoning"|"Pickpocket"|"TALENT_ResistDead"|"TALENT_Sadist"|"Ranged"|"Polymorph"|"TALENT_Flanking"|"TALENT_ActionPoints"|"TALENT_AirSpells"|"TALENT_Elf_Lore"|"TALENT_Haymaker"|"Strength"|"PainReflection"|"TALENT_Backstab"|"TALENT_AvoidDetection"|"TALENT_WarriorLoreNaturalArmor"|"TALENT_RogueLoreMovementBonus"|"TALENT_Elf_CorpseEater"|"TALENT_Lizard_Resistance"|"TALENT_WildMag"|"Intelligence"|"Telekinesis"|"TALENT_Trade"|"TALENT_WhatARush"|"TALENT_Sourcerer"|"Constitution"|"WaterSpecialist"|"TALENT_Lockpick"|"TRAIT_Compassionate"|"Barter"|"TALENT_Damage"|"TALENT_Awareness"|"TRAIT_Bold"|"Sourcery"|"TALENT_Sight"|"TALENT_ResistFear"|"TALENT_Initiative"|"TALENT_Human_Inventive"|"TRAIT_Timid"|"Tag"|"Crafting"|"TALENT_Carry"|"TALENT_LightStep"|"TALENT_ViolentMagic"|"Thievery"|"Wand"|"TALENT_Kinetics"|"TALENT_ResurrectToFullHealth"|"TALENT_Politician"|"TALENT_RogueLoreHoldResistance"|"TRAIT_Independent"|"TALENT_GreedyVessel"|"Loremaster"|"TALENT_Repair"|"TALENT_ExtraStatPoints"|"TALENT_FolkDancer"|"TALENT_ActionPoints2"|"TALENT_ExpGain"|"TALENT_EarthSpells"|"TALENT_RangerLoreEvasionBonus"|"TALENT_Lizard_Persuasion"|"Reflexes"|"TALENT_Vitality"|"TALENT_AnimalEmpathy"|"TALENT_Perfectionist"|"Repair"|"Sneaking"|"TALENT_ItemMovement"|"TALENT_Charm"|"TALENT_FaroutDude"|"TRAIT_Vindictive"|"RangerLore"|"TALENT_ResistPoison"|"TALENT_Reason"|"TALENT_GoldenMage"|"TALENT_RogueLoreDaggerAPBonus"|"TALENT_Elementalist"|"None"|"RogueLore"|"TALENT_ItemCreation"|"TALENT_FireSpells"|"TALENT_Luck"|"TALENT_Indomitable"|"Reason"|"Persuasion"|"TALENT_ResistKnockdown"|"TALENT_InventoryAccess"|"TALENT_Escapist"|"TALENT_WarriorLoreGrenadeRange"|"TALENT_Dwarf_Sneaking"|"TALENT_Memory"|"Intimidate"|"TALENT_AttackOfOpportunity"|"TALENT_Scientist"|"TALENT_Raistlin"|"TALENT_FiveStarRestaurant"|"TALENT_LivingArmor"|"TALENT_Torturer"|"Wits"|"Leadership"|"TALENT_Leech"|"TALENT_WeatherProof"|"TALENT_Unstable"|"TRAIT_Materialistic"|"TALENT_MagicCycles"|"DualWielding"|"TALENT_ExtraSkillPoints"|"TALENT_Bully"|"TALENT_SpillNoBlood"|"TRAIT_Obedient"|"Perseverance"|"TALENT_Criticals"|"TALENT_Intimidate"|"TALENT_LoneWolf"|"TALENT_WarriorLoreNaturalHealth"|"TRAIT_Pragmatic"|"PhysicalArmorMastery"|"TALENT_StandYourGround"|"TALENT_Zombie"|"TALENT_Executioner"|"TRAIT_Forgiving"|"TALENT_ElementalRanger"|"TALENT_Demon"|"TRAIT_Altruistic"|"TRAIT_Romantic"|"TALENT_Soulcatcher"|"Shield"|"Luck"|"TALENT_ChanceToHitRanged"|"TALENT_ResistSilence"|"TALENT_WalkItOff"|"TRAIT_Spiritual"|"TALENT_Gladiator"|"TALENT_ChanceToHitMelee"|"TALENT_WaterSpells"|"TALENT_RogueLoreGrenadePrecision"|"TALENT_ExtraWandCharge"|"TALENT_Jitterbug"|"Memory"|"SingleHanded"|"TALENT_ResistStun"|"TALENT_RangerLoreArrowRecover"|"TALENT_DualWieldingDodging"|"TALENT_Dwarf_Sturdy"|"TRAIT_Righteous"|"Level"|"TwoHanded"|"TALENT_MrKnowItAll"|"TALENT_ElementalAffinity"|"TALENT_Ambidextrous"|"TRAIT_Renegade"|"Combat"|"WarriorLore"|"TALENT_Courageous"|"TRAIT_Blunt"|"TRAIT_Considerate"|"AirSpecialist"|"TALENT_Durability"|"TALENT_IceKing"|"TALENT_Kickstarter"|"TALENT_RangerLoreRangedAPBonus"|"TRAIT_Heartless"|"EarthSpecialist"|"TALENT_IncreasedArmor"|"TALENT_Human_Civil"|"MinKarma"
--- @alias StatsTalentType string|"Vitality"|"Charm"|"LightStep"|"WalkItOff"|"WandCharge"|"Scientist"|"ElementalAffinity"|"FolkDancer"|"WarriorLoreNaturalHealth"|"ItemCreation"|"Raistlin"|"IceKing"|"SpillNoBlood"|"RogueLoreDaggerBackStab"|"Flanking"|"MrKnowItAll"|"Lizard_Persuasion"|"Backstab"|"WhatARush"|"RogueLoreGrenadePrecision"|"Human_Inventive"|"ResurrectExtraHealth"|"Lockpick"|"ChanceToHitMelee"|"FaroutDude"|"Human_Civil"|"WildMag"|"ActionPoints2"|"ExpGain"|"ElementalRanger"|"RangerLoreEvasionBonus"|"Dwarf_Sneaking"|"Criticals"|"Durability"|"LightningRod"|"WarriorLoreGrenadeRange"|"Dwarf_Sturdy"|"IncreasedArmor"|"Sight"|"Politician"|"Elf_Lore"|"Quest_SpidersKiss_Per"|"ResistFear"|"WeatherProof"|"Demon"|"Perfectionist"|"ResistKnockdown"|"FiveStarRestaurant"|"LoneWolf"|"Executioner"|"ResistStun"|"RogueLoreMovementBonus"|"ViolentMagic"|"ActionPoints"|"ResistPoison"|"Lizard_Resistance"|"QuickStep"|"Sadist"|"ResistSilence"|"Carry"|"Initiative"|"BeastMaster"|"NaturalConductor"|"ResistDead"|"Repair"|"ExtraSkillPoints"|"Quest_GhostTree"|"Throwing"|"RangerLoreRangedAPBonus"|"LivingArmor"|"None"|"Zombie"|"DualWieldingDodging"|"Torturer"|"Reason"|"Quest_SpidersKiss_Null"|"Ambidextrous"|"ItemMovement"|"AttackOfOpportunity"|"ExtraStatPoints"|"Intimidate"|"WarriorLoreNaturalResistance"|"Unstable"|"AnimalEmpathy"|"WarriorLoreNaturalArmor"|"Quest_Rooted"|"Rager"|"Trade"|"Awareness"|"RogueLoreHoldResistance"|"PainDrinker"|"Max"|"Escapist"|"Quest_SpidersKiss_Str"|"Sourcerer"|"Damage"|"FireSpells"|"DeathfogResistant"|"Elementalist"|"WaterSpells"|"ResurrectToFullHealth"|"Bully"|"Haymaker"|"AirSpells"|"Luck"|"RogueLoreDaggerAPBonus"|"Gladiator"|"EarthSpells"|"Elf_CorpseEating"|"Indomitable"|"InventoryAccess"|"Stench"|"Memory"|"Quest_TradeSecrets"|"Jitterbug"|"ChanceToHitRanged"|"AvoidDetection"|"Soulcatcher"|"Kickstarter"|"RangerLoreArrowRecover"|"MasterThief"|"StandYourGround"|"Courageous"|"NoAttackOfOpportunity"|"GreedyVessel"|"SurpriseAttack"|"Leech"|"GoldenMage"|"Quest_SpidersKiss_Int"|"MagicCycles"
--- @alias StatsWeaponType string|"Sentinel"|"Spear"|"Sword"|"Axe"|"Bow"|"Wand"|"Arrow"|"None"|"Knife"|"Crossbow"|"Club"|"Staff"|"Rifle"


--- @class ActivePersistentLevelTemplate
--- @field Area AreaTrigger
--- @field Template LevelTemplate


--- @class Actor
--- @field MeshBindings MeshBinding[]
--- @field PhysicsRagdoll PhysicsRagdoll
--- @field Skeleton Skeleton
--- @field TextKeyPrepareFlags uint8
--- @field Time GameTime
--- @field Visual Visual


--- @class AiHintAreaTriggerData:ITriggerData
--- @field IsAnchor boolean


--- @class AnimatableObject:RenderableObject
--- @field MeshBinding MeshBinding


--- @class AnimationSet
--- @field AnimationSubSets table<int32, table<FixedString, AnimationSetAnimationDescriptor>>
--- @field Type FixedString


--- @class AnimationSetAnimationDescriptor
--- @field ID FixedString
--- @field Name FixedString


--- @class AppliedMaterial
--- @field BlendState uint8
--- @field DefaultShaderHandle ComponentHandle
--- @field DefaultShaderType int32
--- @field DefaultVertexFormatHandle ComponentHandle
--- @field DepthShaderHandle ComponentHandle
--- @field DepthShaderType int32
--- @field DepthVertexFormatHandle ComponentHandle
--- @field ForceForwardShading boolean
--- @field ForcePostRefractionAlpha boolean
--- @field ForwardShaderHandle ComponentHandle
--- @field ForwardShaderType int32
--- @field ForwardVertexFormatHandle ComponentHandle
--- @field HasDebugMaterial boolean
--- @field IsOverlay boolean
--- @field Material Material
--- @field MaterialParameters MaterialParameters
--- @field OverlayOffset number
--- @field Renderable RenderableObject
--- @field ShadowShaderHandle ComponentHandle
--- @field ShadowShaderType int32
--- @field ShadowVertexFormatHandle ComponentHandle


--- @class AreaTrigger:Trigger
--- @field ObjectsInTrigger ComponentHandle[]
--- @field Physics AreaTriggerPhysics


--- @class AreaTriggerPhysics
--- @field AreaTypeId int32


--- @class AtmosphereTrigger:AreaTrigger
--- @field AtmosphereResourceIds FixedString[]
--- @field CurrentAtmosphere FixedString
--- @field FadeTime number


--- @class AtmosphereTriggerData:ITriggerData
--- @field Atmospheres FixedString[]
--- @field FadeTime number


--- @class BaseComponent
--- @field Component ComponentHandleWithType
--- @field Entity EntityHandle


--- @class BookActionData:IActionData
--- @field BookId FixedString


--- @class Bound
--- @field Center vec3
--- @field IsCenterSet boolean
--- @field Max vec3
--- @field Min vec3
--- @field Radius number


--- @class CDivinityStatsCharacter:StatsObjectInstance
--- @field APMaximum int32
--- @field APRecovery int32
--- @field APStart int32
--- @field Accuracy int32
--- @field AcidImmunity boolean
--- @field AirResistance int32
--- @field AirSpecialist int32
--- @field ArmorAfterHitCooldownMultiplier int32
--- @field Arrow boolean
--- @field AttributeFlags StatAttributeFlags
--- @field AttributeFlagsUpdated boolean
--- @field Barter int32
--- @field BaseAPMaximum int32
--- @field BaseAPRecovery int32
--- @field BaseAPStart int32
--- @field BaseAccuracy int32
--- @field BaseAirResistance int32
--- @field BaseAirSpecialist int32
--- @field BaseAttributeFlags StatAttributeFlags
--- @field BaseBarter int32
--- @field BaseBlockChance int32
--- @field BaseBrewmaster int32
--- @field BaseChanceToHitBoost int32
--- @field BaseCharm int32
--- @field BaseConstitution int32
--- @field BaseCorrosiveResistance int32
--- @field BaseCrafting int32
--- @field BaseCriticalChance int32
--- @field BaseCustomResistance int32
--- @field BaseDamageBoost int32
--- @field BaseDodge int32
--- @field BaseDualWielding int32
--- @field BaseEarthResistance int32
--- @field BaseEarthSpecialist int32
--- @field BaseFinesse int32
--- @field BaseFireResistance int32
--- @field BaseFireSpecialist int32
--- @field BaseHearing int32
--- @field BaseInitiative int32
--- @field BaseIntelligence int32
--- @field BaseIntimidate int32
--- @field BaseLeadership int32
--- @field BaseLifeSteal int32
--- @field BaseLoremaster int32
--- @field BaseLuck int32
--- @field BaseMagicArmorMastery int32
--- @field BaseMagicResistance int32
--- @field BaseMaxArmor int32
--- @field BaseMaxMagicArmor int32
--- @field BaseMaxMp int32
--- @field BaseMaxSummons int32
--- @field BaseMaxVitality int32
--- @field BaseMemory int32
--- @field BaseMovement int32
--- @field BaseNecromancy int32
--- @field BasePainReflection int32
--- @field BasePerseverance int32
--- @field BasePersuasion int32
--- @field BasePhysicalArmorMastery int32
--- @field BasePhysicalResistance int32
--- @field BasePickpocket int32
--- @field BasePiercingResistance int32
--- @field BasePoisonResistance int32
--- @field BasePolymorph int32
--- @field BaseRanged int32
--- @field BaseRangerLore int32
--- @field BaseReason int32
--- @field BaseReflexes int32
--- @field BaseRepair int32
--- @field BaseRogueLore int32
--- @field BaseRunecrafting int32
--- @field BaseSentinel int32
--- @field BaseShadowResistance int32
--- @field BaseShield int32
--- @field BaseSight int32
--- @field BaseSingleHanded int32
--- @field BaseSneaking int32
--- @field BaseSourcery int32
--- @field BaseStrength int32
--- @field BaseSulfurology int32
--- @field BaseSummoning int32
--- @field BaseTelekinesis int32
--- @field BaseThievery int32
--- @field BaseTwoHanded int32
--- @field BaseVitalityMastery int32
--- @field BaseWand int32
--- @field BaseWarriorLore int32
--- @field BaseWaterResistance int32
--- @field BaseWaterSpecialist int32
--- @field BaseWits int32
--- @field BleedingImmunity boolean
--- @field BlessedImmunity boolean
--- @field Blind boolean
--- @field BlindImmunity boolean
--- @field BlockChance int32
--- @field BonusActionPoints int32
--- @field Brewmaster int32
--- @field BurnContact boolean
--- @field BurnImmunity boolean
--- @field ChanceToHitBoost int32
--- @field Character IGameObject
--- @field Charm int32
--- @field CharmImmunity boolean
--- @field ChickenImmunity boolean
--- @field ChillContact boolean
--- @field ChilledImmunity boolean
--- @field ClairvoyantImmunity boolean
--- @field Constitution int32
--- @field CorrosiveResistance int32
--- @field Crafting int32
--- @field CrippledImmunity boolean
--- @field CriticalChance int32
--- @field CurrentAP int32
--- @field CurrentArmor int32
--- @field CurrentMagicArmor int32
--- @field CurrentVitality int32
--- @field CursedImmunity boolean
--- @field CustomResistance int32
--- @field DamageBoost int32
--- @field DecayingImmunity boolean
--- @field DeflectProjectiles boolean
--- @field DisarmedImmunity boolean
--- @field DiseasedImmunity boolean
--- @field Dodge int32
--- @field DrinkedPotion boolean
--- @field DrunkImmunity boolean
--- @field DualWielding int32
--- @field DynamicStats StatsCharacterDynamicStat[]
--- @field EarthResistance int32
--- @field EarthSpecialist int32
--- @field EnragedImmunity boolean
--- @field EntangledContact boolean
--- @field EquipmentValidated boolean
--- @field Experience int32
--- @field FearImmunity boolean
--- @field Finesse int32
--- @field FireResistance int32
--- @field FireSpecialist int32
--- @field Flags StatsCharacterFlags
--- @field Flanked uint8
--- @field Floating boolean
--- @field FreezeContact boolean
--- @field FreezeImmunity boolean
--- @field Grounded boolean
--- @field HasTwoHandedWeapon int32
--- @field HastedImmunity boolean
--- @field Hearing int32
--- @field IgnoreClouds boolean
--- @field IgnoreCursedOil boolean
--- @field InParty boolean
--- @field InfectiousDiseasedImmunity boolean
--- @field Initiative int32
--- @field Intelligence int32
--- @field Intimidate int32
--- @field InvisibilityImmunity boolean
--- @field Invisible boolean
--- @field IsIncapacitatedRefCount int32
--- @field IsPlayer boolean
--- @field IsSneaking boolean
--- @field ItemBoostedAttributeFlags StatAttributeFlags
--- @field Karma int32
--- @field KnockdownImmunity boolean
--- @field Leadership int32
--- @field LifeSteal int32
--- @field LootableWhenEquipped boolean
--- @field Loremaster int32
--- @field LoseDurabilityOnCharacterHit boolean
--- @field Luck int32
--- @field MPStart int32
--- @field MadnessImmunity boolean
--- @field MagicArmorAfterHitCooldownMultiplier int32
--- @field MagicArmorMastery int32
--- @field MagicResistance int32
--- @field MagicalSulfur boolean
--- @field MainWeapon CDivinityStatsItem
--- @field MaxArmor int32
--- @field MaxMagicArmor int32
--- @field MaxMp int32
--- @field MaxMpOverride int32
--- @field MaxResistance int32
--- @field MaxSummons int32
--- @field MaxVitality int32
--- @field Memory int32
--- @field Movement int32
--- @field MuteImmunity boolean
--- @field MyGuid FixedString
--- @field Necromancy int32
--- @field NetID NetId
--- @field OffHandWeapon CDivinityStatsItem
--- @field PainReflection int32
--- @field Perseverance int32
--- @field Persuasion int32
--- @field PetrifiedImmunity boolean
--- @field PhysicalArmorMastery int32
--- @field PhysicalResistance int32
--- @field Pickpocket int32
--- @field PickpocketableWhenEquipped boolean
--- @field PiercingResistance int32
--- @field PoisonContact boolean
--- @field PoisonImmunity boolean
--- @field PoisonResistance int32
--- @field Polymorph int32
--- @field Position vec3
--- @field ProtectFromSummon boolean
--- @field Ranged int32
--- @field RangerLore int32
--- @field Reason int32
--- @field Reflexes int32
--- @field RegeneratingImmunity boolean
--- @field Repair int32
--- @field Reputation int32
--- @field RogueLore int32
--- @field Runecrafting int32
--- @field Sentinel int32
--- @field ShacklesOfPainImmunity boolean
--- @field ShadowResistance int32
--- @field Shield int32
--- @field ShockedImmunity boolean
--- @field Sight int32
--- @field SingleHanded int32
--- @field SleepingImmunity boolean
--- @field SlippingImmunity boolean
--- @field SlowedImmunity boolean
--- @field Sneaking int32
--- @field Sourcery int32
--- @field StatsFromStatsEntry StatsCharacterDynamicStat
--- @field Strength int32
--- @field StunContact boolean
--- @field StunImmunity boolean
--- @field SuffocatingImmunity boolean
--- @field Sulfurology int32
--- @field Summoning int32
--- @field SurfacePathInfluences SurfacePathInfluence[]
--- @field TALENT_ActionPoints boolean
--- @field TALENT_ActionPoints2 boolean
--- @field TALENT_AirSpells boolean
--- @field TALENT_Ambidextrous boolean
--- @field TALENT_AnimalEmpathy boolean
--- @field TALENT_AttackOfOpportunity boolean
--- @field TALENT_AvoidDetection boolean
--- @field TALENT_Awareness boolean
--- @field TALENT_Backstab boolean
--- @field TALENT_BeastMaster boolean
--- @field TALENT_Bully boolean
--- @field TALENT_Carry boolean
--- @field TALENT_ChanceToHitMelee boolean
--- @field TALENT_ChanceToHitRanged boolean
--- @field TALENT_Charm boolean
--- @field TALENT_Courageous boolean
--- @field TALENT_Criticals boolean
--- @field TALENT_Damage boolean
--- @field TALENT_DeathfogResistant boolean
--- @field TALENT_Demon boolean
--- @field TALENT_DualWieldingDodging boolean
--- @field TALENT_Durability boolean
--- @field TALENT_Dwarf_Sneaking boolean
--- @field TALENT_Dwarf_Sturdy boolean
--- @field TALENT_EarthSpells boolean
--- @field TALENT_ElementalAffinity boolean
--- @field TALENT_ElementalRanger boolean
--- @field TALENT_Elementalist boolean
--- @field TALENT_Elf_CorpseEating boolean
--- @field TALENT_Elf_Lore boolean
--- @field TALENT_Escapist boolean
--- @field TALENT_Executioner boolean
--- @field TALENT_ExpGain boolean
--- @field TALENT_ExtraSkillPoints boolean
--- @field TALENT_ExtraStatPoints boolean
--- @field TALENT_FaroutDude boolean
--- @field TALENT_FireSpells boolean
--- @field TALENT_FiveStarRestaurant boolean
--- @field TALENT_Flanking boolean
--- @field TALENT_FolkDancer boolean
--- @field TALENT_Gladiator boolean
--- @field TALENT_GoldenMage boolean
--- @field TALENT_GreedyVessel boolean
--- @field TALENT_Haymaker boolean
--- @field TALENT_Human_Civil boolean
--- @field TALENT_Human_Inventive boolean
--- @field TALENT_IceKing boolean
--- @field TALENT_IncreasedArmor boolean
--- @field TALENT_Indomitable boolean
--- @field TALENT_Initiative boolean
--- @field TALENT_Intimidate boolean
--- @field TALENT_InventoryAccess boolean
--- @field TALENT_ItemCreation boolean
--- @field TALENT_ItemMovement boolean
--- @field TALENT_Jitterbug boolean
--- @field TALENT_Kickstarter boolean
--- @field TALENT_Leech boolean
--- @field TALENT_LightStep boolean
--- @field TALENT_LightningRod boolean
--- @field TALENT_LivingArmor boolean
--- @field TALENT_Lizard_Persuasion boolean
--- @field TALENT_Lizard_Resistance boolean
--- @field TALENT_Lockpick boolean
--- @field TALENT_LoneWolf boolean
--- @field TALENT_Luck boolean
--- @field TALENT_MagicCycles boolean
--- @field TALENT_MasterThief boolean
--- @field TALENT_Max boolean
--- @field TALENT_Memory boolean
--- @field TALENT_MrKnowItAll boolean
--- @field TALENT_NaturalConductor boolean
--- @field TALENT_NoAttackOfOpportunity boolean
--- @field TALENT_None boolean
--- @field TALENT_PainDrinker boolean
--- @field TALENT_Perfectionist boolean
--- @field TALENT_Politician boolean
--- @field TALENT_Quest_GhostTree boolean
--- @field TALENT_Quest_Rooted boolean
--- @field TALENT_Quest_SpidersKiss_Int boolean
--- @field TALENT_Quest_SpidersKiss_Null boolean
--- @field TALENT_Quest_SpidersKiss_Per boolean
--- @field TALENT_Quest_SpidersKiss_Str boolean
--- @field TALENT_Quest_TradeSecrets boolean
--- @field TALENT_QuickStep boolean
--- @field TALENT_Rager boolean
--- @field TALENT_Raistlin boolean
--- @field TALENT_RangerLoreArrowRecover boolean
--- @field TALENT_RangerLoreEvasionBonus boolean
--- @field TALENT_RangerLoreRangedAPBonus boolean
--- @field TALENT_Reason boolean
--- @field TALENT_Repair boolean
--- @field TALENT_ResistDead boolean
--- @field TALENT_ResistFear boolean
--- @field TALENT_ResistKnockdown boolean
--- @field TALENT_ResistPoison boolean
--- @field TALENT_ResistSilence boolean
--- @field TALENT_ResistStun boolean
--- @field TALENT_ResurrectExtraHealth boolean
--- @field TALENT_ResurrectToFullHealth boolean
--- @field TALENT_RogueLoreDaggerAPBonus boolean
--- @field TALENT_RogueLoreDaggerBackStab boolean
--- @field TALENT_RogueLoreGrenadePrecision boolean
--- @field TALENT_RogueLoreHoldResistance boolean
--- @field TALENT_RogueLoreMovementBonus boolean
--- @field TALENT_Sadist boolean
--- @field TALENT_Scientist boolean
--- @field TALENT_Sight boolean
--- @field TALENT_Soulcatcher boolean
--- @field TALENT_Sourcerer boolean
--- @field TALENT_SpillNoBlood boolean
--- @field TALENT_StandYourGround boolean
--- @field TALENT_Stench boolean
--- @field TALENT_SurpriseAttack boolean
--- @field TALENT_Throwing boolean
--- @field TALENT_Torturer boolean
--- @field TALENT_Trade boolean
--- @field TALENT_Unstable boolean
--- @field TALENT_ViolentMagic boolean
--- @field TALENT_Vitality boolean
--- @field TALENT_WalkItOff boolean
--- @field TALENT_WandCharge boolean
--- @field TALENT_WarriorLoreGrenadeRange boolean
--- @field TALENT_WarriorLoreNaturalArmor boolean
--- @field TALENT_WarriorLoreNaturalHealth boolean
--- @field TALENT_WarriorLoreNaturalResistance boolean
--- @field TALENT_WaterSpells boolean
--- @field TALENT_WeatherProof boolean
--- @field TALENT_WhatARush boolean
--- @field TALENT_WildMag boolean
--- @field TALENT_Zombie boolean
--- @field TauntedImmunity boolean
--- @field Telekinesis int32
--- @field Thievery int32
--- @field ThrownImmunity boolean
--- @field Torch boolean
--- @field TraitOrder int32[]
--- @field TwoHanded int32
--- @field Unbreakable boolean
--- @field Unrepairable boolean
--- @field Unstorable boolean
--- @field VitalityMastery int32
--- @field Wand int32
--- @field WarmImmunity boolean
--- @field WarriorLore int32
--- @field WaterResistance int32
--- @field WaterSpecialist int32
--- @field WeakImmunity boolean
--- @field WebImmunity boolean
--- @field WetImmunity boolean
--- @field Wits int32
--- @field GetItemBySlot fun(self:CDivinityStatsCharacter, slot:StatsItemSlot, mustBeEquipped:boolean|nil):CDivinityStatsItem


--- @class CDivinityStatsEquipmentAttributes
--- @field APRecovery int32
--- @field AccuracyBoost int32
--- @field AcidImmunity boolean
--- @field AirResistance int32
--- @field AirSpecialist int32
--- @field Arrow boolean
--- @field Barter int32
--- @field BleedingImmunity boolean
--- @field BlessedImmunity boolean
--- @field BlindImmunity boolean
--- @field Bodybuilding int32
--- @field BoostName FixedString
--- @field Brewmaster int32
--- @field BurnContact boolean
--- @field BurnImmunity boolean
--- @field ChanceToHitBoost int32
--- @field Charm int32
--- @field CharmImmunity boolean
--- @field ChickenImmunity boolean
--- @field ChillContact boolean
--- @field ChilledImmunity boolean
--- @field ClairvoyantImmunity boolean
--- @field ConstitutionBoost int32
--- @field CorrosiveResistance int32
--- @field Crafting int32
--- @field CrippledImmunity boolean
--- @field CriticalChance int32
--- @field CursedImmunity boolean
--- @field CustomResistance int32
--- @field DecayingImmunity boolean
--- @field DeflectProjectiles boolean
--- @field DisarmedImmunity boolean
--- @field DiseasedImmunity boolean
--- @field DodgeBoost int32
--- @field DrunkImmunity boolean
--- @field DualWielding int32
--- @field Durability int32
--- @field DurabilityDegradeSpeed int32
--- @field EarthResistance int32
--- @field EarthSpecialist int32
--- @field EnragedImmunity boolean
--- @field EntangledContact boolean
--- @field FearImmunity boolean
--- @field FinesseBoost int32
--- @field FireResistance int32
--- @field FireSpecialist int32
--- @field Floating boolean
--- @field FreezeContact boolean
--- @field FreezeImmunity boolean
--- @field Grounded boolean
--- @field HastedImmunity boolean
--- @field HearingBoost int32
--- @field IgnoreClouds boolean
--- @field IgnoreCursedOil boolean
--- @field InfectiousDiseasedImmunity boolean
--- @field Initiative int32
--- @field IntelligenceBoost int32
--- @field Intimidate int32
--- @field InvisibilityImmunity boolean
--- @field ItemColor FixedString
--- @field KnockdownImmunity boolean
--- @field Leadership int32
--- @field LifeSteal int32
--- @field LootableWhenEquipped boolean
--- @field Loremaster int32
--- @field LoseDurabilityOnCharacterHit boolean
--- @field Luck int32
--- @field MadnessImmunity boolean
--- @field MagicArmorMastery int32
--- @field MagicResistance int32
--- @field MagicalSulfur boolean
--- @field MaxAP int32
--- @field MaxSummons int32
--- @field MemoryBoost int32
--- @field ModifierType uint32
--- @field Movement int32
--- @field MovementSpeedBoost int32
--- @field MuteImmunity boolean
--- @field Necromancy int32
--- @field ObjectInstanceName FixedString
--- @field PainReflection int32
--- @field Perseverance int32
--- @field Persuasion int32
--- @field PetrifiedImmunity boolean
--- @field PhysicalArmorMastery int32
--- @field PhysicalResistance int32
--- @field Pickpocket int32
--- @field PickpocketableWhenEquipped boolean
--- @field PiercingResistance int32
--- @field PoisonContact boolean
--- @field PoisonImmunity boolean
--- @field PoisonResistance int32
--- @field Polymorph int32
--- @field ProtectFromSummon boolean
--- @field Ranged int32
--- @field RangerLore int32
--- @field Reason int32
--- @field Reflection StatsReflectionSet
--- @field Reflexes int32
--- @field RegeneratingImmunity boolean
--- @field Repair int32
--- @field RogueLore int32
--- @field RuneSlots int32
--- @field RuneSlots_V1 int32
--- @field Runecrafting int32
--- @field Sentinel int32
--- @field ShacklesOfPainImmunity boolean
--- @field ShadowResistance int32
--- @field Shield int32
--- @field ShockedImmunity boolean
--- @field SightBoost int32
--- @field SingleHanded int32
--- @field Skills FixedString
--- @field SleepingImmunity boolean
--- @field SlippingImmunity boolean
--- @field SlowedImmunity boolean
--- @field Sneaking int32
--- @field SourcePointsBoost int32
--- @field Sourcery int32
--- @field StartAP int32
--- @field StatsType StatsEquipmentStatsType
--- @field StrengthBoost int32
--- @field StunContact boolean
--- @field StunImmunity boolean
--- @field SuffocatingImmunity boolean
--- @field Sulfurology int32
--- @field Summoning int32
--- @field TALENT_ActionPoints boolean
--- @field TALENT_ActionPoints2 boolean
--- @field TALENT_AirSpells boolean
--- @field TALENT_Ambidextrous boolean
--- @field TALENT_AnimalEmpathy boolean
--- @field TALENT_AttackOfOpportunity boolean
--- @field TALENT_AvoidDetection boolean
--- @field TALENT_Awareness boolean
--- @field TALENT_Backstab boolean
--- @field TALENT_BeastMaster boolean
--- @field TALENT_Bully boolean
--- @field TALENT_Carry boolean
--- @field TALENT_ChanceToHitMelee boolean
--- @field TALENT_ChanceToHitRanged boolean
--- @field TALENT_Charm boolean
--- @field TALENT_Courageous boolean
--- @field TALENT_Criticals boolean
--- @field TALENT_Damage boolean
--- @field TALENT_DeathfogResistant boolean
--- @field TALENT_Demon boolean
--- @field TALENT_DualWieldingDodging boolean
--- @field TALENT_Durability boolean
--- @field TALENT_Dwarf_Sneaking boolean
--- @field TALENT_Dwarf_Sturdy boolean
--- @field TALENT_EarthSpells boolean
--- @field TALENT_ElementalAffinity boolean
--- @field TALENT_ElementalRanger boolean
--- @field TALENT_Elementalist boolean
--- @field TALENT_Elf_CorpseEating boolean
--- @field TALENT_Elf_Lore boolean
--- @field TALENT_Escapist boolean
--- @field TALENT_Executioner boolean
--- @field TALENT_ExpGain boolean
--- @field TALENT_ExtraSkillPoints boolean
--- @field TALENT_ExtraStatPoints boolean
--- @field TALENT_FaroutDude boolean
--- @field TALENT_FireSpells boolean
--- @field TALENT_FiveStarRestaurant boolean
--- @field TALENT_Flanking boolean
--- @field TALENT_FolkDancer boolean
--- @field TALENT_Gladiator boolean
--- @field TALENT_GoldenMage boolean
--- @field TALENT_GreedyVessel boolean
--- @field TALENT_Haymaker boolean
--- @field TALENT_Human_Civil boolean
--- @field TALENT_Human_Inventive boolean
--- @field TALENT_IceKing boolean
--- @field TALENT_IncreasedArmor boolean
--- @field TALENT_Indomitable boolean
--- @field TALENT_Initiative boolean
--- @field TALENT_Intimidate boolean
--- @field TALENT_InventoryAccess boolean
--- @field TALENT_ItemCreation boolean
--- @field TALENT_ItemMovement boolean
--- @field TALENT_Jitterbug boolean
--- @field TALENT_Kickstarter boolean
--- @field TALENT_Leech boolean
--- @field TALENT_LightStep boolean
--- @field TALENT_LightningRod boolean
--- @field TALENT_LivingArmor boolean
--- @field TALENT_Lizard_Persuasion boolean
--- @field TALENT_Lizard_Resistance boolean
--- @field TALENT_Lockpick boolean
--- @field TALENT_LoneWolf boolean
--- @field TALENT_Luck boolean
--- @field TALENT_MagicCycles boolean
--- @field TALENT_MasterThief boolean
--- @field TALENT_Max boolean
--- @field TALENT_Memory boolean
--- @field TALENT_MrKnowItAll boolean
--- @field TALENT_NaturalConductor boolean
--- @field TALENT_NoAttackOfOpportunity boolean
--- @field TALENT_None boolean
--- @field TALENT_PainDrinker boolean
--- @field TALENT_Perfectionist boolean
--- @field TALENT_Politician boolean
--- @field TALENT_Quest_GhostTree boolean
--- @field TALENT_Quest_Rooted boolean
--- @field TALENT_Quest_SpidersKiss_Int boolean
--- @field TALENT_Quest_SpidersKiss_Null boolean
--- @field TALENT_Quest_SpidersKiss_Per boolean
--- @field TALENT_Quest_SpidersKiss_Str boolean
--- @field TALENT_Quest_TradeSecrets boolean
--- @field TALENT_QuickStep boolean
--- @field TALENT_Rager boolean
--- @field TALENT_Raistlin boolean
--- @field TALENT_RangerLoreArrowRecover boolean
--- @field TALENT_RangerLoreEvasionBonus boolean
--- @field TALENT_RangerLoreRangedAPBonus boolean
--- @field TALENT_Reason boolean
--- @field TALENT_Repair boolean
--- @field TALENT_ResistDead boolean
--- @field TALENT_ResistFear boolean
--- @field TALENT_ResistKnockdown boolean
--- @field TALENT_ResistPoison boolean
--- @field TALENT_ResistSilence boolean
--- @field TALENT_ResistStun boolean
--- @field TALENT_ResurrectExtraHealth boolean
--- @field TALENT_ResurrectToFullHealth boolean
--- @field TALENT_RogueLoreDaggerAPBonus boolean
--- @field TALENT_RogueLoreDaggerBackStab boolean
--- @field TALENT_RogueLoreGrenadePrecision boolean
--- @field TALENT_RogueLoreHoldResistance boolean
--- @field TALENT_RogueLoreMovementBonus boolean
--- @field TALENT_Sadist boolean
--- @field TALENT_Scientist boolean
--- @field TALENT_Sight boolean
--- @field TALENT_Soulcatcher boolean
--- @field TALENT_Sourcerer boolean
--- @field TALENT_SpillNoBlood boolean
--- @field TALENT_StandYourGround boolean
--- @field TALENT_Stench boolean
--- @field TALENT_SurpriseAttack boolean
--- @field TALENT_Throwing boolean
--- @field TALENT_Torturer boolean
--- @field TALENT_Trade boolean
--- @field TALENT_Unstable boolean
--- @field TALENT_ViolentMagic boolean
--- @field TALENT_Vitality boolean
--- @field TALENT_WalkItOff boolean
--- @field TALENT_WandCharge boolean
--- @field TALENT_WarriorLoreGrenadeRange boolean
--- @field TALENT_WarriorLoreNaturalArmor boolean
--- @field TALENT_WarriorLoreNaturalHealth boolean
--- @field TALENT_WarriorLoreNaturalResistance boolean
--- @field TALENT_WaterSpells boolean
--- @field TALENT_WeatherProof boolean
--- @field TALENT_WhatARush boolean
--- @field TALENT_WildMag boolean
--- @field TALENT_Zombie boolean
--- @field TauntedImmunity boolean
--- @field Telekinesis int32
--- @field Thievery int32
--- @field ThrownImmunity boolean
--- @field Torch boolean
--- @field TwoHanded int32
--- @field Unbreakable boolean
--- @field Unrepairable boolean
--- @field Unstorable boolean
--- @field Value int32
--- @field VitalityBoost int32
--- @field VitalityMastery int32
--- @field Wand int32
--- @field WarmImmunity boolean
--- @field WarriorLore int32
--- @field WaterResistance int32
--- @field WaterSpecialist int32
--- @field WeakImmunity boolean
--- @field WebImmunity boolean
--- @field Weight int32
--- @field WetImmunity boolean
--- @field Willpower int32
--- @field WitsBoost int32


--- @class CDivinityStatsEquipmentAttributesArmor:CDivinityStatsEquipmentAttributes
--- @field ArmorBoost int32
--- @field ArmorValue int32
--- @field MagicArmorBoost int32
--- @field MagicArmorValue int32


--- @class CDivinityStatsEquipmentAttributesShield:CDivinityStatsEquipmentAttributes
--- @field ArmorBoost int32
--- @field ArmorValue int32
--- @field Blocking int32
--- @field MagicArmorBoost int32
--- @field MagicArmorValue int32


--- @class CDivinityStatsEquipmentAttributesWeapon:CDivinityStatsEquipmentAttributes
--- @field AttackAPCost int32
--- @field CleaveAngle int32
--- @field CleavePercentage number
--- @field CriticalDamage int32
--- @field DamageBoost int32
--- @field DamageFromBase int32
--- @field DamageType StatsDamageType
--- @field MaxDamage int32
--- @field MinDamage int32
--- @field Projectile FixedString
--- @field WeaponRange number


--- @class CDivinityStatsItem:StatsObjectInstance
--- @field AcidImmunity boolean
--- @field AirSpecialist int32
--- @field AnimType int32
--- @field Arrow boolean
--- @field AttributeFlags StatAttributeFlags
--- @field Barter int32
--- @field BaseAirSpecialist int32
--- @field BaseBarter int32
--- @field BaseBrewmaster int32
--- @field BaseCharm int32
--- @field BaseCrafting int32
--- @field BaseDualWielding int32
--- @field BaseEarthSpecialist int32
--- @field BaseFireSpecialist int32
--- @field BaseIntimidate int32
--- @field BaseLeadership int32
--- @field BaseLoremaster int32
--- @field BaseLuck int32
--- @field BaseMagicArmorMastery int32
--- @field BaseNecromancy int32
--- @field BasePainReflection int32
--- @field BasePerseverance int32
--- @field BasePersuasion int32
--- @field BasePhysicalArmorMastery int32
--- @field BasePickpocket int32
--- @field BasePolymorph int32
--- @field BaseRanged int32
--- @field BaseRangerLore int32
--- @field BaseReason int32
--- @field BaseReflexes int32
--- @field BaseRepair int32
--- @field BaseRogueLore int32
--- @field BaseRunecrafting int32
--- @field BaseSentinel int32
--- @field BaseShield int32
--- @field BaseSingleHanded int32
--- @field BaseSneaking int32
--- @field BaseSourcery int32
--- @field BaseSulfurology int32
--- @field BaseSummoning int32
--- @field BaseTelekinesis int32
--- @field BaseThievery int32
--- @field BaseTwoHanded int32
--- @field BaseVitalityMastery int32
--- @field BaseWand int32
--- @field BaseWarriorLore int32
--- @field BaseWaterSpecialist int32
--- @field BleedingImmunity boolean
--- @field BlessedImmunity boolean
--- @field BlindImmunity boolean
--- @field Brewmaster int32
--- @field BurnContact boolean
--- @field BurnImmunity boolean
--- @field Charges int32
--- @field Charm int32
--- @field CharmImmunity boolean
--- @field ChickenImmunity boolean
--- @field ChillContact boolean
--- @field ChilledImmunity boolean
--- @field ClairvoyantImmunity boolean
--- @field Crafting int32
--- @field CrippledImmunity boolean
--- @field CursedImmunity boolean
--- @field DamageTypeOverwrite StatsDamageType
--- @field DecayingImmunity boolean
--- @field DeflectProjectiles boolean
--- @field DeltaMods FixedString[]
--- @field DisarmedImmunity boolean
--- @field DiseasedImmunity boolean
--- @field DrunkImmunity boolean
--- @field DualWielding int32
--- @field Durability uint32
--- @field DurabilityCounter uint32
--- @field DynamicStats CDivinityStatsEquipmentAttributes[]
--- @field EarthSpecialist int32
--- @field EnragedImmunity boolean
--- @field EntangledContact boolean
--- @field EquipmentType StatsEquipmentStatsType
--- @field FearImmunity boolean
--- @field FireSpecialist int32
--- @field Floating boolean
--- @field FreezeContact boolean
--- @field FreezeImmunity boolean
--- @field GameObject IGameObject
--- @field Grounded boolean
--- @field HasModifiedSkills boolean
--- @field HastedImmunity boolean
--- @field IgnoreClouds boolean
--- @field IgnoreCursedOil boolean
--- @field InfectiousDiseasedImmunity boolean
--- @field Intimidate int32
--- @field InvisibilityImmunity boolean
--- @field IsIdentified uint32
--- @field IsTwoHanded boolean
--- @field ItemSlot StatsItemSlot32
--- @field ItemType StatsEquipmentStatsType
--- @field ItemTypeReal FixedString
--- @field KnockdownImmunity boolean
--- @field Leadership int32
--- @field LevelGroupIndex uint8
--- @field LootableWhenEquipped boolean
--- @field Loremaster int32
--- @field LoseDurabilityOnCharacterHit boolean
--- @field Luck int32
--- @field MadnessImmunity boolean
--- @field MagicArmorMastery int32
--- @field MagicalSulfur boolean
--- @field MaxCharges int32
--- @field MuteImmunity boolean
--- @field NameCool uint32
--- @field NameGroupIndex uint8
--- @field NameIndex uint8
--- @field Necromancy int32
--- @field PainReflection int32
--- @field Perseverance int32
--- @field Persuasion int32
--- @field PetrifiedImmunity boolean
--- @field PhysicalArmorMastery int32
--- @field Pickpocket int32
--- @field PickpocketableWhenEquipped boolean
--- @field PoisonContact boolean
--- @field PoisonImmunity boolean
--- @field Polymorph int32
--- @field ProtectFromSummon boolean
--- @field Ranged int32
--- @field RangerLore int32
--- @field Rarity FixedString
--- @field Reason int32
--- @field Reflexes int32
--- @field RegeneratingImmunity boolean
--- @field Repair int32
--- @field RogueLore int32
--- @field RootGroupIndex uint8
--- @field Runecrafting int32
--- @field Sentinel int32
--- @field ShacklesOfPainImmunity boolean
--- @field Shield int32
--- @field ShockedImmunity boolean
--- @field ShouldSyncStats boolean
--- @field SingleHanded int32
--- @field Skills FixedString
--- @field SleepingImmunity boolean
--- @field SlippingImmunity boolean
--- @field SlowedImmunity boolean
--- @field Sneaking int32
--- @field Sourcery int32
--- @field StunContact boolean
--- @field StunImmunity boolean
--- @field SuffocatingImmunity boolean
--- @field Sulfurology int32
--- @field Summoning int32
--- @field TALENT_ActionPoints boolean
--- @field TALENT_ActionPoints2 boolean
--- @field TALENT_AirSpells boolean
--- @field TALENT_Ambidextrous boolean
--- @field TALENT_AnimalEmpathy boolean
--- @field TALENT_AttackOfOpportunity boolean
--- @field TALENT_AvoidDetection boolean
--- @field TALENT_Awareness boolean
--- @field TALENT_Backstab boolean
--- @field TALENT_BeastMaster boolean
--- @field TALENT_Bully boolean
--- @field TALENT_Carry boolean
--- @field TALENT_ChanceToHitMelee boolean
--- @field TALENT_ChanceToHitRanged boolean
--- @field TALENT_Charm boolean
--- @field TALENT_Courageous boolean
--- @field TALENT_Criticals boolean
--- @field TALENT_Damage boolean
--- @field TALENT_DeathfogResistant boolean
--- @field TALENT_Demon boolean
--- @field TALENT_DualWieldingDodging boolean
--- @field TALENT_Durability boolean
--- @field TALENT_Dwarf_Sneaking boolean
--- @field TALENT_Dwarf_Sturdy boolean
--- @field TALENT_EarthSpells boolean
--- @field TALENT_ElementalAffinity boolean
--- @field TALENT_ElementalRanger boolean
--- @field TALENT_Elementalist boolean
--- @field TALENT_Elf_CorpseEating boolean
--- @field TALENT_Elf_Lore boolean
--- @field TALENT_Escapist boolean
--- @field TALENT_Executioner boolean
--- @field TALENT_ExpGain boolean
--- @field TALENT_ExtraSkillPoints boolean
--- @field TALENT_ExtraStatPoints boolean
--- @field TALENT_FaroutDude boolean
--- @field TALENT_FireSpells boolean
--- @field TALENT_FiveStarRestaurant boolean
--- @field TALENT_Flanking boolean
--- @field TALENT_FolkDancer boolean
--- @field TALENT_Gladiator boolean
--- @field TALENT_GoldenMage boolean
--- @field TALENT_GreedyVessel boolean
--- @field TALENT_Haymaker boolean
--- @field TALENT_Human_Civil boolean
--- @field TALENT_Human_Inventive boolean
--- @field TALENT_IceKing boolean
--- @field TALENT_IncreasedArmor boolean
--- @field TALENT_Indomitable boolean
--- @field TALENT_Initiative boolean
--- @field TALENT_Intimidate boolean
--- @field TALENT_InventoryAccess boolean
--- @field TALENT_ItemCreation boolean
--- @field TALENT_ItemMovement boolean
--- @field TALENT_Jitterbug boolean
--- @field TALENT_Kickstarter boolean
--- @field TALENT_Leech boolean
--- @field TALENT_LightStep boolean
--- @field TALENT_LightningRod boolean
--- @field TALENT_LivingArmor boolean
--- @field TALENT_Lizard_Persuasion boolean
--- @field TALENT_Lizard_Resistance boolean
--- @field TALENT_Lockpick boolean
--- @field TALENT_LoneWolf boolean
--- @field TALENT_Luck boolean
--- @field TALENT_MagicCycles boolean
--- @field TALENT_MasterThief boolean
--- @field TALENT_Max boolean
--- @field TALENT_Memory boolean
--- @field TALENT_MrKnowItAll boolean
--- @field TALENT_NaturalConductor boolean
--- @field TALENT_NoAttackOfOpportunity boolean
--- @field TALENT_None boolean
--- @field TALENT_PainDrinker boolean
--- @field TALENT_Perfectionist boolean
--- @field TALENT_Politician boolean
--- @field TALENT_Quest_GhostTree boolean
--- @field TALENT_Quest_Rooted boolean
--- @field TALENT_Quest_SpidersKiss_Int boolean
--- @field TALENT_Quest_SpidersKiss_Null boolean
--- @field TALENT_Quest_SpidersKiss_Per boolean
--- @field TALENT_Quest_SpidersKiss_Str boolean
--- @field TALENT_Quest_TradeSecrets boolean
--- @field TALENT_QuickStep boolean
--- @field TALENT_Rager boolean
--- @field TALENT_Raistlin boolean
--- @field TALENT_RangerLoreArrowRecover boolean
--- @field TALENT_RangerLoreEvasionBonus boolean
--- @field TALENT_RangerLoreRangedAPBonus boolean
--- @field TALENT_Reason boolean
--- @field TALENT_Repair boolean
--- @field TALENT_ResistDead boolean
--- @field TALENT_ResistFear boolean
--- @field TALENT_ResistKnockdown boolean
--- @field TALENT_ResistPoison boolean
--- @field TALENT_ResistSilence boolean
--- @field TALENT_ResistStun boolean
--- @field TALENT_ResurrectExtraHealth boolean
--- @field TALENT_ResurrectToFullHealth boolean
--- @field TALENT_RogueLoreDaggerAPBonus boolean
--- @field TALENT_RogueLoreDaggerBackStab boolean
--- @field TALENT_RogueLoreGrenadePrecision boolean
--- @field TALENT_RogueLoreHoldResistance boolean
--- @field TALENT_RogueLoreMovementBonus boolean
--- @field TALENT_Sadist boolean
--- @field TALENT_Scientist boolean
--- @field TALENT_Sight boolean
--- @field TALENT_Soulcatcher boolean
--- @field TALENT_Sourcerer boolean
--- @field TALENT_SpillNoBlood boolean
--- @field TALENT_StandYourGround boolean
--- @field TALENT_Stench boolean
--- @field TALENT_SurpriseAttack boolean
--- @field TALENT_Throwing boolean
--- @field TALENT_Torturer boolean
--- @field TALENT_Trade boolean
--- @field TALENT_Unstable boolean
--- @field TALENT_ViolentMagic boolean
--- @field TALENT_Vitality boolean
--- @field TALENT_WalkItOff boolean
--- @field TALENT_WandCharge boolean
--- @field TALENT_WarriorLoreGrenadeRange boolean
--- @field TALENT_WarriorLoreNaturalArmor boolean
--- @field TALENT_WarriorLoreNaturalHealth boolean
--- @field TALENT_WarriorLoreNaturalResistance boolean
--- @field TALENT_WaterSpells boolean
--- @field TALENT_WeatherProof boolean
--- @field TALENT_WhatARush boolean
--- @field TALENT_WildMag boolean
--- @field TALENT_Zombie boolean
--- @field TauntedImmunity boolean
--- @field Telekinesis int32
--- @field Thievery int32
--- @field ThrownImmunity boolean
--- @field Torch boolean
--- @field TwoHanded int32
--- @field Unbreakable boolean
--- @field Unrepairable boolean
--- @field Unstorable boolean
--- @field VitalityMastery int32
--- @field Wand int32
--- @field WarmImmunity boolean
--- @field WarriorLore int32
--- @field WaterSpecialist int32
--- @field WeakImmunity boolean
--- @field WeaponRange uint32
--- @field WeaponType StatsWeaponType
--- @field WebImmunity boolean
--- @field WetImmunity boolean


--- @class CacheTemplateManagerBase
--- @field NewTemplates GameObjectTemplate[]
--- @field RefCountsByHandle table<TemplateHandle, uint32>
--- @field TemplateManagerType TemplateType
--- @field Templates table<FixedString, GameObjectTemplate>
--- @field TemplatesByHandle table<TemplateHandle, GameObjectTemplate>


--- @class CharacterSkillData
--- @field AIParams SkillAIParams
--- @field SkillId FixedString


--- @class CharacterTemplate:EoCGameObjectTemplate
--- @field ActivationGroupId FixedString
--- @field AvoidTraps boolean
--- @field BloodSurfaceType SurfaceType
--- @field CanBeTeleported boolean
--- @field CanClimbLadders boolean
--- @field CanOpenDoors boolean
--- @field CanShootThrough boolean
--- @field ClimbAttachSpeed number
--- @field ClimbDetachSpeed number
--- @field ClimbLoopSpeed number
--- @field CombatComponent CombatComponentTemplate
--- @field CombatTemplate CombatComponentTemplate
--- @field CoverAmount uint8
--- @field DefaultDialog FixedString
--- @field DefaultState uint32
--- @field EmptyVisualSet boolean
--- @field Equipment FixedString
--- @field EquipmentClass uint32
--- @field ExplodedResourceID FixedString
--- @field ExplosionFX FixedString
--- @field Floating boolean
--- @field FootstepWeight uint32
--- @field ForceUnsheathSkills boolean
--- @field GeneratePortrait string
--- @field GhostTemplate FixedString
--- @field HardcoreOnly boolean
--- @field HitFX FixedString
--- @field Icon FixedString
--- @field InfluenceTreasureLevel boolean
--- @field InventoryType uint32
--- @field IsArenaChampion boolean
--- @field IsEquipmentLootable boolean
--- @field IsHuge boolean
--- @field IsLootable boolean
--- @field IsPlayer boolean
--- @field ItemList InventoryItemData[]
--- @field JumpUpLadders boolean
--- @field LevelOverride int32
--- @field LightID FixedString
--- @field NoRotate boolean
--- @field NotHardcore boolean
--- @field OnDeathActions IActionData[]
--- @field PickingPhysicsTemplates table<FixedString, FixedString>
--- @field RagdollTemplate FixedString
--- @field RunSpeed number
--- @field SkillList CharacterSkillData[]
--- @field SkillSet FixedString
--- @field SoftBodyCollisionTemplate FixedString
--- @field SoundAttachBone FixedString
--- @field SoundAttenuation int16
--- @field SoundInitEvent FixedString
--- @field SpeakerGroup string
--- @field SpotSneakers boolean
--- @field Stats FixedString
--- @field TradeTreasures FixedString[]
--- @field Treasures FixedString[]
--- @field TrophyID FixedString
--- @field VisualSet VisualSet
--- @field VisualSetIndices VisualSetIndices
--- @field VisualSetResourceID FixedString
--- @field WalkSpeed number
--- @field WalkThrough boolean
--- @field GetColorChoices fun(self:CharacterTemplate, slot:VisualTemplateColorIndex):uint32[]
--- @field GetVisualChoices fun(self:CharacterTemplate, slot:VisualTemplateVisualIndex):FixedString[]


--- @class CombatComponentTemplate
--- @field Alignment FixedString
--- @field CanFight boolean
--- @field CanJoinCombat boolean
--- @field CombatGroupID FixedString
--- @field IsBoss boolean
--- @field IsInspector boolean
--- @field StartCombatRange number


--- @class ComponentHandleWithType
--- @field Handle ComponentHandle
--- @field TypeId int64


--- @class ConstrainActionData:IActionData
--- @field Damage number


--- @class ConsumeActionData:IActionData
--- @field Consume boolean
--- @field StatsId FixedString


--- @class CraftActionData:IActionData
--- @field CraftingStationType CraftingStationType


--- @class CreatePuddleActionData:IActionData
--- @field CellAtGrow int32
--- @field ExternalCauseAsSurfaceOwner boolean
--- @field GrowTimer number
--- @field LifeTime number
--- @field SurfaceType SurfaceType
--- @field Timeout number
--- @field TotalCells int32


--- @class CreateSurfaceActionData:IActionData
--- @field ExternalCauseAsSurfaceOwner boolean
--- @field LifeTime number
--- @field Radius number
--- @field SurfaceType SurfaceType


--- @class CrimeAreaTriggerData:ITriggerData
--- @field CrimeArea int16


--- @class CrimeRegionTriggerData:ITriggerData
--- @field TriggerRegion FixedString


--- @class CustomDamageTypeCallbacks
--- @field DamageTypeId uint32
--- @field GetArmorDamageCallback RegistryEntry|nil
--- @field GetDamageBoostCallback RegistryEntry|nil
--- @field GetMagicArmorDamageCallback RegistryEntry|nil
--- @field GetResistanceCallback RegistryEntry|nil


--- @class CustomDamageTypeDescriptor
--- @field AmountAndTypeText string
--- @field AmountAndTypeTextHandle FixedString
--- @field Callbacks CustomDamageTypeCallbacks
--- @field ColorCode int32
--- @field DamageTypeId uint32
--- @field Description string
--- @field DescriptionHandle FixedString
--- @field EnumLabel FixedString
--- @field Name string
--- @field NameExtended string
--- @field NameExtendedHandle FixedString
--- @field NameHandle FixedString


--- @class DefaultCameraController:RfCameraController
--- @field CameraMoveSpeed number
--- @field IsActive boolean
--- @field MoveSpeedRightVector number
--- @field MoveSpeedUpVector number
--- @field SomePositionX number
--- @field SomePositionY number
--- @field field_364 int32


--- @class DefaultServerOnlyTriggerData:ITriggerData


--- @class DefaultSyncedTriggerData:ITriggerData


--- @class DeferredLoadableResource:Resource


--- @class DestroyParametersData:IActionData
--- @field ExplodeFX FixedString
--- @field TemplateAfterDestruction FixedString
--- @field VisualDestruction FixedString


--- @class DisarmTrapActionData:IActionData
--- @field Consume boolean


--- @class DoorActionData:IActionData
--- @field SecretDoor boolean


--- @class DragDropManager
--- @field PlayerDragDrops table<int16, DragDropManagerPlayerDragInfo>
--- @field StartDraggingName fun(self:DragDropManager, playerId:int16, objectId:FixedString):boolean
--- @field StartDraggingObject fun(self:DragDropManager, playerId:int16, objectHandle:ComponentHandle):boolean
--- @field StopDragging fun(self:DragDropManager, playerId:int16):boolean


--- @class DragDropManagerPlayerDragInfo
--- @field DragId FixedString
--- @field DragObject ComponentHandle
--- @field IsActive boolean
--- @field IsDragging boolean
--- @field MousePos vec2


--- @class EffectCameraController:DefaultCameraController
--- @field LookAtShake vec3
--- @field Wobble number
--- @field Wobble2 number


--- @class EoCGameObjectTemplate:GameObjectTemplate
--- @field AIBoundsAIType uint8
--- @field AIBoundsHeight number
--- @field AIBoundsMax vec3
--- @field AIBoundsMin vec3
--- @field AIBoundsRadius number
--- @field DisplayName TranslatedString
--- @field FadeGroup FixedString
--- @field FadeIn boolean
--- @field Fadeable boolean
--- @field GameMasterSpawnSection int32
--- @field GameMasterSpawnSubSection TranslatedString
--- @field Opacity number
--- @field SeeThrough boolean


--- @class EventTriggerData:ITriggerData
--- @field EnterEvent FixedString
--- @field LeaveEvent FixedString


--- @class ExplorationTriggerData:ITriggerData
--- @field ExplorationReward int32


--- @class FireEventDesc
--- @field DeviceId int16
--- @field Event InputEvent
--- @field EventDesc InputEventDesc
--- @field PlayerIndex int32


--- @class GameObjectTemplate
--- @field AllowReceiveDecalWhenAnimated boolean
--- @field CameraOffset vec3
--- @field CastShadow boolean
--- @field FileName Path
--- @field Flags GameObjectTemplateFlags
--- @field GroupID uint32
--- @field Handle uint32
--- @field HasGameplayValue boolean
--- @field HasParentModRelation boolean
--- @field Id FixedString
--- @field IsDeleted boolean
--- @field IsGlobal boolean
--- @field IsReflecting boolean
--- @field IsShadowProxy boolean
--- @field LevelName FixedString
--- @field ModFolder FixedString
--- @field Name string
--- @field NonUniformScale boolean
--- @field PhysicsTemplate FixedString
--- @field ReceiveDecal boolean
--- @field RenderChannel uint8
--- @field RootTemplate FixedString
--- @field Tags FixedString[]
--- @field Transform Transform
--- @field Type TemplateType
--- @field VisualTemplate FixedString


--- @class GameTime
--- @field DeltaTime number
--- @field Ticks int32
--- @field Time number


--- @class GlobalCacheTemplateManager:CacheTemplateManagerBase


--- @class GlobalCameraSwitches
--- @field BaseCameraDistance number
--- @field BaseTargetCameraDistance number
--- @field CamAvoidanceOffset number
--- @field CamAvoidanceOffsetWithTarget number
--- @field CamAvoidancePower number
--- @field CamAvoidancePowerWithTarget number
--- @field CameraAvoidanceMultiplier number
--- @field CameraAvoidanceMultiplierWithTarget number
--- @field DefaultCameraDistance number
--- @field DefaultCameraDistanceController number
--- @field DefaultCameraDistanceOverhead number
--- @field DefaultCameraDistanceWithTarget number
--- @field DefaultCombatPosition vec3
--- @field DefaultCombatPosition2 vec3
--- @field DefaultControllerPosition vec3
--- @field DefaultControllerPosition2 vec3
--- @field DefaultOverheadPosition vec3
--- @field DefaultOverheadPosition2 vec3
--- @field DefaultPosition vec3
--- @field DefaultPosition2 vec3
--- @field DistanceAdjustmentSpeedMultiplier number
--- @field DistanceAdjustmentSpeedMultiplier2 number
--- @field DistanceAdjustmentSpeedPower number
--- @field DistanceAdjustmentThreshold number
--- @field FOV number
--- @field FOVController number
--- @field LookAtDistanceThreshold number
--- @field LookAtSpeedPower number
--- @field LookAtSpeedPower2 number
--- @field MaxCameraDistance number
--- @field MaxCameraDistanceController number
--- @field MaxCameraDistanceOverhead number
--- @field MaxCameraDistanceWithTarget number
--- @field MinCameraDistance number
--- @field MinCameraDistanceController number
--- @field MinCameraDistanceOverhead number
--- @field MinCameraDistanceWithTarget number
--- @field MoveSpeed number
--- @field TargetHeightOffset number
--- @field TargetHeightOffsetController number
--- @field ViewportFrameCursorThreshold1 number
--- @field ViewportFrameCursorThreshold2 number
--- @field field_44 number
--- @field field_48 number


--- @class GlobalSwitches
--- @field AIBoundsSizeMultiplier number
--- @field AddGenericKeyWords boolean
--- @field AddStoryKeyWords boolean
--- @field AiLevelScaleScores boolean
--- @field AiUsePositionScores boolean
--- @field AllowMovementFreePointer boolean
--- @field AllowXPGain boolean
--- @field AlwaysShowSplitterInTrade boolean
--- @field ArenaCharacterHighlightFlag boolean
--- @field ArenaCharacterHighlightMode int32
--- @field AutoFillHotbarCategories uint8
--- @field AutoRemoveHotbarSkills boolean
--- @field CacheDialogs boolean
--- @field CameraSpeedMultiplier number
--- @field CameraSwitchesMode0 GlobalCameraSwitches
--- @field CameraSwitchesMode1 GlobalCameraSwitches
--- @field CanAutoSave boolean
--- @field ChatLanguage string
--- @field CheckRequirements boolean
--- @field Cleave_M boolean
--- @field CombatCaracterHighlightFlag boolean
--- @field CombatCharacterHighlightMode int32
--- @field ControllerCharacterRunThreshold number
--- @field ControllerCharacterWalkThreshold number
--- @field ControllerLayout uint32
--- @field ControllerMoveSweepCone number
--- @field ControllerSensitivity int32
--- @field ControllerStickDeadZone int32
--- @field ControllerStickPressDeadZone int32
--- @field ControllerTriggerDeadZone int32
--- @field DebugItemColorOverride FixedString
--- @field DebugViewType int32
--- @field Difficulty uint8
--- @field DirectConnectAddress string
--- @field DisableArmorSavingThrows boolean
--- @field DisableEdgePanning boolean
--- @field DisableLocalMessagePassing boolean
--- @field DisableStoryPatching boolean
--- @field DoUnlearnCheck boolean
--- @field DualDialogsEnabled boolean
--- @field EnableAiLogging boolean
--- @field EnableAiThinking boolean
--- @field EnableAngularCulling boolean
--- @field EnableBlending boolean
--- @field EnableGameOver boolean
--- @field EnableGenome boolean
--- @field EnableModuleHashing boolean
--- @field EnablePortmapping boolean
--- @field EnableSoundErrorLogging boolean
--- @field EnableSteamP2P boolean
--- @field EnableVoiceLogging boolean
--- @field EscClosesAllUI boolean
--- @field FadeCharacters boolean
--- @field Fading boolean
--- @field FileLoadingLog boolean
--- @field FileSavingLog boolean
--- @field Floats3 number[]
--- @field ForcePort uint16
--- @field ForceSplitscreen boolean
--- @field ForceStoryPatching boolean
--- @field GameCamAvoidScenery boolean
--- @field GameCameraControllerMode boolean
--- @field GameCameraEnableCloseUpDialog boolean
--- @field GameCameraEnableDynamicCombatCamera boolean
--- @field GameCameraRotation number
--- @field GameCameraRotationLocked boolean
--- @field GameCameraShakeEnabled boolean
--- @field GameMasterBind int32
--- @field GameMode uint8
--- @field GameVisibilityDirect int32
--- @field GameVisibilityLAN int32
--- @field GameVisibilityOnline int32
--- @field GodMode boolean
--- @field LoadAllEffectPools boolean
--- @field LoadScenery boolean
--- @field LoadTextShaders boolean
--- @field LogSaveLoadErrors boolean
--- @field MaxAmountDialogsInLog int32
--- @field MaxNrOfAutoSaves uint32
--- @field MaxNrOfQuickSaves uint32
--- @field MaxNrOfReloadSaves uint32
--- @field MaxRotateSpeed number
--- @field MouseLock boolean
--- @field MouseScrollSensitivity int32
--- @field MouseSensitivity int32
--- @field MoveDirectionCount uint64
--- @field MuteSoundWhenNotFocused boolean
--- @field NextServerMode int32
--- @field NodeWaitTimeMultiplier number
--- @field NrOfAutoSaves uint32
--- @field NrOfQuickSaves uint32
--- @field NrOfReloadSaves uint32
--- @field Options2 GlobalSwitchesSomeOption[]
--- @field OverheadZoomEnabled boolean
--- @field OverheadZoomModifier number
--- @field PeaceCharacterHighlightFlag boolean
--- @field PeaceCharacterHighlightMode int32
--- @field PeaceMode boolean
--- @field ResetTutorialsOnNewGame boolean
--- @field RotateMinimap boolean
--- @field RotateRampSpeed number
--- @field ScriptLog boolean
--- @field ServerFrameCap uint32
--- @field ServerMode int32
--- @field ServerMonitor boolean
--- @field ShorOriginIntroInCC boolean
--- @field ShowBuildVersion boolean
--- @field ShowCharacterCreation boolean
--- @field ShowCloths boolean
--- @field ShowDebugLines boolean
--- @field ShowDrawStats boolean
--- @field ShowFPS boolean
--- @field ShowLocalizationMarkers boolean
--- @field ShowOverheadDialog boolean
--- @field ShowOverheadText boolean
--- @field ShowPhysXBoxes boolean
--- @field ShowPhysics boolean
--- @field ShowRagdollInfo boolean
--- @field ShowRaycasting boolean
--- @field ShowSpeechBubble boolean
--- @field ShowSubtitles boolean
--- @field ShroudEnabled boolean
--- @field SomeConditionalStoryLogField boolean
--- @field SomePhysXRagdollFlag boolean
--- @field SoundPartyLosingThreshold int32
--- @field SoundPartyWinningThreshold int32
--- @field SoundQuality int32
--- @field SoundSettings GlobalSwitchesSoundSetting[]
--- @field StatsArgPassed boolean
--- @field Story boolean
--- @field StoryEvents boolean
--- @field StoryLog boolean
--- @field TacticalCharacterHighlightFlag boolean
--- @field TacticalCharacterHighlightMode int32
--- @field TexelDensityExtreme number
--- @field TexelDensityIdeal number
--- @field TexelDensityMax number
--- @field TexelDensityMin number
--- @field TutorialBoxMode int32
--- @field UIScaling number
--- @field UpdateInvisibilityOverlayMaterials_M boolean
--- @field UpdateOffstageOverlayMaterials_M boolean
--- @field UpdatePhysXScene boolean
--- @field UpdateScene boolean
--- @field UseEndTurnFallback boolean
--- @field UseLevelCache boolean
--- @field UseRadialContextMenu boolean
--- @field VisualizeTextures boolean
--- @field WeaponRangeMultiplier number
--- @field YieldOnLostFocus boolean
--- @field field_11 boolean
--- @field field_14 boolean
--- @field field_16 boolean
--- @field field_19 boolean
--- @field field_1B boolean
--- @field field_1D boolean
--- @field field_1E boolean
--- @field field_1F boolean
--- @field field_20 boolean
--- @field field_21 boolean
--- @field field_3D boolean
--- @field field_63 boolean
--- @field field_89 boolean
--- @field field_BB0 number
--- @field field_BF4 int32
--- @field field_C0 int32
--- @field field_C1A boolean
--- @field field_C1B boolean
--- @field field_C8 boolean
--- @field field_C9 boolean
--- @field field_CC boolean
--- @field field_CD boolean
--- @field field_CE boolean
--- @field field_CF boolean
--- @field field_E boolean
--- @field field_E8 boolean


--- @class GlobalSwitchesSomeOption
--- @field A number
--- @field B number
--- @field C number
--- @field D number


--- @class GlobalSwitchesSoundSetting
--- @field ConfigKey FixedString
--- @field Name TranslatedString
--- @field PreviewKeyName string
--- @field RTPCKeyName string
--- @field Value number
--- @field Value2 number
--- @field Value3 number


--- @class GraphicSettings
--- @field AnimationAllowedPixelError int32
--- @field AntiAliasing int32
--- @field BloomEnabled boolean
--- @field ClothGPUAcceleration boolean
--- @field DOFEnabled boolean
--- @field EnableLightAssignmentStage boolean
--- @field EnableSSR boolean
--- @field EnableSpotLightsSMAA boolean
--- @field FakeFullscreenEnabled boolean
--- @field FrameCapEnabled boolean
--- @field FrameCapFPS int16
--- @field Fullscreen boolean
--- @field GammaCorrection number
--- @field GodRaysEnabled boolean
--- @field HDRGamma number
--- @field HDRMaxNits number
--- @field HDRPaperWhite number
--- @field LensFlareEnabled boolean
--- @field LightingDetail int32
--- @field MaxDrawDistance number
--- @field MaxDrawDistance2 number
--- @field ModelDetail int32
--- @field MonitorIndex int32
--- @field MotionBlurEnabled boolean
--- @field PointLightShadowsEnabled boolean
--- @field PostProcessingInjectTexture boolean
--- @field RefreshRateDenominator int32
--- @field RefreshRateNumerator int32
--- @field RenderMT boolean
--- @field SSAOEnabled boolean
--- @field ScreenHeight int32
--- @field ScreenWidth int32
--- @field ShadowQuality int32
--- @field ShadowsEnabled boolean
--- @field ShowHDRCalibration boolean
--- @field TextureDetail int32
--- @field TextureFiltering int32
--- @field TextureStreamingEnabled boolean
--- @field TripleBuffering boolean
--- @field UseForwardRendering boolean
--- @field VSync boolean
--- @field VSyncDivider int32


--- @class HoldRepeatEventDesc
--- @field Acceleration int32
--- @field FireEvent FireEventDesc
--- @field HoldTime number
--- @field PressTime number
--- @field ReleaseTime number


--- @class IActionData
--- @field Type ActionDataType


--- @class IEoCClientObject:IGameObject
--- @field Base BaseComponent
--- @field DisplayName string|nil
--- @field GetStatus fun(self:IEoCClientObject, statusId:FixedString):EclStatus
--- @field GetStatusByType fun(self:IEoCClientObject, type:StatusType):EclStatus
--- @field GetStatusObjects fun(self:IEoCClientObject):EclStatus
--- @field GetStatuses fun(self:IEoCClientObject):FixedString[]


--- @class IEoCClientReplicatedObject:IEoCClientObject
--- @field MyGuid FixedString
--- @field NetID NetId


--- @class IEoCServerObject:IGameObject
--- @field Base BaseComponent
--- @field DisplayName string|nil
--- @field MyGuid FixedString
--- @field NetID NetId
--- @field CreateCacheTemplate fun(self:IEoCServerObject):GameObjectTemplate
--- @field ForceSyncToPeers fun(self:IEoCServerObject)
--- @field GetStatus fun(self:IEoCServerObject, statusId:FixedString):EsvStatus
--- @field GetStatusByHandle fun(self:IEoCServerObject, handle:ComponentHandle):EsvStatus
--- @field GetStatusByType fun(self:IEoCServerObject, type:StatusType):EsvStatus
--- @field GetStatusObjects fun(self:IEoCClientObject):EclStatus
--- @field GetStatuses fun(self:IEoCServerObject):FixedString[]
--- @field TransformTemplate fun(self:IEoCServerObject, tmpl:GameObjectTemplate)


--- @class IGameObject
--- @field Handle ComponentHandle
--- @field Height number
--- @field Rotation mat3
--- @field Scale number
--- @field Translate vec3
--- @field Velocity vec3
--- @field Visual Visual
--- @field GetTags fun(self:IGameObject):FixedString[]
--- @field HasTag fun(self:IGameObject, a1:FixedString):boolean
--- @field IsTagged fun(self:IGameObject, tag:FixedString):boolean


--- @class ITriggerData


--- @class IdentifyActionData:IActionData
--- @field Consume boolean


--- @class InjectDeviceEvent
--- @field DeviceId uint16
--- @field EventId uint32


--- @class InjectInputData:InputRawChange


--- @class InjectTextData
--- @field DeviceId int16


--- @class InputBinding:InputRaw
--- @field Alt boolean
--- @field Ctrl boolean
--- @field Gui boolean
--- @field Modifiers InputModifier
--- @field Shift boolean


--- @class InputBindingDesc
--- @field Binding InputBinding
--- @field BindingIndex int32
--- @field InputId InputRawType
--- @field IsGlobal boolean
--- @field PlayerIndex int32


--- @class InputDevice
--- @field ControllerMapping int32
--- @field DeviceId int16
--- @field InputPlayerIndex int64
--- @field field_14 number[]
--- @field field_24 uint8
--- @field field_8 int32


--- @class InputEvent
--- @field AcceleratedRepeat boolean
--- @field ClearOnRelease boolean
--- @field DeviceId FixedString
--- @field EventId int32
--- @field Hold boolean
--- @field InputPlayerIndex int32
--- @field NewValue InputValue
--- @field OldValue InputValue
--- @field Press boolean
--- @field Release boolean
--- @field Repeat boolean
--- @field ValueChange boolean
--- @field WasPreferred boolean


--- @class InputEventDesc
--- @field CategoryName FixedString
--- @field EventDesc TranslatedString
--- @field EventID int32
--- @field EventName string
--- @field Flags1 uint8
--- @field Type InputType
--- @field field_E8 int32


--- @class InputEventText
--- @field PlayerId int32
--- @field Text string
--- @field TextLength uint64


--- @class InputManager
--- @field Alt boolean
--- @field ControllerAllowKeyboardMouseInput boolean
--- @field Ctrl boolean
--- @field CurrentRemap InputSchemeBinding
--- @field DeviceEventInjects InjectDeviceEvent[]
--- @field Gui boolean
--- @field InputDefinitions table<int32, InputEventDesc>
--- @field InputInjects InjectInputData[]
--- @field InputScheme InputScheme
--- @field InputStates InputSetState[]
--- @field LastUpdateTime number
--- @field PerDeviceData InputDevice[]
--- @field PerPlayerHoldRepeatEvents table<int32, InputManagerHoldRepeatEvent>
--- @field PlayerDeviceIDs int16[]
--- @field PlayerDevices int32[]
--- @field PressedModifiers InputModifier
--- @field RawInputs InputRaw[]
--- @field RawInputs2 InputRaw[]
--- @field Shift boolean


--- @class InputManagerHoldRepeatEvent
--- @field HoldEvents FireEventDesc[]
--- @field HoldRepeatEvents HoldRepeatEventDesc[]
--- @field PressEvents FireEventDesc[]
--- @field ReleaseEvents FireEventDesc[]
--- @field RepeatEvents FireEventDesc[]
--- @field ValueChangeEvents FireEventDesc[]


--- @class InputRaw
--- @field DeviceId "Key"|"Mouse"|"C"|"Touchbar"|"Unknown"
--- @field InputId InputRawType


--- @class InputRawChange
--- @field Input InputRaw
--- @field Value InputValue


--- @class InputScheme
--- @field Bindings InputSchemeBindingSet[]
--- @field DeviceIdToPlayerId table<uint16, int32>
--- @field PerPlayerBindings table<int32, InputBinding[]>[]


--- @class InputSchemeBinding
--- @field Binding InputBinding
--- @field PlayerId int32


--- @class InputSchemeBindingSet
--- @field Bindings InputBindingDesc[]
--- @field Initialized boolean


--- @class InputSetState
--- @field Initialized boolean
--- @field Inputs InputValue[]


--- @class InputValue
--- @field State InputState
--- @field Value number
--- @field Value2 number


--- @class InventoryItemData
--- @field AIParams SkillAIParams
--- @field Amount int32
--- @field ItemName string
--- @field LevelName string
--- @field TemplateID FixedString
--- @field Type int32
--- @field UUID FixedString
--- @field field_10 string


--- @class ItemTemplate:EoCGameObjectTemplate
--- @field ActivationGroupId FixedString
--- @field AllowSummonTeleport boolean
--- @field AltSpeaker FixedString
--- @field Amount int32
--- @field BloodSurfaceType int32
--- @field CanBeMoved boolean
--- @field CanBePickedUp boolean
--- @field CanClickThrough boolean
--- @field CanShootThrough boolean
--- @field CombatComponent CombatComponentTemplate
--- @field CoverAmount uint8
--- @field DefaultState FixedString
--- @field Description TranslatedString
--- @field Destroyed boolean
--- @field DropSound FixedString
--- @field EquipSound FixedString
--- @field Equipment ItemTemplateEquipment
--- @field Floating boolean
--- @field FreezeGravity boolean
--- @field HardcoreOnly boolean
--- @field HitFX FixedString
--- @field Hostile boolean
--- @field Icon FixedString
--- @field InventoryMoveSound FixedString
--- @field IsBlocker boolean
--- @field IsHuge boolean
--- @field IsInteractionDisabled boolean
--- @field IsKey boolean
--- @field IsPinnedContainer boolean
--- @field IsPointerBlocker boolean
--- @field IsPublicDomain boolean
--- @field IsSourceContainer boolean
--- @field IsSurfaceBlocker boolean
--- @field IsSurfaceCloudBlocker boolean
--- @field IsTrap boolean
--- @field IsWall boolean
--- @field ItemList InventoryItemData[]
--- @field Key FixedString
--- @field LevelOverride int32
--- @field LockLevel int32
--- @field LoopSound FixedString
--- @field MaxStackAmount int32
--- @field MeshProxy FixedString
--- @field NotHardcore boolean
--- @field OnDestroyActions IActionData[]
--- @field OnUseDescription TranslatedString
--- @field OnUsePeaceActions IActionData[]
--- @field Owner FixedString
--- @field PickupSound FixedString
--- @field PinnedContainerTags FixedString[]
--- @field Race uint32
--- @field SoundAttachBone FixedString
--- @field SoundAttenuation int16
--- @field SoundInitEvent FixedString
--- @field Speaker FixedString
--- @field SpeakerGroup string
--- @field Stats FixedString
--- @field StoryItem boolean
--- @field Tooltip uint32
--- @field TreasureLevel int32
--- @field TreasureOnDestroy boolean
--- @field Treasures FixedString[]
--- @field UnequipSound FixedString
--- @field Unimportant boolean
--- @field UnknownDescription TranslatedString
--- @field UnknownDisplayName TranslatedString
--- @field UseOnDistance boolean
--- @field UsePartyLevelForTreasureLevel boolean
--- @field UseRemotely boolean
--- @field UseSound FixedString
--- @field Wadable boolean
--- @field WalkOn boolean
--- @field WalkThrough boolean


--- @class ItemTemplateEquipment
--- @field EquipmentSlots uint32
--- @field SyncAnimationWithParent boolean[]
--- @field VisualResources FixedString[]
--- @field VisualSetSlots uint32


--- @class Level
--- @field ActivePersistentLevelTemplates ActivePersistentLevelTemplate[]
--- @field GameObjects IGameObject[]
--- @field LevelDesc LevelDesc
--- @field LocalTemplateManager LocalTemplateManager
--- @field PhysicsScene PhysicsScene


--- @class LevelCacheTemplateManager:CacheTemplateManagerBase
--- @field LevelName FixedString


--- @class LevelDesc
--- @field CustomDisplayLevelName string
--- @field LevelName FixedString
--- @field Paths Path[]
--- @field Type uint8
--- @field UniqueKey FixedString


--- @class LevelTemplate:GameObjectTemplate
--- @field IsPersistent boolean
--- @field LevelBoundTrigger FixedString
--- @field LocalLevelBound Bound
--- @field SubLevelName FixedString
--- @field WorldLevelBound Bound


--- @class Light:MoveableObject
--- @field CastShadow boolean
--- @field Color vec3
--- @field FlickerAmount number
--- @field FlickerSpeed number
--- @field Intensity number
--- @field IntensityOffset number
--- @field IsEnabled boolean
--- @field IsFlickering boolean
--- @field IsMoving boolean
--- @field IsUpdateJobRunning boolean
--- @field LightType int32
--- @field LightVolume boolean
--- @field LightVolumeMapping int32
--- @field LightVolumeSamples int32
--- @field MovementAmount number
--- @field MovementSpeed number
--- @field Radius number
--- @field TranslateOffset vec3
--- @field TranslateOffset2 vec3
--- @field VolumetricLightCollisionProbability number
--- @field VolumetricLightIntensity number


--- @class LocalTemplateManager
--- @field Templates table<FixedString, GameObjectTemplate>
--- @field TemplatesByHandle table<TemplateHandle, GameObjectTemplate>
--- @field TemplatesByType table<uint16, GameObjectTemplate[]>


--- @class LockpickActionData:IActionData
--- @field Consume boolean


--- @class LyingActionData:IActionData
--- @field Heal number


--- @class Material
--- @field BlendState uint8
--- @field DepthState uint8
--- @field Flags uint16
--- @field ForwardLightingMode int32
--- @field MaterialID FixedString
--- @field MaterialParameters MaterialParameters
--- @field MaterialPassHint uint8
--- @field MaterialType int32
--- @field RasterizerState uint8
--- @field ShaderDescs ShaderDesc[]
--- @field ShaderPaths Path[]
--- @field ShadingModel int32
--- @field StencilRef int32
--- @field UVCount int32


--- @class MaterialInfo
--- @field ApplyCurrentNormalMap boolean
--- @field ApplyFlags StatusMaterialApplyFlags
--- @field ApplyOnArmor boolean
--- @field ApplyOnBody boolean
--- @field ApplyOnHorns boolean
--- @field ApplyOnOverhead boolean
--- @field ApplyOnWeapon boolean
--- @field ApplyOnWings boolean
--- @field Flags MaterialInfoFlags
--- @field Force boolean
--- @field GameObjectHandle ComponentHandle
--- @field IsActive boolean
--- @field IsOverlay boolean
--- @field MaterialUUID FixedString
--- @field OverlayOffset number


--- @class MaterialParameter
--- @field Enabled boolean
--- @field Parameter FixedString
--- @field ShaderFlags uint16
--- @field UniformName FixedString


--- @class MaterialParameterWithValue_MaterialSamplerState:MaterialParameter
--- @field Value MaterialSamplerState


--- @class MaterialParameterWithValue_MaterialTexture2D:MaterialParameter
--- @field Value MaterialTexture2D


--- @class MaterialParameterWithValue_MaterialVector3:MaterialParameter
--- @field Value MaterialVector3


--- @class MaterialParameterWithValue_MaterialVector4:MaterialParameter
--- @field Value MaterialVector4


--- @class MaterialParameterWithValue_float:MaterialParameter
--- @field Value number


--- @class MaterialParameterWithValue_Glmvec2:MaterialParameter
--- @field Value vec2


--- @class MaterialParameters
--- @field ParentAppliedMaterial AppliedMaterial
--- @field ParentMaterial Material
--- @field SamplerStates MaterialParameterWithValue_MaterialSamplerState[]
--- @field Scalars MaterialParameterWithValue_float[]
--- @field Texture2Ds MaterialParameterWithValue_MaterialTexture2D[]
--- @field Vector2s MaterialParameterWithValue_Glmvec2[]
--- @field Vector3s MaterialParameterWithValue_MaterialVector3[]
--- @field Vector4s MaterialParameterWithValue_MaterialVector4[]


--- @class MaterialSamplerState
--- @field TextureAddressMode uint8
--- @field TextureFilterOverride uint8


--- @class MaterialTexture2D
--- @field ID FixedString


--- @class MaterialVector3
--- @field IsColor boolean
--- @field Value vec3


--- @class MaterialVector4
--- @field IsColor boolean
--- @field Value vec4


--- @class MeshBinding
--- @field Bound Bound
--- @field Link AnimatableObject
--- @field Transform mat4


--- @class ModManager
--- @field AvailableMods Module[]
--- @field BaseModule Module
--- @field Flag uint8
--- @field Settings ModuleSettings


--- @class Module
--- @field AddonModules Module[]
--- @field BFSReset boolean
--- @field ContainedModules Module[]
--- @field DependentModules Module[]
--- @field FinishedLoading boolean
--- @field HasValidHash boolean
--- @field Info ModuleInfo
--- @field LoadOrderedModules Module[]
--- @field UsesLsfFormat boolean


--- @class ModuleInfo
--- @field Author string
--- @field CharacterCreationLevel FixedString
--- @field Description string
--- @field Directory string
--- @field DisplayDescription TranslatedString
--- @field DisplayName TranslatedString
--- @field GMTemplate FixedString
--- @field Hash string
--- @field LobbyLevel FixedString
--- @field MenuLevel FixedString
--- @field ModVersion Version
--- @field ModuleType FixedString
--- @field ModuleUUID FixedString
--- @field Name string
--- @field NumPlayers uint8
--- @field PhotoBoothLevel FixedString
--- @field PublishVersion Version
--- @field StartLevel FixedString
--- @field Tags string[]
--- @field TargetModes FixedString[]

--- Separate from ModuleInfo, this is a table with specific keys.  
--- @class LegacyModuleInfo
--- @field UUID FixedString
--- @field Name string
--- @field Description string
--- @field Directory string
--- @field ModuleType FixedString
--- @field Version integer
--- @field PublishVersion integer
--- @field Dependencies FixedString[]




--- @class ModuleSettings
--- @field ModOrder FixedString[]
--- @field Mods ModuleShortDesc[]


--- @class ModuleShortDesc
--- @field Folder string
--- @field MD5 string
--- @field ModuleUUID FixedString
--- @field Name string
--- @field Version Version


--- @class MoveableObject
--- @field DirtyFlags uint8
--- @field LocalTransform Transform
--- @field ObjectFlags uint8
--- @field WorldTransform Transform


--- @class MusicVolumeTriggerData:ITriggerData
--- @field MusicEvents MusicVolumeTriggerDataMusicEvent[]


--- @class MusicVolumeTriggerDataMusicEvent
--- @field Bansuri FixedString
--- @field Cello FixedString
--- @field IsStinger boolean
--- @field MusicEvent FixedString
--- @field OriginOnly FixedString
--- @field OriginTheme FixedString
--- @field OriginThemeAddInstrument boolean
--- @field Tambura FixedString
--- @field TriggerOnEnter boolean
--- @field TriggerOnlyOnce boolean
--- @field Ud FixedString


--- @class OverlayMaterialRequest
--- @field ApplyCurrentNormalMap boolean
--- @field ApplyFlags StatusMaterialApplyFlags
--- @field ApplyOnArmor boolean
--- @field ApplyOnBody boolean
--- @field ApplyOnHorns boolean
--- @field ApplyOnOverhead boolean
--- @field ApplyOnWeapon boolean
--- @field ApplyOnWings boolean
--- @field FadeValue number
--- @field Flags MaterialInfoFlags
--- @field Force boolean
--- @field GameObjectHandle ComponentHandle
--- @field IsActive boolean
--- @field IsOverlay boolean
--- @field MaterialUUID FixedString
--- @field OverlayOffset number


--- @class PhysicsObject
--- @field CollidesWith uint32
--- @field GameObject IGameObject
--- @field ManagedScale number
--- @field PhysicsGroups uint32
--- @field Rotate mat3
--- @field Scale number
--- @field Shapes PhysicsShape[]
--- @field TemplateID FixedString
--- @field Translate vec3


--- @class PhysicsRagdoll


--- @class PhysicsScene
--- @field Objects PhysicsObject[]


--- @class PhysicsShape
--- @field Name FixedString
--- @field Rotate mat3
--- @field Scale number
--- @field Translate vec3


--- @class PlaySoundActionData:IActionData
--- @field ActivateSoundEvent FixedString
--- @field PlayOnHUD boolean


--- @class PlayerManager
--- @field FreePlayerIds int16[]
--- @field InputPlayerIndexToPlayerId table<int32, int16>
--- @field NextPlayerId int16
--- @field PlayerIds int16[]
--- @field Players table<int16, PlayerManagerPlayerData>


--- @class PlayerManagerPlayerData
--- @field InputPlayerIndex int32


--- @class ProjectileTemplate:EoCGameObjectTemplate
--- @field Acceleration number
--- @field BeamFX FixedString
--- @field CastBone FixedString
--- @field DestroyTrailFXOnImpact boolean
--- @field DetachBeam boolean
--- @field IgnoreRoof boolean
--- @field ImpactFX FixedString
--- @field ImpactFXSize number
--- @field LifeTime number
--- @field NeedsArrowImpactSFX boolean
--- @field PathMaxArcDist number
--- @field PathMinArcDist number
--- @field PathRadius number
--- @field PathRepeat int32
--- @field PathShift number
--- @field PreviewPathImpactFX FixedString
--- @field PreviewPathMaterial FixedString
--- @field PreviewPathRadius number
--- @field ProjectilePath boolean
--- @field RotateImpact boolean
--- @field Speed number
--- @field TrailFX FixedString


--- @class RecipeActionData:IActionData
--- @field RecipeID FixedString


--- @class RegionTriggerData:ITriggerData
--- @field RegionCameraLock boolean
--- @field RegionCameraLockPos boolean
--- @field RegionShroudGen boolean
--- @field RegionShroudVisibleInWorld boolean


--- @class RenderableObject:MoveableObject
--- @field ActiveAppliedMaterial AppliedMaterial
--- @field AppliedMaterials AppliedMaterial[]
--- @field AppliedOverlayMaterials AppliedMaterial[]
--- @field ClothPhysicsShape PhysicsShape
--- @field HasPhysicsProxy boolean
--- @field IsSimulatedCloth boolean
--- @field LOD uint8
--- @field PropertyList DsePropertyList


--- @class RepairActionData:IActionData
--- @field Consume boolean


--- @class Resource
--- @field IsActive boolean
--- @field IsDirty boolean
--- @field IsLocalized boolean
--- @field IsOriginal boolean
--- @field ModName FixedString
--- @field Name string
--- @field PackageName FixedString
--- @field SourceFile Path
--- @field UUID FixedString


--- @class RuntimeStringHandle
--- @field Handle FixedString
--- @field ReferenceString string


--- @class SecretRegionTriggerData:ITriggerData
--- @field SecretRegionUnlocked boolean


--- @class ShaderDesc
--- @field Flags uint16
--- @field PSHash FixedString
--- @field VSHash FixedString


--- @class ShowStoryElementUIActionData:IActionData
--- @field UIStoryInstance string
--- @field UIType int32


--- @class SitActionData:IActionData
--- @field Heal number


--- @class Skeleton


--- @class SkillAIConditions
--- @field HasNoMagicalArmor boolean
--- @field HasNoPhysicalArmor boolean
--- @field MaximumHealthPercentage int32
--- @field MinimumHealthPercentage int32
--- @field Tags FixedString[]


--- @class SkillAIParams
--- @field AIFlags uint8
--- @field CasualExplorer boolean
--- @field Classic boolean
--- @field HonorHardcore boolean
--- @field MinimumImpact int32
--- @field OnlyCastOnSelf boolean
--- @field ScoreModifier number
--- @field SourceConditions SkillAIConditions
--- @field StartRound int32
--- @field TacticianHardcore boolean
--- @field TargetConditions SkillAIConditions


--- @class SkillBookActionData:IActionData
--- @field Consume boolean
--- @field SkillID FixedString


--- @class SoundComponent:BaseComponent
--- @field EntityName FixedString
--- @field Flags SoundComponentFlags
--- @field LevelName FixedString


--- @class SoundRTPCSync
--- @field RTPC FixedString
--- @field SyncNeeded boolean
--- @field Value number


--- @class SoundResource:DeferredLoadableResource
--- @field DisplayName string
--- @field Duration number
--- @field GMSoundCategory uint8
--- @field GMSubSection string
--- @field Internal boolean
--- @field MaxDistance number
--- @field Preload boolean
--- @field SoundBank FixedString
--- @field SoundCategory uint8
--- @field SoundCodec uint32
--- @field SoundEvent FixedString
--- @field SoundEventID int32
--- @field SoundEventUUID Guid


--- @class SoundStateSync
--- @field State FixedString
--- @field Switch FixedString
--- @field SyncNeeded boolean


--- @class SoundVolumeTrigger:AreaTrigger
--- @field SoundRTPCSyncs SoundRTPCSync[]
--- @field SoundStateSyncs SoundStateSync[]


--- @class SoundVolumeTriggerData:ITriggerData
--- @field AmbientSound FixedString
--- @field AuxBus1 uint8
--- @field AuxBus2 uint8
--- @field AuxBus3 uint8
--- @field AuxBus4 uint8
--- @field Occlusion number


--- @class SpawnCharacterActionData:IActionData
--- @field LocalTemplate FixedString
--- @field RootTemplate FixedString
--- @field SpawnFX FixedString


--- @class StartTriggerData:ITriggerData
--- @field Angle number
--- @field Player uint8
--- @field Team uint8


--- @class StatsAreaTriggerData:ITriggerData
--- @field LevelOverride int32
--- @field ParentGuid Guid
--- @field TreasureLevelOverride int32


--- @class SurfacePathInfluence
--- @field Influence int32
--- @field MaskFlags ESurfaceFlag
--- @field MatchFlags ESurfaceFlag


--- @class SurfaceTemplate:GameObjectTemplate
--- @field AlwaysUseDefaultLifeTime boolean
--- @field CanEnterCombat boolean
--- @field CanSeeThrough boolean
--- @field CanShootThrough boolean
--- @field DamageCharacters boolean
--- @field DamageItems boolean
--- @field DamageTorches boolean
--- @field DamageWeapon FixedString
--- @field DecalMaterial FixedString
--- @field DefaultLifeTime number
--- @field Description TranslatedString
--- @field DisplayName TranslatedString
--- @field FX SurfaceTemplateVisualData[]
--- @field FadeInSpeed number
--- @field FadeOutSpeed number
--- @field InstanceVisual SurfaceTemplateVisualData[]
--- @field IntroFX SurfaceTemplateVisualData[]
--- @field RemoveDestroyedItems boolean
--- @field Seed int32
--- @field Statuses SurfaceTemplateStatusData[]
--- @field Summon FixedString
--- @field SurfaceGrowTimer number
--- @field SurfaceType FixedString
--- @field SurfaceTypeId int32


--- @class SurfaceTemplateStatusData
--- @field ApplyToCharacters boolean
--- @field ApplyToItems boolean
--- @field Chance number
--- @field Duration number
--- @field ForceStatus boolean
--- @field KeepAlive boolean
--- @field OnlyWhileMoving boolean
--- @field RemoveStatus boolean
--- @field StatusId FixedString
--- @field VanishOnReapply boolean


--- @class SurfaceTemplateVisualData
--- @field GridSize number
--- @field Height number
--- @field RandomPlacement number
--- @field Rotation ivec2
--- @field Scale vec2
--- @field SpawnCell int32
--- @field SurfaceNeeded number
--- @field SurfaceRadiusMax number
--- @field Visual FixedString


--- @class TeleportActionData:IActionData
--- @field EventID FixedString
--- @field Source FixedString
--- @field SourceType uint8
--- @field Target FixedString
--- @field TargetType uint8
--- @field Visibility uint8


--- @class TeleportTriggerData:ITriggerData
--- @field Angle number
--- @field Zoom boolean


--- @class Transform
--- @field Matrix mat4
--- @field Rotate mat3
--- @field Scale vec3
--- @field Translate vec3


--- @class TranslatedString
--- @field ArgumentString RuntimeStringHandle
--- @field Handle RuntimeStringHandle


--- @class Trigger:IGameObject
--- @field Flags uint16
--- @field IsGlobal boolean
--- @field Level FixedString
--- @field SyncFlags uint16
--- @field Template TriggerTemplate
--- @field TriggerType FixedString
--- @field Id Guid The trigger Guid.



--- @class TriggerTemplate:GameObjectTemplate
--- @field Color4 vec4
--- @field PhysicsType uint32
--- @field TriggerGizmoOverride FixedString
--- @field TriggerType FixedString


--- @class TypeInformation
--- @field ElementType TypeInformationRef
--- @field EnumValues table<FixedString, uint64>
--- @field HasWildcardProperties boolean
--- @field IsBuiltin boolean
--- @field KeyType TypeInformationRef
--- @field Kind LuaTypeId
--- @field Members table<FixedString, TypeInformationRef>
--- @field Methods table<FixedString, TypeInformation>
--- @field ModuleRole FixedString
--- @field NativeName FixedString
--- @field Params TypeInformationRef[]
--- @field ParentType TypeInformationRef
--- @field ReturnValues TypeInformationRef[]
--- @field TypeName FixedString
--- @field VarargParams boolean
--- @field VarargsReturn boolean


--- @class TypeInformationRef:TypeInformation


--- @class UIObject
--- @field AnchorId string
--- @field AnchorObjectName FixedString
--- @field AnchorPos string
--- @field AnchorTPos string
--- @field AnchorTarget string
--- @field ChildUIHandle ComponentHandle
--- @field CustomScale number
--- @field Flags UIObjectFlags
--- @field FlashMovieSize vec2
--- @field FlashSize vec2
--- @field HasAnchorPos boolean
--- @field InputFocused boolean
--- @field IsDragging boolean
--- @field IsDragging2 boolean
--- @field IsMoving2 boolean
--- @field IsUIMoving boolean
--- @field Layer int32
--- @field Left number
--- @field MinSize vec2
--- @field MovieLayout int32
--- @field OF_Activated boolean
--- @field OF_DeleteOnChildDestroy boolean
--- @field OF_DontHideOnDelete boolean
--- @field OF_FullScreen boolean
--- @field OF_KeepCustomInScreen boolean
--- @field OF_KeepInScreen boolean
--- @field OF_Load boolean
--- @field OF_Loaded boolean
--- @field OF_PauseRequest boolean
--- @field OF_PlayerInput1 boolean
--- @field OF_PlayerInput2 boolean
--- @field OF_PlayerInput3 boolean
--- @field OF_PlayerInput4 boolean
--- @field OF_PlayerModal1 boolean
--- @field OF_PlayerModal2 boolean
--- @field OF_PlayerModal3 boolean
--- @field OF_PlayerModal4 boolean
--- @field OF_PlayerTextInput1 boolean
--- @field OF_PlayerTextInput2 boolean
--- @field OF_PlayerTextInput3 boolean
--- @field OF_PlayerTextInput4 boolean
--- @field OF_PrecacheUIData boolean
--- @field OF_PreventCameraMove boolean
--- @field OF_RequestDelete boolean
--- @field OF_SortOnAdd boolean
--- @field OF_Visible boolean
--- @field ParentUIHandle ComponentHandle
--- @field Path Path
--- @field PlayerId int16
--- @field RenderDataPrepared boolean
--- @field RenderOrder int32
--- @field Right number
--- @field SysPanelPosition ivec2
--- @field SysPanelSize vec2
--- @field Top number
--- @field Type int32
--- @field UIObjectHandle ComponentHandle
--- @field UIScale number
--- @field UIScaling boolean
--- @field CaptureExternalInterfaceCalls fun(self:UIObject)
--- @field CaptureInvokes fun(self:UIObject)
--- @field ClearCustomIcon fun(self:UIObject, element:string)
--- @field Destroy fun(self:UIObject)
--- @field EnableCustomDraw fun(self:UIObject)
--- @field ExternalInterfaceCall fun(self:UIObject, method:string, ...:string|number|boolean)
--- @field GetHandle fun(self:UIObject):ComponentHandle
--- @field GetPlayerHandle fun(self:UIObject):ComponentHandle|nil
--- @field GetPosition fun(self:UIObject):ivec2|nil
--- @field GetRoot fun(self:UIObject):FlashMainTimeline|table
--- @field GetTypeId fun(self:UIObject):int32
--- @field GetUIScaleMultiplier fun(self:UIObject):number
--- @field GetValue fun(self:UIObject, path:string, typeName:string|nil, arrayIndex:int32|nil):IggyInvokeDataValue|nil
--- @field GotoFrame fun(self:UIObject, frame:int32, force:boolean|nil)
--- @field Invoke fun(self:UIObject, method:string, ...:string|number|boolean):boolean
--- @field Resize fun(self:UIObject, a1:number, a2:number, a3:number|nil)
--- @field SetCustomIcon fun(self:UIObject, element:string, icon:string, width:int32, height:int32, materialGuid:string|nil)
--- @field SetPosition fun(self:UIObject, x:int32, y:int32)
--- @field SetValue fun(self:UIObject, path:string, value:IggyInvokeDataValue, arrayIndex:int32|nil)
local UIObject = {}



--- Hides the UI element.
--- Location: Lua/Client/ClientUI.cpp:686
function UIObject:Hide() end

--- Displays the UI element.
--- Location: Lua/Client/ClientUI.cpp:678
function UIObject:Show() end



--- @class UseSkillActionData:IActionData
--- @field Consume boolean
--- @field SkillID FixedString


--- @class Visual:MoveableObject
--- @field Actor Actor
--- @field AllowReceiveDecalWhenAnimated boolean
--- @field Attachments VisualAttachment[]
--- @field CastShadow boolean
--- @field ChildVisualHasCloth boolean
--- @field CullFlags uint16
--- @field FadeOpacity number
--- @field GameObject IGameObject
--- @field Handle ComponentHandle
--- @field HasCloth boolean
--- @field IsShadowProxy boolean
--- @field LODDistances number[]
--- @field Parent Visual
--- @field PlayingAttachedEffects boolean
--- @field ReceiveColorFromParent boolean
--- @field ReceiveDecal boolean
--- @field Reflecting boolean
--- @field ShowMesh boolean
--- @field Skeleton Skeleton
--- @field SubObjects VisualObjectDesc[]
--- @field TextKeyPrepareFlags uint8
--- @field VisualResource VisualResource
--- @field OverrideScalarMaterialParameter fun(self:Visual, a1:FixedString, a2:number)
--- @field OverrideTextureMaterialParameter fun(self:Visual, parameter:FixedString, textureId:FixedString)
--- @field OverrideVec2MaterialParameter fun(self:Visual, a1:FixedString, a2:vec2)
--- @field OverrideVec3MaterialParameter fun(self:Visual, parameter:FixedString, vec:vec3, isColor:boolean)
--- @field OverrideVec4MaterialParameter fun(self:Visual, parameter:FixedString, vec:vec4, isColor:boolean)


--- @class VisualAttachment
--- @field Armor boolean
--- @field AttachmentBoneName FixedString
--- @field BoneIndex int16
--- @field BonusWeaponFX boolean
--- @field DestroyWithParent boolean
--- @field DoNotUpdate boolean
--- @field DummyAttachmentBoneIndex int16
--- @field ExcludeFromBounds boolean
--- @field Horns boolean
--- @field InheritAnimations boolean
--- @field KeepRot boolean
--- @field KeepScale boolean
--- @field Overhead boolean
--- @field ParticleSystem boolean
--- @field UseLocalTransform boolean
--- @field Visual Visual
--- @field Weapon boolean
--- @field WeaponFX boolean
--- @field WeaponOverlayFX boolean
--- @field Wings boolean


--- @class VisualObjectDesc
--- @field Renderable RenderableObject
--- @field field_8 uint8


--- @class VisualResource
--- @field AnimationWaterfall FixedString[]
--- @field Attachments VisualResourceAttachment[]
--- @field BlueprintInstanceResourceID FixedString
--- @field Bones table<FixedString, VisualResourceBonePosRot>
--- @field ClothParams VisualResourceClothParam[]
--- @field CustomAnimationSet AnimationSet
--- @field KnownAnimationSetOverrides table<FixedString, FixedString>
--- @field LODDistances number[]
--- @field Objects VisualResourceObjectDesc[]
--- @field ResolvedAnimationSet AnimationSet
--- @field Template FixedString


--- @class VisualResourceAttachment
--- @field Name FixedString
--- @field UUID FixedString


--- @class VisualResourceBonePosRot
--- @field Position vec3
--- @field Rotation mat3


--- @class VisualResourceClothParam
--- @field AtmosphericWindEnabled boolean
--- @field BendingStiffness number
--- @field Drag number
--- @field FrontalWindFrequency number
--- @field FrontalWindSpeed number
--- @field FrontalWindVariance number
--- @field Iterations int32
--- @field Lift number
--- @field LinearStiffness number
--- @field Margin number
--- @field MassPerMeterSqr number
--- @field PoseMatching number
--- @field UUID FixedString


--- @class VisualResourceObjectDesc
--- @field LOD uint8
--- @field MaterialID FixedString
--- @field ObjectID FixedString


--- @class VisualSet
--- @field Colors uint32[][]
--- @field Visuals FixedString[][]


--- @class VisualSetIndices
--- @field GetColor fun(self:VisualSetIndices, a1:uint32):uint32
--- @field GetVisual fun(self:VisualSetIndices, a1:uint32):uint32
--- @field SetColor fun(self:VisualSetIndices, a1:uint32, a2:uint32)
--- @field SetVisual fun(self:VisualSetIndices, a1:uint32, a2:uint32)


--- @class VisualSetResource
--- @field LastUsedPreviewVisual FixedString
--- @field VisualSet VisualSet
--- @field VisualSetIndices VisualSetIndices


--- @class CharacterCreationAbilityChange
--- @field Ability StatsAbilityType
--- @field AmountIncreased int32


--- @class CharacterCreationAiPersonalityDesc
--- @field ID FixedString
--- @field Name TranslatedString


--- @class CharacterCreationAttributeChange
--- @field AmountIncreased int32
--- @field Attribute PlayerUpgradeAttribute


--- @class CharacterCreationCharacterCreationManager
--- @field AiPersonalities CharacterCreationAiPersonalityDesc[]
--- @field ClassPresets CharacterCreationClassDesc[]
--- @field CompanionPresets CharacterCreationClassDesc[]
--- @field DefaultAiPersonality TranslatedString
--- @field DefaultOrigin CharacterCreationOriginDesc
--- @field DefaultRace CharacterCreationRaceDesc
--- @field DefaultVoice CharacterCreationVoiceDesc
--- @field GenericOriginPresets CharacterCreationOriginDesc[]
--- @field HenchmanPresets CharacterCreationClassDesc[]
--- @field OriginPresets CharacterCreationOriginDesc[]
--- @field RacePresets CharacterCreationRaceDesc[]
--- @field Tags table<uint32, FixedString[]>
--- @field Voices CharacterCreationVoiceDesc[]


--- @class CharacterCreationClassDesc
--- @field AbilityChanges CharacterCreationAbilityChange[]
--- @field AreStatsWeighted boolean
--- @field AttributeChanges CharacterCreationAttributeChange[]
--- @field ClassDescription TranslatedString
--- @field ClassLongDescription TranslatedString
--- @field ClassName TranslatedString
--- @field ClassType FixedString
--- @field EquipmentProperties CharacterCreationEquipmentProperty[]
--- @field Icon int32
--- @field NumStartingAttributePoints int32
--- @field NumStartingCivilAbilityPoints int32
--- @field NumStartingCombatAbilityPoints int32
--- @field NumStartingTalentPoints int32
--- @field Price int32
--- @field SkillSet FixedString
--- @field TalentsAdded StatsTalentType[]
--- @field Voice int32


--- @class CharacterCreationColorDesc
--- @field ColorName TranslatedString
--- @field ID uint32
--- @field Value uint32


--- @class CharacterCreationEquipmentProperty
--- @field PreviewEquipmentSet FixedString
--- @field RaceName FixedString
--- @field StartingEquipmentSet FixedString


--- @class CharacterCreationOriginDesc
--- @field AbilityChanges CharacterCreationAbilityChange[]
--- @field AttributeChanges CharacterCreationAttributeChange[]
--- @field CharacterUUID FixedString
--- @field OriginDescription TranslatedString
--- @field OriginDisplayName TranslatedString
--- @field OriginLongDescription TranslatedString
--- @field OriginName FixedString
--- @field PreviewEquipmentSet FixedString
--- @field RootTemplateOverride FixedString
--- @field SkillSet FixedString
--- @field SupportedGenders uint32[]
--- @field SupportedRaces FixedString[]
--- @field TalentsAdded StatsTalentType[]
--- @field UserCanAlterTags boolean


--- @class CharacterCreationRaceDesc
--- @field AbilityChanges CharacterCreationAbilityChange[]
--- @field AllowBald boolean
--- @field AttributeChanges CharacterCreationAttributeChange[]
--- @field ClothColors CharacterCreationColorDesc[]
--- @field DefaultColor CharacterCreationColorDesc
--- @field FemaleDefaultHairColor uint32
--- @field FemaleDefaultPlayerNames TranslatedString[]
--- @field FemaleDefaultSkinColor uint32
--- @field FemaleIcons FixedString[]
--- @field FemaleRootTemplateIDs FixedString[]
--- @field HairColors CharacterCreationColorDesc[]
--- @field MaleDefaultHairColor uint32
--- @field MaleDefaultPlayerNames TranslatedString[]
--- @field MaleDefaultSkinColor uint32
--- @field MaleIcons FixedString[]
--- @field MaleRootTemplateIDs FixedString[]
--- @field RaceDescription TranslatedString
--- @field RaceDisplayName TranslatedString
--- @field RaceName FixedString
--- @field SkillSet FixedString
--- @field SkinColors CharacterCreationColorDesc[]
--- @field TagsAdded FixedString[]
--- @field TalentsAdded StatsTalentType[]


--- @class CharacterCreationVoiceDesc
--- @field FemaleSpeakerID FixedString
--- @field ID uint32
--- @field MaleSpeakerID FixedString
--- @field Name TranslatedString


--- @class DsePropertyList
--- @field AlphaChannel uint8
--- @field OpaqueChannel uint8
--- @field RenderPasses int32
--- @field TransformType int32
--- @field field_6 uint8
--- @field field_7 uint8


--- @class EclBaseController
--- @field CharacterHandle ComponentHandle
--- @field TypeId int32


--- @class EclCharacter:IEoCClientReplicatedObject
--- @field AI EocAi
--- @field Activated boolean
--- @field AnimType int32
--- @field AnimationSetOverride FixedString
--- @field AnimationSpeed number
--- @field Archetype FixedString
--- @field AttitudeMaps table<ComponentHandle, int32>
--- @field CanShootThrough boolean
--- @field CannotFight boolean
--- @field CannotMove boolean
--- @field CharCreationInProgress boolean
--- @field CharacterBody EclCharacterBody
--- @field CharacterCreationFinished boolean
--- @field Color uint8
--- @field CorpseCharacterHandle ComponentHandle
--- @field CorpseLootable boolean
--- @field CorpseOwnerHandle ComponentHandle
--- @field CrimeState int32
--- @field CurrentLevel FixedString
--- @field CurrentTemplate CharacterTemplate
--- @field DamageCounter int64
--- @field Dead boolean
--- @field Detached boolean
--- @field DialogId int32
--- @field DisableSneaking boolean
--- @field DisplayNameOverride TranslatedString
--- @field EquipmentColor FixedString
--- @field FadeTimer number
--- @field FeetStatusId int32
--- @field Flags EclCharacterFlags
--- @field Floating boolean
--- @field FollowCharacterHandle ComponentHandle
--- @field FootStepEffectTimer number
--- @field FootStepMaterialType int32
--- @field GMControl boolean
--- @field GMState uint32
--- @field Global boolean
--- @field HasCloth boolean
--- @field HasCustomVisualIndices boolean
--- @field HasCustomVisualSet boolean
--- @field HasInventory boolean
--- @field HasOwner boolean
--- @field HasRunSpeedOverride boolean
--- @field HasSkillTargetEffect boolean
--- @field HasWalkSpeedOverride boolean
--- @field HealCounter int64
--- @field HighlightCircleEffect ComponentHandle
--- @field InArena boolean
--- @field InCombat boolean
--- @field InDialog boolean
--- @field InParty boolean
--- @field InputController EclInputController
--- @field InventoryHandle ComponentHandle
--- @field Invisible boolean
--- @field IsArenaParticipant boolean
--- @field IsDialogAiControlled boolean
--- @field IsGameMaster boolean
--- @field IsHuge boolean
--- @field IsPlayer boolean
--- @field IsPossessed boolean
--- @field IsRunning boolean
--- @field IsTrader boolean
--- @field IsTrading boolean
--- @field ItemTags FixedString[]
--- @field KillCounter int64
--- @field LadderPosition vec3
--- @field LifeTime number
--- @field LootCorpseRequested boolean
--- @field LootedByHandle ComponentHandle
--- @field MovementStartPosition vec3
--- @field MovementTargetPosition vec3
--- @field MovementUpdated boolean
--- @field MovingProjectileHandle ComponentHandle
--- @field Multiplayer boolean
--- @field NetID2 NetId
--- @field NetID3 NetId
--- @field NoCover boolean
--- @field NoReputationEffects boolean
--- @field NoRotate boolean
--- @field OffStage boolean
--- @field OriginalDisplayName TranslatedString
--- @field OriginalTemplate CharacterTemplate
--- @field OwnerCharacterHandle ComponentHandle
--- @field PartyFollower boolean
--- @field PartyHandle ComponentHandle
--- @field Physics PhysicsObject
--- @field PhysicsBoneIndex int32
--- @field PhysicsPickingState int32
--- @field PickpocketRequested boolean
--- @field Pickpocketing boolean
--- @field PlayerCustomData EocPlayerCustomData
--- @field PlayerData EclPlayerData
--- @field PlayerUpgrade EocPlayerUpgrade
--- @field RefreshVisualsRequested boolean
--- @field RemainingMoveDistance number
--- @field ReservedForDialog boolean
--- @field ReservedForPlayerId int16
--- @field ReservedForPlayerId2 int16
--- @field RootTemplate CharacterTemplate
--- @field RunSpeedOverride number
--- @field SkillManager EclSkillManager
--- @field SneakCheckTimer number
--- @field SoundBoneIndex int32
--- @field SoundComponents EntityHandle[]
--- @field Spectating boolean
--- @field SpotSneakers boolean
--- @field Stats CDivinityStatsCharacter
--- @field StatusMachine EclStatusMachine
--- @field StoryDisplayName TranslatedString
--- @field StoryNPC boolean
--- @field Summon boolean
--- @field SummonHandles ComponentHandle[]
--- @field SurfacePathInfluences SurfacePathInfluence[]
--- @field Tags FixedString[]
--- @field TalkingIconEffect ComponentHandle
--- @field Team uint8
--- @field TransformMoveAttachedEffects boolean
--- @field UnknownDialogFlag boolean
--- @field UseOverlayMaterials boolean
--- @field UserID UserId
--- @field UserVars UserReturn
--- @field ViewConeEffectHandle ComponentHandle
--- @field VisionGridInvisible2 boolean
--- @field VisualSetIndices VisualSetIndices
--- @field VisualsUpdated boolean
--- @field VoiceSet FixedString[]
--- @field WalkSpeedOverride number
--- @field WalkThrough boolean
--- @field WeaponSheathed boolean
--- @field WorldPos vec3
--- @field GetCustomStat fun(self:EclCharacter, a1:FixedString):int32|nil
--- @field GetInventoryItems fun(self:EclCharacter):FixedString[]
--- @field GetItemBySlot fun(self:EclCharacter, slot:StatsItemSlot32):FixedString|nil
--- @field GetItemObjectBySlot fun(self:EclCharacter, a1:StatsItemSlot32):EclItem
--- @field SetScale fun(self:EclCharacter, a1:number)


--- @class EclCharacterBody
--- @field AnimRandom number
--- @field AnimationDuration number
--- @field AnimationOverride FixedString[]
--- @field AnimationRunning boolean
--- @field CharacterHandle ComponentHandle
--- @field ClimbLoopDuration number
--- @field CurrentAnimation FixedString
--- @field IsImmediate boolean
--- @field NextAnimation FixedString
--- @field QueuedAnimation FixedString
--- @field SpeedScaledAnimationDuration number
--- @field WaitForLoad boolean


--- @class EclCharacterConversionHelpers
--- @field ActivatedCharacters table<FixedString, EclCharacter[]>
--- @field RegisteredCharacters table<FixedString, EclCharacter[]>


--- @class EclCharacterTask
--- @field ActionTypeId int32
--- @field BoundCharacter EclCharacter
--- @field CurrentContext EclCharacterTaskContext
--- @field Flags EclCharacterTaskFlags
--- @field IsInPreviewMode boolean
--- @field RequestRun boolean
--- @field SavedContext EclCharacterTaskContext


--- @class EclCharacterTaskContext
--- @field CurrentAP int32
--- @field Position vec3
--- @field RemainingMoveDistance int32
--- @field field_18 ComponentHandle[]


--- @class EclCombatComponent:EocCombatComponent


--- @class EclCursorControl
--- @field CursorShown boolean
--- @field Flags uint32
--- @field HWMouseCursor FixedString
--- @field HasTextDisplay boolean
--- @field HideTimer number
--- @field InputHideTimer number
--- @field MouseCursor EclCursorType
--- @field RequestedFlags uint32
--- @field TextDisplayUIHandle ComponentHandle
--- @field field_34 uint32


--- @class EclEntityManager
--- @field CharacterConversionHelpers EclCharacterConversionHelpers
--- @field ItemConversionHelpers EclItemConversionHelpers
--- @field ProjectileConversionHelpers EclProjectileConversionHelpers
--- @field SceneryConversionHelpers EclSceneryConversionHelpers
--- @field TriggerConversionHelpers EclTriggerConversionHelpers


--- @class EclEoCUI:UIObject


--- @class EclEocCamera:EffectCameraController
--- @field IsInShroud boolean
--- @field LookAt vec3
--- @field NeedsLookAtUpdate boolean
--- @field PlayerIndex int16
--- @field ResetAtmosphere boolean


--- @class EclEquipmentVisualSystemSetParam
--- @field AllowTPose boolean
--- @field ApplyColors boolean
--- @field Armor boolean
--- @field AttachFlags VisualAttachmentFlags
--- @field AttachmentBoneName FixedString
--- @field BonusWeaponFX boolean
--- @field CastShadow boolean
--- @field ClearEffects boolean
--- @field ClothHandle ComponentHandle
--- @field Colors vec4[]
--- @field DeactivateAmulet boolean
--- @field DeactivateArms boolean
--- @field DeactivateBeard boolean
--- @field DeactivateBelt boolean
--- @field DeactivateBoots boolean
--- @field DeactivateBootsVisual boolean
--- @field DeactivateBreast boolean
--- @field DeactivateGloves boolean
--- @field DeactivateHair boolean
--- @field DeactivateHead boolean
--- @field DeactivateHelmet boolean
--- @field DeactivateHorns boolean
--- @field DeactivateLeggings boolean
--- @field DeactivateOverhead boolean
--- @field DeactivateRing boolean
--- @field DeactivateRing2 boolean
--- @field DeactivateShield boolean
--- @field DeactivateTorso boolean
--- @field DeactivateTrousers boolean
--- @field DeactivateWeapon boolean
--- @field DeactivateWings boolean
--- @field DestroyWithParent boolean
--- @field DoNotUpdate boolean
--- @field EquipmentSlotMask EclDeactivateEquipmentSlotMask
--- @field ExcludeFromBounds boolean
--- @field HasHair boolean
--- @field Horns boolean
--- @field InheritAnimations boolean
--- @field IsShadowProxy boolean
--- @field KeepRot boolean
--- @field KeepScale boolean
--- @field LevelName FixedString
--- @field Overhead boolean
--- @field ParticleSystem boolean
--- @field ReceiveDecal boolean
--- @field Reflecting boolean
--- @field Slot StatsItemSlot32
--- @field SomeWeaponFlag boolean
--- @field UseLocalTransform boolean
--- @field VisualResourceID FixedString
--- @field VisualSetSlotMask EclDeactivateVisualSetSlotMask
--- @field Weapon boolean
--- @field WeaponFX boolean
--- @field WeaponOverlayFX boolean
--- @field Wings boolean


--- @class EclEquipmentVisualsComponent
--- @field Visuals EclEquipmentVisualsComponentVisualEntry[]


--- @class EclEquipmentVisualsComponentLoadingVisual
--- @field Handle ComponentHandle


--- @class EclEquipmentVisualsComponentVisualEntry
--- @field DeactivateAmulet boolean
--- @field DeactivateArms boolean
--- @field DeactivateBeard boolean
--- @field DeactivateBelt boolean
--- @field DeactivateBoots boolean
--- @field DeactivateBootsVisual boolean
--- @field DeactivateBreast boolean
--- @field DeactivateGloves boolean
--- @field DeactivateHair boolean
--- @field DeactivateHead boolean
--- @field DeactivateHelmet boolean
--- @field DeactivateHorns boolean
--- @field DeactivateLeggings boolean
--- @field DeactivateOverhead boolean
--- @field DeactivateRing boolean
--- @field DeactivateRing2 boolean
--- @field DeactivateShield boolean
--- @field DeactivateTorso boolean
--- @field DeactivateTrousers boolean
--- @field DeactivateWeapon boolean
--- @field DeactivateWings boolean
--- @field EquipmentSlotMask EclDeactivateEquipmentSlotMask
--- @field LoadingVisual EclEquipmentVisualsComponentLoadingVisual
--- @field Visual Visual
--- @field VisualSetSlotMask EclDeactivateVisualSetSlotMask


--- @class EclFadingMaterialInfo:EclMaterialInfo
--- @field FadeParameter FixedString
--- @field FadeValue number
--- @field InitialFadeValue number
--- @field RequestFlags uint8
--- @field StateFlags uint32


--- @class EclGameCamera:EclEocCamera
--- @field CamAvoidanceValue1 number
--- @field CamAvoidanceValue2 number
--- @field CameraMode int32
--- @field CameraPanX number
--- @field CameraPanY number
--- @field CharacterHandle ComponentHandle
--- @field CurrentCameraDistance number
--- @field CurrentLookAt vec3
--- @field Flags uint8
--- @field Flags2 uint8
--- @field InputCameraDistanceRate number
--- @field InputRotationRate number
--- @field InputRotationRateMode1 number
--- @field Position2 vec3
--- @field Position_M vec3
--- @field Rotation number
--- @field RotationRate number
--- @field SomeFadeFloat number
--- @field SomeModeFlag boolean
--- @field TargetCameraDistance number
--- @field TargetCameraDistance2 number
--- @field TargetLookAt vec3
--- @field Targets ComponentHandle[]
--- @field field_3E0 int32
--- @field field_3E4 int32
--- @field field_3E8 int32
--- @field field_3F0 int32
--- @field field_410 int64
--- @field field_418 int32
--- @field field_499 uint8


--- @class EclInputController:EclBaseController
--- @field ChosenTasks EclCharacterTask[]
--- @field CurrentTask EclCharacterTask
--- @field DraggingObjectHandle ComponentHandle
--- @field Flags uint8
--- @field HasExited boolean
--- @field IsActive boolean
--- @field IsDragging boolean
--- @field IsPicking boolean
--- @field MousePos vec2
--- @field PickedNoObject boolean
--- @field PreviewTask EclCharacterTask
--- @field TaskFlags uint8
--- @field TaskPrototypes EclCharacterTask[]
--- @field TimeElapsed number
--- @field field_21 boolean
--- @field field_2C int32


--- @class EclInventory
--- @field EquipmentSlots uint8
--- @field Flags uint8
--- @field GUID FixedString
--- @field ItemsBySlot ComponentHandle[]
--- @field NetID NetId
--- @field OwnerCharacterHandleUI ComponentHandle
--- @field ParentHandle ComponentHandle
--- @field PinnedContainers ComponentHandle[]
--- @field UpdateViews ComponentHandle[]


--- @class EclInventoryView
--- @field Handle ComponentHandle
--- @field ItemHandles ComponentHandle[]
--- @field ItemNetIdToIndex table<NetId, int32>
--- @field NetID NetId
--- @field ParentInventories NetId[]
--- @field ParentNetId_M NetId


--- @class EclItem:IEoCClientReplicatedObject
--- @field AI EocAi
--- @field AIBoundSize number
--- @field Activated boolean
--- @field Amount int32
--- @field BaseWeightOverwrite int32
--- @field CachedItemDescription string
--- @field CanBeMoved boolean
--- @field CanBePickedUp boolean
--- @field CanShootThrough boolean
--- @field CanUse boolean
--- @field CanUseRemotely boolean
--- @field CanWalkThrough boolean
--- @field Consumable boolean
--- @field CoverAmount boolean
--- @field CurrentLevel FixedString
--- @field CurrentSlot int16
--- @field CurrentTemplate ItemTemplate
--- @field CustomBookContent TranslatedString
--- @field CustomDescription TranslatedString
--- @field CustomDisplayName TranslatedString
--- @field Destroyed boolean
--- @field DontAddToBottomBar boolean
--- @field EnableHighlights boolean
--- @field Fade boolean
--- @field FallTimer number
--- @field Flags EclItemFlags
--- @field Flags2 EclItemFlags2
--- @field Floating boolean
--- @field FoldDynamicStats boolean
--- @field FreezeGravity boolean
--- @field Global boolean
--- @field GoldValueOverride int32
--- @field GravityTimer number
--- @field HasPendingNetUpdate boolean
--- @field Hostile boolean
--- @field Icon FixedString
--- @field InUseByCharacterHandle ComponentHandle
--- @field InUseByUserId int32
--- @field InteractionDisabled boolean
--- @field InventoryHandle ComponentHandle
--- @field InventoryParentHandle ComponentHandle
--- @field Invisible boolean
--- @field Invulnerable boolean
--- @field IsCraftingIngredient boolean
--- @field IsDoor boolean
--- @field IsGrenade boolean
--- @field IsKey boolean
--- @field IsLadder boolean
--- @field IsSecretDoor boolean
--- @field IsSourceContainer boolean
--- @field ItemColorOverride FixedString
--- @field ItemType FixedString
--- @field JoinedDialog boolean
--- @field KeyName FixedString
--- @field Known boolean
--- @field Level int32
--- @field LockLevel int32
--- @field MovementUpdated boolean
--- @field OwnerCharacterHandle ComponentHandle
--- @field Physics PhysicsObject
--- @field PhysicsDisabled boolean
--- @field PhysicsFlag1 boolean
--- @field PhysicsFlag2 boolean
--- @field PhysicsFlag3 boolean
--- @field PhysicsFlags EclItemPhysicsFlags
--- @field PinnedContainer boolean
--- @field Registered boolean
--- @field RequestRaycast boolean
--- @field RequestWakeNeighbours boolean
--- @field RootTemplate ItemTemplate
--- @field Stats CDivinityStatsItem
--- @field StatsFromName StatsObject
--- @field StatsId FixedString
--- @field StatusMachine EclStatusMachine
--- @field Sticky boolean
--- @field Stolen boolean
--- @field StoryItem boolean
--- @field Tags FixedString[]
--- @field TeleportOnUse boolean
--- @field UnEquipLocked boolean
--- @field Unimportant boolean
--- @field UnknownTimer number
--- @field UseSoundsLoaded boolean
--- @field UserVars UserReturn
--- @field Vitality int32
--- @field Wadable boolean
--- @field WakePosition vec3
--- @field Walkable boolean
--- @field WasOpened boolean
--- @field WorldPos vec3
--- @field GetDeltaMods fun(self:EclItem):FixedString[]
--- @field GetInventoryItems fun(self:EclItem):FixedString[]
--- @field GetOwnerCharacter fun(self:EclItem):FixedString|nil


--- @class EclItemConversionHelpers
--- @field ActivatedItems table<FixedString, EclItem[]>
--- @field RegisteredItems table<FixedString, EclItem[]>


--- @class EclLevel:Level
--- @field AiGrid EocAiGrid
--- @field EntityManager EclEntityManager
--- @field LevelCacheTemplateManager LevelCacheTemplateManager
--- @field ShroudManager EclShroudManager
--- @field SurfaceManager EclSurfaceManager


--- @class EclLevelManager
--- @field CurrentLevel EclLevel
--- @field LevelDescs LevelDesc[]
--- @field Levels table<FixedString, EclLevel>
--- @field Levels2 table<FixedString, EclLevel>


--- @class EclMaterialInfo:MaterialInfo
--- @field RefCount uint8
--- @field ScalarParamaters table<FixedString, number>
--- @field Vec2Parameters table<FixedString, vec2>
--- @field Vec3Parameters table<FixedString, vec3>
--- @field Vec4Parameters table<FixedString, vec4>


--- @class EclMaterialManager
--- @field FadingMaterials table<ComponentHandle, EclFadingMaterialInfo[]>
--- @field Materials table<ComponentHandle, EclMaterialInfo[]>
--- @field OverlayMaterialRequests OverlayMaterialRequest[]


--- @class EclMultiEffectHandler
--- @field AttachedVisualComponents ComponentHandle[]
--- @field Effects ComponentHandle[]
--- @field ListenForTextKeysHandle ComponentHandle
--- @field ListeningOnTextKeys boolean
--- @field Position vec3
--- @field TargetObjectHandle ComponentHandle
--- @field TextKeyEffects table<FixedString, EclMultiEffectHandlerEffectInfo[]>
--- @field Visuals EclMultiEffectHandlerMultiEffectVisual[]
--- @field WeaponAttachments EclMultiEffectHandlerWeaponAttachmentInfo[]
--- @field WeaponBones FixedString


--- @class EclMultiEffectHandlerEffectInfo
--- @field Beam boolean
--- @field BoneNames FixedString[]
--- @field Detach boolean
--- @field Effect FixedString
--- @field EffectAttached boolean
--- @field FaceSource boolean
--- @field FollowScale boolean
--- @field KeepRot boolean


--- @class EclMultiEffectHandlerMultiEffectVisual
--- @field MultiEffectHandler EclMultiEffectHandler
--- @field OS_FS FixedString[]


--- @class EclMultiEffectHandlerWeaponAttachmentInfo
--- @field BoneNames FixedString
--- @field EffectName FixedString
--- @field VisualId FixedString


--- @class EclPickPosition
--- @field Position vec3
--- @field Position2 vec3


--- @class EclPickingHelper:EclPickingHelperBase
--- @field Level EclLevel
--- @field PlayerId int16
--- @field ScreenPos vec2


--- @class EclPickingHelperBase
--- @field GameObjectPickFlags uint8
--- @field HoverAliveCharacterHandle ComponentHandle
--- @field HoverCharacterPos EclPickPosition
--- @field HoverDeadCharacterHandle ComponentHandle
--- @field HoverGameObjectHandle ComponentHandle
--- @field HoverItemHandle ComponentHandle
--- @field HoverItemPos EclPickPosition
--- @field PlaceableObjectHandle ComponentHandle
--- @field PlaceablePos EclPickPosition
--- @field WalkableAiFlags uint64
--- @field WalkablePickFlags uint8
--- @field WalkablePos EclPickPosition


--- @class EclPlayerCustomData:EocPlayerCustomData


--- @class EclPlayerData
--- @field ArmorOptionState boolean
--- @field AttitudeOverrideMap table<ComponentHandle, int32>
--- @field CachedTension int32
--- @field CorpseLootTargetNetID NetId
--- @field CustomData EclPlayerCustomData
--- @field HelmetOptionState boolean
--- @field LockedAbilities uint32[]
--- @field MemorisedSkills FixedString[]
--- @field OriginalTemplate FixedString
--- @field PickpocketTargetNetID NetId
--- @field QuestSelected FixedString
--- @field Region FixedString
--- @field SelectedSkillSet uint8
--- @field SkillBarItems EocSkillBarItem[]


--- @class EclPlayerManager:PlayerManager
--- @field ClientPlayerData table<int16, EclPlayerManagerClientPlayerInfo>
--- @field ConnectedPlayers int16[]
--- @field CurrentInputPlayerIndex int32
--- @field DebugLocalPlayerId int16
--- @field DebugPlayerIds int16[]
--- @field DebugPlayerProfileGuids FixedString[]
--- @field IsDisconnecting boolean
--- @field LinkedDevices int32[]
--- @field NextLocalPlayerId uint64
--- @field PlayerIdToNetId table<int16, NetId>


--- @class EclPlayerManagerClientPlayerInfo
--- @field CameraControllerId FixedString
--- @field CharacterNetId NetId
--- @field ProfileGuid FixedString


--- @class EclProjectile:IEoCClientReplicatedObject
--- @field Caster ComponentHandle
--- @field CurrentTemplate ProjectileTemplate
--- @field ExplodeRadius number
--- @field ImpactFX FixedString
--- @field IsGlobal boolean
--- @field Level FixedString
--- @field LifeTime number
--- @field MovingObject ComponentHandle
--- @field RequestDelete boolean
--- @field RotateImpact boolean
--- @field SkillID FixedString
--- @field Source ComponentHandle
--- @field SourcePos vec3
--- @field SpawnEffect FixedString
--- @field SpawnFXOverridesImpactFX boolean
--- @field TargetCharacter ComponentHandle
--- @field TargetPos vec3


--- @class EclProjectileConversionHelpers
--- @field RegisteredProjectiles table<FixedString, EclProjectile[]>


--- @class EclScenery:IEoCClientObject
--- @field AllowReceiveDecalWhenAnimated boolean
--- @field CanShootThrough boolean
--- @field CastShadow boolean
--- @field CoverAmount uint8
--- @field DefaultState FixedString
--- @field Destroyed boolean
--- @field Fade boolean
--- @field FadeParams EclSceneryFadeSettings
--- @field Fadeable boolean
--- @field Flags EclSceneryFlags
--- @field ForceUseAnimationBlueprint boolean
--- @field GUID FixedString
--- @field Invisible boolean
--- @field IsBlocker boolean
--- @field IsReflecting boolean
--- @field IsShadowProxy boolean
--- @field IsWall boolean
--- @field LevelName FixedString
--- @field Physics PhysicsObject
--- @field ReceiveDecals boolean
--- @field RenderChannel uint8
--- @field RenderFlags EclSceneryRenderFlags
--- @field SeeThrough boolean
--- @field SoundParams EclScenerySoundSettings
--- @field Template GameObjectTemplate
--- @field UnknownFlag100 boolean
--- @field UnknownFlag80 boolean
--- @field UnknownRenderFlag1 boolean
--- @field UnknownRenderFlag20 boolean
--- @field VisualResourceID FixedString
--- @field Wadable boolean
--- @field Walkable boolean


--- @class EclSceneryFadeSettings
--- @field FadeGroup FixedString
--- @field FadeIn boolean
--- @field Opacity number


--- @class EclScenerySoundSettings
--- @field LoopSound FixedString
--- @field SoundAttenuation int16
--- @field SoundInitEvent FixedString


--- @class EclSceneryConversionHelpers
--- @field RegisteredScenery table<FixedString, EclScenery[]>


--- @class EclShroudManager
--- @field RebuildHeightMapTexture fun(self:EclShroudManager)


--- @class EclSkill
--- @field ActiveCooldown number
--- @field CanActivate boolean
--- @field CauseListSize int32
--- @field Handle ComponentHandle
--- @field HasCooldown boolean
--- @field IsActivated boolean
--- @field IsLearned boolean
--- @field MaxCharges int32
--- @field NetID NetId
--- @field NumCharges int32
--- @field OwnerHandle ComponentHandle
--- @field SkillId FixedString
--- @field Type SkillType
--- @field ZeroMemory boolean


--- @class EclSkillManager
--- @field OwnerHandle ComponentHandle
--- @field Skills table<FixedString, EclSkill>


--- @class EclStatus
--- @field CurrentLifeTime number
--- @field Flags EclStatusFlags
--- @field LifeTime number
--- @field NetID NetId
--- @field OwnerHandle ComponentHandle
--- @field StartTimer number
--- @field StatsMultiplier number
--- @field StatusHandle ComponentHandle
--- @field StatusId FixedString
--- @field StatusSourceHandle ComponentHandle
--- @field StatusType FixedString


--- @class EclStatusActiveDefense:EclStatusConsumeBase
--- @field Charges int32
--- @field EffectHandle ComponentHandle
--- @field TargetHandle ComponentHandle
--- @field TargetPos vec3
--- @field TargetSize int32


--- @class EclStatusAdrenaline:EclStatusConsumeBase
--- @field InitialAPMod int32
--- @field SecondaryAPMod int32


--- @class EclStatusAoO:EclStatus
--- @field ShowOverhead boolean
--- @field SourceHandle ComponentHandle
--- @field TargetHandle ComponentHandle


--- @class EclStatusBoost:EclStatus
--- @field BoostId FixedString


--- @class EclStatusChallenge:EclStatusConsumeBase
--- @field SourceHandle ComponentHandle
--- @field Target boolean


--- @class EclStatusCharmed:EclStatusConsumeBase
--- @field EffectHandle ComponentHandle
--- @field OriginalOwnerCharacterHandle ComponentHandle


--- @class EclStatusClean:EclStatus
--- @field EffectHandler EclMultiEffectHandler
--- @field OverlayMaterial FixedString


--- @class EclStatusClimbing:EclStatus
--- @field ClimbLoopDuration number
--- @field Direction boolean
--- @field JumpUpLadders boolean
--- @field LadderHandle ComponentHandle
--- @field NeedsCharacterUpdate boolean
--- @field Started boolean
--- @field UpdateTimer number


--- @class EclStatusCombat:EclStatus
--- @field CombatTeamId EocCombatTeamId


--- @class EclStatusConsumeBase:EclStatus
--- @field ApplyEffectHandler EclMultiEffectHandler
--- @field AttributeHandle int32
--- @field AuraFX FixedString
--- @field AuraFXHandle ComponentHandle
--- @field AuraRadius number
--- @field CurrentStatsMultiplier number
--- @field EffectHandler EclMultiEffectHandler
--- @field HasItemFlag0x200000000000 boolean
--- @field Icon FixedString
--- @field Material FixedString
--- @field MaterialApplyFlags StatusMaterialApplyFlags
--- @field MaterialApplyNormalMap boolean
--- @field MaterialFadeAmount number
--- @field MaterialOverlayOffset number
--- @field MaterialParams EclStatusConsumeBaseMaterialParam[]
--- @field MaterialType uint32
--- @field OriginalWeaponId FixedString
--- @field OverrideWeaponHandle ComponentHandle
--- @field OverrideWeaponId FixedString
--- @field SourceDirection vec3
--- @field StackId FixedString
--- @field StatsDataPerTurn EclStatusConsumeBaseStatsData[]
--- @field StatsId FixedString
--- @field StatusEffectOverrideForItems FixedString
--- @field TooltipText string
--- @field TooltipTextNeedsUpdating boolean
--- @field TooltipTexts string[]
--- @field Turn int32


--- @class EclStatusConsumeBaseMaterialParam
--- @field Name FixedString
--- @field Value FixedString


--- @class EclStatusConsumeBaseStatsData
--- @field StatsId FixedString
--- @field TurnIndex int32


--- @class EclStatusDamage:EclStatusConsumeBase
--- @field DamageLevel int32
--- @field DamageStats FixedString


--- @class EclStatusDamageOnMove:EclStatusConsumeBase
--- @field DamageAmount int32
--- @field DistancePerDamage number


--- @class EclStatusDying:EclStatus
--- @field AttackDirection int32
--- @field CombatId boolean
--- @field DeathType StatsDeathType
--- @field DieActionsCompleted boolean
--- @field ImpactDirection vec3
--- @field InflicterNetId NetId
--- @field IsAlreadyDead boolean
--- @field SkipAnimation boolean


--- @class EclStatusEffect:EclStatus
--- @field Active boolean
--- @field BeamEffect FixedString
--- @field EffectHandler EclMultiEffectHandler
--- @field Icon FixedString
--- @field PeaceOnly boolean
--- @field PlayerHasTag FixedString
--- @field PlayerSameParty boolean
--- @field StatusEffect FixedString


--- @class EclStatusEncumbered:EclStatusConsumeBase
--- @field NextMessageDelay number
--- @field State int32


--- @class EclStatusFloating:EclStatusConsumeBase
--- @field OnlyWhileMoving boolean


--- @class EclStatusGuardianAngel:EclStatusConsumeBase
--- @field EffectHandle ComponentHandle


--- @class EclStatusHeal:EclStatus
--- @field EffectHandler EclMultiEffectHandler
--- @field EffectTime number
--- @field HealAmount int32
--- @field HealEffect uint32
--- @field HealEffectId FixedString
--- @field HealType int32
--- @field TargetDependentHeal boolean
--- @field TargetDependentHealAmount int32[]


--- @class EclStatusHealing:EclStatusConsumeBase
--- @field HealAmount int32
--- @field HealStat int32


--- @class EclStatusHit:EclStatus
--- @field AllowInterruptAction_IsIdleAction boolean
--- @field DamageInfo StatsHitDamageInfo
--- @field DeleteRequested boolean
--- @field HitByHandle ComponentHandle
--- @field HitByType int32
--- @field HitReason int32
--- @field ImpactDirection vec3
--- @field ImpactPosition vec3
--- @field Interruption boolean
--- @field TimeRemaining number
--- @field WeaponHandle ComponentHandle


--- @class EclStatusIdentify:EclStatus
--- @field Identifier ComponentHandle
--- @field Level int32


--- @class EclStatusIncapacitated:EclStatusConsumeBase
--- @field CurrentFreezeTime number
--- @field FreezeTime number


--- @class EclStatusKnockedDown:EclStatus
--- @field EffectHandler EclMultiEffectHandler
--- @field KnockedDownState int32


--- @class EclStatusLeadership:EclStatusConsumeBase
--- @field Strength number


--- @class EclStatusMachine
--- @field IsStatusMachineActive boolean
--- @field OwnerObjectHandle ComponentHandle
--- @field PreventStatusApply boolean
--- @field Statuses EclStatus[]


--- @class EclStatusMaterial:EclStatus
--- @field ApplyFlags StatusMaterialApplyFlags
--- @field ApplyNormalMap boolean
--- @field Fading boolean
--- @field Force boolean
--- @field HasVisuals boolean
--- @field IsOverlayMaterial boolean
--- @field MaterialUUID FixedString


--- @class EclStatusPolymorphed:EclStatusConsumeBase
--- @field DisableInteractions boolean
--- @field PolymorphResult FixedString


--- @class EclStatusRepair:EclStatus
--- @field Repaired int32
--- @field RepairerHandle ComponentHandle


--- @class EclStatusRotate:EclStatus
--- @field Yaw number


--- @class EclStatusSitting:EclStatus
--- @field Index int32
--- @field ItemHandle ComponentHandle
--- @field NetId int32
--- @field Position vec3


--- @class EclStatusSmelly:EclStatus
--- @field EffectHandler EclMultiEffectHandler
--- @field OverlayMaterial FixedString


--- @class EclStatusSneaking:EclStatus
--- @field ClientRequestStop boolean
--- @field EffectHandler EclMultiEffectHandler
--- @field Failed boolean


--- @class EclStatusSpark:EclStatusConsumeBase
--- @field Charges int32


--- @class EclStatusSpirit:EclStatus
--- @field Characters NetId[]


--- @class EclStatusSummoning:EclStatus
--- @field AnimationDuration number
--- @field SummonLevel int32


--- @class EclStatusTeleportFall:EclStatus
--- @field EffectHandler EclMultiEffectHandler
--- @field HasDamage boolean
--- @field ReappearTime number
--- @field SkillId FixedString
--- @field Target vec3


--- @class EclStatusThrown:EclStatus
--- @field AnimationDuration number
--- @field IsThrowingSelf boolean
--- @field Landed boolean
--- @field LandingEstimate number
--- @field Level int32


--- @class EclStatusTutorialBed:EclStatus
--- @field AnimationDuration number
--- @field AnimationDuration2 number
--- @field BedHandle ComponentHandle


--- @class EclStatusUnlock:EclStatus
--- @field Unlocked int32


--- @class EclStatusUnsheathed:EclStatus
--- @field Force boolean


--- @class EclSubSurface
--- @field Changes EclSubSurfaceChangingCell[]
--- @field FadeInSpeed number
--- @field FadeOutSpeed number
--- @field Layer SurfaceLayer
--- @field SoundPositions vec3[]
--- @field SoundSurfaceRegionList EclSubSurfaceCell[]
--- @field Surface EclSurface
--- @field SurfaceTypeId SurfaceType
--- @field X uint8
--- @field Y uint8


--- @class EclSubSurfaceChangingCell
--- @field X int16
--- @field Y int16
--- @field field_4 int32
--- @field field_8 boolean


--- @class EclSubSurfaceCell
--- @field LayerIndex int16
--- @field SoundPositionIndex int16


--- @class EclSurface
--- @field AiGrid EocAiGrid
--- @field Cells i16vec2[]
--- @field Effects table<int32, ComponentHandle>[]
--- @field EntityHandle EntityHandle
--- @field FadeOut EclSurfaceFadeOutInstance[]
--- @field Flags uint8
--- @field Height int32
--- @field InstanceVisuals FixedString[]
--- @field NeedsSoundUpdate boolean
--- @field NumIntroFXVisuals uint64
--- @field OS_Vec3f vec3[]
--- @field RandomSeed uint32
--- @field Regions EclSurfaceRegion[]
--- @field SubSurfaces EclSubSurface[]
--- @field SurfaceManager EclSurfaceManager
--- @field SurfaceTypeId SurfaceType
--- @field Template SurfaceTemplate
--- @field Width int32


--- @class EclSurfaceFadeOutInstance
--- @field Position vec3


--- @class EclSurfaceManager
--- @field Level EclLevel
--- @field MetaData EclSurfaceMetaData[]
--- @field NeedsVisualReload boolean
--- @field Regions EclSurfaceRegion[]
--- @field ShouldReloadEffects boolean
--- @field ShouldReloadInstances boolean
--- @field SurfacesByType EclSurface[]


--- @class EclSurfaceMetaData
--- @field CombatTeamId EocCombatTeamId
--- @field OwnerHandle ComponentHandle
--- @field TimeElapsed number
--- @field field_14 boolean


--- @class EclSurfaceRegion
--- @field X int8
--- @field Y int8


--- @class EclTriggerConversionHelpers
--- @field RegisteredTriggers table<FixedString, Trigger[]>


--- @class EclTurnManager
--- @field CameraControl boolean
--- @field CombatGroups table<FixedString, EocCombatGroupInfo>
--- @field Combats table<uint8, EclTurnManagerCombat>
--- @field EntityHandles EntityHandle[]
--- @field PlayerIdToCombatantNetId table<int16, NetId>


--- @class EclTurnManagerCombat
--- @field CombatGroups ComponentHandleWithType[]
--- @field CurrentRoundTeams ComponentHandleWithType[]
--- @field InitialEnemyTeamId EocCombatTeamId
--- @field InitialPlayerTeamId EocCombatTeamId
--- @field LevelName FixedString
--- @field NextRoundTeams ComponentHandleWithType[]
--- @field OrderChanges EclTurnManagerCombatOrderChange[]
--- @field Teams table<uint32, ComponentHandleWithType>
--- @field TurnTimer number


--- @class EclTurnManagerCombatOrderChange
--- @field OrderIndex int32
--- @field TeamId uint32


--- @class EclUICursorInfo:EclEoCUI
--- @field ForceClearTooltipText boolean
--- @field HasSurfaceText boolean
--- @field IsActive boolean
--- @field RequestClearTooltipText boolean
--- @field SurfaceIndex int16
--- @field SurfaceIndex2 int16
--- @field SurfaceTurns int32
--- @field SurfaceTurns2 int32
--- @field Text string
--- @field TooltipArrayUpdated boolean
--- @field WorldScreenPositionX int32
--- @field WorldScreenPositionY int32


--- @class EclCharacterCreationCharacterCreationManager
--- @field CharCreationUI1 EclEoCUI
--- @field CharCreationUI2 EclEoCUI
--- @field CharCreationUIHandle1 ComponentHandle
--- @field CharCreationUIHandle2 ComponentHandle
--- @field Customizations table<NetId, EclCharacterCreationCharacterCustomization>
--- @field RefCounts table<NetId, int32>


--- @class EclCharacterCreationCharacterCustomization
--- @field ArmorState uint8
--- @field CharacterHandle ComponentHandle
--- @field State EclCharacterCreationCharacterCustomizationState
--- @field State2 EclCharacterCreationCharacterCustomizationState


--- @class EclCharacterCreationCharacterCustomizationState
--- @field AiPersonality TranslatedString
--- @field AttributePointsAssigned int32
--- @field CivilAbilityPointsAssigned int32
--- @field Class CharacterCreationClassDesc
--- @field Color1 CharacterCreationColorDesc
--- @field Color2 CharacterCreationColorDesc
--- @field Color3 CharacterCreationColorDesc
--- @field CombatAbilityPointsAssigned int32
--- @field Equipment FixedString
--- @field HairColor CharacterCreationColorDesc
--- @field Icon FixedString
--- @field IsMale boolean
--- @field MusicInstrumentID FixedString
--- @field Name string
--- @field Origin FixedString
--- @field RootTemplate FixedString
--- @field Skills FixedString[]
--- @field SkinColor CharacterCreationColorDesc
--- @field TalentPointsAssigned int32
--- @field VerifiedName string
--- @field VisualSetIndices uint64
--- @field VoiceId int32


--- @class EclCharacterCreationUICharacterCreationWizard:EclEoCUI
--- @field AssignedPoints EclCharacterCreationUICharacterCreationWizardPoints
--- @field AvailablePoints EclCharacterCreationUICharacterCreationWizardPoints
--- @field CCFlags uint8
--- @field CCFlags2 uint8
--- @field CharIconHeight int32
--- @field CharIconWidth int32
--- @field CharacterCreationManager EclCharacterCreationCharacterCreationManager
--- @field CharacterHandle ComponentHandle
--- @field CharacterNetId NetId
--- @field ChosenListCols uint16
--- @field ChosenListIconSize uint8
--- @field ChosenListSpacingH int32
--- @field ChosenListSpacingV int32
--- @field CurrentStep uint8
--- @field ExtraStatPointTalentFlags uint8
--- @field HelmetState uint8
--- @field IconSize number
--- @field IconSpacing number
--- @field IconSpacingH uint8
--- @field IconSpacingV uint8
--- @field IsMale boolean
--- @field ListSpacing uint8
--- @field MousePos int32
--- @field NumberOfCols uint8
--- @field OriginIndex uint8
--- @field PlayerIndex int32
--- @field Position vec3
--- @field RotateFlag uint8
--- @field Rotation number
--- @field SkillIconSize uint8
--- @field UserIconHeight number
--- @field UserIconWidth number
--- @field Visuals Visual[]
--- @field Voices FixedString[]
--- @field Zoom number
--- @field ZoomCameraDistance number
--- @field field_460 string


--- @class EclCharacterCreationUICharacterCreationWizardPoints
--- @field Ability uint8
--- @field Attribute uint8
--- @field Civil uint8
--- @field SkillSlots uint8
--- @field Talent uint8


--- @class EclLuaCreateEquipmentVisualsRequestEvent:LuaEventBase
--- @field Character EclCharacter
--- @field Entity EntityHandle
--- @field Params EclEquipmentVisualSystemSetParam


--- @class EclLuaGameStateChangedEvent:LuaEventBase
--- @field FromState EclGameState
--- @field ToState EclGameState


--- @class EclLuaInputEvent:LuaEventBase
--- @field Event InputEvent


--- @class EclLuaRawInputEvent:LuaEventBase
--- @field Input InjectInputData


--- @class EclLuaSkillGetDescriptionParamEvent:LuaEventBase
--- @field Character CDivinityStatsCharacter
--- @field Description string
--- @field IsFromItem boolean
--- @field Params string[]
--- @field Skill StatsSkillPrototype


--- @class EclLuaSkillGetPropertyDescriptionEvent:LuaEventBase
--- @field Description string
--- @field Property StatsPropertyExtender


--- @class EclLuaStatusGetDescriptionParamEvent:LuaEventBase
--- @field Description string
--- @field Owner StatsObjectInstance
--- @field Params string[]
--- @field Status StatsStatusPrototype
--- @field StatusSource StatsObjectInstance


--- @class EclLuaUICallEvent:LuaEventBase
--- @field Args IggyInvokeDataValue[]
--- @field Function string
--- @field UI UIObject
--- @field When UICallbackEventType


--- @class EclLuaUIObjectCreatedEvent:LuaEventBase
--- @field UI UIObject


--- @class ClientMultiVisualAddVisualOptions
--- @field AllowTPose boolean
--- @field ResetScale boolean
--- @field SyncAnimationWithParent boolean
--- @field Bone FixedString
--- @field Color1 vec4
--- @field Color2 vec4
--- @field Color3 vec4
--- @field Color4 vec4
--- @field Color5 vec4
--- @field ExcludeFromBounds boolean
--- @field KeepRot boolean
--- @field KeepScale boolean
--- @field UseLocalTransform boolean
--- @field InheritAnimations boolean
--- @field ParticleSystem boolean
--- @field DoNotUpdate boolean
--- @field Armor boolean
--- @field Weapon boolean
--- @field Wings boolean
--- @field Horns boolean
--- @field Overhead boolean
--- @field WeaponFX boolean
--- @field BonusWeaponFX boolean
--- @field WeaponOverlayFX boolean
--- @field CastShadow boolean
--- @field ReceiveDecal boolean
--- @field Reflecting boolean
--- @field IsShadowProxy boolean
--- @field AllowReceiveDecalWhenAnimated boolean



--- @class EclLuaVisualClientMultiVisual:EclMultiEffectHandler
--- @field AttachedVisuals ComponentHandle[]
--- @field Handle ComponentHandle
--- @field AddVisual fun(self:EclLuaVisualClientMultiVisual, visualId:FixedString, opts:ClientMultiVisualAddVisualOptions|nil):Visual
--- @field Delete fun(self:EclLuaVisualClientMultiVisual)
--- @field ParseFromStats fun(self:EclLuaVisualClientMultiVisual, effect:string, weaponBones:string|nil)


--- @class EocAi
--- @field AIBoundsHeight number
--- @field AIBoundsMax vec3
--- @field AIBoundsMin vec3
--- @field AIBoundsRadius number
--- @field AIBoundsSize number
--- @field AiBoundType int32
--- @field AiFlags uint64
--- @field AiFlags2 uint16
--- @field AiGrid EocAiGrid
--- @field GameObject IGameObject
--- @field MyHandle ComponentHandle
--- @field Settings uint16
--- @field UseOnDistance boolean
--- @field XZ vec2


--- @class EocAiGrid
--- @field GridScale number
--- @field Height uint32
--- @field OffsetX number
--- @field OffsetY number
--- @field OffsetZ number
--- @field Width uint32
--- @field FindCellsInRect fun(self:EocAiGrid, minX:number, minZ:number, maxX:number, maxZ:number, anyFlags:uint64, allFlags:uint64):vec2[]
--- @field GetAiFlags fun(self:EocAiGrid, x:number, z:number):uint64|nil
--- @field GetCellInfo fun(self:EocAiGrid, x:number, z:number):{Flags:integer, Height:number, Objects:ComponentHandle[]|nil}
--- @field GetHeight fun(self:EocAiGrid, x:number, z:number):number|nil
--- @field SearchForCell fun(self:EocAiGrid, x:number, z:number, radius:number, flags:ESurfaceFlag, bias:number):boolean
--- @field SetAiFlags fun(self:EocAiGrid, x:number, z:number, aiFlags:uint64)
--- @field SetHeight fun(self:EocAiGrid, x:number, z:number, height:number)
--- @field UpdateAiFlagsInRect fun(self:EocAiGrid, minX:number, minZ:number, maxX:number, maxZ:number, setFlags:uint64, clearFlags:uint64):boolean


--- @class EocAiPathCheckpoint
--- @field NetID NetId
--- @field X number
--- @field Y number


--- @class EocCombatComponent
--- @field Alignment1 FixedString
--- @field Alignment2 FixedString
--- @field Base BaseComponent
--- @field CanFight boolean
--- @field CanForceEndTurn boolean
--- @field CanGuard boolean
--- @field CanJoinCombat boolean
--- @field CanUseResistDeadTalent boolean
--- @field CombatAndTeamIndex EocCombatTeamId
--- @field CombatGroupId FixedString
--- @field CounterAttacked boolean
--- @field DelayDeathCount boolean
--- @field Flags EocCombatComponentFlags
--- @field FleeOnEndTurn boolean
--- @field GuardOnEndTurn boolean
--- @field Guarded boolean
--- @field HasAttackOfOpportunity boolean
--- @field InArena boolean
--- @field Initiative uint16
--- @field IsBoss boolean
--- @field IsInspector boolean
--- @field IsTicking boolean
--- @field RequestEndTurn boolean
--- @field RequestEnterCombat boolean
--- @field RequestTakeExtraTurn boolean
--- @field TookExtraTurn boolean
--- @field TurnEnded boolean


--- @class EocCombatGroupInfo
--- @field Id FixedString
--- @field Name TranslatedString


--- @class EocCombatTeamId
--- @field CombatId uint8
--- @field CombinedId uint32
--- @field TeamId uint32


--- @class EocItemDefinition
--- @field Active boolean
--- @field Amount uint32
--- @field CanBeMoved boolean
--- @field CanBePickedUp boolean
--- @field CanBeUsed boolean
--- @field CanUseRemotely boolean
--- @field CustomBookContent string
--- @field CustomDescription string
--- @field CustomDisplayName string
--- @field CustomRequirements boolean
--- @field DamageTypeOverwrite StatsDamageType
--- @field DeltaModSet FixedString[]
--- @field EquipmentStatsType uint32
--- @field Floating boolean
--- @field GMFolding boolean
--- @field GenerationBoostSet FixedString[]
--- @field GenerationItemType FixedString
--- @field GenerationLevel uint16
--- @field GenerationRandom uint32
--- @field GenerationStatsId FixedString
--- @field GoldValueOverwrite int32
--- @field HP int32
--- @field HasGeneratedStats boolean
--- @field HasModifiedSkills boolean
--- @field InventoryNetID NetId
--- @field InventorySubContainerNetID NetId
--- @field Invisible boolean
--- @field IsGlobal boolean
--- @field IsIdentified boolean
--- @field IsPinnedContainer boolean
--- @field ItemNetId NetId
--- @field ItemType FixedString
--- @field Key FixedString
--- @field Known boolean
--- @field LevelGroupIndex int8
--- @field LockLevel uint32
--- @field NameCool uint8
--- @field NameIndex int8
--- @field NetID NetId
--- @field OriginalRootTemplate FixedString
--- @field OriginalRootTemplateType uint32
--- @field PinnedContainerTags FixedString[]
--- @field RootGroupIndex int16
--- @field RootTemplate FixedString
--- @field RootTemplateType uint32
--- @field RuneBoostSet FixedString[]
--- @field Scale_M number
--- @field Skills FixedString
--- @field Slot int16
--- @field StatsEntryName FixedString
--- @field StatsLevel uint32
--- @field Tags FixedString[]
--- @field UUID FixedString
--- @field Version uint32
--- @field WeightValueOverwrite int32
--- @field WorldRot mat3
--- @field ResetProgression fun(self:EocItemDefinition)


--- @class EocPathMover
--- @field DestinationPos vec3
--- @field PathAcceleration number
--- @field PathInterpolateValue number
--- @field PathMaxArcDist number
--- @field PathMinArcDist number
--- @field PathRadius number
--- @field PathRandom uint8
--- @field PathRepeat uint64
--- @field PathRotateOrig quat
--- @field PathRotateTarget quat
--- @field PathRotationType int32
--- @field PathShift number
--- @field PathSpeed number
--- @field PathSpeedSet number
--- @field PathType uint8
--- @field StartingPosition vec3


--- @class EocPathParams
--- @field AiGridFlag0x10 boolean
--- @field AvoidTraps boolean
--- @field GridRadius int32
--- @field NoSurfaceEffects boolean
--- @field SurfacePathInfluences SurfacePathInfluence[]
--- @field XZ vec2
--- @field field_16 boolean


--- @class EocPlayerCustomData
--- @field AiPersonality FixedString
--- @field ClassType FixedString
--- @field ClothColor1 uint32
--- @field ClothColor2 uint32
--- @field ClothColor3 uint32
--- @field CustomLookEnabled boolean
--- @field HairColor uint32
--- @field Icon FixedString
--- @field IsMale boolean
--- @field MusicInstrument FixedString
--- @field Name string
--- @field NameTranslated TranslatedString
--- @field OriginName FixedString
--- @field OwnerProfileID FixedString
--- @field Race FixedString
--- @field ReservedProfileID FixedString
--- @field SkinColor uint32
--- @field Speaker FixedString


--- @class EocPlayerUpgrade
--- @field Abilities int32[]
--- @field AttributePoints uint32
--- @field Attributes int32[]
--- @field CivilAbilityPoints uint32
--- @field CombatAbilityPoints uint32
--- @field IsCustom boolean
--- @field TalentPoints uint32
--- @field Traits uint16[]


--- @class EocSkillBarItem
--- @field ItemHandle ComponentHandle
--- @field SkillOrStatId FixedString
--- @field Type EocSkillBarItemType


--- @class EsvAIHintAreaTrigger:EsvAreaTriggerBase
--- @field IsAnchor boolean


--- @class EsvASAttack:EsvActionState
--- @field AlwaysHit boolean
--- @field AnimationFinished boolean
--- @field DamageDurability boolean
--- @field DelayDeathCharacterHandles ComponentHandle[]
--- @field HitCount int32
--- @field HitCountOffHand int32
--- @field HitObject1 EsvHitObject
--- @field HitObject2 EsvHitObject
--- @field IsFinished boolean
--- @field MainHandHitType int32
--- @field MainWeaponDamageList table<ComponentHandle, StatsHitDamageInfo>
--- @field MainWeaponHandle ComponentHandle
--- @field OffHandDamageList table<ComponentHandle, StatsHitDamageInfo>
--- @field OffHandHitType int32
--- @field OffWeaponHandle ComponentHandle
--- @field ProjectileStartPosition vec3
--- @field ProjectileTargetPosition vec3
--- @field ProjectileUsesHitObject boolean
--- @field ShootCount int32
--- @field ShootCountOffHand int32
--- @field TargetHandle ComponentHandle
--- @field TargetPosition vec3
--- @field TimeRemaining number
--- @field TotalHitOffHand int32
--- @field TotalHits int32
--- @field TotalShoots int32
--- @field TotalShootsOffHand int32


--- @class EsvASPrepareSkill:EsvActionState
--- @field IsEntered boolean
--- @field IsFinished boolean
--- @field PrepareAnimationInit FixedString
--- @field PrepareAnimationLoop FixedString
--- @field SkillId FixedString


--- @class EsvASUseSkill:EsvActionState
--- @field OriginalSkill EsvSkillState
--- @field Skill EsvSkillState


--- @class EsvActionMachine
--- @field CharacterHandle ComponentHandle
--- @field Layers EsvActionMachineLayer[]


--- @class EsvActionMachineLayer
--- @field State EsvActionState


--- @class EsvActionState
--- @field TransactionId uint32
--- @field Type EsvActionStateType


--- @class EsvAiAction
--- @field AIParams SkillAIParams
--- @field APCost int32
--- @field APCost2 int32
--- @field ActionFinalScore number
--- @field ActionType AiActionType
--- @field AiHandle ComponentHandle
--- @field CloseEnough number
--- @field CloseEnough2 number
--- @field CostModifier number
--- @field Distance number
--- @field EndPosition vec3
--- @field EndPosition2 vec3
--- @field FinalScore number
--- @field FreeActionMultiplier number
--- @field IgnoreMovementScore boolean
--- @field IsFinished boolean
--- @field IsPositionCalculated boolean
--- @field LookAtPosition vec3
--- @field MagicCost int32
--- @field MovementFinalScore number
--- @field MovementSkillId FixedString
--- @field MovementSkillItem EsvItem
--- @field MovementSkillTargetPosition vec3
--- @field MovementType int32
--- @field PositionFinalScore number
--- @field SavingActionPoints boolean
--- @field Score1 EsvAiScore
--- @field ScoreWithoutMovement EsvAiScore
--- @field SkillId FixedString
--- @field SkillItemHandle ComponentHandle
--- @field Target2Handle ComponentHandle
--- @field TargetHandle ComponentHandle
--- @field TargetPosition2 vec3
--- @field TauntedSourceHandle ComponentHandle
--- @field UseMovementSkill boolean
--- @field field_94 int32
--- @field field_D8 boolean
--- @field field_DA boolean


--- @class EsvAiCombos
--- @field Combos EsvAiCombosCombo[]
--- @field DamageTypes StatsDamageType[]
--- @field SkillPrototypes FixedString[]
--- @field SurfaceComboIndices table<SurfaceType, uint64[]>


--- @class EsvAiCombosCombo
--- @field SurfaceType1 SurfaceType
--- @field SurfaceType2 SurfaceType
--- @field TransformType SurfaceTransformActionType
--- @field field_C uint8


--- @class EsvAiHelpers
--- @field AiFlagsStack uint8[]
--- @field AiTranslateOverrides table<ComponentHandle, EsvAiTranslateOverride>
--- @field AllyCharacters EsvCharacter[]
--- @field AllyCharacters2 EsvCharacter[]
--- @field CharacterAiRequests table<ComponentHandle, EsvAiRequest>
--- @field Combos EsvAiCombos
--- @field CurrentAiRequestObjects ComponentHandle[]
--- @field CurrentItem EsvItem
--- @field EnemyCharacters EsvCharacter[]
--- @field Flags uint8
--- @field HighestCharacterHP int32
--- @field Items EsvItem[]
--- @field KnownState EsvAiKnownStateSet
--- @field KnownStateStack EsvAiKnownStateSet[]
--- @field LowestCharacterHP int32
--- @field Modifiers EsvAiHelpersModifiers
--- @field NeutralCharacters EsvCharacter[]
--- @field OS_FixedString FixedString[]
--- @field OverrideTranslate vec3
--- @field PendingCharacters ComponentHandle[]
--- @field PreparingAiGrid EocAiGrid
--- @field PreparingCharacter EsvCharacter
--- @field PreparingLevel EsvLevel
--- @field SameCombatCharacters EsvCharacter[]
--- @field SameCombatEnemyCharacters EsvCharacter[]
--- @field SameCombatNeutralCharacters EsvCharacter[]
--- @field Surfaces EsvSurface[]


--- @class EsvAiHelpersModifiers
--- @field Modifiers number[]


--- @class EsvAiItemData
--- @field AIParams SkillAIParams
--- @field ItemHandle ComponentHandle


--- @class EsvAiKnownState
--- @field State EsvAiKnownStateImpl


--- @class EsvAiKnownStateImpl
--- @field States EsvAiKnownStateSet
--- @field States2 EsvAiKnownStateSet


--- @class EsvAiKnownStateSet
--- @field StatusInteractions EsvAiKnownStateSetStatusInteraction[]
--- @field SurfaceInteractions EsvAiKnownStateSetSurfaceInteraction[]
--- @field SurfaceOnObjectActions EsvAiKnownStateSetSurfaceOnObjectAction[]


--- @class EsvAiKnownStateSetStatusInteraction
--- @field HasLifeTime boolean
--- @field StatusId FixedString
--- @field TargetHandle ComponentHandle


--- @class EsvAiKnownStateSetSurfaceInteraction
--- @field SurfaceHandle ComponentHandle
--- @field SurfaceTransform SurfaceTransformActionType
--- @field SurfaceTypeByLayer SurfaceType[]
--- @field field_14 int32


--- @class EsvAiKnownStateSetSurfaceOnObjectAction
--- @field HasLifeTime boolean
--- @field SurfaceTypeId SurfaceType
--- @field TargetHandle ComponentHandle


--- @class EsvAiModifiers
--- @field ArchetypeNames FixedString[]
--- @field Archetypes table<FixedString, EsvAiHelpersModifiers>
--- @field BaseModifiers EsvAiHelpersModifiers


--- @class EsvAiRequest
--- @field ActionCount uint64
--- @field AiActionToExecute uint32
--- @field AiActions EsvAiAction[]
--- @field CalculationFinished boolean
--- @field CanMove boolean
--- @field CurrentAiActionIndex uint32
--- @field CurrentAiItemIndex uint32
--- @field CurrentAiSkill2Index uint32
--- @field CurrentAiSkillIndex uint32
--- @field HasAiOnPositionSkillData boolean
--- @field HighestActionScore number
--- @field IsCalculating boolean
--- @field Items EsvAiItemData[]
--- @field LastStep AiActionStep
--- @field OnPositionSkills EsvAiSkillData[]
--- @field Skills EsvAiSkillData[]
--- @field UnknownHandles ComponentHandle[]
--- @field UseBehaviorVariables boolean


--- @class EsvAiScore
--- @field Score EsvAiScoreImpl


--- @class EsvAiScoreImpl
--- @field DamageAmounts table<ComponentHandle, EsvAiScoreImplDamageAmount>
--- @field FailScore number
--- @field Flags1 uint16
--- @field FutureScore number
--- @field Reason AiScoreReasonFlags
--- @field SoftReasonFlags2 uint32
--- @field field_4 int32


--- @class EsvAiScoreImplDamageAmount
--- @field ArmorDamage number
--- @field BoostAmount number
--- @field ControlAmount number
--- @field DamageAmount number
--- @field DoTAmount number
--- @field HealAmount number
--- @field HoTAmount number
--- @field MagicArmorDamage number
--- @field PositionScore number


--- @class EsvAiSkillData
--- @field AIParams SkillAIParams
--- @field ItemHandle ComponentHandle
--- @field SkillId FixedString


--- @class EsvAiTranslateOverride
--- @field NewTranslate vec3
--- @field OriginalTranslate vec3


--- @class EsvAlignment:EsvHasRelationsObject
--- @field Entities EsvAlignmentEntity[]
--- @field MatrixIndex int32
--- @field MatrixIndex2 int32


--- @class EsvAlignmentContainer
--- @field Get fun(self:EsvAlignmentContainer):EsvAlignmentData
--- @field GetAll fun(self:EsvAlignmentContainer):ComponentHandle[]
--- @field IsPermanentEnemy fun(self:EsvAlignmentContainer, source:ComponentHandle, target:ComponentHandle):boolean
--- @field IsTemporaryEnemy fun(self:EsvAlignmentContainer, source:ComponentHandle, target:ComponentHandle):boolean
--- @field SetAlly fun(self:EsvAlignmentContainer, source:ComponentHandle, target:ComponentHandle, enabled:boolean)
--- @field SetPermanentEnemy fun(self:EsvAlignmentContainer, source:ComponentHandle, target:ComponentHandle, enabled:boolean)
--- @field SetTemporaryEnemy fun(self:EsvAlignmentContainer, source:ComponentHandle, target:ComponentHandle, enabled:boolean)


--- @class EsvAlignmentData
--- @field Alignment EsvAlignment
--- @field Handle ComponentHandle
--- @field HasOwnAlignment boolean
--- @field MatrixIndex int32
--- @field Name FixedString
--- @field NetID NetId
--- @field ParentAlignment EsvAlignment


--- @class EsvAlignmentEntity:EsvHasRelationsObject


--- @class EsvAreaTriggerBase:AreaTrigger


--- @class EsvAtmosphereTrigger:AtmosphereTrigger


--- @class EsvAura
--- @field AppliedAuras table<ComponentHandle, EsvAuraAppliedAura>
--- @field AuraAllies FixedString[]
--- @field AuraEnemies FixedString[]
--- @field AuraItems FixedString[]
--- @field AuraNeutrals FixedString[]
--- @field AuraSelf FixedString[]
--- @field Owner ComponentHandle
--- @field SomeObjHandle ComponentHandle
--- @field TickTimer number


--- @class EsvAuraAppliedAura
--- @field AppliedStatuses ComponentHandle[]
--- @field LifeTime number


--- @class EsvBaseController
--- @field Character ComponentHandle
--- @field TypeId int32


--- @class EsvChangeSurfaceOnPathAction:EsvCreateSurfaceActionBase
--- @field CheckExistingSurfaces boolean
--- @field FollowObject ComponentHandle
--- @field IgnoreIrreplacableSurfaces boolean
--- @field IgnoreOwnerCells boolean
--- @field IsFinished boolean
--- @field Radius number
--- @field SurfaceCollisionFlags uint32
--- @field SurfaceCollisionNotOnFlags uint32


--- @class EsvCharacter:IEoCServerObject
--- @field AI EocAi
--- @field ActionMachine EsvActionMachine
--- @field Activated boolean
--- @field AnimType uint8
--- @field AnimationOverride FixedString
--- @field AnimationSetOverride FixedString
--- @field AoOVelocityOverride vec3
--- @field Archetype FixedString
--- @field CanShootThrough boolean
--- @field CannotAttachToGroups boolean
--- @field CannotDie boolean
--- @field CannotMove boolean
--- @field CannotRun boolean
--- @field CharCreationInProgress boolean
--- @field CharacterControl boolean
--- @field CharacterCreationFinished boolean
--- @field Color uint8
--- @field CorpseCharacterHandle ComponentHandle
--- @field CorpseLootable boolean
--- @field CoverAmount boolean
--- @field CreatedTemplateItems FixedString[]
--- @field CrimeHandle ComponentHandle
--- @field CrimeInterrogationEnabled boolean
--- @field CrimeState uint32
--- @field CrimeWarningsEnabled boolean
--- @field CurrentLevel FixedString
--- @field CurrentTemplate CharacterTemplate
--- @field CustomBloodSurface SurfaceType
--- @field CustomDisplayName string
--- @field CustomLookEnabled boolean
--- @field CustomTradeTreasure FixedString
--- @field DamageCounter uint64
--- @field Deactivated boolean
--- @field Dead boolean
--- @field DeferredRemoveEscapist boolean
--- @field DelayDeathCount uint8
--- @field DelayedDyingStatus EsvStatus
--- @field Detached boolean
--- @field Dialog uint32
--- @field DialogController EsvTaskController
--- @field DisableCulling boolean
--- @field DisableWaypointUsage boolean
--- @field DisabledCrime FixedString[]
--- @field DoNotFaceFlag boolean
--- @field DontCacheTemplate boolean
--- @field DroppingItemHandle_M ComponentHandle
--- @field EnemyCharacterHandle ComponentHandle
--- @field EnemyHandles ComponentHandle[]
--- @field EquipmentColor FixedString
--- @field FightMode boolean
--- @field FindValidPositionOnActivate boolean
--- @field Flags EsvCharacterFlags
--- @field Flags2 EsvCharacterFlags2
--- @field Flags3 EsvCharacterFlags3
--- @field FlagsEx uint8
--- @field Floating boolean
--- @field FollowCharacterHandle ComponentHandle
--- @field ForceNonzeroSpeed boolean
--- @field ForceSynchCount uint8
--- @field GMReroll boolean
--- @field Global boolean
--- @field HasDefaultDialog boolean
--- @field HasOsirisDialog boolean
--- @field HasOwner boolean
--- @field HasRunSpeedOverride boolean
--- @field HasWalkSpeedOverride boolean
--- @field HealCounter uint64
--- @field HostControl boolean
--- @field IgnoresTriggers boolean
--- @field InArena boolean
--- @field InDialog boolean
--- @field InParty boolean
--- @field InventoryHandle ComponentHandle
--- @field InvestigationTimer uint32
--- @field Invulnerable boolean
--- @field IsAlarmed boolean
--- @field IsCompanion_M boolean
--- @field IsDialogAiControlled boolean
--- @field IsGameMaster boolean
--- @field IsHuge boolean
--- @field IsPet boolean
--- @field IsPlayer boolean
--- @field IsPossessed boolean
--- @field IsSpectating boolean
--- @field IsTrading boolean
--- @field KillCounter uint64
--- @field LevelTemplatePosition vec3
--- @field LevelTransitionPending boolean
--- @field LifeTime number
--- @field Loaded boolean
--- @field ManuallyLeveled boolean
--- @field MovementMachine EsvMovementMachine
--- @field MovingCasterHandle ComponentHandle
--- @field Multiplayer boolean
--- @field NeedsMakePlayerUpdate boolean
--- @field NeedsUpdateCount uint8
--- @field NextCheckpointPosition vec3
--- @field NoReptuationEffects boolean
--- @field NoRotate boolean
--- @field NoiseTimer number
--- @field NumConsumables uint8
--- @field OffStage boolean
--- @field OriginalTemplate CharacterTemplate
--- @field OriginalTransformDisplayName TranslatedString
--- @field OsirisController EsvTaskController
--- @field OwnerHandle ComponentHandle
--- @field PartialAP number
--- @field PartyFollower boolean
--- @field PartyHandle ComponentHandle
--- @field PlayerCustomData EocPlayerCustomData
--- @field PlayerData EsvPlayerData
--- @field PlayerUpgrade EocPlayerUpgrade
--- @field PreferredAiTarget FixedString[]
--- @field PreviousCrimeHandle ComponentHandle
--- @field PreviousCrimeState uint32
--- @field PreviousLevel FixedString
--- @field ProjectileTemplate FixedString
--- @field Radius number
--- @field ReadyCheckBlocked boolean
--- @field RealtimeWalkSpeed number
--- @field RegisteredForAutomatedDialog boolean
--- @field RegisteredTriggerHandles ComponentHandle[]
--- @field RegisteredTriggers FixedString[]
--- @field RequestStartTurn boolean
--- @field ReservedForDialog boolean
--- @field ReservedUserID UserId
--- @field Resurrected boolean
--- @field RootTemplate CharacterTemplate
--- @field RunSpeedOverride number
--- @field ScriptForceUpdateCount uint8
--- @field ServerControlRefCount uint32
--- @field SkillBeingPrepared FixedString
--- @field SkillManager EsvSkillManager
--- @field SomePositionOverride vec3
--- @field SpeedMultiplier number
--- @field SpiritCharacterHandle ComponentHandle
--- @field SpotSneakers boolean
--- @field Stats CDivinityStatsCharacter
--- @field StatusController EsvStatusController
--- @field StatusMachine EsvStatusMachine
--- @field StatusesFromItems table<ComponentHandle, StatsPropertyStatus[]>
--- @field StoryNPC boolean
--- @field Summon boolean
--- @field SummonHandles ComponentHandle[]
--- @field SurfacePathInfluences SurfacePathInfluence[]
--- @field Tags FixedString[]
--- @field TagsFromItems FixedString[]
--- @field Team uint8
--- @field TemplateUsedForSkills CharacterTemplate
--- @field Temporary boolean
--- @field TimeElapsed uint32
--- @field Totem boolean
--- @field Trader boolean
--- @field TreasureGeneratedForTrader boolean
--- @field Treasures FixedString[]
--- @field TriggerTrapsTimer number
--- @field TurnTimer number
--- @field UserID UserId
--- @field UserVars UserReturn
--- @field VisualSetIndices VisualSetIndices
--- @field VoiceSet FixedString[]
--- @field WalkSpeedOverride number
--- @field WalkThrough boolean
--- @field WorldPos vec3
--- @field WorldRot mat3
--- @field GetCustomStat fun(self:EsvCharacter, a1:FixedString):int32|nil
--- @field GetInventoryItems fun(self:EsvCharacter):FixedString[]
--- @field GetSkillInfo fun(self:EsvCharacter, skill:FixedString):EsvSkill
--- @field GetSkills fun(self:EsvCharacter):FixedString[]
--- @field GetSummons fun(self:EsvCharacter):FixedString[]
--- @field SetCustomStat fun(self:EsvCharacter, a1:FixedString, a2:int32):boolean
--- @field SetScale fun(self:EsvCharacter, a1:number)
local EsvCharacter = {}



--- Retrieves the GUID of all characters within the specified range.
--- Location: Lua/Server/ServerCharacter.inl:55
--- @param distance number Maximum character distance
--- @return FixedString[]
function EsvCharacter:GetNearbyCharacters(distance) end



--- @class EsvCharacterConversionHelpers
--- @field ActivatedCharacters table<FixedString, EsvCharacter[]>
--- @field RegisteredCharacters table<FixedString, EsvCharacter[]>


--- @class EsvCharacterManager
--- @field ActiveAnimationBlueprints EsvCharacterManagerAnimationBlueprintEntry[]
--- @field ActiveCharacters EsvCharacter[]
--- @field NetPendingTransforms EsvCharacterManagerTransformParams[]
--- @field PendingTransforms EsvCharacterManagerTransformParams[]
--- @field RegisteredCharacters EsvCharacter[]


--- @class EsvCharacterManagerAnimationBlueprintEntry
--- @field Character EsvCharacter


--- @class EsvCharacterManagerTransformParams
--- @field DiscardOriginalDisplayName boolean
--- @field DontCheckRootTemplate boolean
--- @field DontReplaceCombatState boolean
--- @field EquipmentSetName FixedString
--- @field Flags EsvCharacterTransformFlags
--- @field Immediate boolean
--- @field ImmediateSync boolean
--- @field MoveAttachedEffects boolean
--- @field PeerID int32
--- @field ReleasePlayerData boolean
--- @field ReplaceCurrentTemplate boolean
--- @field ReplaceCustomLooks boolean
--- @field ReplaceCustomNameIcon boolean
--- @field ReplaceEquipment boolean
--- @field ReplaceInventory boolean
--- @field ReplaceOriginalTemplate boolean
--- @field ReplaceScale boolean
--- @field ReplaceScripts boolean
--- @field ReplaceSkills boolean
--- @field ReplaceStats boolean
--- @field ReplaceTags boolean
--- @field ReplaceVoiceSet boolean
--- @field SaveOriginalDisplayName boolean
--- @field TargetCharacterHandle ComponentHandle
--- @field Template TemplateHandle
--- @field TemplateCharacterHandle ComponentHandle
--- @field TransformType EsvCharacterTransformType


--- @class EsvCombatComponent:EocCombatComponent


--- @class EsvCreatePuddleAction:EsvCreateSurfaceActionBase
--- @field CellAtGrow EsvSurfaceCell[]
--- @field ClosedCells EsvSurfaceCell[]
--- @field GrowSpeed number
--- @field GrowTimer number
--- @field IgnoreIrreplacableSurfaces boolean
--- @field IsFinished boolean
--- @field Step int32
--- @field SurfaceCells int32


--- @class EsvCreateSurfaceAction:EsvCreateSurfaceActionBase
--- @field CheckExistingSurfaces boolean
--- @field CurrentCellCount int32
--- @field ExcludeRadius number
--- @field GrowStep int32
--- @field GrowTimer number
--- @field IgnoreIrreplacableSurfaces boolean
--- @field MaxHeight number
--- @field Radius number
--- @field SurfaceCells EsvSurfaceCell[]
--- @field SurfaceCollisionFlags uint64
--- @field SurfaceCollisionNotOnFlags uint64
--- @field SurfaceLayer SurfaceLayer
--- @field Timer number


--- @class EsvCreateSurfaceActionBase:EsvSurfaceAction
--- @field Duration number
--- @field OwnerHandle ComponentHandle
--- @field Position vec3
--- @field StatusChance number
--- @field SurfaceHandlesByType ComponentHandle[]
--- @field SurfaceType SurfaceType


--- @class EsvCrimeAreaTrigger:EsvAreaTriggerBase
--- @field CrimeArea int32


--- @class EsvCrimeRegionTrigger:EsvAreaTriggerBase
--- @field TriggerRegion FixedString


--- @class EsvDamageHelpers
--- @field CriticalRoll StatsCriticalRoll
--- @field DamageSourceType CauseType
--- @field ForceReduceDurability boolean
--- @field HighGround StatsHighGroundBonus
--- @field HitReason uint32
--- @field HitType StatsHitType
--- @field NoHitRoll boolean
--- @field ProcWindWalker boolean
--- @field SimulateHit boolean
--- @field Strength number


--- @class EsvDefaultProjectileHit
--- @field CasterHandle ComponentHandle
--- @field IsFromItem boolean
--- @field Level int32
--- @field SkillId FixedString


--- @class EsvEffect:BaseComponent
--- @field BeamTarget ComponentHandle
--- @field BeamTargetBone FixedString
--- @field BeamTargetPos vec3
--- @field Bone FixedString
--- @field DetachBeam boolean
--- @field Duration number
--- @field EffectName FixedString
--- @field ForgetEffect boolean
--- @field IsDeleted boolean
--- @field IsForgotten boolean
--- @field Loop boolean
--- @field NetID NetId
--- @field Position vec3
--- @field Rotation mat3
--- @field Scale number
--- @field Target ComponentHandle
--- @field Delete fun(self:EsvEffect)


--- @class EsvEffectManager
--- @field DeletedEffects EsvEffect[]
--- @field Effects EsvEffect[]
--- @field ForgottenEffects EsvEffect[]


--- @class EsvEntityManager
--- @field CharacterConversionHelpers EsvCharacterConversionHelpers
--- @field ItemConversionHelpers EsvItemConversionHelpers
--- @field ProjectileConversionHelpers EsvProjectileConversionHelpers
--- @field TriggerConversionHelpers EsvTriggerConversionHelpers


--- @class EsvEnvironmentalInfluences
--- @field HasWeatherProofTalent boolean
--- @field OwnerHandle ComponentHandle
--- @field PermanentInfluences table<FixedString, EsvEnvironmentalInfluencesPermanentInfluence>
--- @field Statuses table<FixedString, EsvEnvironmentalInfluencesStatus>
--- @field TemporaryStatuses table<FixedString, EsvEnvironmentalInfluencesTemporaryStatus>
--- @field Unknown FixedString[]


--- @class EsvEnvironmentalInfluencesPermanentInfluence
--- @field Strength number
--- @field WeatherStrength number


--- @class EsvEnvironmentalInfluencesStatus
--- @field FirstAttempt boolean
--- @field Handle ComponentHandle
--- @field IsForced boolean


--- @class EsvEnvironmentalInfluencesTemporaryStatus
--- @field Handle ComponentHandle
--- @field IsForced boolean
--- @field LifeTime number
--- @field Strength number
--- @field WeatherStrength number


--- @class EsvEnvironmentalStatusManager
--- @field EnvironmentalInfluences table<ComponentHandle, EsvEnvironmentalInfluences>
--- @field Timer number


--- @class EsvEocAreaTrigger:EsvAreaTriggerBase


--- @class EsvEocPointTrigger:EsvPointTriggerBase


--- @class EsvEventTrigger:EsvAreaTriggerBase
--- @field EnterEvent FixedString
--- @field LeaveEvent FixedString


--- @class EsvExplorationTrigger:EsvAreaTriggerBase
--- @field ExplorationReward int32
--- @field OS_FS FixedString[]


--- @class EsvExtinguishFireAction:EsvCreateSurfaceActionBase
--- @field ExtinguishPosition vec3
--- @field GrowTimer number
--- @field Percentage number
--- @field Radius number
--- @field Step number


--- @class EsvGameAction
--- @field ActionType GameActionType
--- @field ActivateTimer number
--- @field Active boolean
--- @field Dirty boolean
--- @field Handle ComponentHandle
--- @field NetID NetId


--- @class EsvGameActionManager
--- @field GameActions EsvGameAction[]


--- @class EsvGameObjectMoveAction:EsvGameAction
--- @field BeamEffectName FixedString
--- @field CasterCharacterHandle ComponentHandle
--- @field DoneMoving boolean
--- @field Owner ComponentHandle
--- @field PathMover EocPathMover


--- @class EsvHasRelationsObject
--- @field Handle ComponentHandle
--- @field Name FixedString
--- @field NetID NetId
--- @field TemporaryRelations table<ComponentHandle, int32>
--- @field TemporaryRelations2 table<ComponentHandle, int32>


--- @class EsvHitObject
--- @field HitInterpolation int32
--- @field Position vec3
--- @field Target ComponentHandle


--- @class EsvInventory
--- @field BuyBackAmounts table<ComponentHandle, uint32>
--- @field CachedGoldAmount int32
--- @field CachedWeight int32
--- @field EquipmentSlots uint8
--- @field GUID FixedString
--- @field Handle ComponentHandle
--- @field IsGlobal boolean
--- @field ItemsBySlot ComponentHandle[]
--- @field NetID NetId
--- @field ParentHandle ComponentHandle
--- @field PinnedContainers ComponentHandle[]
--- @field TimeItemAddedToInventory table<ComponentHandle, uint32>
--- @field UpdateViews ComponentHandle[]
--- @field Views table<int32, ComponentHandle>


--- @class EsvInventoryView
--- @field Handle ComponentHandle
--- @field ItemIndices table<ComponentHandle, int32>
--- @field Items ComponentHandle[]
--- @field NetID NetId
--- @field Owner ComponentHandle
--- @field ParentType uint32
--- @field Parents ComponentHandle[]
--- @field PinnedContainerTags ComponentHandle[]
--- @field ViewId uint32


--- @class EsvItem:IEoCServerObject
--- @field AI EocAi
--- @field Activated boolean
--- @field Amount uint32
--- @field Armor uint32
--- @field CanBeMoved boolean
--- @field CanBePickedUp boolean
--- @field CanConsume boolean
--- @field CanOnlyBeUsedByOwner boolean
--- @field CanShootThrough boolean
--- @field CanUse boolean
--- @field ClientSync1 boolean
--- @field ComputedVitality int32
--- @field CurrentLevel FixedString
--- @field CurrentTemplate ItemTemplate
--- @field CustomBookContent string
--- @field CustomDescription string
--- @field CustomDisplayName string
--- @field Destroy boolean
--- @field Destroyed boolean
--- @field DisableInventoryView80 boolean
--- @field DisableSync boolean
--- @field DontAddToHotbar boolean
--- @field Flags EsvItemFlags
--- @field Flags2 EsvItemFlags2
--- @field Floating boolean
--- @field ForceClientSync boolean
--- @field ForceSync boolean
--- @field ForceSynch boolean
--- @field FreezeGravity boolean
--- @field Frozen boolean
--- @field GMFolding boolean
--- @field Generation EsvItemGeneration
--- @field Global boolean
--- @field GoldValueOverwrite int32
--- @field HideHP boolean
--- @field InAutomatedDialog boolean
--- @field InPartyInventory boolean
--- @field InUseByCharacterHandle ComponentHandle
--- @field InteractionDisabled boolean
--- @field InventoryHandle ComponentHandle
--- @field Invisible boolean
--- @field Invisible2 boolean
--- @field Invulnerable boolean
--- @field Invulnerable2 boolean
--- @field IsContainer boolean
--- @field IsDoor boolean
--- @field IsKey boolean
--- @field IsLadder boolean
--- @field IsMoving boolean
--- @field IsSecretDoor boolean
--- @field IsSurfaceBlocker boolean
--- @field IsSurfaceCloudBlocker boolean
--- @field Key FixedString
--- @field Known boolean
--- @field LevelOverride int32
--- @field LevelTransitionPending boolean
--- @field LoadedTemplate boolean
--- @field LockLevel uint32
--- @field LuckyFind boolean
--- @field NoCover boolean
--- @field OffStage boolean
--- @field OriginalOwnerCharacter ComponentHandle
--- @field OriginalTemplateType uint64
--- @field OwnerHandle ComponentHandle
--- @field ParentInventoryHandle ComponentHandle
--- @field PinnedContainer boolean
--- @field PositionChanged boolean
--- @field PreviousLevel FixedString
--- @field Rarity FixedString
--- @field ReservedForDialog boolean
--- @field RootTemplate ItemTemplate
--- @field Slot uint16
--- @field SourceContainer boolean
--- @field Stats CDivinityStatsItem
--- @field StatsFromName StatsObject
--- @field StatsId FixedString
--- @field StatusMachine EsvStatusMachine
--- @field Sticky boolean
--- @field StoryItem boolean
--- @field Summon boolean
--- @field Tags FixedString[]
--- @field TeleportOnUse boolean
--- @field TeleportTargetOverride uint64
--- @field TeleportUseCount int32
--- @field Totem boolean
--- @field TransformChanged boolean
--- @field TreasureGenerated boolean
--- @field TreasureLevel int32
--- @field UnEquipLocked boolean
--- @field UnsoldGenerated boolean
--- @field UseRemotely boolean
--- @field UserId uint32
--- @field UserVars UserReturn
--- @field VisualResourceID FixedString
--- @field Vitality uint32
--- @field WakePhysics boolean
--- @field WalkOn boolean
--- @field WalkThrough boolean
--- @field WeightValueOverwrite int32
--- @field WorldPos vec3
--- @field GetDeltaMods fun(self:EsvItem):FixedString[]
--- @field GetGeneratedBoosts fun(self:EsvItem):FixedString[]
--- @field GetInventoryItems fun(self:EsvItem):FixedString[]
--- @field GetNearbyCharacters fun(self:EsvItem, distance:number):FixedString[]
--- @field SetDeltaMods fun(self:EsvItem)
--- @field SetGeneratedBoosts fun(self:EsvItem)


--- @class EsvItemConversionHelpers
--- @field ActivatedItems table<FixedString, EsvItem[]>
--- @field GlobalItemHandles table<FixedString, ComponentHandle>
--- @field RegisteredItems table<FixedString, EsvItem[]>


--- @class EsvItemGeneration
--- @field Base FixedString
--- @field Boosts FixedString[]
--- @field ItemType FixedString
--- @field Level uint16
--- @field Random uint32


--- @class EsvItemManager
--- @field ActiveItems EsvItem[]
--- @field Items EsvItem[]
--- @field Mover EsvItemMover
--- @field NetPendingTransforms EsvItemManagerTransformParams[]
--- @field PendingTransforms EsvItemManagerTransformParams[]


--- @class EsvItemManagerTransformParams
--- @field Flags EsvItemTransformFlags
--- @field Immediate boolean
--- @field ItemHandle ComponentHandle
--- @field ReplaceScripts boolean
--- @field ReplaceStats boolean
--- @field TemplateHandle TemplateHandle


--- @class EsvItemMovement
--- @field AiBounds vec3
--- @field DoHitTest boolean
--- @field HeightForced boolean
--- @field InventoryAdd EsvItemMovementInventoryAddParams
--- @field ItemHandle ComponentHandle
--- @field MoveEventName string
--- @field MoverHandle ComponentHandle
--- @field Moving boolean
--- @field MovingInWorld boolean
--- @field MovingToInventory boolean
--- @field WakePhysics boolean


--- @class EsvItemMovementInventoryAddParams
--- @field Flags uint32
--- @field InventoryNetId NetId
--- @field OwnerCharacterHandle ComponentHandle
--- @field Slot StatsItemSlot


--- @class EsvItemMover
--- @field Movements table<ComponentHandle, EsvItemMovement>


--- @class EsvLevel:Level
--- @field AiGrid EocAiGrid
--- @field CharacterManager EsvCharacterManager
--- @field EffectManager EsvEffectManager
--- @field EntityManager EsvEntityManager
--- @field EnvironmentalStatusManager EsvEnvironmentalStatusManager
--- @field GameActionManager EsvGameActionManager
--- @field ItemManager EsvItemManager
--- @field LevelCacheTemplateManager LevelCacheTemplateManager
--- @field ProjectileManager EsvProjectileManager
--- @field SurfaceManager EsvSurfaceManager


--- @class EsvLevelManager
--- @field CurrentLevel EsvLevel
--- @field LevelDescs LevelDesc[]
--- @field Levels table<FixedString, EsvLevel>
--- @field Levels2 table<FixedString, EsvLevel>


--- @class EsvMSMoveTo:EsvMovementState
--- @field AiBounds_M vec3
--- @field AiFlags uint64
--- @field AiFloodDone boolean
--- @field AiHandle ComponentHandle
--- @field AiPathId int32
--- @field AutoFreeMovement boolean
--- @field CanRun boolean
--- @field CannotMove boolean
--- @field CharCurrentPositionXY vec2
--- @field CharacterScale number
--- @field CloseEnoughDistMax number
--- @field CloseEnoughDistMin number
--- @field CurPosition vec3
--- @field CurPositionOld vec3
--- @field CurPositionOld2 vec3
--- @field FreeMovement boolean
--- @field Horizon int16
--- @field IsPathfinding boolean
--- @field MovementAP number
--- @field NextAiHandle ComponentHandle
--- @field NextCloseEnoughDistMax number
--- @field NextCloseEnoughDistMin number
--- @field NextHorizon int16
--- @field NextSurfaceNavigationType int32
--- @field NextTargetCheckType int32
--- @field NextUseCharacterRadius boolean
--- @field Nextfield_46 uint8
--- @field PathParams EocPathParams
--- @field PathTimeout2 number
--- @field Paused boolean
--- @field Position vec3
--- @field Position2 vec3
--- @field PositionUpdated boolean
--- @field Speed number
--- @field SurfaceNavigationType int32
--- @field TargetAiGridFlag0x10 boolean
--- @field TargetCheckType int32
--- @field TimeUntilNextPathfinding number
--- @field TimeUntilNextPathing number
--- @field UseCharacterRadius boolean
--- @field field_D4 int32
--- @field field_DC boolean
--- @field field_DD boolean
--- @field field_E4 int32


--- @class EsvMSMovement:EsvMovementState


--- @class EsvMoveTask:EsvTask
--- @field ArriveEvent string
--- @field Checkpoints EocAiPathCheckpoint[]
--- @field CurrentTarget vec3
--- @field MoveTransactionId uint32
--- @field TimeSpent number
--- @field TimeTargetFound number


--- @class EsvMovementMachine
--- @field Active boolean
--- @field CharacterHandle ComponentHandle
--- @field Layers EsvMovementState[]


--- @class EsvMovementState
--- @field TransactionId int32
--- @field Type EsvMovementStateType


--- @class EsvMusicVolumeTrigger:EsvAreaTriggerBase
--- @field MusicEvents MusicVolumeTriggerDataMusicEvent[]
--- @field TriggeredBy EsvMusicVolumeTriggerTriggered[]


--- @class EsvMusicVolumeTriggerTriggered
--- @field Index int16
--- @field Slot int16


--- @class EsvOsirisAppearTask:EsvTask
--- @field Angle number
--- @field Animation FixedString
--- @field FinishedEvent string
--- @field OnTrail boolean
--- @field OutOfSight boolean
--- @field PlayerSpawn boolean
--- @field SpawnState uint32
--- @field Target ComponentHandle
--- @field TargetPos vec3


--- @class EsvOsirisAttackTask:EsvTask
--- @field AlwaysHit boolean
--- @field ArriveEvent string
--- @field BehaviorTransactionId uint32
--- @field Target ComponentHandle
--- @field TargetPos vec3
--- @field WithoutMove boolean


--- @class EsvOsirisDisappearTask:EsvTask
--- @field Angle number
--- @field DefaultSpeed number
--- @field DisappearCount int32
--- @field FinishedEvent string
--- @field IncreaseSpeed boolean
--- @field OffStage boolean
--- @field OutOfSight boolean
--- @field Running boolean
--- @field SpeedMultiplier number
--- @field Target ComponentHandle
--- @field TargetPos vec3
--- @field ValidTarget boolean


--- @class EsvOsirisDropTask:EsvTask
--- @field Item ComponentHandle
--- @field TargetPos vec3


--- @class EsvOsirisFleeTask:EsvMoveTask
--- @field FleeFromRelation int32
--- @field FleeFromRelationRange number
--- @field FleeFromTileStates uint64
--- @field OutOfSight boolean
--- @field StartPosition vec3
--- @field StartedMoving boolean
--- @field SurfacePathInfluences SurfacePathInfluence[]


--- @class EsvOsirisFollowNPCTask:EsvTask
--- @field Target ComponentHandle


--- @class EsvOsirisMoveInRangeTask:EsvMoveTask
--- @field AttackMove boolean
--- @field CachedCloseEnough number
--- @field CachedResult boolean
--- @field CachedTarget vec3
--- @field CachedTargetPos vec3
--- @field FallbackMoveCloser boolean
--- @field HintTriggers EsvAIHintAreaTrigger[]
--- @field MaxRange number
--- @field MinRange number
--- @field MustBeInTrigger boolean
--- @field ProjectileTemplate GameObjectTemplate
--- @field Skill StatsSkillPrototype
--- @field Target ComponentHandle
--- @field TargetPos vec3
--- @field WantedRange number


--- @class EsvOsirisMoveItemTask:EsvTask
--- @field Amount int32
--- @field ArriveEvent string
--- @field BehaviorTransactionId uint32
--- @field Item ComponentHandle
--- @field Position vec3


--- @class EsvOsirisMoveToAndTalkTask:EsvTask
--- @field BehaviorTransactionId uint32
--- @field DialogInstanceID FixedString
--- @field IsAutomatedDialog boolean
--- @field Movement FixedString
--- @field Target ComponentHandle
--- @field Timeout number


--- @class EsvOsirisMoveToLocationTask:EsvMoveTask
--- @field MaxDistance number
--- @field MinDistance number
--- @field TargetRotation number
--- @field TargetRotationSet boolean
--- @field TargetToIgnore ComponentHandle


--- @class EsvOsirisMoveToObjectTask:EsvMoveTask
--- @field DefaultSpeed number
--- @field IncreaseSpeed boolean
--- @field MaxDistance number
--- @field MinDistance number
--- @field SpeedMultiplier number
--- @field Target ComponentHandle


--- @class EsvOsirisPickupItemTask:EsvTask
--- @field ArriveEvent string
--- @field BehaviorTransactionId uint32
--- @field Item ComponentHandle


--- @class EsvOsirisPlayAnimationTask:EsvTask
--- @field ActionTransactionId uint32
--- @field Animation FixedString
--- @field AnimationDuration number
--- @field AnimationNames FixedString[]
--- @field CurrentTime number
--- @field EndAnimation FixedString
--- @field ExitOnFinish boolean
--- @field FinishedEvent string
--- @field NoBlend boolean
--- @field OriginalAnimation FixedString
--- @field Time number
--- @field Timer number
--- @field WaitForCompletion boolean


--- @class EsvOsirisResurrectTask:EsvTask
--- @field Animation FixedString
--- @field HPPercentage int32
--- @field IsResurrected boolean


--- @class EsvOsirisSteerTask:EsvTask
--- @field AngleTolerance number
--- @field LookAt boolean
--- @field SnapToTarget boolean
--- @field SteeringTransactionId uint32
--- @field Target ComponentHandle
--- @field TargetPos vec3


--- @class EsvOsirisTeleportToLocationTask:EsvTask
--- @field ArriveEvent string
--- @field Executed boolean
--- @field FindFleePosition boolean
--- @field FindPosition boolean
--- @field LeaveCombat boolean
--- @field Level FixedString
--- @field Position vec3
--- @field PreviousLevel FixedString
--- @field Rotation mat3
--- @field SetRotation boolean
--- @field UnchainFollowers boolean


--- @class EsvOsirisUseItemTask:EsvTask
--- @field ArriveEvent string
--- @field BehaviorTransactionId uint32
--- @field Item ComponentHandle


--- @class EsvOsirisUseSkillTask:EsvTask
--- @field BehaviorTransactionId uint32
--- @field Force boolean
--- @field IgnoreChecks boolean
--- @field IgnoreHasSkill boolean
--- @field Skill FixedString
--- @field Success boolean
--- @field Target ComponentHandle
--- @field TargetPos vec3


--- @class EsvOsirisWanderTask:EsvTask
--- @field Anchor ComponentHandle
--- @field Duration number
--- @field Range number
--- @field Running boolean
--- @field Start vec3
--- @field Trigger ComponentHandle


--- @class EsvPathAction:EsvGameAction
--- @field Anchor uint64
--- @field Distance number
--- @field Finished boolean
--- @field HitCharacters ComponentHandle[]
--- @field HitItems ComponentHandle[]
--- @field HitRadius number
--- @field Interpolation number
--- @field IsFromItem boolean
--- @field Owner ComponentHandle
--- @field Position vec3
--- @field PreviousAnchor uint64
--- @field SkillId FixedString
--- @field SkillProperties StatsPropertyList
--- @field Speed number
--- @field SurfaceAction ComponentHandle
--- @field Target vec3
--- @field TurnTimer number
--- @field Waypoints vec3[]


--- @class EsvPendingHit
--- @field AttackerHandle ComponentHandle
--- @field CapturedCharacterHit boolean
--- @field CharacterHit StatsHitDamageInfo
--- @field CharacterHitDamageList StatsDamagePairList
--- @field CharacterHitPointer StatsHitDamageInfo
--- @field CriticalRoll StatsCriticalRoll
--- @field ForceReduceDurability boolean
--- @field HighGround StatsHighGroundBonus
--- @field HitType StatsHitType
--- @field Id uint32
--- @field NoHitRoll boolean
--- @field ProcWindWalker boolean
--- @field Status EsvStatusHit
--- @field TargetHandle ComponentHandle
--- @field WeaponStats CDivinityStatsItem


--- @class EsvPlayerCustomData:EocPlayerCustomData


--- @class EsvPlayerData
--- @field CachedTension uint32
--- @field CustomData EsvPlayerCustomData
--- @field HelmetOption boolean
--- @field LevelUpMarker boolean
--- @field LockedAbility uint32[]
--- @field OriginalTemplate FixedString
--- @field PickpocketTarget ComponentHandle
--- @field PreviousPickpocketTargets table<ComponentHandle, EsvPlayerDataPickpocketData>
--- @field PreviousPositionId uint32
--- @field PreviousPositions vec3[]
--- @field QuestSelected FixedString
--- @field RecruiterHandle ComponentHandle
--- @field Region FixedString
--- @field Renown uint32
--- @field SelectedSkillSetIndex uint8
--- @field ShouldReevaluateSkillBar boolean
--- @field SkillBar EocSkillBarItem[]
--- @field SomeObjectHandle ComponentHandle


--- @class EsvPlayerDataPickpocketData
--- @field Value int64
--- @field Weight int64


--- @class EsvPointTriggerBase:Trigger


--- @class EsvPolygonSurfaceAction:EsvCreateSurfaceActionBase
--- @field Characters ComponentHandle[]
--- @field CurrentGrowTimer number
--- @field DamageList StatsDamagePairList
--- @field GrowStep int32
--- @field GrowTimer number
--- @field Items ComponentHandle[]
--- @field LastSurfaceCellCount int32
--- @field PolygonVertices vec2[]
--- @field PositionX number
--- @field PositionZ number
--- @field SurfaceCells EsvSurfaceCell[]


--- @class EsvProjectile:IEoCServerObject
--- @field AlwaysDamage boolean
--- @field BoostConditions uint8
--- @field CanDeflect boolean
--- @field CasterHandle ComponentHandle
--- @field CleanseStatuses FixedString
--- @field CurrentLevel FixedString
--- @field DamageList StatsDamagePairList
--- @field DamageSourceType CauseType
--- @field DamageType StatsDamageType
--- @field DeathType StatsDeathType
--- @field DivideDamage boolean
--- @field EffectHandle ComponentHandle
--- @field ExplodeRadius0 number
--- @field ExplodeRadius1 number
--- @field ForceTarget boolean
--- @field HitInterpolation number
--- @field HitObjectHandle ComponentHandle
--- @field IgnoreObjects boolean
--- @field IgnoreRoof boolean
--- @field IsFromItem boolean
--- @field IsTrap boolean
--- @field Launched boolean
--- @field LifeTime number
--- @field MovingEffectHandle ComponentHandle
--- @field OnHitAction EsvDefaultProjectileHit
--- @field Position vec3
--- @field PrevPosition vec3
--- @field PropertyList StatsPropertyList
--- @field ReduceDurability boolean
--- @field RequestDelete boolean
--- @field RootTemplate ProjectileTemplate
--- @field SkillId FixedString
--- @field SourceHandle ComponentHandle
--- @field SourcePosition vec3
--- @field SpawnEffect FixedString
--- @field SpawnFXOverridesImpactFX boolean
--- @field StatusClearChance number
--- @field TargetObjectHandle ComponentHandle
--- @field TargetPosition vec3
--- @field UseCharacterStats boolean
--- @field WeaponHandle ComponentHandle


--- @class EsvProjectileConversionHelpers
--- @field RegisteredProjectiles table<FixedString, EsvProjectile[]>


--- @class EsvProjectileManager
--- @field ActiveProjectiles EsvProjectile[]
--- @field ProjectilesToDestroy EsvProjectile[]


--- @class EsvProjectileTargetDesc
--- @field Target ComponentHandle
--- @field TargetPosition vec3
--- @field TargetPosition2 vec3


--- @class EsvRainAction:EsvGameAction
--- @field AreaRadius number
--- @field ConsequencesStartTime number
--- @field Duration number
--- @field Finished boolean
--- @field FirstTick boolean
--- @field IsFromItem boolean
--- @field LifeTime number
--- @field Owner ComponentHandle
--- @field Position vec3
--- @field SkillId FixedString
--- @field SkillProperties FixedString
--- @field TurnTimer number


--- @class EsvRectangleSurfaceAction:EsvCreateSurfaceActionBase
--- @field AiFlags uint64
--- @field Characters ComponentHandle[]
--- @field CurrentCellCount uint64
--- @field CurrentGrowTimer number
--- @field DamageList StatsDamagePairList
--- @field DeathType StatsDeathType
--- @field GrowStep int32
--- @field GrowTimer number
--- @field Items ComponentHandle[]
--- @field Length number
--- @field LineCheckBlock uint64
--- @field MaxHeight number
--- @field SkillProperties StatsPropertyList
--- @field SurfaceArea number
--- @field SurfaceCells EsvSurfaceCell[]
--- @field Target vec3
--- @field Width number


--- @class EsvRegionTrigger:EsvAreaTriggerBase


--- @class EsvSecretRegionTrigger:EsvAreaTriggerBase
--- @field SecretRegionUnlocked boolean


--- @class EsvShootProjectileHelper
--- @field Caster ComponentHandle
--- @field CasterLevel int32
--- @field CleanseStatuses FixedString
--- @field DamageList StatsDamagePairList
--- @field EndPosition vec3
--- @field HitObject EsvHitObject
--- @field IgnoreObjects boolean
--- @field IsFromItem boolean
--- @field IsStealthed boolean
--- @field IsTrap boolean
--- @field Random uint8
--- @field SkillId FixedString
--- @field Source ComponentHandle
--- @field StartPosition vec3
--- @field StatusClearChance number
--- @field Target ComponentHandle
--- @field UnknownFlag1 boolean


--- @class EsvSkill
--- @field AIParams SkillAIParams
--- @field ActiveCooldown number
--- @field CauseList ComponentHandle[]
--- @field IsActivated boolean
--- @field IsLearned boolean
--- @field MaxCharges int32
--- @field NetID NetId
--- @field NumCharges int32
--- @field OncePerCombat boolean
--- @field OwnerHandle ComponentHandle
--- @field ShouldSyncCooldown boolean
--- @field SkillId FixedString
--- @field ZeroMemory boolean


--- @class EsvSkillManager
--- @field CurrentSkillState EsvSkillState
--- @field FreeMemorySlots uint32
--- @field IsLoading boolean
--- @field OwnerHandle ComponentHandle
--- @field Skills table<FixedString, EsvSkill>
--- @field TimeItemAddedToSkillManager table<FixedString, uint32>


--- @class EsvSkillState
--- @field CanEnter boolean
--- @field CharacterHandle ComponentHandle
--- @field CharacterHasSkill boolean
--- @field CleanseStatuses FixedString
--- @field IgnoreChecks boolean
--- @field IsFinished boolean
--- @field IsStealthed boolean
--- @field PrepareTimerRemaining number
--- @field ShouldExit boolean
--- @field SkillId FixedString
--- @field SourceItemHandle ComponentHandle
--- @field StatusClearChance number


--- @class EsvSkillStatusAura:EsvAura
--- @field AreaRadius number
--- @field Position vec3


--- @class EsvSoundVolumeTrigger:SoundVolumeTrigger


--- @class EsvStartTrigger:EsvPointTriggerBase
--- @field Angle number
--- @field Player uint8
--- @field Team uint8


--- @class EsvStatsAreaTrigger:EsvAreaTriggerBase
--- @field LevelOverride int32
--- @field ParentGuid Guid
--- @field TreasureLevelOverride int32


--- @class EsvStatus
--- @field BringIntoCombat boolean
--- @field CanEnterChance int32
--- @field Channeled boolean
--- @field CleansedByHandle ComponentHandle
--- @field CurrentLifeTime number
--- @field DamageSourceType CauseType
--- @field Flags0 EsvStatusFlags0
--- @field Flags1 EsvStatusFlags1
--- @field Flags2 EsvStatusFlags2
--- @field ForceFailStatus boolean
--- @field ForceStatus boolean
--- @field Influence boolean
--- @field InitiateCombat boolean
--- @field IsFromItem boolean
--- @field IsHostileAct boolean
--- @field IsInvulnerable boolean
--- @field IsLifeTimeSet boolean
--- @field IsOnSourceSurface boolean
--- @field IsResistingDeath boolean
--- @field KeepAlive boolean
--- @field LifeTime number
--- @field NetID NetId
--- @field OwnerHandle ComponentHandle
--- @field RequestClientSync boolean
--- @field RequestClientSync2 boolean
--- @field RequestDelete boolean
--- @field RequestDeleteAtTurnEnd boolean
--- @field StartTime number
--- @field StartTimer number
--- @field Started boolean
--- @field StatsMultiplier number
--- @field StatusHandle ComponentHandle
--- @field StatusId FixedString
--- @field StatusOwner ComponentHandle[]
--- @field StatusSourceHandle ComponentHandle
--- @field StatusType FixedString
--- @field Strength number
--- @field TargetHandle ComponentHandle
--- @field TurnTimer number


--- @class EsvStatusActiveDefense:EsvStatusConsumeBase
--- @field Charges int32
--- @field PreviousTargets ComponentHandle[]
--- @field Projectile FixedString
--- @field Radius number
--- @field StatusTargetHandle ComponentHandle
--- @field TargetPos vec3


--- @class EsvStatusAdrenaline:EsvStatusConsumeBase
--- @field CombatTurn int32
--- @field InitialAPMod int32
--- @field SecondaryAPMod int32


--- @class EsvStatusAoO:EsvStatus
--- @field ActivateAoOBoost boolean
--- @field AoOTargetHandle ComponentHandle
--- @field PartnerHandle ComponentHandle
--- @field ShowOverhead boolean
--- @field SourceHandle ComponentHandle


--- @class EsvStatusBoost:EsvStatus
--- @field BoostId FixedString
--- @field EffectTime number


--- @class EsvStatusChallenge:EsvStatusConsumeBase
--- @field SourceHandle ComponentHandle
--- @field Target boolean


--- @class EsvStatusCharmed:EsvStatusConsumeBase
--- @field OriginalOwnerCharacterHandle ComponentHandle
--- @field UserId uint32


--- @class EsvStatusClimbing:EsvStatus
--- @field Direction boolean
--- @field JumpUpLadders boolean
--- @field LadderHandle ComponentHandle
--- @field Level FixedString
--- @field MoveDirection vec3
--- @field Status int32


--- @class EsvStatusCombat:EsvStatus
--- @field OwnerTeamId EocCombatTeamId
--- @field ReadyForCombat boolean


--- @class EsvStatusConsume:EsvStatusConsumeBase


--- @class EsvStatusConsumeBase:EsvStatus
--- @field ApplyStatusOnTick FixedString
--- @field EffectTime number
--- @field HealEffectOverride HealEffect
--- @field ItemHandles ComponentHandle[]
--- @field Items FixedString[]
--- @field LoseControl boolean
--- @field OriginalWeaponStatsId FixedString
--- @field OverrideWeaponHandle ComponentHandle
--- @field OverrideWeaponStatsId FixedString
--- @field Poisoned boolean
--- @field ResetAllCooldowns boolean
--- @field ResetCooldownsAbilities uint32[]
--- @field ResetOncePerCombat boolean
--- @field SavingThrow int32
--- @field ScaleWithVitality boolean
--- @field Skill FixedString[]
--- @field SourceDirection vec3
--- @field StackId FixedString
--- @field StatsId FixedString
--- @field StatsIds EsvStatusConsumeBaseStatsData[]
--- @field SurfaceChanges SurfaceTransformActionType[]
--- @field Turn int32


--- @class EsvStatusConsumeBaseStatsData
--- @field StatsId FixedString
--- @field Turn int32


--- @class EsvStatusController:EsvBaseController
--- @field ActionTransactionId int32
--- @field CombatStartPosition vec3
--- @field CombatStartPositionFloodDone boolean
--- @field DeathAnimationTransactionId int32
--- @field Flags EsvStatusControllerFlags
--- @field Flags2 uint32
--- @field KnockDownQueued boolean
--- @field PolymorphingTransactionId int32
--- @field ResurrectedEvent FixedString
--- @field SteerToEnemyTransactionId int32
--- @field SummoningTransactionId int32
--- @field TeleportFallingTransactionId int32


--- @class EsvStatusDamage:EsvStatusConsumeBase
--- @field DamageEvent int32
--- @field DamageLevel int32
--- @field DamageStats FixedString
--- @field HitTimer number
--- @field SpawnBlood boolean
--- @field TimeElapsed number


--- @class EsvStatusDamageOnMove:EsvStatusDamage
--- @field DistancePerDamage number
--- @field DistanceTraveled number


--- @class EsvStatusDomeAction:EsvGameAction
--- @field AreaRadius number
--- @field Aura EsvSkillStatusAura
--- @field Finished boolean
--- @field LifeTime number
--- @field Owner ComponentHandle
--- @field Position vec3
--- @field SkillId FixedString


--- @class EsvStatusDrain:EsvStatus
--- @field Infused int32


--- @class EsvStatusDying:EsvStatus
--- @field AttackDirection int32
--- @field CombatId uint8
--- @field DeathType StatsDeathType
--- @field DieActionsCompleted boolean
--- @field DisputeTargetHandle ComponentHandle
--- @field DontThrowDeathEvent boolean
--- @field ForceNoGhost boolean
--- @field IgnoreGodMode boolean
--- @field ImpactDirection vec3
--- @field InflicterHandle ComponentHandle
--- @field IsAlreadyDead boolean
--- @field SkipAnimation boolean
--- @field SourceHandle ComponentHandle
--- @field SourceType int32


--- @class EsvStatusExplode:EsvStatus
--- @field Projectile FixedString


--- @class EsvStatusHeal:EsvStatus
--- @field AbsorbSurfaceRange int32
--- @field AbsorbSurfaceTypes SurfaceType[]
--- @field EffectTime number
--- @field HealAmount int32
--- @field HealEffect HealEffect
--- @field HealEffectId FixedString
--- @field HealType StatusHealType
--- @field TargetDependentHeal boolean
--- @field TargetDependentHealAmount int32[]
--- @field TargetDependentValue int32[]


--- @class EsvStatusHealSharing:EsvStatusConsumeBase
--- @field CasterHandle ComponentHandle


--- @class EsvStatusHealSharingCaster:EsvStatusConsumeBase
--- @field BeamEffects table<ComponentHandle, ComponentHandle>
--- @field StatusTargets ComponentHandle[]


--- @class EsvStatusHealing:EsvStatusConsumeBase
--- @field AbsorbSurfaceRange int32
--- @field HealAmount int32
--- @field HealEffect HealEffect
--- @field HealEffectId FixedString
--- @field HealStat StatusHealType
--- @field HealingEvent int32
--- @field SkipInitialEffect boolean
--- @field TimeElapsed number


--- @class EsvStatusHit:EsvStatus
--- @field AllowInterruptAction boolean
--- @field DecDelayDeathCount boolean
--- @field ForceInterrupt boolean
--- @field Hit StatsHitDamageInfo
--- @field HitByHandle ComponentHandle
--- @field HitReason uint32
--- @field HitWithHandle ComponentHandle
--- @field ImpactDirection vec3
--- @field ImpactOrigin vec3
--- @field ImpactPosition vec3
--- @field Interruption boolean
--- @field PropertyList StatsPropertyList
--- @field SkillId FixedString
--- @field WeaponHandle ComponentHandle


--- @class EsvStatusIdentify:EsvStatus
--- @field Identified int32
--- @field IdentifierHandle ComponentHandle
--- @field Level int32


--- @class EsvStatusInSurface:EsvStatus
--- @field Force boolean
--- @field Layers ESurfaceFlag
--- @field SurfaceDistanceCheck number
--- @field SurfaceTimerCheck number
--- @field Translate vec3


--- @class EsvStatusIncapacitated:EsvStatusConsumeBase
--- @field CurrentFreezeTime number
--- @field FreezeTime number
--- @field FrozenFlag uint8


--- @class EsvStatusInfectiousDiseased:EsvStatusConsumeBase
--- @field InfectTimer number
--- @field Infections int32
--- @field Radius number
--- @field StatusTargetHandle ComponentHandle


--- @class EsvStatusInvisible:EsvStatusConsumeBase
--- @field InvisiblePosition vec3


--- @class EsvStatusKnockedDown:EsvStatus
--- @field IsInstant boolean
--- @field KnockedDownState int32


--- @class EsvStatusLying:EsvStatus
--- @field Heal number
--- @field Index int32
--- @field ItemHandle ComponentHandle
--- @field Position vec3
--- @field TimeElapsed number


--- @class EsvStatusMachine
--- @field IsStatusMachineActive boolean
--- @field OwnerObjectHandle ComponentHandle
--- @field PreventStatusApply boolean
--- @field Statuses EsvStatus[]


--- @class EsvStatusMaterial:EsvStatus
--- @field ApplyFlags StatusMaterialApplyFlags
--- @field ApplyNormalMap boolean
--- @field Fading boolean
--- @field Force boolean
--- @field IsOverlayMaterial boolean
--- @field MaterialUUID FixedString


--- @class EsvStatusPolymorphed:EsvStatusConsumeBase
--- @field DisableInteractions boolean
--- @field OriginalTemplate FixedString
--- @field OriginalTemplateType int32
--- @field PolymorphResult FixedString
--- @field TransformedRace FixedString


--- @class EsvStatusRepair:EsvStatus
--- @field Level int32
--- @field Repaired int32
--- @field RepairerHandle ComponentHandle


--- @class EsvStatusRotate:EsvStatus
--- @field RotationSpeed number
--- @field Yaw number


--- @class EsvStatusShacklesOfPain:EsvStatusConsumeBase
--- @field CasterHandle ComponentHandle


--- @class EsvStatusShacklesOfPainCaster:EsvStatusConsumeBase
--- @field VictimHandle ComponentHandle


--- @class EsvStatusSneaking:EsvStatus
--- @field ClientRequestStop boolean


--- @class EsvStatusSpark:EsvStatusConsumeBase
--- @field Charges int32
--- @field Projectile FixedString
--- @field Radius number


--- @class EsvStatusSpirit:EsvStatus
--- @field Characters ComponentHandle[]


--- @class EsvStatusSpiritVision:EsvStatusConsumeBase
--- @field SpiritVisionSkillId FixedString


--- @class EsvStatusStance:EsvStatusConsumeBase
--- @field SkillId FixedString


--- @class EsvStatusSummoning:EsvStatus
--- @field AnimationDuration number
--- @field SummonLevel int32


--- @class EsvStatusTeleportFall:EsvStatus
--- @field HasDamage boolean
--- @field HasDamageBeenApplied boolean
--- @field ReappearTime number
--- @field SkillId FixedString
--- @field Target vec3


--- @class EsvStatusThrown:EsvStatus
--- @field AnimationDuration number
--- @field CasterHandle ComponentHandle
--- @field IsThrowingSelf boolean
--- @field Landed boolean
--- @field LandingEstimate number
--- @field Level int32


--- @class EsvStatusUnlock:EsvStatus
--- @field Key FixedString
--- @field Level int32
--- @field SourceHandle ComponentHandle
--- @field Unlocked int32


--- @class EsvStatusUnsheathed:EsvStatus
--- @field Force boolean


--- @class EsvStormAction:EsvGameAction
--- @field Finished boolean
--- @field IsFromItem boolean
--- @field LifeTime number
--- @field Owner ComponentHandle
--- @field Position vec3
--- @field ProjectileSkills FixedString[]
--- @field ProjectileTargets EsvProjectileTargetDesc[]
--- @field SkillId FixedString
--- @field StrikeTimer number
--- @field Strikes EsvStormActionStrike[]
--- @field TurnTimer number


--- @class EsvStormActionStrike
--- @field Object ComponentHandle
--- @field SkillId FixedString
--- @field Source vec3
--- @field Target vec3


--- @class EsvSurface
--- @field Flags uint8
--- @field Index uint16
--- @field LifeTime number
--- @field LifeTimeFromTemplate boolean
--- @field MyHandle ComponentHandle
--- @field NeedsSplitEvaluation boolean
--- @field NetID NetId
--- @field OwnerHandle ComponentHandle
--- @field OwnershipTimer number
--- @field RootTemplate SurfaceTemplate
--- @field StatusChance number
--- @field SurfaceType SurfaceType
--- @field TeamId EocCombatTeamId


--- @class EsvSurfaceAction
--- @field MyHandle ComponentHandle


--- @class EsvSurfaceCell
--- @field Position i16vec2


--- @class EsvSurfaceManager
--- @field SurfaceActions EsvSurfaceAction[]
--- @field SurfaceCellSetsByLayer EsvSurfaceCell[][]
--- @field SurfaceCells EsvSurfaceCell[]
--- @field Surfaces EsvSurface[]


--- @class EsvSwapSurfaceAction:EsvCreateSurfaceActionBase
--- @field CheckExistingSurfaces boolean
--- @field CurrentCellCount int32
--- @field ExcludeRadius number
--- @field GrowStep int32
--- @field GrowTimer number
--- @field IgnoreIrreplacableSurfaces boolean
--- @field LineCheckBlock uint64
--- @field MaxHeight number
--- @field Radius number
--- @field SurfaceCellMap table<SurfaceType, EsvSurfaceCell[]>
--- @field SurfaceCells EsvSurfaceCell[]
--- @field SurfaceCollisionFlags uint64
--- @field SurfaceCollisionNotOnFlags uint64
--- @field Target vec3
--- @field TargetCellMap table<SurfaceType, EsvSurfaceCell[]>
--- @field TargetCells EsvSurfaceCell[]
--- @field Timer number


--- @class EsvTask
--- @field Character ComponentHandle
--- @field Failed boolean
--- @field Flags uint32
--- @field TaskState uint32
--- @field TaskTypeId EsvTaskType


--- @class EsvTaskController:EsvBaseController
--- @field FlushRequested boolean
--- @field RemoveNextTask_M boolean
--- @field Tasks EsvTask[]
--- @field UpdateInProgress boolean


--- @class EsvTeleportTrigger:EsvPointTriggerBase
--- @field Angle number
--- @field Zoom boolean


--- @class EsvTornadoAction:EsvGameAction
--- @field AnchorList vec3[]
--- @field CleanseStatuses FixedString
--- @field Finished boolean
--- @field HitCharacterHandles ComponentHandle[]
--- @field HitItemHandles ComponentHandle[]
--- @field HitRadius number
--- @field Interpolation number
--- @field IsFromItem boolean
--- @field Owner ComponentHandle
--- @field Position vec3
--- @field SkillId FixedString
--- @field SkillProperties StatsPropertyList
--- @field StatusClearChance number
--- @field SurfaceActionHandle ComponentHandle
--- @field Target vec3
--- @field TurnTimer number


--- @class EsvTransformSurfaceAction:EsvSurfaceAction
--- @field Finished boolean
--- @field GrowCellPerSecond number
--- @field OriginSurface SurfaceType
--- @field OwnerHandle ComponentHandle
--- @field Position vec3
--- @field SurfaceCellMap table<SurfaceType, EsvSurfaceCell[]>
--- @field SurfaceLayer SurfaceLayer
--- @field SurfaceLifetime number
--- @field SurfaceMap table<SurfaceType, ComponentHandle>
--- @field SurfaceRemoveCloudCellMap EsvSurfaceCell[]
--- @field SurfaceRemoveGroundCellMap EsvSurfaceCell[]
--- @field SurfaceStatusChance number
--- @field SurfaceTransformAction SurfaceTransformActionType
--- @field Timer number


--- @class EsvTriggerConversionHelpers
--- @field RegisteredTriggers table<FixedString, Trigger[]>


--- @class EsvTurnManager
--- @field AttachedCombatComponents ComponentHandleWithType[]
--- @field CombatEntities EntityHandle[]
--- @field CombatEntities2 EntityHandle[]
--- @field CombatGroups table<FixedString, EsvTurnManagerCombatGroup>
--- @field CombatParticipants EsvCharacter[]
--- @field Combats table<uint8, EsvTurnManagerCombat>
--- @field EntitiesLeftCombat ComponentHandle[]
--- @field EntityWrappers EsvTurnManagerEntityWrapper[]
--- @field FreeCombatIds uint8[]
--- @field NextCombatId uint8
--- @field TeamMode int32
--- @field TimeoutOverrides table<uint8, EsvTurnManagerTimeoutOverride>


--- @class EsvTurnManagerCombat
--- @field CombatGroups EsvTurnManagerCombatGroup[]
--- @field CombatRound uint8
--- @field CombatStartEventSent boolean
--- @field CurrentRoundTeams EsvTurnManagerCombatTeam[]
--- @field CurrentTurnChangeNotificationTeamIds EocCombatTeamId[]
--- @field HasParticipantSurfaces boolean
--- @field HasParticipantSurfacesNumTicks uint8
--- @field InitialEnemyHandle ComponentHandle
--- @field InitialPlayerHandle ComponentHandle
--- @field IsActive boolean
--- @field IsFightBetweenPlayers boolean
--- @field LevelName FixedString
--- @field NewRound uint8
--- @field NextRoundTeams EsvTurnManagerCombatTeam[]
--- @field NextTeamId uint32
--- @field NextTurnChangeNotificationTeamIds EocCombatTeamId[]
--- @field Teams table<uint32, EsvTurnManagerCombatTeam>
--- @field TimeLeft_M number
--- @field TimeSpentInTurn number
--- @field TimeSpentTryingToEndTurn number
--- @field TurnTimer_M number
--- @field WaitingForCharComponents ComponentHandle[]
--- @field GetAllTeams fun(self:EsvTurnManagerCombat):table<uint32, EsvTurnManagerCombatTeam>
--- @field GetCurrentTurnOrder fun(self:EsvTurnManagerCombat):EsvTurnManagerCombatTeam[]
--- @field GetNextTurnOrder fun(self:EsvTurnManagerCombat):EsvTurnManagerCombatTeam[]
--- @field UpdateCurrentTurnOrder fun(self:EsvTurnManagerCombat)
--- @field UpdateNextTurnOrder fun(self:EsvTurnManagerCombat)


--- @class EsvTurnManagerCombatGroup
--- @field CombatTeamsOrdered EocCombatTeamId[]
--- @field Initiative uint16
--- @field LastAddedTeamIndex uint64
--- @field Party uint8


--- @class EsvTurnManagerCombatTeam
--- @field AddedNextTurnNotification boolean
--- @field Character EsvCharacter
--- @field CombatGroup EsvTurnManagerCombatGroup
--- @field CombatId uint8
--- @field CombatTeamRound uint16
--- @field ComponentHandle ComponentHandleWithType
--- @field EntityWrapper EsvTurnManagerEntityWrapper
--- @field Id EocCombatTeamId
--- @field Initiative uint16
--- @field Item EsvItem
--- @field StillInCombat boolean
--- @field TeamId uint32


--- @class EsvTurnManagerEntityWrapper
--- @field Character EsvCharacter
--- @field CombatComponentPtr EocCombatComponent
--- @field Handle EntityHandle
--- @field Item EsvItem


--- @class EsvTurnManagerTimeoutOverride
--- @field Handle ComponentHandleWithType
--- @field Timeout number


--- @class EsvWallAction:EsvGameAction
--- @field Finished boolean
--- @field GrowTimePerWall number
--- @field GrowTimeout number
--- @field IsFromItem boolean
--- @field LifeTime number
--- @field NumWallsGrown uint64
--- @field Owner ComponentHandle
--- @field SkillId FixedString
--- @field Source vec3
--- @field State int32
--- @field Target vec3
--- @field TimeSinceLastWall number
--- @field TurnTimer number
--- @field Walls ComponentHandle[]


--- @class EsvZoneAction:EsvCreateSurfaceActionBase
--- @field AiFlags uint64
--- @field AngleOrBase number
--- @field BackStart number
--- @field Characters ComponentHandle[]
--- @field CurrentCellCount uint64
--- @field DamageList StatsDamagePairList
--- @field DeathType StatsDeathType
--- @field FrontOffset number
--- @field GrowStep uint32
--- @field GrowTimer number
--- @field GrowTimerStart number
--- @field IsFromItem boolean
--- @field Items ComponentHandle[]
--- @field MaxHeight number
--- @field Radius number
--- @field Shape int32
--- @field SkillId FixedString
--- @field SkillProperties StatsPropertyList
--- @field SurfaceCells EsvSurfaceCell[]
--- @field Target vec3


--- @class EsvLuaAfterCraftingExecuteCombinationEvent:LuaEventBase
--- @field Character EsvCharacter
--- @field CombinationId FixedString
--- @field CraftingStation CraftingStationType
--- @field Items EsvItem[]
--- @field Quantity uint8
--- @field Succeeded boolean


--- @class EsvLuaAiRequestSortEvent:LuaEventBase
--- @field CharacterHandle ComponentHandle
--- @field Request EsvAiRequest


--- @class EsvLuaBeforeCharacterApplyDamageEvent:LuaEventBase
--- @field Attacker StatsObjectInstance
--- @field Cause CauseType
--- @field Context EsvPendingHit
--- @field Handled boolean
--- @field Hit StatsHitDamageInfo
--- @field ImpactDirection vec3
--- @field Target EsvCharacter


--- @class EsvLuaBeforeCraftingExecuteCombinationEvent:LuaEventBase
--- @field Character EsvCharacter
--- @field CombinationId FixedString
--- @field CraftingStation CraftingStationType
--- @field Items EsvItem[]
--- @field Processed boolean
--- @field Quantity uint8


--- @class EsvLuaBeforeShootProjectileEvent:LuaEventBase
--- @field Projectile EsvShootProjectileHelper


--- @class EsvLuaBeforeStatusApplyEvent:LuaEventBase
--- @field Owner IGameObject
--- @field PreventStatusApply boolean
--- @field Status EsvStatus


--- @class EsvLuaCalculateTurnOrderEvent:LuaEventBase
--- @field Combat EsvTurnManagerCombat


--- @class EsvLuaComputeCharacterHitEvent:LuaEventBase
--- @field AlwaysBackstab boolean
--- @field Attacker CDivinityStatsCharacter
--- @field CriticalRoll StatsCriticalRoll
--- @field DamageList StatsDamagePairList
--- @field ForceReduceDurability boolean
--- @field Handled boolean
--- @field HighGround StatsHighGroundBonus
--- @field Hit StatsHitDamageInfo
--- @field HitType StatsHitType
--- @field NoHitRoll boolean
--- @field SkillProperties StatsPropertyList
--- @field Target CDivinityStatsCharacter
--- @field Weapon CDivinityStatsItem


--- @class EsvLuaGameStateChangedEvent:LuaEventBase
--- @field FromState EsvGameState
--- @field ToState EsvGameState


--- @class EsvLuaGroundHitEvent:LuaEventBase
--- @field Caster IGameObject
--- @field DamageList StatsDamagePairList
--- @field Position vec3


--- @class EsvLuaOnExecutePropertyDataOnPositionEvent:LuaEventBase
--- @field AreaRadius number
--- @field Attacker IGameObject
--- @field Hit StatsHitDamageInfo
--- @field IsFromItem boolean
--- @field Position vec3
--- @field Property StatsPropertyExtender
--- @field Skill StatsSkillPrototype


--- @class EsvLuaOnExecutePropertyDataOnTargetEvent:LuaEventBase
--- @field Attacker IGameObject
--- @field Hit StatsHitDamageInfo
--- @field ImpactOrigin vec3
--- @field IsFromItem boolean
--- @field Property StatsPropertyExtender
--- @field Skill StatsSkillPrototype
--- @field Target IGameObject


--- @class EsvLuaOnPeekAiActionEvent:LuaEventBase
--- @field ActionType AiActionType
--- @field CharacterHandle ComponentHandle
--- @field IsFinished boolean
--- @field Request EsvAiRequest


--- @class EsvLuaProjectileHitEvent:LuaEventBase
--- @field HitObject IGameObject
--- @field Position vec3
--- @field Projectile EsvProjectile


--- @class EsvLuaShootProjectileEvent:LuaEventBase
--- @field Projectile EsvProjectile


--- @class EsvLuaStatusDeleteEvent:LuaEventBase
--- @field Status EsvStatus


--- @class EsvLuaStatusGetEnterChanceEvent:LuaEventBase
--- @field EnterChance int32|nil
--- @field IsEnterCheck boolean
--- @field Status EsvStatus


--- @class EsvLuaStatusHitEnterEvent:LuaEventBase
--- @field Context EsvPendingHit
--- @field Hit EsvStatusHit


--- @class EsvLuaTreasureItemGeneratedEvent:LuaEventBase
--- @field Item EsvItem
--- @field ResultingItem EsvItem


--- @class LuaDoConsoleCommandEvent:LuaEventBase
--- @field Command string


--- @class LuaEmptyEvent:LuaEventBase


--- @class LuaEventBase
--- @field ActionPrevented boolean
--- @field CanPreventAction boolean
--- @field Name FixedString
--- @field Stopped boolean
--- @field PreventAction fun(self:LuaEventBase)
--- @field StopPropagation fun(self:LuaEventBase)


--- @class LuaGetAttackAPCostEvent:LuaEventBase
--- @field APCost int32|nil
--- @field Attacker CDivinityStatsCharacter


--- @class LuaGetHitChanceEvent:LuaEventBase
--- @field Attacker CDivinityStatsCharacter
--- @field HitChance int32|nil
--- @field Target CDivinityStatsCharacter


--- @class LuaGetSkillAPCostEvent:LuaEventBase
--- @field AP int32|nil
--- @field AiGrid EocAiGrid
--- @field Character CDivinityStatsCharacter
--- @field ElementalAffinity boolean|nil
--- @field Position vec3
--- @field Radius number
--- @field Skill StatsSkillPrototype


--- @class LuaGetSkillDamageEvent:LuaEventBase
--- @field Attacker StatsObjectInstance
--- @field AttackerPosition vec3
--- @field DamageList StatsDamagePairList
--- @field DeathType StatsDeathType|nil
--- @field IsFromItem boolean
--- @field Level int32
--- @field NoRandomization boolean
--- @field Skill StatsSkillPrototype
--- @field Stealthed boolean
--- @field TargetPosition vec3


--- @class LuaNetMessageEvent:LuaEventBase
--- @field Channel string
--- @field Payload string
--- @field UserID UserId


--- @class LuaTickEvent:LuaEventBase
--- @field Time GameTime


--- @class RfCamera
--- @field CullingPlanes vec4[]
--- @field NVertices vec3[]
--- @field PVertices vec3[]
--- @field ProjectionMatrix mat4
--- @field ProjectionMatrixInverse mat4
--- @field ViewMatrix mat4
--- @field ViewMatrixInverse mat4
--- @field ViewProjection mat4


--- @class RfCameraController
--- @field AspectRatio number
--- @field Camera RfCamera
--- @field FOV number
--- @field FarPlane number
--- @field FrustumPoints vec3[]
--- @field ID FixedString
--- @field NearPlane number
--- @field OrthoBottom number
--- @field OrthoLeft number
--- @field OrthoRight number
--- @field OrthoTop number
--- @field Position vec3
--- @field ProjectionDirty boolean
--- @field Transform Transform
--- @field ViewDirty boolean
--- @field field_98 number


--- @class RfCameraManager
--- @field Controllers table<FixedString, RfCameraController>


--- @class StatsCharacterDynamicStat
--- @field APCostBoost int32
--- @field APMaximum int32
--- @field APRecovery int32
--- @field APStart int32
--- @field Accuracy int32
--- @field AcidImmunity boolean
--- @field AirResistance int32
--- @field AirSpecialist int32
--- @field Armor int32
--- @field ArmorBoost int32
--- @field ArmorBoostGrowthPerLevel int32
--- @field Arrow boolean
--- @field Barter int32
--- @field BleedingImmunity boolean
--- @field BlessedImmunity boolean
--- @field BlindImmunity boolean
--- @field Bodybuilding int32
--- @field BonusWeapon FixedString
--- @field BonusWeaponDamageMultiplier int32
--- @field Brewmaster int32
--- @field BurnContact boolean
--- @field BurnImmunity boolean
--- @field ChanceToHitBoost int32
--- @field Charm int32
--- @field CharmImmunity boolean
--- @field ChickenImmunity boolean
--- @field ChillContact boolean
--- @field ChilledImmunity boolean
--- @field ClairvoyantImmunity boolean
--- @field Constitution int32
--- @field CorrosiveResistance int32
--- @field Crafting int32
--- @field CrippledImmunity boolean
--- @field CriticalChance int32
--- @field CursedImmunity boolean
--- @field CustomResistance int32
--- @field DamageBoost int32
--- @field DamageBoostGrowthPerLevel int32
--- @field DecayingImmunity boolean
--- @field DeflectProjectiles boolean
--- @field DisarmedImmunity boolean
--- @field DiseasedImmunity boolean
--- @field Dodge int32
--- @field DrunkImmunity boolean
--- @field DualWielding int32
--- @field EarthResistance int32
--- @field EarthSpecialist int32
--- @field EnragedImmunity boolean
--- @field EntangledContact boolean
--- @field FOV int32
--- @field FearImmunity boolean
--- @field Finesse int32
--- @field FireResistance int32
--- @field FireSpecialist int32
--- @field Floating boolean
--- @field FreezeContact boolean
--- @field FreezeImmunity boolean
--- @field Gain int32
--- @field Grounded boolean
--- @field HastedImmunity boolean
--- @field Hearing int32
--- @field IgnoreClouds boolean
--- @field IgnoreCursedOil boolean
--- @field InfectiousDiseasedImmunity boolean
--- @field Initiative int32
--- @field Intelligence int32
--- @field Intimidate int32
--- @field InvisibilityImmunity boolean
--- @field KnockdownImmunity boolean
--- @field Leadership int32
--- @field Level int32
--- @field LifeSteal int32
--- @field LootableWhenEquipped boolean
--- @field Loremaster int32
--- @field LoseDurabilityOnCharacterHit boolean
--- @field Luck int32
--- @field MadnessImmunity boolean
--- @field MagicArmor int32
--- @field MagicArmorBoost int32
--- @field MagicArmorBoostGrowthPerLevel int32
--- @field MagicArmorMastery int32
--- @field MagicPoints int32
--- @field MagicResistance int32
--- @field MagicalSulfur boolean
--- @field MaxResistance int32
--- @field MaxSummons int32
--- @field Memory int32
--- @field Movement int32
--- @field MovementSpeedBoost int32
--- @field MuteImmunity boolean
--- @field Necromancy int32
--- @field PainReflection int32
--- @field Perseverance int32
--- @field Persuasion int32
--- @field PetrifiedImmunity boolean
--- @field PhysicalArmorMastery int32
--- @field PhysicalResistance int32
--- @field Pickpocket int32
--- @field PickpocketableWhenEquipped boolean
--- @field PiercingResistance int32
--- @field PoisonContact boolean
--- @field PoisonImmunity boolean
--- @field PoisonResistance int32
--- @field Polymorph int32
--- @field ProtectFromSummon boolean
--- @field RangeBoost int32
--- @field Ranged int32
--- @field RangerLore int32
--- @field Reason int32
--- @field Reflexes int32
--- @field RegeneratingImmunity boolean
--- @field Repair int32
--- @field RogueLore int32
--- @field Runecrafting int32
--- @field SPCostBoost int32
--- @field Sentinel int32
--- @field ShacklesOfPainImmunity boolean
--- @field ShadowResistance int32
--- @field Shield int32
--- @field ShockedImmunity boolean
--- @field Sight int32
--- @field SingleHanded int32
--- @field SleepingImmunity boolean
--- @field SlippingImmunity boolean
--- @field SlowedImmunity boolean
--- @field Sneaking int32
--- @field Sourcery int32
--- @field StepsType uint32
--- @field Strength int32
--- @field StunContact boolean
--- @field StunImmunity boolean
--- @field SuffocatingImmunity boolean
--- @field Sulfurology int32
--- @field SummonLifelinkModifier int32
--- @field Summoning int32
--- @field TALENT_ActionPoints boolean
--- @field TALENT_ActionPoints2 boolean
--- @field TALENT_AirSpells boolean
--- @field TALENT_Ambidextrous boolean
--- @field TALENT_AnimalEmpathy boolean
--- @field TALENT_AttackOfOpportunity boolean
--- @field TALENT_AvoidDetection boolean
--- @field TALENT_Awareness boolean
--- @field TALENT_Backstab boolean
--- @field TALENT_BeastMaster boolean
--- @field TALENT_Bully boolean
--- @field TALENT_Carry boolean
--- @field TALENT_ChanceToHitMelee boolean
--- @field TALENT_ChanceToHitRanged boolean
--- @field TALENT_Charm boolean
--- @field TALENT_Courageous boolean
--- @field TALENT_Criticals boolean
--- @field TALENT_Damage boolean
--- @field TALENT_DeathfogResistant boolean
--- @field TALENT_Demon boolean
--- @field TALENT_DualWieldingDodging boolean
--- @field TALENT_Durability boolean
--- @field TALENT_Dwarf_Sneaking boolean
--- @field TALENT_Dwarf_Sturdy boolean
--- @field TALENT_EarthSpells boolean
--- @field TALENT_ElementalAffinity boolean
--- @field TALENT_ElementalRanger boolean
--- @field TALENT_Elementalist boolean
--- @field TALENT_Elf_CorpseEating boolean
--- @field TALENT_Elf_Lore boolean
--- @field TALENT_Escapist boolean
--- @field TALENT_Executioner boolean
--- @field TALENT_ExpGain boolean
--- @field TALENT_ExtraSkillPoints boolean
--- @field TALENT_ExtraStatPoints boolean
--- @field TALENT_FaroutDude boolean
--- @field TALENT_FireSpells boolean
--- @field TALENT_FiveStarRestaurant boolean
--- @field TALENT_Flanking boolean
--- @field TALENT_FolkDancer boolean
--- @field TALENT_Gladiator boolean
--- @field TALENT_GoldenMage boolean
--- @field TALENT_GreedyVessel boolean
--- @field TALENT_Haymaker boolean
--- @field TALENT_Human_Civil boolean
--- @field TALENT_Human_Inventive boolean
--- @field TALENT_IceKing boolean
--- @field TALENT_IncreasedArmor boolean
--- @field TALENT_Indomitable boolean
--- @field TALENT_Initiative boolean
--- @field TALENT_Intimidate boolean
--- @field TALENT_InventoryAccess boolean
--- @field TALENT_ItemCreation boolean
--- @field TALENT_ItemMovement boolean
--- @field TALENT_Jitterbug boolean
--- @field TALENT_Kickstarter boolean
--- @field TALENT_Leech boolean
--- @field TALENT_LightStep boolean
--- @field TALENT_LightningRod boolean
--- @field TALENT_LivingArmor boolean
--- @field TALENT_Lizard_Persuasion boolean
--- @field TALENT_Lizard_Resistance boolean
--- @field TALENT_Lockpick boolean
--- @field TALENT_LoneWolf boolean
--- @field TALENT_Luck boolean
--- @field TALENT_MagicCycles boolean
--- @field TALENT_MasterThief boolean
--- @field TALENT_Max boolean
--- @field TALENT_Memory boolean
--- @field TALENT_MrKnowItAll boolean
--- @field TALENT_NaturalConductor boolean
--- @field TALENT_NoAttackOfOpportunity boolean
--- @field TALENT_None boolean
--- @field TALENT_PainDrinker boolean
--- @field TALENT_Perfectionist boolean
--- @field TALENT_Politician boolean
--- @field TALENT_Quest_GhostTree boolean
--- @field TALENT_Quest_Rooted boolean
--- @field TALENT_Quest_SpidersKiss_Int boolean
--- @field TALENT_Quest_SpidersKiss_Null boolean
--- @field TALENT_Quest_SpidersKiss_Per boolean
--- @field TALENT_Quest_SpidersKiss_Str boolean
--- @field TALENT_Quest_TradeSecrets boolean
--- @field TALENT_QuickStep boolean
--- @field TALENT_Rager boolean
--- @field TALENT_Raistlin boolean
--- @field TALENT_RangerLoreArrowRecover boolean
--- @field TALENT_RangerLoreEvasionBonus boolean
--- @field TALENT_RangerLoreRangedAPBonus boolean
--- @field TALENT_Reason boolean
--- @field TALENT_Repair boolean
--- @field TALENT_ResistDead boolean
--- @field TALENT_ResistFear boolean
--- @field TALENT_ResistKnockdown boolean
--- @field TALENT_ResistPoison boolean
--- @field TALENT_ResistSilence boolean
--- @field TALENT_ResistStun boolean
--- @field TALENT_ResurrectExtraHealth boolean
--- @field TALENT_ResurrectToFullHealth boolean
--- @field TALENT_RogueLoreDaggerAPBonus boolean
--- @field TALENT_RogueLoreDaggerBackStab boolean
--- @field TALENT_RogueLoreGrenadePrecision boolean
--- @field TALENT_RogueLoreHoldResistance boolean
--- @field TALENT_RogueLoreMovementBonus boolean
--- @field TALENT_Sadist boolean
--- @field TALENT_Scientist boolean
--- @field TALENT_Sight boolean
--- @field TALENT_Soulcatcher boolean
--- @field TALENT_Sourcerer boolean
--- @field TALENT_SpillNoBlood boolean
--- @field TALENT_StandYourGround boolean
--- @field TALENT_Stench boolean
--- @field TALENT_SurpriseAttack boolean
--- @field TALENT_Throwing boolean
--- @field TALENT_Torturer boolean
--- @field TALENT_Trade boolean
--- @field TALENT_Unstable boolean
--- @field TALENT_ViolentMagic boolean
--- @field TALENT_Vitality boolean
--- @field TALENT_WalkItOff boolean
--- @field TALENT_WandCharge boolean
--- @field TALENT_WarriorLoreGrenadeRange boolean
--- @field TALENT_WarriorLoreNaturalArmor boolean
--- @field TALENT_WarriorLoreNaturalHealth boolean
--- @field TALENT_WarriorLoreNaturalResistance boolean
--- @field TALENT_WaterSpells boolean
--- @field TALENT_WeatherProof boolean
--- @field TALENT_WhatARush boolean
--- @field TALENT_WildMag boolean
--- @field TALENT_Zombie boolean
--- @field TauntedImmunity boolean
--- @field Telekinesis int32
--- @field Thievery int32
--- @field ThrownImmunity boolean
--- @field Torch boolean
--- @field TranslationKey FixedString
--- @field TwoHanded int32
--- @field Unbreakable boolean
--- @field Unrepairable boolean
--- @field Unstorable boolean
--- @field Vitality int32
--- @field VitalityBoost int32
--- @field VitalityMastery int32
--- @field Wand int32
--- @field WarmImmunity boolean
--- @field WarriorLore int32
--- @field WaterResistance int32
--- @field WaterSpecialist int32
--- @field WeakImmunity boolean
--- @field WebImmunity boolean
--- @field Weight int32
--- @field WetImmunity boolean
--- @field Willpower int32
--- @field Wits int32


--- @class StatsDamagePairList
--- @field Add fun(self:StatsDamagePairList, a1:StatsDamageType, a2:int32)
--- @field AggregateSameTypeDamages fun(self:StatsDamagePairList)
--- @field Clear fun(self:StatsDamagePairList, a1:StatsDamageType|nil)
--- @field ConvertDamageType fun(self:StatsDamagePairList, newType:StatsDamageType)
--- @field CopyFrom fun(self:StatsDamagePairList, other:StatsDamagePairList)
--- @field GetByType fun(self:StatsDamagePairList, damageType:StatsDamageType):int32
--- @field Merge fun(self:StatsDamagePairList, other:StatsDamagePairList)
--- @field Multiply fun(self:StatsDamagePairList, multiplier:number)
--- @field ToTable fun(self:StatsDamagePairList):{Amount:integer, DamageType:StatsDamageType}[]


--- @class StatsHitDamageInfo
--- @field ArmorAbsorption int32
--- @field AttackDirection uint32
--- @field Backstab boolean
--- @field Bleeding boolean
--- @field Blocked boolean
--- @field Burning boolean
--- @field CounterAttack boolean
--- @field CriticalHit boolean
--- @field DamageDealt int32
--- @field DamageList StatsDamagePairList
--- @field DamageType StatsDamageType
--- @field DamagedMagicArmor boolean
--- @field DamagedPhysicalArmor boolean
--- @field DamagedVitality boolean
--- @field DeathType StatsDeathType
--- @field DoT boolean
--- @field Dodged boolean
--- @field DontCreateBloodSurface boolean
--- @field Equipment uint32
--- @field Flanking boolean
--- @field FromSetHP boolean
--- @field FromShacklesOfPain boolean
--- @field Hit boolean
--- @field HitWithWeapon boolean
--- @field LifeSteal int32
--- @field Missed boolean
--- @field NoDamageOnOwner boolean
--- @field NoEvents boolean
--- @field Poisoned boolean
--- @field ProcWindWalker boolean
--- @field PropagatedFromOwner boolean
--- @field Reflection boolean
--- @field Surface boolean
--- @field TotalDamageDone int32


--- @class StatsItemColorDefinition
--- @field Color1 uint32
--- @field Color2 uint32
--- @field Color3 uint32


--- @class StatsObject
--- @field AIFlags FixedString
--- @field ComboCategories FixedString[]
--- @field DisplayName TranslatedString
--- @field FS2 FixedString
--- @field Handle int32
--- @field Level int32
--- @field MemorizationRequirements StatsRequirement[]
--- @field ModId FixedString
--- @field ModifierListIndex int32
--- @field Name FixedString
--- @field PropertyLists table<FixedString, StatsPropertyList>
--- @field Requirements StatsRequirement[]
--- @field StatsEntry StatsObject|StatEntryType
--- @field StringProperties1 FixedString[]


--- @class StatsObjectInstance:StatsObject
--- @field InstanceId uint32


---@alias AnyStatProperty StatPropertyCustom|StatPropertyStatus|StatPropertySurfaceChange|StatPropertySabotage|StatPropertySummon|StatPropertyForce|StatPropertyGameAction|StatPropertyOsirisTask|StatPropertyExtender

--- @class StatProperty
--- @field Context string[] Target|Self|AoE|SelfOnHit|SelfOnEquip
--- @field Type string Custom|Status|SurfaceChange|GameAction|OsirisTask|Sabotage|Summon|Force
--- @field Condition string|nil

--- @class StatPropertyCustom : StatProperty
--- @field Type string|"Custom"
--- @field Action string

--- @class StatPropertyStatus : StatProperty
--- @field Type string|"Status"
--- @field Action string Status name
--- @field StatusChance number
--- @field Duration number
--- @field StatsId string
--- @field Arg4 integer
--- @field Arg5 integer
--- @field SurfaceBoost boolean
--- @field SurfaceBoosts string[] Labels from "Surface Type" enumeration

--- @class StatPropertySurfaceChange : StatProperty
--- @field Type string|"SurfaceChange"
--- @field Action string Label from "Surface Change" enumeration
--- @field SurfaceChance number
--- @field Lifetime number
--- @field StatusChance number
--- @field Radius number

--- @class StatPropertySabotage : StatProperty
--- @field Type string|"Sabotage"
--- @field Amount integer

--- @class StatPropertySummon : StatProperty
--- @field Type string|"Summon"
--- @field Template string
--- @field Duration number
--- @field IsTotem boolean
--- @field Skill string

--- @class StatPropertyForce : StatProperty
--- @field Type string|"Force"
--- @field Distance integer The force distance in meters.

--- @class StatPropertyGameAction : StatProperty
--- @field Type string|"GameAction"
--- @field Action string Label from "Game Action" enumeration
--- @field Arg1 number
--- @field Arg2 number
--- @field Arg3 string
--- @field Arg4 number
--- @field Arg5 number
--- @field StatusHealType string Label from"StatusHealType"enumeration

--- @class StatPropertyOsirisTask : StatProperty
--- @field Type string|"OsirisTask"
--- @field Action string Label from "Osiris Task" enumeration
--- @field Chance number
--- @field VitalityOnRevive integer

--- @class StatPropertyExtender : StatProperty
--- @field Type string|"Extender"
--- @field Action string Custom SkillProperties action name
--- @field Arg1 number
--- @field Arg2 number
--- @field Arg3 string
--- @field Arg4 integer
--- @field Arg5 integer


--- @class StatsPropertyData
--- @field Context StatsPropertyContext
--- @field Name FixedString
--- @field TypeId StatsPropertyType


--- @class StatsPropertyExtender:StatsPropertyData
--- @field Action FixedString
--- @field Arg1 number
--- @field Arg2 number
--- @field Arg3 FixedString
--- @field Arg4 int32
--- @field Arg5 int32
--- @field PropertyName FixedString


--- @class StatsPropertyList
--- @field AllPropertyContexts StatsPropertyContext
--- @field Name FixedString


--- @class StatsPropertyStatus:StatsPropertyData
--- @field Arg4 int32
--- @field Arg5 int32
--- @field Duration number
--- @field StatsId FixedString
--- @field Status FixedString
--- @field StatusChance number
--- @field SurfaceBoost boolean
--- @field SurfaceBoosts SurfaceType[]


--- @class StatsReflection
--- @field DamageType StatsDamageType
--- @field MeleeOnly boolean


--- @class StatsReflectionSet


--- @class StatsRequirement
--- @field Not boolean
--- @field Param int32
--- @field Requirement StatsRequirementType
--- @field Tag FixedString


--- @class StatsSkillPrototype
--- @field Ability int16
--- @field ActionPoints int32
--- @field AiFlags AIFlags
--- @field ChargeDuration number
--- @field ChildPrototypes StatsSkillPrototype[]
--- @field Cooldown number
--- @field CooldownReduction number
--- @field DisplayName string
--- @field Icon FixedString
--- @field Level int32
--- @field MagicCost int32
--- @field MemoryCost int32
--- @field Requirement int32
--- @field RootSkillPrototype StatsSkillPrototype
--- @field SkillId FixedString
--- @field SkillTypeId SkillType
--- @field StatsObject StatsObject|StatEntryType
--- @field Tier int32


--- @class StatsStatusPrototype
--- @field AbsorbSurfaceTypes SurfaceType[]
--- @field DisplayName TranslatedString
--- @field HasStats boolean
--- @field Icon FixedString
--- @field StatsObject StatsObject
--- @field StatusId StatusType
--- @field StatusName FixedString


--- @class StatsTreasureSubTableDescription
--- @field Amounts int32[]
--- @field Categories StatsTreasureSubTableDescriptionCategory[]
--- @field CategoryFrequencies int32[]
--- @field DropCounts StatsTreasureSubTableDescriptionDropCount[]
--- @field EndLevel int32
--- @field StartLevel int32
--- @field TotalCount int32
--- @field TotalFrequency int32


--- @class StatsTreasureSubTableDescriptionCategory
--- @field CurrentFrequencies uint16[]
--- @field Frequencies uint16[]
--- @field Frequency int32
--- @field Index int32
--- @field IsTreasureTable boolean
--- @field IsTreasureTable2 boolean


--- @class StatsTreasureSubTableDescriptionDropCount
--- @field Amount int32
--- @field Chance int32


--- @class StatsTreasureTable
--- @field CanMerge boolean
--- @field IgnoreLevelDiff boolean
--- @field MaxLevel int32
--- @field MinLevel int32
--- @field Name FixedString
--- @field SubTables StatsTreasureSubTableDescription[]
--- @field UseTreasureGroupContainers boolean


--#region Flash Types

--- Represents an object in Flash
--- Implements the __index and __newindex metamethods using string keys (i.e. allows table-like behavior):
--- obj.field = 123 -- Can assign values to any object property
--- Ext.Print(obj.field) -- Can read any object property
--- Field values are returned using the appropriate Lua type;
--- Flash Boolean/Number/string = Lua boolean/number/string
--- Flash Object = Lua engine class FlashObject
--- Flash array = Lua engine class FlashArray
--- Flash function = Lua engine class FlashFunction
--- @class FlashObject
--- @field toString fun():string Returns the string representation of the specified object.
--- @field toLocaleString fun():string Returns the string representation of this object, formatted according to locale-specific conventions.
--- @field hasOwnProperty fun(name:string):boolean Indicates whether an object has a specified property defined.

--- Represents an array in Flash that begins at index 0
--- Implements the __index, __newindex and __len metamethods using integer keys (i.e. allows table-like behavior):
--- arr[2] = 123 -- Can assign values to any array index
--- Ext.Print(arr[2]) -- Can read any array index
--- Ext.Print(#arr) -- Can query length of array
--- @class FlashArray<T>: { [integer]: T }

--- Represents a function or method in Flash
--- Implements the __call metamethod (i.e. can be called)
--- The passed arguments are forwarded to the Flash method and the return value of the Flash method is returned
--- @class FlashFunction

--- @class FlashEvent:FlashObject

--- @class FlashEventDispatcher:FlashObject
--- @field dispatchEvent fun(event:FlashEvent):boolean Dispatches an event into the event flow.
--- @field hasEventListener fun(type:string):boolean Checks whether the EventDispatcher object has any listeners registered for a specific type of event.
--- @field willTrigger fun(type:string):boolean Checks whether an event listener is registered with this EventDispatcher object or any of its ancestors for the specified event type.

--- Currently unsupported type 12
--- @class FlashDisplayObject:FlashEventDispatcher
--- @field alpha number Indicates the alpha transparency value of the object specified.
--- @field blendMode string A value from the BlendMode class that specifies which blend mode to use.
--- @field blendShader Shader [write-only] Sets a shader that is used for blending the foreground and background.
--- @field cacheAsBitmap boolean If set to true, Flash runtimes cache an internal bitmap representation of the display object.
--- @field cacheAsBitmapMatrix Matrix If non-null, this Matrix object defines how a display object is rendered when cacheAsBitmap is set to true.
--- @field filters Array An indexed array that contains each filter object currently associated with the display object.
--- @field height number Indicates the height of the display object, in pixels.
--- @field loaderInfo LoaderInfo [read-only] Returns a LoaderInfo object containing information about loading the file to which this display object belongs.
--- @field mask FlashDisplayObject The calling display object is masked by the specified mask object.
--- @field metaData FlashObject Obtains the meta data object of the DisplayObject instance if meta data was stored alongside the the instance of this DisplayObject in the SWF file through a PlaceObject4 tag.
--- @field mouseX number [read-only] Indicates the x coordinate of the mouse or user input device position, in pixels.
--- @field mouseY number [read-only] Indicates the y coordinate of the mouse or user input device position, in pixels.
--- @field name string Indicates the instance name of the DisplayObject.
--- @field opaqueBackground FlashObject Specifies whether the display object is opaque with a certain background color.
--- @field parent FlashDisplayObjectContainer [read-only] Indicates the DisplayObjectContainer object that contains this display object.
--- @field root FlashDisplayObject [read-only] For a display object in a loaded SWF file, the root property is the top-most display object in the portion of the display list's tree structure represented by that SWF file.
--- @field rotation number Indicates the rotation of the DisplayObject instance, in degrees, from its original orientation.
--- @field rotationX number Indicates the x-axis rotation of the DisplayObject instance, in degrees, from its original orientation relative to the 3D parent container.
--- @field rotationY number Indicates the y-axis rotation of the DisplayObject instance, in degrees, from its original orientation relative to the 3D parent container.
--- @field rotationZ number Indicates the z-axis rotation of the DisplayObject instance, in degrees, from its original orientation relative to the 3D parent container.
--- @field scale9Grid FlashRectangle The current scaling grid that is in effect.
--- @field scaleX number Indicates the horizontal scale (percentage) of the object as applied from the registration point.
--- @field scaleY number Indicates the vertical scale (percentage) of an object as applied from the registration point of the object.
--- @field scaleZ number Indicates the depth scale (percentage) of an object as applied from the registration point of the object.
--- @field scrollRect FlashRectangle The scroll rectangle bounds of the display object.
--- @field stage Stage [read-only] The Stage of the display object.
--- @field transform flash.geom:Transform An object with properties pertaining to a display object's matrix, color transform, and pixel bounds.
--- @field visible boolean Whether or not the display object is visible.
--- @field width number Indicates the width of the display object, in pixels.
--- @field x number Indicates the x coordinate of the DisplayObject instance relative to the local coordinates of the parent DisplayObjectContainer.
--- @field y number Indicates the y coordinate of the DisplayObject instance relative to the local coordinates of the parent DisplayObjectContainer.
--- @field z number Indicates the z coordinate position along the z-axis of the DisplayObject instance relative to the 3D parent container.
--- @field hitTestPoint fun(x:number, y:number, shapeFlag:boolean|nil):boolean Evaluates the display object to see if it overlaps or intersects with the point specified by the x and y parameters.

--- @class FlashInteractiveObject:FlashDisplayObject
--- @field doubleClickEnabled boolean Specifies whether the object receives doubleClick events
--- @field mouseEnabled boolean Specifies whether this object receives mouse, or other user input, messages
--- @field tabEnabled boolean Specifies whether this object is in the tab order
--- @field tabIndex integer Specifies the tab ordering of objects in a SWF file

--- @alias FlashTextFormatAlign "center"|"end"|"justify"|"left"|"right"|"start"

--- flash.text:TextFormat
--- @class FlashTextFormat:FlashObject
--- @field align FlashTextFormatAlign Indicates the alignment of the paragraph.
--- @field blockIndent boolean|nil Indicates the block indentation in pixels.
--- @field bold boolean|nil Specifies whether the text is boldface.
--- @field bullet boolean|nil Indicates that the text is part of a bulleted list.
--- @field color uint32|nil Indicates the color of the text.
--- @field font string The name of the font for text in this text format, as a string.
--- @field indent number|nil Indicates the indentation from the left margin to the first character in the paragraph.
--- @field italic boolean|nil Indicates whether text in this text format is italicized.
--- @field kerning number|nil A Boolean value that indicates whether kerning is enabled (true) or disabled (false).
--- @field leading number|nil An integer representing the amount of vertical space (called leading) between lines.
--- @field leftMargin number|nil The left margin of the paragraph, in pixels.
--- @field letterSpacing number|nil A number representing the amount of space that is uniformly distributed between all characters.
--- @field rightMargin number|nil The right margin of the paragraph, in pixels.
--- @field size number|nil The size in pixels of text in this text format.
--- @field tabStops FlashArray Specifies custom tab stops as an array of non-negative integers.
--- @field target string Indicates the target window where the hyperlink is displayed.
--- @field underline boolean|nil Indicates whether the text that uses this text format is underlined (true) or not (false).
--- @field url string Indicates the target URL for the text in this text format.

--- @class FlashTextField:FlashInteractiveObject
--- @field alwaysShowSelection boolean When set to true and the text field is not in focus, Flash Player highlights the selection in the text field in gray.
--- @field antiAliasType string The type of anti-aliasing used for this text field.
--- @field autoSize string Controls automatic sizing and alignment of text fields.
--- @field background boolean Specifies whether the text field has a background fill.
--- @field backgroundColor uint32 The color of the text field background.
--- @field border boolean Specifies whether the text field has a border.
--- @field borderColor uint32 The color of the text field border.
--- @field bottomScrollV integer [read-only] An integer (1-based index) that indicates the bottommost line that is currently visible in the specified text field.
--- @field caretIndex integer [read-only] The index of the insertion point (caret) position.
--- @field condenseWhite boolean A Boolean value that specifies whether extra white space (spaces, line breaks, and so on) in a text field with HTML text is removed.
--- @field defaultTextFormat FlashTextFormat Specifies the format applied to newly inserted text, such as text entered by a user or text inserted with the replaceSelectedText() method.
--- @field displayAsPassword boolean Specifies whether the text field is a password text field.
--- @field embedFonts boolean Specifies whether to render by using embedded font outlines.
--- @field gridFitType string The type of grid fitting used for this text field.
--- @field htmlText string Contains the HTML representation of the text field contents.
--- @field length integer [read-only] The number of characters in a text field.
--- @field maxChars integer The maximum number of characters that the text field can contain, as entered by a user.
--- @field maxScrollH integer [read-only] The maximum value of scrollH.
--- @field maxScrollV integer [read-only] The maximum value of scrollV.
--- @field mouseWheelEnabled boolean A Boolean value that indicates whether Flash Player automatically scrolls multiline text fields when the user clicks a text field and rolls the mouse wheel.
--- @field multiline boolean Indicates whether field is a multiline text field.
--- @field numLines integer [read-only] Defines the number of text lines in a multiline text field.
--- @field restrict string Indicates the set of characters that a user can enter into the text field.
--- @field scrollH integer The current horizontal scrolling position.
--- @field scrollV integer The vertical position of text in a text field.
--- @field selectable boolean A Boolean value that indicates whether the text field is selectable.
--- @field selectionBeginIndex integer [read-only] The zero-based character index value of the first character in the current selection.
--- @field selectionEndIndex integer [read-only] The zero-based character index value of the last character in the current selection.
--- @field sharpness number The sharpness of the glyph edges in this text field.
--- @field styleSheet StyleSheet Attaches a style sheet to the text field.
--- @field text string A string that is the current text in the text field.
--- @field textColor uint32 The color of the text in a text field, in hexadecimal format.
--- @field textHeight number [read-only] The height of the text in pixels.
--- @field textInteractionMode string [read-only] The interaction mode property, Default value is TextInteractionMode.NORMAL.
--- @field textWidth number [read-only] The width of the text in pixels.
--- @field thickness number The thickness of the glyph edges in this text field.
--- @field type string The type of the text field.
--- @field useRichTextClipboard boolean Specifies whether to copy and paste the text formatting along with the text.
--- @field wordWrap boolean A Boolean value that indicates whether the text field has word wrap.
--- @field appendText fun(newText:string) Appends the string specified by the newText parameter to the end of the text of the text field.
--- @field getCharBoundaries fun(charIndex:integer):FlashRectangle Returns a rectangle that is the bounding box of the character.
--- @field getCharIndexAtPoint fun(x:Number, y:Number):integer Returns the zero-based index value of the character at the point specified by the x and y parameters.
--- @field getFirstCharInParagraph fun(charIndex:integer):integer Given a character index, returns the index of the first character in the same paragraph.
--- @field getImageReference fun(id:string):FlashDisplayObject Returns a DisplayObject reference for the given id, for an image or SWF file that has been added to an HTML-formatted text field by using an <img> tag.
--- @field getLineIndexAtPoint fun(x:Number, y:Number):integer Returns the zero-based index value of the line at the point specified by the x and y parameters.
--- @field getLineIndexOfChar fun(charIndex:integer):integer Returns the zero-based index value of the line containing the character specified by the charIndex parameter.
--- @field getLineLength fun(lineIndex:integer):integer Returns the number of characters in a specific text line.
--- @field getLineMetrics fun(lineIndex:integer):flash.text:TextLineMetrics Returns metrics information about a given text line.
--- @field getLineOffset fun(lineIndex:integer):integer Returns the character index of the first character in the line that the lineIndex parameter specifies.
--- @field getLineText fun(lineIndex:integer):string Returns the text of the line specified by the lineIndex parameter.
--- @field getParagraphLength fun(charIndex:integer):integer Given a character index, returns the length of the paragraph containing the given character.	
--- @field getTextFormat fun(beginIndex:integer, endIndex:integer):flash.text:TextFormat Returns a TextFormat object that contains formatting information for the range of text that the beginIndex and endIndex parameters specify.
--- @field isFontCompatible fun(fontName:string, fontStyle:string):boolean [static] Returns true if an embedded font is available with the specified fontName and fontStyle where Font.fontType is flash.text.FontType.EMBEDDED.
--- @field replaceSelectedText fun(value:string) Replaces the current selection with the contents of the value parameter.	
--- @field replaceText fun(beginIndex:integer, endIndex:integer, newText:string) Replaces the range of characters that the beginIndex and endIndex parameters specify with the contents of the newText parameter.	
--- @field setSelection fun(beginIndex:integer, endIndex:integer) Sets as selected the text designated by the index values of the first and last characters, which are specified with the beginIndex and endIndex parameters.
--- @field setTextFormat fun(FlashTextFormat, beginIndex:integer, endIndex:integer) Applies the text formatting that the format parameter specifies to the specified text in a text field.

--- @class FlashBitmapData
--- @class FlashMatrix
--- @class FlashVector
--- @class FlashTextSnapshot
--- @class FlashPoint
--- @class FlashEvent
--- @class FlashMouseEvent

--- @class FlashGraphics:FlashObject
--- @field beginBitmapFill fun(bitmap:FlashBitmapData, matrix:FlashMatrix, repeat:boolean, smooth:boolean) Fills a drawing area with a bitmap image
--- @field beginFill fun(color:integer, alpha:number) Specifies a simple one-color fill that subsequent calls to other Graphics methods (such as lineTo or drawCircle) use when drawing
--- @field beginGradientFill fun(type:string, colors:FlashArray, alphas:FlashArray, ratios:FlashArray, matrix:FlashMatrix, spreadMethod:string, interpolationMethod:string, focalPointRatio:number) Specifies a gradient fill used by subsequent calls to other Graphics methods (such as lineTo or drawCircle) for the object
--- @field clear function Clears the graphics that were drawn to this Graphics object, and resets fill and line style settings
--- @field curveTo fun(controlX:number, controlY:number, anchorX:number, anchorY:number) Draws a quadratic Bezier curve using the current line style from the current drawing position to the control point specified
--- @field drawCircle fun(x:number, y:number, radius:number) Draws a circle
--- @field drawEllipse fun(x:number, y:number, width:number, height:number) Draws an ellipse
--- @field drawPath fun(commands:FlashVector, data:FlashVector, winding:string) Submits a series of commands for drawing
--- @field drawRect fun(x:number, y:number, width:number, height:number) Draws a rectangle
--- @field drawRoundRect fun(x:number, y:number, width:number, height:number, ellipseWidth:number, ellipseHeight:number) Draws a rounded rectangle
--- @field endFill function Applies a fill to the lines and curves that were added since the last call to beginFill, beginGradientFill, or beginBitmapFill methods
--- @field lineGradientStyle fun(type:string, colors:FlashArray, alphas:FlashArray, ratios:FlashArray, matrix:FlashMatrix, spreadMethod:string, interpolationMethod:string, focalPointRatio:number) Specifies a gradient to use for the stroke when drawing lines
--- @field lineStyle fun(thickness:number, color:integer, alpha:number, pixelHinting:boolean, scaleMode:string, caps:string, joints:string, miterLimit:number) Specifies a line style used for subsequent calls to Graphics methods such as the lineTo method or the drawCircle method
--- @field lineTo fun(x:number, y:number) Draws a line using the current line style from the current drawing position
--- @field moveTo fun(x:number, y:number) Moves the current drawing position,

--- @class FlashDisplayObjectContainer:FlashInteractiveObject
--- @field mouseChildren boolean Determines whether or not the children of the object are mouse, or user input device, enabled
--- @field numChildren integer Returns the number of children of this object. [read-only]
--- @field tabChildren boolean Determines whether the children of the object are tab enabled
--- @field textSnapshot FlashTextSnapshot Returns a TextSnapshot object for this DisplayObjectContainer instance. [read-only]
--- @field addChild fun(child:FlashDisplayObject):FlashDisplayObject Adds a child DisplayObject instance to this DisplayObjectContainer instance
--- @field addChildAt fun(child:FlashDisplayObject, index:integer):FlashDisplayObject Adds a child DisplayObject instance to this DisplayObjectContainer instance
--- @field areInaccessibleObjectsUnderPoint fun(point:FlashPoint):boolean Indicates whether the security restrictions would cause any display objects to be omitted from the list returned by calling the DisplayObjectContainer.getObjectsUnderPoint() method with the specified point point
--- @field contains fun(child:FlashDisplayObject):boolean Determines whether the specified display object is a child of the DisplayObjectContainer instance or the instance itself
--- @field getChildAt fun(index:integer):FlashDisplayObject Returns the child display object instance that exists at the specified index
--- @field getChildByName fun(name:string):FlashDisplayObject Returns the child display object that exists with the specified name
--- @field getChildIndex fun(child:FlashDisplayObject):integer Returns the index position of a child DisplayObject instance
--- @field getObjectsUnderPoint fun(point:FlashPoint):table Returns an array of objects that lie under the specified point and are children (or grandchildren, and so on) of this DisplayObjectContainer instance
--- @field removeChild fun(child:FlashDisplayObject):FlashDisplayObject Removes the specified child DisplayObject instance from the child list of the DisplayObjectContainer instance
--- @field removeChildAt fun(index:integer):FlashDisplayObject Removes a child DisplayObject from the specified index position in the child list of the DisplayObjectContainer
--- @field removeChildren fun(beginIndex:integer, endIndex:integer) Removes all child DisplayObject instances from the child list of the DisplayObjectContainer instance
--- @field setChildIndex fun(child:FlashDisplayObject, index:integer) Changes the position of an existing child in the display object container
--- @field swapChildren fun(child1:FlashDisplayObject, child2:FlashDisplayObject) Swaps the z-order (front-to-back order) of the two specified child objects
--- @field swapChildrenAt fun(index1:integer, index2:integer) Swaps the z-order (front-to-back order) of the child objects at the two specified index positions in the child list

--- @class FlashRectangle:FlashObject
--- @field x number
--- @field y number
--- @field top number
--- @field bottom number
--- @field left number
--- @field right number

--- @class FlashSprite:FlashDisplayObjectContainer
--- @field buttonMode boolean Specifies the button mode of this sprite
--- @field graphics FlashGraphics Specifies the Graphics object that belongs to this sprite where vector drawing commands can occur. [read-only]
--- @field soundTransform number Controls sound within this sprite
--- @field useHandCursor boolean A value that indicates whether the pointing hand (hand cursor) appears when the pointer rolls over a sprite in which the buttonMode property is set to true

--- @class FlashMovieClip:FlashSprite
--- @field currentFrame integer Specifies the number of the frame in which the playhead is located in the timeline of the MovieClip instance. [read-only]
--- @field currentFrameLabel string The label at the current frame in the timeline of the MovieClip instance. [read-only]
--- @field currentLabel string The current label in which the playhead is located in the timeline of the MovieClip instance. [read-only]
--- @field currentLabels string[] Returns an array of FrameLabel objects from the current scene. [read-only]
--- @field currentScene FlashObject The current scene in which the playhead is located in the timeline of the MovieClip instance. [read-only]
--- @field scenes FlashArray[] An array of Scene objects, each listing the name, the number of frames, and the frame labels for a scene in the MovieClip instance. [read-only]
--- @field enabled boolean A Boolean value that indicates whether a movie clip is enabled
--- @field framesLoaded integer The number of frames that are loaded from a streaming SWF file. [read-only]
--- @field isPlaying boolean A Boolean value that indicates whether a movie clip is curently playing. [read-only]
--- @field totalFrames integer The total number of frames in the MovieClip instance. [read-only]
--- @field trackAsMenu boolean Indicates whether other display objects that are SimpleButton or MovieClip objects can receive mouse release events or other user input release events
--- @field gotoAndPlay fun(frame:string|integer, scene:string|nil) Starts playing the SWF file at the specified frame
--- @field gotoAndStop fun(frame:string|integer, scene:string|nil) Brings the playhead to the specified frame of the movie clip and stops it there
--- @field nextFrame function Sends the playhead to the next frame and stops it
--- @field nextScene function Moves the playhead to the next scene of the MovieClip instance
--- @field play function Moves the playhead in the timeline of the movie clip
--- @field prevFrame function Sends the playhead to the previous frame and stops it
--- @field prevScene function Moves the playhead to the previous scene of the MovieClip instance
--- @field stop function Stops the playhead in the movie clip
--- @field hitTest fun(x:number, y:number, shapeFlag:boolean|nil):boolean

--- @class FlashMainTimeline:FlashMovieClip
--- @field events string[] An array of input keys this UI should listen for, in the form of 'IE Name', such as 'IE UICreationTabPrev'. The engine will invoke onEventDown/onEventUp when these keys are pressed, if they haven't been handled
--- @field onEventDown fun(id:number):boolean Invoked by the engine when a valid input key in this.events is pressed. If true is returned, the key is"handled"and won't send events to other UI objects
--- @field onEventUp fun(id:number):boolean Invoked by the engine when a valid input key in this.events is released. If true is returned, the key is"handled"and won't send events to other UI objects
--- @field onEventResolution fun(width:number, height:number) Invoked by the engine when the screen is resized
--- @field onEventInit function Invoked by the engine. Typically used to register the anchor id and layout with ExternalInterface.call

--#endregion

--#region ExtraData

Ext.ExtraData = {
    FirstItemTypeShift = 9,
    SecondItemTypeShift = 16,
    PickpocketGoldValuePerPoint = 200,
    PickpocketWeightPerPoint = 2000,
    PickpocketExperienceLevelsPerPoint = 4,
    PersuasionAttitudeBonusPerPoint = 5,
    WandUsesMax = 10,
    AttributeBaseValue = 10,
    AttributeCharCreationBonus = 1,
    AttributeLevelGrowth = 2,
    AttributeBoostGrowth = 0.75,
    AttributeGrowthDamp = 0.7,
    AttributeSoftCap = 40,
    WitsGrowthDamp = 0.5,
    VitalityStartingAmount = 21,
    VitalityExponentialGrowth = 1.25,
    VitalityLinearGrowth = 9.091,
    VitalityToDamageRatio = 5,
    VitalityToDamageRatioGrowth = 0.2,
    ExpectedDamageBoostFromAttributePerLevel = 0.065,
    ExpectedDamageBoostFromSkillAbilityPerLevel = 0.015,
    ExpectedDamageBoostFromWeaponAbilityPerLevel = 0.025,
    ExpectedConGrowthForArmorCalculation = 1,
    FirstVitalityLeapLevel = 9,
    FirstVitalityLeapGrowth = 1.25,
    SecondVitalityLeapLevel = 13,
    SecondVitalityLeapGrowth = 1.25,
    ThirdVitalityLeapLevel = 16,
    ThirdVitalityLeapGrowth = 1.25,
    FourthVitalityLeapLevel = 18,
    FourthVitalityLeapGrowth = 1.35,
    DamageBoostFromAttribute = 0.05,
    MonsterDamageBoostPerLevel = 0.02,
    PhysicalArmourBoostFromAttribute = 0,
    MagicArmourBoostFromAttribute = 0,
    VitalityBoostFromAttribute = 0.07,
    DodgingBoostFromAttribute = 0,
    HealToDamageRatio = 1.3,
    ArmorToVitalityRatio = 0.55,
    ArmorRegenTimer = 0.01,
    ArmorRegenConstGrowth = 1,
    ArmorRegenPercentageGrowth = 10,
    ArmorAfterHitCooldown = -7,
    MagicArmorRegenTimer = 0.01,
    MagicArmorRegenConstGrowth = 1,
    MagicArmorRegenPercentageGrowth = 10,
    MagicArmorAfterHitCooldown = -7,
    ArmorHeadPercentage = 0.15,
    ArmorUpperBodyPercentage = 0.3,
    ArmorLowerBodyPercentage = 0.2,
    ArmorShieldPercentage = 0.5,
    ArmorHandsPercentage = 0.15,
    ArmorFeetPercentage = 0.15,
    ArmorBeltPercentage = 0.1,
    ArmorAmuletPercentage = 0.12,
    ArmorRingPercentage = 0.08,
    SkillMemoryCostReductionFromAbility = 0,
    CharacterBaseMemoryCapacity = 3,
    CharacterBaseMemoryCapacityGrowth = 0.5,
    CharacterAttributePointsPerMemoryCapacity = 1,
    LoremasterBonusToMemory = 2,
    AbilityBaseValue = 0,
    NumStartingCombatAbilityPoints = 2,
    CombatAbilityCap = 10,
    CombatAbilityLevelGrowth = 1,
    CombatAbilityNpcGrowth = 0.1,
    CombatAbilityDamageBonus = 5,
    CombatAbilityCritBonus = 1,
    CombatAbilityCritMultiplierBonus = 5,
    CombatAbilityAccuracyBonus = 5,
    CombatAbilityDodgingBonus = 1,
    CombatAbilityReflectionBonus = 5,
    LeadershipRange = 8,
    LeadershipDodgingBonus = 2,
    LeadershipAllResBonus = 3,
    NumStartingCivilAbilityPoints = 2,
    CivilAbilityCap = 5,
    CivilAbilityLevelGrowth = 4,
    CivilPointOffset = 2,
    SavethrowLowChance = 15,
    SavethrowHighChance = 50,
    SavethrowBelowLowPenalty = 5,
    SavethrowPenaltyCap = -30,
    CriticalBonusFromWits = 1,
    InitiativeBonusFromWits = 1,
    WeaponAccuracyPenaltyPerLevel = -20,
    WeaponAccuracyPenaltyCap = -80,
    ShieldAPCost = 0,
    CharacterWeightLight = 40000,
    CharacterWeightMedium = 70000,
    CharacterWeightHeavy = 120000,
    WeaponWeightLight = 1000,
    WeaponWeightMedium = 3000,
    WeaponWeightHeavy = 6000,
    HighGroundThreshold = 2.4,
    HighGroundBaseDamageBonus = 0.2,
    HighGroundMeleeRange = 1,
    HighGroundRangeMultiplier = 2.5,
    LowGroundBaseDamagePenalty = -0.1,
    SneakDefaultAPCost = 4,
    SneakSpeedBoost = -30,
    BlindRangePenalty = 3,
    RangeBoostedGlobalCap = 30,
    SurfaceDurationFromHitFloorReaction = 18,
    SurfaceDurationFireIgniteOverride = 12,
    SurfaceDurationFromCharacterBleeding = -1,
    SurfaceDurationBlessedCursed = -1,
    SurfaceDurationAfterDecay = -1,
    SmokeDurationAfterDecay = 6,
    DualWieldingAPPenalty = 2,
    DualWieldingDamagePenalty = 0.5,
    GhostLifeTime = 3,
    ChanceToSetStatusOnContact = 100,
    AbilityPhysArmorBonusBase = 5,
    AbilityPhysArmorBonusPerPoint = 2,
    AbilityPhysArmorBonusMax = 5,
    AbilityMagicArmorBonusBase = 5,
    AbilityMagicArmorBonusPerPoint = 2,
    AbilityMagicArmorBonusMax = 5,
    AbilityVitalityBonusBase = 3,
    AbilityVitalityBonusPerPoint = 1,
    AbilityVitalityBonusMax = 3,
    SkillAbilityDamageToPhysicalArmorPerPoint = 0,
    SkillAbilityDamageToMagicArmorPerPoint = 0,
    SkillAbilityArmorRestoredPerPoint = 5,
    SkillAbilityVitalityRestoredPerPoint = 5,
    SkillAbilityHighGroundBonusPerPoint = 5,
    SkillAbilityFireDamageBoostPerPoint = 5,
    SkillAbilityPoisonAndEarthDamageBoostPerPoint = 5,
    SkillAbilityAirDamageBoostPerPoint = 5,
    SkillAbilityWaterDamageBoostPerPoint = 5,
    SkillAbilityPhysicalDamageBoostPerPoint = 5,
    SkillAbilityLifeStealPerPoint = 10,
    LifestealFromReflectionModifier = 0,
    SkillAbilityCritMultiplierPerPoint = 5,
    SkillAbilityMovementSpeedPerPoint = 30,
    SkillAbilitySummonsStatsPerPoint = 4,
    SneakingAbilityMovementSpeedPerPoint = 6,
    TalentAttributePointsBonus = 2,
    TalentCombatAbilityPointsBonus = 1,
    TalentCivilAbilityPointsBonus = 1,
    TalentMemoryBonus = 3,
    TalentQuestRootedMemoryBonus = 3,
    TalentRagerPercentage = 70,
    SoftLevelCap = 20,
    PickpocketRequirementDecreaseFromFinesse = 1,
    SkillCombustionRadius = 3,
    TalentPerfectionistAccuracyBonus = 10,
    TalentPerfectionistCriticalChanceBonus = 10,
    TalentExecutionerActionPointBonus = 2,
    TalentPointOffset = 2,
    TalentViolentMagicCriticalChancePercent = 100,
    TalentPointPerLevels = 5,
    TalentQuickStepPartialApBonus = 1,
    SkillHeightRangeMultiplier = 1,
    AbilityPersuasionBonusPerPoint = 4,
    FreeMovementDistanceWhenAttacking = 1,
    TalentSneakingDamageBonus = 40,
    MaximumSummonsInCombat = 4,
    SpiritVisionFallbackRadius = 10,
    AbilityPerseveranceArmorPerPoint = 5,
    AiCoverProjectileTurnMemory = 2,
    CarryWeightBase = 0,
    CarryWeightPerStr = 10000,
    MoveToCarryWeightRatio = 0.75,
    TalentResistDeathVitalityPercentage = 20,
    DeflectProjectileRange = 1,
    SummoningAbilityBonus = 10,
    SurfaceAbsorbBoostPerTilesCount = 10,
    TalentWhatARushThreshold = 50,
    IncarnateSummoningLevel = 10,
    CleaveRangeOverride = 125,
    DamageToThrownWeightRatio = 0.5,
    FleeDistance = 13,
    GlobalGoldValueMultiplier = 1,
    PriceGrowth = 1.12,
    FirstPriceLeapLevel = 9,
    FirstPriceLeapGrowth = 1.75,
    SecondPriceLeapLevel = 13,
    SecondPriceLeapGrowth = 1.15,
    ThirdPriceLeapLevel = 16,
    ThirdPriceLeapGrowth = 1.5,
    FourthPriceLeapLevel = 18,
    FourthPriceLeapGrowth = 1.15,
    PriceModCasualDifficulty = 2.1,
    PriceModClassicDifficulty = 2.5,
    PriceModHardcoreDifficulty = 2.7,
    PriceBarterCoefficient = 0.1,
    PriceAttitudeCoefficient = 0.005,
    PriceRoundToFiveAfterAmount = 100,
    PriceRoundToTenAfterAmount = 1000,
    LevelCap = 35,
    GMCharacterAttributeCap = 100,
    GMCharacterArmorCap = 999999,
    GMCharacterResistanceMin = -1000,
    GMCharacterResistanceMax = 1000,
    GMCharacterAPCap = 100,
    GMCharacterSPCap = 3,
    GMItemLevelCap = 50,
    GMItemAttributeCap = 100,
    GMItemArmorMin = -999999,
    GMItemArmorMax = 999999,
    GMItemResistanceMin = -1000,
    GMItemResistanceMax = 1000,
    LoneWolfMaxAPBonus = 2,
    LoneWolfAPBonus = 2,
    LoneWolfMagicArmorBoostPercentage = 60,
    LoneWolfArmorBoostPercentage = 60,
    LoneWolfVitalityBoostPercentage = 30,
    LivingArmorHealPercentage = 35,
    TorturerDamageStatusTurnIncrease = 1,
    UnstableDamagePercentage = 50,
    UnstableRadius = 3,
    TalentResurrectExtraHealthPercent = 10,
    PoisonedFoodDamage = 1,
    PoisonedFoodDamageRange = 10,
    PoisonedFoodDamageMultiplier = 40,
    TraderDroppedItemsPercentage = 51,
    TraderDroppedItemsCap = 5,
    StatusDefaultDistancePerDamage = 0.75,
    TraderDonationsRequiredAttitude = -45,
    TeleportUnchainDistance = 50,
    TalentHumanCriticalMultiplier = 10,
    TalentHumanCriticalChance = 5,
    TalentSneakingAPCost = 1
}

--#endregion


--- @class Ext_ClientAudio
local Ext_ClientAudio = {}


--- Get the value for a 'Real-Time Parameter Controls' WWise variable.
--- @param gameObject ObjectHandle|SoundObjectId|nil 
--- @param rtpcName RTPCName|string 
--- @return number
function Ext_ClientAudio.GetRTPC(gameObject, rtpcName) end

function Ext_ClientAudio.PauseAllSounds() end

--- Get a value for a 'Real-Time Parameter Controls' WWise variable.
--- @param gameObject ObjectHandle|SoundObjectId|nil 
--- @param eventName string Event to trigger
--- @param path string Audio file path (relative to data directory)
--- @param codecId integer Audio file path (relative to data directory)
--- @return boolean
function Ext_ClientAudio.PlayExternalSound(gameObject, eventName, path, codecId) end

--- Get a value for a 'Real-Time Parameter Controls' WWise variable.
--- @param gameObject ObjectHandle|SoundObjectId|nil 
--- @param eventName string Event to trigger
--- @param positionSec number|nil Optional position to start at in the audio track.
--- @return boolean
function Ext_ClientAudio.PostEvent(gameObject, eventName, positionSec) end

--- Reset a 'Real-Time Parameter Controls' WWise variable.
--- @param gameObject ObjectHandle|SoundObjectId|nil 
--- @param rtpcName RTPCName|string 
--- @return boolean
function Ext_ClientAudio.ResetRTPC(gameObject, rtpcName) end

function Ext_ClientAudio.ResumeAllSounds() end

--- Set a 'Real-Time Parameter Controls' WWise variable.
--- @param gameObject ObjectHandle|SoundObjectId|nil 
--- @param rtpcName RTPCName|string 
--- @param value number 
--- @return boolean
function Ext_ClientAudio.SetRTPC(gameObject, rtpcName, value) end

--- Set an audio state for a given SyncGroup.  
--- You can find the the group and state names in this file:  
--- `Public\Shared\Assets\Sound\SoundSyncs.lsx`
--- @param stateGroup StateGroupName|string 
--- @param state string A SoundSyncs Switch name within the given SyncGroup, i.e. "Boss_Theme_01".
--- @return boolean
function Ext_ClientAudio.SetState(stateGroup, state) end

--- Set an audio state switch, such as making a character sound like a ghoul.  
--- You can find the the group and switch names in this file:  
--- `Public\Shared\Assets\Sound\SoundSyncs.lsx`
--- @param gameObject ObjectHandle|SoundObjectId|nil 
--- @param switchGroup SwitchGroupName|string 
--- @param state string A SoundSyncs Switch name within the given SyncGroup, i.e. "Boss_Theme_01".
--- @return boolean
function Ext_ClientAudio.SetSwitch(gameObject, switchGroup, state) end

--- Stop audio for an object.
--- @param gameObject ObjectHandle|SoundObjectId|nil 
function Ext_ClientAudio.Stop(gameObject) end



--- @class Ext_ClientBehavior
--- @field AttachCharacterTask fun(a1:EclCharacter, a2:FixedString):boolean
--- @field RegisterCharacterTask fun(a1:FixedString, a2:Ref)
local Ext_ClientBehavior = {}




--- @class Ext_ClientClient
--- @field GetCameraManager fun():RfCameraManager
local Ext_ClientClient = {}


--- @return EsvGameState gameState
function Ext_ClientClient.GetGameState() end

--- @return ModManager
function Ext_ClientClient.GetModManager() end

--- Updates shroud data for a cell in the specified position.
--- Location: Lua/Libs/Client.inl:20
--- @param x number 
--- @param y number 
--- @param layer ShroudType 
--- @param value int32 
function Ext_ClientClient.UpdateShroud(x, y, layer, value) end



--- @class Ext_ClientEntity
--- @field GetCharacterLegacy fun(handle:ComponentHandle|Guid|NetId):EclCharacter
--- @field GetCombatComponent fun(a1:ComponentHandle):EclCombatComponent
--- @field GetItemLegacy fun(handle:ComponentHandle|Guid|NetId):EclItem
--- @field GetPlayerManager fun():EclPlayerManager
--- @field GetProjectile fun(handle:ComponentHandle|NetId):EclProjectile
--- @field GetTurnManager fun():EclTurnManager
local Ext_ClientEntity = {}


--- @return EocAiGrid
function Ext_ClientEntity.GetAiGrid() end

--- @param handle ComponentHandle|Guid|NetId Character Guid, Handle, or NetID.
--- @return EclCharacter character
function Ext_ClientEntity.GetCharacter(handle) end

--- @return EclLevel
function Ext_ClientEntity.GetCurrentLevel() end

--- @param handle ComponentHandle|Guid Game object handle or Guid. NetID will fail since it has no type information (an item and a character could have the same NetID).
--- @return IEoCClientObject object
function Ext_ClientEntity.GetGameObject(handle) end

--- @param handle ComponentHandle 
--- @return EclInventory
function Ext_ClientEntity.GetInventory(handle) end

--- @param handle ComponentHandle|Guid|NetId Character Guid, Handle, or NetID.
--- @return EclItem item
function Ext_ClientEntity.GetItem(handle) end

--- Get a specific, active status from a character.
--- @param characterHandle ComponentHandle|Guid|NetId 
--- @param statusHandle ComponentHandle|NetId 
--- @return EclStatus
function Ext_ClientEntity.GetStatus(characterHandle, statusHandle) end

--- @return ComponentHandle
function Ext_ClientEntity.NullHandle() end



--- @class Ext_ClientInput
--- @field GetInputManager fun():InputManager
--- @field InjectInput fun(a1:FixedString, a2:InputRawType, a3:InputState, a4:number, a5:number, a6:boolean|nil):boolean
local Ext_ClientInput = {}




--- @class Ext_ClientNet
local Ext_ClientNet = {}


--- @param channel string 
--- @param payload string 
function Ext_ClientNet.PostMessageToServer(channel, payload) end



--- @class Ext_ClientTemplate
local Ext_ClientTemplate = {}


--- @param templateId FixedString 
--- @return GameObjectTemplate
function Ext_ClientTemplate.GetCacheTemplate(templateId) end

--- @param templateId FixedString 
--- @return GameObjectTemplate
function Ext_ClientTemplate.GetLocalCacheTemplate(templateId) end

--- @param templateId FixedString 
--- @return GameObjectTemplate
function Ext_ClientTemplate.GetLocalTemplate(templateId) end

--- @param templateId FixedString 
--- @return GameObjectTemplate
function Ext_ClientTemplate.GetRootTemplate(templateId) end

--- @param templateId FixedString 
--- @return GameObjectTemplate
function Ext_ClientTemplate.GetTemplate(templateId) end



--- @class Ext_ClientUI
--- @field GetByHandle fun(a1:ComponentHandle):UIObject
--- @field GetCursorControl fun():EclCursorControl
--- @field GetMouseFlashPos fun():vec2
--- @field GetPickingHelper fun(a1:int32|nil):EclPickingHelper
local Ext_ClientUI = {}


--- @class EclPickingState
--- @field WorldPosition number[] Position of cursor in world coordinates
--- @field WalkablePosition number[] Closest walkable position to cursor in world coordinates
--- @field HoverCharacter ComponentHandle Character hovered by cursor
--- @field HoverCharacter2 ComponentHandle Character hovered by cursor (???)
--- @field HoverCharacterPosition number[] Position of character hovered by cursor
--- @field HoverItem ComponentHandle Item hovered by cursor
--- @field HoverItemPosition number[] Position of item hovered by cursor
--- @field PlaceableEntity ComponentHandle Placeable game object hovered by cursor
--- @field PlaceablePosition number[] Position of placeable game object hovered by cursor

--- @alias BuiltinUISWFName "actionProgression" | "addContent" | "addContent_c" | "areaInteract_c" | "arenaResult" | "book" | "bottomBar_c" | "buttonLayout_c" | "calibrationScreen" | "campaignManager" | "characterAssign" | "characterAssign_c" | "characterCreation" | "characterCreation_c" | "characterSheet" | "chatLog" | "combatLog" | "combatLog_c" | "combatTurn" | "connectionMenu" | "connectivity_c" | "containerInventory" | "containerInventoryGM" | "contextMenu" | "contextMenu_c" | "craftPanel_c" | "credits" | "dialog" | "dialog_c" | "dummyOverhead" | "encounterPanel" | "enemyHealthBar" | "engrave" | "equipmentPanel_c" | "examine" | "examine_c" | "feedback_c" | "formation" | "formation_c" | "fullScreenHUD" | "gameMenu" | "gameMenu_c" | "giftBagContent" | "giftBagsMenu" | "gmInventory" | "GMItemSheet" | "GMJournal" | "GMMetadataBox" | "GMMinimap" | "GMMoodPanel" | "GMPanelHUD" | "GMRewardPanel" | "GMSkills" | "hotBar" | "installScreen_c" | "inventorySkillPanel_c" | "itemAction" | "itemGenerator" | "itemSplitter" | "itemSplitter_c" | "journal" | "journal_c" | "journal_csp" | "loadingScreen" | "mainMenu" | "mainMenu_c" | "menuBG" | "minimap" | "minimap_c" | "mods" | "mods_c" | "monstersSelection" | "mouseIcon" | "msgBox" | "msgBox_c" | "notification" | "optionsInput" | "optionsSettings" | "optionsSettings_c" | "overhead" | "overviewMap" | "panelSelect_c" | "partyInventory" | "partyInventory_c" | "partyManagement_c" | "pause" | "peace" | "playerInfo" | "playerInfo_c" | "possessionBar" | "pyramid" | "pyramid_c" | "reputationPanel" | "reward" | "reward_c" | "roll" | "saveLoad" | "saveLoad_c" | "saving" | "serverlist" | "serverlist_c" | "skills" | "skillsSelection" | "sortBy_c" | "startTurnRequest" | "startTurnRequest_c" | "statsPanel_c" | "statusConsole" | "statusPanel" | "stickiesPanel" | "sticky" | "storyElement" | "surfacePainter" | "textDisplay" | "tooltip" | "trade" | "trade_c" | "tutorialBox" | "tutorialBox_c" | "uiCraft" | "uiFade" | "userProfile" | "vignette" | "voiceNotification_c" | "watermark" | "waypoints" | "waypoints_c" | "worldTooltip"

--- @overload fun(string:BuiltinUISWFName):integer
Ext_ClientUI.TypeID = {
	actionProgression = 0,
	addContent = 57,
	addContent_c = 81,
	areaInteract_c = 68,
	arenaResult = 125,
	book = 2,
	bottomBar_c = 59,
	buttonLayout_c = 95,
	calibrationScreen = 98,
	campaignManager = 124,
	characterAssign = 52,
	characterAssign_c = 92,
	characterCreation = 3,
	characterCreation_c = 4,
	characterSheet = 119,
	chatLog = 6,
	combatLog = 7,
	combatLog_c = 65,
	combatTurn = 8,
	connectionMenu = 33,
	connectivity_c = 34,
	containerInventory = { Default = 9, Pickpocket = 37 },
	containerInventoryGM = 143,
	contextMenu = { Default = 10, Object = 11 },
	contextMenu_c = { Default = 12, Object = 96 },
	craftPanel_c = 84,
	credits = 53,
	dialog = 14,
	dialog_c = 66,
	dummyOverhead = 15,
	encounterPanel = 105,
	enemyHealthBar = 42,
	engrave = 69,
	equipmentPanel_c = 64,
	examine = 104,
	examine_c = 67,
	feedback_c = 97,
	formation = 130,
	formation_c = 135,
	fullScreenHUD = 100,
	gameMenu = 19,
	gameMenu_c = 77,
	giftBagContent = 147,
	giftBagsMenu = 146,
	gmInventory = 126,
	GMItemSheet = 107,
	GMJournal = 139,
	GMMetadataBox = 109,
	GMMinimap = 113,
	GMMoodPanel = 108,
	GMPanelHUD = 120,
	GMRewardPanel = 131,
	GMSkills = 123,
	hotBar = 40,
	installScreen_c = 80,
	inventorySkillPanel_c = 62,
	itemAction = 86,
	itemGenerator = 106,
	itemSplitter = 21,
	itemSplitter_c = 85,
	journal = 22,
	journal_c = 70,
	journal_csp = 140,
	loadingScreen = 23,
	mainMenu = 28,
	mainMenu_c = 87, -- Still mainMenu.swf, but this is used for controllers after clicking "Options" in the gameMenu_c
	menuBG = 56,
	minimap = 30,
	minimap_c = 60,
	mods = 49,
	mods_c = 103,
	monstersSelection = 127,
	mouseIcon = 31,
	msgBox = 29,
	msgBox_c = 75,
	notification = 36,
	optionsInput = 13,
	optionsSettings = { Default = 45, Video = 45, Audio = 1, Game = 17 },
	optionsSettings_c = { Default = 91, Video = 91, Audio = 88, Game = 89 },
	overhead = 5,
	overviewMap = 112,
	panelSelect_c = 83,
	partyInventory = 116,
	partyInventory_c = 142,
	partyManagement_c = 82,
	pause = 121,
	peace = 122,
	playerInfo = 38,
	playerInfo_c = 61, --Still playerInfo.swf, but the ID is different.
	possessionBar = 110,
	pyramid = 129,
	pyramid_c = 134,
	reputationPanel = 138,
	reward = 136,
	reward_c = 137,
	roll = 118,
	saveLoad = 39,
	saveLoad_c = 74,
	saving = 99,
	serverlist = 26,
	serverlist_c = 27,
	skills = 41,
	skillsSelection = 54,
	sortBy_c = 79,
	startTurnRequest = 145,
	startTurnRequest_c = 144,
	statsPanel_c = 63,
	statusConsole = 117,
	statusPanel = 128,
	stickiesPanel = 133,
	sticky = 132,
	storyElement = 71,
	surfacePainter = 111,
	textDisplay = 43,
	tooltip = 44,
	trade = 46,
	trade_c = 73,
	tutorialBox = 55,
	tutorialBox_c = 94,
	uiCraft = 102,
	uiFade = 16,
	userProfile = 51,
	vignette = 114,
	voiceNotification_c = 93,
	watermark = 141,
	waypoints = 47,
	waypoints_c = 78,
	worldTooltip = 48,
}


--- Creates a new UI element. Returns the UI object on success and `nil` on failure.
--- Location: Lua/Libs/ClientUI.inl:15
--- @param name string A user-defined unique name that identifies the UI element. To avoid name collisions, the name should always be prefixed with the mod name (e.g. `NRD_CraftingUI`)
--- @param path string Path of the SWF file relative to the data directory (e.g. `"Public/ModName/GUI/CraftingUI.swf"`).**This path must exist or the game will crash!**
--- @param layer int32 Stack order of the UI element. Overlapping elements with a larger layer value cover those with a smaller one.
--- @param flags int32|UIObjectFlags[]|nil Optional UI object flags; default: `{"Load"|"PlayerInput1"|"DeleteOnChildDestroy"}`
--- @return UIObject
function Ext_ClientUI.Create(name, path, layer, flags) end

--- Destroys the specified UI element.
--- Location: Lua/Libs/ClientUI.inl:141
--- @param name string Name passed to `Ext.UI.Create` when creating the UI element
function Ext_ClientUI.Destroy(name) end

--- double to int64 handle conversion hack for use in Flash external interface calls (Some of the builtin functions treat handles as double values)
--- Location: Lua/Libs/ClientUI.inl:209
--- @param dbl number Flash double value to convert
--- @return ComponentHandle
function Ext_ClientUI.DoubleToHandle(dbl) end

--- Toggles printing of Flash elements where the custom draw callback is being called. (i.e. icons where the image is supplied by engine code)
--- Location: Lua/Libs/ClientUI.inl:186
--- @param enabled boolean 
function Ext_ClientUI.EnableCustomDrawCallDebugging(enabled) end

--- Retrieves a UI element with the specified name. If no such element exists, the function returns `nil`.
--- Location: Lua/Libs/ClientUI.inl:93
--- @param name string Name passed to `Ext.UI.Create` when creating the UI element
--- @return UIObject
function Ext_ClientUI.GetByName(name) end

--- Retrieves a built-in UI element at the specified path. If no such element exists, the function returns `nil`.
--- Location: Lua/Libs/ClientUI.inl:121
--- @param path string SWF path relative to data directory (e.g. `"Public/ModName/GUI/CraftingUI.swf"`)
--- @return UIObject
function Ext_ClientUI.GetByPath(path) end

--- Retrieves an engine UI element. If no such element exists, the function returns `nil`.
--- Location: Lua/Libs/ClientUI.inl:103
--- @param typeId int32 Engine UI ID
--- @return UIObject
function Ext_ClientUI.GetByType(typeId) end

--- Returns the character creation UI. (The object returned by this call can be used to access additional character creation-specific fields that are not available via `GetByPath()` etc.)
--- Location: Lua/Libs/ClientUI.inl:218
--- @return EclCharacterCreationUICharacterCreationWizard
function Ext_ClientUI.GetCharacterCreationWizard() end

--- @return DragDropManager
function Ext_ClientUI.GetDragDrop() end

--- @param playerIndex int32|nil 
--- @return EclPickingState pickingState
function Ext_ClientUI.GetPickingState(playerIndex) end

--- Returns the size of the viewport (game window)
--- Location: Lua/Libs/ClientUI.inl:304
--- @return ivec2
function Ext_ClientUI.GetViewportSize() end

--- int64 handle to double conversion hack for use in Flash external interface calls (Some of the builtin functions treat handles as double values)
--- Location: Lua/Libs/ClientUI.inl:199
--- @param handle ComponentHandle Handle to convert
--- @return number
function Ext_ClientUI.HandleToDouble(handle) end

--- Loads a Flash library; other SWF files can import symbols from this library. Note: The game can load at most 7 additional libraries, so only use this feature when necessary!
--- Location: Lua/Libs/ClientUI.inl:292
--- @param moduleName string Library name
--- @param path string SWF path relative to data directory (e.g. `"Public/ModName/GUI/SomeLibrary.swf"`)
--- @return boolean
function Ext_ClientUI.LoadFlashLibrary(moduleName, path) end

--- Experimental! Forces an UI refresh for the specified character. Supported flag values:
--- 
---  - 0x1 - AP
---  - 0x10 - Abilities
---  - 0x60 - Status icons
---  - 0x40000 - Health
---  - 0x80000 - Skill set
---  - 0x1000000 - Inventory
---  - 0x10000000 - Character transform
---  - 0x80000000 - Relations
--- Location: Lua/Libs/ClientUI.inl:166
--- @param handle ComponentHandle UI object handle
--- @param flags uint64 Dirty flags
function Ext_ClientUI.SetDirty(handle, flags) end



--- @class Ext_ClientVisual
--- @field AddFadingOverlayMaterial fun(a1:ComponentHandle, a2:FixedString, a3:FixedString, a4:number, a5:StatusMaterialApplyFlags, a6:boolean, a7:number|nil, a8:boolean|nil, a9:boolean|nil):EclMaterialInfo
--- @field AddOverlayMaterial fun(a1:ComponentHandle, a2:FixedString, a3:StatusMaterialApplyFlags, a4:boolean, a5:number|nil, a6:boolean|nil):EclMaterialInfo
--- @field AddReplacementMaterial fun(a1:ComponentHandle, a2:FixedString, a3:StatusMaterialApplyFlags, a4:boolean, a5:boolean|nil):EclMaterialInfo
--- @field GetMaterialManager fun():EclMaterialManager
--- @field GetVisual fun(a1:ComponentHandle):Visual
--- @field RemoveFadingMaterial fun(a1:ComponentHandle, a2:FixedString)
--- @field RemoveMaterial fun(a1:ComponentHandle, a2:FixedString)
local Ext_ClientVisual = {}


--- @param position vec3 
--- @return EclLuaVisualClientMultiVisual
function Ext_ClientVisual.Create(position) end

--- @param position vec3 The position of the visual.
--- @param target EclCharacter|nil Optional target.
--- @param listenForTextKeys EclCharacter|nil Optional target to sync with animations.
--- @return EclLuaVisualClientMultiVisual
function Ext_ClientVisual.CreateOnCharacter(position, target, listenForTextKeys) end

--- @param position vec3 The position of the visual.
--- @param target EclItem|nil Optional target.
--- @param listenForTextKeys EclItem|nil Optional target to sync with animations.
--- @return EclLuaVisualClientMultiVisual
function Ext_ClientVisual.CreateOnItem(position, target, listenForTextKeys) end

--- @param handle ComponentHandle 
--- @return EclLuaVisualClientMultiVisual
function Ext_ClientVisual.Get(handle) end



--- @class Ext_Debug
--- @field DebugBreak fun()
local Ext_Debug = {}


function Ext_Debug.DebugDumpLifetimes() end

function Ext_Debug.DumpNetworking() end

function Ext_Debug.DumpStack() end

--- @param builtinOnly boolean|nil 
function Ext_Debug.GenerateIdeHelpers(builtinOnly) end

--- @return boolean
function Ext_Debug.IsDeveloperMode() end



--- @class Ext_IO
local Ext_IO = {}


--- @param path string 
--- @param overridePath string 
function Ext_IO.AddPathOverride(path, overridePath) end

--- @param path string 
--- @return string|nil
function Ext_IO.GetPathOverride(path) end

--- @param path string 
--- @param context FixedString|nil 
--- @return string|nil
function Ext_IO.LoadFile(path, context) end

--- @param path string 
--- @param contents string 
--- @return boolean
function Ext_IO.SaveFile(path, contents) end



--- @class Ext_Json
local Ext_Json = {}

--- @class JsonStringifyOptions
--- @field Beautify boolean Sorts the output table, and indents with tabs. Defaults to true.
--- @field StringifyInternalTypes boolean Defaults to false.
--- @field IterateUserdata boolean Defaults to false.
--- @field AvoidRecursion boolean Defaults to false.
--- @field MaxDepth integer Defaults to 64, the maximum value.
--- @field LimitDepth integer Defaults to -1 (off).
--- @field LimitArrayElements integer Defaults to -1 (off).


--- @param str string String to convert to a table.
--- @return table parsedTable
function Ext_Json.Parse(str) end

--- @param data table Table to convert to a string.
--- @param opts JsonStringifyOptions|nil Optional settings for the stringify operation
--- @return string tableAsString
function Ext_Json.Stringify(data, opts) end



--- @class Ext_L10N
local Ext_L10N = {}


--- @param keyStr string 
--- @param value string 
--- @return string|nil
function Ext_L10N.CreateTranslatedString(keyStr, value) end

--- @param handleStr string 
--- @param value string 
--- @return boolean
function Ext_L10N.CreateTranslatedStringHandle(handleStr, value) end

--- @param keyStr string 
--- @param handleStr string 
--- @return boolean
function Ext_L10N.CreateTranslatedStringKey(keyStr, handleStr) end

--- @param translatedStringKey string 
--- @param fallbackText string|nil 
--- @return string
function Ext_L10N.GetTranslatedString(translatedStringKey, fallbackText) end

--- @param key FixedString 
function Ext_L10N.GetTranslatedStringFromKey(key) end



--- @class Ext_Math
local Ext_Math = {}


--- Arc cosine. Returns an angle whose sine is x.
--- Location: Lua/Libs/Math.inl:810
--- @param x number 
--- @return number
function Ext_Math.Acos(x) end

function Ext_Math.Add() end

--- Returns the absolute angle between two vectors. Parameters need to be normalized.
--- Location: Lua/Libs/Math.inl:357
function Ext_Math.Angle() end

--- Arc sine. Returns an angle whose sine is x.
--- Location: Lua/Libs/Math.inl:818
--- @param x number 
--- @return number
function Ext_Math.Asin(x) end

--- Arc tangent. Returns an angle whose tangent is y_over_x.
--- Location: Lua/Libs/Math.inl:827
--- @param y_over_x number 
--- @return number
function Ext_Math.Atan(y_over_x) end

--- Arc tangent. Returns an angle whose tangent is y / x. The signs of x and y are used to determine what quadrant the angle is in.
--- Location: Lua/Libs/Math.inl:837
--- @param x number 
--- @param y number 
--- @return number
function Ext_Math.Atan2(x, y) end

--- Build a matrix from axis and angle.
--- Location: Lua/Libs/Math.inl:698
--- @param axis vec3 
--- @param angle number 
--- @return mat3
function Ext_Math.BuildFromAxisAngle3(axis, angle) end

--- Build a matrix from axis and angle.
--- Location: Lua/Libs/Math.inl:706
--- @param axis vec3 
--- @param angle number 
--- @return mat4
function Ext_Math.BuildFromAxisAngle4(axis, angle) end

--- Creates a 3D 3 * 3 homogeneous rotation matrix from euler angles `(X * Y * Z)`.
--- Location: Lua/Libs/Math.inl:639
--- @param angle vec3 
--- @return mat3
function Ext_Math.BuildFromEulerAngles3(angle) end

--- Creates a 3D 4 * 4 homogeneous rotation matrix from euler angles `(X * Y * Z)`.
--- Location: Lua/Libs/Math.inl:631
--- @param angle vec3 
--- @return mat4
function Ext_Math.BuildFromEulerAngles4(angle) end

--- Builds a rotation 3 * 3 matrix created from an axis of 3 scalars and an angle expressed in radians.
--- Location: Lua/Libs/Math.inl:581
--- @param v vec3 
--- @param angle number 
--- @return mat3
function Ext_Math.BuildRotation3(v, angle) end

--- Builds a rotation 4 * 4 matrix created from an axis of 3 scalars and an angle expressed in radians.
--- Location: Lua/Libs/Math.inl:573
--- @param v vec3 
--- @param angle number 
--- @return mat4
function Ext_Math.BuildRotation4(v, angle) end

--- Builds a scale 4 * 4 matrix created from 3 scalars.
--- Location: Lua/Libs/Math.inl:597
--- @param v vec3 
--- @return mat4
function Ext_Math.BuildScale(v) end

--- Builds a translation 4 * 4 matrix created from a vector of 3 components.
--- Location: Lua/Libs/Math.inl:589
--- @param v vec3 
--- @return mat4
function Ext_Math.BuildTranslation(v) end

--- Returns `min(max(x, minVal), maxVal)` for each component in x using the floating-point values minVal and maxVal.
--- Location: Lua/Libs/Math.inl:786
--- @param val number 
--- @param min number 
--- @param max number 
--- @return number
function Ext_Math.Clamp(val, min, max) end

--- Returns the cross product of x and y.
--- Location: Lua/Libs/Math.inl:366
--- @param x vec3 
--- @param y vec3 
function Ext_Math.Cross(x, y) end

--- Decomposes a model matrix to translations, rotation and scale components.
--- Location: Lua/Libs/Math.inl:647
--- @param m mat4 
--- @param scale_ vec3 
--- @param yawPitchRoll vec3 
--- @param translation_ vec3 
function Ext_Math.Decompose(m, scale_, yawPitchRoll, translation_) end

--- Return the determinant of a mat3/mat4 matrix.
--- Location: Lua/Libs/Math.inl:450
function Ext_Math.Determinant() end

--- Returns the distance between p0 and p1, i.e., `length(p0 - p1)`.
--- Location: Lua/Libs/Math.inl:381
--- @param p0 vec3 
--- @param p1 vec3 
--- @return number
function Ext_Math.Distance(p0, p1) end

function Ext_Math.Div() end

--- Returns the dot product of x and y, i.e., `result = x * y`.
--- Location: Lua/Libs/Math.inl:390
--- @param x vec3 
--- @param y vec3 
--- @return number
function Ext_Math.Dot(x, y) end

--- Get the axis and angle of the rotation from a matrix.
--- Location: Lua/Libs/Math.inl:666
--- @return number
function Ext_Math.ExtractAxisAngle() end

--- Extracts the `(X * Y * Z)` Euler angles from the rotation matrix M.
--- Location: Lua/Libs/Math.inl:605
--- @return vec3
function Ext_Math.ExtractEulerAngles() end

--- Return x - floor(x).
--- Location: Lua/Libs/Math.inl:762
--- @param val number 
--- @return number
function Ext_Math.Fract(val) end

--- Return the inverse of a mat3/mat4 matrix.
--- Location: Lua/Libs/Math.inl:474
function Ext_Math.Inverse() end

--- Returns true if x holds a positive infinity or negative infinity representation.
--- Location: Lua/Libs/Math.inl:853
--- @param x number 
--- @return boolean
function Ext_Math.IsInf(x) end

--- Returns true if x holds a NaN (not a number) representation.
--- Location: Lua/Libs/Math.inl:845
--- @param x number 
--- @return boolean
function Ext_Math.IsNaN(x) end

--- Returns the length of x, i.e., `sqrt(x * x)`.
--- Location: Lua/Libs/Math.inl:408
function Ext_Math.Length() end

--- Returns x * (1.0 - a) + y * a, i.e., the linear blend of x and y using the floating-point value a.
--- Location: Lua/Libs/Math.inl:802
--- @param x number 
--- @param y number 
--- @param a number 
--- @return number
function Ext_Math.Lerp(x, y, a) end

function Ext_Math.Mul() end

--- Returns a vector in the same direction as x but with length of 1.
--- Location: Lua/Libs/Math.inl:432
function Ext_Math.Normalize() end

--- Treats the first parameter `c` as a column vector and the second parameter `r` as a row vector and does a linear algebraic matrix multiply `c * r`.
--- Location: Lua/Libs/Math.inl:522
function Ext_Math.OuterProduct() end

--- Projects `x` on a perpendicular axis of `normal`.
--- Location: Lua/Libs/Math.inl:730
function Ext_Math.Perpendicular() end

--- Projects `x` on `normal`.
--- Location: Lua/Libs/Math.inl:754
function Ext_Math.Project() end

--- For the incident vector `I` and surface orientation `N`, returns the reflection direction: `result = I - 2.0 * dot(N, I) * N`.
--- Location: Lua/Libs/Math.inl:331
function Ext_Math.Reflect() end

--- Builds a rotation matrix created from an axis of 3 scalars and an angle expressed in radians.
--- Location: Lua/Libs/Math.inl:530
function Ext_Math.Rotate() end

--- Transforms a matrix with a scale 4 * 4 matrix created from 3 scalars.
--- Location: Lua/Libs/Math.inl:564
--- @param m mat4 
--- @param scale vec3 
function Ext_Math.Scale(m, scale) end

--- Returns 1.0 if x > 0, 0.0 if x == 0, or -1.0 if x < 0.
--- Location: Lua/Libs/Math.inl:778
--- @param x number 
--- @return number
function Ext_Math.Sign(x) end

--- Returns 0.0 if x <= edge0 and 1.0 if x >= edge1 and performs smooth Hermite interpolation between 0 and 1 when edge0 < x < edge1.
--- Location: Lua/Libs/Math.inl:794
--- @param edge0 number 
--- @param edge1 number 
--- @param x number 
--- @return number
function Ext_Math.Smoothstep(edge0, edge1, x) end

function Ext_Math.Sub() end

--- Transforms a matrix with a translation 4 * 4 matrix created from a vector of 3 components.
--- Location: Lua/Libs/Math.inl:555
--- @param m mat4 
--- @param translation vec3 
function Ext_Math.Translate(m, translation) end

--- Returns the transposed matrix of `x`.
--- Location: Lua/Libs/Math.inl:498
function Ext_Math.Transpose() end

--- Returns a value equal to the nearest integer to x whose absolute value is not larger than the absolute value of x.
--- Location: Lua/Libs/Math.inl:770
--- @param val number 
--- @return number
function Ext_Math.Trunc(val) end



--- @class Ext_Mod
local Ext_Mod = {}


--- @return Module
function Ext_Mod.GetBaseMod() end

--- Returns the list of loaded module UUIDs in the order they're loaded in.
--- Location: Lua/Libs/Mod.inl:38
--- @return FixedString[]
function Ext_Mod.GetLoadOrder() end

--- Returns detailed information about the specified (loaded) module.
--- Location: Lua/Libs/Mod.inl:108
--- @param modNameGuid string Mod UUID to query
--- @return Module
function Ext_Mod.GetMod(modNameGuid) end

--- Returns detailed information about the specified (loaded) module. This function is deprecated; use `Ext.Mod.GetMod()` instead.
---         Example:
--- ```lua
--- local loadOrder = Ext.Mods.GetLoadOrder()
--- for k, uuid in pairs(loadOrder) do
---     local mod = Ext.GetModInfo(uuid)
---     Ext.Dump(mod)
--- end
--- ```
--- Location: Lua/Libs/Mod.inl:64
--- @param modNameGuid string Mod UUID to query
--- @return LegacyModuleInfo info
function Ext_Mod.GetModInfo(modNameGuid) end

--- Returns whether the module with the specified GUID is loaded. This is equivalent to Osiris `NRD_IsModLoaded`, but is callable when the Osiris scripting runtime is not yet available (i.e. `ModuleLoading˙, etc events).
---         Example:
--- ```lua
--- if (Ext.IsModLoaded("5cc23efe-f451-c414-117d-b68fbc53d32d")) then
---     Ext.Print("Mod loaded")
--- end
--- ```
--- Location: Lua/Libs/Mod.inl:19
--- @param modNameGuid string UUID of mod to check
--- @return boolean
function Ext_Mod.IsModLoaded(modNameGuid) end



--- @class Ext_Resource
local Ext_Resource = {}


--- @param type ResourceType 
--- @param templateId FixedString 
--- @return Resource
function Ext_Resource.Get(type, templateId) end



--- @class Ext_ServerAction
--- @field CreateGameAction fun(a1:GameActionType, a2:FixedString, a3:EsvCharacter):EsvGameAction
--- @field CreateOsirisTask fun(a1:EsvTaskType, a2:EsvCharacter):EsvTask
--- @field DestroyGameAction fun(a1:EsvGameAction)
--- @field ExecuteGameAction fun(action:EsvGameAction, targetPos:vec3|nil):Visual
--- @field QueueOsirisTask fun(a1:EsvTask)
local Ext_ServerAction = {}




--- @class Ext_ServerAi
local Ext_ServerAi = {}


--- @return EsvAiHelpers
function Ext_ServerAi.GetAiHelpers() end

--- @return EsvAiModifiers
function Ext_ServerAi.GetArchetypes() end



--- @class Ext_ServerCombat
--- @field GetTurnManager fun():EsvTurnManager
local Ext_ServerCombat = {}




--- @class CustomStatDefinition
--- @field Id Guid
--- @field Name string
--- @field Description string


--- @class Ext_ServerCustomStat
local Ext_ServerCustomStat = {}


--- @param name string 
--- @param description string 
--- @return FixedString|nil
function Ext_ServerCustomStat.Create(name, description) end

function Ext_ServerCustomStat.GetAll() end

--- @param statId string 
function Ext_ServerCustomStat.GetById(statId) end

--- @param statName string 
function Ext_ServerCustomStat.GetByName(statName) end



--- @class Ext_ServerEffect
local Ext_ServerEffect = {}


--- @param effectName FixedString 
--- @param sourceHandle ComponentHandle 
--- @param castBone FixedString|nil 
--- @return EsvEffect
function Ext_ServerEffect.CreateEffect(effectName, sourceHandle, castBone) end

--- @return ComponentHandle[]
function Ext_ServerEffect.GetAllEffectHandles() end

--- @param handle ComponentHandle 
--- @return EsvEffect
function Ext_ServerEffect.GetEffect(handle) end



--- @class EsvCombatTeam
--- @field TeamId integer A number identifying the team instance
--- @field CombatId integer 	Identifies which combat the team is a participant of
--- @field Initiative integer Computed initiative value of the team
--- @field StillInCombat boolean Can the team currently fight, or is it temporarily out of combat?
--- @field Character EsvCharacter|nil Character object if the team is a character; nil otherwise
--- @field Item EsvItem|nil Item object if the team is an item; nil otherwise

--- @class EsvCombat
--- @field CombatId integer A number identifying the combat instance
--- @field LevelName string Level where the combat is taking place
--- @field IsActive boolean
local EsvCombat = {}

--- Retrieves the turn order of the current round.
--- @return EsvCombatTeam[]
function EsvCombat:GetCurrentTurnOrder() end
    
--- Retrieves the turn order of the next round.
--- @return EsvCombatTeam[]
function EsvCombat:GetNextTurnOrder() end

--- Updates the turn order of the current round. 
--- The turnOrder argument should be a reordered version of the table returned by GetCurrentTurnOrder().
--- Notes:
---  It is possible to remove or add characters to the current turn by adding/removing their Team object from the table.
---  It is possible to add a character to the current turn more than once, the character will only appear once in the UI however.
---  The character whose turn is currently active (the very first item) should not be removed or reordered. 
---    This only applies for GetCurrentTurnOrder, the first item can be freely reordered in GetNextTurnOrder.
---  Changed performed using this function are synchronized to the client at the end of the current server tick.
--- @param turnOrder EsvCombatTeam[]
function EsvCombat:UpdateCurrentTurnOrder(turnOrder) end
    
--- Updates the turn order of the next round. 
--- The turnOrder argument should be a reordered version of the table returned by GetNextTurnOrder().
--- Notes:
---  It is possible to remove or add characters to the next turn by adding/removing their Team object from the table.
---  It is possible to add a character to the next turn more than once, the character will only appear once in the UI however.
---  Changed performed using this function are synchronized to the client at the end of the current server tick.
--- @param turnOrder EsvCombatTeam[]
function EsvCombat:UpdateNextTurnOrder(turnOrder) end
    
--- Retrieves all participants of the combat
--- @return EsvCombatTeam[]
function EsvCombat:GetAllTeams() end


--- @class Ext_ServerEntity
--- @field GetCharacterLegacy fun()
--- @field GetCombatComponent fun(a1:ComponentHandle):EsvCombatComponent
--- @field GetProjectile fun():EsvProjectile
local Ext_ServerEntity = {}


--- @return EocAiGrid
function Ext_ServerEntity.GetAiGrid() end

--- @return EsvAlignmentContainer
function Ext_ServerEntity.GetAlignmentManager() end

--- @param levelName FixedString|nil 
--- @return FixedString[]
function Ext_ServerEntity.GetAllCharacterGuids(levelName) end

--- @param levelName FixedString|nil 
--- @return FixedString[]
function Ext_ServerEntity.GetAllItemGuids(levelName) end

--- @param levelName FixedString|nil 
--- @return FixedString[]
function Ext_ServerEntity.GetAllTriggerGuids(levelName) end

--- @param handle ComponentHandle|Guid|NetId Character Guid, Handle, or NetID.
--- @return EsvCharacter character
function Ext_ServerEntity.GetCharacter(handle) end

--- Returns the UUID of all characters within a radius around the specified point.
--- Location: Lua/Libs/ServerEntity.inl:309
--- @param x number Surface action type
--- @param y number Surface action type
--- @param z number Surface action type
--- @param distance number Surface action type
--- @return FixedString[]
function Ext_ServerEntity.GetCharacterGuidsAroundPosition(x, y, z, distance) end

--- @param combatId uint32 
--- @return EsvCombat combat
function Ext_ServerEntity.GetCombat(combatId) end

--- @return EsvLevel
function Ext_ServerEntity.GetCurrentLevel() end

--- @return LevelDesc levelData
function Ext_ServerEntity.GetCurrentLevelData() end

--- @param handle ComponentHandle|Guid Game object handle or Guid. NetID will fail since it has no type information (an item and a character could have the same NetID).
--- @return IEoCServerObject object
function Ext_ServerEntity.GetGameObject(handle) end

--- @param handle ComponentHandle 
--- @return EsvInventory
function Ext_ServerEntity.GetInventory(handle) end

--- @param handle ComponentHandle|Guid|NetId Character Guid, Handle, or NetID.
--- @return EsvItem item
function Ext_ServerEntity.GetItem(handle) end

--- @param x number 
--- @param y number 
--- @param z number 
--- @param distance number 
--- @return FixedString[]
function Ext_ServerEntity.GetItemGuidsAroundPosition(x, y, z, distance) end

--- Get a specific, active status from a character or item.
--- @param objectHandle ComponentHandle|NetId 
--- @param statusHandle ComponentHandle|integer 
--- @return EsvStatus
function Ext_ServerEntity.GetStatus(objectHandle, statusHandle) end

--- @param handle ComponentHandle 
--- @return EsvSurface
function Ext_ServerEntity.GetSurface(handle) end

--- @param handle ComponentHandle|Guid|NetId Trigger Guid, Handle, or NetID.
--- @return EsvTrigger trigger
function Ext_ServerEntity.GetTrigger(handle) end

--- @return ComponentHandle
function Ext_ServerEntity.NullHandle() end



--- @class Ext_ServerNet
local Ext_ServerNet = {}


--- @param channel string 
--- @param payload string 
--- @param excludeCharacterGuid string|nil 
function Ext_ServerNet.BroadcastMessage(channel, payload, excludeCharacterGuid) end

--- @param characterGuid string 
--- @return boolean|nil
function Ext_ServerNet.PlayerHasExtender(characterGuid) end

--- @param characterGuid string 
--- @param channel string 
--- @param payload string 
function Ext_ServerNet.PostMessageToClient(characterGuid, channel, payload) end

--- @param userId int32 
--- @param channel string 
--- @param payload string 
function Ext_ServerNet.PostMessageToUser(userId, channel, payload) end



--- @class Ext_ServerOsiris
local Ext_ServerOsiris = {}


--- @return boolean
function Ext_ServerOsiris.IsCallable() end

function Ext_ServerOsiris.NewCall() end

function Ext_ServerOsiris.NewEvent() end

function Ext_ServerOsiris.NewQuery() end

--- @param name string 
--- @param arity integer 
--- @param typeName OsirisEventType 
--- @param callback fun(...:string|number)
function Ext_ServerOsiris.RegisterListener(name, arity, typeName, callback) end



--- @class Ext_ServerPropertyList
local Ext_ServerPropertyList = {}


--- @param statsEntryName FixedString 
--- @param propertyName FixedString 
--- @param attacker EsvCharacter 
--- @param position vec3 
--- @param radius number 
--- @param propertyContext StatsPropertyContext 
--- @param isFromItem boolean 
--- @param skillId FixedString|nil 
function Ext_ServerPropertyList.ExecuteExtraPropertiesOnPosition(statsEntryName, propertyName, attacker, position, radius, propertyContext, isFromItem, skillId) end

--- @param statsEntryName FixedString 
--- @param propertyName FixedString 
--- @param attacker EsvCharacter 
--- @param target EsvCharacter 
--- @param position vec3 
--- @param propertyContext StatsPropertyContext 
--- @param isFromItem boolean 
--- @param skillId FixedString|nil 
function Ext_ServerPropertyList.ExecuteExtraPropertiesOnTarget(statsEntryName, propertyName, attacker, target, position, propertyContext, isFromItem, skillId) end

--- Execute the SkillProperties of a given skill on a position.
--- @param skillId FixedString 
--- @param attacker EsvCharacter 
--- @param position vec3 
--- @param radius number 
--- @param propertyContext StatsPropertyContext|StatsPropertyContext[] The context of the skill properties, such as Self or Target, which controls which actions are applied to whom.
--- @param isFromItem boolean 
function Ext_ServerPropertyList.ExecuteSkillPropertiesOnPosition(skillId, attacker, position, radius, propertyContext, isFromItem) end

--- Execute the SkillProperties of a given skill on a character.
--- @param skillId FixedString 
--- @param attacker EsvCharacter 
--- @param target EsvCharacter 
--- @param position vec3 
--- @param propertyContext StatsPropertyContext|StatsPropertyContext[] The context of the skill properties, such as Self or Target, which controls which actions are applied to whom.
--- @param isFromItem boolean 
function Ext_ServerPropertyList.ExecuteSkillPropertiesOnTarget(skillId, attacker, target, position, propertyContext, isFromItem) end



--- @class Ext_ServerServer
--- @field GetLevelManager fun():EsvLevelManager
local Ext_ServerServer = {}


--- @return EsvGameState gameState
function Ext_ServerServer.GetGameState() end

--- @return ModManager
function Ext_ServerServer.GetModManager() end



--- @class Ext_ServerSurfaceAction
local Ext_ServerSurfaceAction = {}


--- @param actionHandle ComponentHandle 
function Ext_ServerSurfaceAction.Cancel(actionHandle) end

--- Prepares a new surface action for execution
--- Location: Lua/Libs/ServerSurfaceAction.inl:16
--- @param type SurfaceActionType Surface action type
--- @return EsvSurfaceAction
function Ext_ServerSurfaceAction.Create(type) end

--- @param action EsvSurfaceAction 
function Ext_ServerSurfaceAction.Execute(action) end



--- @class Ext_ServerTemplate
local Ext_ServerTemplate = {}


--- @param templateId FixedString 
--- @return GameObjectTemplate
function Ext_ServerTemplate.CreateCacheTemplate(templateId) end

--- @param templateId FixedString 
--- @return GameObjectTemplate
function Ext_ServerTemplate.GetCacheTemplate(templateId) end

--- @param templateId FixedString 
--- @return GameObjectTemplate
function Ext_ServerTemplate.GetLocalCacheTemplate(templateId) end

--- @param templateId FixedString 
--- @return GameObjectTemplate
function Ext_ServerTemplate.GetLocalTemplate(templateId) end

--- @param templateId FixedString 
--- @return GameObjectTemplate
function Ext_ServerTemplate.GetRootTemplate(templateId) end

--- @param templateId FixedString 
--- @return GameObjectTemplate
function Ext_ServerTemplate.GetTemplate(templateId) end



--- @class StatEntryWeapon
--- @field Name string
--- @field Level integer
--- @field Using string
--- @field Damage integer
--- @field Act string See Act enumeration
--- @field Handedness string See Handedness enumeration
--- @field IsTwoHanded string See YesNo enumeration
--- @field DamageBoost integer
--- @field DamageFromBase integer
--- @field CriticalDamage integer
--- @field CriticalChance integer
--- @field Movement integer
--- @field Initiative integer
--- @field Requirements StatRequirement[]
--- @field Slot string See Itemslot enumeration
--- @field Durability integer
--- @field DurabilityDegradeSpeed string See Qualifier enumeration
--- @field Value integer
--- @field WeaponType string See WeaponType enumeration
--- @field AnimType string See AnimType enumeration
--- @field WeaponRange integer
--- @field ModifierType string See ModifierType enumeration
--- @field Projectile string
--- @field StrengthBoost string See Penalty Qualifier enumeration
--- @field FinesseBoost string See Penalty Qualifier enumeration
--- @field IntelligenceBoost string See Penalty Qualifier enumeration
--- @field ConstitutionBoost string See Penalty Qualifier enumeration
--- @field MemoryBoost string See Penalty Qualifier enumeration
--- @field WitsBoost string See Penalty Qualifier enumeration
--- @field SingleHanded integer
--- @field TwoHanded integer
--- @field Ranged integer
--- @field DualWielding integer
--- @field RogueLore integer
--- @field WarriorLore integer
--- @field RangerLore integer
--- @field FireSpecialist integer
--- @field WaterSpecialist integer
--- @field AirSpecialist integer
--- @field EarthSpecialist integer
--- @field Sourcery integer
--- @field Necromancy integer
--- @field Polymorph integer
--- @field Summoning integer
--- @field Leadership integer
--- @field PainReflection integer
--- @field Perseverance integer
--- @field Telekinesis integer
--- @field Sneaking integer
--- @field Thievery integer
--- @field Loremaster integer
--- @field Repair integer
--- @field Barter integer
--- @field Persuasion integer
--- @field Luck integer
--- @field Fire integer
--- @field Earth integer
--- @field Water integer
--- @field Air integer
--- @field Poison integer
--- @field Physical integer
--- @field Piercing integer
--- @field SightBoost string See Penalty Qualifier enumeration
--- @field HearingBoost string See Penalty Qualifier enumeration
--- @field VitalityBoost integer
--- @field MagicPointsBoost string See Penalty Qualifier enumeration
--- @field ChanceToHitBoost integer
--- @field APMaximum integer
--- @field APStart integer
--- @field APRecovery integer
--- @field AccuracyBoost integer
--- @field DodgeBoost integer
--- @field Weight integer
--- @field AttackAPCost integer
--- @field ComboCategory string
--- @field Flags string See AttributeFlags enumeration
--- @field Boosts string
--- @field InventoryTab string See InventoryTabs enumeration
--- @field Charges integer
--- @field MaxCharges integer
--- @field Skills string
--- @field Reflection string
--- @field ItemGroup string
--- @field ObjectCategory string
--- @field MinAmount integer
--- @field MaxAmount integer
--- @field Priority integer
--- @field Unique integer
--- @field MinLevel integer
--- @field MaxLevel integer
--- @field ItemColor string
--- @field MaxSummons integer
--- @field RuneSlots integer
--- @field RuneSlots_V1 integer
--- @field NeedsIdentification string See YesNo enumeration
--- @field LifeSteal integer
--- @field CleavePercentage integer
--- @field CleaveAngle integer
--- @field Talents string
--- @field IgnoreVisionBlock string See YesNo enumeration
--- @field Tags string
--- @field ExtraProperties AnyStatProperty[]
local StatEntryWeapon = {
	--- @type string See Damage Type enumeration
	['Damage Type'] = "",
	--- @type integer
	['Damage Range'] = 0,
	--- @type string See ActPart enumeration
	['Act part'] = ""
}

--- @class StatEntryArmor
--- @field Name string
--- @field Level integer
--- @field Using string
--- @field ArmorBoost integer
--- @field MagicArmorBoost integer
--- @field Movement integer
--- @field Initiative integer
--- @field Requirements StatRequirement[]
--- @field Slot string See Itemslot enumeration
--- @field Durability integer
--- @field DurabilityDegradeSpeed string See Qualifier enumeration
--- @field Value integer
--- @field ModifierType string See ModifierType enumeration
--- @field Act string See Act enumeration
--- @field Fire integer
--- @field Air integer
--- @field Water integer
--- @field Earth integer
--- @field Poison integer
--- @field Piercing integer
--- @field Physical integer
--- @field StrengthBoost string See Penalty Qualifier enumeration
--- @field FinesseBoost string See Penalty Qualifier enumeration
--- @field IntelligenceBoost string See Penalty Qualifier enumeration
--- @field ConstitutionBoost string See Penalty Qualifier enumeration
--- @field MemoryBoost string See Penalty Qualifier enumeration
--- @field WitsBoost string See Penalty Qualifier enumeration
--- @field SingleHanded integer
--- @field TwoHanded integer
--- @field Ranged integer
--- @field DualWielding integer
--- @field RogueLore integer
--- @field WarriorLore integer
--- @field RangerLore integer
--- @field FireSpecialist integer
--- @field WaterSpecialist integer
--- @field AirSpecialist integer
--- @field EarthSpecialist integer
--- @field Sourcery integer
--- @field Necromancy integer
--- @field Polymorph integer
--- @field Summoning integer
--- @field PainReflection integer
--- @field Perseverance integer
--- @field Leadership integer
--- @field Telekinesis integer
--- @field Sneaking integer
--- @field Thievery integer
--- @field Loremaster integer
--- @field Repair integer
--- @field Barter integer
--- @field Persuasion integer
--- @field Luck integer
--- @field SightBoost string See Penalty Qualifier enumeration
--- @field HearingBoost string See Penalty Qualifier enumeration
--- @field VitalityBoost integer
--- @field MagicPointsBoost string See Penalty Qualifier enumeration
--- @field ChanceToHitBoost integer
--- @field APMaximum integer
--- @field APStart integer
--- @field APRecovery integer
--- @field AccuracyBoost integer
--- @field DodgeBoost integer
--- @field CriticalChance integer
--- @field ComboCategory string
--- @field Weight integer
--- @field InventoryTab string See InventoryTabs enumeration
--- @field Flags string See AttributeFlags enumeration
--- @field ArmorType string See ArmorType enumeration
--- @field Boosts string
--- @field Skills string
--- @field ItemColor string
--- @field Reflection string
--- @field ItemGroup string
--- @field ObjectCategory string
--- @field MinAmount integer
--- @field MaxAmount integer
--- @field Priority integer
--- @field Unique integer
--- @field MinLevel integer
--- @field MaxLevel integer
--- @field MaxSummons integer
--- @field NeedsIdentification string See YesNo enumeration
--- @field Charges integer
--- @field RuneSlots integer
--- @field RuneSlots_V1 integer
--- @field MaxCharges integer
--- @field Talents string
--- @field Tags string
--- @field ExtraProperties AnyStatProperty[]
local StatEntryArmor = {
	['Armor Defense Value'] = 0,
	['Magic Armor Value'] = 0,
	--- @type string See ActPart enumeration
	['Act part'] = ""
}

--- @class StatEntryShield
--- @field Name string
--- @field Level integer
--- @field Using string
--- @field ArmorBoost integer
--- @field MagicArmorBoost integer
--- @field Movement integer
--- @field Initiative integer
--- @field Requirements StatRequirement[]
--- @field Slot string See Itemslot enumeration
--- @field Durability integer
--- @field DurabilityDegradeSpeed string See Qualifier enumeration
--- @field Value integer
--- @field ModifierType string See ModifierType enumeration
--- @field Act string See Act enumeration
--- @field Fire integer
--- @field Air integer
--- @field Water integer
--- @field Earth integer
--- @field Poison integer
--- @field Piercing integer
--- @field Physical integer
--- @field Blocking integer
--- @field StrengthBoost string See Penalty Qualifier enumeration
--- @field FinesseBoost string See Penalty Qualifier enumeration
--- @field IntelligenceBoost string See Penalty Qualifier enumeration
--- @field ConstitutionBoost string See Penalty Qualifier enumeration
--- @field MemoryBoost string See Penalty Qualifier enumeration
--- @field WitsBoost string See Penalty Qualifier enumeration
--- @field SingleHanded integer
--- @field TwoHanded integer
--- @field Ranged integer
--- @field DualWielding integer
--- @field RogueLore integer
--- @field WarriorLore integer
--- @field RangerLore integer
--- @field FireSpecialist integer
--- @field WaterSpecialist integer
--- @field AirSpecialist integer
--- @field EarthSpecialist integer
--- @field Sourcery integer
--- @field Necromancy integer
--- @field Polymorph integer
--- @field Summoning integer
--- @field Leadership integer
--- @field PainReflection integer
--- @field Perseverance integer
--- @field Telekinesis integer
--- @field Sneaking integer
--- @field Thievery integer
--- @field Loremaster integer
--- @field Repair integer
--- @field Barter integer
--- @field Persuasion integer
--- @field Luck integer
--- @field SightBoost string See Penalty Qualifier enumeration
--- @field HearingBoost string See Penalty Qualifier enumeration
--- @field VitalityBoost integer
--- @field MagicPointsBoost string See Penalty Qualifier enumeration
--- @field ChanceToHitBoost integer
--- @field APMaximum integer
--- @field APStart integer
--- @field APRecovery integer
--- @field AccuracyBoost integer
--- @field DodgeBoost integer
--- @field CriticalChance string See Penalty Qualifier enumeration
--- @field ComboCategory string
--- @field Weight integer
--- @field InventoryTab string See InventoryTabs enumeration
--- @field Flags string See AttributeFlags enumeration
--- @field Skills string
--- @field Reflection string
--- @field ItemGroup string
--- @field ObjectCategory string
--- @field MinAmount integer
--- @field MaxAmount integer
--- @field Priority integer
--- @field Unique integer
--- @field MinLevel integer
--- @field MaxLevel integer
--- @field ItemColor string
--- @field MaxSummons integer
--- @field RuneSlots integer
--- @field RuneSlots_V1 integer
--- @field NeedsIdentification string See YesNo enumeration
--- @field Talents string
--- @field Tags string
--- @field ExtraProperties AnyStatProperty[]
local StatEntryShield = {
	['Armor Defense Value'] = 0,
	['Magic Armor Value'] = 0,
	--- @type string See ActPart enumeration
	['Act part'] = ""
}

--- @class StatEntryPotion
--- @field Name string
--- @field Level integer
--- @field Using string
--- @field ModifierType string See ModifierType enumeration
--- @field VitalityBoost integer
--- @field Strength string See Penalty PreciseQualifier enumeration
--- @field Finesse string See Penalty PreciseQualifier enumeration
--- @field Intelligence string See Penalty PreciseQualifier enumeration
--- @field Constitution string See Penalty PreciseQualifier enumeration
--- @field Memory string See Penalty PreciseQualifier enumeration
--- @field Wits string See Penalty PreciseQualifier enumeration
--- @field SingleHanded integer
--- @field TwoHanded integer
--- @field Ranged integer
--- @field DualWielding integer
--- @field RogueLore integer
--- @field WarriorLore integer
--- @field RangerLore integer
--- @field FireSpecialist integer
--- @field WaterSpecialist integer
--- @field AirSpecialist integer
--- @field EarthSpecialist integer
--- @field Sourcery integer
--- @field Necromancy integer
--- @field Polymorph integer
--- @field Summoning integer
--- @field PainReflection integer
--- @field Perseverance integer
--- @field Leadership integer
--- @field Telekinesis integer
--- @field Sneaking integer
--- @field Thievery integer
--- @field Loremaster integer
--- @field Repair integer
--- @field Barter integer
--- @field Persuasion integer
--- @field Luck integer
--- @field FireResistance integer
--- @field EarthResistance integer
--- @field WaterResistance integer
--- @field AirResistance integer
--- @field PoisonResistance integer
--- @field PhysicalResistance integer
--- @field PiercingResistance integer
--- @field Sight integer
--- @field Hearing string See Penalty Qualifier enumeration
--- @field Initiative integer
--- @field Vitality integer
--- @field VitalityPercentage integer
--- @field MagicPoints integer
--- @field ActionPoints integer
--- @field ChanceToHitBoost integer
--- @field AccuracyBoost integer
--- @field DodgeBoost integer
--- @field DamageBoost integer
--- @field APCostBoost integer
--- @field SPCostBoost integer
--- @field APMaximum integer
--- @field APStart integer
--- @field APRecovery integer
--- @field Movement integer
--- @field MovementSpeedBoost integer
--- @field Gain string See BigQualifier enumeration
--- @field Armor integer
--- @field MagicArmor integer
--- @field ArmorBoost integer
--- @field MagicArmorBoost integer
--- @field CriticalChance integer
--- @field Act string See Act enumeration
--- @field Duration integer
--- @field UseAPCost integer
--- @field ComboCategory string
--- @field StackId string
--- @field BoostConditions string
--- @field Flags string See AttributeFlags enumeration
--- @field StatusMaterial string
--- @field StatusEffect string
--- @field StatusIcon string
--- @field SavingThrow string See SavingThrow enumeration
--- @field Weight integer
--- @field Value integer
--- @field InventoryTab string See InventoryTabs enumeration
--- @field UnknownBeforeConsume string See YesNo enumeration
--- @field Reflection string
--- @field Damage string See Qualifier enumeration
--- @field DamageType string See Damage Type enumeration
--- @field AuraRadius integer
--- @field AuraSelf string
--- @field AuraAllies string
--- @field AuraEnemies string
--- @field AuraNeutrals string
--- @field AuraItems string
--- @field AuraFX string
--- @field RootTemplate string
--- @field ObjectCategory string
--- @field MinAmount integer
--- @field MaxAmount integer
--- @field Priority integer
--- @field Unique integer
--- @field MinLevel integer
--- @field MaxLevel integer
--- @field BloodSurfaceType string
--- @field MaxSummons integer
--- @field AddToBottomBar string See YesNo enumeration
--- @field SummonLifelinkModifier integer
--- @field IgnoredByAI string See YesNo enumeration
--- @field RangeBoost integer
--- @field BonusWeapon string
--- @field AiCalculationStatsOverride string
--- @field RuneEffectWeapon string
--- @field RuneEffectUpperbody string
--- @field RuneEffectAmulet string
--- @field RuneLevel integer
--- @field LifeSteal integer
--- @field IsFood string See YesNo enumeration
--- @field IsConsumable string See YesNo enumeration
--- @field ExtraProperties AnyStatProperty[]
local StatEntryPotion = {
	['Damage Multiplier'] = 0,
	['Damage Range'] = 0,
	--- @type string See ActPart enumeration
	['Act part'] = ""
}

--- @class StatEntryObject
--- @field Name string
--- @field Level integer
--- @field Using string
--- @field ModifierType string See ModifierType enumeration
--- @field Act string See Act enumeration
--- @field UseAPCost integer
--- @field Value integer
--- @field ComboCategory string
--- @field Weight integer
--- @field Strength string See PreciseQualifier enumeration
--- @field Finesse string See PreciseQualifier enumeration
--- @field Intelligence string See PreciseQualifier enumeration
--- @field Constitution string See PreciseQualifier enumeration
--- @field Memory string See PreciseQualifier enumeration
--- @field Wits string See PreciseQualifier enumeration
--- @field Vitality integer
--- @field Armor string See PreciseQualifier enumeration
--- @field FireResistance integer
--- @field EarthResistance integer
--- @field WaterResistance integer
--- @field AirResistance integer
--- @field PoisonResistance integer
--- @field PiercingResistance integer
--- @field PhysicalResistance integer
--- @field ShadowResistance integer
--- @field Flags string See AttributeFlags enumeration
--- @field Requirements StatRequirement[]
--- @field InventoryTab string See InventoryTabs enumeration
--- @field RootTemplate string
--- @field ObjectCategory string
--- @field MinAmount integer
--- @field MaxAmount integer
--- @field Priority integer
--- @field Unique integer
--- @field MinLevel integer
--- @field RuneEffectWeapon string
--- @field RuneEffectUpperbody string
--- @field RuneEffectAmulet string
--- @field RuneLevel integer
--- @field MaxLevel integer
--- @field AddToBottomBar string See YesNo enumeration
--- @field IgnoredByAI string See YesNo enumeration
local StatEntryObject = {
	--- @type string See ActPart enumeration
	['Act part'] = ""
}

--- @class StatEntryCharacter
--- @field Name string
--- @field Level integer
--- @field Using string
--- @field Strength string See PreciseQualifier enumeration
--- @field Finesse string See PreciseQualifier enumeration
--- @field Intelligence string See PreciseQualifier enumeration
--- @field Constitution string See PreciseQualifier enumeration
--- @field Memory string See PreciseQualifier enumeration
--- @field Wits string See PreciseQualifier enumeration
--- @field SingleHanded integer
--- @field TwoHanded integer
--- @field Ranged integer
--- @field DualWielding integer
--- @field RogueLore integer
--- @field WarriorLore integer
--- @field RangerLore integer
--- @field FireSpecialist integer
--- @field WaterSpecialist integer
--- @field AirSpecialist integer
--- @field EarthSpecialist integer
--- @field Sourcery integer
--- @field Necromancy integer
--- @field Polymorph integer
--- @field Summoning integer
--- @field PainReflection integer
--- @field Leadership integer
--- @field Perseverance integer
--- @field Telekinesis integer
--- @field Sneaking integer
--- @field Thievery integer
--- @field Loremaster integer
--- @field Repair integer
--- @field Barter integer
--- @field Persuasion integer
--- @field Luck integer
--- @field FireResistance integer
--- @field EarthResistance integer
--- @field WaterResistance integer
--- @field AirResistance integer
--- @field PoisonResistance integer
--- @field PiercingResistance integer
--- @field PhysicalResistance integer
--- @field Sight string See Penalty Qualifier enumeration
--- @field Hearing string See Penalty Qualifier enumeration
--- @field FOV integer
--- @field APMaximum integer
--- @field APStart integer
--- @field APRecovery integer
--- @field Initiative integer
--- @field Vitality integer
--- @field MagicPoints integer
--- @field ChanceToHitBoost integer
--- @field Movement integer
--- @field MovementSpeedBoost integer
--- @field CriticalChance string See Penalty Qualifier enumeration
--- @field Gain string See BigQualifier enumeration
--- @field Armor integer
--- @field ArmorBoost integer
--- @field ArmorBoostGrowthPerLevel integer
--- @field MagicArmor integer
--- @field MagicArmorBoost integer
--- @field MagicArmorBoostGrowthPerLevel integer
--- @field Accuracy integer
--- @field Dodge integer
--- @field Act string See Act enumeration
--- @field MaxResistance integer
--- @field Weight integer
--- @field Talents string
--- @field Traits string
--- @field PathInfluence string
--- @field Flags string See AttributeFlags enumeration
--- @field Reflection string
--- @field StepsType string See StepsType enumeration
--- @field MaxSummons integer
--- @field MPStart integer
--- @field DamageBoost integer
--- @field DamageBoostGrowthPerLevel integer
local StatEntryCharacter = {
	--- @type string See ActPart enumeration
	['Act part'] = "",
	--- @type string See Penalty Qualifier enumeration
	['Act strength'] = ""
}


--- @class StatEntrySkillData
--- @field Name string
--- @field Level integer
--- @field Using string
--- @field SkillType string
--- @field Ability SkillAbility
--- @field Element SkillElement
--- @field Requirement string See SkillRequirement enumeration
--- @field Requirements StatRequirement[]
--- @field DisplayName string
--- @field DisplayNameRef string
--- @field Description string
--- @field DescriptionRef string
--- @field StatsDescription string
--- @field StatsDescriptionRef string
--- @field StatsDescriptionParams string
--- @field Icon string
--- @field FXScale integer
--- @field PrepareAnimationInit string
--- @field PrepareAnimationLoop string
--- @field PrepareEffect string
--- @field PrepareEffectBone string
--- @field CastAnimation string
--- @field CastTextEvent string
--- @field CastAnimationCheck string See CastCheckType enumeration
--- @field CastEffect string
--- @field CastEffectTextEvent string
--- @field TargetCastEffect string
--- @field TargetHitEffect string
--- @field TargetEffect string
--- @field SourceTargetEffect string
--- @field TargetTargetEffect string
--- @field LandingEffect string
--- @field ImpactEffect string
--- @field MaleImpactEffects string
--- @field FemaleImpactEffects string
--- @field OnHitEffect string
--- @field SelectedCharacterEffect string
--- @field SelectedObjectEffect string
--- @field SelectedPositionEffect string
--- @field DisappearEffect string
--- @field ReappearEffect string
--- @field ReappearEffectTextEvent string
--- @field RainEffect string
--- @field StormEffect string
--- @field FlyEffect string
--- @field SpatterEffect string
--- @field ShieldMaterial string
--- @field ShieldEffect string
--- @field ContinueEffect string
--- @field SkillEffect string
--- @field Template string
--- @field TemplateCheck string See CastCheckType enumeration
--- @field TemplateOverride string
--- @field TemplateAdvanced string
--- @field Totem string See YesNo enumeration
--- @field Template1 string
--- @field Template2 string
--- @field Template3 string
--- @field WeaponBones string
--- @field TeleportSelf string See YesNo enumeration
--- @field CanTargetCharacters string See YesNo enumeration
--- @field CanTargetItems string See YesNo enumeration
--- @field CanTargetTerrain string See YesNo enumeration
--- @field ForceTarget string See YesNo enumeration
--- @field TargetProjectiles string See YesNo enumeration
--- @field UseCharacterStats string See YesNo enumeration
--- @field UseWeaponDamage string See YesNo enumeration
--- @field UseWeaponProperties string See YesNo enumeration
--- @field SingleSource string See YesNo enumeration
--- @field ContinueOnKill string See YesNo enumeration
--- @field Autocast string See YesNo enumeration
--- @field AmountOfTargets integer
--- @field AutoAim string See YesNo enumeration
--- @field AddWeaponRange string See YesNo enumeration
--- @field ActionPoints integer
--- @field Cooldown integer
--- @field CooldownReduction integer
--- @field ChargeDuration integer
--- @field CastDelay integer
--- @field Offset integer
--- @field Lifetime integer
--- @field Duration string See Qualifier enumeration
--- @field TargetRadius integer
--- @field ExplodeRadius integer
--- @field AreaRadius integer
--- @field HitRadius integer
--- @field RadiusMax integer
--- @field Range integer
--- @field MaxDistance integer
--- @field Angle integer
--- @field TravelSpeed integer
--- @field Acceleration integer
--- @field Height integer
--- @field Damage string See DamageSourceType enumeration
--- @field DamageType string See Damage Type enumeration
--- @field DamageMultiplier string See PreciseQualifier enumeration
--- @field DeathType string See Death Type enumeration
--- @field BonusDamage string See Qualifier enumeration
--- @field HitPointsPercent integer
--- @field MinHitsPerTurn integer
--- @field MaxHitsPerTurn integer
--- @field HitDelay integer
--- @field MaxAttacks integer
--- @field NextAttackChance integer
--- @field NextAttackChanceDivider integer
--- @field EndPosRadius integer
--- @field JumpDelay integer
--- @field TeleportDelay integer
--- @field PointsMaxOffset integer
--- @field RandomPoints integer
--- @field ChanceToPierce integer
--- @field MaxPierceCount integer
--- @field MaxForkCount integer
--- @field ForkLevels integer
--- @field ForkChance integer
--- @field HealAmount string See PreciseQualifier enumeration
--- @field StatusClearChance integer
--- @field SurfaceType string See Surface Type enumeration
--- @field SurfaceLifetime integer
--- @field SurfaceStatusChance integer
--- @field SurfaceTileCollision string See SurfaceCollisionFlags enumeration
--- @field SurfaceGrowInterval integer
--- @field SurfaceGrowStep integer
--- @field SurfaceRadius integer
--- @field TotalSurfaceCells integer
--- @field SurfaceMinSpawnRadius integer
--- @field MinSurfaces integer
--- @field MaxSurfaces integer
--- @field MinSurfaceSize integer
--- @field MaxSurfaceSize integer
--- @field GrowSpeed integer
--- @field GrowOnSurface string See SurfaceCollisionFlags enumeration
--- @field GrowTimeout integer
--- @field SkillBoost string
--- @field SkillAttributeFlags string See AttributeFlags enumeration
--- @field SkillProperties AnyStatProperty[]
--- @field CleanseStatuses string
--- @field AoEConditions string See Conditions enumeration
--- @field TargetConditions string See Conditions enumeration
--- @field ForkingConditions string See Conditions enumeration
--- @field CycleConditions string See Conditions enumeration
--- @field ShockWaveDuration integer
--- @field TeleportTextEvent string
--- @field SummonEffect string
--- @field ProjectileCount integer
--- @field ProjectileDelay integer
--- @field StrikeCount integer
--- @field StrikeDelay integer
--- @field PreviewStrikeHits string See YesNo enumeration
--- @field SummonLevel integer
--- @field StartTextEvent string
--- @field StopTextEvent string
--- @field Atmosphere string See AtmosphereType enumeration
--- @field ConsequencesStartTime integer
--- @field ConsequencesDuration integer
--- @field HealthBarColor integer
--- @field Skillbook string
--- @field PreviewImpactEffect string
--- @field IgnoreVisionBlock string See YesNo enumeration
--- @field HealEffectId string
--- @field AddRangeFromAbility string See Ability enumeration
--- @field DivideDamage string See YesNo enumeration
--- @field OverrideMinAP string See YesNo enumeration
--- @field OverrideSkillLevel string See YesNo enumeration
--- @field Tier string See SkillTier enumeration
--- @field GrenadeBone string
--- @field GrenadeProjectile string
--- @field GrenadePath string
--- @field MovingObject string
--- @field SpawnObject string
--- @field SpawnEffect string
--- @field SpawnFXOverridesImpactFX string See YesNo enumeration
--- @field SpawnLifetime integer
--- @field ProjectileTerrainOffset string See YesNo enumeration
--- @field ProjectileType string See ProjectileType enumeration
--- @field HitEffect string
--- @field PushDistance integer
--- @field ForceMove string See YesNo enumeration
--- @field Stealth string See YesNo enumeration
--- @field Distribution string See ProjectileDistribution enumeration
--- @field Shuffle string See YesNo enumeration
--- @field PushPullEffect string
--- @field BackStart integer
--- @field FrontOffset integer
--- @field TargetGroundEffect string
--- @field PositionEffect string
--- @field BeamEffect string
--- @field PreviewEffect string
--- @field CastSelfAnimation string
--- @field IgnoreCursed string See YesNo enumeration
--- @field IsEnemySkill string See YesNo enumeration
--- @field DomeEffect string
--- @field AuraSelf string
--- @field AuraAllies string
--- @field AuraEnemies string
--- @field AuraNeutrals string
--- @field AuraItems string
--- @field AIFlags string See AIFlags enumeration
--- @field Shape string
--- @field Base integer
--- @field AiCalculationSkillOverride string
--- @field TeleportSurface string See YesNo enumeration
--- @field ProjectileSkills string
--- @field SummonCount integer
--- @field LinkTeleports string See YesNo enumeration
--- @field TeleportsUseCount integer
--- @field HeightOffset integer
--- @field ForGameMaster string See YesNo enumeration
--- @field IsMelee string See YesNo enumeration
--- @field MemorizationRequirements StatRequirement[]
--- @field IgnoreSilence string See YesNo enumeration
--- @field IgnoreHeight string See YesNo enumeration
local StatEntrySkillData = {
	['Memory Cost'] = 0,
	['Magic Cost'] = 0,
	['Damage Multiplier'] = 0,
	['Damage Range'] = 0,
	['Chance To Hit Multiplier'] = 0,
	['Damage On Jump'] = 0,
	['Damage On Landing'] = 0,
	['Healing Multiplier'] = 0,
	['Stealth Damage Multiplier'] = 0,
	['Distance Damage Multiplier'] = 0
}

--- @class StatEntryStatusData
--- @field Name string
--- @field Level integer
--- @field Using string
--- @field StatusType string
--- @field Icon string
--- @field DisplayName string
--- @field DisplayNameRef string
--- @field Description string
--- @field DescriptionRef string
--- @field DescriptionParams string
--- @field OverrideDefaultDescription string See YesNo enumeration
--- @field FormatColor string See FormatStringColor enumeration
--- @field SavingThrow string See SavingThrow enumeration
--- @field IsChanneled string See YesNo enumeration
--- @field Instant string See YesNo enumeration
--- @field StatusEffect string
--- @field StatusEffectOverrideForItems string
--- @field StatusEffectOnTurn string
--- @field MaterialType string See MaterialType enumeration
--- @field Material string
--- @field MaterialApplyBody string See YesNo enumeration
--- @field MaterialApplyArmor string See YesNo enumeration
--- @field MaterialApplyWeapon string See YesNo enumeration
--- @field MaterialApplyNormalMap string See YesNo enumeration
--- @field MaterialFadeAmount integer
--- @field MaterialOverlayOffset integer
--- @field MaterialParameters string
--- @field HealingEvent string See StatusEvent enumeration
--- @field HealStat string See StatusHealType enumeration
--- @field HealType string See HealValueType enumeration
--- @field HealValue integer
--- @field StatsId string
--- @field IsInvulnerable string See YesNo enumeration
--- @field IsDisarmed string See YesNo enumeration
--- @field StackId string
--- @field StackPriority integer
--- @field AuraRadius integer
--- @field AuraSelf string
--- @field AuraAllies string
--- @field AuraEnemies string
--- @field AuraNeutrals string
--- @field AuraItems string
--- @field AuraFX string
--- @field ImmuneFlag string See AttributeFlag enumeration
--- @field CleanseStatuses string
--- @field MaxCleanseCount integer
--- @field ApplyAfterCleanse string See YesNo enumeration
--- @field SoundStart string
--- @field SoundLoop string
--- @field SoundStop string
--- @field DamageEvent string See StatusEvent enumeration
--- @field DamageStats string
--- @field DeathType string See Death Type enumeration
--- @field DamageCharacters string See YesNo enumeration
--- @field DamageItems string See YesNo enumeration
--- @field DamageTorches string See YesNo enumeration
--- @field FreezeTime integer
--- @field SurfaceChange string
--- @field PermanentOnTorch string See YesNo enumeration
--- @field AbsorbSurfaceType string
--- @field AbsorbSurfaceRange integer
--- @field Skills string
--- @field BonusFromAbility string See Ability enumeration
--- @field Items string
--- @field OnlyWhileMoving string See YesNo enumeration
--- @field DescriptionCaster string
--- @field DescriptionTarget string
--- @field WinBoost string See Properties enumeration
--- @field LoseBoost string See Properties enumeration
--- @field WeaponOverride string
--- @field ApplyEffect string
--- @field ForGameMaster string See YesNo enumeration
--- @field ResetCooldowns string
--- @field ResetOncePerCombat string See YesNo enumeration
--- @field PolymorphResult string
--- @field DisableInteractions string See YesNo enumeration
--- @field LoseControl string See YesNo enumeration
--- @field AiCalculationSkillOverride string
--- @field HealEffectId string
--- @field ScaleWithVitality string See YesNo enumeration
--- @field VampirismType string See VampirismType enumeration
--- @field BeamEffect string
--- @field HealMultiplier integer
--- @field InitiateCombat string See YesNo enumeration
--- @field Projectile string
--- @field Radius integer
--- @field Charges integer
--- @field MaxCharges integer
--- @field DefendTargetPosition string See YesNo enumeration
--- @field TargetConditions string See Conditions enumeration
--- @field Toggle string See YesNo enumeration
--- @field LeaveAction string
--- @field DieAction string
--- @field PlayerSameParty string See YesNo enumeration
--- @field PlayerHasTag string
--- @field PeaceOnly string See YesNo enumeration
--- @field Necromantic string See YesNo enumeration
--- @field RetainSkills string
--- @field BringIntoCombat string See YesNo enumeration
--- @field ApplyStatusOnTick string
--- @field IsResistingDeath string See YesNo enumeration
--- @field TargetEffect string
--- @field DamagePercentage integer
--- @field ForceOverhead string See YesNo enumeration
--- @field TickSFX string
--- @field ForceStackOverwrite string See YesNo enumeration
--- @field FreezeCooldowns string See YesNo enumeration

--- @class StatEntryCrime
--- @field Name string
--- @field Level integer
--- @field Using string
--- @field CrimeType string
--- @field TensionWeight integer
--- @field Lifetime integer
--- @field Audible string See YesNo enumeration
--- @field DetectionRange integer
--- @field CreatesCrimescene string See YesNo enumeration
--- @field Continuous string See YesNo enumeration
--- @field VictimReaction string
--- @field NoticedAD string
--- @field WarningDialog string
--- @field ArrestDialog string
--- @field InterrogateDialog string
--- @field ReactionCategory_Tag string
--- @field ReactionCategory_Priority integer
--- @field ReactionCategory_PrimaryReaction string
--- @field ReactionCategory_SecondaryReaction string
--- @field MaxInvestigators integer
--- @field ContinuousDelayTimer integer
--- @field SoundEvent string
--- @field AudibleRange integer
--- @field VictimDialog string
--- @field CanRaiseAlarm string See YesNo enumeration
--- @field CanMerge string See YesNo enumeration

--- @class StatSkillSet
--- @field Name string
--- @field Skills string[]

--- @class StatEquipmentGroup
--- @field Name string
--- @field Equipment string[]

--- @class StatEquipmentSet
--- @field Name string
--- @field Groups StatEquipmentGroup[]

--- @class DeltaModBoost
--- @field Boost string
--- @field Count integer
local DeltaModBoost = {}

--- @class DeltaMod
--- @field ModifierType string
--- @field SlotType string
--- @field WeaponType string
--- @field ArmorType string
--- @field Handedness string
--- @field Name string
--- @field BoostType string
--- @field MinLevel integer
--- @field MaxLevel integer
--- @field Frequency integer
--- @field Boosts DeltaModBoost[]
local DeltaMod = {}

---@class DeltaModStatEntry
---@field Name string
---@field ModifierType string

--- @class ItemComboIngredients
--- @field Object string
--- @field IngredientType string See IngredientType enumeration
--- @field Transform string See IngredientTransformType enumeration
--- @field ItemRarity string See ItemDataRarity enumeration (ValueLists.txt only!)
local ItemComboIngredients = {}

--- @class ItemComboResultElement
--- @field Result string
--- @field Boost string
--- @field ResultAmount number
local ItemComboResultElement = {}

--- @class ItemComboResult
--- @field Requirement string See Ability enum
--- @field ReqLevel number
--- @field PreviewStatsId string
--- @field PreviewIcon string
--- @field PreviewTooltip string
--- @field Name string
--- @field Results ItemComboResultElement[]
local ItemComboResult = {}

--- @class ItemCombo
--- @field Name string
--- @field RecipeCategory string See RecipeCategory enum
--- @field CraftingStation string See CraftingStationType enum
--- @field Ingredients ItemComboIngredients[]
--- @field Results ItemComboResult[]
local ItemCombo = {}

--- @class ItemComboPreviewData
--- @field Name string
--- @field Type string
--- @field StatsId string
--- @field Tooltip string
--- @field Icon string
local ItemComboPreviewData = {}

--- @class ItemComboPropertyElement
--- @field ObjectId string
--- @field IngredientType string See IngredientType enumeration
--- @field Result string
local ItemComboPropertyElement = {}

--- @class ItemComboProperty
--- @field Name string
--- @field PreviewIcon string
--- @field PreviewTooltip string
--- @field Entries ItemComboPropertyElement[]
local ItemComboProperty = {}

---@class StatTreasureTableCategory
---@field Frequency integer
---@field TreasureTable string|nil
---@field TreasureCategory string|nil
---@field Common integer
---@field Uncommon integer
---@field Rare integer
---@field Epic integer
---@field Legendary integer
---@field Divine integer
---@field Unique integer

---@class StatTreasureDropCount
---@field Chance integer
---@field Amount integer

---@class StatTreasureTableSubTable
---@field Categories StatTreasureTableCategory[]
---@field DropCounts StatTreasureDropCount[]
---@field Amounts integer[]
---@field StartLevel integer
---@field EndLevel integer
---@field TotalCount integer
---@field TotalFrequency integer

---@class StatTreasureTable
---@field Name string
---@field MinLevel integer
---@field MaxLevel integer
---@field IgnoreLevelDiff boolean
---@field UseTreasureGroupContainers boolean
---@field CanMerge boolean
---@field SubTables StatTreasureTableSubTable[]

---@class StatTreasureCategory
---@field Category FixedString
---@field Items StatTreasureCategoryItem[]

---@class StatTreasureCategoryItem
---@field Name FixedString
---@field ActPart integer
---@field MinAmount integer
---@field MaxAmount integer
---@field MinLevel integer
---@field MaxLevel integer
---@field Priority integer
---@field Unique integer

--- @class ItemNameGroupLink
--- @field NameGroup string
--- @field NoneCoolSuffix number
--- @field ItemName string
local ItemNameGroupLink = {}

--- @class ItemRootGroup
--- @field MinLevel number
--- @field MaxLevel number
--- @field RootGroup string
--- @field NameGroupLinks ItemNameGroupLink[]
local ItemRootGroup = {}

--- @class ItemLevelGroup
--- @field MinLevel number
--- @field MaxLevel number
--- @field Name string
--- @field RootGroups ItemRootGroup[]
local ItemLevelGroup = {}

--- @class ItemGroup
--- @field Name string
--- @field LevelGroups ItemLevelGroup[]
local ItemGroup = {}

--- @class ItemNameGroupName
--- @field Name string
--- @field Name2 string
local ItemNameGroupName = {}

--- @class ItemNameGroup
--- @field Name string
--- @field Names ItemNameGroupName[]
--- @field NamesCool ItemNameGroupName[]
local ItemNameGroup = {}


--- @class StatRequirement
--- @field Requirement string
--- @field Param string|integer
--- @field Not boolean
local StatRequirement = {}


--- @class Ext_Stats
--- @field DeltaMod Ext_StatsDeltaMod
--- @field EquipmentSet Ext_StatsEquipmentSet
--- @field ItemColor Ext_StatsItemColor
--- @field ItemCombo Ext_StatsItemCombo
--- @field ItemComboPreview Ext_StatsItemComboPreview
--- @field ItemComboProperty Ext_StatsItemComboProperty
--- @field ItemGroup Ext_StatsItemGroup
--- @field ItemSet Ext_StatsItemSet
--- @field NameGroup Ext_StatsNameGroup
--- @field SkillSet Ext_StatsSkillSet
--- @field TreasureCategory Ext_StatsTreasureCategory
--- @field TreasureTable Ext_StatsTreasureTable
--- @field AddAttribute fun(a1:FixedString, a2:FixedString, a3:FixedString):boolean
--- @field AddDamageType fun(a1:FixedString, a2:boolean|nil):CustomDamageTypeDescriptor
--- @field AddEnumerationValue fun(a1:FixedString, a2:FixedString):int32|nil
local Ext_Stats = {}


--- Adds a custom property description to the specified stat entry. (The blue text in the skill description tooltip). This function can only be called from a `ModuleLoading` listener.
---         Example:
--- ```lua
--- Ext.Stats.AddCustomDescription("Dome_CircleOfProtection", "SkillProperties", "Custom desc one")
--- ```
--- Location: Lua/Libs/Stats.inl:762
--- @param statName string Stats name to fetch
--- @param attributeName string Property list to expand (SkillProperties or ExtraProperties)
--- @param description string Description text
function Ext_Stats.AddCustomDescription(statName, attributeName, description) end

--- @param speakerGuid FixedString 
--- @param translatedStringKey FixedString 
--- @param source string 
--- @param length number 
--- @param priority int32|nil 
function Ext_Stats.AddVoiceMetaData(speakerGuid, translatedStringKey, source, length, priority) end

--- Creates a new stats entry. If a stat object with the same name already exists, the specified modifier type is invalid or the specified template doesn't exist, the function returns `nil`. After all stat properties were initialized, the stats entry must be synchronized by calling `SyncStat()`.
--- 
---  - If the entry was created on the server, `SyncStat()` will replicate the stats entry to all clients.If the entry was created on the client, `SyncStat()` will only update it locally. Example:
--- ```lua
--- local stat = Ext.Stats.Create("NRD_Dynamic_Skill", "SkillData", "Rain_Water")
--- stat.RainEffect = "RS3_FX_Environment_Rain_Fire_01"
--- stat.SurfaceType = "Fire"
--- Ext.Stats.Sync("NRD_Dynamic_Skill")
--- ```
--- Location: Lua/Libs/Stats.inl:955
--- @param statName FixedString Name of stats entry to create; it should be globally unique
--- @param modifierList FixedString Stats entry type (eg. `SkillData`, `StatusData`, `Weapon`, etc.)
--- @param copyFromTemplate FixedString|nil If this parameter is not `nil`, stats properties are copied from the specified stats entry to the newly created entry
--- @param byRef boolean|nil Specifies whether the created object should use by-value or by-ref properties (default: by-value)
function Ext_Stats.Create(statName, modifierList, copyFromTemplate, byRef) end

--- @param enumName FixedString 
--- @param index int32 
function Ext_Stats.EnumIndexToLabel(enumName, index) end

--- @param enumName FixedString 
--- @param label FixedString 
function Ext_Stats.EnumLabelToIndex(enumName, label) end

--- @return StatEntryType
--- Returns the specified stats entry as an object for easier manipulation. If the `level` argument is not specified or is `nil`, the table will contain stat values as specified in the stat entry. If the `level` argument is not `nil`, the table will contain level - scaled values for the specified level. A `level` value of `-1` will use the level specified in the stat entry.
---         The behavior of getting a table entry is identical to that of `StatGetAttribute` and setting a table entry is identical to `StatSetAttribute`.
--- The `StatSetAttribute` example rewritten using `GetStat`:
--- ```lua
--- -- Swap DamageType from Poison to Air on all skills
--- for i, name in pairs(Ext.Stats.GetEntries("SkillData")) do
---     local stat = Ext.Stats.Get(name)
---     if stat.DamageType == "Poison" then
---         stat.DamageType = "Air"
---     end
--- end
--- ```
--- Location: Lua/Libs/Stats.inl:881
--- @param statName string Stats name to fetch
--- @param level int32|nil Specify `nil` to return raw (unscaled) stat values, `-1` to return values scaled to the stats level, or a specific level value to scale returned stats to that level
--- @param warnOnError boolean|nil Log a warning in the console if the stats object could not be found?
--- @param byRef boolean|nil Specifies whether the returned object should use by-value or by-ref properties (default: by-value)
function Ext_Stats.Get(statName, level, warnOnError, byRef) end

--- Returns the specified `attribute` of the stat entry. If the stat entry does not exist, the stat entry doesn't have an attribute named `attribute`, or the attribute is not supported, the function returns `nil`.
---         ** Notes: **
---  - For enumerations, the function will return the enumeration label(eg. `Corrosive`). See `ModifierLists.txt` or `Enumerations.xml` for a list of enumerationsand enumeration labels.
---  - The following fields are not supported: `AoEConditions`, `TargetConditions`, `ForkingConditions`, `CycleConditions` `Requirements` and `MemorizationRequirements` are returned in the following format :
--- ```lua
--- [
---     {
---         "Not" : false, // Negated condition?
---         "Param" : 1, // Parameter; number for ability/attribute level, string for Tag
---         "Requirement" : "FireSpecialist" // Requirement name
---     },
---     {
---         "Not" : false,
---         "Param" : 1,
---         "Requirement" : "Necromancy"
---     }
--- ]
--- ```
--- Location: Lua/Libs/Stats.inl:678
--- @param statName string 
--- @param attributeName FixedString 
function Ext_Stats.GetAttribute(statName, attributeName) end

--- @return CharacterCreationCharacterCreationManager
function Ext_Stats.GetCharacterCreation() end

--- Returns a table with the names of all stat entries. When the optional parameter `statType` is specified, it'll only return stats with the specified type. (The type of a stat entry is specified in the stat .txt file itself (eg. `type "StatusData"`). The following types are supported: `StatusData`, `SkillData`, `Armor`, `Shield`, `Weapon`, `Potion`, `Character`, `Object`, `SkillSet`, `EquipmentSet`, `TreasureTable`, `ItemCombination`, `ItemComboProperty`, `CraftingPreviewData`, `ItemGroup`, `NameGroup`, `DeltaMod`
--- Location: Lua/Libs/Stats.inl:348
--- @param statType FixedString|nil Type of stat to fetch
--- @return FixedString[]
function Ext_Stats.GetStats(statType) end

--- Returns a table with the names of all stat entries that were loaded before the specified mod. This function is useful for retrieving stats that can be overridden by a mod according to the module load order. When the optional parameter `statType` is specified, it'll only return stats with the specified type. (The type of a stat entry is specified in the stat .txt file itself (eg. `type "StatusData"`).
--- Location: Lua/Libs/Stats.inl:401
--- @param modUuid FixedString Return stats entries declared before this module was loaded
--- @param statType FixedString|nil Type of stat to fetch
--- @return FixedString[]
function Ext_Stats.GetStatsLoadedBefore(modUuid, statType) end

--- @return StatsDamagePairList damageList
function Ext_Stats.NewDamageList() end

--- Updates the specified `attribute` of the stat entry. This essentially allows on-the-fly patching of stat .txt files from script without having to override the while stat entry. If the function is called while the module is loading (i.e. from a `ModuleLoading`/`StatsLoaded` listener) no additional calls are needed. If the function is called after module load, the stats entry must be synchronized with the client via the `Sync` call. ** Notes: **
--- 
---  - For enumerations, the function accepts both the enumeration label (a string value, eg. `Corrosive`) and the enumeration index (an integer value, eg, `7`). See `ModifierLists.txt` or `Enumerations.xml` for a list of enumerations and enumeration labels.
---  - Be aware that a lot of number-like attributes are in fact enums; eg. the `Strength`, `Finesse`, `Intelligence`, etc. attributes of `Potion` are enumerations and setting them by passing an integer value to this function will yield unexpected results. For example, calling `SetAttribute("SomePotion", "Strength", 5)` will set the `Strength` value to `-9.6`! The proper way to set these values is by passing the enumeration label as string, eg. `SetAttribute("SomePotion", "Strength", "5")`
--- Example:
--- ```lua
--- -- Swap DamageType from Poison to Air on all skills
--- for i,name in pairs(Ext.Stats.GetEntries("SkillData")) do
---     local damageType = Ext.Stats.GetAttribute(name, "DamageType")
---     if damageType == "Poison" then
---         Ext.Stats.SetAttribute(name, "DamageType", "Air")
---     end
--- end
--- ```
--- When modifying stat attributes that contain tables (i.e. `Requirements`, `TargetConditions`, `SkillProperties` etc.) it is not sufficient to just modify the table, the modified table must be reassigned to the property:
--- ```lua
--- local requirements = Ext.Stats.GetAttribute(name, "MemorizationRequirements")
--- table.insert(requirements, {Name = "Intelligence", Param = 10, Not = false})
--- Ext.Stats.SetAttribute(name, "Requirements", requirements)
--- ```
--- Stat entries that are modified on the fly (i.e. after `ModuleLoading`/`StatsLoaded`) must be synchronized via `SyncStat()`. Neglecting to do this will cause the stat entry to be different on the client and the server.
--- ```lua
--- local stat = Ext.Stats.Get(name)
--- stat.DamageType = "Air"
--- stat.Damage = 10
--- Ext.Stats.Sync(name)
--- ```
--- Location: Lua/Libs/Stats.inl:738
--- @param statName string The stat ID
--- @param attributeName FixedString Stat attribute to change
--- @param value any 
--- @return boolean
function Ext_Stats.SetAttribute(statName, attributeName, value) end

--- Replaces level scaling formula for the specified stat. This function can only be called from a `ModuleLoading` listener.
---         `func` must satisfy the following requirements :
---  - Must be a Lua function that receives two arguments `(attributeValue, level)`and returns the integer level scaled value.
---  - Must have no side effects(i.e.can't set external variables, call external functions, etc)
---  - Must always returns the same result when given the same argument values
---  - Since the function is called very frequently (up to 50, 000 calls during a level load), it should execute as quickly as possible
--- Location: Lua/Libs/Stats.inl:806
--- @param modifierListName FixedString Stat attribute to override (`Strength`, `Constitution`, ...)
--- @param modifierName FixedString 
function Ext_Stats.SetLevelScaling(modifierListName, modifierName) end

--- Toggles whether the specified stats entry should be persisted to savegames. Changes made to non - persistent stats will be lost the next time a game is reloaded. If a dynamically created stats entry is marked as non - persistent, the entry will be deleted completely after the next reload.Make sure that you don't delete entries that are still in use as it could break the game in various ways.
--- Location: Lua/Libs/Stats.inl:1028
--- @param statName FixedString Name of stats entry to update
--- @param persist boolean Is the stats entry persistent, i.e. if it will be written to savegames
function Ext_Stats.SetPersistence(statName, persist) end

--- Synchronizes the changes made to the specified stats entry to each client. `Sync` must be called each time a stats entry is modified dynamically (after `ModuleLoading`/`StatsLoaded`) to ensure that the hostand all clients see the same properties.
--- Location: Lua/Libs/Stats.inl:998
--- @param statName FixedString Name of stats entry to sync
--- @param persist boolean|nil Is the stats entry persistent, i.e. if it will be written to savegames. (default `true`)
function Ext_Stats.Sync(statName, persist) end



--- @class Ext_StatsDeltaMod
--- @field GetLegacy fun(id:FixedString):DeltaMod
--- @field Update fun(tbl:DeltaMod)
local Ext_StatsDeltaMod = {}




--- @class Ext_StatsEquipmentSet
--- @field GetLegacy fun(id:FixedString):StatEquipmentSet
--- @field Update fun(tbl:StatEquipmentSet)
local Ext_StatsEquipmentSet = {}




--- @class Ext_StatsItemColor
--- @field Get fun(id:FixedString):StatsItemColorDefinition
--- @field GetAll fun():StatsItemColorDefinition[]
--- @field Update fun(tbl:StatsItemColorDefinition)
local Ext_StatsItemColor = {}




--- @class Ext_StatsItemCombo
--- @field GetLegacy fun(id:FixedString):ItemCombo
--- @field Update fun(tbl:ItemCombo)
local Ext_StatsItemCombo = {}




--- @class Ext_StatsItemComboPreview
--- @field GetLegacy fun(id:FixedString):ItemComboPreviewData
--- @field Update fun(tbl:ItemComboPreviewData)
local Ext_StatsItemComboPreview = {}




--- @class Ext_StatsItemComboProperty
--- @field GetLegacy fun(a1:FixedString)
--- @field Update fun()
local Ext_StatsItemComboProperty = {}




--- @class Ext_StatsItemGroup
--- @field GetLegacy fun(id:FixedString):ItemGroup
local Ext_StatsItemGroup = {}




--- @class ItemSet
--- @field TranslatedName TranslatedString
--- @field TranslatedSetEffect TranslatedString
--- @field Name FixedString
--- @field Items FixedString[]
--- @field GrantedSkills FixedString[]
--- @field GrantedStatuses FixedString[]
--- @field ItemsUpgradeLevel integer


--- @class Ext_StatsItemSet
--- @field GetLegacy fun(id:FixedString):ItemSet
--- @field Update fun(tbl:ItemSet)
local Ext_StatsItemSet = {}




--- @class Ext_StatsMath
--- @field GetDamageBoostByType fun(a1:CDivinityStatsCharacter, a2:StatsDamageType):number
--- @field GetResistance fun(a1:CDivinityStatsCharacter, a2:StatsDamageType, a3:boolean|nil):int32
local Ext_StatsMath = {}




--- @class Ext_StatsNameGroup
--- @field GetLegacy fun(id:FixedString):ItemNameGroup
local Ext_StatsNameGroup = {}




--- @class Ext_StatsSkillSet
--- @field GetLegacy fun(id:FixedString):StatSkillSet
--- @field Update fun(tbl:StatSkillSet)
local Ext_StatsSkillSet = {}




--- @class Ext_StatsTreasureCategory
--- @field GetLegacy fun(id:FixedString):StatTreasureCategory
--- @field Update fun(id:FixedString, tbl:StatTreasureCategory)
local Ext_StatsTreasureCategory = {}




--- @class Ext_StatsTreasureTable
--- @field Get fun(a1:FixedString):StatsTreasureTable
--- @field GetLegacy fun(id:FixedString):StatTreasureTable
--- @field Update fun(tbl:StatTreasureTable)
local Ext_StatsTreasureTable = {}




--- @class Ext_Surface
local Ext_Surface = {}

--- @field Action Ext_ServerSurfaceAction


--- @param type SurfaceType 
--- @return SurfaceTemplate
function Ext_Surface.GetTemplate(type) end

function Ext_Surface.GetTransformRules() end

function Ext_Surface.UpdateTransformRules() end



--- @class Ext_Types
local Ext_Types = {}

--- Generate an ExtIdeHelpers file  
--- @param outputPath string|nil Optional path to save the generated helper file, relative to the `Documents\Larian Studios\Divinity Original Sin 2 Definitive Edition\Osiris Data` folder  
--- @param addOsi boolean|nil If true, all Osiris functions will be included in the Osi global table. This is optional, due to the possible performance cost of having so many functions  
--- @return string fileContents Returns the file contents, for use with Ext.IO.SaveFile
function Ext_Types.GenerateIdeHelpers(outputPath, addOsi) end



--- @return FixedString[]
function Ext_Types.GetAllTypes() end

--- Get the type name for any userdata, lightuserdata, or cppobject.
--- @param object any Any userdata, lightuserdata, or cppobject type.
--- @return FixedString|"userdata"|"lightuserdata" objectType
function Ext_Types.GetObjectType(object) end

--- @param typeName FixedString 
--- @return TypeInformation
function Ext_Types.GetTypeInfo(typeName) end



--- @class ComponentHandleProxy:userdata
--- @field Get (fun():IEoCServerObject|IEoCClientObject)
--- @field TypeId integer
--- @field Type FixedString
--- @field Salt uint32
--- @field Index uint32


--- @class Ext_Utils
--- @field LoadString fun(a1:string)
--- @field RegisterUserVariable fun(a1:FixedString)
--- @field Version fun():int32
local Ext_Utils = {}


--- @return string|nil
function Ext_Utils.GameVersion() end

--- @return GameDifficultyValue difficulty
function Ext_Utils.GetDifficulty() end

--- @return "Campaign"|"Arena"|"GameMaster"
function Ext_Utils.GetGameMode() end

--- @return GlobalSwitches
function Ext_Utils.GetGlobalSwitches() end

--- @return GraphicSettings
function Ext_Utils.GetGraphicSettings() end

--- @param handle ComponentHandle 
--- @return FixedString|nil
function Ext_Utils.GetHandleType(handle) end

--- @param value any 
--- @return string|"nil"|"number"|"string"|"boolean"|"table"|"function"|"thread"|"userdata"|"lightuserdata" valueType # Returns a base type, lightuserdata, or an extender type.
function Ext_Utils.GetValueType(value) end

--- Converts a handle to an integer value for serialization purposes.
--- Location: Lua/Libs/Utils.inl:184
--- @param handle ComponentHandle Handle to convert
--- @return int64
function Ext_Utils.HandleToInteger(handle) end

--- @param modGUID string|nil The ModuleUUID value
--- @param path string The path to the script, relative to the Lua folder
--- @param replaceGlobals table|nil If set, the global environment of the script is replaced with this table
--- @return any returnedValue Whatever the script returned, if anything
function Ext_Utils.Include(modGUID, path, replaceGlobals) end

--- Converts an integer value to a handle for serialization purposes.
--- Location: Lua/Libs/Utils.inl:193
--- @param i int64 Integer value to convert
--- @return ComponentHandle
function Ext_Utils.IntegerToHandle(i) end

--- @param handle ComponentHandle 
--- @return boolean
function Ext_Utils.IsValidHandle(handle) end

--- @param handle ComponentHandle 
--- @return ComponentHandleProxy handleObject
function Ext_Utils.MakeHandleObject(handle) end

--- @return int64
function Ext_Utils.MonotonicTime() end

--- @vararg any
function Ext_Utils.Print(...) end

--- @vararg any
function Ext_Utils.PrintError(...) end

--- @vararg any
function Ext_Utils.PrintWarning(...) end

--- @overload fun(num:integer|nil):int64
--- @param low integer 
--- @param up integer 
--- @return int64 result
function Ext_Utils.Random(low, up) end

--- @param val number 
--- @return int64
function Ext_Utils.Round(val) end

--- @param message string 
function Ext_Utils.ShowErrorAndExitGame(message) end



--- @class ExtClient
--- @field Audio Ext_ClientAudio
--- @field ClientAudio Ext_ClientAudio
--- @field Behavior Ext_ClientBehavior
--- @field ClientBehavior Ext_ClientBehavior
--- @field Client Ext_ClientClient
--- @field ClientClient Ext_ClientClient
--- @field Entity Ext_ClientEntity
--- @field ClientEntity Ext_ClientEntity
--- @field Input Ext_ClientInput
--- @field ClientInput Ext_ClientInput
--- @field Net Ext_ClientNet
--- @field ClientNet Ext_ClientNet
--- @field Template Ext_ClientTemplate
--- @field ClientTemplate Ext_ClientTemplate
--- @field UI Ext_ClientUI
--- @field ClientUI Ext_ClientUI
--- @field Visual Ext_ClientVisual
--- @field ClientVisual Ext_ClientVisual
--- @field Debug Ext_Debug
--- @field IO Ext_IO
--- @field Json Ext_Json
--- @field L10N Ext_L10N
--- @field Math Ext_Math
--- @field Mod Ext_Mod
--- @field Resource Ext_Resource
--- @field Stats Ext_Stats
--- @field Stats.Math Ext_StatsMath
--- @field Surface Ext_Surface
--- @field Types Ext_Types
--- @field Utils Ext_Utils

--- @class ExtServer
--- @field Debug Ext_Debug
--- @field IO Ext_IO
--- @field Json Ext_Json
--- @field L10N Ext_L10N
--- @field Math Ext_Math
--- @field Mod Ext_Mod
--- @field Resource Ext_Resource
--- @field Action Ext_ServerAction
--- @field ServerAction Ext_ServerAction
--- @field Ai Ext_ServerAi
--- @field ServerAi Ext_ServerAi
--- @field Combat Ext_ServerCombat
--- @field ServerCombat Ext_ServerCombat
--- @field CustomStat Ext_ServerCustomStat
--- @field ServerCustomStat Ext_ServerCustomStat
--- @field Effect Ext_ServerEffect
--- @field ServerEffect Ext_ServerEffect
--- @field Entity Ext_ServerEntity
--- @field ServerEntity Ext_ServerEntity
--- @field Net Ext_ServerNet
--- @field ServerNet Ext_ServerNet
--- @field Osiris Ext_ServerOsiris
--- @field ServerOsiris Ext_ServerOsiris
--- @field PropertyList Ext_ServerPropertyList
--- @field ServerPropertyList Ext_ServerPropertyList
--- @field Server Ext_ServerServer
--- @field ServerServer Ext_ServerServer
--- @field Surface.Action Ext_ServerSurfaceAction
--- @field ServerSurface.Action Ext_ServerSurfaceAction
--- @field Template Ext_ServerTemplate
--- @field ServerTemplate Ext_ServerTemplate
--- @field Stats Ext_Stats
--- @field Stats.Math Ext_StatsMath
--- @field Surface Ext_Surface
--- @field Types Ext_Types
--- @field Utils Ext_Utils

--- @class Ext
--- @field Audio Ext_ClientAudio
--- @field ClientAudio Ext_ClientAudio
--- @field Behavior Ext_ClientBehavior
--- @field ClientBehavior Ext_ClientBehavior
--- @field Client Ext_ClientClient
--- @field ClientClient Ext_ClientClient
--- @field Entity Ext_ClientEntity|Ext_ServerEntity
--- @field ClientEntity Ext_ClientEntity
--- @field Input Ext_ClientInput
--- @field ClientInput Ext_ClientInput
--- @field Net Ext_ClientNet|Ext_ServerNet
--- @field ClientNet Ext_ClientNet
--- @field Template Ext_ClientTemplate|Ext_ServerTemplate
--- @field ClientTemplate Ext_ClientTemplate
--- @field UI Ext_ClientUI
--- @field ClientUI Ext_ClientUI
--- @field Visual Ext_ClientVisual
--- @field ClientVisual Ext_ClientVisual
--- @field Debug Ext_Debug
--- @field IO Ext_IO
--- @field Json Ext_Json
--- @field L10N Ext_L10N
--- @field Math Ext_Math
--- @field Mod Ext_Mod
--- @field Resource Ext_Resource
--- @field Action Ext_ServerAction
--- @field ServerAction Ext_ServerAction
--- @field Ai Ext_ServerAi
--- @field ServerAi Ext_ServerAi
--- @field Combat Ext_ServerCombat
--- @field ServerCombat Ext_ServerCombat
--- @field CustomStat Ext_ServerCustomStat
--- @field ServerCustomStat Ext_ServerCustomStat
--- @field Effect Ext_ServerEffect
--- @field ServerEffect Ext_ServerEffect
--- @field ServerEntity Ext_ServerEntity
--- @field ServerNet Ext_ServerNet
--- @field Osiris Ext_ServerOsiris
--- @field ServerOsiris Ext_ServerOsiris
--- @field PropertyList Ext_ServerPropertyList
--- @field ServerPropertyList Ext_ServerPropertyList
--- @field Server Ext_ServerServer
--- @field ServerServer Ext_ServerServer
--- @field Surface.Action Ext_ServerSurfaceAction
--- @field ServerSurface.Action Ext_ServerSurfaceAction
--- @field ServerTemplate Ext_ServerTemplate
--- @field Stats Ext_Stats
--- @field Stats.Math Ext_StatsMath
--- @field Surface Ext_Surface
--- @field Types Ext_Types
--- @field Utils Ext_Utils
Ext = {Events = {}}



--- @class SubscribableEvent<T>:{ (Subscribe:fun(self:SubscribableEvent, callback:fun(e:T|LuaEventBase), opts:{Priority:integer, Once:boolean}|nil):integer), (Unsubscribe:fun(self:SubscribableEvent, index:integer)), (Throw:fun(self:SubscribableEvent, e:T|LuaEventBase))}

--- Developer functions for the SubscribableEvent type. 
--- Throw can be used to manually throw the event, but special care may be needed to ensure the table used for the event data is valid.  
--- @class SubscribableEventDev<T>:{ (Throw:fun(self:SubscribableEvent, e:T|LuaEventBase))}


--#region Extender Events

--- 🔨**Server-Only**🔨  
--- @type SubscribableEvent<EsvLuaAfterCraftingExecuteCombinationEvent>  
Ext.Events.AfterCraftingExecuteCombination = {}

--- 🔧**Client-Only**🔧  
--- @type SubscribableEvent<EclLuaUICallEvent>  
Ext.Events.AfterUICall = {}

--- 🔧**Client-Only**🔧  
--- @type SubscribableEvent<EclLuaUICallEvent>  
Ext.Events.AfterUIInvoke = {}

--- 🔨**Server-Only**🔨  
--- @type SubscribableEvent<EsvLuaBeforeCharacterApplyDamageEvent>  
Ext.Events.BeforeCharacterApplyDamage = {}

--- 🔨**Server-Only**🔨  
--- @type SubscribableEvent<EsvLuaBeforeCraftingExecuteCombinationEvent>  
Ext.Events.BeforeCraftingExecuteCombination = {}

--- 🔨**Server-Only**🔨  
--- @type SubscribableEvent<EsvLuaBeforeShootProjectileEvent>  
Ext.Events.BeforeShootProjectile = {}

--- 🔨**Server-Only**🔨  
--- @type SubscribableEvent<EsvLuaBeforeStatusApplyEvent>  
Ext.Events.BeforeStatusApply = {}

--- 🔨**Server-Only**🔨  
--- @type SubscribableEvent<EsvLuaStatusDeleteEvent>  
Ext.Events.BeforeStatusDelete = {}

--- 🔨**Server-Only**🔨  
--- @type SubscribableEvent<EsvLuaCalculateTurnOrderEvent>  
Ext.Events.CalculateTurnOrder = {}

--- 🔨**Server-Only**🔨  
--- @type SubscribableEvent<EsvLuaComputeCharacterHitEvent>  
Ext.Events.ComputeCharacterHit = {}

--- 🔧**Client-Only**🔧  
--- @type SubscribableEvent<EclLuaCreateEquipmentVisualsRequestEvent>  
Ext.Events.CreateEquipmentVisualsRequest = {}

--- 🔨🔧**Server/Client**🔧🔨  
--- @type SubscribableEvent<LuaDoConsoleCommandEvent>  
Ext.Events.DoConsoleCommand = {}

--- 🔨🔧**Server/Client**🔧🔨  
--- @type SubscribableEvent<EclLuaGameStateChangedEvent|EsvLuaGameStateChangedEvent>  
Ext.Events.GameStateChanged = {}

--- 🔨🔧**Server/Client**🔧🔨  
--- @type SubscribableEvent<LuaGetAttackAPCostEvent>  
Ext.Events.GetAttackAPCost = {}

--- 🔨🔧**Server/Client**🔧🔨  
--- @type SubscribableEvent<LuaGetHitChanceEvent>  
Ext.Events.GetHitChance = {}

--- 🔨🔧**Server/Client**🔧🔨  
--- @type SubscribableEvent<LuaGetSkillAPCostEvent>  
Ext.Events.GetSkillAPCost = {}

--- 🔨🔧**Server/Client**🔧🔨  
--- @type SubscribableEvent<LuaGetSkillDamageEvent>  
Ext.Events.GetSkillDamage = {}

--- 🔨**Server-Only**🔨  
--- @type SubscribableEvent<EsvLuaGroundHitEvent>  
Ext.Events.GroundHit = {}

--- 🔧**Client-Only**🔧  
--- @type SubscribableEvent<EclLuaInputEvent>  
Ext.Events.InputEvent = {}

--- 🔨🔧**Server/Client**🔧🔨  
--- @type SubscribableEvent<LuaEmptyEvent>  
Ext.Events.ModuleLoadStarted = {}

--- 🔨🔧**Server/Client**🔧🔨  
--- @type SubscribableEvent<LuaEmptyEvent>  
Ext.Events.ModuleLoading = {}

--- 🔨🔧**Server/Client**🔧🔨  
--- @type SubscribableEvent<LuaEmptyEvent>  
Ext.Events.ModuleResume = {}

--- 🔨🔧**Server/Client**🔧🔨  
--- @type SubscribableEvent<LuaNetMessageEvent>  
Ext.Events.NetMessageReceived = {}

--- 🔨**Server-Only**🔨  
--- @type SubscribableEvent<EsvLuaAiRequestSortEvent>  
Ext.Events.OnAfterSortAiActions = {}

--- 🔨**Server-Only**🔨  
--- @type SubscribableEvent<EsvLuaAiRequestSortEvent>  
Ext.Events.OnBeforeSortAiActions = {}

--- 🔨**Server-Only**🔨  
--- @type SubscribableEvent<EsvLuaOnExecutePropertyDataOnPositionEvent>  
Ext.Events.OnExecutePropertyDataOnPosition = {}

--- 🔨**Server-Only**🔨  
--- @type SubscribableEvent<EsvLuaOnExecutePropertyDataOnTargetEvent>  
Ext.Events.OnExecutePropertyDataOnTarget = {}

--- 🔨**Server-Only**🔨  
--- @type SubscribableEvent<EsvLuaOnPeekAiActionEvent>  
Ext.Events.OnPeekAiAction = {}

--- 🔨**Server-Only**🔨  
--- @type SubscribableEvent<EsvLuaProjectileHitEvent>  
Ext.Events.ProjectileHit = {}

--- 🔧**Client-Only**🔧  
--- @type SubscribableEvent<EclLuaRawInputEvent>  
Ext.Events.RawInput = {}

--- 🔨🔧**Server/Client**🔧🔨  
--- @type SubscribableEvent<LuaEmptyEvent>  
Ext.Events.ResetCompleted = {}

--- 🔨🔧**Server/Client**🔧🔨  
--- @type SubscribableEvent<LuaEmptyEvent>  
Ext.Events.SessionLoaded = {}

--- 🔨🔧**Server/Client**🔧🔨  
--- @type SubscribableEvent<LuaEmptyEvent>  
Ext.Events.SessionLoading = {}

--- 🔨**Server-Only**🔨  
--- @type SubscribableEvent<EsvLuaShootProjectileEvent>  
Ext.Events.ShootProjectile = {}

--- 🔧**Client-Only**🔧  
--- @type SubscribableEvent<EclLuaSkillGetDescriptionParamEvent>  
Ext.Events.SkillGetDescriptionParam = {}

--- 🔧**Client-Only**🔧  
--- @type SubscribableEvent<EclLuaSkillGetPropertyDescriptionEvent>  
Ext.Events.SkillGetPropertyDescription = {}

--- 🔨🔧**Server/Client**🔧🔨  
--- @type SubscribableEvent<LuaEmptyEvent>  
Ext.Events.StatsLoaded = {}

--- 🔨🔧**Server/Client**🔧🔨  
--- @type SubscribableEvent<LuaEmptyEvent>  
Ext.Events.StatsStructureLoaded = {}

--- 🔨**Server-Only**🔨  
--- @type SubscribableEvent<EsvLuaStatusDeleteEvent>  
Ext.Events.StatusDelete = {}

--- 🔧**Client-Only**🔧  
--- @type SubscribableEvent<EclLuaStatusGetDescriptionParamEvent>  
Ext.Events.StatusGetDescriptionParam = {}

--- 🔨**Server-Only**🔨  
--- @type SubscribableEvent<EsvLuaStatusGetEnterChanceEvent>  
Ext.Events.StatusGetEnterChance = {}

--- 🔨**Server-Only**🔨  
--- @type SubscribableEvent<EsvLuaStatusHitEnterEvent>  
Ext.Events.StatusHitEnter = {}

--- 🔨🔧**Server/Client**🔧🔨  
--- @type SubscribableEvent<LuaTickEvent>  
Ext.Events.Tick = {}

--- 🔨**Server-Only**🔨  
--- @type SubscribableEvent<EsvLuaTreasureItemGeneratedEvent>  
Ext.Events.TreasureItemGenerated = {}

--- 🔧**Client-Only**🔧  
--- @type SubscribableEvent<EclLuaUICallEvent>  
Ext.Events.UICall = {}

--- 🔧**Client-Only**🔧  
--- @type SubscribableEvent<EclLuaUICallEvent>  
Ext.Events.UIInvoke = {}

--- 🔧**Client-Only**🔧  
--- @type SubscribableEvent<EclLuaUIObjectCreatedEvent>  
Ext.Events.UIObjectCreated = {}

--#endregion



--#region Extender Functions / Globals

--- Loads the specified Lua file
--- @param fileName string|nil Path of Lua file, relative to Mods/<Mod>/Story/RawFiles/Lua
--- @see Ext_Utils#Include
--- @return any
function Ext.Require(fileName) end

--- Returns whether the code is executing in a client context
--- @return boolean
function Ext.IsClient() end

--- Returns whether the code is executing in a server context
--- @return boolean
function Ext.IsServer() end

--- Console window shortcut for Ext.Dump
_D = Ext.Dump

--- Console window shortcut for Ext.Utils.Print
_P = Ext.Utils.Print

--- Console window helper to get current player character 
--- This is the host on the server, or the hotbar character on the client  
--- @return EsvCharacter|EclCharacter
_C = function() end

--- Console window helper to get character being examined on the client-side  
--- @return EclCharacter
_E = function() end

--- Console window helper to get the host's equipped weapon on the server-side  
--- @return EsvItem
_W = function() end

--- Helper for dumping variables to the console  
--- This is essentially `Ext.Utils.Print(Ext.DumpExport(val))`  
--- @param val any
function Ext.Dump(val) end

--- Helper for dumping variables to a string
--- @param val any
--- @return string
function Ext.DumpExport(val) end

--- Register a callback that runs on the next tick, and is then unregistered afterwards  
--- @param callback fun(e:LuaTickEventParams)
function Ext.OnNextTick(callback) end

--- @class CustomSkillProperty
--- @field GetDescription fun(property:StatsPropertyExtender):string|nil
--- @field ExecuteOnPosition fun(property:StatsPropertyExtender, attacker: EsvCharacter|EsvItem, position: vec3, areaRadius: number, isFromItem: boolean, skill: StatsSkillPrototype|nil, hit: StatsHitDamageInfo|nil)
--- @field ExecuteOnTarget fun(property:StatsPropertyExtender, attacker: EsvCharacter|EsvItem, target: EsvCharacter|EsvItem, position: vec3, isFromItem: boolean, skill: StatsSkillPrototype|nil, hit: StatsHitDamageInfo|nil)

--- Registers a new skill property that can be triggered via SkillProperties
--- Stat syntax: data"SkillProperties""EXT:<PROPERTY_NAME>[,<int>,<int>,<string>,<int>,<int>]"
--- The property name must always be preceded by the string "EXT:". 
--- Target contexts (SELF:, TARGET:, ...) and useing multiple actions in the same SkillProperties are supported
--- Conditions for EXT: properties (i.e. "IF(COND):") are _NOT YET_ supported
--- @param name string Skill property name
--- @param defn CustomSkillProperty Event handlers for the skill property
function Ext.RegisterSkillProperty(name, defn) end

--- @alias UICallbackHandler fun(ui:UIObject, event:string, ...:string|boolean|number)
--- @alias UICallbackEventType "Before"|"After"

--- Registers a listener that is called when the specified function is called from Flash
--- @param object UIObject UI object returned from Ext.CreateUI, Ext.GetUI or Ext.GetBuiltinUI
--- @param name string ExternalInterface function name
--- @param handler UICallbackHandler Lua handler
--- @param type UICallbackEventType|nil Event type - 'Before' or 'After'
function Ext.RegisterUICall(object, name, handler, type) end

--- Registers a listener that is called when the specified function is called from Flash
--- The event is triggered for every UI element with the specified type ID
--- @param typeId number Engine UI element type ID
--- @param name string ExternalInterface function name
--- @param handler UICallbackHandler Lua handler
--- @param type UICallbackEventType|nil Event type - 'Before' or 'After'
function Ext.RegisterUITypeCall(typeId, name, handler, type) end

--- Registers a listener that is called when the specified function is called from Flash
--- The event is triggered regardless of which UI element it was called on
--- (Function call capture must be enabled for every element type that needs to monitored!)
--- @param name string ExternalInterface function name
--- @param handler UICallbackHandler Lua handler
--- @param type UICallbackEventType|nil Event type - 'Before' or 'After'
function Ext.RegisterUINameCall(name, handler, type) end

--- Registers a listener that is called when the specified method is called on the main timeline of the Flash object
--- @param object UIObject UI object returned from Ext.CreateUI, Ext.GetUI or Ext.GetBuiltinUI
--- @param name string Flash method name
--- @param handler UICallbackHandler Lua handler
--- @param type UICallbackEventType|nil Event type - 'Before' or 'After'
function Ext.RegisterUIInvokeListener(object, name, handler, type) end

--- Registers a listener that is called when the specified method is called on the main timeline of the Flash object
--- The event is triggered for every UI element with the specified type ID
--- @param typeId number Engine UI element type ID
--- @param name string Flash method name
--- @param handler UICallbackHandler Lua handler
--- @param type UICallbackEventType|nil Event type - 'Before' or 'After'
function Ext.RegisterUITypeInvokeListener(typeId, name, handler, type) end

--- Registers a listener that is called when the specified method is called on the main timeline of the Flash object
--- The event is triggered regardless of which UI element it was called on
--- @param name string Flash method name
--- @param handler UICallbackHandler Lua handler
--- @param type UICallbackEventType|nil Event type - 'Before' or 'After'
function Ext.RegisterUINameInvokeListener(name, handler, type) end

--- Registers a listener that is called when a console command is entered in the dev console
--- @param cmd string Console command
--- @param handler fun(cmd:string, ...:string)
function Ext.RegisterConsoleCommand(cmd, handler) end

--- Prepares a new surface action for execution
--- @param type string Surface action type
--- @return EsvSurfaceAction
function Ext.CreateSurfaceAction(type) end

--- Executes a surface action
--- @param action EsvSurfaceAction Action to execute
function Ext.ExecuteSurfaceAction(action) end

--- @deprecated
--- Cancels a surface action
--- @param actionHandle integer Action to cancel
function Ext.CancelSurfaceAction(actionHandle) end

--- @class ItemDefinition
--- @field RootTemplate FixedString
--- @field OriginalRootTemplate FixedString
--- @field Slot integer
--- @field Amount integer
--- @field GoldValueOverwrite integer
--- @field WeightValueOverwrite integer
--- @field DamageTypeOverwrite StatsDamageType
--- @field ItemType ItemDataRarity
--- @field CustomDisplayName string
--- @field CustomDescription string
--- @field CustomBookContent string
--- @field GenerationStatsId FixedString
--- @field GenerationItemType ItemDataRarity
--- @field GenerationRandom integer
--- @field GenerationLevel integer
--- @field StatsLevel integer
--- @field Key string
--- @field LockLevel integer
--- @field EquipmentStatsType integer
--- @field HasModifiedSkills boolean
--- @field Skills string
--- @field HasGeneratedStats boolean
--- @field IsIdentified boolean
--- @field GMFolding boolean
--- @field CanUseRemotely boolean
--- @field GenerationBoosts FixedString[]
--- @field RuneBoosts FixedString[]
--- @field DeltaMods FixedString[]
local ItemDefinition = {}

--- Clears item progression data (name group, level group, etc.)
function ItemDefinition:ResetProgression() end

--- @alias ItemConstructorDefinitionAccessor ItemDefinition[]

--- Creates an item based on a parsed item or newly created item definition.
--- Should be initialized using Ext.CreateItemConstructor(item/template) first.
--- Item definitions can be accessed using c[1], c[2], etc.
--- For non-recursive item cloning (i.e. creating a single item), there is only one item (c[1]).
--- For container cloning, the contained items are accessible using c[2], c[3], etc.
--- @class ItemConstructorBase
local ItemConstructorBase = {}

--- Constructs an instance of the item contained in the constructor definition.
--- The definition is cleared after the item is created.
--- @return EsvItem|nil
function ItemConstructorBase:Construct() end

--- @alias ItemConstructor ItemConstructorBase|ItemConstructorDefinitionAccessor

--- Starts creating a new item using template UUID or cloning an existing item
--- @param from EsvItem|string Template UUID or item to clone
--- @param recursive boolean|nil Copy items in container? (cloning only)
--- @return ItemConstructor
function Ext.CreateItemConstructor(from, recursive) end

--- Begin applying a status on the specified character or item
--- @param target string|ComponentHandle Target character/item
--- @param statusId string Status ID to apply
--- @param lifeTime number Status lifetime (-1 = infinite, -2 = keep alive)
--- @return EsvStatus|nil
function Ext.PrepareStatus(target, statusId, lifeTime) end

--- Finish applying a status on the specified character or item
--- @param status EsvStatus Status to apply
function Ext.ApplyStatus(status) end

--- Returns the current client/server game state machine state
--- @return EsvGameState
function Ext.GetGameState() end

--#endregion

--#region Deprecated Functions (moved to Ext modules)

--- @alias ExtEngineEvent string|"SessionLoading"|"SessionLoaded"|"ModuleLoading"|"ModuleLoadStarted"|"ModuleResume"|"StatsLoaded"|"GameStateChanged"|"SkillGetDescriptionParam"|"StatusGetDescriptionParam"|"GetSkillDamage"|"GetSkillAPCost"|"ComputeCharacterHit"|"CalculateTurnOrder"|"GetHitChance"|"StatusGetEnterChance"|"StatusHitEnter"|"BeforeCharacterApplyDamage"|"UIInvoke"|"UICall"|"AfterUIInvoke"|"AfterUICall"|"BeforeShootProjectile"|"ShootProjectile"|"ProjectileHit"|"GroundHit"|"InputEvent"|"RawInput"|"TreasureItemGenerated"

--- @alias StatType string|"SkillData"|"Potion"|"StatusData"|"Weapon"|"Armor"|"Shield"|"DeltaMod"|"Object"|"Character"
--- @alias StatEntryType StatEntryArmor|StatEntryCharacter|StatEntryObject|StatEntryPotion|StatEntryShield|StatEntrySkillData|StatEntryStatusData|StatEntryWeapon|StatEntryCrime

--- @deprecated
--- Returns the version number of the Osiris Extender
--- @return integer
function Ext.Version() end

--- @deprecated
--- Returns the version number of the game
--- @return string
function Ext.GameVersion() end

--- @deprecated
--- Registers a function to call when an extender event is thrown
--- @param event ExtEngineEvent Event to listen for
--- @param callback function Lua function to run when the event fires
function Ext.RegisterListener(event, callback) end

--- @alias OsirisEventType string|"before"|"after"|"beforeDelete"|"afterDelete"

--- @deprecated
--- Registers a function that is called when certain Osiris functions are called
--- Supports events, built-in queries, DBs, PROCs, QRYs (user queries)
--- @param name string Osiris function/database name
--- @param arity number Number of columns for DBs or the number of parameters (both IN and OUT) for functions
--- @param event OsirisEventType Event type ('before' - triggered before Osiris call; 'after' - after Osiris call; 'beforeDelete'/'afterDelete' - before/after delete from DB)
--- @param handler function Lua function to run when the event fires
function Ext.RegisterOsirisListener(name, arity, event, handler) end

--- @deprecated
--- Registers a new call in Osiris
--- @param func function Function to register
--- @param funcName string Name of call to register
--- @param arguments string Call argument list
function Ext.NewCall(func, funcName, arguments) end

--- @deprecated
--- Registers a new query in Osiris
--- @param func function Function to register
--- @param funcName string Name of query to register
--- @param arguments string Query argument list
function Ext.NewQuery(func, funcName, arguments) end

--- @deprecated
--- Registers a new event in Osiris
--- @param funcName string Name of event to register
--- @param arguments string Event argument list
function Ext.NewEvent(funcName, arguments) end

--- @deprecated
--- Print to console window and editor messages pane
--- @vararg any
function Ext.Print(...) end

--- @deprecated
--- Print warning to console window and editor messages pane
--- @vararg any
function Ext.PrintWarning(...) end

--- @deprecated
--- Print error to console window and editor messages pane
--- @vararg any
function Ext.PrintError(...) end

--- @deprecated
--- Parse a JSON document into a Lua object
--- @param json string JSON string to parse
--- @return any
function Ext.JsonParse(json) end

--- @deprecated
--- Converts a Lua value into a JSON document
--- @param val any Value to serialize
--- @return string JSON document
function Ext.JsonStringify(val) end

--- @deprecated
--- Returns whether the specified mod is loaded
--- @param modGuid string UUID of the module
--- @return boolean
function Ext.IsModLoaded(modGuid) end

--- @deprecated
--- Returns the list of loaded modules in load order
--- @return string[]
function Ext.GetModLoadOrder() end

--- @deprecated
--- Returns detailed information about the specified (loaded) module
--- @param modGuid string UUID of the module
--- @return LegacyModuleInfo
function Ext.GetModInfo(modGuid) end

--- @deprecated
--- Returns the list of loaded stat entries
--- @param type string|nil Type of stat entry to fetch (StatusData, SkillData, ...)
--- @return string[]
function Ext.GetStatEntries(type) end

--- @deprecated
--- Returns the list of stat entries that were loaded before the specified mod
--- @param modId string Mod UUID to check
--- @param type string|nil Type of stat entry to fetch (StatusData, SkillData, ...)
--- @return string[]
function Ext.GetStatEntriesLoadedBefore(modId, type) end

--- @deprecated
--- Returns an attribute of the specified stat entry
--- @param stat string Stat entry name
--- @param attribute string Stat attribute name
--- @return any
function Ext.StatGetAttribute(stat, attribute) end

--- @deprecated
--- Updates an attribute of the specified stat entry
--- @param stat string Stat entry name
--- @param attribute string Stat attribute name
--- @param value any New stat value
function Ext.StatSetAttribute(stat, attribute, value) end

--- @deprecated
--- Adds a property description to the specified stat entry
--- @param stat string Stat entry name
--- @param attribute string Property list attribute name
--- @param description any Description to add
function Ext.StatAddCustomDescription(stat, attribute, description) end

--- @deprecated
--- Returns all skills from the specified skill set
--- @param name string Name of skill set entry
--- @return StatSkillSet|nil
function Ext.GetSkillSet(name) end

--- @deprecated
--- Updates all properties of the specified skill set
--- The function expects a table in the same format as the one returned by GetSkillSet
--- @param skillSet StatSkillSet
function Ext.UpdateSkillSet(skillSet) end

--- @deprecated
--- Returns all equipment from the specified equipment set
--- @param name string Name of equipment set entry
--- @return StatEquipmentSet|nil
function Ext.GetEquipmentSet(name) end

--- @deprecated
--- Updates all properties of the specified equipment set
--- The function expects a table in the same format as the one returned by GetEquipmentSet
--- @param equipmentSet StatEquipmentSet
function Ext.UpdateEquipmentSet(equipmentSet) end

--- @deprecated
--- Returns the specified DeltaMod or nil on failure
--- @param name string Name of delta mod
--- @param modifierType string Modifier type (Armor/Weapon)
--- @return DeltaMod
function Ext.GetDeltaMod(name, modifierType) end

--- @deprecated
--- Updates all properties of the specified DeltaMod
--- The function expects a table in the same format as the one returned by GetDeltaMod
--- @param deltaMod DeltaMod Name of delta mod
function Ext.UpdateDeltaMod(deltaMod) end

--- @deprecated
--- Returns the specified crafting item combination or nil on failure
--- @param name string Name of item combo
--- @return ItemCombo|nil
function Ext.GetItemCombo(name) end

--- @deprecated
--- Updates all properties of the specified item combination
--- The function expects a table in the same format as the one returned by GetItemCombo
--- @param itemCombo ItemCombo
function Ext.UpdateItemCombo(itemCombo) end

--- @deprecated
--- Returns the specified crafting preview data or nil on failure
--- @param name string Name of item combo preview data
--- @return ItemComboPreviewData|nil
function Ext.GetItemComboPreviewData(name) end

--- Updates all properties of the specified item combo preview
--- The function expects a table in the same format as the one returned by GetItemComboPreviewData
--- @param previewData ItemComboPreviewData
function Ext.UpdateItemComboPreviewData(previewData) end

--- @deprecated
--- Returns the specified crafting property or nil on failure
--- @param name string Name of item combo property
--- @return ItemComboProperty|nil
function Ext.GetItemComboProperty(name) end

--- @deprecated
--- Updates all properties of the specified item combo property
--- The function expects a table in the same format as the one returned by GetItemComboProperty
--- @param itemComboProperty ItemComboProperty
function Ext.UpdateItemComboProperty(itemComboProperty) end

--- @deprecated
--- Returns the specified treasure table or nil on failure
--- @param name string Name of treasure table
--- @return StatTreasureTable|nil
function Ext.GetTreasureTable(name) end

--- @deprecated
--- Updates all properties of the specified treasure table
--- The function expects a table in the same format as the one returned by GetTreasureTable
--- @param treasureTable StatTreasureTable
function Ext.UpdateTreasureTable(treasureTable) end

--- @deprecated
--- Returns the specified treasure category or nil on failure
--- @param name string Name of treasure category
--- @return StatTreasureCategory|nil
function Ext.GetTreasureCategory(name) end

--- @deprecated
--- Updates all properties of the specified treasure category
--- The function expects a table in the same format as the one returned by GetTreasureCategory
--- @param name string Name of treasure category
--- @param treasureCategory StatTreasureCategory
function Ext.UpdateTreasureCategory(name, treasureCategory) end

--- @deprecated
--- Returns the specified item progression item group or nil on failure
--- @param name string Name of item group
--- @return ItemGroup|nil
function Ext.GetItemGroup(name) end

--- @deprecated
--- Returns the specified item progression name group or nil on failure
--- @param name string Name of name group
--- @return ItemNameGroup|nil
function Ext.GetNameGroup(name) end

--- @deprecated
--- Replaces level scaling formula for the specified stat
--- @param statType string Stat entry type
--- @param attribute string Stat attribute name
--- @param func function Replacement scaling function
function Ext.StatSetLevelScaling(statType, attribute, func) end

--- @deprecated
--- Returns the property proxy of the specified stats entry
--- Returns level scaled values if the level parameter is not nil
--- @param stat string Stat entry name
--- @param level integer|nil Level scaling level
--- @return StatEntryType
function Ext.GetStat(stat, level) end

--- @deprecated
--- Creates a new stats entry on the server
--- @param name string Stat entry name
--- @param type string Stat entry type (i.e. SkillData, StatusData, etc.)
--- @param template string|nil When not nil, all properties are copied from the specified stats entry
--- @return StatEntryType
function Ext.CreateStat(name, type, template) end

--- @deprecated
--- Synchronizes all modifications of the specified stat to all clients
--- @param name string Stat entry name
--- @param persist boolean|nil Persist stats entry to savegame?
function Ext.SyncStat(name, persist) end

--- @deprecated
--- Toggles whether the specified stats entry should be persisted to savegames
--- @param name string Stat entry name
--- @param persist boolean Persist stats entry to savegame?
function Ext.StatSetPersistence(name, persist) end

--- @deprecated
--- Returns the textual label assigned to the specified enumeration value
--- @param enum string Engine enumeration name
--- @param index number Value index to look up
--- @return string|nil
function Ext.EnumIndexToLabel(enum, index) end

--- @deprecated
--- Returns the numeric index assigned to the specified enumeration label
--- @param enum string Engine enumeration name
--- @param label string Value name to look for
--- @return number|nil
function Ext.EnumLabelToIndex(enum, label) end

--- @deprecated
--- Execute the SkillProperties of the specified skill on a target character
--- @param skillId string Stats skill ID
--- @param attacker ComponentHandle|number|string Attacker character handle/NetID/UUID
--- @param target ComponentHandle|number|string Target character handle/NetID/UUID
--- @param position number[]
--- @param propertyContext string Target|AoE|Self|SelfOnHit|SelfOnEquip
--- @param isFromItem boolean
function Ext.ExecuteSkillPropertiesOnTarget(skillId, attacker, target, position, propertyContext, isFromItem) end

--- @deprecated
--- Execute the SkillProperties of the specified skill on a position
--- @param skillId string Stats skill ID
--- @param attacker ComponentHandle|number|string Attacker character handle/NetID/UUID
--- @param position number[]
--- @param radius number
--- @param propertyContext string Target|AoE|Self|SelfOnHit|SelfOnEquip
--- @param isFromItem boolean
function Ext.ExecuteSkillPropertiesOnPosition(skillId, attacker, position, radius, propertyContext, isFromItem) end

--- @class SurfaceInteractionSet
--- @field TransformType string Surface transform to apply (Bless, Curse, Ignite, ...)
--- @field ActionableSurfaces string[][] Surface types that this transform applies to

--- @deprecated
--- Returns the transformation rules that are applied when two neighbouring surfaces interact
--- @return table<integer, SurfaceInteractionSet[]>
function Ext.GetSurfaceTransformRules() end

--- @deprecated
--- Returns the surface template for the specified surface type
--- @param type string See SurfaceType enumeration
--- @return SurfaceTemplate
function Ext.GetSurfaceTemplate(type) end

--- @deprecated
--- Updates the transformation rules that are applied when two neighbouring surfaces interact
--- @param rules table<integer, SurfaceInteractionSet[]> New rules to apply
function Ext.UpdateSurfaceTransformRules(rules) end

--- @deprecated
--- Returns a table containing the UUID of all registered custom stat definitions
--- @return string[]
--- @see Ext_ServerCustomStat#GetAll
function Ext.GetAllCustomStats() end

--- @deprecated
--- Retrieve a custom stat definition by name
--- @param statName string Custom stat name to look for
--- @return CustomStatDefinition|nil
function Ext.GetCustomStatByName(statName) end

--- @deprecated
--- Retrieve a custom stat definition by id
--- @param statId string Custom stat UUID to look for
--- @return CustomStatDefinition|nil
function Ext.GetCustomStatById(statId) end

--- @deprecated
--- Create a new custom stat definition
--- @param name string Custom stat name
--- @param description string Custom stat description
--- @return string|nil Custom stat UUID
function Ext.CreateCustomStat(name, description) end

--- @deprecated
--- Returns the UUID of all characters on the specified level. 
--- Uses the current level if no level name was specified
--- @param level string|nil Optional level name
--- @return string[]
function Ext.GetAllCharacters(level) end

--- @deprecated
--- Returns the UUID of all characters within a radius around the specified point
--- @param x number
--- @param y number
--- @param z number
--- @param distance number
--- @return string[]
function Ext.GetCharactersAroundPosition(x, y, z, distance) end

--- @deprecated
--- Returns the UUID of all items on the specified level. 
--- Uses the current level if no level name was specified
--- @param level string|nil Optional level name
--- @return string[]
function Ext.GetAllItems(level) end

--- @deprecated
--- Returns the UUID of all items within a radius around the specified point
--- @param x number
--- @param y number
--- @param z number
--- @param distance number
--- @return string[]
function Ext.GetItemsAroundPosition(x, y, z, distance) end

--- @deprecated
--- Returns the UUID of all triggers on the specified level. 
--- Uses the current level if no level name was specified
--- @param level string|nil Optional level name
--- @return string[]
function Ext.GetAllTriggers(level) end

--- @deprecated
--- Returns the property proxy of the specified character
--- @param id string|integer|ComponentHandle Character UUID or handle or NetID
--- @return EsvCharacter|EclCharacter
function Ext.GetCharacter(id) end

--- @deprecated
--- Returns the property proxy of the specified item
--- @param id string|integer|ComponentHandle Item UUID or handle or NetID
--- @return EsvItem|EclItem
function Ext.GetItem(id) end

--- @deprecated
--- Returns the property proxy of the specified trigger (server only)
--- @param id string|ComponentHandle Trigger UUID or handle
--- @return Trigger
function Ext.GetTrigger(id) end

--- @deprecated
--- Returns the property proxy of the specified character, item, projectile or trigger
--- @param handle ComponentHandle|string Game object handle or UUID. NetID will fail since it has no type information (an item and a character could have the same NetID)
--- @return IEoCServerObject|IEoCClientObject
function Ext.GetGameObject(handle) end

--- @deprecated
--- Returns the property proxy of the specified surface
--- @param handle ComponentHandle Surface handle
--- @return EsvSurface
function Ext.GetSurface(handle) end

--- @deprecated
--- Returns the property proxy of the specified status
--- @param character Guid|ComponentHandle Character Guid or handle
--- @param handle integer|ComponentHandle Status handle or NetID
--- @return EsvStatus
function Ext.GetStatus(character, handle) end

--- @deprecated
--- Returns the specified turn-based combat
--- @param combatId integer Combat ID
--- @return EsvCombat
function Ext.GetCombat(combatId) end

--- @deprecated
--- Returns the AI grid for the currently active level
--- @return EocAiGrid
function Ext.GetAiGrid() end

--- @deprecated
--- Returns information about the currently active level
--- @return LevelDesc
function Ext.GetCurrentLevelData() end

--- @deprecated
--- Creates a new damage list object
--- @return StatsDamagePairList
function Ext.NewDamageList() end

--- @deprecated
--- Returns whether Osiris is currently accessible or not
--- @return boolean
function Ext.OsirisIsCallable() end

--- @deprecated
--- Returns a random number; equivalent to Lua random
--- @param low integer
--- @param up integer
--- @return integer|number
function Ext.Random(low, up) end

--- @deprecated
--- Rounds the specified number
--- @param n number
--- @return number
function Ext.Round(n) end

--- @deprecated
--- Generate Lua IDE helpers for the currently loaded module
--- @param builtin boolean|nil Only export built-in functions and names exported by Lua?
function Ext.GenerateIdeHelpers(builtin) end

--- @deprecated
--- Returns whether the Developer Mode switch is enabled
--- @return boolean
function Ext.IsDeveloperMode() end

--- @deprecated
--- Returns the current gamemode
--- @return string|"Campaign"|"GameMaster"|"Arena"
function Ext.GetGameMode() end

--- @deprecated
--- Returns the current difficulty (0-3). 0 = Story, 1 = Explorer, 2 = Classic, 3 = Tactician, 4 = Honour
--- @return integer
function Ext.GetDifficulty() end

--- @deprecated
--- Broadcast a message to all peers
--- @param channel string Channel that will receive the message
--- @param payload string Message payload
--- @param excludeCharacter string|nil Optional peer to exclude from broadcast
function Ext.BroadcastMessage(channel, payload, excludeCharacter) end

--- @deprecated
--- Sends a message to the peer that controls the specified character
--- @param characterGuid string Character that will receive the message
--- @param channel string Channel that will receive the message
--- @param payload string Message payload
function Ext.PostMessageToClient(characterGuid, channel, payload) end

--- @deprecated
--- Sends a message to the specified peer
--- @param userId number User that will receive the message
--- @param channel string Channel that will receive the message
--- @param payload string Message payload
function Ext.PostMessageToUser(userId, channel, payload) end

--- @deprecated
--- Sends a message to the server
--- @param channel string Channel that will receive the message
--- @param payload string Message payload
function Ext.PostMessageToServer(channel, payload) end

--- @alias NetListenerCallback fun(channel:string, payload:string, user:integer|nil)

--- Registers a listener that is called when a network message is received on the specified channel
--- @param channel string Network channel name
--- @param handler NetListenerCallback Lua handler
function Ext.RegisterNetListener(channel, handler) end

--- @deprecated
--- Registers a new dialog voice line for the specified speaker
--- @param speakerGuid string Speaker character UUID
--- @param textKey string Translated string key of text line
--- @param path string Path to audio .WEM
--- @param length number Length of audio in seconds
function Ext.AddVoiceMetaData(speakerGuid, textKey, path, length) end

--- @deprecated
--- @param handle string Translated string handle
--- @param fallback string Fallback string if the specified handle is not found
--- @return string Translated string
function Ext.GetTranslatedString(handle, fallback) end

--- @deprecated
--- @param key string Translated string key
--- @return string,string Translated string and handle
function Ext.GetTranslatedStringFromKey(key) end

--- @deprecated
--- @param key string Translated string key
--- @param handle string Translated string handle
--- @return boolean
function Ext.CreateTranslatedStringKey(key, handle) end

--- @deprecated
--- @param handle string Translated string handle
--- @param text string Display text
--- @return boolean
function Ext.CreateTranslatedStringHandle(handle, text) end

--- @deprecated
--- @param key string Translated string key
--- @param text string Display text
--- @return string|nil Created string handle
function Ext.CreateTranslatedString(key, text) end

--- @deprecated
--- Redirects all file accesses to the specified path to another file
--- @param path string Original path
--- @param newPath string New (redirected) path
function Ext.AddPathOverride(path, newPath) end

--- @deprecated
--- Returns whether the specified path is currently redirected to another path
--- @param path string Original path
--- @return string|nil Overridden path
function Ext.GetPathOverride(path) end

--- @deprecated
--- Constructs a new Flash UI element
--- @param name string User-defined unique name that identifies the UI element
--- @param path string Path of the SWF file relative to the data directory
--- @param layer integer Stack order of the UI element
--- @param flags integer Optional UI flags to test (experimental)
--- @return UIObject|nil
function Ext.CreateUI(name, path, layer, flags) end

--- @deprecated
--- Retrieves an UI element with the specified name
--- @param name string User-defined unique name that identifies the UI element
--- @return UIObject|nil
function Ext.GetUI(name) end

--- @deprecated
--- Retrieves a built-in UI element at the specified path
--- If no such element exists, the function returns nil
--- @param path string UI SWF path relative to Data\
--- @return UIObject|nil
function Ext.GetBuiltinUI(path) end

--- @deprecated
--- Retrieves an engine UI element with the specified engine type ID
--- If no such element exists, the function returns nil
--- @param typeId number Engine UI element type ID
--- @return UIObject|nil
function Ext.GetUIByType(typeId) end

--- @deprecated
--- Destroys the specified UI element
--- @param name string User-defined unique name that identifies the UI element
function Ext.DestroyUI(name) end

--- @deprecated
--- Refresh the UI of the specified character
--- @param character ComponentHandle Handle of character
--- @param flags integer UI elements to refresh
function Ext.UISetDirty(character, flags) end

--- @deprecated
--- Enable/disable debug prints on Flash custom draw callbacks. Useful if you need to see what icon names a UI is handling, for usage with UIObject:SetCustomIcon
--- @param enable boolean
function Ext.UIEnableCustomDrawCallDebugging(enable) end

--- @deprecated
--- Write data to an external (persistent) file
--- @param path string File path relative to Documents\Larian Studios\Divinity Original Sin 2 Definitive Edition\Osiris Data
--- @param contents string File contents to write
function Ext.SaveFile(path, contents) end

--- @deprecated
--- Read data from an external (persistent) file
--- @param path string File path relative to Documents\Larian Studios\Divinity Original Sin 2 Definitive Edition\Osiris Data
--- @param context string|nil Path context (nil or"user"means relative to the Osiris Data directory;"data"means relative to game data path)
--- @return string File contents
function Ext.LoadFile(path, context) end

--- @deprecated
--- Returns a monotonic value representing the current time in milliseconds
--- Useful for performance measurements / measuring real world time
--- (Note: This value is not synchronized between peers and different clients may report different time values!)
--- @return number Time
function Ext.MonotonicTime() end

--- @deprecated
--- Returns whether the player has a compatible Script Extender version installed
--- @param playerGuid string UUID of player character
--- @return boolean
function Ext.PlayerHasExtender(playerGuid) end

--- @deprecated
--- Returns information about current mouse position and hovered objects
--- @return EclPickingState
function Ext.GetPickingState() end

--- @deprecated
--- Use Ext.Debug.DebugBreak instead.  
--- Triggers a breakpoint in the Lua debugger
--- If no debugger is connected, the function does nothing
--- @see Ext_Debug#DebugBreak
function Ext.DebugBreak() end

--- @deprecated
--- Use Ext.UI.HandleToDouble instead.  
--- Handle to double conversion hack for use in Flash external interface calls
--- (Some of the builtin functions treat handles as double values)
--- @param handle ComponentHandle|ComponentHandle Handle to cast
--- @return number Double handle
--- @see Ext_ClientUI#HandleToDouble
function Ext.HandleToDouble(handle) end

--- @deprecated
--- Use Ext.UI.DoubleToHandle instead.  
--- Double to handle conversion hack for use in Flash external interface calls
--- (Some of the builtin functions treat handles as double values)
--- @param handle number Double handle to cast
--- @return ComponentHandle|ComponentHandle
--- @see Ext_ClientUI#DoubleToHandle
function Ext.DoubleToHandle(handle) end

--- @deprecated
--- Use Ext.Client.UpdateShroud instead.  
--- Modifies the shroud value at a given cell
--- @param x number
--- @param y number
--- @param layer ShroudType
--- @param value integer The shroud cell value, 0-255
--- @see Ext_ClientClient#UpdateShroud
function Ext.UpdateShroud(x, y, layer, value) end

--- @deprecated
--- No longer needed
function Ext.EnableExperimentalPropertyWrites() end

--#endregion

