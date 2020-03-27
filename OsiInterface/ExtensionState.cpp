#include <stdafx.h>
#include <OsirisProxy.h>
#include <ExtensionState.h>
#include <Version.h>
#include <fstream>
#include "json/json.h"

namespace dse
{
	std::unordered_set<std::string> ExtensionState::sAllFeatureFlags = {
		"OsirisExtensions",
		"Lua",
		"CustomStats",
		"CustomStatsPane",
		"FormulaOverrides",
		"Preprocessor",
		"DisableFolding"
	};

	void ExtensionState::Reset()
	{
		time_t tm;
		OsiRng.seed(time(&tm));
	}

	void ExtensionState::LoadConfigs()
	{
		auto modManager = GetModManager();
		if (modManager == nullptr) {
			OsiErrorS("Mod manager not available");
			return;
		}

		auto & mods = modManager->BaseModule.LoadOrderedModules.Set;

		unsigned numConfigs{ 0 };
		for (uint32_t i = 0; i < mods.Size; i++) {
			auto const & mod = mods[i];
			auto dir = ToUTF8(mod.Info.Directory.GetPtr());
			auto configFile = "Mods/" + dir + "/OsiToolsConfig.json";
			auto reader = GetStaticSymbols().MakeFileReader(configFile);

			if (reader.IsLoaded()) {
				ExtensionModConfig config;
				if (LoadConfig(mod, reader.ToString(), config)) {
					std::stringstream featureFlags;
					for (auto const & flag : config.FeatureFlags) {
						featureFlags << flag << " ";
					}

					INFO(L"Configuration for '%s':\r\n\tMinVersion %d; Feature flags: %s", mod.Info.Name.GetPtr(),
						config.MinimumVersion, FromUTF8(featureFlags.str()).c_str());

					if (config.MinimumVersion != 0 && config.MinimumVersion < 42) {
						OsiError("Module '" << ToUTF8(mod.Info.Name.GetPtr()) << "' uses extender version v" << config.MinimumVersion << ", which uses the old unified client/server model.");
						OsiError("The client and server components were split in v42 to improve support for client-side features.");
						OsiError("Please migrate to v42+!");
					}

					if (config.MinimumVersion != 0 && config.MinimumVersion > MergedConfig.MinimumVersion) {
						MergedConfig.MinimumVersion = config.MinimumVersion;
						HighestVersionMod = &mod;
					}

					for (auto const & flag : config.FeatureFlags) {
						MergedConfig.FeatureFlags.insert(flag);
					}

					numConfigs++;
				}
			}
		}

		if (numConfigs > 0) {
			INFO("%d mod configuration(s) loaded.", numConfigs);
			std::stringstream featureFlags;
			for (auto const & flag : MergedConfig.FeatureFlags) {
				featureFlags << flag << " ";
			}

			INFO(L"Merged configuration:\r\n\tMinVersion %d; Feature flags: %s",
				MergedConfig.MinimumVersion, FromUTF8(featureFlags.str()).c_str());
		}

		if (CurrentVersion < MergedConfig.MinimumVersion && HighestVersionMod != nullptr) {
			std::wstringstream msg;
			msg << L"Module \"" << HighestVersionMod->Info.Name.GetPtr() << "\" requires extension version "
				<< MergedConfig.MinimumVersion << "; current version is v" << CurrentVersion;
			gOsirisProxy->GetLibraryManager().ShowStartupError(msg.str(), false, true);
		}
	}

	bool ExtensionState::LoadConfig(Module const & mod, std::string const & configText, ExtensionModConfig & config)
	{
		Json::CharReaderBuilder factory;
		auto reader = std::unique_ptr<Json::CharReader>(factory.newCharReader());

		Json::Value root;
		std::string errs;
		if (!reader->parse(configText.c_str(), configText.c_str() + configText.size(), &root, &errs)) {
			OsiError("Unable to parse configuration for mod '" << ToUTF8(mod.Info.Name.GetPtr()) << "': " << errs);
			return false;
		}

		return LoadConfig(mod, root, config);
	}

	std::optional<bool> GetConfigBool(Json::Value & config, std::string const & key)
	{
		auto value = config[key];
		if (!value.isNull()) {
			if (value.isBool()) {
				return value.asBool();
			} else {
				OsiError("Config option '" << key << "' should be a boolean.");
				return {};
			}
		} else {
			return {};
		}
	}

