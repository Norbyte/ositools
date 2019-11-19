#pragma once

#include "../CustomFunctions.h"
#include "../ExtensionState.h"
#include <GameDefinitions/EntitySystem.h>
#include <GameDefinitions/Character.h>
#include <GameDefinitions/Item.h>
#include <GameDefinitions/GameAction.h>
#include <GameDefinitions/Misc.h>

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

		void PostStartup();
		void OnBaseModuleLoaded();

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