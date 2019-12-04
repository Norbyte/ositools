#pragma once

#include "ExtensionHelpers.h"
#include "Lua/LuaBinding.h"
#include <random>

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

	class ExtensionState
	{
	public:
		bool EnableExtensions{ false };
		bool EnableLua{ false };
		bool EnableCustomStats{ false };
		bool EnableCustomStatsPane{ false };
		bool EnableFormulaOverrides{ false };
		bool PreprocessStory{ false };

		uint32_t MinimumVersion{ 0 };
		Module const * HighestVersionMod{ nullptr };

		DamageHelperPool DamageHelpers;
		PendingStatuses PendingStatuses;
		std::unique_ptr<ObjectSet<eoc::ItemDefinition>> PendingItemClone;
		std::mt19937_64 OsiRng;
		std::unique_ptr<ShootProjectileApiHelper> ProjectileHelper;

		void Reset();
		void LoadConfigs();
		void LoadConfig(Module const & mod, std::string const & config);
		void LoadConfig(Module const & mod, Json::Value & config);

		void StoryLoaded();
		void StoryFunctionMappingsUpdated();

		void IncLuaRefs();
		void DecLuaRefs();
		void LuaReset(bool startup);
		void LuaLoadExternalFile(std::string const & path);
		void LuaLoadGameFile(FileReaderPin & reader);
		void LuaLoadGameFile(std::string const & path);
		void LuaLoadGameFile(std::string const & modNameGuid, std::string const & fileName);

		static ExtensionState & Get();

	protected:
		friend class LuaStatePin;
		std::unique_ptr<LuaState> Lua;
		unsigned LuaRefs{ 0 };
		bool LuaPendingDelete{ false };
		bool LuaPendingStartup{ false };

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
			state_.IncLuaRefs();
		}

		inline ~LuaStatePin()
		{
			state_.DecLuaRefs();
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

		inline LuaState * operator ->() const
		{
			assert(*this);
			return state_.Lua.get();
		}

	private:
		ExtensionState & state_;
	};

}