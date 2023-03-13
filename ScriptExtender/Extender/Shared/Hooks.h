#pragma once

#include <Osiris/Shared/CustomFunctions.h>
#include <Extender/Shared/ExtensionState.h>
#include <GameDefinitions/EntitySystem.h>
#include <GameDefinitions/Components/Character.h>
#include <GameDefinitions/Components/Item.h>
#include <GameDefinitions/Components/Inventory.h>
#include <GameDefinitions/GameObjects/GameAction.h>
#include <GameDefinitions/Net.h>
#include <GameDefinitions/Misc.h>
#include <GameDefinitions/GameObjects/ActionMachine.h>
#include <GameDefinitions/GameObjects/AiHelpers.h>

BEGIN_SE()

class Hooks
{
public:
	void Startup();

	int32_t OnGetHitChance(stats::CDivinityStats_Character__GetHitChance* wrappedGetHitChance,
		stats::Character* attacker, stats::Character* target);
	int32_t OnGetAttackAPCost(stats::Character::GetAttackAPCostProc* wrapped, stats::Character* self);

	bool OnOsiShowNotification(osi::OsirisCallHandlerProc* wrappedHandler, void* proc, osi::ExecutionContext* ctx);

	static esv::Projectile* OnShootProjectile(esv::ProjectileHelpers__ShootProjectile* next, esv::ShootProjectileHelper* helper);
	void OnProjectileExplode(esv::Projectile* projectile);
	void OnActionMachineSetState(esv::ActionMachine* self, uint64_t actionLayer, esv::ActionState* actionState, int* somePtr, bool force, bool setLayer, bool succeeded);
	void OnBeforeActionMachineSetState(esv::ActionMachine* self, uint64_t actionLayer, esv::ActionState* actionState, int* somePtr, bool force, bool setLayer);
	void OnActionMachineResetState(esv::ActionMachine* self, bool force);
	void OnParseStructureFolder(stats::RPGStats* self, ObjectSet<STDString>* paths);
	stats::PropertyData* OnParseSkillProperties(stats::RPGStats::ParsePropertiesProc* next, stats::RPGStats* self, STDString* str);
	void OnSkillFormatDescriptionParam(stats::SkillPrototype::FormatDescriptionParamProc* next, stats::SkillPrototype* skillPrototype,
		stats::Character* tgtCharStats, eoc::Text* eocText, int paramIndex, bool isFromItem,
		float xmm9_4_0, FixedString* paramText, ObjectSet<STDString>* stdStringSet);
#if defined(OSI_EOCAPP)
	int OnGetSkillAPCost(stats::SkillPrototype::GetAttackAPCostProc* next, stats::SkillPrototype* self, stats::Character* character, eoc::AiGrid* aiGrid,
		glm::vec3* position, float* radius, int* pElementalAffinity);
#else
	int OnGetSkillAPCost(stats::SkillPrototype::GetAttackAPCostProc* next, stats::SkillPrototype* self, stats::Character* character, eoc::AiGrid* aiGrid,
		glm::vec3* position, float* radius, bool unused, int* pElementalAffinity);
#endif
	void OnStatusFormatDescriptionParam(stats::StatusPrototype::FormatDescriptionParamProc* next, stats::StatusPrototype* prototype,
		stats::ObjectInstance* owner, stats::ObjectInstance* statusSource, float multiplier,
		eoc::Text* text, int paramIndex, FixedString* param, ObjectSet<STDString>* paramSet);
	void OnUpdateTurnOrder(esv::TurnManager* self, uint8_t combatId);
	void OnCreateUIObject(UIObjectManager* self, ComponentHandle* handle, unsigned int layerIndex, unsigned int creatorId, UIObjectFlags flags,
		uint64_t resourceFlags, PlayerId playerId, ComponentHandle* result);
	void OnExecutePropertyDataOnGroundHit(glm::vec3* position, uint64_t casterHandle, stats::DamagePairList* damageList, stats::PropertyList* propertyList,
		stats::DamageType damageType);
	void OnExecuteCharacterSetExtraProperties(stats::PropertyList* extraProperties, uint64_t attackerHandle,
		ObjectSet<esv::Character*> const& targets, glm::vec3 const& impactOrigin, stats::PropertyContext propertyContext,
		bool isFromItem, stats::SkillPrototype* skillProto, stats::HitDamageInfo* damageInfo, float statusStartTimer, esv::Character* refTarget,
		bool statusFlag0x40, float a12);
	void OnExecutePropertyDataOnPositionOnly(stats::PropertyList* properties, uint64_t attackerHandle,
		glm::vec3 const* position, float areaRadius, stats::PropertyContext propertyContext, bool isFromItem,
		stats::SkillPrototype* skillPrototype, stats::HitDamageInfo* damageInfo, float unkn);

