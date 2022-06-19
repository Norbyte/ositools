#pragma once

#include <Osiris/Shared/CustomFunctions.h>
#include <Extender/Shared/ExtensionState.h>
#include <GameDefinitions/EntitySystem.h>
#include <GameDefinitions/Components/Character.h>
#include <GameDefinitions/Components/Item.h>
#include <GameDefinitions/GameObjects/GameAction.h>
#include <GameDefinitions/Misc.h>
#include <GameDefinitions/GameObjects/ActionMachine.h>

namespace dse::esv
{
	struct PendingHit
	{
		uint32_t Id;
		ComponentHandle TargetHandle;
		ComponentHandle AttackerHandle;

		// Properties captured during esv::Character::Hit
		bool CapturedCharacterHit{ false };
		stats::Item* WeaponStats{ nullptr };
		stats::HitDamageInfo* CharacterHitPointer{ nullptr };
		stats::DamagePairList CharacterHitDamageList;
		stats::HitDamageInfo CharacterHit;
		stats::HitType HitType{ stats::HitType::Melee };
		bool NoHitRoll{ false };
		bool ProcWindWalker{ false };
		bool ForceReduceDurability{ false };
		stats::HighGroundBonus HighGround{ stats::HighGroundBonus::Unknown };
		stats::CriticalRoll CriticalRoll{ stats::CriticalRoll::Roll };

		// Captured during esv::StatusHit::Setup
		bool CapturedStatusSetup{ false };
		bool CapturedStatusApply{ false };
		StatusHit* Status{ nullptr };

		// Captured during esv::StatusHit::Enter
		bool CapturedStatusEnter{ false };
	};

	class PendingHitManager
	{
	public:
		PendingHit* OnCharacterHit(esv::Character* character, stats::Character* attacker,
			stats::Item* weapon, stats::DamagePairList* damageList, stats::HitType hitType, bool noHitRoll,
			stats::HitDamageInfo* damageInfo, int forceReduceDurability, stats::HighGroundBonus highGround,
			bool procWindWalker, stats::CriticalRoll criticalRoll);
		PendingHit* OnStatusHitSetup(esv::StatusHit* status, stats::HitDamageInfo* hit);
		PendingHit* OnApplyHit(esv::StatusMachine* self, esv::StatusHit* status);
		PendingHit* OnStatusHitEnter(esv::StatusHit* status);
		void OnStatusHitDestroy(esv::StatusHit* status);
		PendingHit* OnCharacterApplyDamage(stats::HitDamageInfo* hit);

		void DeleteHit(PendingHit* hit);

	private:
		uint32_t nextHitId_{ 1 };
		std::unordered_map<uint32_t, std::unique_ptr<PendingHit>> hits_;
		std::unordered_map<stats::HitDamageInfo*, PendingHit*> characterHitMap_;
		std::unordered_map<StatusHit*, PendingHit*> hitStatusMap_;
		std::unordered_map<stats::HitDamageInfo*, PendingHit*> hitStatusDamageMap_;
	};

	class HitProxy
	{
	public:
		HitProxy(dse::ScriptExtender& osiris);

		void PostStartup();

		void OnStatusHitSetup(esv::StatusHit* status, stats::HitDamageInfo* hit);
		void OnStatusHitEnter(esv::Status* status);

		void OnCharacterHit(esv::Character::HitProc* wrappedHit, esv::Character* self, stats::Character* attackerStats,
			stats::Item* itemStats, stats::DamagePairList* damageList, stats::HitType hitType, bool noHitRoll,
			stats::HitDamageInfo* damageInfo, int forceReduceDurability, stats::PropertyList* skillProperties, stats::HighGroundBonus highGroundFlag,
			bool procWindWalker, stats::CriticalRoll criticalRoll);
		void OnCharacterHitInternal(stats::Character::HitInternalProc next, stats::Character* self,
			stats::Character* attackerStats, stats::Item* item, stats::DamagePairList* damageList, stats::HitType hitType, bool noHitRoll,
			bool forceReduceDurability, stats::HitDamageInfo* damageInfo, stats::PropertyList* skillProperties,
			stats::HighGroundBonus highGroundFlag, stats::CriticalRoll criticalRoll);
		void OnCharacterApplyDamage(esv::Character::ApplyDamageProc* next, esv::Character* self, stats::HitDamageInfo& hit,
			uint64_t attackerHandle, CauseType causeType, glm::vec3& impactDirection);

		void OnApplyStatus(esv::StatusMachine::ApplyStatusProc* wrappedApply, esv::StatusMachine* self, esv::Status* status);
		void OnGetSkillDamage(stats::SkillPrototype::GetSkillDamageProc* next, stats::SkillPrototype* self, stats::DamagePairList* damageList,
			stats::ObjectInstance* attackerStats, bool isFromItem, bool stealthed, glm::vec3 const& attackerPosition,
			glm::vec3 const& targetPosition, stats::DeathType* pDeathType, int level, bool noRandomization);

	private:
		dse::ScriptExtender& osiris_;
		bool PostLoaded{ false };
	};

}