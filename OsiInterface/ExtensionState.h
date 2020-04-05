#pragma once

#include "ExtensionHelpers.h"
#include "Lua/LuaBinding.h"
#include <random>
#include <unordered_set>

namespace Json { class Value; }

namespace dse
{
	struct ExtensionModConfig
	{
		uint32_t MinimumVersion{ 0 };
		// Name to use in Lua Mods global table (>= v43)
		STDString ModTable;
		std::unordered_set<STDString> FeatureFlags;
	};

	class ExtensionState
	{
	public:
		std::mt19937_64 OsiRng;

		virtual void Reset();
		virtual lua::State * GetLua() = 0;
		virtual ModManager * GetModManager() = 0;
		virtual char const * GetBootstrapFileName() = 0;

		void LoadConfigs();
		bool LoadConfig(Module const & mod, STDString const & configText, ExtensionModConfig & config);
		bool LoadConfig(Module const & mod, Json::Value & json, ExtensionModConfig & config);
		bool HasFeatureFlag(char const * flag) const;

		inline bool WasStatLoadTriggered() const
		{
			return StatLoadTriggered;
		}

		void OnGameSessionLoading();
		void OnGameSessionLoaded();
		void OnModuleLoading();
		void OnModuleResume();

		void IncLuaRefs();
		void DecLuaRefs();
		void LuaReset(bool startup);

		std::optional<STDString> ResolveModScriptPath(STDString const& modNameGuid, STDString const& fileName);
		STDString ResolveModScriptPath(Module const& mod, STDString const& fileName);

		std::optional<int> LuaLoadExternalFile(STDString const & path);
		std::optional<int> LuaLoadGameFile(FileReaderPin & reader, STDString const & scriptName);
		std::optional<int> LuaLoadGameFile(STDString const & path, STDString const & scriptName, bool warnOnError = true);
		std::optional<int> LuaLoadModScript(STDString const & modNameGuid, STDString const & fileName, bool warnOnError = true);

	protected:
		friend class LuaVirtualPin;
		static std::unordered_set<std::string_view> sAllFeatureFlags;

		ExtensionModConfig MergedConfig;
		Module const* HighestVersionMod{ nullptr };
		std::unordered_map<std::string, ExtensionModConfig> modConfigs_;


		unsigned LuaRefs{ 0 };
		bool LuaPendingDelete{ false };
		bool LuaPendingStartup{ false };
		bool StatLoadTriggered{ false };

		void LuaResetInternal();
		virtual void DoLuaReset() = 0;
		virtual void LuaStartup();
		void LuaLoadBootstrap(ExtensionModConfig const& config, Module const& mod);
	};


	class LuaVirtualPin
	{
	public:
		inline LuaVirtualPin(ExtensionState & state)
			: state_(&state)
		{
			if (state_->GetLua()) state_->IncLuaRefs();
		}

		inline LuaVirtualPin(ExtensionState * state)
			: state_(state)
		{
			if (state_ && state_->GetLua()) state_->IncLuaRefs();
		}

		inline ~LuaVirtualPin()
		{
			if (state_ && state_->GetLua()) state_->DecLuaRefs();
		}

		inline operator bool() const
		{
			return state_ 
				&& !state_->LuaPendingDelete
				&& state_->GetLua();
		}

		inline lua::State & Get() const
		{
			assert(*this);
			return *state_->GetLua();
		}

		inline lua::State & operator *() const
		{
			assert(*this);
			return *state_->GetLua();
		}

		inline lua::State * operator ->() const
		{
			assert(*this);
			return state_->GetLua();
		}

	private:
		ExtensionState * state_;
	};


	template <class T, class TLua>
	class LuaStatePin
	{
	public:
		inline LuaStatePin(T & state)
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

		inline TLua & Get() const
		{
			assert(*this);
			return *state_.Lua;
		}

		inline TLua & operator *() const
		{
			assert(*this);
			return *state_.Lua;
		}

		inline TLua * operator ->() const
		{
			assert(*this);
			return state_.Lua.get();
		}

	private:
		T & state_;
	};

}