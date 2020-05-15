#pragma once

#include "BaseTypes.h"
#include "Enumerations.h"

namespace dse
{
    struct IGameObject;
}

namespace dse::esv
{
    struct Surface;
}

namespace dse::eoc
{
#pragma pack(push, 1)
    struct AiGrid;

    struct Pool
    {
        int PoolSize;
        int MaxSize;
        char* PoolName;
    };

    struct AiGridPortal
    {
        uint64_t field_0;
        AiGrid* AiGrid;
        int Type0X;
        int Type0Z;
        int Type1X;
        int Type1Z;
        bool IsAttached;
        bool field_21;
        int16_t field_22;
        int field_24;
    };

    struct PathParams
    {
        uint64_t field_0;
        int GridRadius;
        float XZ[2];
        bool field_14;
        bool field_15;
        bool field_16;
        bool field_17;
        ObjectSet<int> SurfacePathInfluences; // Set<SurfacePathInfluence>
    };

    struct AiPath
    {
        bool field_0;
        bool PathfindingComplete;
        bool field_2;
        bool field_3;
        bool field_4;
        uint8_t _Pad1[3];
        int field_8;
        int field_C;
        int field_10;
        uint8_t _Pad2[4];
        uint64_t Flags;
        uint64_t Flags2;
        ObjectSet<int> SurfacePathInfluences; // Set<SurfacePathInfluence>
        int field_48;
        int field_4C;
        int field_50;
        uint8_t _Pad3[4];
        ObjectHandle AiHandle;
        ObjectHandle TargetAiHandle_M;
        PathParams* PathParams;
        float RadiusX;
        float field_74;
        float RadiusZ;
        float CloseEnoughDistMax;
        float CloseEnoughDistMin;
        int TargetCheckType;
        float TelekinesisRange;
        float StartingPosition[2];
        float TargetPosition[2];
        float AiBounds_M[3];
        float Height_M;
        int Horizon;
        int field_B0;
        int field_B4;
        bool field_B8;
        bool field_B9;
        uint8_t _Pad4[6];
        void* VisionGrid;
        int NetID;
        uint8_t _Pad5[4];
        ObjectSet<void*> Checkpoints; // AiPathCheckpoint*
        ObjectSet<void*> Checkpoints2; // AiPathCheckpoint*
        ObjectSet<bool> UnknownSet;
        ObjectSet<ObjectHandle> IgnoreCollision;
        float FindPathStartPosition[2];
    };

    struct Ai
    {
        AiGrid* AiGrid;
        void* VisionGrid;
        IGameObject* GameObject;
        int AiBoundType;
        float AIBoundsHeight;
        float AIBoundsHeight2;
        float AIBoundsRadius;
        float AIBoundsMin[3];
        float AIBoundsMax[3];
        float AIBoundsSize;
        uint16_t MetaDataIndex;
        char field_46;
        uint8_t _Pad0[1];
        float XZ[2];
        float Rotation[9];
        uint8_t _Pad1[4];
        uint16_t AiCollidingRefCount;
        uint16_t Settings;
        uint8_t _Pad2[4];
        uint64_t AiFlags;
        uint16_t AiFlags2;
        uint16_t AiFlags2Original;
        char field_8C;
        uint8_t _Pad3[3];
        float XZ2[2];
        bool UseOnDistance;
        uint8_t _Pad4[7];
        ObjectHandle MyHandle;
    };

    struct AiMetaData
    {
        ObjectSet<Ai*> Ai;
        ObjectSet<AiGridPortal*> Portals;
        ObjectSet<AiGridPortal*> Portals2;
        uint16_t X;
        uint16_t Z;
        int field_64;
    };

    struct AiSurfaceMetaData
    {
        int16_t BaseSurfaceIndex;
        int16_t CloudSurfaceIndex;
        uint16_t X;
        uint16_t Z;
    };

    struct AiGridTile
    {
        static constexpr uint64_t BaseSurfaceLayerMask  = 0x01C3C007FF000000ull;
        static constexpr uint64_t CloudSurfaceLayerMask = 0x0E3C3FF800000000ull;

        uint64_t AiFlags;
        int16_t Height;
        int16_t AiMetaDataIndex;
        int16_t SurfaceIndexAndMeta;
        int16_t field_E;
    };


    struct AiGridDataTiles
    {
        uint64_t field_0;
        int Width;
        int Height;
        uint64_t field_10;
        AiGridTile* Tiles;
        AiGridTile* TilesEnd;

        AiGridTile* Get(glm::i32vec2 pos) const
        {
            if (pos.x < 0 || pos.x > Width || pos.y < 0 || pos.y > Height) {
                return nullptr;
            }

            return &Tiles[pos.x + pos.y * Width];
        }
    };

    struct DataGrid
    {
        void* VMT;
        float OffsetX;
        float OffsetY;
        float OffsetZ;
        float GridScale;
        int Width;
        int Height;

        inline glm::i32vec2 eoc::DataGrid::PositionToCellPosition(glm::vec2 pos) const
        {
            return glm::i32vec2(
                (int)((pos.x - OffsetX) / GridScale),
                (int)((pos.y - OffsetZ) / GridScale)
            );
        }
    };

    struct AiGrid
    {
        void* VMT;
        uint64_t field_8;
        DataGrid DataGrid;
        int field_30;
        int field_34;
        uint64_t field_38;
        AiGridDataTiles* Tiles;
        ObjectSet<AiMetaData*> AiMetaData;
        Pool AiMetaDataPool;
        ObjectSet<AiSurfaceMetaData*> Surfaces;
        Pool SurfacePool;
        ObjectSet<Ai*> Ai;
        ObjectSet<AiGridPortal*> Portals;
        void* AiFactory;
        int NextPathId;
        int field_F4;
        ObjectSet<AiPath*> Paths;
        RefMap<uint32_t, AiPath*> PathMap;
        ObjectSet<AiPath*> Paths2;
        bool field_148;
        bool field_149;
        bool field_14A;
        bool field_14B;
        int field_14C;
        ObjectSet<void*> FullTiles; // AiFullTile*
        uint64_t Unknown[12];
        ObjectSet<void*> FloodTiles; // AiFloodTile*
        uint64_t field_1F0;
        RefMap<uint32_t, void*> field_1F8;
        uint64_t field_208;
        uint64_t field_210;
        ObjectSet<uint64_t> Flags;
        uint64_t field_238;
        uint64_t field_240;
        ObjectSet<void*> Listeners;
        int field_268;
        int field_26C;
        int field_270;
        int field_274;
        uint64_t field_278;
        uint64_t field_280;
        uint64_t field_288;
        uint64_t field_290;
        uint64_t field_298;
        bool field_2A0;
        bool field_2A1;
        bool field_2A2;
        bool AiObjectsChanged;
        bool field_2A4;
        bool field_2A5;
        bool field_2A6;
        bool field_2A7;
        int FileVersion;
        float HighGroundMeleeRange;
        void* NavigationPortals;
        uint64_t field_2B8;
        ObjectSet<glm::i32vec2> UnknVectors;
        uint64_t field_2E0;
        uint64_t field_2E8;

        inline AiGridTile* GetCell(glm::vec2 pos) const
        {
            auto cellPos = DataGrid.PositionToCellPosition(pos);
            return Tiles->Get(cellPos);
        }

        int16_t GetSurfaceIndex(AiGridTile* tile, uint8_t layer) const;
        eoc::AiMetaData* GetAiMetaData(AiGridTile* tile) const;
    };



#pragma pack(pop)
}
