#pragma once

#include "../CustomFunctions.h"

namespace osidbg
{
	FixedString ToFixedString(const char * s);
	char const * NameGuidToFixedString(std::string const & nameGuid);
	esv::EoCServerObject * FindGameObjectByNameGuid(std::string const & nameGuid, bool logError = true);
	esv::EoCServerObject * FindGameObjectByHandle(ObjectHandle const & handle, bool logError = true);
	esv::Character * FindCharacterByNameGuid(std::string const & nameGuid, bool logError = true);
	esv::Character * FindCharacterByHandle(ObjectHandle const & handle, bool logError = true);
	esv::Item * FindItemByNameGuid(std::string const & nameGuid, bool logError = true);
	esv::Item * FindItemByHandle(ObjectHandle const & handle, bool logError = true);
	esv::GameAction * FindGameActionByHandle(ObjectHandle const & handle);


	struct ShootProjectileApiHelper
	{
		esv::ShootProjectileHelper Helper;
		bool HasStartPosition{ false };
		bool HasEndPosition{ false };

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
		bool CallCharacterHit{ false };
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

	class DamageHelperPool
	{
	public:
		void Clear();
		DamageHelpers * Create();
		bool Destroy(int64_t handle);
		DamageHelpers * Get(int64_t handle) const;

	private:
		std::unordered_map<int64_t, std::unique_ptr<DamageHelpers>> helpers_;
		int64_t nextHelperId_{ 0 };
	};

	class ExtensionState
	{
	public:
		DamageHelperPool DamageHelpers;

		void Reset();
	};

	class CustomFunctionLibrary
	{
	public:
		CustomFunctionLibrary(class OsirisProxy & osiris);

		void Register();
		void RegisterHelperFunctions();
		void RegisterMathFunctions();
		void RegisterStatFunctions();
		void RegisterStatusFunctions();
		void RegisterGameActionFunctions();
		void RegisterProjectileFunctions();
		void RegisterHitFunctions();
		void RegisterPlayerFunctions();
		void RegisterItemFunctions();
		void RegisterCharacterFunctions();

		void PostStartup();

		void OnStatusHitEnter(esv::Status * status);
		void OnStatusHealEnter(esv::Status * status);
		void OnCharacterHit(esv::Character__Hit wrappedHit, esv::Character * self, CDivinityStats_Character * attackerStats,
			CDivinityStats_Item * itemStats, DamagePairList * damageList, HitType hitType, bool rollForDamage,
			HitDamageInfo * damageInfo, int forceReduceDurability, void * skillProperties, HighGroundBonus highGroundFlag, 
			bool procWindWalker, CriticalRoll criticalRoll);
		void OnApplyStatus(esv::StatusMachine__ApplyStatus wrappedApply, esv::StatusMachine * self, esv::Status * status);

	private:
		OsirisProxy & osiris_;
		bool PostLoaded{ false };
	};

}