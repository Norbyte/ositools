#include <Lua/Shared/LuaMethodHelpers.h>
#include <Extender/ScriptExtender.h>

BEGIN_NS(ecl::lua::ecs)

void RegisterEntityLib()
{
	DECLARE_MODULE(Entity, Client)
	BEGIN_MODULE()
	// FIXME
	END_MODULE()
}

END_NS()
