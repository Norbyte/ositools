#pragma once

#include "BaseTypes.h"

namespace dse
{
	namespace eoc
	{
		struct AiGrid;
	}

	namespace esv
	{
		struct SurfaceManager;
		struct GameActionManager;

		struct LevelDesc
		{
			void * VMT;
			FixedString LevelName;
			FixedString UniqueKey;
			STDWString CustomDisplayLevelName;
			ObjectSet<Path> Paths;
			uint8_t Type;
		};

		struct Level
		{
			void * VMT;
			LevelDesc * LevelDesc;
			void * LocalTemplateManager;
			char Unknown;
			void * DummyManager;
			void * ReferenceManager;
			void * Scene;
			void * PhysicsScene;
			ObjectSet<void *> GameObjects;
			void * Text3DManager;
			ObjectSet<void *> ActivePersistentLevelTemplates;
			void * LayerManager;
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
			void * ShroudManager;
			void * field_F8;
			void * EnvironmentalStatusManager;
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

}
