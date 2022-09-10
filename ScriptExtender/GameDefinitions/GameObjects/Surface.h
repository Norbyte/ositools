#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/EntitySystem.h>

BEGIN_NS(eoc)

struct SurfaceTransformInteractions
{
    struct Interaction
    {
        SurfaceTransformActionType ActionType;
        ESurfaceFlag ActionableSurfaces[17];
    };

    struct InteractionSet
    {
        Interaction Interactions[5];
    };

    InteractionSet SurfaceTypes[0x4F];
};

END_NS()


BEGIN_NS(esv)

struct SurfaceCell
{
    glm::i16vec2 Position;
};

struct SubSurface : public ProtectedGameObject<SubSurface>
{
    Surface* Surface;
    uint64_t SurfaceStateFlags;
    PrimitiveSmallSet<SurfaceCell> SurfaceCells1;
    PrimitiveSmallSet<SurfaceCell> SurfaceCells2;
    uint16_t SurfaceIndex;
    uint8_t Position[2];
    int field_44;
};


struct Surface : ProtectedGameObject<Surface>
{
    static constexpr auto ObjectTypeIndex = ObjectHandleType::Unknown;

    FixedString FS1;
    NetId NetID;
    ComponentHandle MyHandle;
    SurfaceType SurfaceType;
    uint8_t Flags;
    int field_20;
    int CombatComponentIdx;
    eoc::CombatTeamId TeamId;
    ComponentHandle OwnerHandle;
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
    PrimitiveSmallSet<SurfaceCell> Cells;
};

struct SurfaceManager : public ComponentFactory<Surface>
{
    BaseComponentProcessingSystem<EntityWorld> ComponentProcessingSystem;
    uint64_t field_98;
    ObjectSet<SurfaceAction*> SurfaceActions;
    Array<Surface*> Surfaces;
    PrimitiveSmallSet<SurfaceCell> SurfaceCells;
    SurfaceCollectCellsFlood CollectCellsFlood;
    ObjectSet<PrimitiveSmallSet<SurfaceCell>> SurfaceCellSetsByLayer;
    RefMap<uint64_t, uint64_t> field_158;
};

struct SurfaceActionVMT
{
    void (*Destroy)(SurfaceAction*, bool);
    SurfaceActionType (*GetTypeId)(SurfaceAction*);
    bool (*Visit)(SurfaceAction* self, ObjectVisitor* visitor);
    int* (*Unknown)(SurfaceAction* self, int*, bool);
    void (*Enter)(SurfaceAction* self);
    void (*Update)(SurfaceAction* self, void* time);
    void (*Unknown2)(SurfaceAction* self);
    void (*Reset)(SurfaceAction* self);
    bool (*IsFinished)(SurfaceAction* self);
    bool (*Unknown3)(SurfaceAction* self);
};

struct SurfaceAction : ProtectedGameObject<SurfaceAction>
{
    static constexpr auto ObjectTypeIndex = ObjectHandleType::ServerSurfaceAction;

    SurfaceActionVMT* VMT;
    Level* Level;
    ComponentHandle MyHandle;
};

struct CreateSurfaceActionBase : public SurfaceAction
{
    ComponentHandle OwnerHandle; // Init param
    float Duration; // Init param
    float StatusChance; // Init param
    glm::vec3 Position; // Init param
    SurfaceType SurfaceType; // Init param
    std::array<ComponentHandle, 79> SurfaceHandlesByType;
    std::array<PrimitiveSmallSet<SurfaceCell>*, 79> SurfaceChanges;
    std::array<PrimitiveSmallSet<SurfaceCell>*, 2> SurfaceCellsByLayer;
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
    PrimitiveSmallSet<SurfaceCell> SurfaceCells;
    SurfaceLayer SurfaceLayer; // Init param
};

struct ChangeSurfaceOnPathAction : public CreateSurfaceActionBase
{
    ComponentHandle FollowObject; // Init param
    float Radius; // Init param
    bool IsFinished;
    PrimitiveSmallSet<SurfaceCell> SurfaceCells;
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
    PrimitiveSmallSet<SurfaceCell> CellAtGrow;
    PrimitiveSmallSet<SurfaceCell> ClosedCells;
    float GrowTimer;
};

struct ExtinguishFireAction : public CreateSurfaceActionBase
{
    glm::vec3 ExtinguishPosition;
    float Radius;
    float Percentage;
    float GrowTimer;
    float Step;
    PrimitiveSmallSet<SurfaceCell> Cells1;
    PrimitiveSmallSet<SurfaceCell> Cells2;
};

struct TransformSurfaceAction : public SurfaceAction
{
    using InitProc = void (TransformSurfaceAction* self, SurfaceTransformActionType, SurfaceLayer, SurfaceType);

    float Timer;
    SurfaceTransformActionType SurfaceTransformAction; // Init param
    SurfaceType OriginSurface; // Init param
    SurfaceLayer SurfaceLayer; // Init param
    float GrowCellPerSecond;
    bool Finished;
    void* CellSearcher;
    ComponentHandle OwnerHandle; // Init param
    glm::vec3 Position; // Init param
    float SurfaceLifetime; // Init param
    float SurfaceStatusChance; // Init param
    RefMap<SurfaceType, ComponentHandle> SurfaceMap;
    RefMap<SurfaceType, PrimitiveSmallSet<SurfaceCell>> SurfaceCellMap;
    PrimitiveSmallSet<SurfaceCell> SurfaceRemoveGroundCellMap;
    PrimitiveSmallSet<SurfaceCell> SurfaceRemoveCloudCellMap;
};

