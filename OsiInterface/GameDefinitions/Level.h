#pragma once

#include "BaseTypes.h"

namespace dse
{
	namespace eoc
	{
		struct AiGrid;
	}

	struct LevelDesc
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
		void* LocalTemplateManager;
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
		struct SurfaceManager;
		struct GameActionManager;
		struct ShroudManager;

		struct EnvironmentalInfluences
		{
			struct Status
			{
				ObjectHandle Handle;
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
				ObjectHandle Handle;
				float Strength;
				float WeatherStrength;
				float LifeTime;
				bool IsForced;
			};

			ObjectSet<FixedString> Unknown;
			ObjectHandle OwnerHandle;
			Map<FixedString, PermanentInfluence> PermanetInfluences;
			Map<FixedString, TemporaryStatus> TemporaryStatuses;
			Map<FixedString, Status> Statuses;
			bool HasWeatherProofTalent;
		};


		struct EnvironmentalStatusManager
		{
			void* VMT;
			__int64 field_8;
			float Timer;
			int field_14;
			Map<ObjectHandle, EnvironmentalInfluences> EnvironmentalInfluences;
			ObjectSet<ObjectHandle> UnknownHandles;
		};


		struct Level : public dse::Level
		{
			void* LayerManager;
#if !defined(OSI_EOCAPP)
			void * field_90;
#endif
			EntityManager * EntityManager;
			eoc::AiGrid * AiGrid;
			void * VisionGrid;
			void * SightManager;
			void * CharacterManager;
			void * ItemManager;
			void * ProjectileManager;
			SurfaceManager * SurfaceManager;
			GameActionManager * GameActionManager;
			void * EffectManager;
			void * NoiseManager;
			ShroudManager* ShroudManager;
			void * field_F8;
			EnvironmentalStatusManager* EnvironmentalStatusManager;
			ObjectSet<void *> TemplateTypeDescs;
			void * LevelCacheTemplateManager;
			ObjectSet<void *> TemplateTraceSet;
		};

		struct LevelManager
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
		struct ShroudManager;

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
			void* PickingHelperManager;
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
			__int64 LevelCacheTemplateManager;
		};


		struct LevelManager
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
