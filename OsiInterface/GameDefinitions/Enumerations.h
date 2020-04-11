#pragma once

#include <cstdint>

namespace dse
{
	template <class T, class L>
	struct EnumInfoBase
	{
		struct Label
		{
			T Val;
			char const * Name;
		};

		static std::optional<T> Find(char const * name)
		{
			for (auto i = 0; i < std::size(L::Values); i++) {
				if (strcmp(name, L::Values[i].Name) == 0) {
					return L::Values[i].Val;
				}
			}

			return {};
		}

		static std::optional<char const *> Find(T val)
		{
			for (auto i = 0; i < std::size(L::Values); i++) {
				if (val == L::Values[i].Val) {
					return L::Values[i].Name;
				}
			}

			return {};
		}
	};

	template<typename T> struct EnumInfoFakeDep : public std::false_type {};

	template <class T>
	struct EnumInfo
	{
		static_assert(EnumInfoFakeDep<T>::value, "EnumInfo not implemented for this type!");
	};

	#define MAKE_ENUM_INFO(typeName, size) \
		template <> struct EnumInfo<typeName> : public EnumInfoBase<typeName, EnumInfo<typeName>> { \
			static constexpr char const * Name = #typeName; \
			static Label const Values[size]; \
		}


	enum PathRootType
	{
		Root = 0, // Filesystem root
		Data = 1, // Game data folder
		Public = 2, // Game data/Public folder
		MyDocuments = 3, // User's My Documents folder
		GameStorage = 4 // Game settings/savegames folder
	};

	namespace ecl
	{
		enum class GameState : uint32_t
		{
			Unknown = 0,
			Init,
			InitMenu,
			InitNetwork,
			InitConnection,
			Idle,
			LoadMenu,
			Menu,
			Exit,
			SwapLevel,
			LoadLevel = 10,
			LoadModule,
			LoadSession,
			LoadGMCampaign,
			UnloadLevel,
			UnloadModule,
			UnloadSession,
			Paused,
			PrepareRunning,
			Running,
			Disconnect = 20,
			Join,
			StartLoading,
			StopLoading,
			StartServer,
			Movie,
			Installation,
			GameMasterPause,
			ModReceiving,
			Lobby,
			BuildStory = 30
		};


		enum class ComponentType : uint32_t
		{
			// Registered from App::InitGame
			AnimationBlueprint = 0,
			Spline,
			Combat,
			CustomStats,
			GameMaster,
			GMJournalNode,
			Container,
			ContainerElement,

			// Registered from BaseApp::RegisterBaseComponentAndSystemTypes
			Visual,
			Light,
			LightProbe,
			Decal,
			Sound,
			PublishingRequest,
			Effect,
			ParentEntity,

			// Registered from ecl::EoCClient::ConfigureECS
			Net,
			OverviewMap,
			Vignette,
			Note,
			CustomStatDefinition,
			Scenery,
			Item,
			Character,
			Egg,
			Projectile,
			// ...
		};
	}

	MAKE_ENUM_INFO(ecl::ComponentType, 18);

	namespace esv
	{
		enum class GameState : uint32_t
		{
			Unknown = 0,
			Uninitialized,
			Init,
			Idle,
			Exit,
			LoadLevel,
			LoadModule,
			LoadGMCampaign,
			LoadSession,
			UnloadLevel,
			UnloadModule = 10,
			UnloadSession,
			Sync,
			Paused,
			Running,
			Save,
			Disconnect,
			GameMasterPause,
			BuildStory,
			ReloadStory,
			Installation = 20
		};


		enum class ObjectType : uint32_t
		{
			CustomStatDefinitionComponent = 0x35,
			Egg,
			Character,
			Item,
			Projectile,
			Inventory,
			InventoryView,
			Party,
			OverviewMap,
			Vignette,
			Note,
			SurfaceAction,
			EoCPointTrigger,
			// ... other triggers
		};


		enum class ComponentType : uint32_t
		{
			// Registered from App::InitGame
			AnimationBlueprint = 0,
			Spline,
			Combat,
			CustomStats,
			GameMaster,
			GMJournalNode,
			Container,
			ContainerElement,

			// Registered from esv::EocServer::ConfigureECS
			Net,
			OverviewMap,
			Note,
			Vignette,
			CustomStatDefinition,
			Item,
			Character,
			Egg,
			Projectile,
			// ...
		};

		enum class SystemType : uint32_t
		{
			AnimationBlueprint = 0,
			Container,
			ContainerElement,
			TurnManager,
			SightManager,
			GameAction,
			Surface,
			Character,
			Projectile,
			Trigger,
			Item,
			Egg,
			Effect,
			Shroud,
			EnvironmentalStatus,
			NetEntity,
			CustomStat,
			Reward,
			CharacterSpline,
			CameraSpline,
			GM,
			GMCampaign,
			GMJournal
		};
	}

	MAKE_ENUM_INFO(esv::ComponentType, 17);