struct RectangleSurfaceAction : public CreateSurfaceActionBase
{
    glm::vec3 Target; // Init param
    uint8_t _Pad1[4];
    int field_548;
    float SurfaceArea;
    float Width; // Init param
    float Length; // Init param
    float GrowTimer; // Init param
    float MaxHeight; // Init param
    int GrowStep; // Init param
    uint64_t AiFlags; // Init param
    stats::DamagePairList DamageList; // Init param
    stats::DeathType DeathType; // Init param
    uint64_t LineCheckBlock;
    stats::PropertyList* SkillProperties;
    float CurrentGrowTimer;
    PrimitiveSmallSet<SurfaceCell> SurfaceCells;
    ObjectSet<ComponentHandle> Characters;
    ObjectSet<ComponentHandle> Items;
    uint64_t CurrentCellCount;
};

struct PolygonSurfaceAction : public CreateSurfaceActionBase
{
    FixedString field_538;
    ObjectSet<glm::vec2> PolygonVertices; // Init param
    int field_560;
    stats::DamagePairList DamageList;
    char field_588;
    __int64 field_590;
    float CurrentGrowTimer;
    float GrowTimer; // Init param
    float PositionX;
    float PositionZ;
    int GrowStep; // Init param
    int LastSurfaceCellCount;
    PrimitiveSmallSet<SurfaceCell> SurfaceCells;
    ObjectSet<ComponentHandle> Characters;
    ObjectSet<ComponentHandle> Items;
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
    PrimitiveSmallSet<SurfaceCell> SurfaceCells;
    PrimitiveSmallSet<SurfaceCell> TargetCells;
    RefMap<dse::SurfaceType, PrimitiveSmallSet<SurfaceCell>> SurfaceCellMap;
    RefMap<dse::SurfaceType, PrimitiveSmallSet<SurfaceCell>> TargetCellMap;
};

struct ZoneAction : public CreateSurfaceActionBase
{
    FixedString SkillId;
    glm::vec3 Target; // Init param
    int Shape; // Init param
    float Radius; // Init param
    float AngleOrBase; // Init param
    float BackStart; // Init param
    float FrontOffset; // Init param
    float MaxHeight; // Init param
    float GrowTimer; // Init param
    uint32_t GrowStep; // Init param
    int field_56C;
    uint64_t AiFlags;
    stats::DamagePairList DamageList;
    stats::DeathType DeathType;
    stats::PropertyList* SkillProperties;
    float GrowTimerStart;
    int field_5AC;
    PrimitiveSmallSet<SurfaceCell> SurfaceCells;
    ObjectSet<ComponentHandle> Characters;
    ObjectSet<ComponentHandle> Items;
    uint64_t CurrentCellCount;
    bool IsFromItem;
};


struct SurfaceActionFactory : public ComponentFactory<SurfaceAction>
{
    using CreateActionProc = esv::SurfaceAction* (esv::SurfaceActionFactory* self, SurfaceActionType actionId, uint64_t handle);
};

END_NS()


BEGIN_NS(ecl)

struct Surface;

struct SurfaceRegion
{
    int8_t X;
    int8_t Y;
};

struct SurfaceMetaData
{
    eoc::CombatTeamId CombatTeamId;
    ComponentHandle OwnerHandle;
    float TimeElapsed;
    bool field_14;
};


struct SurfaceManager
{
    void* VMT;
    __int64 field_8;
    std::array<Surface*, 79> SurfacesByType;
    Level* Level;
    void* AlphaRenderer; // SurfaceAlphaRenderer*
    Array<SurfaceRegion> Regions;
    bool NeedsVisualReload;
    bool ShouldReloadInstances;
    bool ShouldReloadEffects;
    Array<SurfaceMetaData> MetaData;
};

struct SubSurface;

struct Surface : public ProtectedGameObject<Surface>
{
    struct FadeOutInstance : public ProtectedGameObject<FadeOutInstance>
    {
        __int64 field_0;
        glm::vec3 Position;
        int field_14;
        __int64 field_18;
    };


    void* VMT;
    SubSurface* SubSurfaceGrid;
    int Width;
    int Height;
    ObjectSet<SubSurface*> SubSurfaces;
    SurfaceManager* SurfaceManager;
    eoc::AiGrid* AiGrid;
    void* SceneInstanceManager;
    void* Random;
    int SomeCellSizeRemainder;
    int field_5C;
    __int64 field_60;
    __int64 field_68;
    SurfaceType SurfaceTypeId;
    uint8_t Flags;
    SurfaceTemplate* Template;
    ObjectSet<FixedString> InstanceVisuals;
    ObjectSet<FadeOutInstance> FadeOut;
    ObjectSet<RefMap<int, ComponentHandle>*> Effects; // PooledRefMap*
    uint64_t NumIntroFXVisuals;
    EntityHandle EntityHandle;
    ObjectSet<glm::vec3> OS_Vec3f;
    ObjectSet<SurfaceRegion> Regions;
    bool NeedsSoundUpdate;
    uint32_t RandomSeed;
    ObjectSet<glm::i16vec2> Cells;
};

struct SubSurfaceCell
{
    int16_t LayerIndex;
    int16_t SoundPositionIndex;
};

struct SubSurface : public ProtectedGameObject<SubSurface>
{
    struct ChangingCell
    {
        int16_t X;
        int16_t Y;
        int field_4;
        bool field_8;
    };

    uint8_t X;
    uint8_t Y;
    void* CullObject;
    ObjectSet<ChangingCell> Changes;
    float FadeInSpeed;
    float FadeOutSpeed;
    SurfaceType SurfaceTypeId;
    SurfaceLayer Layer;
    ecl::Surface* Surface;
    std::array<ecl::SubSurfaceCell, 256> SoundSurfaceRegionList;
    ObjectSet<glm::vec3> SoundPositions;
};



END_NS()
