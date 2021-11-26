#pragma once

struct lua_State;

namespace Json
{
	class Value;
}

BEGIN_SE()

struct PropertyMapBase;

struct GameObjectTemplate;
struct EoCGameObjectTemplate;
struct CombatComponentTemplate;
struct SoundVolumeTriggerData;
struct CharacterTemplate;
struct ItemTemplate;
struct ProjectileTemplate;
struct SurfaceTemplate;
struct TriggerTemplate;

struct IGameObjectBase;
struct IGameObject;
struct Trigger;

struct ModuleInfo;
struct Module;
struct ModManager;

struct ObjectVisitor;
struct FileReader;
struct TranslatedStringRepository;

namespace stats
{
	struct RPGStats;
	struct SkillPrototype;
	struct DamagePairList;
	struct HitDamageInfo;
	struct Object;
	struct Character;
	struct Item;
}

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
	struct Text;
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
	struct StatusMachine;
	struct Trigger;

	struct SurfaceManager;
	struct GameActionManager;
	struct ShroudManager;
	struct EntityManager;
	struct CombineManager;

	struct ItemFactory;
	struct CharacterFactory;

	struct EntityWorld;

	struct PendingHit;
	class ExtensionState;
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
	class ExtensionState;
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

class OsirisExtender;
class ScriptExtender;

// Forward declarations for custom Lua serializers
namespace lua
{
	class LifetimeHolder;
}

END_SE()