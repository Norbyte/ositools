#include <stdafx.h>
#include <Extender/Shared/Hooks.h>
#include <Extender/ScriptExtender.h>
#include <ScriptHelpers.h>

BEGIN_NS(esv)

extern FunctionHandle ActionStateEnterHandle;
extern FunctionHandle ActionStateExitHandle;

END_NS()

BEGIN_SE()

void Hooks::Startup()
{
	if (loaded_) {
		return;
	}

	using namespace std::placeholders;
	auto& lib = gExtender->GetEngineHooks();
	// CreateUIObject always enabled for main menu version display
	lib.UIObjectManager__CreateUIObject.SetPostHook(
		std::bind(&Hooks::OnCreateUIObject, this, _1, _2, _3, _4, _5, _6, _7, _8)
	);

	lib.esv__ActionMachine__SetState.AddPreHook(
		std::bind(&Hooks::OnBeforeActionMachineSetState, this, _1, _2, _3, _4, _5, _6)
	);
	lib.esv__ActionMachine__SetState.AddPostHook(
		std::bind(&Hooks::OnActionMachineSetState, this, _1, _2, _3, _4, _5, _6, _7)
	);
	lib.esv__ActionMachine__ResetState.SetPreHook(
		std::bind(&Hooks::OnActionMachineResetState, this, _1, _2)
	);
	lib.esv__ProjectileHelpers__ShootProjectile.SetWrapper(
		std::bind(&Hooks::OnShootProjectile, _1, _2)
	);
	lib.esv__Projectile__Explode.SetPreHook(
		std::bind(&Hooks::OnProjectileExplode, this, _1)
	);

	lib.RPGStats__ParseStructureFolder.SetPostHook(
		std::bind(&Hooks::OnParseStructureFolder, this, _1, _2)
	);
	lib.RPGStats__ParseProperties.SetWrapper(
		std::bind(&Hooks::OnParseSkillProperties, this, _1, _2, _3)
	);
	lib.SkillPrototype__FormatDescriptionParam.SetWrapper(
		std::bind(&Hooks::OnSkillFormatDescriptionParam, this, _1, _2, _3, _4, _5, _6, _7, _8, _9)
	);
#if defined(OSI_EOCAPP)
	lib.SkillPrototype__GetAttackAPCost.SetWrapper(
		std::bind(&Hooks::OnGetSkillAPCost, this, _1, _2, _3, _4, _5, _6, _7)
	);
#else
	lib.SkillPrototype__GetAttackAPCost.SetWrapper(
		std::bind(&Hooks::OnGetSkillAPCost, this, _1, _2, _3, _4, _5, _6, _7, _8)
	);
#endif
	lib.StatusPrototype__FormatDescriptionParam.SetWrapper(
		std::bind(&Hooks::OnStatusFormatDescriptionParam, this, _1, _2, _3, _4, _5, _6, _7, _8, _9)
	);
	lib.esv__TurnManager__UpdateTurnOrder.SetPostHook(
		std::bind(&Hooks::OnUpdateTurnOrder, this, _1, _2)
	);
	lib.esv__ItemHelpers__GenerateTreasureItem.SetWrapper(
		std::bind(&Hooks::OnGenerateTreasureItem, this, _1, _2, _3)
	);
	lib.esv__CombineManager__ExecuteCombination.SetWrapper(
		std::bind(&Hooks::OnCraftingExecuteCombination, this, _1, _2, _3, _4, _5, _6, _7, _8)
	);
	lib.esv__ExecutePropertyDataOnGroundHit.SetPostHook(
		std::bind(&Hooks::OnExecutePropertyDataOnGroundHit, this, _1, _2, _3, _4, _5)
	);
	lib.esv__ExecutePropertyDataOnPositionOnly.SetPostHook(
		std::bind(&Hooks::OnExecutePropertyDataOnPositionOnly, this, _1, _2, _3, _4, _5, _6, _7, _8, _9)
	);
	lib.esv__ExecuteCharacterSetExtraProperties.SetPostHook(
		std::bind(&Hooks::OnExecuteCharacterSetExtraProperties, this, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12)
	);
	lib.esv__AiHelpers__SortActions.SetWrapper(
		std::bind(&Hooks::OnSortAiActions, this, _1, _2)
	);
	lib.esv__AiHelpers__PeekAction.SetPreHook(
		std::bind(&Hooks::OnPeekAiAction, this, _1, _2, _3, _4)
	);
	GetStaticSymbols().CharStatsGetters.WrapperHitChance.SetWrapper(
		std::bind(&Hooks::OnGetHitChance, this, _1, _2, _3)
	);
	lib.esv__LoadProtocol__HandleModuleLoaded.SetWrapper(
		std::bind(&Hooks::OnPeerModuleLoaded, this, _1, _2, _3, _4)
	);
	lib.App__OnInputEvent.SetPreHook(
		std::bind(&Hooks::OnAppInputEvent, this, _1, _2, _3)
	);
	lib.ecl__InventoryProtocol__PostUpdate.SetWrapper(
		std::bind(&Hooks::OnInventoryProtocolPostUpdate, this, _1, _2)
	);
	lib.ls__InputManager__InjectInput.SetPreHook(
		std::bind(&Hooks::OnInjectInput, this, _1, _2)
	);
	lib.ecl__EquipmentVisualsSystem__CreateVisuals.SetWrapper(
		std::bind(&Hooks::OnCreateEquipmentVisuals, this, _1, _2, _3, _4)
	);
#if defined(OSI_EOCAPP)
	lib.osi__ShowNotification.SetWrapper(
		std::bind(&Hooks::OnOsiShowNotification, this, _1, _2, _3)
	);
#endif
	lib.CDivinityStats_Character__GetAttackAPCost.SetWrapper(
		std::bind(&Hooks::OnGetAttackAPCost, this, _1, _2)
	);

	lib.CheckRequirement1.SetWrapper(
		std::bind(&Hooks::OnCheckRequirement, this, _1, _2, _3, _4, _5, _6, _7, _8)
	);
	lib.CheckRequirement2.SetWrapper(
		std::bind(&Hooks::OnCheckRequirement, this, _1, _2, _3, _4, _5, _6, _7, _8)
	);
	lib.RequirementToTranslatedString.SetWrapper(
		std::bind(&Hooks::OnRequirementToTranslatedString, this, _1, _2, _3, _4)
	);
	lib.StringToRequirement.SetWrapper(
		std::bind(&Hooks::OnStringToRequirement, this, _1, _2)
	);
#if defined(OSI_EOCAPP)
	lib.CDivinityStats_Character__ReevaluateRequirements.SetWrapper(
		std::bind(&Hooks::OnReevaluateRequirements, this, _1, _2, _3, _4)
	);
#else
	lib.CDivinityStats_Character__ReevaluateRequirements.SetWrapper(
		std::bind(&Hooks::OnReevaluateRequirements, this, _1, _2, _3, _4, _5)
	);
#endif
	lib.ecl__Item__SetupDescriptionToFlash.SetWrapper(
		std::bind(&Hooks::OnItemSetupDescriptionToFlash, this, _1, _2, _3, _4, _5, _6, _7, _8)
	);
	lib.esv__Character__CheckSkillRequirements.SetWrapper(
		std::bind(&Hooks::OnServerCharacterCheckSkillRequirements, this, _1, _2, _3, _4, _5, _6)
	);
	lib.ecl__Character__CheckSkillRequirements.SetWrapper(
		std::bind(&Hooks::OnClientCharacterCheckSkillRequirements, this, _1, _2, _3, _4)
	);
	lib.GetSkillRequirements.SetWrapper(
		std::bind(&Hooks::OnGetSkillRequirements, this, _1, _2, _3, _4, _5, _6)
	);
	lib.esv__SkillManager__CanMemorize.SetWrapper(
		std::bind(&Hooks::OnSkillManagerCanMemorize, this, _1, _2, _3, _4)
	);
	lib.ecl__SkillManager__CheckSkillRequirements.SetWrapper(
		std::bind(&Hooks::OnSkillManagerCheckSkillRequirements, this, _1, _2, _3)
	);

	auto ccr = &gExtender->GetCustomConditionRegistry();
	lib.esv__ServerConditionCheck__ProcessCondition.SetWrapper(
		std::bind(&CustomConditionRegistry::ServerConditionCheckProcess, ccr, _1, _2, _3, _4)
	);
	lib.ecl__ClientConditionCheck__ProcessCondition.SetWrapper(
		std::bind(&CustomConditionRegistry::ClientConditionCheckProcess, ccr, _1, _2, _3, _4)
	);

	auto cdt = &gExtender->GetCustomDamageTypes();
	lib.eoc__GetDamageType.SetWrapper(
		std::bind(&CustomDamageTypeHelpers::GetDamageType, cdt, _1, _2)
	);
	lib.eoc__GetDamageTypeString.SetWrapper(
		std::bind(&CustomDamageTypeHelpers::GetDamageTypeString, cdt, _1, _2)
	);
	lib.eoc__DamageTypeToTranslateString.SetWrapper(
		std::bind(&CustomDamageTypeHelpers::DamageTypeToTranslateString, cdt, _1, _2, _3)
	);
	lib.eoc__DamageDescriptionToTranslateString.SetWrapper(
		std::bind(&CustomDamageTypeHelpers::DamageDescriptionToTranslateString, cdt, _1, _2, _3)
	);
	lib.eoc__DamageTypeToTranslateStringExtended.SetWrapper(
		std::bind(&CustomDamageTypeHelpers::DamageTypeToTranslateStringExtended, cdt, _1, _2, _3)
	);
	lib.fmt__GetColorCodeDmg.SetWrapper(
		std::bind(&CustomDamageTypeHelpers::GetColorCodeDmg, cdt, _1, _2)
	);
	lib.fmt__ColorCodeAndTypeDmg.SetWrapper(
		std::bind(&CustomDamageTypeHelpers::GetColorCodeAndTypeDmg, cdt, _1, _2, _3, _4, _5)
	);
#if defined(OSI_EOCAPP)
	lib.CDivinityStats_Character__ComputeScaledDamage.SetWrapper(
		std::bind(&CustomDamageTypeHelpers::ComputeScaledDamage, cdt, _1, _2, _3, _4, _5)
	);
	lib.CDivinityStats_Character__GetResistance.SetWrapper(
		std::bind(&CustomDamageTypeHelpers::GetResistance, cdt, _1, _2, _3, _4)
	);
	lib.CDivinityStats_Character__GetDamageBoostByType.SetWrapper(
		std::bind(&CustomDamageTypeHelpers::GetDamageBoostByType, cdt, _1, _2, _3)
	);
#else
	lib.CDivinityStats_Character__ComputeScaledDamage.SetWrapper(
		std::bind(&CustomDamageTypeHelpers::ComputeScaledDamage, cdt, _1, _2, _3, _4, _5, _6)
	);
	lib.CDivinityStats_Character__GetResistance.SetWrapper(
		std::bind(&CustomDamageTypeHelpers::GetResistance, cdt, _1, _2, _3, _4, _5)
	);
	lib.CDivinityStats_Character__GetDamageBoostByType.SetWrapper(
		std::bind(&CustomDamageTypeHelpers::GetDamageBoostByType, cdt, _1, _2, _3, _4)
	);
#endif

	lib.CDivinityStats_Item__ComputeDamage.SetWrapper(
		std::bind(&CustomDamageTypeHelpers::ComputeItemDamage, cdt, _1, _2, _3, _4)
	);

	loaded_ = true;
}

