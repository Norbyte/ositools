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
		if (parent.GetType() == (uint32_t)ObjectHandleType::ClientItem) {
			auto item = GetEntityWorld()->GetComponent<Item>(parent);
			if (item) {
				inventory = FindInventoryByHandle(item->InventoryParentHandle);
			} else {
				break;
			}
		} else if (parent.GetType() == (uint32_t)ObjectHandleType::ClientCharacter) {
			auto character = GetEntityWorld()->GetComponent<Character>(parent);
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

bool Item::LuaFallbackGet(lua_State* L, lua::LifetimeHolder const& lifetime, Item* object, FixedString const& prop)
{
	auto const& map = StaticLuaPropertyMap<stats::Item>::PropertyMap;
	if (object->Stats) {
		if (map.GetProperty(L, lifetime, object->Stats, prop)) {
			WarnDeprecated56("Getting stats properties through an ecl::Item instance is deprecated! (Use item.Stats instead)");
			return true;
		}
	} else if (map.HasProperty(prop)) {
		WarnDeprecated56("Getting stats properties through an ecl::Item instance is deprecated! (Use item.Stats instead)");
		push(L, nullptr);
		return true;
	}

	return false;
}

bool Item::LuaFallbackSet(lua_State* L, lua::LifetimeHolder const& lifetime, Item* object, FixedString const& prop, int index)
{
	if (object->Stats) {
		auto const& map = StaticLuaPropertyMap<stats::Item>::PropertyMap;
		return map.SetProperty(L, lifetime, object->Stats, prop, index);
	} else {
		return false;
	}
}

END_NS()
