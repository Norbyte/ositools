#include <Lua/Shared/LuaSerializers.h>

/// <lua_module>Types</lua_module>
BEGIN_NS(lua::types)

std::optional<STDString> GetUserdataObjectTypeName(lua_State * L, int index)
{
	auto object = Userdata<ObjectProxy2>::AsUserData(L, index);
	if (object) {
		return object->GetImpl()->GetTypeName().GetString();
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
		return impl->GetArrayType().TypeName.GetString();
	}

	case MetatableTag::MapProxy:
	{
		auto impl = gExtender->GetPropertyMapManager().GetMapProxy(meta.PropertyMapTag);
		return impl->GetArrayType().TypeName.GetString();
	}

	case MetatableTag::EnumValue:
	{
		CppValueMetadata val;
		lua_get_cppvalue(L, index, val);
		return EnumValueMetatable::GetTypeName(L, val);
	}

	case MetatableTag::BitfieldValue:
	{
		CppValueMetadata val;
		lua_get_cppvalue(L, index, val);
		return BitfieldValueMetatable::GetTypeName(L, val);
	}

	case MetatableTag::UserVariableHolder:
	{
		CppValueMetadata val;
		lua_get_cppvalue(L, index, val);
		return UserVariableHolderMetatable::GetTypeName(L, val);
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

void RegisterEnumeration(lua_State* L, EnumInfoStore<EnumUnderlyingType> const& ty)
{
	lua_newtable(L);

	for (auto const& elem : ty.Values) {
		EnumValueMetatable::Make(L, elem.Value, ty.RegistryIndex);
		lua_setfield(L, -2, elem.Key.GetString());

		push(L, elem.Value);
		EnumValueMetatable::Make(L, elem.Value, ty.RegistryIndex);
		lua_settable(L, -3);
	}

	lua_setfield(L, -2, ty.LuaName.GetString());
}

void RegisterEnumeration(lua_State* L, BitmaskInfoStore<EnumUnderlyingType> const& ty)
{
	lua_newtable(L);

	for (auto const& elem : ty.Values) {
		BitfieldValueMetatable::Make(L, elem.Value, ty.RegistryIndex);
		lua_setfield(L, -2, elem.Key.GetString());

		push(L, elem.Value);
		BitfieldValueMetatable::Make(L, elem.Value, ty.RegistryIndex);
		lua_settable(L, -3);
	}

	lua_setfield(L, -2, ty.LuaName.GetString());
}

void RegisterEnumerations(lua_State* L)
{
	StackCheck _(L);
	lua_getglobal(L, "Ext");
	lua_newtable(L);

	for (auto const& ty : EnumRegistry::Get().EnumsById) {
		RegisterEnumeration(L, *ty);
	}
	
	for (auto const& ty : BitmaskRegistry::Get().BitfieldsById) {
		RegisterEnumeration(L, *ty);
	}

	lua_setfield(L, -2, "Enums");
	lua_pop(L, 1);
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
