#pragma once

#include "BaseTypes.h"
#include "Enumerations.h"
#include "EntitySystem.h"
#include "Stats.h"
#include "Status.h"
#include "ActionMachine.h"

namespace dse
{
    struct Transform
    {
        glm::mat4 Matrix;
        glm::mat3 Rotate;
        glm::vec3 Translate;
        glm::vec3 Scale;
    };

    enum class GameObjectTemplateFlags
    {
        IsCustom = 1
    };

    template <class T>
    struct OverrideableProperty
    {
        using Type = T;

        T Value;
        bool IsOverridden;
    };

    struct GameObjectTemplate
    {
        void * VMT;
        uint32_t Flags;
        uint8_t Flag;
        ObjectSet<FixedString> Tags;
        bool HasAnyTags;
        FixedString Id;
        STDString Name;
        FixedString TemplateName;
        uint32_t TemplateHandle;
        bool IsGlobal;
        bool IsDeleted;
        FixedString LevelName;
        FixedString ModFolder;
        uint32_t GroupID;
        bool Flag2;
        Transform Transform;
        bool NonUniformScale;
        OverrideableProperty<FixedString> VisualTemplate;
        OverrideableProperty<FixedString> PhysicsTemplate;
        // uint8_t _Pad1[7];
        OverrideableProperty<bool> CastShadow;
        OverrideableProperty<bool> ReceiveDecal;
        OverrideableProperty<bool> AllowReceiveDecalWhenAnimated;
        OverrideableProperty<bool> IsReflecting;
        OverrideableProperty<bool> IsShadowProxy;
        OverrideableProperty<uint8_t> RenderChannel;
        OverrideableProperty<glm::vec3> CameraOffset;
        // uint8_t _Pad2[3];
        bool HasParentModRelation;
        bool Unknown;
        void* ContainingLevelTemplate; // ls::LevelTemplate*
        OverrideableProperty<bool> HasGameplayValue;
        STDString DevComment;
        ObjectSet<FixedString> IsOverriddenFrom;
        RefMap<FixedString, void*> LayerLists; // LayerList*
        Path FileName;
    };

    struct EoCGameObjectTemplate : public GameObjectTemplate
    {
        float AIBoundsMin[3];
        float AIBoundsMax[3];
        OverrideableProperty<float> AIBoundsRadius;
        OverrideableProperty<float> AIBoundsHeight;
        uint8_t AIBoundsAIType;
        OverrideableProperty<TranslatedString> DisplayName;
        OverrideableProperty<float> Opacity;
        OverrideableProperty<bool> Fadeable;
        OverrideableProperty<bool> FadeIn;
        OverrideableProperty<bool> SeeThrough;
        OverrideableProperty<FixedString> FadeGroup;
        FixedString GameMasterSpawnSection;
        OverrideableProperty<TranslatedString> GameMasterSpawnSubSection;
    };

    struct CombatComponentTemplate
    {
        void* field_0;
        OverrideableProperty<FixedString> Alignment;
        OverrideableProperty<bool> CanFight;
        OverrideableProperty<bool> CanJoinCombat;
        OverrideableProperty<FixedString> CombatGroupID;
        OverrideableProperty<bool> IsBoss;
        OverrideableProperty<bool> IsInspector;
        OverrideableProperty<int32_t> StartCombatRange;
        bool CanFight2;
    };

