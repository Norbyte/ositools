#include <Lua/Shared/LuaMethodHelpers.h>
#include <Extender/Server/CustomSkills.h>
#include <GameDefinitions/Symbols.h>

#define FOR_ESV_SKILL_FUNC(ty, name) dse::esv::CustomSkillVMTs<dse::SkillType::ty>::name##Wrapper * dse::esv::CustomSkillVMTs<dse::SkillType::ty>::name##Wrapper::gHook;
#define FOR_ESV_SKILL_TYPE(ty) FOR_ALL_ESV_SKILL_FUNCS(ty)

FOR_ALL_ESV_SKILL_TYPES()

#undef FOR_ESV_SKILL_TYPE
#undef FOR_ESV_SKILL_FUNC


BEGIN_NS(esv)

#define FOR_ESV_SKILL_FUNC(ty, name) vmts.name.Wrap(vmt->name);

template <SkillType Ty>
void HookVMTs(SkillState* state, CustomSkillVMTs<Ty>& vmts)
{
	auto vmt = *reinterpret_cast<SkillState::VMT**>(state);
	FOR_ALL_ESV_SKILL_FUNCS(void)
}

#undef FOR_ESV_SKILL_FUNC

void CustomSkillVMTWrappers::Initialize()
{
	if (initialized_) return;

	auto factory = GetStaticSymbols().esv__SkillStateFactory;
	if (factory && *factory) {
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

#define FOR_ESV_SKILL_TYPE(ty) if ((*factory)->Pools.size() > (unsigned)SkillType::ty) { HookVMTs<SkillType::ty>((*factory)->Pools[(unsigned)SkillType::ty]->Prototype, ##ty); }

		FOR_ALL_ESV_SKILL_TYPES()

#undef FOR_ESV_SKILL_TYPE

		DetourTransactionCommit();
	}

	initialized_ = true;
}

#define FOR_ESV_SKILL_FUNC(ty, name) vmts.##ty.name.SetWrapper(&CustomSkillStateManager::On##name, this);
#define FOR_ESV_SKILL_TYPE(ty) FOR_ALL_ESV_SKILL_FUNCS(ty)

CustomSkillStateManager::CustomSkillStateManager()
{
	auto& vmts = gExtender->GetServer().GetSkillVMTs();
	FOR_ALL_ESV_SKILL_TYPES()
}

#undef FOR_ESV_SKILL_TYPE
#undef FOR_ESV_SKILL_FUNC

#define FOR_ESV_SKILL_FUNC(ty, name) vmts.##ty.name.ClearHook();
#define FOR_ESV_SKILL_TYPE(ty) FOR_ALL_ESV_SKILL_FUNCS(ty)

CustomSkillStateManager::~CustomSkillStateManager()
{
	auto& vmts = gExtender->GetServer().GetSkillVMTs();
	FOR_ALL_ESV_SKILL_TYPES()
}

#undef FOR_ESV_SKILL_TYPE
#undef FOR_ESV_SKILL_FUNC

void CustomSkillStateManager::AddConstructor(lua_State* L, dse::lua::Ref const& ctor)
{
	globalConstructors_.push_back(lua::UserObjectConstructor<UserspaceSkillStateClass>(L, ctor));
}

void CustomSkillStateManager::AddConstructor(SkillType type, lua_State* L, dse::lua::Ref const& ctor)
{
	auto ctors = typeConstructors_.get_or_insert(type);
	ctors->push_back(lua::UserObjectConstructor<UserspaceSkillStateClass>(L, ctor));
}

void CustomSkillStateManager::AddConstructor(FixedString const& skillId, lua_State* L, dse::lua::Ref const& ctor)
{
	auto ctors = idConstructors_.get_or_insert(skillId);
	ctors->push_back(lua::UserObjectConstructor<UserspaceSkillStateClass>(L, ctor));
}

UserspaceSkillStateClass::UserspaceSkillStateClass(lua_State* L, SkillState* state, lua::Ref const& ref)
	: UserspaceClassBase(L, ref),
	state_(state)
{}

std::optional<bool> UserspaceSkillStateClass::Init(CustomSkillEventParams& e)
{
	return CallOptionalMethod("Init", Overload<bool>{}, &e, state_);
}

std::optional<bool> UserspaceSkillStateClass::Enter(CustomSkillEventParams& e)
{
	return CallOptionalMethod("Enter", Overload<bool>{}, &e, state_);
}

std::optional<bool> UserspaceSkillStateClass::Update(CustomSkillEventParams& e, GameTime const& time)
{
	return CallOptionalMethod("Update", Overload<bool>{}, &e, state_, const_cast<GameTime*>(&time));
}

bool UserspaceSkillStateClass::Tick(CustomSkillEventParams& e, int const& teamId)
{
	return CallOptionalMethod("Tick", Overload<void>{}, &e, state_, teamId);
}

bool UserspaceSkillStateClass::Exit(CustomSkillEventParams& e)
{
	return CallOptionalMethod("Exit", Overload<void>{}, &e, state_);
}

std::optional<bool> UserspaceSkillStateClass::EnterBehaviour(CustomSkillEventParams& e)
{
	return CallOptionalMethod("EnterBehaviour", Overload<bool>{}, &e, state_);
}

std::optional<bool> UserspaceSkillStateClass::ContinueBehaviour(CustomSkillEventParams& e)
{
	return CallOptionalMethod("ContinueBehaviour", Overload<bool>{}, &e, state_);
}

