#include <Lua/Shared/LuaMethodHelpers.h>
#include <Extender/Shared/CustomSkills.h>
#include <GameDefinitions/Symbols.h>

#define FOR_SKILL_FUNC(ty, name) dse::ecl::CustomSkillVMTs<dse::SkillType::ty>::name##Wrapper * dse::ecl::CustomSkillVMTs<dse::SkillType::ty>::name##Wrapper::gHook;
#define FOR_SKILL_TYPE(ty) FOR_ALL_SKILL_FUNCS(ty)

FOR_ALL_SKILL_TYPES()

#undef FOR_SKILL_TYPE
#undef FOR_SKILL_FUNC


BEGIN_NS(ecl)

#define FOR_SKILL_FUNC(ty, name) vmts.name.Wrap(vmt->name);

template <SkillType Ty>
void HookVMTs(SkillState* state, CustomSkillVMTs<Ty>& vmts)
{
	auto vmt = *reinterpret_cast<SkillState::VMT**>(state);
	FOR_ALL_SKILL_FUNCS(void)
}

#undef FOR_SKILL_FUNC

void CustomSkillVMTWrappers::Initialize()
{
	if (initialized_) return;

	auto factory = GetStaticSymbols().ecl__SkillStateFactory;
	if (factory && *factory) {
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

#define FOR_SKILL_TYPE(ty) if ((*factory)->Pools.size() > (unsigned)SkillType::ty) { HookVMTs<SkillType::ty>((*factory)->Pools[(unsigned)SkillType::ty]->Prototype, ##ty); }

		FOR_ALL_SKILL_TYPES()

#undef FOR_SKILL_TYPE

		DetourTransactionCommit();
	}

	initialized_ = true;
}

#define FOR_SKILL_FUNC(ty, name) vmts.##ty.name.SetWrapper(&CustomSkillStateManager::On##name, this);
#define FOR_SKILL_TYPE(ty) FOR_ALL_SKILL_FUNCS(ty)

CustomSkillStateManager::CustomSkillStateManager()
{
	auto& vmts = gExtender->GetClient().GetSkillVMTs();
	FOR_ALL_SKILL_TYPES()
}

#undef FOR_SKILL_TYPE
#undef FOR_SKILL_FUNC

#define FOR_SKILL_FUNC(ty, name) vmts.##ty.name.ClearHook();
#define FOR_SKILL_TYPE(ty) FOR_ALL_SKILL_FUNCS(ty)

CustomSkillStateManager::~CustomSkillStateManager()
{
	auto& vmts = gExtender->GetClient().GetSkillVMTs();
	FOR_ALL_SKILL_TYPES()
}

#undef FOR_SKILL_TYPE
#undef FOR_SKILL_FUNC

void CustomSkillStateManager::SetConstructor(SkillType type, lua_State* L, dse::lua::Ref const& ctor)
{
	typeConstructors_.insert(std::make_pair(type, dse::lua::RegistryEntry(L, ctor)));
}

template <class T>
class UserObjectConstructor
{
public:
	UserObjectConstructor(lua_State* L, lua::Ref const& ref)
		: ref_(L, ref)
	{}

	lua::RegistryEntry Construct(lua_State* L, SkillState* state)
	{
		lua::StackCheck _(L);
		lua::ProtectedFunctionCaller<std::tuple<SkillState*>, dse::lua::RegistryEntry> caller;
		caller.Function = ref_;
		caller.Args = std::tuple(state);
		if (caller.Call(L, "constructing user object")) {
			return std::move(caller.Retval.Value);
		} else {
			return {};
		}
	}

private:
	lua::RegistryEntry ref_;
};

UserspaceSkillStateClass::UserspaceSkillStateClass(lua_State* L, SkillState* state, lua::Ref const& ref)
	: UserspaceClassBase(L, ref),
	state_(state)
{}

std::optional<bool> UserspaceSkillStateClass::Enter(CustomSkillEventParams& e)
{
	return CallOptionalMethod("Enter", Overload<bool>{}, &e, state_);
}

std::optional<bool> UserspaceSkillStateClass::Continue(CustomSkillEventParams& e)
{
	return CallOptionalMethod("Continue", Overload<bool>{}, &e, state_);
}

