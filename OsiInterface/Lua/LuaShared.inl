
template <class TCharacter, class TStatus>
int CharacterGetStatus(lua_State* L)
{
	auto self = checked_get<ObjectProxy<TCharacter>*>(L, 1);
	auto statusId = luaL_checkstring(L, 2);

	auto character = self->Get(L);
	auto statusIdFs = ToFixedString(statusId);

	if (!character || !character->StatusMachine || !statusIdFs) {
		return 0;
	}

	auto status = character->StatusMachine->GetStatus(statusIdFs);
	if (status) {
		// FIXME - use handle based proxy
		ObjectProxy<TStatus>::New(L, status);
		return 1;
	}
	else {
		return 0;
	}
}

template <class TCharacter, class TStatus>
int CharacterGetStatusByType(lua_State* L)
{
	auto self = checked_get<ObjectProxy<TCharacter>*>(L, 1);
	auto statusType = luaL_checkstring(L, 2);

	auto character = self->Get(L);
	if (!character || !character->StatusMachine) {
		return 0;
	}

	auto typeId = EnumInfo<StatusType>::Find(statusType);
	if (!typeId) {
		OsiError("Status type unknown: " << statusType);
		return 0;
	}

	for (auto status : character->StatusMachine->Statuses) {
		if (status->GetStatusId() == *typeId) {
			// FIXME - use handle based proxy
			ObjectProxy<TStatus>::New(L, status);
			return 1;
		}
	}

	return 0;
}

template <class TCharacter>
int CharacterGetStatuses(lua_State* L)
{
	auto self = checked_get<ObjectProxy<TCharacter>*>(L, 1);
	auto character = self->Get(L);
	if (!character || !character->StatusMachine) {
		return 0;
	}

	lua_newtable(L);

	int32_t index = 1;
	for (auto status : character->StatusMachine->Statuses) {
		settable(L, index++, status->StatusId);
	}

	return 1;
}

template <class TCharacter>
int CharacterHasTag(lua_State* L)
{
	auto self = checked_get<ObjectProxy<TCharacter>*>(L, 1);
	auto tag = luaL_checkstring(L, 2);

	auto character = self->Get(L);
	auto tagFs = ToFixedString(tag);

	if (!character || !tagFs) {
		push(L, false);
	}
	else {
		push(L, character->IsTagged(tagFs));
	}

	return 1;
}

template <class TCharacter>
int CharacterGetTags(lua_State* L)
{
	auto self = checked_get<ObjectProxy<TCharacter>*>(L, 1);
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

