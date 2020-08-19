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
	class OsirisProxy;
	struct UIObjectManager;
}

namespace dse::esv
{
	class CustomFunctionLibrary
	{
	public:
		CustomFunctionLibrary(OsirisProxy & osiris);

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

		int32_t OnStatusGetEnterChance(esv::Status::GetEnterChanceProc* wrappedGetEnterChance, 
			esv::Status * status, bool isEnterCheck);
		int32_t OnGetHitChance(CDivinityStats_Character__GetHitChance * wrappedGetHitChance, 
			CDivinityStats_Character * attacker, CDivinityStats_Character * target);
		void OnStatusHealEnter(esv::Status * status);
		bool CharacterHasMadnessFix(esv::Character::HasMadnessProc* wrapped, esv::Character* character);
		bool EclCharacterHasMadnessFix(ecl::Character::HasMadnessProc* wrapped, ecl::Character* character);

		static Projectile* OnShootProjectile(esv::ProjectileHelpers__ShootProjectile* next, ShootProjectileHelper* helper);
		void OnProjectileExplode(Projectile* projectile);
		void OnActionMachineSetState(esv::ActionMachine * self, uint64_t actionLayer, esv::ActionState * actionState, int * somePtr, bool force, bool setLayer, bool succeeded);
		void OnBeforeActionMachineSetState(esv::ActionMachine* self, uint64_t actionLayer, esv::ActionState* actionState, int* somePtr, bool force, bool setLayer);
		void OnActionMachineResetState(esv::ActionMachine* self, bool force);
		void OnSkillFormatDescriptionParam(SkillPrototype::FormatDescriptionParamProc* next, SkillPrototype *skillPrototype,
			CDivinityStats_Character *tgtCharStats, eoc::Text *eocText, int paramIndex, bool isFromItem,
			float xmm9_4_0, FixedString * paramText, ObjectSet<STDString> * stdStringSet);
#if defined(OSI_EOCAPP)
		int OnGetSkillAPCost(SkillPrototype::GetAttackAPCostProc* next, SkillPrototype* self, CDivinityStats_Character* character, eoc::AiGrid* aiGrid,
			glm::vec3* position, float* radius, int* pElementalAffinity);
#else
		int OnGetSkillAPCost(SkillPrototype::GetAttackAPCostProc* next, SkillPrototype* self, CDivinityStats_Character* character, eoc::AiGrid* aiGrid,
			glm::vec3* position, float* radius, bool unused, int* pElementalAffinity);
#endif
		void OnStatusFormatDescriptionParam(StatusPrototype::FormatDescriptionParamProc* next, StatusPrototype *prototype, 
			CRPGStats_ObjectInstance* owner, CRPGStats_ObjectInstance* statusSource, float multiplier,
			eoc::Text * text, int paramIndex, FixedString * param, ObjectSet<STDString> * paramSet);
		void OnUpdateTurnOrder(esv::TurnManager * self, uint8_t combatId);
		void OnCreateUIObject(UIObjectManager* self, ObjectHandle* handle, unsigned int layerIndex, unsigned int creatorId, int flags, 
			uint64_t resourceFlags, uint16_t playerId, ObjectHandle* result);
		void OnExecutePropertyDataOnGroundHit(glm::vec3* position, uint64_t casterHandle, DamagePairList* damageList, CRPGStats_Object_Property_List* propertyList,
			DamageType damageType);

		esv::Item* OnGenerateTreasureItem(esv::ItemHelpers__GenerateTreasureItem* next, RPGStats_Treasure_Object_Info* treasureInfo, int level);
		bool OnCraftingExecuteCombination(esv::CombineManager::ExecuteCombinationProc* next, esv::CombineManager* self, CraftingStationType craftingStation, 
			ObjectSet<ObjectHandle>* ingredientHandles, esv::Character* character, uint8_t quantity, char openUI, FixedString* combinationId);

		void ThrowStatusHitEnter(esv::Status* status);
		void ThrowCharacterHit(esv::Character* self, CDivinityStats_Character* attackerStats,
			CDivinityStats_Item* itemStats, DamagePairList* damageList, HitType hitType, bool noHitRoll,
			HitDamageInfo* damageInfo, int forceReduceDurability, CRPGStats_Object_Property_List* skillProperties, HighGroundBonus highGround,
			bool procWindWalker, CriticalRoll criticalRoll, DamageHelpers& helper);
		void ThrowApplyStatus(esv::StatusMachine* self, esv::Status* status);

	private:
		OsirisProxy & osiris_;
		bool PostLoaded{ false };
	};

}