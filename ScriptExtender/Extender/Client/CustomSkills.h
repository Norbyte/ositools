#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/Stats.h>
#include <GameDefinitions/GameObjects/Skill.h>
#include <Lua/Shared/LuaHelpers.h>
#include <Lua/Shared/LuaUserspaceClass.h>

#define FOR_ALL_ECL_SKILL_TYPES() \
	FOR_ECL_SKILL_TYPE(Zone) \
	FOR_ECL_SKILL_TYPE(SkillHeal) \
	FOR_ECL_SKILL_TYPE(Jump) \
	FOR_ECL_SKILL_TYPE(MultiStrike) \
	FOR_ECL_SKILL_TYPE(Path) \
	FOR_ECL_SKILL_TYPE(Projectile) \
	FOR_ECL_SKILL_TYPE(ProjectileStrike) \
	FOR_ECL_SKILL_TYPE(Quake) \
	FOR_ECL_SKILL_TYPE(Rain) \
	FOR_ECL_SKILL_TYPE(Rush) \
	FOR_ECL_SKILL_TYPE(Shout) \
	FOR_ECL_SKILL_TYPE(Storm) \
	FOR_ECL_SKILL_TYPE(Summon) \
	FOR_ECL_SKILL_TYPE(Target) \
	FOR_ECL_SKILL_TYPE(Teleportation) \
	FOR_ECL_SKILL_TYPE(Tornado) \
	FOR_ECL_SKILL_TYPE(Wall) \
	FOR_ECL_SKILL_TYPE(Dome)

#define FOR_ALL_SKILL_FUNCS(ty) \
	FOR_ECL_SKILL_FUNC(ty, Enter) \
	FOR_ECL_SKILL_FUNC(ty, Continue) \
	FOR_ECL_SKILL_FUNC(ty, Update) \
	FOR_ECL_SKILL_FUNC(ty, Tick) \
	FOR_ECL_SKILL_FUNC(ty, Exit) \
	FOR_ECL_SKILL_FUNC(ty, OnInputEvent) \
	FOR_ECL_SKILL_FUNC(ty, EnterBehaviour) \
	FOR_ECL_SKILL_FUNC(ty, ContinueBehaviour) \
	FOR_ECL_SKILL_FUNC(ty, UpdateBehaviour) \
	FOR_ECL_SKILL_FUNC(ty, TickBehaviour) \
	FOR_ECL_SKILL_FUNC(ty, ExitBehaviour) \
	FOR_ECL_SKILL_FUNC(ty, EnterAction) \
	FOR_ECL_SKILL_FUNC(ty, ContinueAction) \
	FOR_ECL_SKILL_FUNC(ty, UpdateAction) \
	FOR_ECL_SKILL_FUNC(ty, TickAction) \
	FOR_ECL_SKILL_FUNC(ty, ExitAction) \
	FOR_ECL_SKILL_FUNC(ty, GetTarget) \
	FOR_ECL_SKILL_FUNC(ty, GetTargetPosition) \
	FOR_ECL_SKILL_FUNC(ty, GetTargetMoveDistance) \
	FOR_ECL_SKILL_FUNC(ty, GetTargetRadius) \
	FOR_ECL_SKILL_FUNC(ty, ValidateTargetSight) \
	FOR_ECL_SKILL_FUNC(ty, ValidateTarget) \
	FOR_ECL_SKILL_FUNC(ty, FinishSkillState)

BEGIN_NS(ecl)

#define FOR_ECL_SKILL_FUNC(ty, name) \
	enum class name##Tag {}; \
	using name##Wrapper = WrappableFunction<name##Tag, SkillState::name##Proc>; \
	name##Wrapper name;

template <SkillType Ty>
struct CustomSkillVMTs
{
	FOR_ALL_SKILL_FUNCS(void)
};

#undef FOR_ECL_SKILL_FUNC


#define FOR_ECL_SKILL_TYPE(ty) CustomSkillVMTs<SkillType::ty> ##ty;

class CustomSkillVMTWrappers
{
public:
	void Initialize();

	FOR_ALL_ECL_SKILL_TYPES()

private:
	bool initialized_{ false };
};

#undef FOR_ECL_SKILL_TYPE

struct CustomSkillEventParams
{
	bool StopEvent{ false };
	bool PreventDefault{ false };
};

class UserspaceSkillStateClass : public lua::UserspaceClassBase
{
public:
	UserspaceSkillStateClass(lua_State* L, SkillState* state, lua::Ref const& ref);

