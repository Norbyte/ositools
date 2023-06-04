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
struct WallConstructionTemplate;

struct IGameObject;
struct IEoCServerObject;
struct IEoCClientObject;
struct Trigger;
struct AreaTrigger;
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
struct PhysicsScene;

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
struct PlayerManager;

class ScriptExtenderMessage;

namespace aspk
{
	struct Effect;
	struct Component;
}

namespace fx
{
	struct FxEffectFactory;
	struct Effect;
}

namespace stats
{
	struct RPGStats;
	struct SkillPrototype;
	struct DamagePairList;
	struct HitDamageInfo;
	struct Object;
	struct Character;
	struct Item;
	struct Condition;
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
	struct VisionGrid;
	struct VisionGridVisual;
	struct Visibility;
	struct NetworkFixedStrings;
	struct Text;
	struct PathMover;
}

namespace character_creation
{
	struct CharacterCreationManager;
}

namespace esv
{
	struct MovementMachine;
	struct MovementState;
	struct ActionMachine;
	struct ActionState;
	struct BaseController;
	struct StatusController;
	struct Task;
	struct TaskController;

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
	struct AreaTriggerBase;
	struct PointTriggerBase;
	struct AIHintAreaTrigger;

	struct SurfaceManager;
	struct GameActionManager;
	struct CharacterManager;
	struct ItemManager;
	struct ProjectileManager;
	struct EffectManager;
	struct ShroudManager;
	struct EntityManager;
	struct CombineManager;
	struct LevelManager;
	struct AlignmentContainer;

	struct ItemFactory;
	struct CharacterFactory;
	struct ProjectileFactory;

	struct EntityWorld;

	struct SkillState;
	struct SkillStateFactory;
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
	struct Projectile;
	struct WallConstructionStruct;
	struct WallConstruction;
	struct WallIntersection;
	struct Wall;
	struct WallManager;

	struct Surface;
	struct SurfaceManager;

	struct EntityManager;
	struct ShroudManager;
	struct PickingHelper;
	struct PickingHelperManager;
	struct TurnManager;
	struct LevelManager;

	struct ProjectileFactory;

	struct GameCamera;

	struct InputController;

	struct PathPreviewer;
	struct SkillState;
	struct SkillStateFactory;
	struct MultiEffectHandler;
	struct BeamEffectHandler;

	struct MovementMachine;
	struct MovementState;
	struct ActionMachine;
	struct ActionState;
	struct BaseController;
	struct StatusController;
	struct DialogController;

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

namespace rf
{
	struct RendererCommandBuffer;

	struct Camera;
	struct CameraController;
	struct CameraManager;
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
	struct CppValueMetadata;
	struct CppObjectMetadata;
	class Ref;
	class UserspaceClassBase;
	class CachedUserVariableManager;
	class CachedModVariableManager;
}

END_SE()