	enum NetMessage : uint32_t
	{
		NETMSG_HOST_WELCOME = 1,
		NETMSG_HOST_REFUSE = 2,
		NETMSG_HOST_REFUSEPLAYER = 3,
		NETMSG_HOST_LEFT = 4,
		NETMSG_CLIENT_CONNECT = 5,
		NETMSG_CLIENT_ACCEPT = 6,
		NETMSG_CLIENT_JOINED = 7,
		NETMSG_CLIENT_LEFT = 8,
		NETMSG_PLAYER_CONNECT = 9,
		NETMSG_PLAYER_DISCONNECT = 10,
		NETMSG_PLAYER_ACCEPT = 11,
		NETMSG_PLAYER_JOINED = 12,
		NETMSG_PLAYER_LEFT = 13,
		NETMSG_VOICEDATA = 14,
		NETMSG_SKIPMOVIE_RESULT = 15,
		NETMSG_SURFACE_CREATE = 17,
		NETMSG_SURFACE_META = 18,
		NETMSG_GAMEACTION = 19,
		NETMSG_CHARACTER_CREATE = 20,
		NETMSG_CHARACTER_DESTROY = 21,
		NETMSG_CHARACTER_CONFIRMATION = 22,
		NETMSG_CHARACTER_ACTIVATE = 23,
		NETMSG_CHARACTER_DEACTIVATE = 24,
		NETMSG_CHARACTER_ASSIGN = 25,
		NETMSG_CHARACTER_POSITION = 26,
		NETMSG_CHARACTER_POSITION_SYNC = 27,
		NETMSG_CHARACTER_STEERING = 28,
		NETMSG_CHARACTER_ACTION = 29,
		NETMSG_CHARACTER_ACTION_DATA = 30,
		NETMSG_CHARACTER_ACTION_REQUEST_RESULT = 31,
		NETMSG_CHARACTER_STATUS = 32,
		NETMSG_CHARACTER_STATUS_LIFETIME = 33,
		NETMSG_CHARACTER_UPDATE = 34,
		NETMSG_CHARACTER_CONTROL = 35,
		NETMSG_CHARACTER_SET_STORY_NAME = 36,
		NETMSG_CHARACTER_DIALOG = 37,
		NETMSG_CHARACTER_SKILLBAR = 38,
		NETMSG_CHARACTER_SELECTEDSKILLSET = 39,
		NETMSG_CHARACTER_USE_AP = 40,
		NETMSG_CHARACTER_UPGRADE = 41,
		NETMSG_CHARACTER_STATS_UPDATE = 42,
		NETMSG_CHARACTER_AOO = 43,
		NETMSG_CHARACTER_BOOST = 44,
		NETMSG_CHARACTER_CUSTOMIZATION = 45,
		NETMSG_CHARACTER_TRANSFORM = 46,
		NETMSG_CHARACTER_TELEPORT = 47,
		NETMSG_CHARACTER_PICKPOCKET = 48,
		NETMSG_CHARACTER_OFFSTAGE = 49,
		NETMSG_CHARACTER_LOCK_ABILITY = 50,
		NETMSG_CHARACTER_LOOT_CORPSE = 51,
		NETMSG_CHARACTER_ITEM_USED = 52,
		NETMSG_CHARACTER_CORPSE_LOOTABLE = 53,
		NETMSG_CHARACTER_IN_DIALOG = 54,
		NETMSG_COMBAT_TURN_TIMER = 55,
		NETMSG_COMBAT_COMPONENT_SYNC = 56,
		NETMSG_UPDATE_COMBAT_GROUP_INFO = 57,
		NETMSG_PARTY_CREATE = 58,
		NETMSG_PARTY_DESTROY = 59,
		NETMSG_PARTYGROUP = 60,
		NETMSG_PARTYORDER = 61,
		NETMSG_PARTYUPDATE = 62,
		NETMSG_PARTYUSER = 63,
		NETMSG_SHROUD_UPDATE = 64,
		NETMSG_SHROUD_FRUSTUM_UPDATE = 65,
		NETMSG_SECRET_REGION_UNLOCK = 66,
		NETMSG_ITEM_CREATE = 67,
		NETMSG_ITEM_DESTROY = 68,
		NETMSG_ITEM_ACTIVATE = 69,
		NETMSG_ITEM_DEACTIVATE = 70,
		NETMSG_ITEM_DESTINATION = 71,
		NETMSG_ITEM_UPDATE = 72,
		NETMSG_ITEM_USE_REMOTELY = 73,
		NETMSG_ITEM_MOVE_TO_WORLD = 74,
		NETMSG_ITEM_MOVE_TO_INVENTORY = 75,
		NETMSG_ITEM_ACTION = 76,
		NETMSG_ITEM_STATUS = 77,
		NETMSG_ITEM_STATUS_LIFETIME = 78,
		NETMSG_ITEM_TRANSFORM = 79,
		NETMSG_ITEM_CONFIRMATION = 80,
		NETMSG_ITEM_OFFSTAGE = 81,
		NETMSG_ITEM_MOVED_INFORM = 82,
		NETMSG_ITEM_MOVE_UUID = 83,
		NETMSG_INVENTORY_CREATE = 84,
		NETMSG_INVENTORY_CREATE_AND_OPEN = 85,
		NETMSG_INVENTORY_DESTROY = 86,
		NETMSG_INVENTORY_VIEW_CREATE = 87,
		NETMSG_INVENTORY_VIEW_DESTROY = 88,
		NETMSG_INVENTORY_VIEW_UPDATE_ITEMS = 89,
		NETMSG_INVENTORY_VIEW_UPDATE_PARENTS = 90,
		NETMSG_INVENTORY_VIEW_SORT = 91,
		NETMSG_INVENTORY_ITEM_UPDATE = 92,
		NETMSG_INVENTORY_LOCKSTATE_SYNC = 93,
		NETMSG_PROJECTILE_CREATE = 94,
		NETMSG_PROJECTILE_EXPLOSION = 95,
		NETMSG_EGG_CREATE = 96,
		NETMSG_EGG_DESTROY = 97,
		NETMSG_TURNBASED_START = 98,
		NETMSG_TURNBASED_STOP = 99,
		NETMSG_TURNBASED_ROUND = 100,
		NETMSG_TURNBASED_ORDER = 101,
		NETMSG_TURNBASED_SUMMONS = 102,
		NETMSG_TURNBASED_SETTEAM = 103,
		NETMSG_TURNBASED_FLEECOMBATRESULT = 104,
		NETMSG_NET_ENTITY_CREATE = 105,
		NETMSG_NET_ENTITY_DESTROY = 106,
		NETMSG_PEER_ACTIVATE = 107,
		NETMSG_PEER_DEACTIVATE = 108,
		NETMSG_SKILL_CREATE = 109,
		NETMSG_SKILL_DESTROY = 110,
		NETMSG_SKILL_UPDATE = 111,
		NETMSG_SKILL_LEARN = 112,
		NETMSG_SKILL_UNLEARN = 113,
		NETMSG_SKILL_ACTIVATE = 114,
		NETMSG_SKILL_REMOVED = 115,
		NETMSG_EFFECT_CREATE = 116,
		NETMSG_EFFECT_FORGET = 117,
		NETMSG_EFFECT_DESTROY = 118,
		NETMSG_GAMECONTROL_UPDATE_S2C = 119,
		NETMSG_GAMECONTROL_UPDATE_C2S = 120,
		NETMSG_GAMECONTROL_PRICETAG = 121,
		NETMSG_FORCE_SHEATH = 122,
		NETMSG_PLAYSOUND = 123,
		NETMSG_PLAYMOVIE = 124,
		NETMSG_PLAY_HUD_SOUND = 125,
		NETMSG_CHARACTER_ERROR = 126,
		NETMSG_ALIGNMENT_CREATE = 127,
		NETMSG_ALIGNMENT_SET = 128,
		NETMSG_ALIGNMENT_RELATION = 129,
		NETMSG_ALIGNMENT_CLEAR = 130,
		NETMSG_CACHETEMPLATE = 131,
		NETMSG_OVERHEADTEXT = 132,
		NETMSG_COMBATLOG = 133,
		NETMSG_COMBATLOGITEMINTERACTION = 134,
		NETMSG_SCREEN_FADE = 135,
		NETMSG_SCREEN_FADE_DONE = 136,
		NETMSG_OPEN_CUSTOM_BOOK_UI_MESSAGE = 137,
		NETMSG_CLOSE_CUSTOM_BOOK_UI_MESSAGE = 138,
		NETMSG_OPEN_KICKSTARTER_BOOK_UI_MESSAGE = 139,
		NETMSG_OPEN_MESSAGE_BOX_MESSAGE = 140,
		NETMSG_CLOSED_MESSAGE_BOX_MESSAGE = 141,
		NETMSG_OPEN_WAYPOINT_UI_MESSAGE = 142,
		NETMSG_CLOSE_UI_MESSAGE = 143,
		NETMSG_SHOW_ENTER_REGION_UI_MESSAGE = 144,
		NETMSG_JOURNAL_RESET = 145,
		NETMSG_QUEST_UPDATE = 146,
		NETMSG_QUEST_CATEGORY_UPDATE = 147,
		NETMSG_QUEST_STATE = 148,
		NETMSG_SECRET_UPDATE = 149,
		NETMSG_TROPHY_UPDATE = 150,
		NETMSG_JOURNALRECIPE_UPDATE = 151,
		NETMSG_MARKER_UI_CREATE = 152,
		NETMSG_MARKER_UI_UPDATE = 153,
		NETMSG_QUEST_TRACK = 154,
		NETMSG_QUEST_POSTPONE = 155,
		NETMSG_UI_QUESTSELECTED = 156,
		NETMSG_MYSTERY_STATE = 157,
		NETMSG_MYSTERY_UPDATE = 158,
		NETMSG_PARTYFORMATION = 159,
		NETMSG_STOP_FOLLOW = 160,
		NETMSG_NOTIFICATION = 161,
		NETMSG_GAMEOVER = 162,
		NETMSG_REGISTER_WAYPOINT = 163,
		NETMSG_FLAG_UPDATE = 164,
		NETMSG_PARTY_CONSUMED_ITEMS = 165,
		NETMSG_PARTY_UNLOCKED_RECIPE = 166,
		NETMSG_PARTY_SPLIT_NOTIFICATION = 167,
		NETMSG_PARTY_MERGE_NOTIFICATION = 168,
		NETMSG_DIALOG_STATE_MESSAGE = 169,
		NETMSG_DIALOG_NODE_MESSAGE = 170,
		NETMSG_DIALOG_ANSWER_MESSAGE = 171,
		NETMSG_DIALOG_ANSWER_HIGHLIGHT_MESSAGE = 172,
		NETMSG_DIALOG_HISTORY_MESSAGE = 173,
		NETMSG_DIALOG_ACTORJOINS_MESSAGE = 174,
		NETMSG_DIALOG_ACTORLEAVES_MESSAGE = 175,
		NETMSG_GM_SPAWN = 176,
		NETMSG_GM_DELETE = 177,
		NETMSG_GM_ITEM_CHANGE = 178,
		NETMSG_GM_DRAW_SURFACE = 179,
		NETMSG_GM_TOGGLE_OVERVIEWMAP = 180,
		NETMSG_GM_TOGGLE_VIGNETTE = 181,
		NETMSG_GM_DAMAGE = 182,
		NETMSG_GM_EXPORT = 183,
		NETMSG_GM_REMOVE_EXPORTED = 184,
		NETMSG_GM_HEAL = 185,
		NETMSG_GM_ADD_EXPERIENCE = 186,
		NETMSG_GM_TOGGLE_PAUSE = 187,
		NETMSG_GM_REQUEST_ROLL = 188,
		NETMSG_GM_ROLL = 189,
		NETMSG_GM_PASS_ROLL = 190,
		NETMSG_GM_REMOVE_ROLL = 191,
		NETMSG_GM_TICK_ROLLS = 192,
		NETMSG_GM_CHANGE_LEVEL = 193,
		NETMSG_GM_VIGNETTE_ANSWER = 194,
		NETMSG_GM_POSITION_SYNC = 195,
		NETMSG_GM_TELEPORT = 196,
		NETMSG_GM_TRAVEL_TO_DESTINATION = 197,
		NETMSG_GM_STOP_TRAVELING = 198,
		NETMSG_GM_CAMPAIGN_SAVE = 199,
		NETMSG_GM_SYNC_SCENES = 200,
		NETMSG_GM_SYNC_OVERVIEW_MAPS = 201,
		NETMSG_GM_SYNC_VIGNETTES = 202,
		NETMSG_GM_SYNC_ASSETS = 203,
		NETMSG_GM_ASSETS_PENDING_SYNCS_INFO = 204,
		NETMSG_GM_REORDER_ELEMENTS = 205,
		NETMSG_GM_SET_START_POINT = 206,
		NETMSG_GM_CONFIGURE_CAMPAIGN = 207,
		NETMSG_GM_LOAD_CAMPAIGN = 208,
		NETMSG_GM_REQUEST_CAMPAIGN_DATA = 209,
		NETMSG_GM_POSSESS = 210,
		NETMSG_GM_CHANGE_NAME = 211,
		NETMSG_GM_DUPLICATE = 212,
		NETMSG_GM_ITEM_USE = 213,
		NETMSG_GM_HOST = 214,
		NETMSG_GM_TOGGLE_PEACE = 215,
		NETMSG_GM_MAKE_TRADER = 216,
		NETMSG_GM_SET_INTERESTED_CHARACTER = 217,
		NETMSG_GM_GIVE_REWARD = 218,
		NETMSG_GM_SET_STATUS = 219,
		NETMSG_GM_REMOVE_STATUS = 220,
		NETMSG_GM_CLEAR_STATUSES = 221,
		NETMSG_GM_CREATE_ITEM = 222,
		NETMSG_GM_SET_ATMOSPHERE = 223,
		NETMSG_GM_SOUND_PLAYBACK = 224,
		NETMSG_GM_CHANGE_SCENE_NAME = 225,
		NETMSG_GM_CHANGE_SCENE_PATH = 226,
		NETMSG_GM_EDIT_ITEM = 227,
		NETMSG_GM_EDIT_CHARACTER = 228,
		NETMSG_GM_UI_OPEN_STICKY = 229,
		NETMSG_GM_SYNC_NOTES = 230,
		NETMSG_GM_MAKE_FOLLOWER = 231,
		NETMSG_GM_DEACTIVATE = 232,
		NETMSG_GM_INVENTORY_OPERATION = 233,
		NETMSG_GM_SET_REPUTATION = 234,
		NETMSG_GM_REQUEST_CHARACTERS_REROLL = 235,
		NETMSG_TRADE_ACTION = 236,
		NETMSG_CAMERA_ACTIVATE = 237,
		NETMSG_CAMERA_ROTATE = 238,
		NETMSG_CAMERA_TARGET = 239,
		NETMSG_CAMERA_MODE = 240,
		NETMSG_ATMOSPHERE_OVERRIDE = 241,
		NETMSG_CHAT = 242,
		NETMSG_GAMETIME_SYNC = 243,
		NETMSG_PARTY_NPC_DATA = 244,
		NETMSG_CHARACTER_ANIMATION_SET_UPDATE = 245,
		NETMSG_REQUESTAUTOSAVE = 246,
		NETMSG_SAVEGAME = 247,
		NETMSG_SAVEGAMEHANDSHAKE = 248,
		NETMSG_MODULE_LOAD = 249,
		NETMSG_MODULE_LOADED = 250,
		NETMSG_MODULES_DOWNLOAD = 251,
		NETMSG_SESSION_LOAD = 252,
		NETMSG_SESSION_LOADED = 253,
		NETMSG_LEVEL_LOAD = 254,
		NETMSG_LEVEL_LOADED = 255,
		NETMSG_LOAD_START = 256,
		NETMSG_LEVEL_INSTANTIATE_SWAP = 257,
		NETMSG_LEVEL_SWAP_READY = 258,
		NETMSG_LEVEL_SWAP_COMPLETE = 259,
		NETMSG_LEVEL_START = 260,
		NETMSG_CHARACTERCREATION_DONE = 261,
		NETMSG_CHARACTERCREATION_READY = 262,
		NETMSG_CHARACTERCREATION_NOT_READY = 263,
		NETMSG_CHANGE_COMBAT_FORMATION = 264,
		NETMSG_DIFFICULTY_CHANGED = 265,
		NETMSG_CHARACTER_CHANGE_OWNERSHIP = 266,
		NETMSG_RUNECRAFT = 267,
		NETMSG_JOURNALDIALOGLOG_UPDATE = 268,
		NETMSG_SHOW_TUTORIAL_MESSAGE = 269,
		NETMSG_ACHIEVEMENT_UNLOCKED_MESSAGE = 270,
		NETMSG_NOTIFY_COMBINE_FAILED_MESSAGE = 271,
		NETMSG_SAVEGAME_LOAD_FAIL = 272,
		NETMSG_SERVER_COMMAND = 273,
		NETMSG_SERVER_NOTIFICATION = 274,
		NETMSG_ITEM_ENGRAVE = 275,
		NETMSG_STORY_ELEMENT_UI = 276,
		NETMSG_OPEN_CRAFT_UI_MESSAGE = 277,
		NETMSG_ACHIEVEMENT_PROGRESS_MESSAGE = 278,
		NETMSG_UNLOCK_WAYPOINT = 279,
		NETMSG_DISCOVERED_PORTALS = 280,
		NETMSG_LOCK_WAYPOINT = 281,
		NETMSG_CRAFT_RESULT = 282,
		NETMSG_UNLOCK_ITEM = 283,
		NETMSG_TELEPORT_PYRAMID = 284,
		NETMSG_TELEPORT_WAYPOINT = 285,
		NETMSG_REALTIME_MULTIPLAY = 286,
		NETMSG_DIALOG_LISTEN = 287,
		NETMSG_MUSIC_STATE = 288,
		NETMSG_MUSIC_EVENT = 289,
		NETMSG_LOBBY_DATAUPDATE = 290,
		NETMSG_LOBBY_USERUPDATE = 291,
		NETMSG_LOBBY_SPECTATORUPDATE = 292,
		NETMSG_LOBBY_COMMAND = 293,
		NETMSG_LOBBY_CHARACTER_SELECT = 294,
		NETMSG_LOBBY_STARTGAME = 295,
		NETMSG_LOBBY_SURRENDER = 296,
		NETMSG_LOBBY_RETURN = 297,
		NETMSG_UPDATE_CHARACTER_TAGS = 298,
		NETMSG_UPDATE_ITEM_TAGS = 299,
		NETMSG_AITEST_UPDATE = 300,
		NETMSG_TELEPORT_ACK = 301,
		NETMSG_PARTYCREATEGROUP = 302,
		NETMSG_ARENA_RESULTS = 303,
		NETMSG_CAMERA_SPLINE = 304,
		NETMSG_CHARACTER_COMPANION_CUSTOMIZATION = 305,
		NETMSG_READYCHECK = 306,
		NETMSG_PAUSE = 307,
		NETMSG_UNPAUSE = 308,
		NETMSG_SET_CHARACTER_ARCHETYPE = 309,
		NETMSG_DIPLOMACY = 310,
		NETMSG_TRIGGER_CREATE = 311,
		NETMSG_TRIGGER_DESTROY = 312,
		NETMSG_TRIGGER_UPDATE = 313,
		NETMSG_PING_BEACON = 314,
		NETMSG_CUSTOM_STATS_DEFINITION_CREATE = 315,
		NETMSG_CUSTOM_STATS_DEFINITION_REMOVE = 316,
		NETMSG_CUSTOM_STATS_DEFINITION_UPDATE = 317,
		NETMSG_CUSTOM_STATS_CREATE = 318,
		NETMSG_CUSTOM_STATS_UPDATE = 319,
		NETMSG_GIVE_REWARD = 320,
		NETMSG_LOAD_GAME_WITH_ADDONS = 321,
		NETMSG_LOAD_GAME_WITH_ADDONS_FAIL = 322,
		NETMSG_SHOW_ERROR = 323,
		NETMSG_CLIENT_GAME_SETTINGS = 324,
		NETMSG_GM_JOURNAL_UPDATE = 325,
		NETMSG_STORY_VERSION = 326,
		NETMSG_MULTIPLE_TARGET_OPERATION = 327,
		NETMSG_DLC_UPDATE = 329,
		NETMSG_MUTATORS_ENABLED = 330,
		NETMSG_HACK_TELL_BUILDNAME = 331
	};


