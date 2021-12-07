#include <Lua/Shared/LuaMethodHelpers.h>
#include <Extender/ScriptExtender.h>

BEGIN_NS(ecl::lua::net)

void PostMessageToServer(char const* channel, char const* payload)
{
	// FIXME - should be done from Lua state ext!
	auto & networkMgr = gExtender->GetClient().GetNetworkManager();
	auto msg = networkMgr.GetFreeMessage();
	if (msg != nullptr) {
		auto postMsg = msg->GetMessage().mutable_post_lua();
		postMsg->set_channel_name(channel);
		postMsg->set_payload(payload);
		networkMgr.Send(msg);
	} else {
		OsiErrorS("Could not get free message!");
	}
}


void RegisterNetLib(lua_State* L)
{
	static const luaL_Reg lib[] = {
		{"PostMessageToServer", LuaWrapFunction(&PostMessageToServer)},
		{0,0}
	};

	RegisterLib(L, "Net", lib);
}

END_NS()
