#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>

namespace dse
{
    struct SurfacePathInfluence
    {
        ESurfaceFlag MatchFlags;
        int Influence;
        ESurfaceFlag MaskFlags;
    };
}

namespace dse::eoc
{
    struct AiPathCheckpoint
    {
        NetId NetID;
        float X;
        float Y;
    };

    struct AiGridPortal : public ProtectedGameObject<AiGridPortal>
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
        glm::vec2 XZ;
        bool AiGridFlag0x10;
        bool AvoidTraps;
        bool field_16;
        bool NoSurfaceEffects;
        ObjectSet<SurfacePathInfluence> SurfacePathInfluences;
    };

    struct AiPath : public ProtectedGameObject<AiPath>
    {
        bool field_0;
        bool PathfindingComplete;
        bool field_2;
        bool field_3;
        bool field_4;
        int field_8;
        int field_C;
        int field_10;
        uint64_t Flags;
        uint64_t Flags2;
        ObjectSet<SurfacePathInfluence> SurfacePathInfluences;
        int field_48;
        int field_4C;
        int field_50;
        ComponentHandle AiHandle;
        ComponentHandle TargetAiHandle_M;
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
        glm::vec3 AiBounds_M;
        float Height_M;
        int Horizon;
        int field_B0;
        int field_B4;
        bool field_B8;
        bool field_B9;
        eoc::VisionGrid* VisionGrid;
        int NetID;
        ObjectSet<void*> Checkpoints; // AiPathCheckpoint*
        ObjectSet<void*> Checkpoints2; // AiPathCheckpoint*
        ObjectSet<bool> UnknownSet;
        ObjectSet<ComponentHandle> IgnoreCollision;
        float FindPathStartPosition[2];
    };

    struct Ai : public ProtectedGameObject<Ai>
    {
        AiGrid* AiGrid;
        eoc::VisionGrid* VisionGrid;
        IGameObject* GameObject;
        int AiBoundType;
        float AIBoundsHeight;
        float AIBoundsHeight2;
        float AIBoundsRadius;
        glm::vec3 AIBoundsMin;
        glm::vec3 AIBoundsMax;
        float AIBoundsSize;
        uint16_t MetaDataIndex;
        char field_46;
        glm::vec2 XZ;
        glm::mat3 Rotation;
        uint16_t AiCollidingRefCount;
        uint16_t Settings;
        uint64_t AiFlags;
        uint16_t AiFlags2;
        uint16_t AiFlags2Original;
        char field_8C;
        float XZ2[2];
        bool UseOnDistance;
        ComponentHandle MyHandle;
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
        uint16_t BaseSurfaceIndex;
        uint16_t CloudSurfaceIndex;
        uint16_t X;
        uint16_t Z;
    };

    struct AiGridTile
    {
        static constexpr uint16_t InvalidIndex = 0xffff;

        static constexpr uint64_t BaseSurfaceLayerMask  = 0x01C3C007FF000000ull;
        static constexpr uint64_t CloudSurfaceLayerMask = 0x0E3C3FF800000000ull;

        uint64_t AiFlags;
        int16_t Height;
        uint16_t AiMetaDataIndex;
        uint16_t SurfaceIndexAndMeta;
        int16_t field_E;
    };


    struct AiGridDataTiles : public ProtectedGameObject<AiGridDataTiles>
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

    struct DataGrid : public ProtectedGameObject<DataGrid>
    {
        void* VMT;
        float OffsetX;
        float OffsetY;
        float OffsetZ;
        float GridScale;
        int Width;
        int Height;

        inline glm::i32vec2 PositionToCellPosition(glm::vec2 pos) const
        {
            return glm::i32vec2(
                (int)((pos.x - OffsetX) / GridScale),
                (int)((pos.y - OffsetZ) / GridScale)
            );
        }
    };

    struct AiGrid : public ProtectedGameObject<AiGrid>
    {
        using SearchForCellProc = bool (eoc::AiGrid* self, float posX, float posZ, float radius, ESurfaceFlag aiFlags, ObjectSet<SurfacePathInfluence>* pathInfluences, float aiBoundsBias);

        // Surface type indicator flags (Fire, Poison, etc.) -- see ESurfaceFlag enumeration
        static constexpr uint64_t SurfaceFlags = 0xFFFFFFFFF000000ull;
        // Base Ai flags (passability, etc.)
        static constexpr uint64_t BaseFlags = 0xFFFFFFull;

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

        uint16_t GetSurfaceIndex(AiGridTile* tile, uint8_t layer) const;
        eoc::AiMetaData* GetAiMetaData(AiGridTile* tile) const;

        bool SearchForCell(float x, float z, float radius, ESurfaceFlag flags, float bias);
        UserReturn GetCellInfo(lua_State* L, float x, float z);
        std::optional<float> GetHeight(float x, float z);
        void SetHeight(float x, float z, float height);
        std::optional<uint64_t> GetAiFlags(float x, float z);
        void SetAiFlags(float x, float z, uint64_t aiFlags);
        bool UpdateAiFlagsInRect(float minX, float minZ, float maxX, float maxZ, uint64_t setFlags, uint64_t clearFlags);
        ObjectSet<glm::vec2> FindCellsInRect(float minX, float minZ, float maxX, float maxZ, uint64_t anyFlags, uint64_t allFlags);
    };

    struct ShroudObject : public ProtectedGameObject<ShroudObject>
    {
        void* LevelAllocator;
        ShroudType ShroudTypeId;
        float Scale;
        uint8_t* DataBuffer;
        uint64_t DataBufferSize;
        int Width;
        int Height;
        int IsFloatData;
        char field_2C;
        float field_30;
        int field_34;

        uint8_t GetByteAtPos(int x, int y)
        {
            if (x < 0 || y < 0 || x >= Width || y >= Height) {
                ERR("Tried to get shroud data on out-of-bounds shroud coordinates (%d,%d); bounds are (%d,%d)", x, y, Width, Height);
                return 0;
            }

            auto offset = x + y * Width;
            return DataBuffer[offset];
        }

        void SetByteAtPos(int x, int y, uint8_t value)
        {
            if (x < 0 || y < 0 || x >= Width || y >= Height) {
                ERR("Tried to set shroud data on out-of-bounds shroud coordinates (%d,%d); bounds are (%d,%d)", x, y, Width, Height);
                return;
            }

            auto offset = x + y * Width;
            DataBuffer[offset] = value;
        }
    };

    struct ShroudData : public ProtectedGameObject<ShroudData>
    {
        void* LevelAllocator;
        ObjectSet<glm::vec2> OS_Vector2f;
        ObjectSet<glm::vec2> OS_Vector2f_2;
        AiGrid* AiGrid;
        ShroudObject* ShroudObjects[8];
        float OffsetX;
        float OffsetY;
        float OffsetZ;
        int GridWidth;
        int GridHeight;
        int GridMinX;
        int GridMinZ;
        int GridMaxX;
        int GridMaxZ;


        uint8_t GetByteAtPos(ShroudType type, float x, float z)
        {
            auto shroudObj = ShroudObjects[(int)type * 2];
            int gridX = (int)floor((x - OffsetX) / AiGrid->DataGrid.GridScale * shroudObj->Scale);
            int gridZ = (int)floor((z - OffsetZ) / AiGrid->DataGrid.GridScale * shroudObj->Scale);
            return shroudObj->GetByteAtPos(gridX, gridZ);
        }

        void SetByteAtPos(ShroudType type, float x, float z, uint8_t value)
        {
            auto shroudObj = ShroudObjects[(int)type * 2];
            int gridX = (int)floor((x - OffsetX) / AiGrid->DataGrid.GridScale * shroudObj->Scale);
            int gridZ = (int)floor((z - OffsetZ) / AiGrid->DataGrid.GridScale * shroudObj->Scale);
            shroudObj->SetByteAtPos(gridX, gridZ, value);
        }
    };

}

