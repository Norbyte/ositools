#include <Extender/Client/CharacterTaskBinder.h>

BEGIN_NS(ecl)

std::optional<uint32_t> CharacterTaskBinder::GetType(FixedString const& typeName) const
{
	auto it = typeIds_.find(typeName);
	if (it == typeIds_.end()) {
		return {};
	} else {
		return it->second;
	}
}

uint32_t CharacterTaskBinder::GetOrRegisterType(FixedString const& typeName)
{
	auto it = typeIds_.find(typeName);
	if (it != typeIds_.end()) {
		return it->second;
	} else {
		auto id = nextTypeId_++;
		typeIds_.insert(std::make_pair(typeName, id));
		return id;
	}
}

END_NS()
