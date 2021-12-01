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
		characterGuids.Add(ch->MyGuid);
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

bool Item::LuaFallbackGet(lua_State* L, lua::LifetimeHolder const& lifetime, Item* object, FixedString const& prop)
{
	if (object->Stats) {
		auto const& map = StaticLuaPropertyMap<stats::Item>::PropertyMap;
		if (map.GetProperty(L, lifetime, object->Stats, prop)) {
			WarnDeprecated56("Getting stats properties through an esv::Item instance is deprecated! (Use item.Stats instead)");
			return true;
		}
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