	enum class PlayerUpgradeAttribute
	{
		Strength = 0,
		Finesse = 1,
		Intelligence = 2,
		Constitution = 3,
		Memory = 4,
		Wits = 5
	};

	MAKE_ENUM_INFO(PlayerUpgradeAttribute, 6);


	enum class StatGetterType
	{
		MaxMp = 0,
		APStart = 1,
		APRecovery = 2,
		APMaximum = 3,

		Strength = 4,
		Finesse = 5,
		Intelligence = 6,
		Constitution = 7,
		Memory = 8,
		Wits = 9,

		Accuracy = 10,
		Dodge = 11,
		CriticalChance = 12,

		FireResistance = 13,
		EarthResistance = 14,
		WaterResistance = 15,
		AirResistance = 16,
		PoisonResistance = 17,
		ShadowResistance = 18,
		CustomResistance = 19,

		LifeSteal = 20,
		Sight = 21,
		Hearing = 22,
		Movement = 23,
		Initiative = 24,
		BlockChance = 25,
		ChanceToHitBoost = 26
	};

	MAKE_ENUM_INFO(StatGetterType, 27);


	enum class RequirementType : uint32_t
	{
		None = 0,
		Level = 1,
		Strength = 2,
		Finesse = 3,
		Intelligence = 4,
		Constitution = 5,
		Memory = 6,
		Wits = 7,
		WarriorLore = 8,
		RangerLore = 9,
		RogueLore = 10,
		SingleHanded = 11,
		TwoHanded = 12,
		PainReflection = 13,
		Ranged = 14,
		Shield = 15,
		Reflexes = 16,
		PhysicalArmorMastery = 17,
		MagicArmorMastery = 18,
		Vitality = 19,
		Sourcery = 20,
		Telekinesis = 21,
		FireSpecialist = 22,
		WaterSpecialist = 23,
		AirSpecialist = 24,
		EarthSpecialist = 25,
		Necromancy = 26,
		Summoning = 27,
		Polymorph = 28,
		Repair = 30,
		Sneaking = 31,
		Pickpocket = 32,
		Thievery = 33,
		Loremaster = 34,
		Crafting = 35,
		Barter = 36,
		Charm = 37,
		Intimidate = 38,
		Reason = 39,
		Persuasion = 40,
		Leadership = 41,
		Luck = 42,
		DualWielding = 43,
		Wand = 44,
		Perseverance = 45,
		TALENT_ItemMovement = 46,
		TALENT_ItemCreation = 47,
		TALENT_Flanking = 48,
		TALENT_AttackOfOpportunity = 49,
		TALENT_Backstab = 50,
		TALENT_Trade = 51,
		TALENT_Lockpick = 52,
		TALENT_ChanceToHitRanged = 53,
		TALENT_ChanceToHitMelee = 54,
		TALENT_Damage = 55,
		TALENT_ActionPoints = 56,
		TALENT_ActionPoints2 = 57,
		TALENT_Criticals = 58,
		TALENT_IncreasedArmor = 59,
		TALENT_Sight = 60,
		TALENT_ResistFear = 61,
		TALENT_ResistKnockdown = 62,
		TALENT_ResistStun = 63,
		TALENT_ResistPoison = 64,
		TALENT_ResistSilence = 65,
		TALENT_ResistDead = 66,
		TALENT_Carry = 67,
		TALENT_Kinetics = 68,
		TALENT_Repair = 69,
		TALENT_ExpGain = 70,
		TALENT_ExtraStatPoints = 71,
		TALENT_ExtraSkillPoints = 72,
		TALENT_Durability = 73,
		TALENT_Awareness = 74,
		TALENT_Vitality = 75,
		TALENT_FireSpells = 76,
		TALENT_WaterSpells = 77,
		TALENT_AirSpells = 78,
		TALENT_EarthSpells = 79,
		TALENT_Charm = 80,
		TALENT_Intimidate = 81,
		TALENT_Reason = 82,
		TALENT_Luck = 83,
		TALENT_Initiative = 84,
		TALENT_InventoryAccess = 85,
		TALENT_AvoidDetection = 86,
		TALENT_AnimalEmpathy = 87,
		TALENT_Escapist = 88,
		TALENT_StandYourGround = 89,
		TALENT_SurpriseAttack = 90,
		TALENT_LightStep = 91,
		TALENT_ResurrectToFullHealth = 92,
		TALENT_Scientist = 93,
		TALENT_Raistlin = 94,
		TALENT_MrKnowItAll = 95,
		TALENT_WhatARush = 96,
		TALENT_FaroutDude = 97,
		TALENT_Leech = 98,
		TALENT_ElementalAffinity = 99,
		TALENT_FiveStarRestaurant = 100,
		TALENT_Bully = 101,
		TALENT_ElementalRanger = 102,
		TALENT_LightningRod = 103,
		TALENT_Politician = 104,
		TALENT_WeatherProof = 105,
		TALENT_LoneWolf = 106,
		TALENT_Zombie = 107,
		TALENT_Demon = 108,
		TALENT_IceKing = 109,
		TALENT_Courageous = 110,
		TALENT_GoldenMage = 111,
		TALENT_WalkItOff = 112,
		TALENT_FolkDancer = 113,
		TALENT_SpillNoBlood = 114,
		TALENT_Stench = 115,
		TALENT_Kickstarter = 116,
		TALENT_WarriorLoreNaturalArmor = 117,
		TALENT_WarriorLoreNaturalHealth = 118,
		TALENT_WarriorLoreNaturalResistance = 119,
		TALENT_RangerLoreArrowRecover = 120,
		TALENT_RangerLoreEvasionBonus = 121,
		TALENT_RangerLoreRangedAPBonus = 122,
		TALENT_RogueLoreDaggerAPBonus = 123,
		TALENT_RogueLoreDaggerBackStab = 124,
		TALENT_RogueLoreMovementBonus = 125,
		TALENT_RogueLoreHoldResistance = 126,
		TALENT_NoAttackOfOpportunity = 127,
		TALENT_WarriorLoreGrenadeRange = 128,
		TALENT_RogueLoreGrenadePrecision = 129,
		TALENT_ExtraWandCharge = 130,
		TALENT_DualWieldingDodging = 131,
		TALENT_Human_Civil = 133,
		TALENT_Human_Inventive = 132,
		TALENT_Dwarf_Sneaking = 134,
		TALENT_Dwarf_Sturdy = 135,
		TALENT_Elf_CorpseEater = 136,
		TALENT_Elf_Lore = 137,
		TALENT_Lizard_Persuasion = 139,
		TALENT_Lizard_Resistance = 138,
		TALENT_Perfectionist = 140,
		TALENT_Executioner = 141,
		TALENT_QuickStep = 143,
		TALENT_ViolentMagic = 142,
		TALENT_Memory = 144,
		TALENT_LivingArmor = 145,
		TALENT_Torturer = 146,
		TALENT_Ambidextrous = 147,
		TALENT_Unstable = 148,
		TALENT_Sourcerer = 149,
		TRAIT_Forgiving = 150,
		TRAIT_Vindictive = 151,
		TRAIT_Bold = 152,
		TRAIT_Timid = 153,
		TRAIT_Altruistic = 154,
		TRAIT_Egotistical = 155,
		TRAIT_Independent = 156,
		TRAIT_Obedient = 157,
		TRAIT_Pragmatic = 158,
		TRAIT_Romantic = 159,
		TRAIT_Spiritual = 160,
		TRAIT_Materialistic = 161,
		TRAIT_Righteous = 162,
		TRAIT_Renegade = 163,
		TRAIT_Blunt = 164,
		TRAIT_Considerate = 165,
		TRAIT_Compassionate = 166,
		TRAIT_Heartless = 167,
		Combat = 168,
		MinKarma = 169,
		MaxKarma = 170,
		Immobile = 171,
		Tag = 172,
	};

