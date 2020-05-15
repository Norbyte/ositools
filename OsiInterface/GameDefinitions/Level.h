#pragma once

#include "BaseTypes.h"

namespace dse
{
	namespace eoc
	{
		struct AiGrid;
	}

#pragma pack(push, 1)
	namespace esv
	{
		struct SurfaceManager;
		struct GameActionManager;

		struct LevelDesc
		{
			void * VMT;
			FixedString FS1;
			FixedString FS2;
			STDWString StdWStr;
			ObjectSet<void *> ObjSet_Path;
			char Unknown;
		};

		struct Level
		{
			void * VMT;
			LevelDesc * LevelDesc;
			void * LocalTemplateManager;
			char Unknown;
			uint8_t _Pad[7];
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
			void * EntityManager;
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
			uint8_t _Pad[4];
			ObjectSet<LevelDesc *> LevelDescs;
			Map<FixedString, Level *> Levels2;
			uint8_t _Pad2[4];
			Map<FixedString, char> FSMapChar;
		};

	}
#pragma pack(pop)

}
