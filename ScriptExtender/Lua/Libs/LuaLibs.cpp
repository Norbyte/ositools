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

#include <Lua/Libs/Utils.inl>
#include <Lua/Libs/Json.inl>
#include <Lua/Libs/Types.inl>
#include <Lua/Libs/Audio.inl>
#include <Lua/Libs/IO.inl>
#include <Lua/Libs/Mod.inl>
#include <Lua/Libs/Localization.inl>
#include <Lua/Libs/Debug.inl>
#include <Lua/Libs/Stats.inl>
#include <Lua/Libs/ClientUI.inl>
#include <Lua/Libs/ClientNet.inl>
#include <Lua/Libs/ClientVisual.inl>
#include <Lua/Libs/ServerOsiris.inl>
#include <Lua/Libs/ServerNet.inl>
#include <Lua/Libs/ServerCustomStats.inl>
#include <Lua/Libs/Surface.inl>
#include <Lua/Libs/ServerSurfaceAction.inl>
#include <Lua/Libs/ServerTemplate.inl>
#include <Lua/Libs/ClientTemplate.inl>
#include <Lua/Libs/AI.inl>
#include <Lua/Libs/Resource.inl>

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

	utils::RegisterUtilsLib(L);
	utils::RegisterJsonLib(L);
	types::RegisterTypesLib(L);
	io::RegisterIOLib(L);
	mod::RegisterModLib(L);
	loca::RegisterLocalizationLib(L);
	debug::RegisterDebugLib(L);
	stats::RegisterStatsLib(L);
	surface::RegisterSurfaceLib(L);
	resource::RegisterResourceLib(L);
}

END_NS()

BEGIN_NS(ecl::lua)

void RegisterClientLibraries(lua_State* L)
{
	audio::RegisterAudioLib(L);
	ui::RegisterUILib(L);
	net::RegisterNetLib(L);
	visual::RegisterVisualLib(L);
	tmpl::RegisterTemplateLib(L);
}

END_NS()

BEGIN_NS(esv::lua)

void RegisterServerLibraries(lua_State* L)
{
	osiris::RegisterOsirisLib(L);
	net::RegisterNetLib(L);
	stats::RegisterCustomStatLib(L);
	surface::RegisterSurfaceActionLib(L);
	tmpl::RegisterTemplateLib(L);
}

END_NS()