	MAKE_ENUM_INFO(RequirementType, 173);

	enum class WeaponType : uint32_t
	{
		None = 0,
		Sword = 1,
		Club = 2,
		Axe = 3,
		Staff = 4,
		Bow = 5,
		Crossbow = 6,
		Spear = 7,
		Knife = 8,
		Wand = 9,
		Arrow = 10,
		Rifle = 11
	};

	MAKE_ENUM_INFO(WeaponType, 12);

	enum class AbilityType
	{
		WarriorLore = 0,
		RangerLore = 1,
		RogueLore = 2,
		SingleHanded = 3,
		TwoHanded = 4,
		PainReflection = 5,
		Ranged = 6,
		Shield = 7,
		Reflexes = 8,
		PhysicalArmorMastery = 9,
		MagicArmorMastery = 10,
		VitalityMastery = 11,
		Sourcery = 12,
		FireSpecialist = 13,
		WaterSpecialist = 14,
		AirSpecialist = 15,
		EarthSpecialist = 16,
		Necromancy = 17,
		Summoning = 18,
		Polymorph = 19,
		Telekinesis = 20,
		Repair = 21,
		Sneaking = 22,
		Pickpocket = 23,
		Thievery = 24,
		Loremaster = 25,
		Crafting = 26,
		Barter = 27,
		Charm = 28,
		Intimidate = 29,
		Reason = 30,
		Persuasion = 31,
		Leadership = 32,
		Luck = 33,
		DualWielding = 34,
		Wand = 35,
		Perseverance = 36,
		Runecrafting = 37,
		Brewmaster = 38,
		Sulfurology = 39,
		Sentinel = 40
	};

