#pragma once

#include <GameDefinitions/Base/Base.h>

BEGIN_SE()

struct SceneNode
{
	void* VMT;
	std::array<SceneNode*, 4> ChildNodes;
	ObjectSet<void*> NodeObjectDescriptions;
	SceneNode* ParentNode;
	float OuterSize;
	float InnerSize;
	int Depth;
	uint8_t field_54;
	uint8_t field_55;
	Bound StrictWorldBound;
	Bound LooseWorldBound;
};


struct Scene
{
	SceneNode* RootNode;
	void* InstanceManager;
	void* PaintedDecalsManager;
	CRITICAL_SECTION CriticalSection;
	ObjectSet<MoveableObject*> Objects;
	bool IsActive;
	void* field_68;
	void* SceneViewMode;
	Level* Level;
	ComponentHandle Handle;
	uint64_t SRWKernelLock;
};


struct LevelMetaData : public ProtectedGameObject<LevelMetaData>
{
	void* VMT;
	Bound Bounds;
	FixedString LevelTemplateBoundTrigger;
	FixedString AtmosphereUUID;
	bool VisualStreamingEnabled;
};

struct LevelDesc : public ProtectedGameObject<LevelDesc>
{
	void* VMT;
	FixedString LevelName;
	FixedString UniqueKey;
	STDWString CustomDisplayLevelName;
	ObjectSet<Path> Paths;
	uint8_t Type;
};

struct ActivePersistentLevelTemplate
{
	LevelTemplate* Template;
	AreaTrigger* Area;
};

struct Level : public ProtectedGameObject<Level>
{
	virtual ~Level() = 0;
	virtual bool Load() = 0;
	virtual bool Instantiate() = 0;
	virtual bool Unload() = 0;
	virtual bool Activate() = 0;
	virtual void Update(GameTime const& time) = 0;
	virtual void Pick(void* ray, void* pickResult, bool) = 0;
	virtual LevelMetaData* GetMetaData() = 0;
	virtual void* GetEntityManager() = 0;

	LevelDesc* LevelDesc;
	LocalTemplateManager* LocalTemplateManager;
	char Unknown;
	void* DummyManager;
	void* ReferenceManager;
	Scene* Scene;
	PhysicsScene* PhysicsScene;
	ObjectSet<IGameObject *> GameObjects;
	void* Text3DManager;
	ObjectSet<ActivePersistentLevelTemplate> ActivePersistentLevelTemplates;
};

END_SE()

BEGIN_NS(esv)

struct EnvironmentalInfluences
{
	struct Status
	{
		ComponentHandle Handle;
		bool FirstAttempt;
		bool IsForced;
	};

	struct PermanentInfluence
	{
		float Strength;
		float WeatherStrength;
	};

	struct TemporaryStatus
	{
		ComponentHandle Handle;
		float Strength;
		float WeatherStrength;
		float LifeTime;
		bool IsForced;
	};

	ObjectSet<FixedString> Unknown;
	ComponentHandle OwnerHandle;
	RefMap<FixedString, PermanentInfluence> PermanentInfluences;
	RefMap<FixedString, TemporaryStatus> TemporaryStatuses;
	RefMap<FixedString, Status> Statuses;
	bool HasWeatherProofTalent;
};


struct EnvironmentalStatusManager : public ProtectedGameObject<EnvironmentalStatusManager>
{
	void* VMT;
	__int64 field_8;
	float Timer;
	int field_14;
	Map<ComponentHandle, EnvironmentalInfluences*> EnvironmentalInfluences;
	ObjectSet<ComponentHandle> UnknownHandles;
};


struct SightManager : public BaseComponentProcessingSystem<EntityWorld>
{
	// TODO - this has a composite (2 handles + a flag) key!
	Map<ComponentHandle, ComponentHandle> CharactersInSight;
	ObjectSet<ComponentHandle> GenerateSightEventsFor;
	eoc::VisionGrid* VisionGrid;
	eoc::AiGrid* AiGrid;
};


struct Level : public dse::Level
{
	struct TemplateTrace
	{
		FixedString Type;
		FixedString Id;
	};

	void* LayerManager;
#if !defined(OSI_EOCAPP)
	void * field_90;
#endif
	EntityManager * EntityManager;
	eoc::AiGrid * AiGrid;
	eoc::VisionGrid* VisionGrid;
	SightManager* SightManager;
	CharacterManager* CharacterManager;
	ItemManager* ItemManager;
	ProjectileManager* ProjectileManager;
	SurfaceManager * SurfaceManager;
	GameActionManager * GameActionManager;
	EffectManager * EffectManager;
	void * NoiseManager;
	ShroudManager* ShroudManager;
	void * field_F8;
	EnvironmentalStatusManager* EnvironmentalStatusManager;
	ObjectSet<void *> TemplateTypeDescs;
	LevelCacheTemplateManager* LevelCacheTemplateManager;
	ObjectSet<TemplateTrace> TemplateTraces;
};

struct LevelManager : public ProtectedGameObject<LevelManager>
{
	void * VMT;
#if defined(OSI_EOCAPP)
	uint64_t Unknown[15];
#endif
	Level * CurrentLevel;
	Map<FixedString, Level *> Levels;
	ObjectSet<LevelDesc *> LevelDescs;
	Map<FixedString, Level *> Levels2;
	Map<FixedString, char> FSMapChar;
};

END_NS()

BEGIN_NS(ecl)

struct Level : public dse::Level
{
	void* LevelAllocator;
	eoc::AiGrid* AiGrid;
	eoc::VisionGrid* VisionGrid;
	void* MaterialGrid;
	void* CharacterManager;
	void* GameMasterManager;
	void* ItemManager;
	void* TerrainManager;
	void* WallManager;
	void* ProjectileManager;
	void* GameActionManager;
	void* FadeManager;
	PickingHelperManager* PickingHelperManager;
	SurfaceManager* SurfaceManager;
	void* AtmosphereManager;
	ShroudManager* ShroudManager;
	void* SeeThroughManager;
	void* field_118;
	void* BulletDepThreadedUpdateManager;
	void* VisionDepThreadedUpdateManager;
	void* FootEffectManager;
	EntityManager* EntityManager;
	int field_140;
	char field_144;
	ObjectSet<void*> TemplateTypes; // ecl::Level::TemplateTypeDesc*
	LevelCacheTemplateManager* LevelCacheTemplateManager;
};


struct LevelManager : public ProtectedGameObject<LevelManager>
{
	void* VMT;
	Level* CurrentLevel;
	Map<FixedString, Level*> Levels;
	ObjectSet<LevelDesc*> LevelDescs;
	Map<FixedString, Level*> Levels2;
	Map<FixedString, char> FSMap_Char;
	uint64_t Unknown[17];
	ObjectSet<void*> OS_pResource; // Resource*
	ObjectSet<FixedString> OS_FS;
};

END_NS()
