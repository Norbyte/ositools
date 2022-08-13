#pragma once

#include <Osiris/Shared/CustomFunctions.h>
#include <GameDefinitions/EntitySystem.h>
#include <GameDefinitions/Components/Character.h>
#include <GameDefinitions/Components/Item.h>
#include <GameDefinitions/GameObjects/GameAction.h>
#include <GameDefinitions/Components/Projectile.h>
#include <GameDefinitions/TurnManager.h>

namespace dse
{
	ecl::EoCClient * GetEoCClient();
	esv::EoCServer * GetEoCServer();
	ModManager * GetModManagerClient();
	ModManager * GetModManagerServer();


	namespace esv
	{
		esv::TurnBasedProtocol* GetTurnBasedProtocol();
		esv::Inventory* FindInventoryByHandle(ComponentHandle const& handle, bool logError = true);
		esv::GameAction* FindGameActionByHandle(ComponentHandle const& handle);

		LegacyPropertyMapBase & StatusToPropertyMap(esv::Status * status);

		struct ShootProjectileApiHelper
		{
			ShootProjectileHelper Helper;
			ShootProjectileHelperHitObject HitObject;
			stats::DamagePairList DamageList;
			bool HasStartPosition{ false };
			bool HasEndPosition{ false };
			bool HasHitObject{ false };
			bool HasHitObjectPosition{ false };
			bool HasDamageList{ false };
			bool AlwaysDamage{ false };
			bool CanDeflect{ true };

			ShootProjectileApiHelper();
			void SetGuidString(FixedString const& prop, char const * value);
			void SetVector(FixedString const& prop, glm::vec3 const & value);
			void AddDamage(stats::DamageType type, int32_t amount);
			bool Shoot();
		};


		struct DamageHelpers
		{
			static constexpr uint32_t HitHandleTypeId = 100;

			enum HelperType
			{
				HT_CustomHit = 0,
				HT_PrepareHitEvent = 1
			};

			ComponentHandle Handle;
			esv::Character * Source{ nullptr };
			IGameObject * Target{ nullptr };

			HelperType Type{ HelperType::HT_CustomHit };
			stats::HitDamageInfo * Hit{ nullptr };
			bool SimulateHit{ false };
			stats::HitType HitType{ stats::HitType::Melee };
			bool NoHitRoll{ false };
			bool ProcWindWalker{ false };
			bool ForceReduceDurability{ false };
			// TODO - SkillProperties
			stats::HighGroundBonus HighGround{ stats::HighGroundBonus::Unknown };
			stats::CriticalRoll CriticalRoll{ stats::CriticalRoll::Roll };

			// 0 - ASAttack
			// 1 - Character::ApplyDamage, StatusDying, ExecPropertyDamage, StatusDamage
			// 2 - AI hit test
			// 3 - Explode
			// 4 - Trap
			// 5 - InSurface
			// 6 - SetHP, osi::ApplyDamage, StatusConsume
			uint32_t HitReason{ 6 };
			FixedString SkillId;
			glm::vec3 ImpactPosition{ .0f };
			bool HasImpactPosition{ false };
			glm::vec3 ImpactOrigin{ .0f };
			bool HasImpactOrigin{ false };
			glm::vec3 ImpactDirection{ .0f };
			bool HasImpactDirection{ false };

			CauseType DamageSourceType{ CauseType::None };
			float Strength{ 0.0f };

			stats::DamagePairList * DamageList{ nullptr };

			DamageHelpers();
			~DamageHelpers();

			void SetInternalDamageInfo();
			void SetExternalDamageInfo(stats::HitDamageInfo * damageInfo, stats::DamagePairList * damageList);

			void SetVector(FixedString const& prop, glm::vec3 const & value);
			void AddDamage(stats::DamageType damageType, int32_t Amount);
			esv::StatusHit * Execute();

		private:
			std::unique_ptr<stats::HitDamageInfo> damageInfo_;
			std::unique_ptr<stats::DamagePairList> damageList_;
		};
	}

	namespace ecl
	{
		ecl::Inventory* FindInventoryByHandle(ComponentHandle const& handle, bool logError = true);
	}
}