#include <Lua/Shared/LuaMethodHelpers.h>
#include <Extender/ScriptExtender.h>

BEGIN_NS(esv::lua::server)

std::optional<GameState> GetGameState()
{
	return GetStaticSymbols().GetServerState();
}

ModManager* GetModManager()
{
	return GetModManagerServer();
}

void RegisterServerLib()
{
	DECLARE_MODULE(Server, Server)
	BEGIN_MODULE()
	MODULE_FUNCTION(GetGameState)
	MODULE_FUNCTION(GetModManager)
	END_MODULE()
}

END_NS()
