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

/*if (propFS == GFS.strHandle) {
	push(L, item->Base.Component.Handle);
	return 1;
}

if (propFS == GFS.strDisplayName) {
	return GameObjectGetDisplayName<esv::Item>(L, item);
}*/

END_NS()