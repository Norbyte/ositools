#include <Lua/Shared/LuaSerializers.h>

BEGIN_NS(lua::types)

std::optional<STDString> GetObjectType(lua_State * L, int index)
{
	if (lua_type(L, index) != LUA_TUSERDATA) {
		return {};
	}

	auto object = Userdata<ObjectProxy2>::AsUserData(L, index);
	if (object) {
		return object->GetImpl()->GetTypeName().GetString();
	}

	auto event = Userdata<EventObject>::AsUserData(L, index);
	if (event) {
		return event->GetImpl()->GetTypeName();
	}

	auto arr = Userdata<ArrayProxy>::AsUserData(L, index);
	if (arr) {
		return STDString("Array<") + arr->GetImpl()->GetTypeName() + ">";
	}

	auto set = Userdata<SetProxy>::AsUserData(L, index);
	if (set) {
		return STDString("Array<") + set->GetImpl()->GetTypeName() + ">";
	}

	auto map = Userdata<MapProxy>::AsUserData(L, index);
	if (map) {
		return STDString("Map<") + map->GetImpl()->GetKeyTypeName() + ", " + map->GetImpl()->GetValueTypeName() + ">";
	}

	return {};
}

int GetObjectType(lua_State * L)
{
	StackCheck _(L, 1);
	auto type = GetObjectType(L, 1);
	if (type) {
		push(L, *type);
	} else {
		push(L, nullptr);
	}

	return 1;
}

void RegisterTypesLib(lua_State* L)
{
	static const luaL_Reg jsonLib[] = {
		{"GetObjectType", GetObjectType},
		{0,0}
	};

	RegisterLib(L, "Types", jsonLib);
}

END_NS()
