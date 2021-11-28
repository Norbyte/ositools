BEGIN_NS(ecl)

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

Item* InventoryGetItemBySlot(ComponentHandle inventoryHandle, uint32_t slot)
{
	auto inventory = ecl::FindInventoryByHandle(inventoryHandle);
	if (inventory != nullptr && slot < inventory->ItemsBySlot.Size) {
		auto itemHandle = inventory->ItemsBySlot[slot];
		if (itemHandle) {
			return ecl::GetEntityWorld()->GetItem(itemHandle);
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

std::optional<int> Character::GetCustomStatValue(FixedString const& statId)
{
	return CustomStatHelpers::GetCharacterStat(Base.EntityObjectHandle, statId);
}

END_NS()
