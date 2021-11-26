#pragma once

#include <Osiris/Shared/CustomFunctions.h>
#include <Extender/Shared/ExtensionState.h>
#include <GameDefinitions/EntitySystem.h>
#include <GameDefinitions/Components/Character.h>
#include <GameDefinitions/Components/Item.h>
#include <GameDefinitions/GameObjects/GameAction.h>
#include <GameDefinitions/Net.h>
#include <GameDefinitions/Misc.h>
#include <GameDefinitions/GameObjects/ActionMachine.h>

namespace dse::esv
{
	class CustomFunctionLibrary
	{
	public:
		CustomFunctionLibrary(OsirisExtender & osiris);

		void Startup();
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

		int32_t OnStatusGetEnterChance(esv::Status::GetEnterChanceProc* wrappedGetEnterChance, 
			esv::Status * status, bool isEnterCheck);
		int32_t OnGetHitChance(stats::CDivinityStats_Character__GetHitChance * wrappedGetHitChance,
			stats::Character * attacker, stats::Character * target);
		void OnStatusHealEnter(esv::Status * status);

		static Projectile* OnShootProjectile(esv::ProjectileHelpers__ShootProjectile* next, ShootProjectileHelper* helper);
		void OnProjectileExplode(Projectile* projectile);
		void OnActionMachineSetState(esv::ActionMachine * self, uint64_t actionLayer, esv::ActionState * actionState, int * somePtr, bool force, bool setLayer, bool succeeded);
		void OnBeforeActionMachineSetState(esv::ActionMachine* self, uint64_t actionLayer, esv::ActionState* actionState, int* somePtr, bool force, bool setLayer);
		void OnActionMachineResetState(esv::ActionMachine* self, bool force);
		stats::PropertyData* OnParseSkillProperties(stats::RPGStats::ParsePropertiesProc* next, stats::RPGStats* self, STDString* str);
		void OnSkillFormatDescriptionParam(stats::SkillPrototype::FormatDescriptionParamProc* next, stats::SkillPrototype *skillPrototype,
			stats::Character *tgtCharStats, eoc::Text *eocText, int paramIndex, bool isFromItem,
			float xmm9_4_0, FixedString * paramText, ObjectSet<STDString> * stdStringSet);
#if defined(OSI_EOCAPP)
		int OnGetSkillAPCost(stats::SkillPrototype::GetAttackAPCostProc* next, stats::SkillPrototype* self, stats::Character* character, eoc::AiGrid* aiGrid,
			glm::vec3* position, float* radius, int* pElementalAffinity);
#else
		int OnGetSkillAPCost(stats::SkillPrototype::GetAttackAPCostProc* next, stats::SkillPrototype* self, stats::Character* character, eoc::AiGrid* aiGrid,
			glm::vec3* position, float* radius, bool unused, int* pElementalAffinity);
#endif
		void OnStatusFormatDescriptionParam(stats::StatusPrototype::FormatDescriptionParamProc* next, stats::StatusPrototype *prototype,
			stats::ObjectInstance* owner, stats::ObjectInstance* statusSource, float multiplier,
			eoc::Text * text, int paramIndex, FixedString * param, ObjectSet<STDString> * paramSet);
		void OnUpdateTurnOrder(esv::TurnManager * self, uint8_t combatId);
		void OnCreateUIObject(UIObjectManager* self, ComponentHandle* handle, unsigned int layerIndex, unsigned int creatorId, int flags, 
			uint64_t resourceFlags, uint16_t playerId, ComponentHandle* result);
		void OnExecutePropertyDataOnGroundHit(glm::vec3* position, uint64_t casterHandle, stats::DamagePairList* damageList, stats::PropertyList* propertyList,
			stats::DamageType damageType);
		void OnExecuteCharacterSetExtraProperties(stats::PropertyList* extraProperties, uint64_t attackerHandle,
			ObjectSet<esv::Character*> const& targets, glm::vec3 const& impactOrigin, stats::PropertyContext propertyContext,
			bool isFromItem, stats::SkillPrototype* skillProto, stats::HitDamageInfo* damageInfo, float statusStartTimer, esv::Character* refTarget,
			bool statusFlag0x40, float a12);
		void OnExecutePropertyDataOnPositionOnly(stats::PropertyList* properties, uint64_t attackerHandle,
			glm::vec3 const* position, float areaRadius, stats::PropertyContext propertyContext, bool isFromItem,
			stats::SkillPrototype* skillPrototype, stats::HitDamageInfo* damageInfo, float unkn);

		esv::Item* OnGenerateTreasureItem(esv::ItemHelpers__GenerateTreasureItem* next, stats::TreasureObjectInfo* treasureInfo, int level);
		bool OnCraftingExecuteCombination(esv::CombineManager::ExecuteCombinationProc* next, esv::CombineManager* self, CraftingStationType craftingStation, 
			ObjectSet<ComponentHandle>* ingredientHandles, esv::Character* character, uint8_t quantity, char openUI, FixedString* combinationId);


		void OnAppInputEvent(App* self, uint16_t& retval, InputEvent const& inputEvent);
		bool OnPeerModuleLoaded(LoadProtocol::HandleModuleLoadedProc* next, LoadProtocol* self, LoadProtocol::PeerModSettings& peerModSettings, 
			ModuleSettings& hostModSettings);

	private:
		OsirisExtender& osiris_;
		bool loaded_{ false };
	};

}