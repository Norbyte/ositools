#pragma once

#include <GameDefinitions/Base/Base.h>
#include <GameDefinitions/Enumerations.h>

BEGIN_NS(ecl)

// Manages registrations for userspace character task types.
// When the Lua state is reinitialized, all registered constructors and handler instances are lost,
// but the CharacterTask instances are still kept on the character.
// When a type is re-registered, updating the task will re-construct the userspace handler object.
class CharacterTaskBinder
{
public:
	std::optional<uint32_t> GetType(FixedString const& typeName) const;
	uint32_t GetOrRegisterType(FixedString const& typeName);

	std::unordered_map<FixedString, uint32_t> const& GetAll() const
	{
		return typeIds_;
	}

private:
	std::unordered_map<FixedString, uint32_t> typeIds_;
	// Highest vanilla type ID is DefaultTargetAction (29)
	uint32_t nextTypeId_{ 30 };
};

END_NS()
