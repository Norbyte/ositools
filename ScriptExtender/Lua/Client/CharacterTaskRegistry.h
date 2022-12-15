#pragma once

#include <Lua/Shared/LuaBinding.h>
#include <Lua/Shared/LuaHelpers.h>

BEGIN_NS(ecl::lua)

class ClientState;

class CharacterTaskRegistry
{
public:
	CharacterTaskRegistry(ClientState& state);

	std::optional<dse::lua::RegistryEntry> ConstructTask(FixedString const& taskType, Character* character);
	void RegisterTaskType(FixedString const& taskType, dse::lua::Ref const& constructor);
	
private:
	ClientState& state_;
	Map<FixedString, dse::lua::RegistryEntry> constructors_;
};

END_NS()
