#include <GameDefinitions/Base/BaseUtilities.h>
#include <Lua/Shared/LuaMethodHelpers.h>
#include <Extender/ScriptExtender.h>
#include <GameDefinitions/GameObjects/AiHelpers.h>

/// <lua_module>Ai</lua_module>
BEGIN_NS(esv::lua::ai)

AiModifiers* GetArchetypes()
{
	return *GetStaticSymbols().esv__gAiModifiers;
}

AiHelpers* GetAiHelpers()
{
	return *GetStaticSymbols().esv__gAiHelpers;
}

void RegisterAiLib()
{
	DECLARE_MODULE(Ai, Server)
	BEGIN_MODULE()
	MODULE_FUNCTION(GetArchetypes)
	MODULE_FUNCTION(GetAiHelpers)
	END_MODULE()
}

END_NS()