	std::optional<int32_t> GetConfigInt(Json::Value & config, std::string const & key)
	{
		auto value = config[key];
		if (!value.isNull()) {
			if (value.isInt()) {
				return value.asInt();
			} else {
				OsiError("Config option '" << key << "' should be an integer.");
				return {};
			}
		} else {
			return {};
		}
	}

	bool ExtensionState::LoadConfig(Module const & mod, Json::Value & json, ExtensionModConfig & config)
	{
		bool hasLegacyFeatureFlags = false;

		auto extendOsiris = GetConfigBool(json, "ExtendOsiris");
		if (extendOsiris && *extendOsiris) {
			config.FeatureFlags.insert("OsirisExtensions");
			hasLegacyFeatureFlags = true;
		}

		auto lua = GetConfigBool(json, "Lua");
		if (lua && *lua) {
			config.FeatureFlags.insert("Lua");
			hasLegacyFeatureFlags = true;
		}

		auto customStats = GetConfigBool(json, "UseCustomStats");
		if (customStats && *customStats) {
			config.FeatureFlags.insert("CustomStats");
			hasLegacyFeatureFlags = true;
		}

		auto customStatsPane = GetConfigBool(json, "UseCustomStatsPane");
		if (customStatsPane && *customStatsPane) {
			config.FeatureFlags.insert("CustomStatsPane");
			hasLegacyFeatureFlags = true;
		}

		auto formulaOverrides = GetConfigBool(json, "FormulaOverrides");
		if (formulaOverrides && *formulaOverrides) {
			config.FeatureFlags.insert("FormulaOverrides");
			hasLegacyFeatureFlags = true;
		}

		auto preprocessStory = GetConfigBool(json, "PreprocessStory");
		if (preprocessStory && *preprocessStory) {
			config.FeatureFlags.insert("Preprocessor");
			hasLegacyFeatureFlags = true;
		}

		auto version = GetConfigInt(json, "RequiredExtensionVersion");
		if (version) {
			config.MinimumVersion = (uint32_t)*version;
		}

		auto featureFlags = json["FeatureFlags"];
		if (featureFlags.isArray()) {
			for (auto const & flag : featureFlags) {
				if (flag.isString()) {
					auto flagStr = flag.asString();
					if (sAllFeatureFlags.find(flagStr) != sAllFeatureFlags.end()) {
						config.FeatureFlags.insert(flagStr);
					} else {
						ERR("Feature flag '%s' not supported!", flagStr.c_str());
					}
				} else {
					ERR("Garbage found in FeatureFlags array");
				}
			}
		}

		if (hasLegacyFeatureFlags) {
			ERR("Deprecated configuration key found (ExtendOsiris, Lua, PreprocessStory, etc.); please use FeatureFlags instead");
		}

		return true;
	}

	bool ExtensionState::HasFeatureFlag(char const * flag) const
	{
		return MergedConfig.FeatureFlags.find(flag) != MergedConfig.FeatureFlags.end();
	}

	void ExtensionState::OnGameSessionLoading()
	{
		LuaVirtualPin lua(*this);
		if (lua) {
			lua->OnGameSessionLoading();
		}
	}

	void ExtensionState::OnModuleLoading()
	{
		StatLoadTriggered = true;
		LuaVirtualPin lua(*this);
		if (lua) {
			lua->OnModuleLoading();
		}
	}

	void ExtensionState::OnModuleResume()
	{
		LuaVirtualPin lua(*this);
		if (lua) {
			lua->OnModuleResume();
		}
	}


	void ExtensionState::IncLuaRefs()
	{
		assert(GetLua());
		LuaRefs++;
	}

	void ExtensionState::DecLuaRefs()
	{
		assert(LuaRefs > 0);
		LuaRefs--;
		if (LuaRefs == 0 && LuaPendingDelete) {
			LuaResetInternal();
		}
	}

	void ExtensionState::LuaReset(bool startup)
	{
		if (!HasFeatureFlag("Lua")) {
			OsiWarn("Lua extensions not enabled; not initializing Lua VM");
			return;
		}

		if (LuaPendingDelete) {
			OsiWarn("State delete is already pending!");
		}

		LuaPendingDelete = true;
		if (startup) {
			LuaPendingStartup = true;
		}

		if (LuaRefs == 0) {
			LuaResetInternal();
		} else {
			OsiWarn("Lua state deletion deferred (" << LuaRefs << " references still alive)");
		}
	}

