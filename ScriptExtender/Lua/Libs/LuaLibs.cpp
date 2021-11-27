#include <stdafx.h>
#include <Lua/Shared/LuaBinding.h>
#include <Lua/Shared/LuaSerializers.h>

#include <Lua/Shared/Proxies/LuaArrayProxy.inl>
// FIXME - disabled until entity system is upgrated
// #include <Lua/Shared/Proxies/LuaEntityProxy.inl>
#include <Lua/Shared/Proxies/LuaEvent.inl>
#include <Lua/Shared/Proxies/LuaMapProxy.inl>
#include <Lua/Shared/Proxies/LuaObjectProxy.inl>
#include <Lua/Shared/Proxies/LuaSetProxy.inl>

#include <Lua/Libs/LuaJson.inl>
#include <Lua/Libs/LuaTypes.inl>

BEGIN_NS(lua)

void InitObjectProxyPropertyMaps();

void RegisterSharedLibraries(lua_State* L)
{
	Userdata<ArrayProxy>::RegisterMetatable(L);
	Userdata<MapProxy>::RegisterMetatable(L);
	Userdata<ObjectProxy2>::RegisterMetatable(L);
	Userdata<SetProxy>::RegisterMetatable(L);
	Userdata<EventObject>::RegisterMetatable(L);
	InitObjectProxyPropertyMaps();

	// RegisterEntityProxy(L);
	utils::RegisterJsonLib(L);
	types::RegisterTypesLib(L);
}

END_NS()
