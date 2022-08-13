#include <GameDefinitions/Base/BaseUtilities.h>
#include <Lua/Shared/LuaMethodHelpers.h>
#include <Extender/ScriptExtender.h>

/// <lua_module>Entity</lua_module>
BEGIN_NS(esv::lua::ecs)

#include <Lua/Shared/LevelIteratorFunctions.inl>

void MakeLegacyServerCharacterObjectRef(lua_State* L, esv::Character* value)
{
	if (value) {
		ObjectProxy2::MakeHandle<esv::Character>(L, value->Base.Component.Handle, State::FromLua(L)->GetCurrentLifetime());
	} else {
		push(L, nullptr);
	}
}

Character* LuaGetCharacter(lua_State* L, int index)
{
	switch (lua_type(L, index)) {
	case LUA_TLIGHTUSERDATA:
	{
		auto handle = get<ComponentHandle>(L, index);
		return GetEntityWorld()->GetComponent<Character>(handle);
	}

	case LUA_TNUMBER:
	{
		NetId netId{ (uint32_t)lua_tointeger(L, index) };
		return GetEntityWorld()->GetComponent<Character>(netId);
	}

	case LUA_TSTRING:
	{
		auto guid = lua_tostring(L, index);
		return GetEntityWorld()->GetComponent<Character>(guid);
	}

	default:
		OsiError("Expected character UUID, Handle or NetId; got " << lua_typename(L, lua_type(L, index)));
		return nullptr;
	}
}

Character* GetCharacter(lua_State* L)
{
	auto lua = State::FromLua(L);
	if (lua->RestrictionFlags & State::RestrictHandleConversion) {
		luaL_error(L, "Attempted to resolve character handle in restricted context");
		return nullptr;
	}

	return LuaGetCharacter(L, 1);
}

UserReturn GetCharacterLegacy(lua_State* L)
{
	auto lua = State::FromLua(L);
	if (lua->RestrictionFlags & State::RestrictHandleConversion) {
		return luaL_error(L, "Attempted to resolve character handle in restricted context");
	}

	StackCheck _(L, 1);
	Character* character = LuaGetCharacter(L, 1);
	MakeLegacyServerCharacterObjectRef(L, character);
	return 1;
}

Item* LuaGetItem(lua_State* L, int index)
{
	switch (lua_type(L, index)) {
	case LUA_TLIGHTUSERDATA:
	{
		auto handle = get<ComponentHandle>(L, index);
		return GetEntityWorld()->GetComponent<Item>(handle);
	}

	case LUA_TNUMBER:
	{
		NetId netId{ (uint32_t)lua_tointeger(L, index) };
		return GetEntityWorld()->GetComponent<Item>(netId);
	}

	case LUA_TSTRING:
	{
		auto guid = lua_tostring(L, index);
		return GetEntityWorld()->GetComponent<Item>(guid);
	}

	default:
		OsiError("Expected item GUID or handle, got " << lua_typename(L, lua_type(L, index)));
		return nullptr;
	}
}

Item* GetItem(lua_State* L)
{
	auto lua = State::FromLua(L);
	if (lua->RestrictionFlags & State::RestrictHandleConversion) {
		luaL_error(L, "Attempted to resolve item handle in restricted context");
	}

	return LuaGetItem(L, 1);
}

Trigger* LuaGetTrigger(lua_State* L, int index)
{
	switch (lua_type(L, 1)) {
	case LUA_TLIGHTUSERDATA:
	{
		auto handle = get<ComponentHandle>(L, 1);
		return GetEntityWorld()->GetTrigger(handle);
	}

	case LUA_TSTRING:
	{
		auto guid = lua_tostring(L, 1);
		return GetEntityWorld()->GetTrigger(guid);
	}

	default:
		OsiError("Expected trigger GUID or handle, got " << lua_typename(L, lua_type(L, 1)));
		return nullptr;
	}
}

Trigger* GetTrigger(lua_State* L)
{
	auto lua = State::FromLua(L);
	if (lua->RestrictionFlags & State::RestrictHandleConversion) {
		luaL_error(L, "Attempted to resolve trigger handle in restricted context");
		return nullptr;
	}

	return LuaGetTrigger(L, 1);
}

IGameObject* LuaGetGameObject(lua_State* L, int idx)
{
	switch (lua_type(L, idx)) {
	case LUA_TLIGHTUSERDATA:
	{
		auto handle = get<ComponentHandle>(L, idx);
		return GetEntityWorld()->GetGameObject(handle);
	}

	case LUA_TSTRING:
	{
		auto guid = get<char const*>(L, idx);
		return GetEntityWorld()->GetGameObject(guid);
	}

	default:
		OsiError("Expected object GUID or handle, got " << lua_typename(L, lua_type(L, idx)));
		return nullptr;
	}
}

