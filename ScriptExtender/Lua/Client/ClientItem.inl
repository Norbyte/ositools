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

ItemFlags Item::LuaGetFlags()
{
	return Flags;
}

void Item::LuaSetFlags(ItemFlags flags)
{
	SetFlags((uint64_t)flags & ~(uint64_t)Flags);
	ClearFlags((uint64_t)Flags & ~(uint64_t)flags);
}

bool Item::LuaHasFlag(ItemFlags flag)
{
	return (Flags & flag) == flag;
}

void Item::LuaSetFlag(ItemFlags flag, bool set)
{
	if (set) {
		SetFlags((uint64_t)flag);
	} else {
		ClearFlags((uint64_t)flag);
	}
}

UserReturn Item::LuaGetUserVariables(lua_State* L)
{
	lua::UserVariableHolderMetatable::Make(L, Base.Entity);
	return 1;
}

PropertyOperationResult Item::LuaFallbackGet(lua_State* L, lua::LifetimeHandle const& lifetime, Item* object, FixedString const& prop)
{
	auto const& map = StaticLuaPropertyMap<stats::Item>::PropertyMap;
	if (object->Stats) {
		if (map.GetProperty(L, lifetime, object->Stats, prop) == PropertyOperationResult::Success) {
			WarnDeprecated56("Getting stats properties through an ecl::Item instance is deprecated! (Use item.Stats instead)");
			return PropertyOperationResult::Success;
		}
	} else if (map.HasProperty(prop)) {
		WarnDeprecated56("Getting stats properties through an ecl::Item instance is deprecated! (Use item.Stats instead)");
		push(L, nullptr);
		return PropertyOperationResult::Success;
	}

	return PropertyOperationResult::NoSuchProperty;
}

PropertyOperationResult Item::LuaFallbackSet(lua_State* L, lua::LifetimeHandle const& lifetime, Item* object, FixedString const& prop, int index)
{
	if (object->Stats) {
		auto const& map = StaticLuaPropertyMap<stats::Item>::PropertyMap;
		return map.SetProperty(L, lifetime, object->Stats, prop, index);
	} else {
		return PropertyOperationResult::NoSuchProperty;
	}
}


SceneryFlags Scenery::LuaGetFlags()
{
	return Flags;
}

void Scenery::LuaSetFlags(SceneryFlags flags)
{
	SetFlags((uint64_t)flags & ~(uint64_t)Flags);
	ClearFlags((uint64_t)Flags & ~(uint64_t)flags);
}

bool Scenery::LuaHasFlag(SceneryFlags flag)
{
	return (Flags & flag) == flag;
}

void Scenery::LuaSetFlag(SceneryFlags flag, bool set)
{
	if (set) {
		SetFlags((uint64_t)flag);
	} else {
		ClearFlags((uint64_t)flag);
	}
}

END_NS()
