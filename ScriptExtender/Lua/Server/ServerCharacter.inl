BEGIN_NS(esv)

using namespace lua;

void CollectInventoryItemGuids(ObjectSet<FixedString>& guids, ComponentHandle inventoryHandle)
{
	auto inventory = esv::FindInventoryByHandle(inventoryHandle);
	if (inventory != nullptr) {
		int32_t index = 1;
		for (auto itemHandle : inventory->ItemsBySlot) {
			if (itemHandle) {
				auto item = esv::GetEntityWorld()->GetItem(itemHandle);
				if (item != nullptr) {
					guids.Add(item->MyGuid);
				}
			}
		}
	}
}

void CollectInventoryItems(ObjectSet<Item*>& items, ComponentHandle inventoryHandle)
{
	auto inventory = esv::FindInventoryByHandle(inventoryHandle);
	if (inventory != nullptr) {
		int32_t index = 1;
		for (auto itemHandle : inventory->ItemsBySlot) {
			if (itemHandle) {
				auto item = esv::GetEntityWorld()->GetItem(itemHandle);
				if (item != nullptr) {
					items.Add(item);
				}
			}
		}
	}
}

ObjectSet<FixedString> Character::GetInventoryItemGuids()
{
	ObjectSet<FixedString> guids;
	CollectInventoryItemGuids(guids, InventoryHandle);
	return guids;
}

ObjectSet<Item*> Character::GetInventoryItems()
{
	ObjectSet<Item*> items;
	CollectInventoryItems(items, InventoryHandle);
	return items;
}

ObjectSet<FixedString> Character::GetNearbyCharacters(float distance)
{
	ObjectSet<Character*> characters;

	auto pos = WorldPos;
	GetCharactersGeneric(characters, FixedString{}, [pos, distance](esv::Character* c) {
		return abs(glm::length(pos - c->WorldPos)) < distance;
	});

	ObjectSet<FixedString> characterGuids;
	for (auto const& ch : characters) {
		characterGuids.Add(ch->MyGuid);
	}

	return characterGuids;
}

ObjectSet<FixedString> Character::GetSummonGuids()
{
	ObjectSet<FixedString> summons;

	for (auto const& handle : SummonHandles) {
		auto summon = esv::GetEntityWorld()->GetCharacter(handle);
		if ((summon->Flags & esv::CharacterFlags::HasOwner) == esv::CharacterFlags::HasOwner) {
			summons.Add(summon->MyGuid);
		}
	}

	return summons;
}

ObjectSet<Character*> Character::GetSummons()
{
	ObjectSet<Character*> summons;

	for (auto const& handle : SummonHandles) {
		auto summon = esv::GetEntityWorld()->GetCharacter(handle);
		if ((summon->Flags & esv::CharacterFlags::HasOwner) == esv::CharacterFlags::HasOwner) {
			summons.Add(summon);
		}
	}

	return summons;
}

ObjectSet<FixedString> Character::GetSkillIds()
{
	ObjectSet<FixedString> skillIds;

	if (SkillManager != nullptr) {
		for (auto const& skill : SkillManager->Skills) {
			skillIds.Add(skill.Key);
		}
	}

	return skillIds;
}

Skill* Character::GetSkillInfo(FixedString const& skillId)
{
	if (SkillManager != nullptr) {
		auto skill = SkillManager->Skills.Find(skillId);
		if (skill) {
			return *skill;
		}
	}

	return nullptr;
}

std::optional<int> Character::GetCustomStatValue(FixedString const& statId)
{
	return esv::CustomStatHelpers::GetCharacterStat(Base.EntityObjectHandle, statId.GetString());
}

bool Character::SetCustomStatValue(FixedString const& statId, int value)
{
	return esv::CustomStatHelpers::SetCharacterStat(Base.EntityObjectHandle, statId.GetString(), value);
}

END_NS()
