BEGIN_NS(lua)


void GetInventoryItems(lua_State* L, ComponentHandle inventoryHandle)
{
	lua_newtable(L);

	auto inventory = esv::FindInventoryByHandle(inventoryHandle);
	if (inventory != nullptr) {
		int32_t index = 1;
		for (auto itemHandle : inventory->ItemsBySlot) {
			if (itemHandle) {
				auto item = esv::GetEntityWorld()->GetItem(itemHandle);
				if (item != nullptr) {
					settable(L, index++, item->MyGuid);
				}
			}
		}
	}
}

char const* const ObjectProxy<esv::Character>::MetatableName = "esv::Character";

int ServerCharacterFetchProperty(lua_State* L, esv::Character* character, FixedString const& prop)
{
	if (prop == GFS.strPlayerCustomData) {
		if (character->PlayerData != nullptr
			&& character->PlayerData->CustomData.Initialized) {
			ComponentHandle handle;
			character->GetObjectHandle(handle);
			ObjectProxy<esv::PlayerCustomData>::New(L, handle);
			return 1;
		}
		else {
			OsiError("Character has no player data, or custom data was not initialized.");
			push(L, nullptr);
			return 1;
		}
	}

	if (prop == GFS.strStats) {
		if (character->Stats != nullptr) {
			ComponentHandle handle;
			character->GetObjectHandle(handle);
			ObjectProxy<CDivinityStats_Character>::New(L, handle);
			return 1;
		}
		else {
			OsiError("Character has no stats.");
			push(L, nullptr);
			return 1;
		}
	}

	if (prop == GFS.strHandle) {
		push(L, character->Base.Component.Handle);
		return 1;
	}

	if (prop == GFS.strRootTemplate) {
		ObjectProxy<CharacterTemplate>::New(L, GetServerLifetime(), character->CurrentTemplate);
		return 1;
	}

	if (prop == GFS.strDisplayName) {
		return GameObjectGetDisplayName<esv::Character>(L, character);
	}

	auto fetched = LuaPropertyMapGet(L, gCharacterPropertyMap, character, prop, true);
	if (!fetched) push(L, nullptr);
	return 1;
}

esv::Character* ObjectProxy<esv::Character>::GetPtr(lua_State* L)
{
	if (obj_) return obj_;
	auto character = esv::GetEntityWorld()->GetCharacter(handle_);
	if (character == nullptr) luaL_error(L, "Character handle invalid");
	return character;
}

int CharacterGetInventoryItems(lua_State* L)
{
	StackCheck _(L, 1);
	auto self = get<ObjectProxy<esv::Character>*>(L, 1);

	GetInventoryItems(L, self->Get(L)->InventoryHandle);

	return 1;
}

int CharacterGetNearbyCharacters(lua_State* L)
{
	StackCheck _(L, 1);
	auto self = get<ObjectProxy<esv::Character>*>(L, 1);
	auto pos = self->Get(L)->WorldPos;
	auto distance = get<float>(L, 2);

	esv::lua::GetCharactersGeneric(L, FixedString{}, [pos, distance](esv::Character* c) {
		return abs(glm::length(pos - c->WorldPos)) < distance;
		});
	return 1;
}

int CharacterGetSummons(lua_State* L)
{
	StackCheck _(L, 1);
	auto self = get<ObjectProxy<esv::Character>*>(L, 1);

	lua_newtable(L);
	int32_t index{ 1 };

	for (auto const& handle : self->Get(L)->SummonHandles) {
		auto summon = esv::GetEntityWorld()->GetCharacter(handle);
		if ((summon->Flags & esv::CharacterFlags::HasOwner) == esv::CharacterFlags::HasOwner) {
			settable(L, index++, summon->MyGuid);;
		}
	}

	return 1;
}

int CharacterGetSkills(lua_State* L)
{
	StackCheck _(L, 1);
	auto self = get<ObjectProxy<esv::Character>*>(L, 1);

	lua_newtable(L);
	int32_t index{ 1 };

	auto skillMgr = self->Get(L)->SkillManager;
	if (skillMgr != nullptr) {
		for (auto const& skill : skillMgr->Skills) {
			settable(L, index++, skill.Key);
		}
	}

	return 1;
}

