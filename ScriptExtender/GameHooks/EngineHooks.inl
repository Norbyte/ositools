HOOK_DEFN(esv__Status__GetEnterChanceHook, esv__Status__GetEnterChance, esv::Status::GetEnterChanceProc)
HOOK_DEFN(esv__StatusHit__Enter, esv__StatusHit__Enter, esv::Status::EnterProc)
HOOK_DEFN(esv__StatusHit__Setup, esv__StatusHit__Setup, esv::StatusHit::SetupProc)
HOOK_DEFN(esv__StatusHeal__Enter, esv__StatusHeal__Enter, esv::Status::EnterProc)

HOOK_DEFN(esv__StatusConsume__Enter, esv__StatusConsume__Enter, esv::Status::EnterProc)
HOOK_DEFN(esv__StatusConsume__Exit, esv__StatusConsume__Exit, esv::Status::ExitProc)
HOOK_DEFN(osi__ShowNotification, osi__ShowNotification, osi::OsirisCallHandlerProc)

HOOK_DEFN(esv__StatusMachine__ApplyStatus, esv__StatusMachine__ApplyStatus, esv::StatusMachine::ApplyStatusProc)
HOOK_DEFN(esv__StatusMachine__EnterStatus, esv__StatusMachine__EnterStatus, esv::StatusMachine::EnterStatusProc)
HOOK_DEFN(esv__StatusMachine__Update, esv__StatusMachine__Update, esv::StatusMachine::UpdateProc)
HOOK_DEFN(esv__StatusMachine__DeleteStatusByHandle, esv__StatusMachine__DeleteStatusByHandle, esv::StatusMachine::DeleteStatusByHandleProc)
HOOK_DEFN(esv__StatusMachine__ExitStatus, esv__StatusMachine__ExitStatus, esv::StatusMachine::ExitStatusProc)

HOOK_DEFN(ecl__StatusMachine__ExitStatus, ecl__StatusMachine__ExitStatus, ecl::StatusMachine::ExitStatusProc)

HOOK_DEFN(esv__ProjectileHelpers__ShootProjectile, esv__ProjectileHelpers__ShootProjectile, esv::ProjectileHelpers__ShootProjectile)
HOOK_DEFN(esv__Projectile__Explode, esv__Projectile__Explode, esv::Projectile::ExplodeProc)
HOOK_DEFN(esv__ActionMachine__SetState, esv__ActionMachine__SetState, esv::ActionMachine::SetStateProc)
HOOK_DEFN(esv__ActionMachine__ResetState, esv__ActionMachine__ResetState, esv::ActionMachine::ResetStateProc)

HOOK_DEFN(RPGStats__Load, RPGStats__Load, stats::RPGStats::LoadProc)
HOOK_DEFN(RPGStats__ParseStructureFolder, RPGStats__ParseStructureFolder, stats::RPGStats::ParseStructureFolderProc)
HOOK_DEFN(RPGStats__ParseProperties, RPGStats__ParseProperties, stats::RPGStats::ParsePropertiesProc)

HOOK_DEFN(eoc__GetDamageType, eoc__GetDamageType, stats::GetDamageTypeProc)
HOOK_DEFN(eoc__GetDamageTypeString, eoc__GetDamageTypeString, stats::GetDamageTypeStringProc)
HOOK_DEFN(eoc__DamageTypeToTranslateString, eoc__DamageTypeToTranslateString, stats::DamageTypeToTranslateStringProc)
HOOK_DEFN(eoc__DamageDescriptionToTranslateString, eoc__DamageDescriptionToTranslateString, stats::DamageDescriptionToTranslateStringProc)
HOOK_DEFN(eoc__DamageTypeToTranslateStringExtended, eoc__DamageTypeToTranslateStringExtended, stats::DamageTypeToTranslateStringExtendedProc)
HOOK_DEFN(fmt__GetColorCodeDmg, fmt__GetColorCodeDmg, stats::GetColorCodeDmgProc)
HOOK_DEFN(fmt__ColorCodeAndTypeDmg, fmt__ColorCodeAndTypeDmg, stats::ColorCodeAndTypeDmgProc)

HOOK_DEFN(esv__ServerConditionCheck__ProcessCondition, esv__ServerConditionCheck__ProcessCondition, esv::ServerConditionCheck::ProcessConditionProc)
HOOK_DEFN(ecl__ClientConditionCheck__ProcessCondition, ecl__ClientConditionCheck__ProcessCondition, ecl::ClientConditionCheck::ProcessConditionProc)

HOOK_DEFN(CheckRequirement1, CheckRequirement1, stats::CheckRequirementProc)
HOOK_DEFN(CheckRequirement2, CheckRequirement2, stats::CheckRequirementProc)
HOOK_DEFN(RequirementToTranslatedString, RequirementToTranslatedString, stats::RequirementToTranslatedStringProc)
HOOK_DEFN(StringToRequirement, StringToRequirement, stats::StringToRequirementProc)