	void OnSortAiActions(esv::AiHelpers::SortActionsProc* next, Set<esv::AiAction>* actions);
	void OnPeekAiAction(esv::AiHelpers* self, esv::Character* character, AiActionType actionType, bool isFinished);

	esv::Item* OnGenerateTreasureItem(esv::ItemHelpers__GenerateTreasureItem* next, stats::TreasureObjectInfo* treasureInfo, int level);
	bool OnCraftingExecuteCombination(esv::CombineManager::ExecuteCombinationProc* next, esv::CombineManager* self, CraftingStationType craftingStation,
		ObjectSet<ComponentHandle>* ingredientHandles, esv::Character* character, uint8_t quantity, char openUI, FixedString* combinationId);


	void OnAppInputEvent(IApp* self, uint16_t& retval, InputEvent const& inputEvent);
	void OnInjectInput(InputManager* self, InjectInputData const& input);
	bool OnPeerModuleLoaded(esv::LoadProtocol::HandleModuleLoadedProc* next, esv::LoadProtocol* self, esv::LoadProtocol::PeerModSettings& peerModSettings,
		ModuleSettings& hostModSettings);
	int OnInventoryProtocolPostUpdate(ecl::InventoryProtocol::PostUpdateProc* next, ecl::InventoryProtocol* self);
	Visual* OnCreateEquipmentVisuals(ecl::EquipmentVisualsSystem::CreateVisualsProc* wrapped, ecl::EquipmentVisualsSystem*,
		EntityHandle entityHandle, ecl::EquipmentVisualSystemSetParam& params);

	bool OnCheckRequirement(stats::CheckRequirementProc* wrapped, stats::Character* self, bool isInCombat, bool isImmobile, bool hasCharges,
		ObjectSet<FixedString> const* tags, stats::Requirement const& requirement, bool excludeBoosts);
	TranslatedString * OnRequirementToTranslatedString(stats::RequirementToTranslatedStringProc* wrapped, TranslatedString* text, stats::RequirementType requirementId, bool negate);
	stats::RequirementType OnStringToRequirement(stats::StringToRequirementProc* wrapped, char const* requirement);
#if defined(OSI_EOCAPP)
	bool OnReevaluateRequirements(stats::Character::ReevaluateRequirementsProc* wrapped, stats::Character* self, stats::ItemSlot32 slot, stats::Requirement* pRequirement);
#else
	bool OnReevaluateRequirements(stats::Character::ReevaluateRequirementsProc* wrapped, stats::Character* self, ItemSlot32 slot, bool checkRequirements, stats::Requirement* pRequirement);
#endif

	void OnItemSetupDescriptionToFlash(ecl::Item::SetupDescriptionToFlashProc* wrapped, ecl::Item* self, ig::FlashObject* flash, char const* path,
		uint32_t displayContext, ecl::Character* owner, void* iconDrawStruct, int amount);
	uint64_t OnServerCharacterCheckSkillRequirements(esv::Character::CheckSkillRequirementsProc* wrapped, esv::Character* self, FixedString const& skillId, esv::Item* item, bool checkAP, bool mustHaveSkill);
	uint64_t OnClientCharacterCheckSkillRequirements(ecl::Character::CheckSkillRequirementsProc* wrapped, ecl::Character* self, FixedString const& skillId, ecl::Item* item);
	void OnGetSkillRequirements(ecl::Character::GetSkillRequirementsProc* wrapped, ecl::Character* character, FixedString const& skillId, ecl::Item* item, uint32_t checkRequirementFlags, eoc::Text& requirementsText);

	bool OnSkillManagerCanMemorize(esv::SkillManager::CanMemorizeProc* wrapped, esv::SkillManager* self, stats::SkillPrototype* skill, bool checkMemoryRequirement);
	bool OnSkillManagerCheckSkillRequirements(ecl::SkillManager::CheckSkillRequirementsProc* wrapped, ecl::SkillManager* self, stats::SkillPrototype* proto);

private:
	bool loaded_{ false };
};

END_SE()
