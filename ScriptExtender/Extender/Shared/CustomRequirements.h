#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>
#include <GameDefinitions/Stats.h>
#include <Lua/Shared/LuaHelpers.h>

BEGIN_SE()

struct CustomRequirementCallbacks;

struct CustomRequirementDescriptor
{
	uint32_t RequirementId;
	FixedString EnumLabel;
	STDString Description;
	FixedString DescriptionHandle;

	CustomRequirementCallbacks* GetCallbacks();
};

class CustomRequirementRegistry
{
public:
	CustomRequirementDescriptor* AssignRequirement(FixedString const& name);
	CustomRequirementDescriptor* RegisterNewRequirement(FixedString const& name);
	void Clear();

	CustomRequirementDescriptor* Get(uint32_t requirementId);
	CustomRequirementDescriptor* Get(FixedString const& name);

	uint32_t MaxRequirementId() const
	{
		return nextRequirementId_;
	}

private:
	std::unordered_map<uint32_t, CustomRequirementDescriptor> requirements_;
	std::unordered_map<FixedString, CustomRequirementDescriptor*> names_;
	uint32_t nextRequirementId_{ (uint32_t)stats::RequirementType::TALENT_MagicCycles + 1 };
};

struct CustomRequirementContext
{
	// Context set for server checks
	esv::Character* ServerCharacter{ nullptr };
	esv::Item* ServerItem{ nullptr };
	
	// Context set for client checks
	ecl::Character* ClientCharacter{ nullptr };
	ecl::Item* ClientItem{ nullptr };

	stats::Item* ItemStats{ nullptr };
	FixedString SkillId;

	// Context set during CheckRequirement()
	stats::Character* CharacterStats{ nullptr };
	bool IsInCombat{ false };
	bool IsImmobile{ false };
	bool HasCharges{ false };
	ObjectSet<FixedString>* Tags{ nullptr };
	stats::Requirement* Requirement{ nullptr };
	bool ExcludeBoosts{ false };
};

struct CustomRequirementCallbacks
{
	uint32_t RequirementId;
	lua::Traits<lua::RegistryEntry, lua::CallableTrait> EvaluateCallback;
};

class CustomRequirementCallbackManager
{
public:
	CustomRequirementCallbacks* GetOrRegister(uint32_t requirementId);
	void Clear();

	std::optional<bool> Evaluate(stats::Character* character, stats::Requirement const& requirement);

private:
	std::unordered_map<uint32_t, CustomRequirementCallbacks> requirements_;
};

END_NS()
