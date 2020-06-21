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

#pragma pack(push, 1)
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
            uint8_t _Pad5[3];
            float LifeTime;
            float HitInterpolation;
            eoc::PathMover PathMover1;
            eoc::PathMover PathMover2;
            float ExplodeRadius0;
            float ExplodeRadius1;
            DamagePairList DamageList;
            DeathType DeathType;
            uint8_t _Pad7[7];
            FixedString SkillId;
            CRPGStats_Object_Property_List* PropertyList;
            uint8_t BoostConditions;
            uint8_t _Pad8[7];
            ObjectHandle WeaponHandle;
            ObjectHandle MovingEffectHandle;
            FixedString SpawnEffect;
            bool SpawnFXOverridesImpactFX;
            uint8_t _Pad9[7];
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
            uint8_t _Pad10[3];
            void* OnHitAction;
            FixedString CleanseStatuses;
            float StatusClearChance;
            uint8_t _Pad11[4];
            uint64_t Flags;
            glm::vec3 Position;
            glm::vec3 PrevPosition;
            glm::vec3 Velocity;
            glm::mat3 Rotate;
            float Scale;
            uint8_t _Pad12[4];
            FixedString CurrentLevel;
            ProjectileTemplate* ProjectileTemplate;
            ObjectSet<int32_t> PrimSetPeerID;
        };

	    struct ShootProjectileHelperHitObject
	    {
		    int HitInterpolation{ 0 };
		    uint8_t _Pad[4];
		    ObjectHandle Target;
		    glm::vec3 Position;
		    uint8_t _Pad2[4];
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
		    uint8_t _Pad[7];
		    DamagePairList * DamageList;
		    int32_t CasterLevel{ -1 };
		    uint32_t _Pad2;
		    ShootProjectileHelperHitObject * HitObject;
		    bool IsTrap{ false };
		    bool UnknownFlag1{ false };
		    uint8_t _Pad3[6];
		    FixedString CleanseStatuses;
		    float StatusClearChance{ 0.0f };
		    bool IsFromItem{ false };
		    bool IsStealthed{ false };
		    bool IgnoreObjects{ false };
	    };

	}
#pragma pack(pop)
}