int32_t Hooks::OnGetHitChance(stats::CDivinityStats_Character__GetHitChance * wrappedGetHitChance,
	stats::Character * attacker, stats::Character * target)
{
	LuaVirtualPin lua(gExtender->GetCurrentExtensionState());
	if (lua) {
		auto hitChance = lua->GetHitChance(attacker, target);
		if (hitChance) {
			return *hitChance;
		}
	}

	return wrappedGetHitChance(attacker, target);
}

bool Hooks::OnOsiShowNotification(osi::OsirisCallHandlerProc* wrappedHandler, void* proc, osi::ExecutionContext* ctx)
{
	auto ch = esv::GetEntityWorld()->GetComponent<esv::Character>(ctx->Params[0].Str, false);
	if (ch == nullptr || !ch->PartyHandle) {
		return false;
	}

	return wrappedHandler(proc, ctx);
}

int32_t Hooks::OnGetAttackAPCost(stats::Character::GetAttackAPCostProc * wrapped,
	stats::Character* self)
{
	LuaVirtualPin lua(gExtender->GetCurrentExtensionState());
	if (lua) {
		auto ap = lua->GetAttackAPCost(self);
		if (ap) {
			return *ap;
		}
	}

	return wrapped(self);
}

void Hooks::OnBeforeActionMachineSetState(ActionMachineBase<esv::ActionState>* self, uint64_t actionLayer, esv::ActionState* actionState, int* somePtr, bool force, bool setLayer)
{
	if (actionLayer >= 3 || self->IsEntering[actionLayer]) return;

	auto curState = self->Layers[actionLayer].State;
	if (curState == nullptr || curState == actionState) return;

	if (!curState->CanExit() && !force) return;

	auto character = esv::GetEntityWorld()->GetComponent<esv::Character>(self->CharacterHandle);
	if (character == nullptr) {
		OsiErrorS("ActionMachine has no character?");
		return;
	}

	auto type = curState->GetType();
	auto typeName = curState->GetTypeName();
	// Avoid event spam from idle/anim states
	if (type == ActionStateType::Idle
		|| type == ActionStateType::Animation
		|| typeName == nullptr) {
		return;
	}

	auto eventArgs = OsiArgumentDesc::Create(OsiArgumentValue{ ValueType::GuidString, *character->GetGuid() });
	eventArgs->Add(OsiArgumentValue{ ValueType::String, typeName });
	gExtender->GetServer().Osiris().GetCustomFunctionInjector().ThrowEvent(esv::ActionStateExitHandle, eventArgs);
	delete eventArgs;
}

