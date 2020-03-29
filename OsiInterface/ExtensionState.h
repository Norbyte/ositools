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
		std::unordered_set<STDString> FeatureFlags;
	};

	class ExtensionState
	{
	public:
		ExtensionModConfig MergedConfig;
		Module const * HighestVersionMod{ nullptr };

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
		void OnModuleLoading();
		void OnModuleResume();

		void IncLuaRefs();
		void DecLuaRefs();
		void LuaReset(bool startup);
		void LuaLoadExternalFile(STDString const & path);
		void LuaLoadGameFile(FileReaderPin & reader, STDString const & scriptName);
		bool LuaLoadGameFile(STDString const & path, STDString const & scriptName, bool warnOnError = true);
		bool LuaLoadModScript(STDString const & modNameGuid, STDString const & fileName, bool warnOnError = true);

	protected:
		friend class LuaVirtualPin;
		static std::unordered_set<std::string_view> sAllFeatureFlags;

		unsigned LuaRefs{ 0 };
		bool LuaPendingDelete{ false };
		bool LuaPendingStartup{ false };
		bool StatLoadTriggered{ false };

		void LuaResetInternal();
		virtual void DoLuaReset() = 0;
		virtual void LuaStartup();
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