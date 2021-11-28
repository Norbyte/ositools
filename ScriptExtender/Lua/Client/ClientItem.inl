BEGIN_NS(ecl)

using namespace lua;

void CollectInventoryItemGuids(ObjectSet<FixedString>& guids, ComponentHandle inventoryHandle);

ObjectSet<FixedString> Item::GetInventoryItemGuids()
{
	ObjectSet<FixedString> guids;
	CollectInventoryItemGuids(guids, InventoryHandle);
	return guids;
}

std::optional<FixedString> Item::GetOwnerCharacter()
{
	auto inventory = FindInventoryByHandle(InventoryParentHandle);

	for (;;) {
		if (inventory == nullptr) {
			break;
		}

		auto parent = inventory->ParentHandle;
		if (parent.GetType() == (uint32_t)ObjectType::ClientItem) {
			auto item = GetEntityWorld()->GetItem(parent);
			if (item) {
				inventory = FindInventoryByHandle(item->InventoryParentHandle);
			} else {
				break;
			}
		} else if (parent.GetType() == (uint32_t)ObjectType::ClientCharacter) {
			auto character = GetEntityWorld()->GetCharacter(parent);
			if (character) {
				return character->MyGuid;
			} else {
				break;
			}
		} else {
			break;
		}
	}

	return {};
}

ObjectSet<FixedString> Item::GetDeltaMods()
{
	if (Stats != nullptr) {
		return Stats->DeltaMods;
	} else {
		return {};
	}
}

END_NS()
