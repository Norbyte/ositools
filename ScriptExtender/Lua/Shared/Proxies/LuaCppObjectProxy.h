#pragma once

#include <Lua/Shared/LuaHelpers.h>
#include <Lua/Shared/LuaLifetime.h>
#include <Lua/Shared/Proxies/LuaCppClass.h>

BEGIN_NS(lua)

class LightObjectProxyByRefMetatable : public LightCppObjectMetatable<LightObjectProxyByRefMetatable>, 
	public Indexable, public NewIndexable, public Iterable, public Stringifiable, public EqualityComparable
{
public:
	static int Index(lua_State* L, CppObjectMetadata& self);
	static int NewIndex(lua_State* L, CppObjectMetadata& self);
	static int ToString(lua_State* L, CppObjectMetadata& self);
	static int IsEqual(lua_State* L, CppObjectMetadata& self, CppObjectMetadata& other);
	static int Next(lua_State* L, CppObjectMetadata& self);
	static char const* GetTypeName(lua_State* L, CppObjectMetadata& self);
};

END_NS()
