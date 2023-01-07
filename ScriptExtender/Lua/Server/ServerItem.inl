BEGIN_NS(esv)

using namespace lua;

void CollectInventoryItemGuids(ObjectSet<FixedString>& guids, ComponentHandle inventoryHandle);
void CollectInventoryItems(ObjectSet<Item*>& items, ComponentHandle inventoryHandle);

ObjectSet<FixedString> Item::GetInventoryItemGuids()
{
	ObjectSet<FixedString> guids;
	CollectInventoryItemGuids(guids, InventoryHandle);
	return guids;
}

ObjectSet<Item*> Item::GetInventoryItems()
{
	ObjectSet<Item*> items;
	CollectInventoryItems(items, InventoryHandle);
	return items;
}

// Move to IEoCServerObject?
ObjectSet<FixedString> Item::GetNearbyCharacters(float distance)
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

CompactObjectSet<FixedString> Item::GetGeneratedBoosts()
{
	if (Generation != nullptr) {
		return Generation->Boosts;
	} else {
		return {};
	}
}

void Item::LuaSetGeneratedBoosts(lua_State* L)
{
	CompactObjectSet<FixedString> boosts;
	LuaRead(L, boosts);

	if (Generation != nullptr) {
		Generation->Boosts = boosts;
	}
}

ObjectSet<FixedString> Item::GetDeltaMods()
{
	if (Stats != nullptr) {
		return Stats->DeltaMods;
	} else {
		return {};
	}
}

void Item::LuaSetDeltaMods(lua_State* L)
{
	ObjectSet<FixedString> boosts;
	LuaRead(L, boosts);

	if (Stats != nullptr) {
		Stats->DeltaMods = boosts;
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
			WarnDeprecated56("Getting stats properties through an esv::Item instance is deprecated! (Use item.Stats instead)");
			return PropertyOperationResult::Success;
		}
	} else if (map.HasProperty(prop)) {
		WarnDeprecated56("Getting stats properties through an esv::Item instance is deprecated! (Use item.Stats instead)");
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

END_NS()