HOOK_DEFN(SkillPrototype__FormatDescriptionParam, SkillPrototype__FormatDescriptionParam, stats::SkillPrototype::FormatDescriptionParamProc)
HOOK_DEFN(SkillPrototype__GetSkillDamage, SkillPrototype__GetSkillDamage, stats::SkillPrototype::GetSkillDamageProc)
HOOK_DEFN(SkillPrototype__GetAttackAPCost, SkillPrototype__GetAttackAPCost, stats::SkillPrototype::GetAttackAPCostProc)
HOOK_DEFN(StatusPrototype__FormatDescriptionParam, StatusPrototype__FormatDescriptionParam, stats::StatusPrototype::FormatDescriptionParamProc)
HOOK_DEFN(esv__ExecutePropertyDataOnGroundHit, esv__ExecutePropertyDataOnGroundHit, esv::ExecutePropertyDataOnGroundHitProc)
HOOK_DEFN(esv__ExecutePropertyDataOnPositionOnly, esv__ExecutePropertyDataOnPositionOnly, esv::ExecutePropertyDataOnPositionOnlyProc)
HOOK_DEFN(esv__ExecuteCharacterSetExtraProperties, esv__ExecuteCharacterSetExtraProperties, esv::ExecuteCharacterSetExtraPropertiesProc)

HOOK_DEFN(esv__SkillManager__CanMemorize, esv__SkillManager__CanMemorize, esv::SkillManager::CanMemorizeProc)
HOOK_DEFN(CDivinityStats_Character_GetWeaponAnimationSetType, CDivinityStats_Character_GetWeaponAnimationSetType, stats::CDivinityStats_Character_GetWeaponAnimationSetTypeProc)
HOOK_DEFN(ecl__SkillManager__CheckSkillRequirements, ecl__SkillManager__CheckSkillRequirements, ecl::SkillManager::CheckSkillRequirementsProc)
HOOK_DEFN(ecl__Item__SetupDescriptionToFlash, ecl__Item__SetupDescriptionToFlash, ecl::Item::SetupDescriptionToFlashProc)

HOOK_DEFN(esv__AiHelpers__PeekAction, esv__AiHelpers__PeekAction, esv::AiHelpers::PeekActionProc)
HOOK_DEFN(esv__AiHelpers__SortActions, esv__AiHelpers__SortActions, esv::AiHelpers::SortActionsProc)

HOOK_DEFN(esv__TurnManager__UpdateTurnOrder, esv__TurnManager__UpdateTurnOrder, esv::TurnManager::UpdateTurnOrderProc)

HOOK_DEFN(UIObjectManager__CreateUIObject, UIObjectManager__CreateUIObject, UIObjectManager::CreateUIObjectProc)

HOOK_DEFN(esv__Character_Hit, esv__Character__Hit, esv::Character::HitProc)
HOOK_DEFN(esv__Character_ApplyDamageHook, esv__Character__ApplyDamage, esv::Character::ApplyDamageProc)
HOOK_DEFN(esv__Character__CheckSkillRequirements, esv__Character__CheckSkillRequirements, esv::Character::CheckSkillRequirementsProc)

HOOK_DEFN(ecl__Character__CheckSkillRequirements, ecl__Character__CheckSkillRequirements, ecl::Character::CheckSkillRequirementsProc)
HOOK_DEFN(GetSkillRequirements, GetSkillRequirements, ecl::Character::GetSkillRequirementsProc)

HOOK_DEFN(CDivinityStats_Character__HitInternal, CDivinityStats_Character__HitInternal, stats::Character::HitInternalProc)
HOOK_DEFN(CDivinityStats_Character__GetAttackAPCost, CDivinityStats_Character__GetAttackAPCost, stats::Character::GetAttackAPCostProc)
HOOK_DEFN(CDivinityStats_Character__GetResistance, CDivinityStats_Character__GetResistance, stats::Character::GetResistanceProc)
HOOK_DEFN(CDivinityStats_Character__GetDamageBoostByType, CDivinityStats_Character__GetDamageBoostByType, stats::Character::GetDamageBoostByTypeProc)
HOOK_DEFN(CDivinityStats_Character__ComputeScaledDamage, CDivinityStats_Character__ComputeScaledDamage, stats::Character::ComputeScaledDamageProc)
HOOK_DEFN(CDivinityStats_Item__ComputeDamage, CDivinityStats_Item__ComputeDamage, stats::Item::ComputeDamageProc)
HOOK_DEFN(CDivinityStats_Character__ReevaluateRequirements, CDivinityStats_Character__ReevaluateRequirements, stats::Character::ReevaluateRequirementsProc)

HOOK_DEFN(esv__ItemHelpers__GenerateTreasureItem, esv__ItemHelpers__GenerateTreasureItem, esv::ItemHelpers__GenerateTreasureItem)
HOOK_DEFN(esv__CombineManager__ExecuteCombination, esv__CombineManager__ExecuteCombination, esv::CombineManager::ExecuteCombinationProc)
HOOK_DEFN(esv__LoadProtocol__HandleModuleLoaded, esv__LoadProtocol__HandleModuleLoaded, esv::LoadProtocol::HandleModuleLoadedProc)
HOOK_DEFN(Module__Hash, Module__Hash, Module::HashProc)
HOOK_DEFN(App__OnInputEvent, App__OnInputEvent, App::VMT::OnInputEventProc)
HOOK_DEFN(FileReader__ctor, ls__FileReader__ctor, FileReader::CtorProc)
HOOK_DEFN(ecl__InventoryProtocol__PostUpdate, ecl__InventoryProtocol__PostUpdate, ecl::InventoryProtocol::PostUpdateProc)
HOOK_DEFN(ls__InputManager__InjectInput, ls__InputManager__InjectInput, InputManager::InjectInputProc)
HOOK_DEFN(ecl__EquipmentVisualsSystem__CreateVisuals, ecl__EquipmentVisualsSystem__CreateVisuals, ecl::EquipmentVisualsSystem::CreateVisualsProc)