	MAKE_ENUM_INFO(AbilityType, 41);

	enum class CriticalRoll
	{
		Roll = 0,
		Critical = 1,
		NotCritical = 2
	};

	MAKE_ENUM_INFO(CriticalRoll, 3);

	enum class HighGroundBonus
	{
		Unknown = 0,
		HighGround = 1,
		EvenGround = 2,
		LowGround = 3
	};

	MAKE_ENUM_INFO(HighGroundBonus, 4);

	enum class EquipmentStatsType : uint32_t
	{
		Weapon = 0,
		Armor = 1,
		Shield = 2
	};

	MAKE_ENUM_INFO(EquipmentStatsType, 3);


	enum EclStatusFlags : uint8_t
	{
		CSF_RequestDelete = 0x01,
		CSF_Started = 0x02,
		CSF_KeepAlive = 0x08,
		CSF_HasVisuals = 0x40
	};

	MAKE_ENUM_INFO(EclStatusFlags, 4);


	enum StatusFlags0 : uint8_t
	{
		SF0_KeepAlive = 1,
		SF0_IsOnSourceSurface = 2,
		SF0_IsFromItem = 4,
		SF0_Channeled = 8,
		SF0_IsLifeTimeSet = 0x10,
		SF0_InitiateCombat = 0x20,
		SF0_Influence = 0x80
	};

	MAKE_ENUM_INFO(StatusFlags0, 7);

	enum StatusFlags1 : uint8_t
	{
		SF1_BringIntoCombat = 1,
		SF1_IsHostileAct = 2,
		SF1_IsInvulnerable = 8,
		SF1_IsResistingDeath = 0x10
	};

