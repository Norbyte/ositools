#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/Stats.h>
#include <GameDefinitions/GameObjects/Skill.h>
#include <Lua/Shared/LuaHelpers.h>
#include <Lua/Shared/LuaUserspaceClass.h>

#define FOR_ALL_ESV_SKILL_TYPES() \
	FOR_ESV_SKILL_TYPE(Zone) \
	FOR_ESV_SKILL_TYPE(SkillHeal) \
	FOR_ESV_SKILL_TYPE(Jump) \
	FOR_ESV_SKILL_TYPE(MultiStrike) \
	FOR_ESV_SKILL_TYPE(Path) \
	FOR_ESV_SKILL_TYPE(Projectile) \
	FOR_ESV_SKILL_TYPE(ProjectileStrike) \
	FOR_ESV_SKILL_TYPE(Quake) \
	FOR_ESV_SKILL_TYPE(Rain) \
	FOR_ESV_SKILL_TYPE(Rush) \
	FOR_ESV_SKILL_TYPE(Shout) \
	FOR_ESV_SKILL_TYPE(Storm) \
	FOR_ESV_SKILL_TYPE(Summon) \
	FOR_ESV_SKILL_TYPE(Target) \
	FOR_ESV_SKILL_TYPE(Teleportation) \
	FOR_ESV_SKILL_TYPE(Tornado) \
	FOR_ESV_SKILL_TYPE(Wall) \
	FOR_ESV_SKILL_TYPE(Dome)

#define FOR_ALL_ESV_SKILL_FUNCS(ty) \
	FOR_ESV_SKILL_FUNC(ty, Init) \
	FOR_ESV_SKILL_FUNC(ty, Enter) \
	FOR_ESV_SKILL_FUNC(ty, Update) \
	FOR_ESV_SKILL_FUNC(ty, Tick) \
	FOR_ESV_SKILL_FUNC(ty, Exit) \
	FOR_ESV_SKILL_FUNC(ty, EnterBehaviour) \
	FOR_ESV_SKILL_FUNC(ty, ContinueBehaviour) \
	FOR_ESV_SKILL_FUNC(ty, UpdateBehaviour) \
	FOR_ESV_SKILL_FUNC(ty, TickBehaviour) \
	FOR_ESV_SKILL_FUNC(ty, ExitBehaviour) \
	FOR_ESV_SKILL_FUNC(ty, EnterAction) \
	FOR_ESV_SKILL_FUNC(ty, ContinueAction) \
	FOR_ESV_SKILL_FUNC(ty, UpdateAction) \
	FOR_ESV_SKILL_FUNC(ty, TickAction) \
	FOR_ESV_SKILL_FUNC(ty, ExitAction) \
	FOR_ESV_SKILL_FUNC(ty, GetTargetDistance) \
	FOR_ESV_SKILL_FUNC(ty, Reset) \
	FOR_ESV_SKILL_FUNC(ty, Finish)

BEGIN_NS(esv)

#define FOR_ESV_SKILL_FUNC(ty, name) \
	enum class name##Tag {}; \
	using name##Wrapper = WrappableFunction<name##Tag, SkillState::name##Proc>; \
	name##Wrapper name;

template <SkillType Ty>
struct CustomSkillVMTs
{
	FOR_ALL_ESV_SKILL_FUNCS(void)
};

#undef FOR_ESV_SKILL_FUNC


#define FOR_ESV_SKILL_TYPE(ty) CustomSkillVMTs<SkillType::ty> ##ty;

class CustomSkillVMTWrappers
{
public:
	void Initialize();

	FOR_ALL_ESV_SKILL_TYPES()

private:
	bool initialized_{ false };
};

#undef FOR_ESV_SKILL_TYPE

struct CustomSkillEventParams
{
	bool StopEvent{ false };
	bool PreventDefault{ false };
};

class UserspaceSkillStateClass : public lua::UserspaceClassBase
{
public:
	UserspaceSkillStateClass(lua_State* L, SkillState* state, lua::Ref const& ref);

