#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/EntitySystem.h>
#include <GameDefinitions/Stats.h>
#include <GameDefinitions/GameObjects/Status.h>
#include <GameDefinitions/GameObjects/ActionMachine.h>
#include <GameDefinitions/GameObjects/UseActions.h>
#include <GameDefinitions/GameObjects/VisualsShared.h>

namespace dse
{
    struct GameObjectTemplate : ProtectedGameObject<GameObjectTemplate>
    {
        virtual FixedString* GetTypeId() = 0;
        virtual GameObjectTemplate* Clone() = 0;
        virtual bool IsValid() = 0;
        virtual void Fix() = 0;
        virtual bool HasCustomVariables() = 0;
        virtual ~GameObjectTemplate() = 0;
        virtual bool Visit(ObjectVisitor* visitor) = 0;
        virtual bool NeedsUUIDPatchForPersistentLevelTemplate() = 0;
        virtual void UpdateFromRootTemplate(GameObjectTemplate* root, bool replaceOverrides) = 0;
        virtual void Verify() = 0;

#if defined(OSI_EOCAPP)
        uint64_t _Unknown;
#endif
        OverrideableProperty<GameObjectTemplateFlags> Flags;
        OverrideableProperty<ObjectSet<FixedString>> Tags;
        FixedString Id;
        STDString Name;
        FixedString RootTemplate;
        TemplateHandle Handle;
        bool IsGlobal;
        bool IsDeleted;
        FixedString LevelName;
        FixedString ModFolder;
        uint32_t GroupID;
        bool Flag2;
        Transform Transform;
        bool NonUniformScale;
#if defined(OSI_EOCAPP)
        uint64_t _Unknown1[2];
#endif
        OverrideableProperty<FixedString> VisualTemplate;
        OverrideableProperty<FixedString> PhysicsTemplate;
        OverrideableProperty<bool> CastShadow;
        OverrideableProperty<bool> ReceiveDecal;
        OverrideableProperty<bool> AllowReceiveDecalWhenAnimated;
        OverrideableProperty<bool> IsReflecting;
        OverrideableProperty<bool> IsShadowProxy;
        OverrideableProperty<uint8_t> RenderChannel;
        OverrideableProperty<glm::vec3> CameraOffset;
        bool HasParentModRelation;
        bool Unknown;
        LevelTemplate* ContainingLevelTemplate;
        OverrideableProperty<bool> HasGameplayValue;
#if !defined(OSI_EOCAPP)
        STDString DevComment;
        ObjectSet<FixedString> IsOverriddenFrom;
        RefMap<FixedString, void*> LayerLists; // LayerList*
#endif
        Path FileName;
    };

    struct GlobalTemplateBank : public ProtectedGameObject<GlobalTemplateBank>
    {
        void* VMT;
        Map<FixedString, GameObjectTemplate*> Templates;
        ObjectSet<GameObjectTemplate*> RootTemplates;
        Map<TemplateHandle, GameObjectTemplate*> RootTemplatesByHandle;
        Map<TemplateHandle, GameObjectTemplate*> GlobalTemplatesByHandle;
        Map<uint16_t, ObjectSet<GameObjectTemplate*>*> TemplatesByType;
        uint32_t FirstHandle;
        uint32_t LastHandle;
        FixedString ModName;
        void* LocalLoadHelper;
    };

    struct GlobalTemplateManager : public ProtectedGameObject<GlobalTemplateManager>
    {
        using GetInstanceProc = GlobalTemplateManager * ();

        void* VMT;
        Map<FixedString, GameObjectTemplate*> Templates;
        GlobalTemplateBank* Banks[2];
    };

    struct LocalTemplateManager : public ProtectedGameObject<LocalTemplateManager>
    {
        void* VMT;
        Map<FixedString, GameObjectTemplate*> Templates;
        Map<uint16_t, ObjectSet<GameObjectTemplate*>*> TemplatesByType;
        Map<TemplateHandle, GameObjectTemplate*> TemplatesByHandle;
        uint32_t RefCount;
        bool Locked;
        uint32_t NextTemplateId;
        int16_t TemplateTypeId;
        void* LocalLoadHelperEx;
        FixedString ModName;
        FixedString LevelName;
        LevelTemplate* LevelTemplate_BAD;
    };

    struct CacheTemplateManagerBase : public ProtectedGameObject<CacheTemplateManagerBase>
    {
        using CacheTemplateProc = GameObjectTemplate* (CacheTemplateManagerBase* self, GameObjectTemplate* tmpl);

