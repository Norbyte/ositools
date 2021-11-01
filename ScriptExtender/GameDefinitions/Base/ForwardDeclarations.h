#pragma once

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

	struct EntityManager;
	struct ShroudManager;
	struct PickingHelperManager;
}


END_SE()