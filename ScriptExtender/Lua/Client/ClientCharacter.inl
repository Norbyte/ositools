BEGIN_NS(ecl)

using namespace lua;

void CollectInventoryItemGuids(ObjectSet<FixedString>& guids, ComponentHandle inventoryHandle)
{
	auto inventory = FindInventoryByHandle(inventoryHandle);
	if (inventory != nullptr) {
		int32_t index = 1;
		for (auto itemHandle : inventory->ItemsBySlot) {
			if (itemHandle) {
				auto item = GetEntityWorld()->GetComponent<Item>(itemHandle);
				if (item != nullptr) {
					guids.push_back(item->MyGuid);
				}
			}
		}
	}
}

Item* InventoryGetItemBySlot(ComponentHandle inventoryHandle, uint32_t slot)
{
	auto inventory = FindInventoryByHandle(inventoryHandle);
	if (inventory != nullptr && slot < inventory->ItemsBySlot.size()) {
		auto itemHandle = inventory->ItemsBySlot[slot];
		if (itemHandle) {
			return GetEntityWorld()->GetComponent<Item>(itemHandle);
		}
	}

	return nullptr;
}

ObjectSet<FixedString> Character::GetInventoryItemGuids()
{
	ObjectSet<FixedString> guids;
	CollectInventoryItemGuids(guids, InventoryHandle);
	return guids;
}

Item* Character::GetItemBySlot(stats::ItemSlot32 slot)
{
	return InventoryGetItemBySlot(InventoryHandle, (uint32_t)slot);
}

std::optional<FixedString> Character::GetItemGuidBySlot(stats::ItemSlot32 slot)
{
	auto item = GetItemBySlot(slot);
	if (item) {
		return item->MyGuid;
	} else {
		return {};
	}
}

std::optional<int> Character::GetCustomStatValue(FixedString const& statId)
{
	return CustomStatHelpers::GetCharacterStat(Base.Entity, statId);
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

END_NS()
