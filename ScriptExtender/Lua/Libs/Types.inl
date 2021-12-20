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
		return event->GetImpl()->GetType().TypeName.GetString();
	}

	auto arr = Userdata<ArrayProxy>::AsUserData(L, index);
	if (arr) {
		return STDString("Array<") + arr->GetImpl()->GetType().TypeName.GetString() + ">";
	}

	auto set = Userdata<SetProxy>::AsUserData(L, index);
	if (set) {
		return STDString("Array<") + set->GetImpl()->GetType().TypeName.GetString() + ">";
	}

	auto map = Userdata<MapProxy>::AsUserData(L, index);
	if (map) {
		return STDString("Map<") + map->GetImpl()->GetKeyType().TypeName.GetString() + ", " + map->GetImpl()->GetValueType().TypeName.GetString() + ">";
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

TypeInformation* GetTypeInfo(FixedString const& typeName)
{
	auto const& type = TypeInformationRepository::GetInstance().GetType(typeName);
	if (type.Kind == LuaTypeId::Unknown) {
		return nullptr;
	} else {
		return const_cast<TypeInformation*>(&type);
	}
}

ObjectSet<FixedString> GetAllTypes()
{
	ObjectSet<FixedString> types;
	auto const& allTypes = TypeInformationRepository::GetInstance().GetAllTypes();
	for (auto const& type : allTypes) {
		types.Add(type.Key);
	}

	return types;
}

void RegisterTypesLib(lua_State* L)
{
	static const luaL_Reg jsonLib[] = {
		{"GetObjectType", GetObjectType},
		{"GetTypeInfo", LuaWrapFunction(&GetTypeInfo)},
		{"GetAllTypes", LuaWrapFunction(&GetAllTypes)},
		{0,0}
	};

	RegisterLib(L, "Types", jsonLib);
}

END_NS()