std::optional<bool> UserspaceSkillStateClass::Update(CustomSkillEventParams& e)
{
	return CallOptionalMethod("Update", Overload<bool>{}, &e, state_);
}

bool UserspaceSkillStateClass::Tick(CustomSkillEventParams& e)
{
	return CallOptionalMethod("Tick", Overload<void>{}, &e, state_);
}

std::optional<bool> UserspaceSkillStateClass::Exit(CustomSkillEventParams& e)
{
	return CallOptionalMethod("Exit", Overload<bool>{}, &e, state_);
}

bool UserspaceSkillStateClass::OnInputEvent(CustomSkillEventParams& e, uint16_t* eventRetVal, InputEvent const& inputEvent)
{
	return CallOptionalMethod("OnInputEvent", Overload<void>{}, &e, state_, const_cast<InputEvent*>(&inputEvent));
}

std::optional<bool> UserspaceSkillStateClass::EnterBehaviour(CustomSkillEventParams& e)
{
	return CallOptionalMethod("EnterBehaviour", Overload<bool>{}, &e, state_);
}

std::optional<bool> UserspaceSkillStateClass::ContinueBehaviour(CustomSkillEventParams& e)
{
	return CallOptionalMethod("ContinueBehaviour", Overload<bool>{}, &e, state_);
}

std::optional<bool> UserspaceSkillStateClass::UpdateBehaviour(CustomSkillEventParams& e)
{
	return CallOptionalMethod("UpdateBehaviour", Overload<bool>{}, &e, state_);
}

bool UserspaceSkillStateClass::TickBehaviour(CustomSkillEventParams& e, uint32_t* teamId, float timeDelta)
{
	return CallOptionalMethod("TickBehaviour", Overload<void>{}, &e, state_);
}

std::optional<bool> UserspaceSkillStateClass::ExitBehaviour(CustomSkillEventParams& e)
{
	return CallOptionalMethod("ExitBehaviour", Overload<bool>{}, &e, state_);
}

std::optional<bool> UserspaceSkillStateClass::EnterAction(CustomSkillEventParams& e)
{
	return CallOptionalMethod("EnterAction", Overload<bool>{}, &e, state_);
}

std::optional<bool> UserspaceSkillStateClass::ContinueAction(CustomSkillEventParams& e)
{
	return CallOptionalMethod("ContinueAction", Overload<bool>{}, &e, state_);
}

std::optional<bool> UserspaceSkillStateClass::UpdateAction(CustomSkillEventParams& e, GameTime const& time)
{
	return CallOptionalMethod("UpdateAction", Overload<bool>{}, &e, state_);
}

bool UserspaceSkillStateClass::TickAction(CustomSkillEventParams& e)
{
	return CallOptionalMethod("TickAction", Overload<void>{}, &e, state_);
}

std::optional<bool> UserspaceSkillStateClass::ExitAction(CustomSkillEventParams& e)
{
	return CallOptionalMethod("ExitAction", Overload<bool>{}, &e, state_);
}

std::optional<bool> UserspaceSkillStateClass::GetTarget(CustomSkillEventParams& e, ComponentHandle& target, glm::vec3& targetPosition)
{
	return CallOptionalMethod("GetTarget", Overload<bool>{}, &e, state_, target, targetPosition);
}

std::optional<bool> UserspaceSkillStateClass::GetTargetPosition(CustomSkillEventParams& e, glm::vec3& targetPosition)
{
	return CallOptionalMethod("GetTargetPosition", Overload<bool>{}, &e, state_, targetPosition);
}

std::optional<float> UserspaceSkillStateClass::GetTargetMoveDistance(CustomSkillEventParams& e)
{
	return CallOptionalMethod("GetTargetMoveDistance", Overload<float>{}, &e, state_);
}

std::optional<float> UserspaceSkillStateClass::GetTargetRadius(CustomSkillEventParams& e)
{
	return CallOptionalMethod("GetTargetRadius", Overload<float>{}, &e, state_);
}

