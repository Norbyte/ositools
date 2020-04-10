#pragma once

#include "ExtensionState.h"
#include <Lua/LuaBindingServer.h>

namespace Json { class Value; }

namespace dse::esv
{
	class DamageHelperPool
	{
	public:
		void Clear();
		DamageHelpers * Create();
		bool Destroy(int64_t handle);
		DamageHelpers * Get(int64_t handle) const;

	private:
		std::unordered_map<int64_t, std::unique_ptr<DamageHelpers>> helpers_;
		int64_t nextHelperId_{ 0 };
	};

	class ExtensionState;
	using LuaServerPin = LuaStatePin<ExtensionState, lua::ServerState>;

	class ExtensionState : public ExtensionStateBase
	{
	public:
		DamageHelperPool DamageHelpers;
		PendingStatuses PendingStatuses;
		std::unique_ptr<ObjectSet<eoc::ItemDefinition>> PendingItemClone;
		std::unique_ptr<ShootProjectileApiHelper> ProjectileHelper;

		void Reset() override;
		lua::State * GetLua() override;
		ModManager * GetModManager() override;

		inline char const * GetBootstrapFileName() override
		{
			return "BootstrapServer.lua";
		}

		void StoryLoaded();
		void StoryFunctionMappingsUpdated();

		static ExtensionState & Get();

	protected:
		friend LuaStatePin<ExtensionState, lua::ServerState>;
		std::unique_ptr<lua::ServerState> Lua;

		void DoLuaReset() override;
		void LuaStartup() override;
	};
}