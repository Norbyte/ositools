BEGIN_NS(esv)

using namespace lua;

void CollectInventoryItemGuids(ObjectSet<FixedString>& guids, ComponentHandle inventoryHandle)
{
	auto inventory = esv::FindInventoryByHandle(inventoryHandle);
	if (inventory != nullptr) {
		int32_t index = 1;
		for (auto itemHandle : inventory->ItemsBySlot) {
			if (itemHandle) {
				auto item = esv::GetEntityWorld()->GetComponent<Item>(itemHandle);
				if (item != nullptr) {
					guids.push_back(item->MyGuid);
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
				auto item = esv::GetEntityWorld()->GetComponent<Item>(itemHandle);
				if (item != nullptr) {
					items.push_back(item);
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

/// <summary>
/// Retrieves the GUID of all characters within the specified range.
/// </summary>
/// <param name="distance">Maximum character distance</param>
ObjectSet<FixedString> Character::GetNearbyCharacters(float distance)
{
	ObjectSet<Character*> characters;

	auto pos = WorldPos;
	GetCharactersGeneric(characters, FixedString{}, [pos, distance](esv::Character* c) {
		return abs(glm::length(pos - c->WorldPos)) < distance;
	});

	ObjectSet<FixedString> characterGuids;
	for (auto const& ch : characters) {
		characterGuids.push_back(ch->MyGuid);
	}

	return characterGuids;
}

ObjectSet<FixedString> Character::GetSummonGuids()
{
	ObjectSet<FixedString> summons;

	for (auto const& handle : SummonHandles) {
		auto summon = esv::GetEntityWorld()->GetComponent<Character>(handle);
		if ((summon->Flags & esv::CharacterFlags::HasOwner) == esv::CharacterFlags::HasOwner) {
			summons.push_back(summon->MyGuid);
		}
	}

	return summons;
}

ObjectSet<Character*> Character::GetSummons()
{
	ObjectSet<Character*> summons;

	for (auto const& handle : SummonHandles) {
		auto summon = esv::GetEntityWorld()->GetComponent<Character>(handle);
		if ((summon->Flags & esv::CharacterFlags::HasOwner) == esv::CharacterFlags::HasOwner) {
			summons.push_back(summon);
		}
	}

	return summons;
}

ObjectSet<FixedString> Character::GetSkillIds()
{
	ObjectSet<FixedString> skillIds;

	if (SkillManager != nullptr) {
		for (auto const& skill : SkillManager->Skills) {
			skillIds.push_back(skill.Key);
		}
	}

	return skillIds;
}

Skill* Character::GetSkillInfo(FixedString const& skillId)
{
	if (SkillManager != nullptr) {
		return SkillManager->Skills.try_get(skillId);
	}

	return nullptr;
}

std::optional<int> Character::GetCustomStatValue(FixedString const& statId)
{
	return esv::CustomStatHelpers::GetCharacterStat(Base.Entity, statId.GetString());
}

bool Character::SetCustomStatValue(FixedString const& statId, int value)
{
	return esv::CustomStatHelpers::SetCharacterStat(Base.Entity, statId.GetString(), value);
}

CharacterFlags Character::LuaGetFlags()
{
	return Flags;
}

void Character::LuaSetFlags(CharacterFlags flags)
{
	SetFlags((uint64_t)flags & ~(uint64_t)Flags);
	ClearFlags((uint64_t)Flags & ~(uint64_t)flags);
}

bool Character::LuaHasFlag(CharacterFlags flag)
{
	return (Flags & flag) == flag;
}

void Character::LuaSetFlag(CharacterFlags flag, bool set)
{
	if (set) {
		SetFlags((uint64_t)flag);
	} else {
		ClearFlags((uint64_t)flag);
	}
}

UserReturn Character::LuaGetUserVariables(lua_State* L)
{
	lua::UserVariableHolderMetatable::Make(L, Base.Entity);
	return 1;
}

END_NS()
