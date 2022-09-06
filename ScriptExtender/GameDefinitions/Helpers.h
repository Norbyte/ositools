#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>

BEGIN_NS(esv)

struct HitObject
{
    int HitInterpolation{ 0 };
    ComponentHandle Target;
    glm::vec3 Position;
};

END_NS()
