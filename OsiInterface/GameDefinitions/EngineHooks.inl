HOOK_DEFN(esv__Status__GetEnterChanceHook, esv__Status__GetEnterChance, esv::Status::GetEnterChanceProc, WrappableFunction)
HOOK_DEFN(esv__StatusHit__Enter, esv__StatusHit__Enter, esv::Status::EnterProc, PreHookableFunction)
HOOK_DEFN(esv__StatusHit__Setup, esv__StatusHit__Setup, esv::StatusHit::SetupProc, PreHookableFunction)
HOOK_DEFN(esv__StatusHeal__Enter, esv__StatusHeal__Enter, esv::Status::EnterProc, PreHookableFunction)
HOOK_DEFN(esv__StatusMachine__ApplyStatus, esv__StatusMachine__ApplyStatus, esv::StatusMachine::ApplyStatusProc, WrappableFunction)

HOOK_DEFN(esv__ActionMachine__SetState, esv__ActionMachine__SetState, esv::ActionMachine::SetStateProc, HookableFunction)
HOOK_DEFN(esv__ActionMachine__ResetState, esv__ActionMachine__ResetState, esv::ActionMachine::ResetStateProc, PreHookableFunction)

HOOK_DEFN(SkillPrototype__FormatDescriptionParam, SkillPrototype__FormatDescriptionParam, SkillPrototype::FormatDescriptionParamProc, WrappableFunction)
HOOK_DEFN(SkillPrototype__GetSkillDamage, SkillPrototype__GetSkillDamage, SkillPrototype::GetSkillDamageProc, WrappableFunction)
HOOK_DEFN(StatusPrototype__FormatDescriptionParam, StatusPrototype__FormatDescriptionParam, StatusPrototype::FormatDescriptionParamProc, WrappableFunction)


HOOK_DEFN(esv__TurnManager__UpdateTurnOrder, esv__TurnManager__UpdateTurnOrder, esv::TurnManager::UpdateTurnOrderProc, PostHookableFunction)

HOOK_DEFN(UIObjectManager__CreateUIObject, UIObjectManager__CreateUIObject, UIObjectManager::CreateUIObjectProc, PostHookableFunction)

HOOK_DEFN(esv__Character_Hit, esv__Character__Hit, esv::Character::HitProc, WrappableFunction)
HOOK_DEFN(esv__Character_ApplyDamageHook, esv__Character__ApplyDamage, esv::Character::ApplyDamageProc, WrappableFunction)
HOOK_DEFN(CDivinityStats_Character__HitInternal, CDivinityStats_Character__HitInternal, CDivinityStats_Character::HitInternalProc, WrappableFunction)
