#pragma once

#include "ExtensionState.h"
#include <LuaBindingServer.h>

namespace Json { class Value; }

namespace osidbg
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

	class ExtensionStateServer;
	using LuaServerPin = LuaStatePin<ExtensionStateServer, LuaStateServer>;

	class ExtensionStateServer : public ExtensionState
	{
	public:
		DamageHelperPool DamageHelpers;
		PendingStatuses PendingStatuses;
		std::unique_ptr<ObjectSet<eoc::ItemDefinition>> PendingItemClone;
		std::unique_ptr<ShootProjectileApiHelper> ProjectileHelper;

		void Reset() override;
		LuaState * GetLua() override;
		ModManager * GetModManager() override;

		inline char const * GetBootstrapFileName() override
		{
			return "BootstrapServer.lua";
		}

		void StoryLoaded();
		void StoryFunctionMappingsUpdated();

		static ExtensionStateServer & Get();

	protected:
		friend LuaStatePin<ExtensionStateServer, LuaStateServer>;
		std::unique_ptr<LuaStateServer> Lua;

		void DoLuaReset() override;
		void LuaStartup() override;
	};
}