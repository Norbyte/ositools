#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/Stats.h>
#include <Lua/Shared/LuaHelpers.h>

BEGIN_SE()

struct CustomConditionCallbacks;

struct CustomConditionDescriptor
{
	uint32_t ConditionId;
	FixedString EnumLabel;

	CustomConditionCallbacks* GetCallbacks();
};

class CustomConditionRegistry
{
public:
	CustomConditionDescriptor* AssignCondition(FixedString const& name);
	CustomConditionDescriptor* RegisterNewCondition(FixedString const& name);
	void Clear();

	CustomConditionDescriptor* Get(uint32_t conditionId);
	CustomConditionDescriptor* Get(FixedString const& name);

	uint32_t MaxConditionId() const
	{
		return nextConditionId_;
	}

	bool ServerConditionCheckProcess(esv::ServerConditionCheck::ProcessConditionProc* wrapped,
		esv::ServerConditionCheck* self, uint32_t conditionId, FixedString const& stringParam);
	bool ClientConditionCheckProcess(ecl::ClientConditionCheck::ProcessConditionProc* wrapped,
		ecl::ClientConditionCheck* self, uint32_t conditionId, FixedString const& stringParam);

private:
	static constexpr stats::ConditionType LastEngineType = stats::ConditionType::MagicArmourUp;

	std::unordered_map<uint32_t, CustomConditionDescriptor> conditions_;
	std::unordered_map<FixedString, CustomConditionDescriptor*> names_;
	uint32_t nextConditionId_{ (uint32_t)LastEngineType + 1 };
};

struct CustomConditionCallbacks
{
	uint32_t ConditionId;
	lua::Traits<lua::RegistryEntry, lua::CallableTrait> EvaluateCallback;
};

class CustomConditionCallbackManager
{
public:
	CustomConditionCallbacks* GetOrRegister(uint32_t conditionId);
	void Clear();

	std::optional<bool> Evaluate(esv::ServerConditionCheck* check, uint32_t conditionId, FixedString const& stringParam);
	std::optional<bool> Evaluate(ecl::ClientConditionCheck* check, uint32_t conditionId, FixedString const& stringParam);

private:
	std::unordered_map<uint32_t, CustomConditionCallbacks> conditions_;
};

END_NS()
