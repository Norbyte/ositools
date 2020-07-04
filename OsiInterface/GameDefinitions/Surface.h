#pragma once

#include "BaseTypes.h"
#include "Enumerations.h"
#include "EntitySystem.h"

namespace dse
{
	namespace eoc
	{
		struct AiGrid;
	}

	namespace esv
	{
        struct Surface;
        struct SurfaceAction;
        struct SurfaceManager;
        struct Level;

        struct SurfaceCell
        {
            glm::i16vec2 Position;
        };

        struct SubSurface
        {
            Surface* Surface;
            uint64_t SurfaceStateFlags;
            CompactObjectSet<SurfaceCell> SurfaceCells1;
            CompactObjectSet<SurfaceCell> SurfaceCells2;
            uint16_t SurfaceIndex;
            uint8_t Position[2];
            int field_44;
        };


        struct Surface : ProtectedGameObject<Surface>
        {
            FixedString FS1;
            NetId NetID;
            ObjectHandle MyHandle;
            SurfaceType SurfaceType;
            uint8_t Flags;
            int field_20;
            int CombatComponentIdx;
            int TeamId;
            ObjectHandle OwnerHandle;
            float LifeTime;
            int8_t Level;
            uint8_t _Pad3[3];
            bool LifeTimeFromTemplate;
            float StatusChance;
            uint64_t PrimSetSurfaceCell[3];
            uint64_t TransformActionObjectHandles[14];
            uint16_t Index;
            SurfaceManager* SurfaceManager;
            bool NeedsSplitEvaluation;
            float OwnershipTimer;
            bool field_E8;
            ObjectSet<SubSurface*> SubSurfaces;
        };

        struct SurfaceCollectCellsFlood
        {
            void* VMT;
            __int64 field_8;
            __int64 field_10;
            __int64 field_18;
            __int64 field_20;
            __int64 field_28;
            PrimitiveSet<SurfaceCell> Cells;
        };

        struct SurfaceManager : public ObjectFactory<Surface, (uint32_t)ObjectType::Unknown>
        {
            BaseComponentProcessingSystem ComponentProcessingSystem;
            uint64_t field_98;
            ObjectSet<SurfaceAction*> SurfaceActions;
            Array<Surface*> Surfaces;
            uint64_t field_E0;
            PrimitiveSet<SurfaceCell> SurfaceCells;
            SurfaceCollectCellsFlood CollectCellsFlood;
            ObjectSet<PrimitiveSet<SurfaceCell>> SurfaceCellSetsByLayer;
            RefMap<uint64_t, uint64_t> field_158;
        };

        struct SurfaceAction : ProtectedGameObject<SurfaceAction>
        {
            void* VMT;
            Level* Level;
            ObjectHandle MyHandle;
        };

        struct CreateSurfaceActionBase : public SurfaceAction
        {
            ObjectHandle OwnerHandle; // Init param
            float Duration; // Init param
            float StatusChance; // Init param
            glm::vec3 Position; // Init param
            SurfaceType SurfaceType; // Init param
            ObjectHandle SurfaceHandlesByType[79];
            PrimitiveSet<SurfaceCell>* SurfaceChanges[79];
            PrimitiveSet<SurfaceCell>* SurfaceCellsByLayer[2];
        };

        struct CreateSurfaceAction : public CreateSurfaceActionBase
        {
            float Radius; // Init param
            float ExcludeRadius;
            float MaxHeight; // Init param
            bool IgnoreIrreplacableSurfaces; // Init param
            bool CheckExistingSurfaces;
            uint64_t SurfaceCollisionFlags;
            uint64_t SurfaceCollisionNotOnFlags;
            uint64_t LineCheckBlock; // Init param
            float Timer;
            float GrowTimer;
            int GrowStep;
            int CurrentCellCount;
            PrimitiveSet<SurfaceCell> SurfaceCells;
            uint32_t SurfaceLayer;
        };

        struct ChangeSurfaceOnPathAction : public CreateSurfaceActionBase
        {
            ObjectHandle OH;
            int Radius;
            bool IsFinished;
            PrimitiveSet<SurfaceCell> SurfaceCells;
            bool IgnoreIrreplacableSurfaces;
            bool CheckExistingSurfaces;
            uint32_t SurfaceCollisionFlags;
            uint32_t SurfaceCollisionNotOnFlags;
            bool IgnoreOwnerCells;
        };