	std::optional<bool> Enter(CustomSkillEventParams& e);
	std::optional<bool> Continue(CustomSkillEventParams& e);
	std::optional<bool> Update(CustomSkillEventParams& e);
	bool Tick(CustomSkillEventParams& e);
	std::optional<bool> Exit(CustomSkillEventParams& e);
	bool OnInputEvent(CustomSkillEventParams& e, uint16_t* eventRetVal, InputEvent const& inputEvent);
	std::optional<bool> EnterBehaviour(CustomSkillEventParams& e);
	std::optional<bool> ContinueBehaviour(CustomSkillEventParams& e);
	std::optional<bool> UpdateBehaviour(CustomSkillEventParams& e);
	bool TickBehaviour(CustomSkillEventParams& e, uint32_t* teamId, float timeDelta);
	std::optional<bool> ExitBehaviour(CustomSkillEventParams& e);
	std::optional<bool> EnterAction(CustomSkillEventParams& e);
	std::optional<bool> ContinueAction(CustomSkillEventParams& e);
	std::optional<bool> UpdateAction(CustomSkillEventParams& e, GameTime const& time);
	bool TickAction(CustomSkillEventParams& e);
	std::optional<bool> ExitAction(CustomSkillEventParams& e);
	std::optional<bool> GetTarget(CustomSkillEventParams& e, ComponentHandle& target, glm::vec3& targetPosition);
	std::optional<bool> GetTargetPosition(CustomSkillEventParams& e, glm::vec3& targetPosition);
	std::optional<float> GetTargetMoveDistance(CustomSkillEventParams& e);
	std::optional<float> GetTargetRadius(CustomSkillEventParams& e);
	std::optional<int32_t> ValidateTargetSight(CustomSkillEventParams& e, glm::vec3 const& target);
	std::optional<uint32_t> ValidateTarget(CustomSkillEventParams& e, uint64_t targetHandle, glm::vec3 const* targetPos, bool snapToGrid, bool fillInHeight);
	bool FinishSkillState(CustomSkillEventParams& e);

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
	bool OnEnter(SkillState::EnterProc* wrapped, SkillState* self);
	bool OnContinue(SkillState::ContinueProc* wrapped, SkillState* self);
	bool OnUpdate(SkillState::UpdateProc* wrapped, SkillState* self);
	void OnTick(SkillState::TickProc* wrapped, SkillState* self);
	bool OnExit(SkillState::ExitProc* wrapped, SkillState* self);

	uint16_t* OnOnInputEvent(SkillState::OnInputEventProc* wrapped, SkillState* self, uint16_t* eventRetVal, InputEvent const& inputEvent);
	bool OnEnterBehaviour(SkillState::EnterBehaviourProc* wrapped, SkillState* self);
	bool OnContinueBehaviour(SkillState::ContinueBehaviourProc* wrapped, SkillState* self);
	bool OnUpdateBehaviour(SkillState::UpdateBehaviourProc* wrapped, SkillState* self);
	void OnTickBehaviour(SkillState::TickBehaviourProc* wrapped, SkillState* self, uint32_t* teamId, float timeDelta);
	bool OnExitBehaviour(SkillState::ExitBehaviourProc* wrapped, SkillState* self);

	bool OnEnterAction(SkillState::EnterActionProc* wrapped, SkillState* self);
	bool OnContinueAction(SkillState::ContinueActionProc* wrapped, SkillState* self);
	bool OnUpdateAction(SkillState::UpdateActionProc* wrapped, SkillState* self, GameTime const& time);
	void OnTickAction(SkillState::TickActionProc* wrapped, SkillState* self);
	bool OnExitAction(SkillState::ExitActionProc* wrapped, SkillState* self);

	bool OnGetTarget(SkillState::GetTargetProc* wrapped, SkillState* self, ComponentHandle& target, glm::vec3& targetPosition);
	bool OnGetTargetPosition(SkillState::GetTargetPositionProc* wrapped, SkillState* self, glm::vec3& targetPosition);
	float OnGetTargetMoveDistance(SkillState::GetTargetMoveDistanceProc* wrapped, SkillState* self);
	float OnGetTargetRadius(SkillState::GetTargetRadiusProc* wrapped, SkillState* self);

	int32_t OnValidateTargetSight(SkillState::ValidateTargetSightProc* wrapped, SkillState* self, glm::vec3 const& target);
	uint32_t OnValidateTarget(SkillState::ValidateTargetProc* wrapped, SkillState* self, uint64_t targetHandle, glm::vec3 const* targetPos, bool snapToGrid, bool fillInHeight);

	void OnFinishSkillState(SkillState::FinishSkillStateProc* wrapped, SkillState* self);

	void ConstructUserState(SkillState* self);
	void DestroyUserState(SkillState* self);

	Vector<lua::UserObjectConstructor<UserspaceSkillStateClass>> globalConstructors_;
	Map<SkillType, Vector<lua::UserObjectConstructor<UserspaceSkillStateClass>>> typeConstructors_;
	Map<FixedString, Vector<lua::UserObjectConstructor<UserspaceSkillStateClass>>> idConstructors_;
	Map<SkillState*, Vector<GameUniquePtr<UserspaceSkillStateClass>>> userStates_;
};

END_NS()
