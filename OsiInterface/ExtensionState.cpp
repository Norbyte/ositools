#include <stdafx.h>
#include <OsirisProxy.h>
#include <ExtensionState.h>
#include <Version.h>
#include <fstream>
#include "json/json.h"

namespace osidbg
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
		DamageHelpers.Clear();

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

	ExtensionState & ExtensionState::Get()
	{
		return gOsirisProxy->GetExtensionState();
	}
}
