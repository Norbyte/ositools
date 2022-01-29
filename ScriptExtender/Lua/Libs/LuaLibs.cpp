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

BEGIN_SE()

template <class Fun>
TypeInformation DoConstructFunctionSignature(Fun f)
{
	TypeInformation sig;
	ConstructFunctionSignature(sig, f);
	return sig;
}

END_SE()

#define DECLARE_MODULE(name, role) { \
	ModuleDefinition mod; \
	mod.Role = ModuleRole::role; \
	mod.Table = FixedString{#name};

#define DECLARE_SUBMODULE(name, sub, role) { \
	ModuleDefinition mod; \
	mod.Role = ModuleRole::role; \
	mod.Table = FixedString{#name}; \
	mod.SubTable = FixedString{#sub};

#define BEGIN_MODULE() mod.Functions = {
#define MODULE_FUNCTION(fun) { FixedString{#fun}, LuaWrapFunction(&fun), DoConstructFunctionSignature(&fun) },
#define MODULE_NAMED_FUNCTION(name, fun){ FixedString{name}, LuaWrapFunction(&fun), DoConstructFunctionSignature(&fun) },
#define END_MODULE() }; \
	gModuleRegistry.RegisterModule(mod); \
}

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
#include <Lua/Libs/ClientEntity.inl>
#include <Lua/Libs/ServerOsiris.inl>
#include <Lua/Libs/ServerNet.inl>
#include <Lua/Libs/ServerCustomStats.inl>
#include <Lua/Libs/ServerEntity.inl>
#include <Lua/Libs/Surface.inl>
#include <Lua/Libs/ServerSurfaceAction.inl>
#include <Lua/Libs/ServerTemplate.inl>
#include <Lua/Libs/ClientTemplate.inl>
#include <Lua/Libs/AI.inl>
#include <Lua/Libs/Resource.inl>

BEGIN_NS(ecl::lua)

void RegisterClientLibraries()
{
	audio::RegisterAudioLib();
	ui::RegisterUILib();
	net::RegisterNetLib();
	visual::RegisterVisualLib();
	ecs::RegisterEntityLib();
	tmpl::RegisterTemplateLib();
}

END_NS()

BEGIN_NS(esv::lua)

void RegisterServerLibraries()
{
	osiris::RegisterOsirisLib();
	net::RegisterNetLib();
	stats::RegisterCustomStatLib();
	ecs::RegisterEntityLib();
	surface::action::RegisterSurfaceActionLib();
	tmpl::RegisterTemplateLib();
}

END_NS()

BEGIN_NS(lua)

void InitObjectProxyPropertyMaps();

void RegisterSharedMetatables(lua_State* L)
{
	Userdata<ArrayProxy>::RegisterMetatable(L);
	Userdata<MapProxy>::RegisterMetatable(L);
	Userdata<ObjectProxy2>::RegisterMetatable(L);
	Userdata<SetProxy>::RegisterMetatable(L);
	Userdata<EventObject>::RegisterMetatable(L);
	InitObjectProxyPropertyMaps();
	// RegisterEntityProxy(L);
	StatsExtraDataProxy::RegisterMetatable(L);
}

void RegisterSharedLibraries()
{
	utils::RegisterUtilsLib();
	json::RegisterJsonLib();
	types::RegisterTypesLib();
	io::RegisterIOLib();
	mod::RegisterModLib();
	loca::RegisterLocalizationLib();
	debug::RegisterDebugLib();
	stats::RegisterStatsLib();
	surface::RegisterSurfaceLib();
	resource::RegisterResourceLib();
}

void RegisterLibraries()
{
	if (!gModuleRegistry.GetModules().empty()) return;

	RegisterSharedLibraries();
	esv::lua::RegisterServerLibraries();
	ecl::lua::RegisterClientLibraries();
}

END_NS()
