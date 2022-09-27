#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/EntitySystem.h>
#include <GameDefinitions/Stats.h>
#include <GameDefinitions/GameObjects/Status.h>
#include <GameDefinitions/Misc.h>
#include <GameDefinitions/Helpers.h>

namespace dse
{

	namespace esv
	{
        struct ProjectileTargetDesc
        {
            ComponentHandle Target;
            glm::vec3 TargetPosition;
            glm::vec3 TargetPosition2;
        };

        struct DefaultProjectileHit : Noncopyable<DefaultProjectileHit>
        {
            virtual void Destroy(bool b) = 0;
            virtual void OnHit(glm::vec3 const& position, ComponentHandle const& objectHandle, Projectile* projectile) = 0;
            virtual void Visit(ObjectVisitor* visitor) = 0;
            virtual int GetTypeId() = 0;

            ComponentHandle CasterHandle;
            int Level{ -1 };
            bool IsFromItem{ false };
            FixedString SkillId;
        };

        struct ProxyProjectileHit : DefaultProjectileHit
        {
            void Destroy(bool b) override;
            void OnHit(glm::vec3 const& position, ComponentHandle const& objectHandle, Projectile* projectile) override;
            void Visit(ObjectVisitor* visitor) override;
            int GetTypeId() override;

            DefaultProjectileHit* WrappedHit{ nullptr };
        };

		struct Projectile : public IEoCServerObject
		{
            static constexpr auto ComponentPoolIndex = EntityComponentIndex::Projectile;
            static constexpr auto ObjectTypeIndex = ObjectHandleType::ServerProjectile;

            using ExplodeProc = void (Projectile*);

            ComponentHandle CasterHandle;
            ComponentHandle SourceHandle;
            ComponentHandle TargetObjectHandle;
            ComponentHandle HitObjectHandle;
            glm::vec3 SourcePosition;
            glm::vec3 TargetPosition;
            stats::DamageType DamageType;
            CauseType DamageSourceType;
            float LifeTime;
            float HitInterpolation;
            eoc::PathMover PathMover1;
            eoc::PathMover PathMover2;
            float ExplodeRadius0;
            float ExplodeRadius1;
            stats::DamagePairList DamageList;
            stats::DeathType DeathType;
            FixedString SkillId;
            stats::PropertyList* PropertyList;
            uint8_t BoostConditions;
            ComponentHandle WeaponHandle;
            ComponentHandle MovingEffectHandle;
            FixedString SpawnEffect;
            bool SpawnFXOverridesImpactFX;
            ComponentHandle EffectHandle;
            bool RequestDelete;
            bool Launched;
            bool IsTrap;
            bool UseCharacterStats;
            bool ReduceDurability;
            bool AlwaysDamage;
            bool ForceTarget;
            bool OnHitActionFlag_M;
            bool IsFromItem;
            bool DivideDamage;
            bool IgnoreRoof;
            bool CanDeflect;
            bool IgnoreObjects;
            DefaultProjectileHit* OnHitAction;
            FixedString CleanseStatuses;
            float StatusClearChance;
            uint64_t Flags;
            glm::vec3 Position;
            glm::vec3 PrevPosition;
            glm::vec3 Velocity;
            glm::mat3 Rotate;
            float Scale;
            FixedString CurrentLevel;
            ProjectileTemplate* ProjectileTemplate;
            ObjectSet<int32_t> PrimSetPeerID;
        };

	    struct ShootProjectileHelper
	    {
		    FixedString SkillId;
		    ComponentHandle Caster;
		    ComponentHandle Source;
		    ComponentHandle Target;
		    glm::vec3 StartPosition{ .0f };
		    glm::vec3 EndPosition{ .0f };
		    uint8_t Random{ 0 };
            stats::DamagePairList * DamageList;
		    int32_t CasterLevel{ -1 };
            HitObject* HitObject;
		    bool IsTrap{ false };
		    bool UnknownFlag1{ false };
		    FixedString CleanseStatuses;
		    float StatusClearChance{ 0.0f };
		    bool IsFromItem{ false };
		    bool IsStealthed{ false };
		    bool IgnoreObjects{ false };
	    };

        using ProjectileHelpers__ShootProjectile = Projectile* (ShootProjectileHelper* ShootProjectileHelper);

        struct ProjectileFactory : public NetworkComponentFactory<Projectile>
        {
            // FIXME - rest unmapped
        };

        struct ProjectileManager : public BaseComponentProcessingSystem<EntityWorld>
        {
            ProjectileFactory* Factory;
            ObjectSet<Projectile*> ActiveProjectiles;
            ObjectSet<Projectile*> ProjectilesToDestroy;
        };
	}
}

BEGIN_NS(ecl)

struct Projectile : public IEoCClientObject
{
    static constexpr auto ComponentPoolIndex = EntityComponentIndex::Projectile;
    static constexpr auto ObjectTypeIndex = ObjectHandleType::ClientProjectile;

    void* VMT;
    FixedString GUID;
    NetId NetID;
    uint32_t _Pad;
    bool RequestDelete;
    ComponentHandle TargetCharacter;
    ComponentHandle Caster;
    ComponentHandle Source;
    ComponentHandle MovingObject;
    FixedString SpawnEffect;
    bool SpawnFXOverridesImpactFX;
    glm::vec3 SourcePos;
    glm::vec3 TargetPos;
    float LifeTime;
    uint64_t field_88;
    FixedString ImpactFX;
    bool RotateImpact;
    float ExplodeRadius;
    uint64_t field_A0;
    eoc::PathMover PathMover;
    uint64_t Flags;
    bool IsGlobal;
    glm::vec3 Translate;
    glm::vec3 Velocity;
    glm::mat3 Rotation;
    float Scale;
    FixedString Level;
    ProjectileTemplate* CurrentTemplate;
    ObjectSet<int> PeerIDClassNames;
    Visual* Visual;
    FixedString SkillID;
    bool IsFirstTick;
};

END_NS()