	MAKE_ENUM_INFO(StatusFlags1, 4);

	enum StatusFlags2 : uint8_t
	{
		SF2_ForceStatus = 1,
		SF2_ForceFailStatus = 2,
		SF2_RequestDelete = 0x20,
		SF2_RequestDeleteAtTurnEnd = 0x40,
		SF2_Started = 0x80
	};

	MAKE_ENUM_INFO(StatusFlags2, 5);

	enum class StatusType : uint32_t
	{
		Hit = 1,
		Dying = 2,
		Heal = 3,
		Muted = 4,
		Charmed = 5,
		KnockedDown = 6,
		Summoning = 7,
		Healing = 8,
		Thrown = 9,
		TeleportFall = 0x0B,
		Consume = 0x0C,
		Combat = 0x0D,
		AoO = 0x0E,
		StoryFrozen = 0x0F,
		Sneaking = 0x10,
		Unlock = 0x11,
		Fear = 0x12,
		Boost = 0x13,
		Unsheathed = 0x14,
		Stance = 0x15,
		Sitting = 0x16,
		Lying = 0x17,
		Blind = 0x18,
		Smelly = 0x19,
		Clean = 0x1A,
		InfectiousDiseased = 0x1B,
		Invisible = 0x1C,
		Rotate = 0x1D,
		Encumbered = 0x1E,
		Identify = 0x1F,
		Repair = 0x20,
		Material = 0x21,
		Leadership = 0x22,
		Explode = 0x23,
		Adrenaline = 0x24,
		ShacklesOfPain = 0x25,
		ShacklesOfPainCaster = 0x26,
		WindWalker = 0x27,
		DarkAvenger = 0x28,
		Remorse = 0x29,
		DecayingTouch = 0x2A,
		Unhealable = 0x2B,
		Flanked = 0x2C,
		Channeling = 0x2D,
		Drain = 0x2E,
		LingeringWounds = 0x2F,
		Infused = 0x30,
		SpiritVision = 0x31,
		Spirit = 0x32,
		Damage = 0x33,
		ForceMove = 0x34,
		Climbing = 0x35,
		Incapacitated = 0x36,
		InSurface = 0x37,
		SourceMuted = 0x38,
		Overpowered = 0x39,
		Combustion = 0x3A,
		Polymorphed = 0x3B,
		DamageOnMove = 0x3C,
		DemonicBargain = 0x3D,
		GuardianAngel = 0x3E,
		Floating = 0x40,
		Challenge = 0x41,
		Disarmed = 0x42,
		HealSharing = 0x43,
		HealSharingCaster = 0x44,
		ExtraTurn = 0x45,
		ActiveDefense = 0x46,
		Spark = 0x47,
		PlayDead = 0x48,
		Constrained = 0x49,
		Effect = 0x4A,
		Deactivated = 0x4B,
		TutorialBed = 0x4C,
	};

	MAKE_ENUM_INFO(StatusType, 74);

	enum class CauseType : uint8_t
	{
		None = 0,
		SurfaceMove = 1,
		SurfaceCreate = 2,
		SurfaceStatus = 3,
		StatusEnter = 4,
		StatusTick = 5,
		Attack = 6,
		Offhand = 7,
		GM = 8
	};

	MAKE_ENUM_INFO(CauseType, 9);

	enum class DeathType : uint8_t
	{
		None = 0,
		Physical = 1,
		Piercing = 2,
		Arrow = 3,
		DoT = 4,
		Incinerate = 5,
		Acid = 6,
		Electrocution = 7,
		FrozenShatter = 8,
		PetrifiedShatter = 9,
		Explode = 10,
		Surrender = 11,
		Hang = 12,
		KnockedDown = 13,
		Lifetime = 14,
		Sulfur = 15,
		Sentinel = 16
	};

	MAKE_ENUM_INFO(DeathType, 17);

	enum class DamageType : uint32_t
	{
		None = 0,
		Physical = 1,
		Piercing = 2,
		Corrosive = 3,
		Magic = 4,
		Chaos = 5,
		Fire = 6,
		Air = 7,
		Water = 8,
		Earth = 9,
		Poison = 10,
		Shadow = 11,
		Sulfuric = 12,
		Sentinel = 13
	};

	MAKE_ENUM_INFO(DamageType, 14);

	enum class HitType : uint32_t
	{
		Melee = 0,
		Magic = 1,
		Ranged = 2,
		WeaponDamage = 3,
		Surface = 4,
		DoT = 5,
		Reflected = 6
	};

	MAKE_ENUM_INFO(HitType, 7);

	enum class ItemSlot : uint8_t
	{
		Helmet = 0,
		Breast = 1,
		Leggings = 2,
		Weapon = 3,
		Shield = 4,
		Ring = 5,
		Belt = 6,
		Boots = 7,
		Gloves = 8,
		Amulet = 9,
		Ring2 = 10,
		Wings = 11,
		Horns = 12,
		Overhead = 13,
		Sentinel = 14
	};

	MAKE_ENUM_INFO(ItemSlot, 15);

	enum StatCharacterFlags : uint8_t
	{
		SCF_IsPlayer = 1,
		SCF_InParty = 2,
		SCF_NotSneaking = 4,
		SCF_Invisible = 8,
		SCF_Blind = 16,
		SCF_DrinkedPotion = 32,
		SCF_EquipmentValidated = 64
	};

	MAKE_ENUM_INFO(StatCharacterFlags, 7);

	enum HitFlag : uint32_t
	{
		HF_Hit = 1,
		HF_Blocked = 2,
		HF_Dodged = 4,
		HF_Missed = 8,
		HF_CriticalHit = 0x10,
		HF_Backstab = 0x20,
		HF_FromSetHP = 0x40,
		HF_DontCreateBloodSurface = 0x80,
		HF_Reflection = 0x200,
		HF_NoDamageOnOwner = 0x400,
		HF_FromShacklesOfPain = 0x800,
		HF_DamagedMagicArmor = 0x1000,
		HF_DamagedPhysicalArmor = 0x2000,
		HF_DamagedVitality = 0x4000,
		HF_Flanking = 0x8000,
		HF_PropagatedFromOwner = 0x10000,
		HF_Surface = 0x20000,
		HF_DoT = 0x40000,
		HF_ProcWindWalker = 0x80000,
		HF_CounterAttack = 0x100000,
		HF_Poisoned = 0x200000,
		HF_Burning = 0x400000,
		HF_Bleeding = 0x800000,
		// Custom flags
		HF_NoEvents = 0x80000000
	};

	MAKE_ENUM_INFO(HitFlag, 24);

	enum StatusHealType : uint32_t
	{
		SHT_None = 0,
		SHT_Vitality = 1,
		SHT_PhysicalArmor = 2,
		SHT_MagicArmor = 3,
		SHT_AllArmor = 4,
		SHT_All = 5,
		SHT_Source = 6
	};

	MAKE_ENUM_INFO(StatusHealType, 7);

	enum HealEffect : uint32_t
	{
		HE_None = 0,
		HE_ResistDeath = 1,
		HE_Heal = 2,
		HE_Behavior = 3,
		HE_Unknown4 = 4,
		HE_Sitting = 5,
		HE_Surface = 6,
		HE_Lifesteal = 7,
		HE_NegativeDamage = 8,
		HE_Unknown9 = 9,
		HE_Script = 10,
		HE_HealSharing = 11,
		HE_Necromantic = 12,
		HE_HealSharingReflected = 13
	};

