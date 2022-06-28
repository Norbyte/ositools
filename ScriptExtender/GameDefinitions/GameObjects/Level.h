#pragma once

#include <GameDefinitions/Base/Base.h>

namespace dse
{
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

	struct Level : public ProtectedGameObject<Level>
	{
		void* VMT;
		LevelDesc* LevelDesc;
		LocalTemplateManager* LocalTemplateManager;
		char Unknown;
		void* DummyManager;
		void* ReferenceManager;
		void* Scene;
		void* PhysicsScene;
		ObjectSet<void*> GameObjects;
		void* Text3DManager;
		ObjectSet<void*> ActivePersistentLevelTemplates;
	};

	namespace esv
	{

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
			Map<FixedString, PermanentInfluence> PermanetInfluences;
			Map<FixedString, TemporaryStatus> TemporaryStatuses;
			Map<FixedString, Status> Statuses;
			bool HasWeatherProofTalent;
		};


		struct EnvironmentalStatusManager : public ProtectedGameObject<EnvironmentalStatusManager>
		{
			void* VMT;
			__int64 field_8;
			float Timer;
			int field_14;
			Map<ComponentHandle, EnvironmentalInfluences> EnvironmentalInfluences;
			ObjectSet<ComponentHandle> UnknownHandles;
		};


		struct SightManager : public BaseComponentProcessingSystem<EntityWorld>
		{
			// TODO - this has a composite (2 handles + a flag) key!
			Map<ComponentHandle, ComponentHandle> CharactersInSight;
			ObjectSet<ComponentHandle> GenerateSightEventsFor;
			void* VisionGrid;
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
			void * VisionGrid;
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

	}

	namespace ecl
	{
		struct Level : public dse::Level
		{
			void* LevelAllocator;
			eoc::AiGrid* AiGrid;
			void* VisionGrid;
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
			void* SurfaceManager;
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

	}
}