    struct CharacterTemplate : public EoCGameObjectTemplate
    {
        CombatComponentTemplate CombatComponent;
        OverrideableProperty<FixedString> Icon;
        OverrideableProperty<FixedString> Stats;
        OverrideableProperty<FixedString> SkillSet;
        OverrideableProperty<FixedString> Equipment;
        OverrideableProperty<ObjectSet<FixedString>> Treasures;
        OverrideableProperty<ObjectSet<FixedString>> TradeTreasures;
        OverrideableProperty<FixedString> LightID;
        OverrideableProperty<FixedString> HitFX;
        OverrideableProperty<FixedString> DefaultDialog;
        OverrideableProperty<STDString> SpeakerGroup;
        OverrideableProperty<STDString> GeneratePortrait;
        OverrideableProperty<float> WalkSpeed;
        OverrideableProperty<float> RunSpeed;
        OverrideableProperty<float> ClimbAttachSpeed;
        OverrideableProperty<float> ClimbLoopSpeed;
        OverrideableProperty<float> ClimbDetachSpeed;
        OverrideableProperty<bool> CanShootThrough;
        OverrideableProperty<bool> WalkThrough;
        OverrideableProperty<bool> CanClimbLadders;
        OverrideableProperty<bool> IsPlayer;
        OverrideableProperty<bool> Floating;
        OverrideableProperty<bool> SpotSneakers;
        OverrideableProperty<bool> CanOpenDoors;
        OverrideableProperty<bool> AvoidTraps;
        OverrideableProperty<bool> InfluenceTreasureLevel;
        OverrideableProperty<bool> HardcoreOnly;
        OverrideableProperty<bool> NotHardcore;
        OverrideableProperty<bool> JumpUpLadders;
        OverrideableProperty<bool> NoRotate;
        OverrideableProperty<bool> IsHuge;
        OverrideableProperty<uint32_t> EquipmentClass;
        OverrideableProperty<ObjectSet<void*>> OnDeathActions; // IActionData*
        OverrideableProperty<SurfaceType> BloodSurfaceType;
        OverrideableProperty<FixedString> ExplodedResourceID;
        OverrideableProperty<FixedString> ExplosionFX;
        OverrideableProperty<ObjectSet<void*>> Scripts; // ScriptData*
        OverrideableProperty<ObjectSet<void*>> SkillList; // eoc::CharacterSkillData
        OverrideableProperty<ObjectSet<void*>> ItemList; // eoc::InventoryItemData
        OverrideableProperty<FixedString> VisualSetResourceID;
        OverrideableProperty<uint64_t> VisualSetIndices;
        OverrideableProperty<FixedString> TrophyID;
        OverrideableProperty<FixedString> SoundInitEvent;
        OverrideableProperty<FixedString> SoundAttachBone;
        OverrideableProperty<int16_t> SoundAttenuation;
        OverrideableProperty<uint8_t> CoverAmount;
        OverrideableProperty<int32_t> LevelOverride;
        OverrideableProperty<bool> ForceUnsheathSkills;
        OverrideableProperty<bool> CanBeTeleported;
        OverrideableProperty<FixedString> ActivationGroupId;
        OverrideableProperty<RefMap<FixedString, FixedString>> PickingPhysicsTemplates;
        OverrideableProperty<FixedString> SoftBodyCollisionTemplate;
        OverrideableProperty<FixedString> RagdollTemplate;
        OverrideableProperty<ObjectSet<void*>> FootStepInfos; // eoc::CharacterTemplate::FootStepInfo
        OverrideableProperty<uint32_t> DefaultState;
        OverrideableProperty<FixedString> GhostTemplate;
        OverrideableProperty<bool> IsLootable;
        OverrideableProperty<bool> IsEquipmentLootable;
        OverrideableProperty<uint32_t> InventoryType;
        OverrideableProperty<bool> IsArenaChampion;
        OverrideableProperty<uint32_t> FootstepWeight;
        char field_700;
        bool EmptyVisualSet;
        void* VisualSetObject;
    };

