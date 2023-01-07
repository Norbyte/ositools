#pragma once

#include <Lua/Shared/LuaHelpers.h>
#include <Lua/Shared/LuaLifetime.h>
#include <Lua/Shared/Proxies/LuaUserdata.h>

BEGIN_NS(lua)

class UserVariableHolderMetatable : public LightCppValueMetatable<UserVariableHolderMetatable>,
	public Indexable, public NewIndexable, public Lengthable, public Iterable, public Stringifiable
{
public:
	static constexpr MetatableTag MetaTag = MetatableTag::UserVariableHolder;

	inline static void Make(lua_State* L, EntityHandle const& entity)
	{
		lua_push_cppvalue(L, MetaTag, 0, entity.Handle);
	}

	static int Index(lua_State* L, CppValueMetadata& self);
	static int NewIndex(lua_State* L, CppValueMetadata& self);
	static int Length(lua_State* L, CppValueMetadata& self);
	static int Next(lua_State* L, CppValueMetadata& self);
	static int ToString(lua_State* L, CppValueMetadata& self);
	static char const* GetTypeName(lua_State* L, CppValueMetadata& self);
};

END_NS()
