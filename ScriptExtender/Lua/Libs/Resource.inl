#pragma once
#include <Lua/Shared/LuaSerializers.h>
#include <Lua/Shared/LuaMethodHelpers.h>
#include <Extender/ScriptExtender.h>

BEGIN_NS(lua::resource)

Resource* GetResource(ResourceType type, FixedString templateId)
{
	auto bank = GetStaticSymbols().GetResourceBank();
	auto res = bank->Container.Banks[(uint32_t)type]->Resources.Find(templateId);
	return res ? *res : nullptr;
}

void RegisterResourceLib(lua_State* L)
{
	static const luaL_Reg lib[] = {
		{"Get", LuaWrapFunction(&GetResource)},
		{0,0}
	};

	RegisterLib(L, "Resource", lib);
}

END_NS()