std::optional<bool> UserspaceSkillStateClass::ValidateTargetSight(CustomSkillEventParams& e, glm::vec3 const& target)
{
	return CallOptionalMethod("ValidateTargetSight", Overload<bool>{}, &e, state_, target);
}

std::optional<uint32_t> UserspaceSkillStateClass::ValidateTarget(CustomSkillEventParams& e, uint64_t targetHandle, glm::vec3 const* targetPos, bool snapToGrid, bool fillInHeight)
{
	return CallOptionalMethod("ValidateTarget", Overload<uint32_t>{}, &e, state_, ComponentHandle{targetHandle}, targetPos ? *targetPos : std::optional<glm::vec3>{}, snapToGrid, fillInHeight);
}

bool UserspaceSkillStateClass::FinishSkillState(CustomSkillEventParams& e)
{
	return CallOptionalMethod("FinishSkillState", Overload<void>{}, &e, state_);
}


void CustomSkillStateManager::ConstructUserState(SkillState* self)
{
	Vector<GameUniquePtr<UserspaceSkillStateClass>> states;

	auto it = typeConstructors_.find(self->GetType());
	if (it != typeConstructors_.end()) {
		LuaClientPin lua(ExtensionState::Get());
		auto L = lua->GetState();
		lua::StackCheck _(L);
		UserObjectConstructor<UserspaceSkillStateClass> ctor(L, it.Value());
		auto userState = ctor.Construct(L, self);
		states.push_back(MakeGameUnique<UserspaceSkillStateClass>(L, self, userState));
	}

	if (!states.empty()) {
		userStates_.insert(std::make_pair(self, std::move(states)));
	}
}

void CustomSkillStateManager::DestroyUserState(SkillState* self)
{
	auto states = userStates_.find(self);
	if (states != userStates_.end()) {
		userStates_.erase(states);
	}
}

#define DISPATCH_SKILL_EVENT(fun, ret, defval, ...) \
	CustomSkillEventParams ev; \
	std::optional<bool> result; \
	auto states = userStates_.find(self); \
	if (states != userStates_.end()) { \
		for (auto& state : states.Value()) { \
			result = state->fun(ev, __VA_ARGS__); \
			if (ev.StopEvent) { \
				break; \
			} \
		} \
	} \
 \
	if (ev.PreventDefault) { \
		return result ? *result : defval; \
	} else { \
		return wrapped(self, __VA_ARGS__); \
	}

#define DISPATCH_SKILL_FUN(fun, ...) \
	CustomSkillEventParams ev; \
	auto states = userStates_.find(self); \
	if (states != userStates_.end()) { \
		for (auto& state : states.Value()) { \
			state->fun(ev, __VA_ARGS__); \
			if (ev.StopEvent) { \
				break; \
			} \
		} \
	} \
 \
	if (!ev.PreventDefault) { \
		wrapped(self, __VA_ARGS__); \
	}


bool CustomSkillStateManager::OnEnter(SkillState::EnterProc* wrapped, SkillState* self)
{
	ConstructUserState(self);

	DISPATCH_SKILL_EVENT(Enter, bool, false)
}

bool CustomSkillStateManager::OnContinue(SkillState::ContinueProc* wrapped, SkillState* self)
{
	DISPATCH_SKILL_EVENT(Continue, bool, false)
}

bool CustomSkillStateManager::OnUpdate(SkillState::UpdateProc* wrapped, SkillState* self)
{
	DISPATCH_SKILL_EVENT(Update, bool, false)
}

void CustomSkillStateManager::OnTick(SkillState::TickProc* wrapped, SkillState* self)
{
	DISPATCH_SKILL_FUN(Tick)
}

bool CustomSkillStateManager::OnExit(SkillState::ExitProc* wrapped, SkillState* self)
{
	DISPATCH_SKILL_EVENT(Exit, bool, false)
}

uint16_t* CustomSkillStateManager::OnOnInputEvent(SkillState::OnInputEventProc* wrapped, SkillState* self, uint16_t* eventRetVal, InputEvent const& inputEvent)
{
	DISPATCH_SKILL_FUN(OnInputEvent, eventRetVal, inputEvent)

	return eventRetVal;
}

