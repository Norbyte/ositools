#include <Extender/Shared/CustomConditions.h>
#include <ScriptHelpers.h>
#include <Lua/Shared/LuaMethodHelpers.h>

BEGIN_SE()

CustomConditionCallbacks* CustomConditionDescriptor::GetCallbacks()
{
	LuaVirtualPin lua(gExtender->GetCurrentExtensionState());
	if (lua) {
		return lua->GetCustomConditionCallbacks().GetOrRegister(ConditionId);
	} else {
		return nullptr;
	}
}


CustomConditionDescriptor* CustomConditionRegistry::AssignCondition(FixedString const& name)
{
	auto id = EnumInfo<stats::ConditionType>::Find(name);
	if (!id) {
		OsiError("Tried to register condition info for nonexistent condition type '" << name << "'!");
		return nullptr;
	}

	auto desc = conditions_.insert(std::make_pair((uint32_t)*id, CustomConditionDescriptor{}));
	desc.first->second.ConditionId = (uint32_t)*id;
	desc.first->second.EnumLabel = name;

	names_.insert(std::make_pair(name, &desc.first->second));
	return &desc.first->second;
}

CustomConditionDescriptor* CustomConditionRegistry::RegisterNewCondition(FixedString const& name)
{
	auto conditionType = EnumInfo<stats::ConditionType>::Find(name);
	if (conditionType) {
		if (*conditionType <= LastEngineType) {
			OsiError("Tried to register condition type '" << name << "' multiple times!");
			return nullptr;
		}

		auto it = conditions_.find((uint32_t)*conditionType);
		assert(it != conditions_.end());
		return &it->second;
	}

	auto id = nextConditionId_++;

	auto desc = conditions_.insert(std::make_pair(id, CustomConditionDescriptor{}));
	desc.first->second.ConditionId = id;
	desc.first->second.EnumLabel = name;

	names_.insert(std::make_pair(name, &desc.first->second));
	EnumInfo<stats::ConditionType>::Store->Add(id, name.GetStringOrDefault());

	auto targetCondEnum = GetStaticSymbols().GetStats()->ModifierValueLists.Find("SkillTargetCondition");
	targetCondEnum->Values.insert(std::make_pair(name, id));

	return &desc.first->second;
}

void CustomConditionRegistry::Clear()
{
	EnumInfo<stats::ConditionType>::Store->Labels.resize((uint32_t)LastEngineType + 1);
	auto& enumInfo = EnumInfo<stats::ConditionType>::Store->Values;
	for (auto const& ty : names_) {
		auto it = enumInfo.find(ty.first);
		if (it != enumInfo.end()) {
			enumInfo.erase(it);
		}
	}

	conditions_.clear();
	names_.clear();
	nextConditionId_ = (uint32_t)LastEngineType + 1;
}

CustomConditionDescriptor* CustomConditionRegistry::Get(uint32_t conditionId)
{
	auto it = conditions_.find(conditionId);
	if (it != conditions_.end()) {
		return &it->second;
	}

	return nullptr;
}

CustomConditionDescriptor* CustomConditionRegistry::Get(FixedString const& name)
{
	auto it = names_.find(name);
	if (it != names_.end()) {
		return it->second;
	}

	return nullptr;
}

bool CustomConditionRegistry::ServerConditionCheckProcess(esv::ServerConditionCheck::ProcessConditionProc* wrapped,
	esv::ServerConditionCheck* self, uint32_t conditionId, FixedString const& stringParam)
{
	LuaVirtualPin lua;
	if (lua) {
		auto result = lua->GetCustomConditionCallbacks().Evaluate(self, conditionId, stringParam);
		if (result) {
			return *result;
		}
	}

	return wrapped(self, conditionId, stringParam);
}

bool CustomConditionRegistry::ClientConditionCheckProcess(ecl::ClientConditionCheck::ProcessConditionProc* wrapped,
	ecl::ClientConditionCheck* self, uint32_t conditionId, FixedString const& stringParam)
{
	LuaVirtualPin lua;
	if (lua) {
		auto result = lua->GetCustomConditionCallbacks().Evaluate(self, conditionId, stringParam);
		if (result) {
			return *result;
		}
	}

	return wrapped(self, conditionId, stringParam);
}


CustomConditionCallbacks* CustomConditionCallbackManager::GetOrRegister(uint32_t conditionId)
{
	auto it = conditions_.find(conditionId);
	if (it != conditions_.end()) {
		return &it->second;
	}

	auto desc = conditions_.insert(std::make_pair(conditionId, CustomConditionCallbacks{}));
	desc.first->second.ConditionId = conditionId;
	return &desc.first->second;
}

void CustomConditionCallbackManager::Clear()
{
	conditions_.clear();
}

std::optional<bool> CustomConditionCallbackManager::Evaluate(esv::ServerConditionCheck* check, uint32_t conditionId, FixedString const& stringParam)
{
	auto it = conditions_.find(conditionId);
	if (it != conditions_.end() && *it->second.EvaluateCallback) {
		auto result = lua::ProtectedCallFunction<bool>(*it->second.EvaluateCallback, check, conditionId, stringParam);
		if (result) {
			return *result;
		}
	}

	return {};
}

std::optional<bool> CustomConditionCallbackManager::Evaluate(ecl::ClientConditionCheck* check, uint32_t conditionId, FixedString const& stringParam)
{
	auto it = conditions_.find(conditionId);
	if (it != conditions_.end() && *it->second.EvaluateCallback) {
		auto result = lua::ProtectedCallFunction<bool>(*it->second.EvaluateCallback, check, conditionId, stringParam);
		if (result) {
			return *result;
		}
	}

	return {};
}

END_SE()
