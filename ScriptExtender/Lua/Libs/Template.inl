#pragma once
#include <Lua/Shared/LuaSerializers.h>
#include <Lua/Shared/LuaMethodHelpers.h>
#include <Extender/ScriptExtender.h>

BEGIN_NS(lua::tmpl)

GameObjectTemplate* GetRootTemplate(FixedString templateId)
{
	auto bank = GetStaticSymbols().GetGlobalTemplateBank();
	auto tmpl = bank->Templates.Find(templateId);
	if (tmpl) {
		return *tmpl;
	} else {
		return nullptr;
	}
}

/*GameObjectTemplate* GetRootTemplate(FixedString templateId)
{
	auto gtm = GetStaticSymbols().ls__GlobalTemplateManager;
	auto tls = *(uint64_t*)__readgsqword(0x58);
	auto slot = ((uint8_t*)tls)[8];
	auto bank = (*gtm)->Banks[slot];
	auto tmpl = bank->Templates.Find(templateId);
	if (tmpl) {
		return *tmpl;
	} else {
		return nullptr;
	}
}*/

GameObjectTemplate* GetLocalTemplate(FixedString templateId)
{
	auto gtm = GetStaticSymbols().esv__LocalTemplateManager;
	auto tmpl = (*gtm)->Templates.Find(templateId);
	if (tmpl) {
		return *tmpl;
	} else {
		return nullptr;
	}
}

void RegisterTemplateLib(lua_State* L)
{
	static const luaL_Reg lib[] = {
		{"GetRootTemplate", LuaWrapFunction(&GetRootTemplate)},
		{"GetLocalTemplate", LuaWrapFunction(&GetLocalTemplate)},
		{0,0}
	};

	RegisterLib(L, "Template", lib);
}

END_NS()