bool UserspaceSkillStateClass::UpdateBehaviour(CustomSkillEventParams& e, GameTime const& time)
{
	return CallOptionalMethod("UpdateBehaviour", Overload<void>{}, &e, state_, const_cast<GameTime*>(&time));
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

bool UserspaceSkillStateClass::UpdateAction(CustomSkillEventParams& e, GameTime const& time)
{
	return CallOptionalMethod("UpdateAction", Overload<void>{}, &e, state_);
}

bool UserspaceSkillStateClass::TickAction(CustomSkillEventParams& e)
{
	return CallOptionalMethod("TickAction", Overload<void>{}, &e, state_);
}

bool UserspaceSkillStateClass::ExitAction(CustomSkillEventParams& e)
{
	return CallOptionalMethod("ExitAction", Overload<void>{}, &e, state_);
}

std::optional<float> UserspaceSkillStateClass::GetTargetDistance(CustomSkillEventParams& e)
{
	return CallOptionalMethod("GetTargetDistance", Overload<float>{}, &e, state_);
}

bool UserspaceSkillStateClass::Finish(CustomSkillEventParams& e)
{
	return CallOptionalMethod("Finish", Overload<void>{}, &e, state_);
}

void ConstructState(lua_State* L, SkillState* skill, Vector<GameUniquePtr<UserspaceSkillStateClass>>& states, lua::UserObjectConstructor<UserspaceSkillStateClass>& ctor)
{
	auto userState = ctor.Construct(L, skill);
	userState.Push();
	auto nonNil = lua_type(L, -1) != LUA_TNIL;
	lua_pop(L, 1);
	if (nonNil) {
		states.push_back(MakeGameUnique<UserspaceSkillStateClass>(L, skill, userState));
	}
}

void CustomSkillStateManager::ConstructUserState(SkillState* self)
{
	LuaServerPin lua(ExtensionState::Get());
	auto L = lua->GetState();
	lua::StackCheck _(L);

	Vector<GameUniquePtr<UserspaceSkillStateClass>> states;

	for (auto& ctor : globalConstructors_) {
		ConstructState(L, self, states, ctor);
	}

	auto it = typeConstructors_.find(self->GetType());
	if (it != typeConstructors_.end()) {
		for (auto& ctor : it.Value()) {
			ConstructState(L, self, states, ctor);
		}
	}

	auto idIt = idConstructors_.find(self->SkillId);
	if (idIt != idConstructors_.end()) {
		for (auto& ctor : it.Value()) {
			ConstructState(L, self, states, ctor);
		}
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
	LuaServerPin lua(ExtensionState::Get()); \
	CustomSkillEventParams ev; \
	std::optional<ret> result; \
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
	LuaServerPin lua(ExtensionState::Get()); \
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


bool CustomSkillStateManager::OnInit(SkillState::EnterProc* wrapped, SkillState* self)
{
	DISPATCH_SKILL_EVENT(Init, bool, false)
}

bool CustomSkillStateManager::OnEnter(SkillState::EnterProc* wrapped, SkillState* self)
{
	DISPATCH_SKILL_EVENT(Enter, bool, false)
}

bool CustomSkillStateManager::OnUpdate(SkillState::UpdateProc* wrapped, SkillState* self, GameTime const& time)
{
	DISPATCH_SKILL_EVENT(Update, bool, false, time)
}

void CustomSkillStateManager::OnTick(SkillState::TickProc* wrapped, SkillState* self, int const& teamId)
{
	DISPATCH_SKILL_FUN(Tick, teamId)
}

void CustomSkillStateManager::OnExit(SkillState::ExitProc* wrapped, SkillState* self)
{
	DISPATCH_SKILL_FUN(Exit)
}

bool CustomSkillStateManager::OnEnterBehaviour(SkillState::EnterBehaviourProc* wrapped, SkillState* self)
{
	DISPATCH_SKILL_EVENT(EnterBehaviour, bool, false)
}

bool CustomSkillStateManager::OnContinueBehaviour(SkillState::ContinueBehaviourProc* wrapped, SkillState* self)
{
	DISPATCH_SKILL_EVENT(ContinueBehaviour, bool, false)
}

void CustomSkillStateManager::OnUpdateBehaviour(SkillState::UpdateBehaviourProc* wrapped, SkillState* self, GameTime const& time)
{
	DISPATCH_SKILL_FUN(UpdateBehaviour, time)
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

void CustomSkillStateManager::OnUpdateAction(SkillState::UpdateActionProc* wrapped, SkillState* self, GameTime const& time)
{
	DISPATCH_SKILL_FUN(UpdateAction, time)
}

void CustomSkillStateManager::OnTickAction(SkillState::TickActionProc* wrapped, SkillState* self)
{
	DISPATCH_SKILL_FUN(TickAction)
}

void CustomSkillStateManager::OnExitAction(SkillState::ExitActionProc* wrapped, SkillState* self)
{
	DISPATCH_SKILL_FUN(ExitAction)
}

float CustomSkillStateManager::OnGetTargetDistance(SkillState::GetTargetDistanceProc* wrapped, SkillState* self)
{
	DISPATCH_SKILL_EVENT(GetTargetDistance, float, 0.0f)
}

void CustomSkillStateManager::OnReset(SkillState::ResetProc* wrapped, SkillState* self)
{
	wrapped(self);
	ConstructUserState(self);
}

bool CustomSkillStateManager::OnFinish(SkillState::FinishProc* wrapped, SkillState* self)
{
	DISPATCH_SKILL_FUN(Finish)

	DestroyUserState(self);
	return true;
}


END_NS()
