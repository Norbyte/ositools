#pragma once

#include "../CustomFunctions.h"
#include "../ExtensionState.h"
#include <GameDefinitions/EntitySystem.h>
#include <GameDefinitions/Character.h>
#include <GameDefinitions/Item.h>
#include <GameDefinitions/GameAction.h>
#include <GameDefinitions/Misc.h>
#include <GameDefinitions/ActionMachine.h>

namespace osidbg
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
		void OnBaseModuleLoaded();

		int32_t OnStatusGetEnterChance(esv::Status__GetEnterChance wrappedGetEnterChance, 
			esv::Status * status, bool useCharacterStats);
		int32_t OnGetHitChance(CDivinityStats_Character__GetHitChance * wrappedGetHitChance, 
			CDivinityStats_Character * attacker, CDivinityStats_Character * target);
		void OnStatusHitEnter(esv::Status * status);
		void OnStatusHealEnter(esv::Status * status);
		void OnCharacterHit(esv::Character__Hit wrappedHit, esv::Character * self, CDivinityStats_Character * attackerStats,
			CDivinityStats_Item * itemStats, DamagePairList * damageList, HitType hitType, bool rollForDamage,
			HitDamageInfo * damageInfo, int forceReduceDurability, void * skillProperties, HighGroundBonus highGroundFlag, 
			bool procWindWalker, CriticalRoll criticalRoll);
		void OnApplyStatus(esv::StatusMachine__ApplyStatus wrappedApply, esv::StatusMachine * self, esv::Status * status);
		void OnActionMachineSetState(esv::ActionMachine * self, uint64_t actionLayer, esv::ActionState * actionState, int * somePtr, bool force, bool setLayer, bool succeeded);

		void EnableStatOverride(char const * stat);
	private:
		OsirisProxy & osiris_;
		bool PostLoaded{ false };
	};

}