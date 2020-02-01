#pragma once

#include <CustomFunctions.h>
#include <GameDefinitions/EntitySystem.h>
#include <GameDefinitions/Character.h>
#include <GameDefinitions/Item.h>
#include <GameDefinitions/GameAction.h>
#include <GameDefinitions/Misc.h>
#include <GameDefinitions/TurnManager.h>

namespace osidbg
{
	ecl::EoCClient * GetEoCClient();
	ModManager * GetModManager();

	EntityWorld * GetEntityWorld();
	esv::TurnManager * GetTurnManager();
	esv::TurnBasedProtocol * GetTurnBasedProtocol();
	FixedString ToFixedString(const char * s);
	FixedString MakeFixedString(const char * s);
	FixedString NameGuidToFixedString(char const * nameGuid);

	void * FindComponentByNameGuid(ComponentType componentType, char const * nameGuid, bool logError = true);
	void * FindComponentByHandle(ComponentType componentType, ObjectHandle const & handle, bool logError = true);

	esv::EoCServerObject * FindGameObjectByNameGuid(char const * nameGuid, bool logError = true);
	esv::EoCServerObject * FindGameObjectByHandle(ObjectHandle const & handle, bool logError = true);
	esv::Character * FindCharacterByNameGuid(char const * nameGuid, bool logError = true);
	esv::Character * FindCharacterByHandle(ObjectHandle const & handle, bool logError = true);
	esv::Item * FindItemByNameGuid(char const * nameGuid, bool logError = true);
	esv::Item * FindItemByHandle(ObjectHandle const & handle, bool logError = true);
	esv::GameAction * FindGameActionByHandle(ObjectHandle const & handle);

	struct PropertyMapBase;
	PropertyMapBase & StatusToPropertyMap(esv::Status * status);

	struct ShootProjectileApiHelper
	{
		esv::ShootProjectileHelper Helper;
		esv::ShootProjectileHelperHitObject HitObject;
		bool HasStartPosition{ false };
		bool HasEndPosition{ false };
		bool HasHitObject{ false };
		bool HasHitObjectPosition{ false };

		ShootProjectileApiHelper();
		void SetInt(char const * prop, int32_t value);
		void SetGuidString(char const * prop, char const * value);
		void SetVector(char const * prop, glm::vec3 const & value);
		void SetString(char const * prop, char const * value);
		bool Shoot();
	};


	struct DamageHelpers
	{
		enum HelperType
		{
			HT_CustomHit = 0,
			HT_PrepareHitEvent = 1
		};

		int64_t Handle;
		esv::Character * Source{ nullptr };
		esv::EoCServerObject * Target{ nullptr };

		HelperType Type;
		HitDamageInfo * Hit{ nullptr };
		bool SimulateHit{ false };
		HitType HitType{ HitType::Melee };
		bool RollForDamage{ false };
		bool ProcWindWalker{ false };
		bool ForceReduceDurability{ false };
		// TODO - SkillProperties
		HighGroundBonus HighGround{ HighGroundBonus::Unknown };
		CriticalRoll Critical{ CriticalRoll::Roll };

		// 0 - ASAttack
		// 1 - Character::ApplyDamage, StatusDying, ExecPropertyDamage, StatusDamage
		// 2 - AI hit test
		// 3 - Explode
		// 4 - Trap
		// 5 - InSurface
		// 6 - SetHP, osi::ApplyDamage, StatusConsume
		uint32_t HitReason{ 6 };
		FixedString SkillId;
		Vector3 ImpactPosition;
		bool HasImpactPosition{ false };
		Vector3 ImpactOrigin;
		bool HasImpactOrigin{ false };
		Vector3 ImpactDirection;
		bool HasImpactDirection{ false };

		CauseType DamageSourceType{ CauseType::None };
		float Strength{ 0.0f };

		DamagePairList * DamageList{ nullptr };

		DamageHelpers();
		~DamageHelpers();

		void SetInternalDamageInfo();
		void SetExternalDamageInfo(HitDamageInfo * damageInfo, DamagePairList * damageList);

		bool GetInt(char const * prop, int32_t & value);
		void SetInt(char const * prop, int32_t value);
		void SetVector(char const * prop, Vector3 const & value);
		bool GetString(char const * prop, char const * & value);
		void SetString(char const * prop, char const * value);
		void AddDamage(DamageType DamageType, int32_t Amount);
		esv::StatusHit * Execute();

	private:
		std::unique_ptr<HitDamageInfo> MyDamageInfo;
		std::unique_ptr<DamagePairList> MyDamageList;
	};
}