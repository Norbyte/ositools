#include <stdafx.h>
#include <OsirisProxy.h>
#include <ExtensionState.h>
#include <Version.h>
#include <fstream>
#include "json/json.h"

namespace osidbg
{
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
			auto const & mod = mods.Buf[i];
			auto dir = ToUTF8(mod.Info.Directory.GetPtr());
			auto configFile = "Mods/" + dir + "/OsiToolsConfig.json";
			auto reader = gOsirisProxy->GetLibraryManager().MakeFileReader(configFile);

			if (reader.IsLoaded()) {
				LoadConfig(mod, reader.ToString());
				numConfigs++;
			}
		}

		if (numConfigs > 0) {
			INFO("%d mod configuration(s) configuration loaded.", numConfigs);
			INFO("Extensions=%d, Lua=%d, CustomStats=%d, CustomStatsPane=%d, FormulaOverrides=%d, MinVersion=%d",
				EnableExtensions, EnableLua, EnableCustomStats, EnableCustomStatsPane,
				EnableFormulaOverrides, MinimumVersion);
		}

		if (CurrentVersion < MinimumVersion && HighestVersionMod != nullptr) {
			std::wstringstream msg;
			msg << L"Module \"" << HighestVersionMod->Info.Name.GetPtr() << "\" requires extension version "
				<< MinimumVersion << "; current version is v" << CurrentVersion;
			gOsirisProxy->GetLibraryManager().ShowStartupError(msg.str(), false, true);
		}
	}

	void ExtensionState::LoadConfig(Module const & mod, std::string const & config)
	{
		Json::CharReaderBuilder factory;
		auto reader = factory.newCharReader();

		Json::Value root;
		std::string errs;
		if (!reader->parse(config.c_str(), config.c_str() + config.size(), &root, &errs)) {
			OsiError("Unable to parse configuration for mod '" << ToUTF8(mod.Info.Name.GetPtr()) << "': " << errs);
			return;
		}

		LoadConfig(mod, root);

		delete reader;
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

	void ExtensionState::LoadConfig(Module const & mod, Json::Value & config)
	{
		auto extendOsiris = GetConfigBool(config, "ExtendOsiris");
		if (extendOsiris && *extendOsiris) {
			EnableExtensions = true;
		}

		auto lua = GetConfigBool(config, "Lua");
		if (lua && *lua) {
			EnableLua = true;
		}

		auto customStats = GetConfigBool(config, "UseCustomStats");
		if (customStats && *customStats) {
			EnableCustomStats = true;
		}

		auto customStatsPane = GetConfigBool(config, "UseCustomStatsPane");
		if (customStatsPane && *customStatsPane) {
			EnableCustomStatsPane = true;
		}

		auto formulaOverrides = GetConfigBool(config, "FormulaOverrides");
		if (formulaOverrides && *formulaOverrides) {
			EnableFormulaOverrides = true;
		}

		auto preprocessStory = GetConfigBool(config, "PreprocessStory");
		if (preprocessStory && *preprocessStory) {
			PreprocessStory = true;
		}

		auto version = GetConfigInt(config, "RequiredExtensionVersion");
		if (version && MinimumVersion < (uint32_t)*version) {
			MinimumVersion = (uint32_t)*version;
			HighestVersionMod = &mod;
		}
	}

	ExtensionState & ExtensionState::Get()
	{
		return gOsirisProxy->GetExtensionState();
	}
}
