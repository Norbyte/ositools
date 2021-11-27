
template <class TObject>
int GameObjectSetScale(lua_State* L)
{
	auto self = get<ObjectProxy<TObject>*>(L, 1);
	auto character = self->Get(L);
	if (!character) {
		return 0;
	}

	auto scale = get<float>(L, 2);
	// Weird things happen if scale is too large/small
	if (scale < 0.01f || scale > 100.0f) {
		OsiError("Scale must be between 0.01 and 100")
	} else {
		character->SetScale(scale);
	}

	return 0;
}

template <class TObject>
int GameObjectGetDisplayName(lua_State* L, TObject* object)
{
	TranslatedString name;
	object->GetDisplayName(&name);
	push(L, name.Handle.ReferenceString);
	return 1;
}

template <class TObject>
int ItemGetDeltaMods(lua_State* L)
{
	auto self = get<ObjectProxy<TObject>*>(L, 1);
	auto item = self->Get(L);
	if (!item) {
		return 0;
	}

	lua_newtable(L);
	int32_t index{ 1 };
	if (item->Stats != nullptr) {
		for (auto const& boost : item->Stats->BoostNameSet) {
			settable(L, index++, boost);
		}
	}

	return 1;
}

template <class TObject>
int ItemSetDeltaMods(lua_State* L)
{
	StackCheck _(L, 0);
	auto self = get<ObjectProxy<TObject>*>(L, 1);
	auto item = self->Get(L);
	if (!item) return 0;

	ObjectSet<FixedString> deltaMods;
	LuaRead(L, deltaMods);

	if (item->Stats != nullptr) {
		item->Stats->BoostNameSet = deltaMods;
	}

	return 0;
}

