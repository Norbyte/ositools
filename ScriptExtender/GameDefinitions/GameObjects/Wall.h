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
    __int64 field_58;
    WallConstructionTemplate* Template;
};

struct WallManager : public ComponentFactory<WallConstruction>
{
    Level* Level;
    Path Path;
};

END_NS()
