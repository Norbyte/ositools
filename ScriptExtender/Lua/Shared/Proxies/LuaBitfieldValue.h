#pragma once

#include <span>
#include <Lua/Shared/LuaHelpers.h>
#include <Lua/Shared/LuaSerializers.h>
#include <Lua/Shared/LuaLifetime.h>
#include <Lua/Shared/Proxies/LuaUserdata.h>
#include <Lua/Shared/Proxies/LuaCppValue.h>

BEGIN_NS(lua)

class BitfieldValueMetatable : public LightCppValueMetatable<BitfieldValueMetatable>,
	public Indexable, public Stringifiable, public EqualityComparable, public HasBinaryOps
{
public:
	static constexpr MetatableTag MetaTag = MetatableTag::BitfieldValue;

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

	static BitmaskInfoStore<EnumUnderlyingType>* GetBitfieldInfo(CppValueMetadata const& val);
	static EnumUnderlyingType GetValue(CppValueMetadata const& self);
	static Json::Value ToJson(CppValueMetadata& self);

	static int Index(lua_State* L, CppValueMetadata& self);
	static int ToString(lua_State* L, CppValueMetadata& self);
	static bool IsEqual(lua_State* L, CppValueMetadata& self, int otherIndex);
	static int BAnd(lua_State* L, CppValueMetadata& self, int otherIndex);
	static int BOr(lua_State* L, CppValueMetadata& self, int otherIndex);
	static int BXor(lua_State* L, CppValueMetadata& self, int otherIndex);
	static int BNot(lua_State* L, CppValueMetadata& self);
	static char const* GetTypeName(lua_State* L, CppValueMetadata& self);
};

EnumUnderlyingType get_bitfield_value(lua_State* L, int index, BitmaskInfoStore<EnumUnderlyingType> const& store, bool maskInvalidBits = false);
std::optional<EnumUnderlyingType> try_get_bitfield_value(lua_State* L, int index, BitmaskInfoStore<EnumUnderlyingType> const& store, bool maskInvalidBits = false);
void push_bitfield_value(lua_State* L, EnumUnderlyingType value, BitmaskInfoStore<EnumUnderlyingType> const& store);

END_NS()