	void ExtensionState::LuaLoadExternalFile(std::string const & path)
	{
		std::ifstream f(path, std::ios::in | std::ios::binary);
		if (!f.good()) {
			OsiError("File does not exist: " << path);
			return;
		}

		f.seekg(0, std::ios::end);
		auto length = f.tellg();
		f.seekg(0, std::ios::beg);
		std::string s(length, '\0');
		f.read(const_cast<char *>(s.data()), length);
		f.close();

		LuaVirtualPin lua(*this);
		if (!lua) {
			OsiErrorS("Called when the Lua VM has not been initialized!");
			return;
		}

		lua->LoadScript(s, path);
		OsiWarn("Loaded external script: " << path);
	}

	void ExtensionState::LuaLoadGameFile(FileReaderPin & reader, std::string const & scriptName)
	{
		if (!reader.IsLoaded()) {
			OsiErrorS("Attempted to load script from invalid file reader");
			return;
		}

		LuaVirtualPin lua(*this);
		if (!lua) {
			OsiErrorS("Called when the Lua VM has not been initialized!");
			return;
		}

		lua->LoadScript(reader.ToString(), scriptName);
	}

	bool ExtensionState::LuaLoadGameFile(std::string const & path, std::string const & scriptName, bool warnOnError)
	{
		auto reader = GetStaticSymbols().MakeFileReader(path);
		if (!reader.IsLoaded()) {
			if (warnOnError) {
				OsiError("Script file could not be opened: " << path);
			}
			return false;
		}

		LuaLoadGameFile(reader, scriptName.empty() ? path : scriptName);
		OsiMsg("Loaded game script: " << path);
		return true;
	}

	bool ExtensionState::LuaLoadModScript(std::string const & modNameGuid, std::string const & fileName, bool warnOnError)
	{
		auto mod = GetModManager()->FindModByNameGuid(modNameGuid.c_str());
		if (mod == nullptr) {
			OsiError("Mod does not exist or is not loaded: " << modNameGuid);
			return false;
		}

		std::string path("Mods/");
		path += ToUTF8(mod->Info.Directory.GetPtr());
		path += "/Story/RawFiles/Lua/";
		path += fileName;

		std::string scriptName = ToUTF8(mod->Info.Directory.GetPtr());
		if (scriptName.length() > 37) {
			// Strip GUID from end of dir
			scriptName = scriptName.substr(0, scriptName.length() - 37);
		}
		scriptName += "/" + fileName;

		return LuaLoadGameFile(path, scriptName, warnOnError);
	}

	void ExtensionState::LuaResetInternal()
	{
		assert(LuaPendingDelete);
		assert(LuaRefs == 0);

		LuaPendingDelete = false;

		// Destroy previous instance first to make sure that no dangling
		// references are made to the old state while constructing the new
		DoLuaReset();
		OsiWarn("LUA VM reset.");

		if (LuaPendingStartup) {
			LuaPendingStartup = false;
			LuaStartup();
		}
	}

	void ExtensionState::LuaStartup()
	{
		LuaVirtualPin lua(*this);
		if (!lua) {
			OsiErrorS("Called when the Lua VM has not been initialized!");
			return;
		}

		auto modManager = GetModManager();
		if (modManager == nullptr) {
			OsiErrorS("Could not bootstrap Lua modules - mod manager not available");
			return;
		}

		auto & mods = modManager->BaseModule.LoadOrderedModules.Set;

		auto bootstrapPath = GetBootstrapFileName();
		lua::Restriction restriction(*lua, lua::State::RestrictAllClient);
		for (uint32_t i = 0; i < mods.Size; i++) {
			auto const & mod = mods[i];

			if (!LuaLoadModScript(mod.Info.ModuleUUID.Str, bootstrapPath, false)) {
				if (LuaLoadModScript(mod.Info.ModuleUUID.Str, "Bootstrap.lua", false)) {
					OsiError("Module '" << ToUTF8(mod.Info.Name.GetPtr()) << "' uses a legacy Lua bootstrap file (Bootstrap.lua)");
					OsiError("Please migrate to separate client/server bootstrap files!");
				}
			}
		}

		lua->FinishStartup();
	}
}