bool CustomSkillStateManager::OnEnterBehaviour(SkillState::EnterBehaviourProc* wrapped, SkillState* self)
{
	DISPATCH_SKILL_EVENT(EnterBehaviour, bool, false)
}

bool CustomSkillStateManager::OnContinueBehaviour(SkillState::ContinueBehaviourProc* wrapped, SkillState* self)
{
	DISPATCH_SKILL_EVENT(ContinueBehaviour, bool, false)
}

bool CustomSkillStateManager::OnUpdateBehaviour(SkillState::UpdateBehaviourProc* wrapped, SkillState* self)
{
	DISPATCH_SKILL_EVENT(UpdateBehaviour, bool, false)
}

void CustomSkillStateManager::OnTickBehaviour(SkillState::TickBehaviourProc* wrapped, SkillState* self, uint32_t* teamId, float timeDelta)
{
	DISPATCH_SKILL_FUN(TickBehaviour, teamId, timeDelta)
}

bool CustomSkillStateManager::OnExitBehaviour(SkillState::ExitBehaviourProc* wrapped, SkillState* self)
{
	DISPATCH_SKILL_EVENT(ExitBehaviour, bool, false)
}

bool CustomSkillStateManager::OnEnterAction(SkillState::EnterActionProc* wrapped, SkillState* self)
{
	DISPATCH_SKILL_EVENT(EnterAction, bool, false)
}

bool CustomSkillStateManager::OnContinueAction(SkillState::ContinueActionProc* wrapped, SkillState* self)
{
	DISPATCH_SKILL_EVENT(ContinueAction, bool, false)
}

bool CustomSkillStateManager::OnUpdateAction(SkillState::UpdateActionProc* wrapped, SkillState* self, GameTime const& time)
{
	DISPATCH_SKILL_EVENT(UpdateAction, bool, false, time)
}

void CustomSkillStateManager::OnTickAction(SkillState::TickActionProc* wrapped, SkillState* self)
{
	DISPATCH_SKILL_FUN(TickAction)
}

bool CustomSkillStateManager::OnExitAction(SkillState::ExitActionProc* wrapped, SkillState* self)
{
	DISPATCH_SKILL_EVENT(ExitAction, bool, false)
}

bool CustomSkillStateManager::OnGetTarget(SkillState::GetTargetProc* wrapped, SkillState* self, ComponentHandle& target, glm::vec3& targetPosition)
{
	DISPATCH_SKILL_EVENT(GetTarget, bool, false, target, targetPosition)
}

bool CustomSkillStateManager::OnGetTargetPosition(SkillState::GetTargetPositionProc* wrapped, SkillState* self, glm::vec3& targetPosition)
{
	DISPATCH_SKILL_EVENT(GetTargetPosition, bool, false, targetPosition)
}

float CustomSkillStateManager::OnGetTargetMoveDistance(SkillState::GetTargetMoveDistanceProc* wrapped, SkillState* self)
{
	DISPATCH_SKILL_EVENT(GetTargetMoveDistance, float, 0.0f)
}

float CustomSkillStateManager::OnGetTargetRadius(SkillState::GetTargetRadiusProc* wrapped, SkillState* self)
{
	DISPATCH_SKILL_EVENT(GetTargetRadius, float, 0.0f)
}

bool CustomSkillStateManager::OnValidateTargetSight(SkillState::ValidateTargetSightProc* wrapped, SkillState* self, glm::vec3 const& target)
{
	DISPATCH_SKILL_EVENT(ValidateTargetSight, bool, false, target)
}

uint32_t CustomSkillStateManager::OnValidateTarget(SkillState::ValidateTargetProc* wrapped, SkillState* self, uint64_t targetHandle, glm::vec3 const* targetPos, bool snapToGrid, bool fillInHeight)
{
	DISPATCH_SKILL_EVENT(ValidateTarget, uint32_t, 0, targetHandle, targetPos, snapToGrid, fillInHeight)
}

void CustomSkillStateManager::OnFinishSkillState(SkillState::FinishSkillStateProc* wrapped, SkillState* self)
{
	DISPATCH_SKILL_FUN(FinishSkillState)

	DestroyUserState(self);
}


END_NS()