int CharacterGetSkillInfo(lua_State* L)
{
	StackCheck _(L, 1);
	auto self = get<ObjectProxy<esv::Character>*>(L, 1);
	auto skillId = get<FixedString>(L, 2);

	auto skillMgr = self->Get(L)->SkillManager;
	if (skillMgr != nullptr) {
		auto skill = skillMgr->Skills.Find(skillId);
		if (skill != nullptr) {
			auto& sk = **skill;
			lua_newtable(L);
			setfield(L, "ActiveCooldown", sk.ActiveCooldown);
			setfield(L, "IsActivated", sk.IsActivated);
			setfield(L, "IsLearned", sk.IsLearned);
			setfield(L, "ZeroMemory", sk.ZeroMemory);
			setfield(L, "OncePerCombat", sk.OncePerCombat);
			setfield(L, "NumCharges", sk.NumCharges);

			lua_newtable(L);
			int32_t causeIdx{ 1 };
			for (auto const& handle : sk.CauseList) {
				settable(L, causeIdx++, handle);
			}

			lua_setfield(L, -2, "CauseList");
			return 1;
		}
	}

	push(L, nullptr);
	return 1;
}

int CharacterGetCustomStatValue(lua_State* L)
{
	StackCheck _(L, 1);
	auto self = get<ObjectProxy<esv::Character>*>(L, 1);
	auto statId = get<char const*>(L, 2);

	auto character = self->Get(L);
	if (character == nullptr) {
		push(L, nullptr);
		return 1;
	}

	auto value = esv::CustomStatHelpers::GetCharacterStat(character->Base.EntityObjectHandle, statId);
	if (value) {
		push(L, *value);
	}
	else {
		push(L, nullptr);
	}

	return 1;
}

int CharacterSetCustomStatValue(lua_State* L)
{
	StackCheck _(L, 1);
	auto self = get<ObjectProxy<esv::Character>*>(L, 1);
	auto statId = get<char const*>(L, 2);
	auto value = get<int>(L, 3);

	auto character = self->Get(L);
	if (character == nullptr) {
		push(L, false);
		return 1;
	}

	auto set = esv::CustomStatHelpers::SetCharacterStat(character->Base.EntityObjectHandle, statId, value);
	push(L, set);
	return 1;
}

int ObjectProxy<esv::Character>::Index(lua_State* L)
{
	auto character = Get(L);
	if (!character) return 0;

	StackCheck _(L, 1);
	auto propFS = get<FixedString>(L, 2);

	if (propFS == GFS.strGetInventoryItems) {
		lua_pushcfunction(L, &CharacterGetInventoryItems);
		return 1;
	}

	if (propFS == GFS.strGetSkills) {
		lua_pushcfunction(L, &CharacterGetSkills);
		return 1;
	}

	if (propFS == GFS.strGetSkillInfo) {
		lua_pushcfunction(L, &CharacterGetSkillInfo);
		return 1;
	}

	if (propFS == GFS.strGetNearbyCharacters) {
		lua_pushcfunction(L, &CharacterGetNearbyCharacters);
		return 1;
	}

	if (propFS == GFS.strGetSummons) {
		lua_pushcfunction(L, &CharacterGetSummons);
		return 1;
	}

	if (propFS == GFS.strHasTag) {
		lua_pushcfunction(L, &GameObjectHasTag<esv::Character>);
		return 1;
	}

	if (propFS == GFS.strGetTags) {
		lua_pushcfunction(L, &GameObjectGetTags<esv::Character>);
		return 1;
	}

	if (propFS == GFS.strGetStatus) {
		lua_pushcfunction(L, (&GameObjectGetStatus<esv::Character, esv::Status>));
		return 1;
	}

	if (propFS == GFS.strGetStatusByType) {
		lua_pushcfunction(L, (&GameObjectGetStatusByType<esv::Character, esv::Status>));
		return 1;
	}

	if (propFS == GFS.strGetStatuses) {
		lua_pushcfunction(L, (&GameObjectGetStatuses<esv::Character>));
		return 1;
	}

	if (propFS == GFS.strGetStatusObjects) {
		lua_pushcfunction(L, (&GameObjectGetStatusObjects<esv::Character, esv::Status>));
		return 1;
	}

	if (propFS == GFS.strSetScale) {
		lua_pushcfunction(L, (&GameObjectSetScale<esv::Character>));
		return 1;
	}

	if (propFS == GFS.strGetCustomStat) {
		lua_pushcfunction(L, &CharacterGetCustomStatValue);
		return 1;
	}

	if (propFS == GFS.strSetCustomStat) {
		lua_pushcfunction(L, &CharacterSetCustomStatValue);
		return 1;
	}

	return ServerCharacterFetchProperty(L, character, propFS);
}

END_NS()