	std::optional<bool> Init(CustomSkillEventParams& e);
	std::optional<bool> Enter(CustomSkillEventParams& e);
	std::optional<bool> Update(CustomSkillEventParams& e, GameTime const& time);
	bool Tick(CustomSkillEventParams& e, int const& teamId);
	bool Exit(CustomSkillEventParams& e);
	std::optional<bool> EnterBehaviour(CustomSkillEventParams& e);
	std::optional<bool> ContinueBehaviour(CustomSkillEventParams& e);
	bool UpdateBehaviour(CustomSkillEventParams& e, GameTime const& time);
	bool TickBehaviour(CustomSkillEventParams& e, uint32_t* teamId, float timeDelta);
	std::optional<bool> ExitBehaviour(CustomSkillEventParams& e);
	std::optional<bool> EnterAction(CustomSkillEventParams& e);
	std::optional<bool> ContinueAction(CustomSkillEventParams& e);
	bool UpdateAction(CustomSkillEventParams& e, GameTime const& time);
	bool TickAction(CustomSkillEventParams& e);
	bool ExitAction(CustomSkillEventParams& e);
	std::optional<float> GetTargetDistance(CustomSkillEventParams& e);
	bool Reset(CustomSkillEventParams& e);
	bool Finish(CustomSkillEventParams& e, bool force);

private:
	SkillState* state_;
};

class CustomSkillStateManager
{
public:
	CustomSkillStateManager();
	~CustomSkillStateManager();

	void AddConstructor(lua_State* L, dse::lua::Ref const& ctor);
	void AddConstructor(SkillType type, lua_State* L, dse::lua::Ref const& ctor);
	void AddConstructor(FixedString const& skillId, lua_State* L, dse::lua::Ref const& ctor);

private:
	bool OnInit(SkillState::EnterProc* wrapped, SkillState* self);
	bool OnEnter(SkillState::EnterProc* wrapped, SkillState* self);
	bool OnUpdate(SkillState::UpdateProc* wrapped, SkillState* self, GameTime const& time);
	void OnTick(SkillState::TickProc* wrapped, SkillState* self, int const& teamId);
	void OnExit(SkillState::ExitProc* wrapped, SkillState* self);

	bool OnEnterBehaviour(SkillState::EnterBehaviourProc* wrapped, SkillState* self);
	bool OnContinueBehaviour(SkillState::ContinueBehaviourProc* wrapped, SkillState* self);
	void OnUpdateBehaviour(SkillState::UpdateBehaviourProc* wrapped, SkillState* self, GameTime const& time);
	void OnTickBehaviour(SkillState::TickBehaviourProc* wrapped, SkillState* self, uint32_t* teamId, float timeDelta);
	bool OnExitBehaviour(SkillState::ExitBehaviourProc* wrapped, SkillState* self);

	bool OnEnterAction(SkillState::EnterActionProc* wrapped, SkillState* self);
	bool OnContinueAction(SkillState::ContinueActionProc* wrapped, SkillState* self);
	void OnUpdateAction(SkillState::UpdateActionProc* wrapped, SkillState* self, GameTime const& time);
	void OnTickAction(SkillState::TickActionProc* wrapped, SkillState* self);
	void OnExitAction(SkillState::ExitActionProc* wrapped, SkillState* self);
	float OnGetTargetDistance(SkillState::GetTargetDistanceProc* wrapped, SkillState* self);

	void OnReset(SkillState::ResetProc* wrapped, SkillState* self);
	bool OnFinish(SkillState::FinishProc* wrapped, SkillState* self, bool force);

	void ConstructUserState(SkillState* self);
	void DestroyUserState(SkillState* self);

	Vector<lua::UserObjectConstructor<UserspaceSkillStateClass>> globalConstructors_;
	Map<SkillType, Vector<lua::UserObjectConstructor<UserspaceSkillStateClass>>> typeConstructors_;
	Map<FixedString, Vector<lua::UserObjectConstructor<UserspaceSkillStateClass>>> idConstructors_;
	Map<SkillState*, Vector<GameUniquePtr<UserspaceSkillStateClass>>> userStates_;
};

END_NS()