        virtual ~CacheTemplateManagerBase() = 0;
        virtual void Clear(uint8_t type) = 0;
        virtual bool ShouldSave(ObjectVisitor* visitor, GameObjectTemplate* tmpl) = 0;
        virtual bool OnLoad(ObjectVisitor* visitor, GameObjectTemplate* tmpl) = 0;
        virtual uint32_t GetNewIndex() = 0;
        virtual bool PostVisit() = 0;

        TemplateType TemplateManagerType;
        Map<FixedString, GameObjectTemplate*> Templates;
        Map<TemplateHandle, GameObjectTemplate*> TemplatesByHandle;
        RefMap<TemplateHandle, uint32_t> RefCountsByHandle;
        ObjectSet<GameObjectTemplate*> NewTemplates;
        ObjectSet<void*> CacheTemplateRemovers;
        bool field_90;
    };


    struct GlobalCacheTemplateManager : public CacheTemplateManagerBase
    {
        uint32_t NextTemplateId;
    };

    struct LevelCacheTemplateManager : public CacheTemplateManagerBase
    {
        FixedString LevelName;
    };


    struct LevelTemplate : public GameObjectTemplate
    {
        OverrideableProperty<FixedString> SubLevelName;
        OverrideableProperty<bool> IsPersistent;
        OverrideableProperty<bool> IsStartingActive;
        Bound LocalLevelBound;
        Bound WorldLevelBound;
        FixedString LevelBoundTrigger;
    };

    struct EoCGameObjectTemplate : public GameObjectTemplate
    {
        virtual glm::vec3* GetAIBoundsMin(glm::vec3& bounds) = 0;
        virtual glm::vec3* GetAIBoundsMax(glm::vec3& bounds) = 0;
        virtual float GetAIBoundsRadius() = 0;
        virtual uint32_t GetAIBoundsAIType() = 0;
        virtual float GetAIBoundSize() = 0;
        virtual void SetFadeable(bool fadeable) = 0;
        virtual void SetFadeIn(bool fadeIn) = 0;
        virtual void SetOpacity(float opacity) = 0;
        virtual void SetSeeThrough(bool seeThrough) = 0;

        glm::vec3 AIBoundsMin;
        uint8_t _Pad0[4];
        glm::vec3 AIBoundsMax;
        uint8_t _Pad1[4];
        OverrideableProperty<float> AIBoundsRadius;
        OverrideableProperty<float> AIBoundsHeight;
        uint8_t AIBoundsAIType;
        OverrideableProperty<TranslatedString> DisplayName;
        OverrideableProperty<float> Opacity;
        OverrideableProperty<bool> Fadeable;
        OverrideableProperty<bool> FadeIn;
        OverrideableProperty<bool> SeeThrough;
        OverrideableProperty<FixedString> FadeGroup;
        int32_t GameMasterSpawnSection;
        OverrideableProperty<TranslatedString> GameMasterSpawnSubSection;
    };

    struct CombatComponentTemplate : ProtectedGameObject<CombatComponentTemplate>
    {
        void* VMT;
        OverrideableProperty<FixedString> Alignment;
        OverrideableProperty<bool> CanFight;
        OverrideableProperty<bool> CanJoinCombat;
        OverrideableProperty<FixedString> CombatGroupID;
        OverrideableProperty<bool> IsBoss;
        OverrideableProperty<bool> IsInspector;
        OverrideableProperty<float> StartCombatRange;
        bool CanFight2;
    };

    struct VisualSet : public ProtectedGameObject<VisualSet>
    {
        void* VMT;
        std::array<ObjectSet<FixedString>, 9> Visuals;
        std::array<ObjectSet<uint32_t>, 3> Colors;
    };

    struct SkillAIConditions
    {
        int32_t MinimumHealthPercentage;
        int32_t MaximumHealthPercentage;
        bool HasNoPhysicalArmor;
        bool HasNoMagicalArmor;
        ObjectSet<FixedString> Tags;
    };

    struct SkillAIParams
    {
        float ScoreModifier;
        int32_t StartRound;
        int32_t MinimumImpact;
        bool OnlyCastOnSelf;
        uint8_t AIFlags; // Enum
        SkillAIConditions SourceConditions;
        SkillAIConditions TargetConditions;
        bool CasualExplorer;
        bool Classic;
        bool TacticianHardcore;
        bool HonorHardcore;
        uint32_t Unknown;
    };

    struct CharacterSkillData
    {
        FixedString SkillId;
        SkillAIParams AIParams;
    };