void Hooks::OnActionMachineResetState(ActionMachineBase<esv::ActionState>* self, bool force)
{
	for (auto layer = 0; layer < 3; layer++) {
		if (self->IsEntering[layer]) continue;

		auto curState = self->Layers[layer].State;
		if (curState == nullptr || (!curState->CanExit() && !force)) continue;

		auto character = esv::GetEntityWorld()->GetComponent<esv::Character>(self->CharacterHandle);
		if (character == nullptr) {
			OsiErrorS("ActionMachine has no character?");
			return;
		}

		auto type = curState->GetType();
		auto typeName = curState->GetTypeName();
		// Avoid event spam from idle/anim states
		if (type == ActionStateType::Idle
			|| type == ActionStateType::Animation
			|| typeName == nullptr) {
			return;
		}

		auto eventArgs = OsiArgumentDesc::Create(OsiArgumentValue{ ValueType::GuidString, *character->GetGuid() });
		eventArgs->Add(OsiArgumentValue{ ValueType::String, typeName });
		gExtender->GetServer().Osiris().GetCustomFunctionInjector().ThrowEvent(esv::ActionStateExitHandle, eventArgs);
		delete eventArgs;
	}
}

void Hooks::OnParseStructureFolder(stats::RPGStats* self, ObjectSet<STDString>* paths)
{
	gExtender->GetCustomDamageTypes().Clear();

	LuaVirtualPin lua(gExtender->GetCurrentExtensionState());
	if (lua) {
		lua->OnStatsStructureLoaded();
	}
}

esv::Projectile* Hooks::OnShootProjectile(esv::ProjectileHelpers__ShootProjectile* next, esv::ShootProjectileHelper* helper)
{
	// Work around crash where the caster handle is no longer valid.
	// The projectile logic will try to fetch the stats of the caster without checking if the
	// caster object is NULL, causing a crash.
	if (helper->Caster) {
		if (helper->Caster.GetType() == (uint32_t)esv::Character::ObjectTypeIndex
			&& esv::GetEntityWorld()->GetComponent<esv::Character>(helper->Caster, false) == nullptr) {
			ERR("Attempted to shoot projectile with nonexistent caster character handle!");
			helper->Caster = ComponentHandle{};
		}

		if (helper->Caster.GetType() == (uint32_t)esv::Item::ObjectTypeIndex
			&& esv::GetEntityWorld()->GetComponent<esv::Item>(helper->Caster, false) == nullptr) {
			ERR("Attempted to shoot projectile with nonexistent caster item handle!");
			helper->Caster = ComponentHandle{};
		}
	}

	{
		esv::LuaServerPin lua(esv::ExtensionState::Get());
		if (lua) {
			lua->OnBeforeShootProjectile(helper);
		}
	}

	auto projectile = next(helper);

	if (projectile) {
		esv::LuaServerPin lua(esv::ExtensionState::Get());
		if (lua) {
			lua->OnShootProjectile(projectile);
		}
	}

	return projectile;
}

