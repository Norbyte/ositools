#include <Lua/Shared/LuaMethodHelpers.h>
#include <Lua/Libs/ClientCharacterTask.inl>

/// <lua_module>Behavior</lua_module>
BEGIN_NS(ecl::lua::behavior)

void RegisterBehaviorLib()
{
	DECLARE_MODULE(Behavior, Client)
	BEGIN_MODULE()
	MODULE_FUNCTION(RegisterCharacterTask)
	MODULE_FUNCTION(AttachCharacterTask)
	END_MODULE()
}

END_NS()