IGameObject* GetGameObject(lua_State* L)
{
	auto lua = State::FromLua(L);
	if (lua->RestrictionFlags & State::RestrictHandleConversion) {
		luaL_error(L, "Attempted to resolve game object handle in restricted context");
	}

	return LuaGetGameObject(L, 1);
}

Inventory* GetInventory(ComponentHandle handle)
{
	auto factory = *GetStaticSymbols().esv__InventoryFactory;
	return factory->Get(handle);
}

Status* GetStatus(lua_State* L)
{
	auto lua = State::FromLua(L);
	if (lua->RestrictionFlags & State::RestrictHandleConversion) {
		luaL_error(L, "Attempted to resolve status handle in restricted context");
	}

	auto gameObject = LuaGetGameObject(L, 1);
	if (gameObject == nullptr) return 0;
	auto statusMachine = static_cast<IEoCServerObject*>(gameObject)->GetStatusMachine();
	if (statusMachine == nullptr) return 0;

	Status* status;
	if (lua_type(L, 2) == LUA_TLIGHTUSERDATA) {
		auto statusHandle = get<ComponentHandle>(L, 2);
		status = statusMachine->GetServerStatus(statusHandle, true);
		if (!status) {
			OsiError("Game object has no status with handle 0x" << std::hex << statusHandle.Handle);
		}
	} else {
		auto index = lua_tointeger(L, 2);

		// We need to keep integer status handle support since some extender Osiris events
		// (eg. NRD_OnHit, NRD_OnPrepareHit, etc.) use these handles and Osiris doesn't support lightuserdata
		if (index > 0xffffffff) {
			ComponentHandle statusHandle{ index };
			status = statusMachine->GetServerStatus(statusHandle, true);
			if (!status) {
				OsiError("Game object has no status with handle 0x" << std::hex << statusHandle.Handle);
			}
		} else {
			NetId statusNetId{ (uint32_t)index };
			status = statusMachine->GetStatus(statusNetId);
			if (!status) {
				OsiError("Character has no status with NetId 0x" << std::hex << index);
			}
		}
	}

	return status;
}

TurnManager::Combat* GetCombat(lua_State* L, uint32_t combatId)
{
	auto lua = State::FromLua(L);
	if (lua->RestrictionFlags & State::RestrictHandleConversion) {
		luaL_error(L, "Attempted to resolve combat ID in restricted context");
		return nullptr;
	}

	auto turnMgr = GetEntityWorld()->GetTurnManager();
	auto combat = turnMgr->Combats.find((uint8_t)combatId);
	if (!combat) {
		OsiError("No combat found with ID " << combatId);
		return nullptr;
	}

	return &combat.Value();
}

Surface* GetSurface(lua_State* L, ComponentHandle handle)
{
	auto lua = State::FromLua(L);
	if (lua->RestrictionFlags & State::RestrictHandleConversion) {
		luaL_error(L, "Attempted to resolve item handle in restricted context");
	}

	auto level = GetStaticSymbols().GetCurrentServerLevel();
	if (!level || !level->SurfaceManager) {
		OsiError("Current level not available yet!");
		return nullptr;
	}

	return level->SurfaceManager->Get(handle);
}

eoc::AiGrid* GetAiGrid()
{
	auto level = GetStaticSymbols().GetCurrentServerLevel();
	if (!level || !level->AiGrid) {
		OsiError("Current level not available yet!");
		return 0;
	}

	return level->AiGrid;
}

UserReturn GetCurrentLevelData(lua_State* L)
{
	auto level = GetStaticSymbols().GetCurrentServerLevel();
	if (!level || !level->LevelDesc) {
		OsiError("Current level not available yet!");
		return 0;
	}

	lua_newtable(L);
	settable(L, "LevelName", level->LevelDesc->LevelName);
	settable(L, "UniqueKey", level->LevelDesc->UniqueKey);
	settable(L, "CustomDisplayLevelName", level->LevelDesc->CustomDisplayLevelName);
	settable(L, "Type", level->LevelDesc->Type);
	return 1;
}

Level* GetCurrentLevel()
{
	return GetStaticSymbols().GetCurrentServerLevel();
}

ObjectSet<FixedString> GetAllCharacterGuids(lua_State* L, std::optional<FixedString> levelName)
{
	ObjectSet<FixedString> guids;
	GetCharacterGuidsGeneric(guids, levelName.value_or(GFS.strEmpty), [](esv::Character*) { return true; });
	return guids;
}

ObjectSet<Character*> GetAllCharacters(std::optional<FixedString> levelName)
{
	ObjectSet<Character*> characters;
	GetCharactersGeneric(characters, levelName.value_or(GFS.strEmpty), [](esv::Character*) { return true; });
	return characters;
}

