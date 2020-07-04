#pragma once

#include "BaseTypes.h"
#include "Enumerations.h"
#include "EntitySystem.h"
#include "Stats.h"
#include "Status.h"
#include "Misc.h"

namespace dse
{
    struct ProjectileTemplate;

	namespace esv
	{
		struct Projectile : public IEoCServerObject
		{
            ObjectHandle CasterHandle;
            ObjectHandle SourceHandle;
            ObjectHandle TargetObjectHandle;
            ObjectHandle HitObjectHandle;
            glm::vec3 SourcePosition;
            glm::vec3 TargetPosition;
            DamageType DamageType;
            CauseType DamageSourceType;
            float LifeTime;
            float HitInterpolation;
            eoc::PathMover PathMover1;
            eoc::PathMover PathMover2;
            float ExplodeRadius0;
            float ExplodeRadius1;
            DamagePairList DamageList;
            DeathType DeathType;
            FixedString SkillId;
            CRPGStats_Object_Property_List* PropertyList;
            uint8_t BoostConditions;
            ObjectHandle WeaponHandle;
            ObjectHandle MovingEffectHandle;
            FixedString SpawnEffect;
            bool SpawnFXOverridesImpactFX;
            ObjectHandle EffectHandle;
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
            void* OnHitAction;
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

	    struct ShootProjectileHelperHitObject
	    {
		    int HitInterpolation{ 0 };
		    ObjectHandle Target;
		    glm::vec3 Position;
	    };

	    struct ShootProjectileHelper
	    {
		    FixedString SkillId;
		    ObjectHandle Caster;
		    ObjectHandle Source;
		    ObjectHandle Target;
		    glm::vec3 StartPosition{ .0f };
		    glm::vec3 EndPosition{ .0f };
		    uint8_t Random{ 0 };
		    DamagePairList * DamageList;
		    int32_t CasterLevel{ -1 };
		    ShootProjectileHelperHitObject * HitObject;
		    bool IsTrap{ false };
		    bool UnknownFlag1{ false };
		    FixedString CleanseStatuses;
		    float StatusClearChance{ 0.0f };
		    bool IsFromItem{ false };
		    bool IsStealthed{ false };
		    bool IgnoreObjects{ false };
	    };

	}
}
