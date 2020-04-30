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
		bool Destroy(ObjectHandle handle);
		DamageHelpers * Get(ObjectHandle handle) const;

	private:
		std::unordered_map<uint32_t, std::unique_ptr<DamageHelpers>> helpers_;
		uint32_t nextHelperId_{ 0 };
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
		void OnGameSessionLoading() override;

		inline char const * GetBootstrapFileName() override
		{
			return "BootstrapServer.lua";
		}

		inline std::unordered_set<FixedString> const& GetRuntimeModifiedStats() const
		{
			return runtimeModifiedStats_;
		}

		void MarkRuntimeModifiedStat(FixedString const& statId);

		void StoryLoaded();
		void StoryFunctionMappingsUpdated();

		static ExtensionState & Get();

	protected:
		friend LuaStatePin<ExtensionState, lua::ServerState>;
		std::unique_ptr<lua::ServerState> Lua;
		std::unordered_set<FixedString> runtimeModifiedStats_;

		void DoLuaReset() override;
		void LuaStartup() override;
	};
}