#include <stdafx.h>
#include <Lua/Libs/LibraryRegistrationHelpers.h>

#include <Lua/Libs/Utils.inl>
#include <Lua/Libs/Json.inl>
#include <Lua/Libs/Types.inl>
#include <Lua/Libs/Audio.inl>
#include <Lua/Libs/IO.inl>
#include <Lua/Libs/Math.inl>
#include <Lua/Libs/Mod.inl>
#include <Lua/Libs/Localization.inl>
#include <Lua/Libs/Debug.inl>
#include <Lua/Libs/Stats.inl>
#include <Lua/Libs/Client.inl>
#include <Lua/Libs/ClientUI.inl>
#include <Lua/Libs/ClientNet.inl>
#include <Lua/Libs/ClientVisual.inl>
#include <Lua/Libs/ClientEntity.inl>
#include <Lua/Libs/ClientInput.inl>
#include <Lua/Libs/Server.inl>
#include <Lua/Libs/ServerAi.inl>
#include <Lua/Libs/ServerOsiris.inl>
#include <Lua/Libs/ServerNet.inl>
#include <Lua/Libs/ServerCustomStats.inl>
#include <Lua/Libs/ServerEntity.inl>
#include <Lua/Libs/ServerEffect.inl>
#include <Lua/Libs/Surface.inl>
#include <Lua/Libs/ServerSurfaceAction.inl>
#include <Lua/Libs/ServerPropertyLists.inl>
#include <Lua/Libs/ServerTemplate.inl>
#include <Lua/Libs/ClientTemplate.inl>
#include <Lua/Libs/AI.inl>
#include <Lua/Libs/Resource.inl>

BEGIN_NS(ecl::lua)

void RegisterClientLibraries()
{
	client::RegisterClientLib();
	audio::RegisterAudioLib();
	ui::RegisterUILib();
	net::RegisterNetLib();
	visual::RegisterVisualLib();
	ecs::RegisterEntityLib();
	tmpl::RegisterTemplateLib();
	input::RegisterInputLib();
}

END_NS()

BEGIN_NS(esv::lua)

void RegisterServerLibraries()
{
	server::RegisterServerLib();
	ai::RegisterAiLib();
	osiris::RegisterOsirisLib();
	net::RegisterNetLib();
	stats::RegisterCustomStatLib();
	ecs::RegisterEntityLib();
	effect::RegisterEffectLib();
	surface::action::RegisterSurfaceActionLib();
	tmpl::RegisterTemplateLib();
	proplist::RegisterPropertyListLib();
}

END_NS()

BEGIN_NS(lua)

void InitObjectProxyPropertyMaps();

void RegisterSharedMetatables(lua_State* L)
{
	Userdata<ObjectProxy2>::RegisterMetatable(L);
	LightObjectProxyByRefMetatable::RegisterMetatable(L);
	ArrayProxyMetatable::RegisterMetatable(L);
	MapProxyMetatable::RegisterMetatable(L);
	InitObjectProxyPropertyMaps();
	RegisterEntityProxy(L);
	StatsExtraDataProxy::RegisterMetatable(L);
}

void RegisterSharedLibraries()
{
	utils::RegisterUtilsLib();
	json::RegisterJsonLib();
	types::RegisterTypesLib();
	io::RegisterIOLib();
	math::RegisterMathLib();
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