	MAKE_ENUM_INFO(HealEffect, 14);

	// Persistent flags: 0x2414170022110
	// AI flags: 0x4000070000060
	enum EsvCharacterFlags : uint64_t
	{
		CF_IsPlayer_M = 0x01, // ???
		CF_IsPlayer2_M = 0x02, // ???
		CF_HostControl = 0x08,
		CF_OffStage = 0x20,
		CF_Dead = 0x40,
		CF_Charmed = 0x80,
		CF_InDialog = 0x100, // ???
		CF_Summon = 0x200,
		CF_CharacterControl = 0x2000, // ???
		CF_Transformed_M = 0x4000, // ???
		CF_InArena = 0x10000,
		CF_Summon2 = 0x2000000,
		CF_PartyFollower = 0x400000000,
		CF_Totem = 0x2000000000
	};

	MAKE_ENUM_INFO(EsvCharacterFlags, 14);

	enum EsvCharacterFlags2 : uint8_t
	{
		CF_Global = 0x01,
		CF_HasOsirisDialog = 0x02,
		CF_HasDefaultDialog = 0x04,
		CF_TreasureGeneratedForTrader = 0x10,
		CF_Trader = 0x20,
		CF_Resurrected = 0x40
	};

	MAKE_ENUM_INFO(EsvCharacterFlags2, 6);

	enum EsvCharacterFlags3 : uint8_t
	{
		CF_IsPet = 0x01,
		CF_IsSpectating = 0x02,
		CF_NoReptuationEffects = 0x04,
		CF_HasWalkSpeedOverride = 0x08,
		CF_HasRunSpeedOverride = 0x10,
		CF_IsGameMaster = 0x20,
		CF_IsPossessed = 0x40
	};

	MAKE_ENUM_INFO(EsvCharacterFlags3, 7);

	enum class GameActionType
	{
		RainAction = 1,
		StormAction = 2,
		WallAction = 4,
		TornadoAction = 6,
		PathAction = 7,
		GameObjectMoveAction = 8,
		StatusDomeAction = 9
	};

	enum class SkillType : uint32_t
	{
		Zone = 0x1,
		SkillHeal = 0x2,
		Jump = 0x3,
		MultiStrike = 0x4,
		Path = 0x5,
		Projectile = 0x6,
		ProjectileStrike = 0x7,
		Quake = 0x8,
		Rain = 0x9,
		Rush = 0xA,
		Shout = 0xB,
		Storm = 0xC,
		Summon = 0xD,
		Target = 0xE,
		Teleportation = 0xF,
		Tornado = 0x10,
		Wall = 0x11,
		Dome = 0x12,
	};


	enum StatAttributeFlags : uint64_t
	{
		AF_FreezeImmunity = 1ull << 0,
		AF_BurnImmunity = 1ull << 1,
		AF_StunImmunity = 1ull << 2,
		AF_PoisonImmunity = 1ull << 3,
		AF_CharmImmunity = 1ull << 4,
		AF_FearImmunity = 1ull << 5,
		AF_KnockdownImmunity = 1ull << 6,
		AF_MuteImmunity = 1ull << 7,
		AF_ChilledImmunity = 1ull << 8,
		AF_WarmImmunity = 1ull << 9,
		AF_WetImmunity = 1ull << 10,
		AF_BleedingImmunity = 1ull << 11,
		AF_CrippledImmunity = 1ull << 12,
		AF_BlindImmunity = 1ull << 13,
		AF_CursedImmunity = 1ull << 14,
		AF_WeakImmunity = 1ull << 15,
		AF_SlowedImmunity = 1ull << 16,
		AF_DiseasedImmunity = 1ull << 17,
		AF_InfectiousDiseasedImmunity = 1ull << 18,
		AF_PetrifiedImmunity = 1ull << 19,
		AF_DrunkImmunity = 1ull << 20,
		AF_SlippingImmunity = 1ull << 21,
		AF_FreezeContact = 1ull << 22,
		AF_BurnContact = 1ull << 23,
		AF_StunContact = 1ull << 24,
		AF_PoisonContact = 1ull << 25,
		AF_ChillContact = 1ull << 26,
		AF_Torch = 1ull << 27,
		AF_Arrow = 1ull << 28,
		AF_Unbreakable = 1ull << 29,
		AF_Unrepairable = 1ull << 30,
		AF_Unstorable = 1ull << 31,
		AF_Grounded = 1ull << 32,
		AF_HastedImmunity = 1ull << 33,
		AF_TauntedImmunity = 1ull << 34,
		AF_SleepingImmunity = 1ull << 35,
		AF_AcidImmunity = 1ull << 36,
		AF_SuffocatingImmunity = 1ull << 37,
		AF_RegeneratingImmunity = 1ull << 38,
		AF_DisarmedImmunity = 1ull << 39,
		AF_DecayingImmunity = 1ull << 40,
		AF_ClairvoyantImmunity = 1ull << 41,
		AF_EnragedImmunity = 1ull << 42,
		AF_BlessedImmunity = 1ull << 43,
		AF_ProtectFromSummon = 1ull << 44,
		AF_Floating = 1ull << 45,
		AF_DeflectProjectiles = 1ull << 46,
		AF_IgnoreClouds = 1ull << 47,
		AF_MadnessImmunity = 1ull << 48,
		AF_ChickenImmunity = 1ull << 49,
		AF_IgnoreCursedOil = 1ull << 50,
		AF_ShockedImmunity = 1ull << 51,
		AF_WebImmunity = 1ull << 52,
		AF_LootableWhenEquipped = 1ull << 53,
		AF_PickpocketableWhenEquipped = 1ull << 54,
		AF_LoseDurabilityOnCharacterHit = 1ull << 55,
		AF_EntangledContact = 1ull << 56,
		AF_ShacklesOfPainImmunity = 1ull << 57,
		AF_MagicalSulfur = 1ull << 58,
		AF_ThrownImmunity = 1ull << 59,
		AF_InvisibilityImmunity = 1ull << 60
	};

	MAKE_ENUM_INFO(StatAttributeFlags, 61);


