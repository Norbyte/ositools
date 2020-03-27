#pragma once

#include "ExtensionState.h"
#include <Lua/LuaBindingClient.h>

namespace Json { class Value; }

namespace dse
{
	class ExtensionStateClient;
	using LuaClientPin = LuaStatePin<ExtensionStateClient, lua::ClientState>;

	class ExtensionStateClient : public ExtensionState
	{
	public:
		static ExtensionStateClient & Get();

		lua::State * GetLua() override;
		ModManager * GetModManager() override;

		inline char const * GetBootstrapFileName() override
		{
			return "BootstrapClient.lua";
		}

	protected:
		friend LuaStatePin<ExtensionStateClient, lua::ClientState>;
		std::unique_ptr<lua::ClientState> Lua;;

		void DoLuaReset() override;
		void LuaStartup() override;
	};
}