    struct InventoryItemData
    {
        FixedString UUID;
        FixedString TemplateID;
        STDString field_10;
        STDString ItemName;
        int Type;
        STDString LevelName;
        int Amount;
        SkillAIParams AIParams;
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
        OverrideableProperty<ObjectSet<IActionData*>> OnDeathActions;
        OverrideableProperty<SurfaceType> BloodSurfaceType;
        OverrideableProperty<FixedString> ExplodedResourceID;
        OverrideableProperty<FixedString> ExplosionFX;
        OverrideableProperty<ObjectSet<void*>> Scripts; // ScriptData*
        OverrideableProperty<ObjectSet<CharacterSkillData>> SkillList;
        OverrideableProperty<ObjectSet<InventoryItemData>> ItemList;
        OverrideableProperty<FixedString> VisualSetResourceID;
        OverrideableProperty<VisualSetIndices> VisualSetIndices;
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
        VisualSet* VisualSet;

        ObjectSet<FixedString>* GetVisualChoices(VisualTemplateVisualIndex slot);
        ObjectSet<uint32_t>* GetColorChoices(VisualTemplateColorIndex slot);
    };


    struct ItemTemplateEquipment
    {
        std::array<FixedString, 17> VisualResources;
        std::array<bool, 17> SyncAnimationWithParent;
        uint32_t VisualSetSlots;
        uint32_t EquipmentSlots;
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
        OverrideableProperty<ObjectSet<FixedString>> Treasures;
        OverrideableProperty<ObjectSet<IActionData*>> OnUsePeaceActions;
        OverrideableProperty<ObjectSet<IActionData*>> OnDestroyActions;
        OverrideableProperty<TranslatedString> OnUseDescription;
        OverrideableProperty<ObjectSet<void*>> Scripts; // ScriptData*
        OverrideableProperty<ObjectSet<InventoryItemData>> ItemList;
        OverrideableProperty<FixedString> DefaultState;
        OverrideableProperty<FixedString> Owner;
        OverrideableProperty<FixedString> Key;
        OverrideableProperty<FixedString> HitFX;
        OverrideableProperty<int32_t> LockLevel;
        OverrideableProperty<int32_t> Amount;
        OverrideableProperty<int32_t> MaxStackAmount;
        OverrideableProperty<int32_t> TreasureLevel;
        OverrideableProperty<ItemTemplateEquipment*> Equipment;
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

    struct SurfaceTemplate : public GameObjectTemplate
    {
        struct VisualData
        {
            FixedString Visual;
            float Height;
            uint8_t gapC[4];
            glm::ivec2 Rotation;
            glm::vec2 Scale;
            float GridSize;
            int SpawnCell;
            float RandomPlacement;
            float SurfaceNeeded;
            float SurfaceRadiusMax;
        };


        struct StatusData
        {
            FixedString StatusId;
            float Chance;
            float Duration;
            bool RemoveStatus;
            bool OnlyWhileMoving;
            bool ApplyToCharacters;
            bool ApplyToItems;
            bool KeepAlive;
            bool VanishOnReapply;
            bool ForceStatus;
        };


        int SurfaceTypeId;
        FixedString SurfaceType;
        OverrideableProperty<TranslatedString> DisplayName;
        OverrideableProperty<TranslatedString> Description;
        OverrideableProperty<FixedString> DecalMaterial;
        OverrideableProperty<bool> CanEnterCombat;
        OverrideableProperty<bool> AlwaysUseDefaultLifeTime;
        OverrideableProperty<float> DefaultLifeTime;
        OverrideableProperty<float> SurfaceGrowTimer;
        OverrideableProperty<float> FadeInSpeed;
        OverrideableProperty<float> FadeOutSpeed;
        OverrideableProperty<int> Seed;
        OverrideableProperty<ObjectSet<VisualData>> InstanceVisual;
        OverrideableProperty<ObjectSet<VisualData>> IntroFX;
        OverrideableProperty<ObjectSet<VisualData>> FX;
        OverrideableProperty<ObjectSet<StatusData>> Statuses;
        OverrideableProperty<FixedString> DamageWeapon;
        OverrideableProperty<FixedString> Summon;
        OverrideableProperty<bool> DamageCharacters;
        OverrideableProperty<bool> DamageItems;
        OverrideableProperty<bool> DamageTorches;
        OverrideableProperty<bool> RemoveDestroyedItems;
        OverrideableProperty<bool> CanSeeThrough;
        OverrideableProperty<bool> CanShootThrough;
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

    struct TriggerTemplate : public GameObjectTemplate
    {
        void* TriggerTypeData;
        void* PhysicsTypeData;
        OverrideableProperty<FixedString> TriggerType;
        OverrideableProperty<uint32_t> PhysicsType;
#if defined(OSI_EOCAPP)
        uint64_t UnknownTrigger1;
#endif
        OverrideableProperty<glm::vec4> Color4;
#if defined(OSI_EOCAPP)
        uint64_t UnknownTrigger2;
#endif
        OverrideableProperty<FixedString> TriggerGizmoOverride;
    };

}
