#pragma once

#include "../CustomFunctions.h"
#include "../ExtensionState.h"
#include <GameDefinitions/EntitySystem.h>
#include <GameDefinitions/Character.h>
#include <GameDefinitions/Item.h>
#include <GameDefinitions/GameAction.h>
#include <GameDefinitions/Misc.h>
#include <GameDefinitions/ActionMachine.h>

namespace dse
{
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
		void RegisterCustomStatFunctions();
		void RegisterLuaFunctions();

		void PostStartup();
		void OnBaseModuleLoadedServer();
		void OnBaseModuleLoadedClient();

		int32_t OnStatusGetEnterChance(esv::Status__GetEnterChance wrappedGetEnterChance, 
			esv::Status * status, bool useCharacterStats);
		int32_t OnGetHitChance(CDivinityStats_Character__GetHitChance * wrappedGetHitChance, 
			CDivinityStats_Character * attacker, CDivinityStats_Character * target);
		void OnStatusHitEnter(esv::Status * status);
		void OnStatusHealEnter(esv::Status * status);
		void OnCharacterHit(esv::Character__Hit wrappedHit, esv::Character * self, CDivinityStats_Character * attackerStats,
			CDivinityStats_Item * itemStats, DamagePairList * damageList, HitType hitType, bool noHitRoll,
			HitDamageInfo * damageInfo, int forceReduceDurability, CRPGStats_Object_Property_List * skillProperties, HighGroundBonus highGroundFlag,
			bool procWindWalker, CriticalRoll criticalRoll);
		void OnCharacterHitInternal(CDivinityStats_Character::HitInternalProc next, CDivinityStats_Character * self, 
			CDivinityStats_Character *attackerStats, CDivinityStats_Item *item, DamagePairList *damageList, HitType hitType, bool noHitRoll,
			bool forceReduceDurability, HitDamageInfo *damageInfo, CRPGStats_Object_Property_List *skillProperties,
			HighGroundBonus highGroundFlag, CriticalRoll criticalRoll);
		void OnApplyStatus(esv::StatusMachine__ApplyStatus wrappedApply, esv::StatusMachine * self, esv::Status * status);
		void OnActionMachineSetState(esv::ActionMachine * self, uint64_t actionLayer, esv::ActionState * actionState, int * somePtr, bool force, bool setLayer, bool succeeded);
		void OnSkillFormatDescriptionParam(SkillPrototype::FormatDescriptionParam next, SkillPrototype *skillPrototype,
			CDivinityStats_Character *tgtCharStats, eoc::Text *eocText, int paramIndex, __int64 isFromItem,
			float xmm9_4_0, FixedString * paramText, ObjectSet<STDString> * stdStringSet);
		void OnGetSkillDamage(SkillPrototype::GetSkillDamage next, SkillPrototype * self, DamagePairList * damageList,
			CRPGStats_ObjectInstance *attackerStats, bool isFromItem, bool stealthed, float * attackerPosition,
			float * targetPosition, DeathType * pDeathType, int level, bool noRandomization);
		void OnStatusFormatDescriptionParam(StatusPrototype::FormatDescriptionParam next, StatusPrototype *prototype, 
			CDivinityStats_Character *statusSource, CDivinityStats_Character *targetCharacter, float multiplier, 
			eoc::Text * text, int paramIndex, FixedString * param, ObjectSet<STDString> * paramSet);
		void OnUpdateTurnOrder(esv::TurnManager * self, uint8_t combatId);

	private:
		OsirisProxy & osiris_;
		bool PostLoaded{ false };
	};

}