void Hooks::OnProjectileExplode(esv::Projectile* projectile)
{
	// We're not sure yet whether the projectile exploded or was deflected, 
	// so install an OnHit hook to detect the actual hit event
	auto hitProxy = GameAlloc<esv::ProxyProjectileHit>();

	// If the hit action has a ProxyProjectileHit VMT, don't hook the hit again.
	if (projectile->OnHitAction != nullptr &&
		*(void**)projectile->OnHitAction == *(void**)hitProxy) {
		GameDelete(hitProxy);
		return;
	}

	hitProxy->WrappedHit = projectile->OnHitAction;
	projectile->OnHitAction = hitProxy;
}

void Hooks::OnActionMachineSetState(ActionMachineBase<esv::ActionState>* self, uint64_t actionLayer, esv::ActionState * actionState, int * somePtr, bool force, bool setLayer, bool succeeded)
{
	if (!succeeded || actionState == nullptr || !setLayer) return;

	auto character = esv::GetEntityWorld()->GetComponent<esv::Character>(self->CharacterHandle);
	if (character == nullptr) {
		OsiErrorS("ActionMachine has no character?");
		return;
	}

	auto type = actionState->GetType();
	auto typeName = actionState->GetTypeName();
	// Avoid event spam from idle/anim states
	if (type == ActionStateType::Idle
		|| type == ActionStateType::Animation
		|| typeName == nullptr) {
		return;
	}

	auto eventArgs = OsiArgumentDesc::Create(OsiArgumentValue{ ValueType::GuidString, *character->GetGuid() });
	eventArgs->Add(OsiArgumentValue{ ValueType::String, typeName });

	gExtender->GetServer().Osiris().GetCustomFunctionInjector().ThrowEvent(esv::ActionStateEnterHandle, eventArgs);

	delete eventArgs;
}

stats::PropertyData* Hooks::OnParseSkillProperties(stats::RPGStats::ParsePropertiesProc* next, stats::RPGStats* self, STDString* str)
{
	auto parsed = next(self, str);
	if (parsed != nullptr && parsed->TypeId == stats::PropertyType::Status) {
		auto status = static_cast<stats::PropertyStatus*>(parsed);
		if (strncmp(status->Status.GetString(), "EXT:", 4) == 0) {
			auto extProp = GameAlloc<stats::PropertyExtender>();
			extProp->Context = status->Context;
			extProp->TypeId = stats::PropertyType::Extender;
			extProp->Conditions = status->Conditions;
			extProp->PropertyName = FixedString(status->Status.GetString() + 4);
			extProp->Arg1 = status->StatusChance;
			extProp->Arg2 = status->Duration;
			extProp->Arg3 = status->StatsId;
			extProp->Arg4 = status->Arg4;
			extProp->Arg5 = status->Arg5;
			return extProp;
		}
	}

	return parsed;
}

void Hooks::OnSkillFormatDescriptionParam(stats::SkillPrototype::FormatDescriptionParamProc* next, stats::SkillPrototype *skillPrototype,
	stats::Character *tgtCharStats, eoc::Text *eocText, int paramIndex, bool isFromItem,
	float xmm9_4_0, FixedString * paramText, ObjectSet<STDString> * paramTexts)
{
	// When fetching subproperties (recursively), paramTexts will be null.
	// We won't post these to Lua since the Lua scripts already processed the original (unwrapped) query
	if (paramTexts != nullptr) {
		ecl::LuaClientPin lua(ecl::ExtensionState::Get());
		if (lua) {
			auto replacement = lua->SkillGetDescriptionParam(skillPrototype, tgtCharStats, *paramTexts, isFromItem);
			if (replacement) {
				eocText->ReplaceParam(paramIndex, *replacement);
				return;
			}
		}
	}

	next(skillPrototype, tgtCharStats, eocText, paramIndex, isFromItem, xmm9_4_0, paramText, paramTexts);
}

#if defined(OSI_EOCAPP)
int Hooks::OnGetSkillAPCost(stats::SkillPrototype::GetAttackAPCostProc* next, stats::SkillPrototype* self, stats::Character* character, eoc::AiGrid* aiGrid,
	glm::vec3* position, float* radius, int* pElementalAffinity)
#else
int Hooks::OnGetSkillAPCost(stats::SkillPrototype::GetAttackAPCostProc* next, stats::SkillPrototype* self, stats::Character* character, eoc::AiGrid* aiGrid,
	glm::vec3* position, float* radius, bool unused, int* pElementalAffinity)
#endif
{
	LuaVirtualPin lua(gExtender->GetCurrentExtensionState());
	if (lua) {
		auto ap = lua->GetSkillAPCost(self, character, aiGrid, position, radius);
		if (ap) {
			if (pElementalAffinity != nullptr) {
				*pElementalAffinity = ap->second ? 1 : 0;
			}

			return ap->first;
		}
	}

#if defined(OSI_EOCAPP)
	return next(self, character, aiGrid, position, radius, pElementalAffinity);
#else
	return next(self, character, aiGrid, position, radius, unused, pElementalAffinity);
#endif
}

