#pragma once

#include "CustomFunctions.h"
#include "ExtensionState.h"
#include <GameDefinitions/EntitySystem.h>
#include <GameDefinitions/Character.h>
#include <GameDefinitions/Item.h>
#include <GameDefinitions/GameAction.h>
#include <GameDefinitions/Misc.h>
#include <GameDefinitions/ActionMachine.h>

namespace dse
{
	class OsirisProxy;
}

namespace dse::esv
{
	class HitProxy
	{
	public:
		HitProxy(OsirisProxy& osiris);

		void PostStartup();

		void OnStatusHitEnter(esv::Status* status);

		void OnCharacterHit(esv::Character::HitProc wrappedHit, esv::Character* self, CDivinityStats_Character* attackerStats,
			CDivinityStats_Item* itemStats, DamagePairList* damageList, HitType hitType, bool noHitRoll,
			HitDamageInfo* damageInfo, int forceReduceDurability, CRPGStats_Object_Property_List* skillProperties, HighGroundBonus highGroundFlag,
			bool procWindWalker, CriticalRoll criticalRoll);
		void OnCharacterHitInternal(CDivinityStats_Character::HitInternalProc next, CDivinityStats_Character* self,
			CDivinityStats_Character* attackerStats, CDivinityStats_Item* item, DamagePairList* damageList, HitType hitType, bool noHitRoll,
			bool forceReduceDurability, HitDamageInfo* damageInfo, CRPGStats_Object_Property_List* skillProperties,
			HighGroundBonus highGroundFlag, CriticalRoll criticalRoll);
		void OnCharacterApplyDamage(esv::Character::ApplyDamageProc next, esv::Character* self, HitDamageInfo& hit,
			uint64_t attackerHandle, CauseType causeType, glm::vec3& impactDirection);

		void OnApplyStatus(esv::StatusMachine__ApplyStatus wrappedApply, esv::StatusMachine* self, esv::Status* status);
		void OnGetSkillDamage(SkillPrototype::GetSkillDamage next, SkillPrototype* self, DamagePairList* damageList,
			CRPGStats_ObjectInstance* attackerStats, bool isFromItem, bool stealthed, float* attackerPosition,
			float* targetPosition, DeathType* pDeathType, int level, bool noRandomization);

	private:
		OsirisProxy& osiris_;
		bool PostLoaded{ false };
	};

}