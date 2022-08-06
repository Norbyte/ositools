#pragma once

struct lua_State;

namespace Json
{
	class Value;
}

BEGIN_SE()

struct LegacyPropertyMapBase;

struct GameObjectTemplate;
struct EoCGameObjectTemplate;
struct CombatComponentTemplate;
struct SoundVolumeTriggerData;
struct CharacterTemplate;
struct ItemTemplate;
struct ProjectileTemplate;
struct SurfaceTemplate;
struct TriggerTemplate;
struct LevelTemplate;

struct IGameObject;
struct IEoCServerObject;
struct IEoCClientObject;
struct Trigger;
struct Level;

struct ModuleInfo;
struct Module;
struct ModManager;

struct ObjectVisitor;
struct FileReader;
struct TranslatedStringRepository;

struct GlobalTemplateManager;
struct LocalTemplateManager;
struct GlobalCacheTemplateManager;
struct LevelCacheTemplateManager;

struct PhysicsObject;
struct PhysicsShape;
struct PhysicsRagdoll;

struct Skeleton;
struct Scene;
struct MoveableObject;
struct Visual;
struct RenderableObject;

struct Material;
struct AppliedMaterial;

struct EoCSoundManager;

struct InputManager;
struct InputValue;
struct InputEvent;
struct InjectInputData;

struct SkillAIParams;

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
	struct CharacterManager;
	struct ItemManager;
	struct ProjectileManager;
	struct EffectManager;
	struct ShroudManager;
	struct EntityManager;
	struct CombineManager;

	struct ItemFactory;
	struct CharacterFactory;

	struct EntityWorld;

	struct PendingHit;
	class ExtensionState;

	struct AiModifiers;
	struct AiHelpers;
	struct AiRequest;
	struct AiAction;
}


namespace ecl
{
	struct Item;
	struct Character;
	struct Inventory;
	struct Level;
	struct Status;
	struct StatusMachine;
	struct Scenery;

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
	struct LifetimeHandle;
	struct CppObjectMetadata;
}

END_SE()