/// <summary>
/// Returns the UUID of all characters within a radius around the specified point.
/// </summary>
/// <lua_export>GetCharacterGuidsAroundPosition</lua_export>
/// <param name="x" type="number">Surface action type</param>
/// <param name="y" type="number">Surface action type</param>
/// <param name="z" type="number">Surface action type</param>
/// <param name="distance" type="number">Surface action type</param>
/// <returns>Array&lt;FixedString&gt;</returns>
ObjectSet<FixedString> GetCharacterGuidsAroundPosition(float x, float y, float z, float distance)
{
	glm::vec3 pos(x, y, z);

	ObjectSet<FixedString> guids;
	GetCharacterGuidsGeneric(guids, FixedString{}, [pos, distance](esv::Character* c) {
		return abs(glm::length(pos - c->WorldPos)) < distance;
	});
	return guids;
}

ObjectSet<Character*> GetCharactersAroundPosition(glm::vec3 pos, float distance)
{
	ObjectSet<Character*> characters;
	GetCharactersGeneric(characters, FixedString{}, [pos, distance](esv::Character* c) {
		return abs(glm::length(pos - c->WorldPos)) < distance;
	});
	return characters;
}

ObjectSet<FixedString> GetAllItemGuids(std::optional<FixedString> levelName)
{
	ObjectSet<FixedString> guids;
	GetItemGuidsGeneric(guids, levelName.value_or(GFS.strEmpty), [](esv::Item*) { return true; });
	return guids;
}

ObjectSet<Item*> GetAllItems(std::optional<FixedString> levelName)
{
	ObjectSet<Item*> items;
	GetItemsGeneric(items, levelName.value_or(GFS.strEmpty), [](esv::Item*) { return true; });
	return items;
}

ObjectSet<FixedString> GetItemGuidsAroundPosition(float x, float y, float z, float distance)
{
	glm::vec3 pos(x, y, z);

	ObjectSet<FixedString> guids;
	GetItemGuidsGeneric(guids, FixedString{}, [pos, distance](esv::Item* c) {
		return abs(glm::length(pos - c->WorldPos)) < distance;
	});
	return guids;
}

ObjectSet<Item*> GetItemsAroundPosition(glm::vec3 pos, float distance)
{
	ObjectSet<Item*> items;
	GetItemsGeneric(items, FixedString{}, [pos, distance](esv::Item* c) {
		return abs(glm::length(pos - c->WorldPos)) < distance;
	});
	return items;
}

ObjectSet<FixedString> GetAllTriggerGuids(std::optional<FixedString> levelName)
{
	ObjectSet<FixedString> guids;
	GetTriggerGuidsGeneric(guids, levelName.value_or(GFS.strEmpty), [](Trigger*) { return true; });
	return guids;
}

AlignmentContainer* GetAlignmentManager()
{
	return *GetStaticSymbols().esv__AlignmentContainer;
}

ComponentHandle NullHandle()
{
	return ComponentHandle(ComponentHandle::NullHandle);
}

void RegisterEntityLib()
{
	DECLARE_MODULE(Entity, Server)
	BEGIN_MODULE()
	MODULE_FUNCTION(NullHandle)
	MODULE_FUNCTION(GetCharacter)
	MODULE_FUNCTION(GetCharacterLegacy)
	MODULE_FUNCTION(GetItem)
	MODULE_FUNCTION(GetTrigger)
	MODULE_FUNCTION(GetGameObject)
	MODULE_FUNCTION(GetInventory)
	MODULE_FUNCTION(GetStatus)
	MODULE_FUNCTION(GetCombat)
	MODULE_FUNCTION(GetSurface)
	MODULE_FUNCTION(GetAiGrid)
	MODULE_FUNCTION(GetCurrentLevelData)
	MODULE_FUNCTION(GetCurrentLevel)
	MODULE_FUNCTION(GetAlignmentManager)

	// Level iterator functions
	// FIXME - need by-ref array return to return object ptrs
	//MODULE_FUNCTION(GetAllCharacters)
	MODULE_FUNCTION(GetAllCharacterGuids)
	//MODULE_FUNCTION(GetCharactersAroundPosition)
	MODULE_FUNCTION(GetCharacterGuidsAroundPosition)
	//MODULE_FUNCTION(GetAllItems)
	MODULE_FUNCTION(GetAllItemGuids)
	//MODULE_FUNCTION(GetItemsAroundPosition)
	MODULE_FUNCTION(GetItemGuidsAroundPosition)
	MODULE_FUNCTION(GetAllTriggerGuids)
	END_MODULE()
}

END_NS()