	enum TalentType
	{
		TALENT_None = 0,
		TALENT_ItemMovement = 1,
		TALENT_ItemCreation = 2,
		TALENT_Flanking = 3,
		TALENT_AttackOfOpportunity = 4,
		TALENT_Backstab = 5,
		TALENT_Trade = 6,
		TALENT_Lockpick = 7,
		TALENT_ChanceToHitRanged = 8,
		TALENT_ChanceToHitMelee = 9,
		TALENT_Damage = 10,
		TALENT_ActionPoints = 11,
		TALENT_ActionPoints2 = 12,
		TALENT_Criticals = 13,
		TALENT_IncreasedArmor = 14,
		TALENT_Sight = 15,
		TALENT_ResistFear = 16,
		TALENT_ResistKnockdown = 17,
		TALENT_ResistStun = 18,
		TALENT_ResistPoison = 19,
		TALENT_ResistSilence = 20,
		TALENT_ResistDead = 21,
		TALENT_Carry = 22,
		TALENT_Throwing = 23,
		TALENT_Repair = 24,
		TALENT_ExpGain = 25,
		TALENT_ExtraStatPoints = 26,
		TALENT_ExtraSkillPoints = 27,
		TALENT_Durability = 28,
		TALENT_Awareness = 29,
		TALENT_Vitality = 30,
		TALENT_FireSpells = 31,
		TALENT_WaterSpells = 32,
		TALENT_AirSpells = 33,
		TALENT_EarthSpells = 34,
		TALENT_Charm = 35,
		TALENT_Intimidate = 36,
		TALENT_Reason = 37,
		TALENT_Luck = 38,
		TALENT_Initiative = 39,
		TALENT_InventoryAccess = 40,
		TALENT_AvoidDetection = 41,
		TALENT_AnimalEmpathy = 42,
		TALENT_Escapist = 43,
		TALENT_StandYourGround = 44,
		TALENT_SurpriseAttack = 45,
		TALENT_LightStep = 46,
		TALENT_ResurrectToFullHealth = 47,
		TALENT_Scientist = 48,
		TALENT_Raistlin = 49,
		TALENT_MrKnowItAll = 50,
		TALENT_WhatARush = 51,
		TALENT_FaroutDude = 52,
		TALENT_Leech = 53,
		TALENT_ElementalAffinity = 54,
		TALENT_FiveStarRestaurant = 55,
		TALENT_Bully = 56,
		TALENT_ElementalRanger = 57,
		TALENT_LightningRod = 58,
		TALENT_Politician = 59,
		TALENT_WeatherProof = 60,
		TALENT_LoneWolf = 61,
		TALENT_Zombie = 62,
		TALENT_Demon = 63,
		TALENT_IceKing = 64,
		TALENT_Courageous = 65,
		TALENT_GoldenMage = 66,
		TALENT_WalkItOff = 67,
		TALENT_FolkDancer = 68,
		TALENT_SpillNoBlood = 69,
		TALENT_Stench = 70,
		TALENT_Kickstarter = 71,
		TALENT_WarriorLoreNaturalArmor = 72,
		TALENT_WarriorLoreNaturalHealth = 73,
		TALENT_WarriorLoreNaturalResistance = 74,
		TALENT_RangerLoreArrowRecover = 75,
		TALENT_RangerLoreEvasionBonus = 76,
		TALENT_RangerLoreRangedAPBonus = 77,
		TALENT_RogueLoreDaggerAPBonus = 78,
		TALENT_RogueLoreDaggerBackStab = 79,
		TALENT_RogueLoreMovementBonus = 80,
		TALENT_RogueLoreHoldResistance = 81,
		TALENT_NoAttackOfOpportunity = 82,
		TALENT_WarriorLoreGrenadeRange = 83,
		TALENT_RogueLoreGrenadePrecision = 84,
		TALENT_WandCharge = 85,
		TALENT_DualWieldingDodging = 86,
		TALENT_Human_Inventive = 87,
		TALENT_Human_Civil = 88,
		TALENT_Elf_Lore = 89,
		TALENT_Elf_CorpseEating = 90,
		TALENT_Dwarf_Sturdy = 91,
		TALENT_Dwarf_Sneaking = 92,
		TALENT_Lizard_Resistance = 93,
		TALENT_Lizard_Persuasion = 94,
		TALENT_Perfectionist = 95,
		TALENT_Executioner = 96,
		TALENT_ViolentMagic = 97,
		TALENT_QuickStep = 98,
		TALENT_Quest_SpidersKiss_Str = 99,
		TALENT_Quest_SpidersKiss_Int = 100,
		TALENT_Quest_SpidersKiss_Per = 101,
		TALENT_Quest_SpidersKiss_Null = 102,
		TALENT_Memory = 103,
		TALENT_Quest_TradeSecrets = 104,
		TALENT_Quest_GhostTree = 105,
		TALENT_BeastMaster = 106,
		TALENT_LivingArmor = 107,
		TALENT_Torturer = 108,
		TALENT_Ambidextrous = 109,
		TALENT_Unstable = 110,
		TALENT_ResurrectExtraHealth = 111,
		TALENT_NaturalConductor = 112,
		TALENT_Quest_Rooted = 113,
		TALENT_PainDrinker = 114,
		TALENT_DeathfogResistant = 115,
		TALENT_Sourcerer = 116,
		TALENT_Rager = 117,
		TALENT_Elementalist = 118,
		TALENT_Sadist = 119,
		TALENT_Haymaker = 120,
		TALENT_Gladiator = 121,
		TALENT_Indomitable = 122,
		TALENT_WildMag = 123,
		TALENT_Jitterbug = 124,
		TALENT_Soulcatcher = 125,
		TALENT_MasterThief = 126,
		TALENT_GreedyVessel = 127,
		TALENT_MagicCycles = 128,
		TALENT_Max = TALENT_MagicCycles,
	};

	MAKE_ENUM_INFO(TalentType, 129);


	enum ESurfaceFlag : uint64_t
	{
		// --- BASE TYPE START
		SurfaceFlag_Fire = 0x1000000,
		SurfaceFlag_Water = 0x2000000,
		SurfaceFlag_Blood = 0x4000000,
		SurfaceFlag_Poison = 0x8000000,
		SurfaceFlag_Oil = 0x10000000,
		SurfaceFlag_Lava = 0x20000000,
		SurfaceFlag_Source = 0x40000000,
		SurfaceFlag_Web = 0x80000000,
		SurfaceFlag_Deepwater = 0x100000000,
		SurfaceFlag_Sulfurium = 0x200000000,
		// UNUSED = 0x400000000,
		// --- BASE TYPE END


		// --- CLOUD TYPE START
		SurfaceFlag_FireCloud = 0x800000000,
		SurfaceFlag_WaterCloud = 0x1000000000,
		SurfaceFlag_BloodCloud = 0x2000000000,
		SurfaceFlag_PoisonCloud = 0x4000000000,
		SurfaceFlag_SmokeCloud = 0x8000000000,
		SurfaceFlag_ExplosionCloud = 0x10000000000,
		SurfaceFlag_FrostCloud = 0x20000000000,
		SurfaceFlag_Deathfog = 0x40000000000,
		SurfaceFlag_ShockwaveCloud = 0x80000000000,
		// UNUSED = 0x100000000000,
		// UNUSED = 0x200000000000,
		// --- CLOUD TYPE END

		SurfaceFlag_Blessed = 0x400000000000,
		SurfaceFlag_Cursed = 0x800000000000,
		SurfaceFlag_Purified = 0x1000000000000,
		// SurfaceFlag_??? = 0x2000000000000,

		// --- SURFACE ADJ START
		SurfaceFlag_CloudBlessed = 0x4000000000000,
		SurfaceFlag_CloudCursed = 0x8000000000000,
		SurfaceFlag_CloudPurified = 0x10000000000000,
		// UNUSED = 0x20000000000000,
		// --- SURFACE ADJ END

		// --- GROUND MOD START
		SurfaceFlag_Electrified = 0x40000000000000,
		SurfaceFlag_Frozen = 0x80000000000000,
		SurfaceFlag_CloudElectrified = 0x100000000000000,
		// --- GROUND MOD END

		// --- CLOUD MOD START
		SurfaceFlag_ElectrifiedDecay = 0x200000000000000,
		SurfaceFlag_SomeDecay = 0x400000000000000,
		// UNUSED = 0x800000000000000,
		// --- CLOUD MOD END
	};
}
