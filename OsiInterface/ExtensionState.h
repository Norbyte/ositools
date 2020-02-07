#pragma once

#include "ExtensionHelpers.h"
#include "Lua/LuaBinding.h"
#include <random>
#include <unordered_set>

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

	struct ExtensionModConfig
	{
		uint32_t MinimumVersion{ 0 };
		std::unordered_set<std::string> FeatureFlags;
	};

	class ExtensionState
	{
	public:
		ExtensionModConfig MergedConfig;
		Module const * HighestVersionMod{ nullptr };

		DamageHelperPool DamageHelpers;
		PendingStatuses PendingStatuses;
		std::unique_ptr<ObjectSet<eoc::ItemDefinition>> PendingItemClone;
		std::mt19937_64 OsiRng;
		std::unique_ptr<ShootProjectileApiHelper> ProjectileHelper;

		inline bool WasStatLoadTriggered() const
		{
			return StatLoadTriggered;
		}

		void Reset();
		void LoadConfigs();
		bool LoadConfig(Module const & mod, std::string const & configText, ExtensionModConfig & config);
		bool LoadConfig(Module const & mod, Json::Value & configJson, ExtensionModConfig & config);
		bool HasFeatureFlag(char const * flag) const;

		void OnGameSessionLoading();
		void OnModuleLoading();
		void OnModuleResume();
		void StoryLoaded();
		void StoryFunctionMappingsUpdated();

		void IncLuaRefs();
		void DecLuaRefs();
		void LuaReset(bool startup);
		void LuaLoadExternalFile(std::string const & path);
		void LuaLoadGameFile(FileReaderPin & reader, std::string const & scriptName);
		void LuaLoadGameFile(std::string const & path, std::string const & scriptName);
		void LuaLoadModScript(std::string const & modNameGuid, std::string const & fileName);

		static ExtensionState & Get();

	protected:
		friend class LuaStatePin;
		static std::unordered_set<std::string> sAllFeatureFlags;

		std::unique_ptr<LuaState> Lua;
		unsigned LuaRefs{ 0 };
		bool LuaPendingDelete{ false };
		bool LuaPendingStartup{ false };
		bool StatLoadTriggered{ false };

	private:
		void LuaResetInternal();
		void LuaStartup();
	};

	class LuaStatePin
	{
	public:
		inline LuaStatePin(ExtensionState & state)
			: state_(state)
		{
			if (state_.Lua) state_.IncLuaRefs();
		}

		inline ~LuaStatePin()
		{
			if (state_.Lua) state_.DecLuaRefs();
		}

		inline operator bool() const
		{
			return !state_.LuaPendingDelete
				&& state_.Lua;
		}

		inline LuaState & Get() const
		{
			assert(*this);
			return *state_.Lua;
		}

		inline LuaState & operator *() const
		{
			assert(*this);
			return *state_.Lua;
		}

		inline LuaState * operator ->() const
		{
			assert(*this);
			return state_.Lua.get();
		}

	private:
		ExtensionState & state_;
	};

}