void Hooks::OnStatusFormatDescriptionParam(stats::StatusPrototype::FormatDescriptionParamProc* next, stats::StatusPrototype *prototype,
	stats::ObjectInstance* owner, stats::ObjectInstance* statusSource, float multiplier,
	eoc::Text * text, int paramIndex, FixedString * param, ObjectSet<STDString> * paramSet)
{
	ecl::LuaClientPin lua(ecl::ExtensionState::Get());
	if (lua) {
		auto replacement = lua->StatusGetDescriptionParam(prototype, owner, statusSource, *paramSet);
		if (replacement) {
			text->ReplaceParam(paramIndex, *replacement);
			return;
		}
	}

	next(prototype, owner, statusSource, multiplier, text, paramIndex, param, paramSet);
}

void Hooks::OnUpdateTurnOrder(esv::TurnManager * self, uint8_t combatId)
{
	esv::LuaServerPin lua(esv::ExtensionState::Get());
	if (lua) {
		lua->OnUpdateTurnOrder(self, combatId);
	}
}


void Hooks::OnCreateUIObject(UIObjectManager* self, ComponentHandle* handle, unsigned int layerIndex,
	unsigned int creatorId, UIObjectFlags flags, uint64_t resourceFlags, PlayerId playerId, ComponentHandle* result)
{
	if (gExtender->GetClient().HasExtensionState()) {
		ecl::LuaClientPin lua(ecl::ExtensionState::Get());
		if (lua) {
			lua->OnCreateUIObject(*result);
		}
	}
}



void Hooks::OnExecutePropertyDataOnGroundHit(glm::vec3* position, uint64_t casterHandle,
	stats::DamagePairList* damageList, stats::PropertyList* propertyList, stats::DamageType damageType)
{
	esv::LuaServerPin lua(esv::ExtensionState::Get());
	if (position && lua) {
		lua->OnExecutePropertyDataOnGroundHit(*position, ComponentHandle(casterHandle), damageList);
	}
}


void Hooks::OnExecuteCharacterSetExtraProperties(stats::PropertyList* properties, uint64_t attackerHandle,
	ObjectSet<esv::Character*> const& targets, glm::vec3 const& impactOrigin, stats::PropertyContext propertyContext,
	bool isFromItem, stats::SkillPrototype* skillPrototype, stats::HitDamageInfo* damageInfo, float statusStartTimer, esv::Character* refTarget,
	bool statusFlag0x40, float a12)
{
	if (!properties || (unsigned)properties->AllPropertyContexts == 0
		|| properties->Properties.Elements.size() == 0) {
		return;
	}

	esv::LuaServerPin lua(esv::ExtensionState::Get());
	if (lua) {
		for (auto prop : properties->Properties.Elements) {
			if (prop->TypeId == stats::PropertyType::Extender) {
				auto extProp = static_cast<stats::PropertyExtender*>(prop);
				for (auto const& target : targets) {
					ComponentHandle targetHandle;
					target->GetObjectHandle(targetHandle);
					lua->ExecutePropertyDataOnTarget(extProp, ComponentHandle{ attackerHandle }, targetHandle, 
						impactOrigin, isFromItem, skillPrototype, damageInfo);
				}
			}
		}
	}
}


void Hooks::OnExecutePropertyDataOnPositionOnly(stats::PropertyList* properties, uint64_t attackerHandle,
	glm::vec3 const* position, float areaRadius, stats::PropertyContext propertyContext, bool isFromItem,
	stats::SkillPrototype* skillPrototype, stats::HitDamageInfo* damageInfo, float unkn)
{
	if (!properties || !position || (unsigned)properties->AllPropertyContexts == 0 
		|| properties->Properties.Elements.size() == 0) {
		return;
	}

	esv::LuaServerPin lua(esv::ExtensionState::Get());
	if (lua) {
		for (auto prop : properties->Properties.Elements) {
			if (prop->TypeId == stats::PropertyType::Extender) {
				auto extProp = static_cast<stats::PropertyExtender*>(prop);
				lua->ExecutePropertyDataOnPosition(extProp, ComponentHandle{ attackerHandle }, *position, areaRadius, isFromItem,
					skillPrototype, damageInfo);
			}
		}
	}
}


void Hooks::OnSortAiActions(esv::AiHelpers::SortActionsProc* next, Set<esv::AiAction>* actions)
{
	esv::AiRequest* currentRequest{ nullptr };
	ComponentHandle characterHandle;
	for (auto const& request : (*GetStaticSymbols().esv__gAiHelpers)->CharacterAiRequests) {
		if (!request.Value->AiActions.empty()
			&& request.Value->AiActions.begin().get() == actions->begin().get()) {
			characterHandle = request.Key;
			currentRequest = request.Value;
			break;
		}
	}

	if (currentRequest) {
		esv::LuaServerPin lua(esv::ExtensionState::Get());
		if (lua) {
			lua->OnBeforeSortAiActions(characterHandle, currentRequest);
		}
	}

	next(actions);

	if (currentRequest) {
		esv::LuaServerPin lua(esv::ExtensionState::Get());
		if (lua) {
			lua->OnAfterSortAiActions(characterHandle, currentRequest);
		}
	}
}


void Hooks::OnPeekAiAction(esv::AiHelpers* self, esv::Character* character, AiActionType actionType, bool isFinished)
{
	auto characterHandle = character->Base.Component.Handle;
	auto request = (*GetStaticSymbols().esv__gAiHelpers)->CharacterAiRequests.try_get(characterHandle);
	if (request == nullptr || request->IsCalculating) return;

	esv::LuaServerPin lua(esv::ExtensionState::Get());
	if (lua) {
		lua->OnPeekAiAction(characterHandle, request, actionType, isFinished);
	}
}