    struct ItemTemplate : public EoCGameObjectTemplate
    {
        CombatComponentTemplate CombatComponent;
        OverrideableProperty<FixedString> Icon;
        OverrideableProperty<bool> CanBePickedUp;
        OverrideableProperty<bool> CanBeMoved;
        OverrideableProperty<uint8_t> CoverAmount;
        OverrideableProperty<bool> CanShootThrough;
        OverrideableProperty<bool> CanClickThrough;
        OverrideableProperty<bool> Destroyed;
        OverrideableProperty<bool> WalkThrough;
        OverrideableProperty<bool> WalkOn;
        OverrideableProperty<bool> Wadable;
        OverrideableProperty<bool> IsInteractionDisabled;
        OverrideableProperty<bool> IsPinnedContainer;
        OverrideableProperty<ObjectSet<FixedString>> PinnedContainerTags;
        OverrideableProperty<bool> StoryItem;
        OverrideableProperty<bool> FreezeGravity;
        OverrideableProperty<bool> IsKey;
        OverrideableProperty<bool> IsTrap;
        OverrideableProperty<bool> IsSurfaceBlocker;
        OverrideableProperty<bool> IsSurfaceCloudBlocker;
        OverrideableProperty<bool> TreasureOnDestroy;
        OverrideableProperty<bool> IsHuge;
        OverrideableProperty<bool> HardcoreOnly;
        OverrideableProperty<bool> NotHardcore;
        OverrideableProperty<bool> UsePartyLevelForTreasureLevel;
        OverrideableProperty<bool> Unimportant;
        OverrideableProperty<bool> Hostile;
        OverrideableProperty<bool> UseOnDistance;
        OverrideableProperty<bool> UseRemotely;
        OverrideableProperty<bool> IsBlocker;
        OverrideableProperty<bool> IsPointerBlocker;
        OverrideableProperty<TranslatedString> UnknownDisplayName;
        OverrideableProperty<uint32_t> Tooltip;
        OverrideableProperty<FixedString> Stats;
        ObjectSet<void*> field_510;
        __int64 field_530;
        OverrideableProperty<ObjectSet<void*>> OnUsePeaceActions; // IActionData*
        OverrideableProperty<ObjectSet<void*>> OnDestroyActions; // IActionData*
        OverrideableProperty<TranslatedString> OnUseDescription;
        OverrideableProperty<ObjectSet<void*>> Scripts; // ScriptData*
        OverrideableProperty<ObjectSet<void*>> ItemList; // eoc::InventoryItemData
        OverrideableProperty<FixedString> DefaultState;
        OverrideableProperty<FixedString> Owner;
        OverrideableProperty<FixedString> Key;
        OverrideableProperty<FixedString> HitFX;
        OverrideableProperty<int32_t> LockLevel;
        OverrideableProperty<int32_t> Amount;
        OverrideableProperty<int32_t> MaxStackAmount;
        OverrideableProperty<int32_t> TreasureLevel;
        void* Equipment;
        __int64 field_6F0;
        OverrideableProperty<FixedString> DropSound;
        OverrideableProperty<FixedString> PickupSound;
        OverrideableProperty<FixedString> UseSound;
        OverrideableProperty<FixedString> EquipSound;
        OverrideableProperty<FixedString> UnequipSound;
        OverrideableProperty<FixedString> InventoryMoveSound;
        OverrideableProperty<FixedString> LoopSound;
        OverrideableProperty<FixedString> SoundInitEvent;
        OverrideableProperty<FixedString> SoundAttachBone;
        OverrideableProperty<int16_t> SoundAttenuation;
        OverrideableProperty<int32_t> BloodSurfaceType;
        __int64 field_798;
        OverrideableProperty<TranslatedString> Description;
        OverrideableProperty<TranslatedString> UnknownDescription;
        OverrideableProperty<FixedString> Speaker;
        OverrideableProperty<FixedString> AltSpeaker;
        OverrideableProperty<STDWString> SpeakerGroup;
        OverrideableProperty<FixedString> ActivationGroupId;
        OverrideableProperty<uint32_t> Race;
        OverrideableProperty<bool> IsWall;
        OverrideableProperty<int32_t> LevelOverride;
        OverrideableProperty<bool> Floating;
        OverrideableProperty<bool> IsSourceContainer;
        OverrideableProperty<FixedString> MeshProxy;
        OverrideableProperty<bool> IsPublicDomain;
        OverrideableProperty<bool> AllowSummonTeleport;
    };

    struct ProjectileTemplate : public EoCGameObjectTemplate
    {
        float LifeTime;
        float Speed;
        float Acceleration;
        FixedString CastBone;
        FixedString ImpactFX;
        FixedString TrailFX;
        bool DestroyTrailFXOnImpact;
        FixedString BeamFX;
        FixedString PreviewPathMaterial;
        FixedString PreviewPathImpactFX;
        float PreviewPathRadius;
        float ImpactFXSize;
        bool RotateImpact;
        bool IgnoreRoof;
        bool DetachBeam;
        bool NeedsArrowImpactSFX;
        bool ProjectilePath;
        float PathShift;
        float PathRadius;
        float PathMinArcDist;
        float PathMaxArcDist;
        int PathRepeat;
    };
}
