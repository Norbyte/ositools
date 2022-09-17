#include <stdafx.h>
#include "DxgiWrapper.h"
#include <Extender/ScriptExtender.h>
#include "json/json.h"
#include <ShellAPI.h>
#include <KnownFolders.h>
#include <ShlObj.h>
#include <sstream>
#include <fstream>

void ConfigGet(Json::Value & node, char const * key, bool & value)
{
	auto configVar = node[key];
	if (!configVar.isNull() && configVar.isBool()) {
		value = configVar.asBool();
	}
}

void ConfigGet(Json::Value& node, char const* key, uint32_t& value)
{
	auto configVar = node[key];
	if (!configVar.isNull() && configVar.isUInt()) {
		value = configVar.asUInt();
	}
}

void ConfigGet(Json::Value& node, char const* key, std::wstring& value)
{
	auto configVar = node[key];
	if (!configVar.isNull() && configVar.isString()) {
		value = FromUTF8(configVar.asString());
	}
}

#pragma message("Move to ConfigLoader?")
void LoadConfig(std::wstring const & configPath, dse::ExtenderConfig & config)
{
	std::ifstream f(configPath, std::ios::in);
	if (!f.good()) {
		return;
	}

	Json::CharReaderBuilder factory;
	Json::Value root;
	std::string errs;
	if (!Json::parseFromStream(factory, f, &root, &errs)) {
		std::wstring werrs = FromUTF8(errs);

		std::wstringstream err;
		err << L"Failed to load configuration file '" << configPath << "':\r\n" << werrs;
		Fail(err.str().c_str());
	}

	ConfigGet(root, "CreateConsole", config.CreateConsole);
	ConfigGet(root, "EnableLogging", config.EnableLogging);
	ConfigGet(root, "LogCompile", config.LogCompile);
	ConfigGet(root, "LogFailedCompile", config.LogFailedCompile);
	ConfigGet(root, "LogRuntime", config.LogRuntime);
	ConfigGet(root, "EnableExtensions", config.EnableExtensions);
	ConfigGet(root, "SendCrashReports", config.SendCrashReports);
	ConfigGet(root, "ForceCrashReporting", config.ForceCrashReporting);
	ConfigGet(root, "DumpNetworkStrings", config.DumpNetworkStrings);
	ConfigGet(root, "SyncNetworkStrings", config.SyncNetworkStrings);
	ConfigGet(root, "EnableDebugger", config.EnableDebugger);
	ConfigGet(root, "EnableLuaDebugger", config.EnableLuaDebugger);
	ConfigGet(root, "OptimizeHashing", config.OptimizeHashing);
	ConfigGet(root, "DisableModValidation", config.DisableModValidation);
	ConfigGet(root, "DeveloperMode", config.DeveloperMode);
	ConfigGet(root, "ShowPerfWarnings", config.ShowPerfWarnings);
	ConfigGet(root, "EnableAchievements", config.EnableAchievements);

	ConfigGet(root, "DebuggerPort", config.DebuggerPort);
	ConfigGet(root, "LuaDebuggerPort", config.LuaDebuggerPort);
	ConfigGet(root, "DebugFlags", config.DebugFlags);

	ConfigGet(root, "LogDirectory", config.LogDirectory);
	ConfigGet(root, "LuaBuiltinResourceDirectory", config.LuaBuiltinResourceDirectory);
}

std::wstring GetConfigPath()
{
	std::wstring exeDir;
	HMODULE exeHandle = GetModuleHandleW(NULL);
	if (exeHandle != NULL) {
		exeDir.resize(MAX_PATH);
		DWORD exeSize = GetModuleFileNameW(exeHandle, exeDir.data(), (DWORD)exeDir.size());
		exeDir.resize(exeSize);
		auto sep = exeDir.find_last_of(L'\\');
		if (sep != std::string::npos) {
			exeDir = exeDir.substr(0, sep + 1);
		}
	}

	return exeDir + L"OsirisExtenderSettings.json";
}

void SetupOsirisProxy(HMODULE hModule)
{
	dse::gExtender = std::make_unique<dse::ScriptExtender>();
	auto & config = dse::gExtender->GetConfig();
	LoadConfig(GetConfigPath(), config);

	DisableThreadLibraryCalls(hModule);
	if (config.CreateConsole) {
		dse::gConsole.Create();
	}

	if (config.DebugFlags == 0) {
		// Disabled: DF_FunctionList, DF_NodeList ,DF_LogSuccessfulFacts, DF_LogFailedFacts, DB_LogFactFailures, DF_DumpDatabases, DF_DebugFacts, DF_LogRuleFailures
		config.DebugFlags = dse::DF_DebugTrace | dse::DF_SuppressInitLog;
	}

	dse::gExtender->Initialize();

#if 0
	DEBUG(" ***** ScriptExtender setup completed ***** ");
#endif
}

bool ShouldRunProxy()
{
	return GetModuleHandleW(L"EoCApp.exe") != NULL
		|| GetModuleHandleW(L"DivinityEngine2.exe") != NULL;
}

HMODULE gThisModule{ NULL };

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		gThisModule = hModule;
		if (ShouldRunProxy()) {
			gDxgiWrapper = std::make_unique<DxgiWrapper>();
			SetupOsirisProxy(hModule);
		}
		break;

	case DLL_PROCESS_DETACH:
		if (gDxgiWrapper) {
			dse::gExtender->Shutdown();
			dse::gExtender.reset();
			gDxgiWrapper.reset();
		}
		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

