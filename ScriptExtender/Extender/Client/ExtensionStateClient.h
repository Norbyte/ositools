#pragma once

#include <Extender/Shared/ExtensionState.h>
#include <Lua/Client/LuaBindingClient.h>

BEGIN_NS(ecl)

using LuaClientPin = LuaStatePin<ExtensionState, lua::ClientState>;

class ExtensionState : public ExtensionStateBase
{
public:
	static ExtensionState & Get();

	lua::State * GetLua() override;
	ModManager * GetModManager() override;

	inline char const * GetBootstrapFileName() override
	{
		return "BootstrapClient.lua";
	}

protected:
	friend LuaStatePin<ExtensionState, lua::ClientState>;
	std::unique_ptr<lua::ClientState> Lua;

	void DoLuaReset() override;
	void LuaStartup() override;
};

END_NS()
