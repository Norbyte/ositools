#include <Lua/Shared/LuaSerializers.h>

/// <lua_module>Types</lua_module>
BEGIN_NS(lua::types)

std::optional<STDString> GetUserdataObjectTypeName(lua_State * L, int index)
{
	auto object = Userdata<ObjectProxy2>::AsUserData(L, index);
	if (object) {
		return object->GetImpl()->GetTypeName().GetString();
	}

	auto event = Userdata<EventObject>::AsUserData(L, index);
	if (event) {
		return event->GetImpl()->GetType().TypeName.GetString();
	}

	auto map = Userdata<MapProxy>::AsUserData(L, index);
	if (map) {
		return STDString("Map<") + map->GetImpl()->GetKeyType().TypeName.GetString() + ", " + map->GetImpl()->GetValueType().TypeName.GetString() + ">";
	}

	return {};
}

std::optional<STDString> GetCppObjectTypeName(lua_State * L, int index)
{
	CppObjectMetadata meta;
	lua_get_cppobject(L, index, meta);

	switch (meta.MetatableTag) {
	case MetatableTag::ObjectProxyByRef:
	{
		auto propertyMap = gExtender->GetPropertyMapManager().GetPropertyMap(meta.PropertyMapTag);
		return propertyMap->Name.GetString();
	}

	case MetatableTag::ArrayProxy:
	{
		auto impl = gExtender->GetPropertyMapManager().GetArrayProxy(meta.PropertyMapTag);
		return impl->GetType().TypeName.GetString();
	}

	default:
		return {};
	}
}

UserReturn GetObjectType(lua_State * L)
{
	StackCheck _(L, 1);
	std::optional<STDString> type;
	switch (lua_type(L, 1)) {
	case LUA_TUSERDATA: type = GetUserdataObjectTypeName(L, 1); break;
	case LUA_TLIGHTCPPOBJECT:
	case LUA_TCPPOBJECT:
		type = GetCppObjectTypeName(L, 1); break;
	}

	push(L, type);
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
		types.push_back(type.Key);
	}

	return types;
}

void RegisterTypesLib()
{
	DECLARE_MODULE(Types, Both)
	BEGIN_MODULE()
	MODULE_FUNCTION(GetObjectType)
	MODULE_FUNCTION(GetTypeInfo)
	MODULE_FUNCTION(GetAllTypes)
	END_MODULE()
}

END_NS()