esv::Item* Hooks::OnGenerateTreasureItem(esv::ItemHelpers__GenerateTreasureItem* next,
	stats::TreasureObjectInfo* treasureInfo, int level)
{
	auto item = next(treasureInfo, level);

	if (item) {
		esv::LuaServerPin lua(esv::ExtensionState::Get());
		if (lua) {
			item = lua->OnGenerateTreasureItem(item);
		}
	}

	return item;
}


bool Hooks::OnCraftingExecuteCombination(esv::CombineManager::ExecuteCombinationProc* next,
	esv::CombineManager* self, CraftingStationType craftingStation, ObjectSet<ComponentHandle>* ingredientHandles,
	esv::Character* character, uint8_t quantity, char openUI, FixedString* combinationId)
{
	esv::LuaServerPin lua(esv::ExtensionState::Get());
	if (lua) {
		if (lua->OnBeforeCraftingExecuteCombination(craftingStation, *ingredientHandles, character, quantity, *combinationId)) {
			return true;
		}
	}

	bool ok = next(self, craftingStation, ingredientHandles, character, quantity, openUI, combinationId);

	if (lua) {
		lua->OnAfterCraftingExecuteCombination(craftingStation, *ingredientHandles, character, quantity, *combinationId, ok);
	}

	return ok;
}

void Hooks::OnAppInputEvent(IApp* self, uint16_t& retval, InputEvent const& inputEvent)
{
	if (gExtender->GetClient().HasExtensionState()) {
		ecl::LuaClientPin lua(ecl::ExtensionState::Get());
		if (lua) {
			lua->OnAppInputEvent(inputEvent);
		}
	}
}

void Hooks::OnInjectInput(InputManager* self, InjectInputData const& input)
{
	if (gExtender->GetClient().HasExtensionState()) {
		ecl::LuaClientPin lua(ecl::ExtensionState::Get());
		if (lua) {
			lua->OnRawInputEvent(input);
		}
	}
}

int Hooks::OnInventoryProtocolPostUpdate(ecl::InventoryProtocol::PostUpdateProc* next, ecl::InventoryProtocol* self)
{
	if (self->ItemUpdates.size() > 0x200) {
		auto lockStateSyncMsg = self->LockStateSyncMsg;
		self->LockStateSyncMsg = nullptr;

		ObjectSet<ecl::InventoryProtocol::InventoryViewItemUpdate> deferredUpdates;
		auto updates = self->ItemUpdates;
		uint32_t chunkOffset = 0;
		while (chunkOffset < updates.size()) {
			self->ItemUpdates.clear();
			auto chunkSize = std::min(updates.size() - chunkOffset, (uint32_t)0x100);
			while (chunkSize--) {
				self->ItemUpdates.push_back(updates[chunkOffset++]);
			}

			next(self);

			for (auto const& update : self->ItemUpdates) {
				deferredUpdates.push_back(update);
			}
		}

		self->ItemUpdates = deferredUpdates;
		self->LockStateSyncMsg = lockStateSyncMsg;
	}

	next(self);
	return 0;
}

Visual* Hooks::OnCreateEquipmentVisuals(ecl::EquipmentVisualsSystem::CreateVisualsProc* wrapped,
	ecl::EquipmentVisualsSystem* self, EntityHandle entityHandle, ecl::EquipmentVisualSystemSetParam& params)
{
#if defined(OSI_EOCAPP)
	// Fix game crash when calling CreateEquipmentVisuals) with an invalid resource UUID
	if (params.VisualResourceID && params.VisualResourceID != GFS.strEmpty) {
		auto bank = GetStaticSymbols().GetResourceBank();
		if (bank->Container.GetResource(ResourceType::Visual, params.VisualResourceID) == nullptr
			&& bank->Container.GetResource(ResourceType::Effect, params.VisualResourceID) == nullptr) {
			ERR("Tried to instantiate nonexistent visual '%s'!", params.VisualResourceID.GetStringOrDefault());
			return nullptr;
		}
	}

	ecl::LuaClientPin lua(ecl::ExtensionState::Get());
	if (lua) {
		lua->OnCreateEquipmentVisuals(entityHandle, params);
	}
#endif

	return wrapped(self, entityHandle, params);
}


bool Hooks::OnCheckRequirement(stats::CheckRequirementProc* wrapped, stats::Character* self, bool isInCombat, bool isImmobile, bool hasCharges,
	ObjectSet<FixedString> const* tags, stats::Requirement const& requirement, bool excludeBoosts)
{
	auto& ctx = gExtender->GetCurrentExtensionState()->GetCustomRequirementContext();
	ctx.CharacterStats = self;
	ctx.IsInCombat = isInCombat;
	ctx.IsImmobile = isImmobile;
	ctx.HasCharges = hasCharges;
	ctx.Tags = const_cast<ObjectSet<FixedString>*>(tags);
	ctx.Requirement = const_cast<stats::Requirement*>(&requirement);
	ctx.ExcludeBoosts = excludeBoosts;
	
	std::optional<bool> result;

	LuaVirtualPin lua(gExtender->GetCurrentExtensionState());
	if (lua) {
		result = lua->GetCustomRequirementCallbacks().Evaluate(self, requirement);
	}

	if (!result) {
		result = wrapped(self, isInCombat, isImmobile, hasCharges, tags, requirement, excludeBoosts);
	}

	ctx.CharacterStats = nullptr;
	ctx.Tags = nullptr;
	ctx.Requirement = nullptr;

	return *result;
}

