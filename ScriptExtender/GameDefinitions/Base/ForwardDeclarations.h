#pragma once

struct lua_State;

BEGIN_SE()

struct PropertyMapBase;

struct GameObjectTemplate;
struct EoCGameObjectTemplate;
struct CombatComponentTemplate;
struct SoundVolumeTriggerData;
struct CharacterTemplate;
struct ItemTemplate;
struct ProjectileTemplate;

struct IGameObject;
struct Trigger;

struct ModuleInfo;
struct Module;
struct ModManager;

struct ObjectVisitor;
struct FileReader;
struct TranslatedStringRepository;

struct CRPGStatsManager;
struct SkillPrototype;
struct DamagePairList;
struct HitDamageInfo;

namespace net
{
	struct GameServer;
	struct Client;
	struct AbstractPeer;
	struct Message;
	struct MessageFactory;
	struct BitstreamSerializer;
}

namespace eoc
{
	struct CombatComponent;
	struct CustomStatsComponent;
	struct Ai;
	struct AiGrid;
	struct NetworkFixedStrings;
}

namespace esv
{
	struct CustomStatDefinitionComponent;
	struct CustomStatSystem;
	struct NetComponent;
	struct Item;
	struct Character;
	struct Inventory;
	struct TurnManager;
	struct Projectile;
	struct Surface;
	struct SurfaceAction;
	struct SurfaceManager;
	struct Level;
	struct Status;

	struct SurfaceManager;
	struct GameActionManager;
	struct ShroudManager;
	struct EntityManager;
	struct CombineManager;

	struct ItemFactory;
	struct CharacterFactory;

	struct EntityWorld;
}


namespace ecl
{
	struct Item;
	struct Character;
	struct Inventory;
	struct Level;
	struct Status;

	struct EntityManager;
	struct ShroudManager;
	struct PickingHelper;
	struct PickingHelperManager;

	struct EoCUI;
}

namespace ig
{
	struct InvokeDataValue;
	struct IggyValuePath;
	struct IggyDataValue;
	struct FlashObject;
	struct FlashPlayer;
	struct IggyBinding;
}

struct UIObject;
struct CustomDrawStruct;
struct UIObjectManager;

// Forward declarations for custom Lua serializers
namespace lua
{
	class LifetimeHolder;
}

END_SE()