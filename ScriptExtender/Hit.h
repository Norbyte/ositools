#pragma once

#include <Osiris/Shared/CustomFunctions.h>
#include <Extender/Shared/ExtensionState.h>
#include <GameDefinitions/EntitySystem.h>
#include <GameDefinitions/Components/Character.h>
#include <GameDefinitions/Components/Item.h>
#include <GameDefinitions/GameObjects/GameAction.h>
#include <GameDefinitions/Misc.h>
#include <GameDefinitions/GameObjects/ActionMachine.h>

namespace dse
{
	class ScriptExtender;
}

namespace dse::esv
{
	struct PendingHit
	{
		uint32_t Id;
		ObjectHandle TargetHandle;
		ObjectHandle AttackerHandle;

		// Properties captured during esv::Character::Hit
		bool CapturedCharacterHit{ false };
		CDivinityStats_Item* WeaponStats{ nullptr };
		HitDamageInfo* CharacterHitPointer{ nullptr };
		DamagePairList CharacterHitDamageList;
		HitDamageInfo CharacterHit;
		HitType HitType{ HitType::Melee };
		bool NoHitRoll{ false };
		bool ProcWindWalker{ false };
		bool ForceReduceDurability{ false };
		HighGroundBonus HighGround{ HighGroundBonus::Unknown };
		CriticalRoll CriticalRoll{ CriticalRoll::Roll };

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
		PendingHit* OnCharacterHit(esv::Character* character, CDivinityStats_Character* attacker,
			CDivinityStats_Item* weapon, DamagePairList* damageList, HitType hitType, bool noHitRoll,
			HitDamageInfo* damageInfo, int forceReduceDurability, HighGroundBonus highGround,
			bool procWindWalker, CriticalRoll criticalRoll);
		PendingHit* OnStatusHitSetup(esv::StatusHit* status, HitDamageInfo* hit);
		PendingHit* OnApplyHit(esv::StatusMachine* self, esv::StatusHit* status);
		PendingHit* OnStatusHitEnter(esv::StatusHit* status);
		void OnStatusHitDestroy(esv::StatusHit* status);
		PendingHit* OnCharacterApplyDamage(HitDamageInfo* hit);

		void DeleteHit(PendingHit* hit);

	private:
		uint32_t nextHitId_{ 1 };
		std::unordered_map<uint32_t, std::unique_ptr<PendingHit>> hits_;
		std::unordered_map<HitDamageInfo*, PendingHit*> characterHitMap_;
		std::unordered_map<StatusHit*, PendingHit*> hitStatusMap_;
		std::unordered_map<HitDamageInfo*, PendingHit*> hitStatusDamageMap_;
	};

	class HitProxy
	{
	public:
		HitProxy(dse::ScriptExtender& osiris);

		void PostStartup();

		void OnStatusHitSetup(esv::StatusHit* status, HitDamageInfo* hit);
		void OnStatusHitEnter(esv::Status* status);

		void OnCharacterHit(esv::Character::HitProc* wrappedHit, esv::Character* self, CDivinityStats_Character* attackerStats,
			CDivinityStats_Item* itemStats, DamagePairList* damageList, HitType hitType, bool noHitRoll,
			HitDamageInfo* damageInfo, int forceReduceDurability, CRPGStats_Object_Property_List* skillProperties, HighGroundBonus highGroundFlag,
			bool procWindWalker, CriticalRoll criticalRoll);
		void OnCharacterHitInternal(CDivinityStats_Character::HitInternalProc next, CDivinityStats_Character* self,
			CDivinityStats_Character* attackerStats, CDivinityStats_Item* item, DamagePairList* damageList, HitType hitType, bool noHitRoll,
			bool forceReduceDurability, HitDamageInfo* damageInfo, CRPGStats_Object_Property_List* skillProperties,
			HighGroundBonus highGroundFlag, CriticalRoll criticalRoll);
		void OnCharacterApplyDamage(esv::Character::ApplyDamageProc* next, esv::Character* self, HitDamageInfo& hit,
			uint64_t attackerHandle, CauseType causeType, glm::vec3& impactDirection);

		void OnApplyStatus(esv::StatusMachine::ApplyStatusProc* wrappedApply, esv::StatusMachine* self, esv::Status* status);
		void OnGetSkillDamage(SkillPrototype::GetSkillDamageProc* next, SkillPrototype* self, DamagePairList* damageList,
			CRPGStats_ObjectInstance* attackerStats, bool isFromItem, bool stealthed, float* attackerPosition,
			float* targetPosition, DeathType* pDeathType, int level, bool noRandomization);

	private:
		dse::ScriptExtender& osiris_;
		bool PostLoaded{ false };
	};

}