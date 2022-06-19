#include <Lua/Shared/LuaMethodHelpers.h>
#include <Extender/ScriptExtender.h>

BEGIN_NS(ecl::lua::ecs)

void MakeLegacyClientCharacterObjectRef(lua_State* L, ecl::Character* value)
{
	if (value) {
		ObjectProxy2::MakeHandle<ecl::Character>(L, value->Base.Component.Handle, State::FromLua(L)->GetCurrentLifetime());
	} else {
		push(L, nullptr);
	}
}

void MakeLegacyClientItemObjectRef(lua_State* L, ecl::Item* value)
{
	if (value) {
		ObjectProxy2::MakeHandle<ecl::Item>(L, value->Base.Component.Handle, State::FromLua(L)->GetCurrentLifetime());
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
		return GetEntityWorld()->GetComponent<ecl::Character>(handle);
	}

	case LUA_TNUMBER:
	{
		NetId netId{ (uint32_t)lua_tointeger(L, index) };
		return GetEntityWorld()->GetComponent<ecl::Character>(netId);
	}

	case LUA_TSTRING:
	{
		auto guid = lua_tostring(L, index);
		return GetEntityWorld()->GetComponent<ecl::Character>(guid);
	}

	default:
		OsiError("Expected character UUID, Handle or NetId");
		return nullptr;
	}
}

UserReturn GetCharacter(lua_State* L)
{
	ecl::Character* character = LuaGetCharacter(L, 1);
	MakeLegacyClientCharacterObjectRef(L, character);
	return 1;
}

Item* LuaGetItem(lua_State* L, int index)
{
	switch (lua_type(L, index)) {
	case LUA_TLIGHTUSERDATA:
	{
		auto handle = get<ComponentHandle>(L, index);
		return GetEntityWorld()->GetComponent<ecl::Item>(handle);
	}

	case LUA_TNUMBER:
	{
		NetId netId{ (uint32_t)lua_tointeger(L, index) };
		return GetEntityWorld()->GetComponent<ecl::Item>(netId);
	}

	case LUA_TSTRING:
	{
		auto guid = lua_tostring(L, index);
		return GetEntityWorld()->GetComponent<ecl::Item>(guid);
	}

	default:
		OsiError("Expected item UUID, Handle or NetId; got " << lua_typename(L, lua_type(L, index)));
		return nullptr;
	}
}

UserReturn GetItem(lua_State* L)
{
	ecl::Item* item = LuaGetItem(L, 1);
	MakeLegacyClientItemObjectRef(L, item);
	return 1;
}

Inventory* GetInventory(ComponentHandle handle)
{
	auto factory = *GetStaticSymbols().ecl__InventoryFactory;
	return factory->Get(handle);
}

Status* GetStatus(lua_State* L)
{
	auto character = LuaGetCharacter(L, 1);
	if (character == nullptr) return 0;

	if (lua_type(L, 2) == LUA_TLIGHTUSERDATA) {
		auto statusHandle = get<ComponentHandle>(L, 2);
		return character->GetStatus(statusHandle);
	} else {
		auto index = lua_tointeger(L, 2);
		NetId statusNetId{ (uint32_t)index };
		return character->GetStatus(statusNetId);
	}
}

IEoCClientObject* GetGameObject(lua_State* L)
{
	StackCheck _(L, 1);
	switch (lua_type(L, 1)) {
	case LUA_TLIGHTUSERDATA:
	{
		auto handle = get<ComponentHandle>(L, 1);
		return GetEntityWorld()->GetGameObject(handle);
	}

	case LUA_TSTRING:
	{
		auto guid = lua_tostring(L, 1);
		return GetEntityWorld()->GetGameObject(guid);
	}

	default:
		OsiError("Expected object GUID or handle, got " << lua_typename(L, lua_type(L, 1)));
		return nullptr;
	}
}

eoc::AiGrid* GetAiGrid()
{
	auto level = GetStaticSymbols().GetCurrentClientLevel();
	if (!level || !level->AiGrid) {
		OsiError("Current level not available yet!");
		return 0;
	}

	return level->AiGrid;
}

Level* GetCurrentLevel()
{
	return GetStaticSymbols().GetCurrentClientLevel();
}

ComponentHandle NullHandle()
{
	return ComponentHandle(ComponentHandle::NullHandle);
}

void RegisterEntityLib()
{
	DECLARE_MODULE(Entity, Client)
	BEGIN_MODULE()
	MODULE_FUNCTION(NullHandle)
	MODULE_FUNCTION(GetCharacter)
	MODULE_FUNCTION(GetItem)
	MODULE_FUNCTION(GetInventory)
	MODULE_FUNCTION(GetStatus)
	MODULE_FUNCTION(GetGameObject)
	MODULE_FUNCTION(GetAiGrid)
	MODULE_FUNCTION(GetCurrentLevel)
	END_MODULE()
}

END_NS()