        struct CreatePuddleAction : public CreateSurfaceActionBase
        {
            int SurfaceCells; // Init param
            int Step; // Init param
            float GrowSpeed; // Init param
            uint8_t _Pad1[4];
            bool IsFinished;
            bool IgnoreIrreplacableSurfaces; // Init param
            PrimitiveSet<SurfaceCell> CellAtGrow;
            PrimitiveSet<SurfaceCell> ClosedCells;
            float GrowTimer;
        };

        struct ExtinguishFireAction : public CreateSurfaceActionBase
        {
            glm::vec3 Position;
            int Radius;
            int Percentage;
            int GrowTimer;
            int Step;
            PrimitiveSet<SurfaceCell> field_558;
            PrimitiveSet<SurfaceCell> field_570;
        };

        struct TransformSurfaceAction : public SurfaceAction
        {
            float Timer;
            SurfaceTransformActionType SurfaceTransformAction; // Init param
            SurfaceType OriginSurface; // Init param
            int SurfaceLayer; // Init param
            float GrowCellPerSecond;
            bool Finished;
            void* CellSearcher;
            ObjectHandle OwnerHandle; // Init param
            glm::vec3 Position; // Init param
            float SurfaceLifetime; // Init param
            float SurfaceStatusChance; // Init param
            RefMap<SurfaceType, ObjectHandle> SurfaceMap;
            RefMap<SurfaceType, PrimitiveSet<SurfaceCell>> SurfaceCellMap;
            PrimitiveSet<SurfaceCell> SurfaceRemoveGroundCellMap;
            PrimitiveSet<SurfaceCell> SurfaceRemoveCloudCellMap;
        };

        struct RectangleSurfaceAction : public CreateSurfaceActionBase
        {
            glm::vec3 Target;
            uint8_t _Pad1[4];
            int field_548;
            float SurfaceArea_M;
            float Width;
            float Length;
            float GrowTimer;
            float MaxHeight;
            int GrowStep;
            uint64_t Flags;
            DamagePairList DamageList;
            bool field_590;
            uint64_t LineCheckBlock;
            uint64_t field_5A0;
            float CurrentGrowTimer;
            PrimitiveSet<SurfaceCell> SurfaceCells;
            ObjectSet<ObjectHandle> Characters;
            ObjectSet<ObjectHandle> Items;
            uint64_t CurrentCellCount;
        };

        struct PolygonSurfaceAction : public CreateSurfaceActionBase
        {
            FixedString field_538;
            ObjectSet<glm::vec2> PolygonVertices; // Init param
            int field_560;
            DamagePairList DamageList;
            char field_588;
            __int64 field_590;
            float CurrentGrowTimer;
            float GrowTimer; // Init param
            float PositionXZ[2];
            int GrowStep; // Init param
            int LastSurfaceCellCount;
            PrimitiveSet<SurfaceCell> SurfaceCells;
            ObjectSet<ObjectHandle> Characters;
            ObjectSet<ObjectHandle> Items;
            char field_608;
        };

        struct SwapSurfaceAction : public CreateSurfaceActionBase
        {
            float Radius;
            float ExcludeRadius;
            float MaxHeight;
            glm::vec3 Target;
            bool IgnoreIrreplacableSurfaces;
            bool CheckExistingSurfaces;
            uint64_t SurfaceCollisionFlags;
            uint64_t SurfaceCollisionNotOnFlags;
            uint64_t LineCheckBlock;
            float Timer;
            float GrowTimer;
            int GrowStep;
            int CurrentCellCount;
            PrimitiveSet<SurfaceCell> SurfaceCells;
            PrimitiveSet<SurfaceCell> TargetCells;
            RefMap<dse::SurfaceType, PrimitiveSet<SurfaceCell>> SurfaceCellMap;
            RefMap<dse::SurfaceType, PrimitiveSet<SurfaceCell>> TargetCellMap;
        };

        struct SurfaceActionFactory : public ObjectFactory<SurfaceAction, (uint32_t)ObjectType::ServerSurfaceAction>
        {};

	}

}
