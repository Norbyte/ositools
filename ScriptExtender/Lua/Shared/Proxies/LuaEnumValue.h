#pragma once

#include <span>
#include <Lua/Shared/LuaHelpers.h>
#include <Lua/Shared/LuaSerializers.h>
#include <Lua/Shared/LuaLifetime.h>
#include <Lua/Shared/Proxies/LuaUserdata.h>
#include <Lua/Shared/Proxies/LuaCppValue.h>

BEGIN_NS(lua)

class EnumValueMetatable : public LightCppValueMetatable<EnumValueMetatable>,
	public Indexable, public Stringifiable, public EqualityComparable
{
public:
	static constexpr MetatableTag MetaTag = MetatableTag::EnumValue;

	inline static void Make(lua_State* L, EnumUnderlyingType value, int registryIndex)
	{
		lua_push_cppvalue(L, MetaTag, registryIndex, static_cast<uint64_t>(value));
	}

	template <class T>
	inline static void Make(lua_State* L, T value)
	{
		using ei = EnumInfo<T>;
		Make(L, static_cast<typename ei::UnderlyingType>(value), ei::Storage.RegistryIndex);
	}

	static EnumInfoStore<EnumUnderlyingType>* GetEnumInfo(CppValueMetadata const& val);
	static FixedString GetLabel(CppValueMetadata const& self);
	static EnumUnderlyingType GetValue(CppValueMetadata const& self);

	static int Index(lua_State* L, CppValueMetadata& self);
	static int ToString(lua_State* L, CppValueMetadata& self);
	static bool IsEqual(lua_State* L, CppValueMetadata& self, int otherIndex);
	static char const* GetTypeName(lua_State* L, CppValueMetadata& self);
};

EnumUnderlyingType get_enum_value(lua_State* L, int index, EnumInfoStore<EnumUnderlyingType> const& store);
std::optional<EnumUnderlyingType> try_get_enum_value(lua_State* L, int index, EnumInfoStore<EnumUnderlyingType> const& store);
void push_enum_value(lua_State* L, EnumUnderlyingType value, EnumInfoStore<EnumUnderlyingType> const& store);

END_NS()
