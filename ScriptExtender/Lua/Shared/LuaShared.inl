
template <class TObject, class TStatus>
int GameObjectGetStatus(lua_State* L)
{
	auto self = get<ObjectProxy<TObject>*>(L, 1);
	auto statusId = get<FixedString>(L, 2);

	auto object = self->Get(L);

	if (!object || !object->StatusMachine) {
		return 0;
	}

	auto status = object->StatusMachine->GetStatus(statusId);
	if (status) {
		// FIXME - use handle based proxy
		ObjectProxy<TStatus>::New(L, GetCurrentLifetime(), status);
		return 1;
	}
	else {
		return 0;
	}
}

template <class TObject, class TStatus>
int GameObjectGetStatusByType(lua_State* L)
{
	auto self = get<ObjectProxy<TObject>*>(L, 1);
	auto statusType = get<StatusType>(L, 2);

	auto object = self->Get(L);
	if (!object || !object->StatusMachine) {
		return 0;
	}

	for (auto status : object->StatusMachine->Statuses) {
		if (status->GetStatusId() == statusType) {
			// FIXME - use handle based proxy
			ObjectProxy<TStatus>::New(L, GetCurrentLifetime(), status);
			return 1;
		}
	}

	return 0;
}

template <class TObject>
int GameObjectGetStatuses(lua_State* L)
{
	auto self = get<ObjectProxy<TObject>*>(L, 1);
	auto object = self->Get(L);
	if (!object || !object->StatusMachine) {
		return 0;
	}

	lua_newtable(L);

	int32_t index = 1;
	for (auto status : object->StatusMachine->Statuses) {
		settable(L, index++, status->StatusId);
	}

	return 1;
}

template <class TObject, class TStatus>
int GameObjectGetStatusObjects(lua_State* L)
{
	auto self = get<ObjectProxy<TObject>*>(L, 1);
	auto object = self->Get(L);
	if (!object || !object->StatusMachine) {
		return 0;
	}

	lua_newtable(L);

	int32_t index = 1;
	for (auto status : object->StatusMachine->Statuses) {
		push(L, index++);
		// FIXME - use handle based proxy
		ObjectProxy<TStatus>::New(L, GetCurrentLifetime(), status);
		lua_settable(L, -3);
	}

	return 1;
}

template <class TObject>
int GameObjectHasTag(lua_State* L)
{
	auto self = get<ObjectProxy<TObject>*>(L, 1);
	auto tag = get<FixedString>(L, 2);

	auto object = self->Get(L);

	if (!object) {
		push(L, false);
	} else {
		push(L, object->IsTagged(tag));
	}

	return 1;
}

template <class TObject>
int GameObjectGetTags(lua_State* L)
{
	auto self = get<ObjectProxy<TObject>*>(L, 1);
	auto character = self->Get(L);
	if (!character) {
		return 0;
	}

	lua_newtable(L);

	int32_t index = 1;
	ObjectSet<FixedString> tags;
	character->GetTags(tags);

	for (auto const& tag : tags) {
		settable(L, index++, tag);
	}

	return 1;
}

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

