#pragma once

#include "ExtensionState.h"
#include <LuaBindingClient.h>

namespace Json { class Value; }

namespace osidbg
{
	class ExtensionStateClient;
	using LuaClientPin = LuaStatePin<ExtensionStateClient, LuaStateClient>;

	class ExtensionStateClient : public ExtensionState
	{
	public:
		static ExtensionStateClient & Get();

		LuaState * GetLua() override;
		ModManager * GetModManager() override;

		inline char const * GetBootstrapFileName() override
		{
			return "BootstrapClient.lua";
		}

	protected:
		friend LuaStatePin<ExtensionStateClient, LuaStateClient>;
		std::unique_ptr<LuaStateClient> Lua;;

		void DoLuaReset() override;
		void LuaStartup() override;
	};
}