TranslatedString* Hooks::OnRequirementToTranslatedString(stats::RequirementToTranslatedStringProc* wrapped, TranslatedString* text, 
	stats::RequirementType requirementId, bool negate)
{
	auto desc = gExtender->GetCustomRequirementRegistry().Get((uint32_t)requirementId);
	if (desc && (!desc->Description.empty() || desc->DescriptionHandle)) {
		new (text) TranslatedString();
		script::GetTranslatedString(desc->DescriptionHandle.GetStringOrDefault(), desc->Description.c_str(), *text);
		return text;
	}

	return wrapped(text, requirementId, negate);
}

stats::RequirementType Hooks::OnStringToRequirement(stats::StringToRequirementProc* wrapped, char const* requirement)
{
	auto desc = gExtender->GetCustomRequirementRegistry().Get(FixedString(requirement));
	if (desc) {
		return (stats::RequirementType)desc->RequirementId;
	}

	return wrapped(requirement);
}

#if defined(OSI_EOCAPP)
bool Hooks::OnReevaluateRequirements(stats::Character::ReevaluateRequirementsProc* wrapped, stats::Character* self, stats::ItemSlot32 slot,
	stats::Requirement* pRequirement)
#else
bool Hooks::OnReevaluateRequirements(stats::Character::ReevaluateRequirementsProc* wrapped, stats::Character* self, stats::ItemSlot32 slot,
	bool checkRequirements, stats::Requirement* pRequirement)
#endif
{
	stats::Item* item{ nullptr };
	for (auto itemRef : self->EquippedItems) {
		if ((stats::ItemSlot32)itemRef->ItemSlot == slot) {
			item = self->DivStats->ItemList->Get(itemRef->ItemStatsHandle);
			break;
		}
	}

	auto& ctx = gExtender->GetCurrentExtensionState()->GetCustomRequirementContext();
	ctx.ItemStats = item;

#if defined(OSI_EOCAPP)
	auto result = wrapped(self, slot, pRequirement);
#else
	auto result = wrapped(self, slot, checkRequirements, pRequirement);
#endif

	ctx.ItemStats = nullptr;

	return result;
}

void Hooks::OnItemSetupDescriptionToFlash(ecl::Item::SetupDescriptionToFlashProc* wrapped, ecl::Item* self, ig::FlashObject* flash, 
	char const* path, uint32_t displayContext, ecl::Character* owner, void* iconDrawStruct, int amount)
{
	auto& ctx = gExtender->GetCurrentExtensionState()->GetCustomRequirementContext();
	ctx.ClientItem = self;

	wrapped(self, flash, path, displayContext, owner, iconDrawStruct, amount);

	ctx.ClientItem = nullptr;
}

uint64_t Hooks::OnServerCharacterCheckSkillRequirements(esv::Character::CheckSkillRequirementsProc* wrapped, esv::Character* self, 
	FixedString const& skillId, esv::Item* item, bool checkAP, bool mustHaveSkill)
{
	auto& ctx = gExtender->GetCurrentExtensionState()->GetCustomRequirementContext();
	ctx.ServerCharacter = self;
	ctx.ServerItem = item;
	ctx.SkillId = skillId;

	auto result = wrapped(self, skillId, item, checkAP, mustHaveSkill);

	ctx.ServerCharacter = nullptr;
	ctx.ServerItem = nullptr;
	ctx.SkillId = FixedString{};

	return result;
}

uint64_t Hooks::OnClientCharacterCheckSkillRequirements(ecl::Character::CheckSkillRequirementsProc* wrapped, ecl::Character* self, 
	FixedString const& skillId, ecl::Item* item)
{
	auto& ctx = gExtender->GetCurrentExtensionState()->GetCustomRequirementContext();
	ctx.ClientCharacter = self;
	ctx.ClientItem = item;
	ctx.SkillId = skillId;

	auto result = wrapped(self, skillId, item);

	ctx.ClientCharacter = nullptr;
	ctx.ClientItem = nullptr;
	ctx.SkillId = FixedString{};

	return result;
}

void Hooks::OnGetSkillRequirements(ecl::Character::GetSkillRequirementsProc* wrapped, ecl::Character* character, 
	FixedString const& skillId, ecl::Item* item, uint32_t checkRequirementFlags, eoc::Text& requirementsText)
{
	auto& ctx = gExtender->GetCurrentExtensionState()->GetCustomRequirementContext();
	ctx.ClientCharacter = character;
	ctx.ClientItem = item;
	ctx.SkillId = skillId;

	wrapped(character, skillId, item, checkRequirementFlags, requirementsText);

	ctx.ClientCharacter = nullptr;
	ctx.ClientItem = nullptr;
	ctx.SkillId = FixedString{};
}

bool Hooks::OnSkillManagerCanMemorize(esv::SkillManager::CanMemorizeProc* wrapped, esv::SkillManager* self, stats::SkillPrototype* skill, 
	bool checkMemoryRequirement)
{
	auto& ctx = gExtender->GetCurrentExtensionState()->GetCustomRequirementContext();
	ctx.ServerCharacter = esv::GetEntityWorld()->GetComponent<esv::Character>(self->OwnerHandle, false);
	ctx.SkillId = skill->SkillId;

	auto result = wrapped(self, skill, checkMemoryRequirement);

	ctx.ServerCharacter = nullptr;
	ctx.SkillId = FixedString{};

	return result;
}

