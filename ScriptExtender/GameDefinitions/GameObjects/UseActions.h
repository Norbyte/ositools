#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>

BEGIN_SE()

struct IActionData
{
    void* VMT;
    ActionDataType Type;
};

struct TeleportActionData : public IActionData
{
    FixedString Source;
    uint8_t SourceType;
    FixedString Target;
    uint8_t TargetType;
    FixedString EventID;
    uint8_t Visibility;
};

struct CreateSurfaceActionData : public IActionData
{
    SurfaceType SurfaceType;
    float LifeTime;
    float Radius;
    bool ExternalCauseAsSurfaceOwner;
};

struct CreatePuddleActionData : public IActionData
{
    SurfaceType SurfaceType;
    float LifeTime;
    float Timeout;
    int TotalCells;
    int CellAtGrow;
    float GrowTimer;
    bool ExternalCauseAsSurfaceOwner;
};

struct DestroyParametersData : public IActionData
{
    FixedString ExplodeFX;
    FixedString VisualDestruction;
    FixedString TemplateAfterDestruction;
};

struct ConsumeActionData : public IActionData
{
    FixedString StatsId;
    bool Consume;
};

struct DoorActionData : public IActionData
{
    bool SecretDoor;
};

struct BookActionData : public IActionData
{
    FixedString BookId;
};

struct UseSkillActionData : public IActionData
{
    FixedString SkillID;
    bool Consume;
};

struct SkillBookActionData : public IActionData
{
    FixedString SkillID;
    bool Consume;
};

struct SitActionData : public IActionData
{
    float Heal;
};

struct LyingActionData : public IActionData
{
    float Heal;
};

struct IdentifyActionData : public IActionData
{
    bool Consume;
};

struct RepairActionData : public IActionData
{
    bool Consume;
};

struct LockpickActionData : public IActionData
{
    bool Consume;
};

struct DisarmTrapActionData : public IActionData
{
    bool Consume;
};

struct ShowStoryElementUIActionData : public IActionData
{
    int Type;
    STDString UIStoryInstance;
};

struct CraftActionData : public IActionData
{
    CraftingStationType CraftingStationType;
};

struct PlaySoundActionData : public IActionData
{
    FixedString ActivateSoundEvent;
    bool PlayOnHUD;
};

struct SpawnCharacterActionData : public IActionData
{
    FixedString RootTemplate;
    FixedString LocalTemplate;
    FixedString SpawnFX;
};

struct ConstrainActionData : public IActionData
{
    float Damage;
};

struct RecipeActionData : public IActionData
{
    FixedString RecipeID;
};


END_SE()
