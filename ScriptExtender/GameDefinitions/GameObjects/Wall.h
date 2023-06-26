#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/Components/Scenery.h>

BEGIN_NS(ecl)

struct WallConstructionStruct
{
    void* VMT;
    FixedString WallPieceVisuals;
    FixedString WallPiecePhysics;
    FixedString WallPieceName;
};

struct WallBase : public Scenery
{
    static constexpr auto ObjectTypeIndex = ObjectHandleType::ClientWallBase;

    virtual WallBase* Clone() = 0;
    virtual void SetWallPieceVisuals(FixedString const& visuals) = 0;
    virtual FixedString const& GetWallPieceVisuals() = 0;
    virtual void LoadWallPiecePhysics() = 0;
    virtual FixedString const& GetWallPiecePhysics() = 0;
    virtual bool VisitWallPieceData(ObjectVisitor&) = 0;

    EntityHandle EntityHandle_;
    WallConstruction* WallConstruction;
    FixedString WallPieceVisuals;
    FixedString WallPiecePhysics;
};

struct WallIntersection : public WallBase
{
    static constexpr auto ObjectTypeIndex = ObjectHandleType::ClientWallIntersection;

    int field_D8;
    OverrideableProperty<Array<Wall*>> Walls;
    Array<int> WallConnectionIds;
};

struct WallBuildingLayer : public WallBase
{
    WallBuildingLayerData BuildingLayerData;
    Visual* Visual;
    bool Walkable;
};

struct Wall
{
    void* VMT;
    Array<WallBase*> WallPieces;
    WallIntersection* Intersection1;
    WallIntersection* Intersection2;
    WallConstruction* WallConstruction;
};

struct WallConstruction
{
    static constexpr auto ObjectTypeIndex = ObjectHandleType::ClientWallConstruction;

    void* VMT;
    ComponentHandle Handle;
    FixedString LevelName;
    OverrideableProperty<Array<Wall*>> Walls;
    OverrideableProperty<Array<WallIntersection*>> WallIntersections;
    WallBuildingLayer* BuildingLayer;
    WallConstructionTemplate* Template;
};

struct WallManager : public ComponentFactory<WallConstruction>
{
    Level* Level;
    Path Path;
};

END_NS()