bool Hooks::OnSkillManagerCheckSkillRequirements(ecl::SkillManager::CheckSkillRequirementsProc* wrapped, ecl::SkillManager* self, 
	stats::SkillPrototype* proto)
{
	auto& ctx = gExtender->GetCurrentExtensionState()->GetCustomRequirementContext();
	ctx.ClientCharacter = ecl::GetEntityWorld()->GetComponent<ecl::Character>(self->OwnerHandle, false);
	ctx.SkillId = proto->SkillId;

	auto result = wrapped(self, proto);

	ctx.ClientCharacter = nullptr;
	ctx.SkillId = FixedString{};

	return result;
}


bool Hooks::OnPeerModuleLoaded(esv::LoadProtocol::HandleModuleLoadedProc* next, esv::LoadProtocol* self,
	esv::LoadProtocol::PeerModSettings& peerModSettings, ModuleSettings& hostModSettings)
{
	auto validate = GetStaticSymbols().ModuleSettings__Validate;
	if (validate != nullptr
		// Don't validate local peer settings, as they can be wrong because of a bug with ArmorSets
		&& peerModSettings.peerId != 1) {
		ObjectSet<ModuleShortDesc> mismatches;

		bool hasEmptyHashes = false;
		for (auto const& mod : hostModSettings.Mods) {
			if (mod.MD5.empty()) {
				hasEmptyHashes = true;
			}
		}

		for (auto const& mod : peerModSettings.modSettings.Mods) {
			if (mod.MD5.empty()) {
				hasEmptyHashes = true;
			}
		}

		// Hashes work differently in vanilla / old extender, so ignore them if the peer has an old version
		auto peerVersion = gExtender->GetServer().GetNetworkManager().GetPeerVersion(peerModSettings.peerId);
		// TEMP WORKAROUND - disabled md5 checks until we can figure out why it fails randomly
		if (!peerVersion || *peerVersion < ScriptExtenderMessage::VerCorrectedHashes || hasEmptyHashes || true) {
			for (auto& mod : peerModSettings.modSettings.Mods) {
				mod.MD5 = "";
			}

			for (auto& mod : hostModSettings.Mods) {
				mod.MD5 = "";
			}
		}

		auto result = validate(hostModSettings, peerModSettings.modSettings, mismatches);

		if (result != 0) {
			STDWString userName = L"(Unknown)";
			auto gameServer = (*GetStaticSymbols().esv__EoCServer)->GameServer;
			auto peer = gameServer->Peers.try_get_ptr(peerModSettings.peerId);
			if (peer != nullptr) {
				for (auto const& user : peer->Users) {
					userName = user.Value.UserName;
				}
			}

			ERR("Validation rc = %d", result);

			INFO("Server modlist:");
			for (auto const& mod : hostModSettings.Mods) {
				INFO("%s - %s - %s - %d - %s", mod.ModuleUUID.GetString(), ToUTF8(mod.Folder).c_str(), ToUTF8(mod.Name).c_str(), mod.Version.Ver, mod.MD5.c_str());
			}

			INFO("Client modlist:");
			for (auto const& mod : peerModSettings.modSettings.Mods) {
				INFO("%s - %s - %s - %d - %s", mod.ModuleUUID.GetString(), ToUTF8(mod.Folder).c_str(), ToUTF8(mod.Name).c_str(), mod.Version.Ver, mod.MD5.c_str());
			}

			std::stringstream ss;
			switch (result) {
			case 2:
			{
				auto v0 = mismatches[0].Version;
				auto v1 = mismatches[1].Version;
				ss << "Client (" << ToUTF8(userName) << ") mod version of " << ToUTF8(mismatches[0].Name) << " (v"
					<< v1.Major() << "." << v1.Minor() << "." << v1.Revision() << "." << v1.Build()
					<< ") doesn't match servers (v" 
					<< v0.Major() << "." << v0.Minor() << "." << v0.Revision() << "." << v0.Build()
					<< ")";
				break;
			}

			case 3:
				ss << "Client (" << ToUTF8(userName) << ") mod hash of " << ToUTF8(mismatches[0].Name) << " (" << mismatches[1].MD5
					<< ") doesn't match servers (" << mismatches[0].MD5 << ")";
				break;

			case 1:
			default:
				ss << "Client (" << ToUTF8(userName) << ") mod settings doesn't match servers";
				break;
			}

			auto server = GetStaticSymbols().esv__EoCServer;
			if (server != nullptr) {
				int32_t userId = peerModSettings.peerId << 16;
				// We use the vanilla HostRefuse message for non-extender clients or old extender
				// versions that don't support the kick message
				if (peerVersion && *peerVersion >= ScriptExtenderMessage::VerAddedKickMessage) {
					auto msg = gExtender->GetServer().GetNetworkManager().GetFreeMessage(UserId(userId));
					if (msg != nullptr) {
						auto postMsg = msg->GetMessage().mutable_s2c_kick();
						postMsg->set_message(ss.str().c_str());
						gExtender->GetServer().GetNetworkManager().Send(msg, UserId(userId));
					} else {
						OsiErrorS("Could not get free message!");
					}
				} else {
					auto host = (*server)->GameServer;
					auto msg = host->GetFreeMessage<net::HostRefuseMessage>();
					msg->ReasonCode = 4;
					host->VMT->SendToPeer(host, &userId, msg);
				}
			}

			gExtender->GetLibraryManager().ShowStartupError(FromUTF8(ss.str().c_str()), false);
		}
	}

	// Sync all dynamic stats to new peer as we didn't do so during savegame loading
	GetStaticSymbols().GetStats()->BroadcastSyncAll();

	return next(self, peerModSettings, hostModSettings);
}

END_SE()