namespace dse::esv
{
    struct ShroudManager : public ProtectedGameObject<ShroudManager>
    {
        void* VMT;
        EntityWorld* EntityWorld;
        ObjectSet<glm::vec2> FrustumUpdateShape;
        eoc::AiGrid* AiGrid;
        Level* Level;
        eoc::ShroudData* ShroudData;
        // RegionMask layer value => trigger UUID
        RefMap<int8_t, FixedString> RegionTriggers;
        float UpdateTimer;
        bool IsEnabled;
    };
}

namespace dse::ecl
{
    struct ShroudManager : public ProtectedGameObject<ShroudManager>
    {
        struct HeightMapTextureCell
        {
            float16 Height;
            float16 Walkable;
        };

        void* VMT;
        void* LevelAllocator;
        ObjectSet<glm::vec2> OS_Vector2f;
        Level* Level;
        eoc::AiGrid* AiGrid;
        eoc::VisionGrid* VisionGrid;
        PhysicsScene* PhysicsScene;
        eoc::ShroudData* ShroudData;
        ComponentHandle TextureHandles[4];
        bool TexturesLoaded[4];
        ComponentHandle AiGridHeightMapTexture;
        char field_88;
        char field_89;
        char field_8A;
        int VisionGridSize_M[2];
        int field_94;
        RefMap<int, FixedString> ShroudTriggers_int_FS;
        ObjectSet<void*> OS_SightTileData;
        ObjectSet<ComponentHandle> OS_ObjectHandle;
        ObjectSet<ComponentHandle> OS_ObjectHandle2;
        char field_108;

        void RebuildHeightMapTexture();
    };
}
