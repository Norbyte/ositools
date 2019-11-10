#include "stdafx.h"
#include "DxgiWrapper.h"
#include "OsirisProxy.h"
#include "SocketProxy.h"
#include "json/json.h"
#include <ShellAPI.h>
#include <KnownFolders.h>
#include <ShlObj.h>
#include <sstream>
#include <fstream>
#include <locale>
#include <codecvt>

struct ToolConfig
{
#if defined(OSI_EXTENSION_BUILD)
	bool CreateConsole{ false };
	bool EnableLogging{ false };
	bool LogCompile{ false };
	bool EnableExtensions{ true };
	bool EnableDebugger{ false };
#else
	bool CreateConsole{ true };
	bool EnableLogging{ false };
	bool LogCompile{ false };
	bool EnableExtensions{ false };
	bool EnableDebugger{ true };
#endif
	bool DumpNetworkStrings{ false };
	uint16_t DebuggerPort{ 9999 };
	uint32_t DebugFlags{ 0 };
	std::wstring LogDirectory;
};

void LoadConfig(std::wstring const & configPath, ToolConfig & config)
{
	std::ifstream f(configPath, std::ios::in);
	if (!f.good()) {
		return;
	}

	Json::CharReaderBuilder factory;
	Json::Value root;
	std::string errs;
	if (!Json::parseFromStream(factory, f, &root, &errs)) {
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		std::wstring werrs = converter.from_bytes(errs);

		std::wstringstream err;
		err << L"Failed to load configuration file '" << configPath << "':\r\n" << werrs;
		Fail(err.str().c_str());
	}

	auto createConsole = root["CreateConsole"];
	if (!createConsole.isNull()) {
		if (createConsole.isBool()) {
			config.CreateConsole = createConsole.asBool();
		} else {
			Fail("Config option 'CreateConsole' should be a boolean.");
		}
	}

	auto enableLogging = root["EnableLogging"];
	if (!enableLogging.isNull()) {
		if (enableLogging.isBool()) {
			config.EnableLogging = enableLogging.asBool();
		} else {
			Fail("Config option 'EnableLogging' should be a boolean.");
		}
	}

	auto logCompile = root["LogCompile"];
	if (!logCompile.isNull()) {
		if (logCompile.isBool()) {
			config.LogCompile = logCompile.asBool();
		}
		else {
			Fail("Config option 'LogCompile' should be a boolean.");
		}
	}

	auto enableExtensions = root["EnableExtensions"];
	if (!enableExtensions.isNull()) {
		if (enableExtensions.isBool()) {
			config.EnableExtensions = enableExtensions.asBool();
		}
		else {
			Fail("Config option 'EnableExtensions' should be a boolean.");
		}
	}

	auto dumpStrings = root["DumpNetworkStrings"];
	if (!dumpStrings.isNull()) {
		if (dumpStrings.isBool()) {
			config.DumpNetworkStrings = dumpStrings.asBool();
		} else {
			Fail("Config option 'DumpNetworkStrings' should be a boolean.");
		}
	}

	auto enableDAP = root["EnableDebugger"];
	if (!enableDAP.isNull()) {
		if (enableDAP.isBool()) {
			config.EnableDebugger = enableDAP.asBool();
		} else {
			Fail("Config option 'EnableDebugger' should be a boolean.");
		}
	}

	auto debuggerPort = root["DebuggerPort"];
	if (!debuggerPort.isNull()) {
		if (debuggerPort.isUInt()) {
			config.DebuggerPort = debuggerPort.asUInt();
		} else {
			Fail("Config option 'DebuggerPort' should be an integer.");
		}
	}

	auto flags = root["DebugFlags"];
	if (!flags.isNull()) {
		if (flags.isUInt()) {
			config.DebugFlags = flags.asUInt();
		} else {
			Fail("Config option 'DebugFlags' should be an integer.");
		}
	}

	auto logDir = root["LogDirectory"];
	if (!logDir.isNull()) {
		if (logDir.isString()) {
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			config.LogDirectory = converter.from_bytes(logDir.asString());
		}
		else {
			Fail("Config option 'LogDirectory' should be a string.");
		}
	}
}

void SetupOsirisProxy(HMODULE hModule)
{
	ToolConfig config;
	LoadConfig(L"OsirisProxy.json", config);

	if (config.CreateConsole) {
		CreateConsole(hModule);
	}

	if (config.DebugFlags == 0) {
		// Disabled: DF_FunctionList, DF_NodeList ,DF_LogSuccessfulFacts, DF_LogFailedFacts, DB_LogFactFailures, DF_DumpDatabases, DF_DebugFacts, DF_LogRuleFailures
		config.DebugFlags = osidbg::DF_DebugTrace | osidbg::DF_SuppressInitLog;
	}

	osidbg::gOsirisProxy = std::make_unique<osidbg::OsirisProxy>();

	if (config.LogDirectory.empty()) {
		TCHAR * DocumentsPath;
		if (SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_SIMPLE_IDLIST, NULL, &DocumentsPath) != S_OK) {
			Fail("Could not get user documents path.");
		}

		std::wstring logDir = DocumentsPath;
		logDir += L"\\OsirisLogs";
		config.LogDirectory = logDir;
	}

	osidbg::gOsirisProxy->SetupLogging(config.EnableLogging, (osidbg::DebugFlag)config.DebugFlags, config.LogDirectory);
	osidbg::gOsirisProxy->EnableCompileLogging(config.LogCompile);

	if (config.EnableLogging) {
		Debug(L"Osiris logs will be written to %s", config.LogDirectory.c_str());
	}

	if (config.EnableDebugger) {
		osidbg::gOsirisProxy->EnableDebugging(true, config.DebuggerPort);
	}

	if (config.DumpNetworkStrings) {
		osidbg::gOsirisProxy->SetupNetworkStringsDump(true);
	}

	osidbg::gOsirisProxy->EnableExtensions(config.EnableExtensions);

	osidbg::gOsirisProxy->Initialize();

#if 0
	Debug(" ***** OsirisProxy setup completed ***** ");
#endif
}

bool ShouldRunProxy()
{
	return GetModuleHandleW(L"EoCApp.exe") != NULL
		|| GetModuleHandleW(L"DivinityEngine2.exe") != NULL;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		if (ShouldRunProxy()) {
			gDxgiWrapper = std::make_unique<DxgiWrapper>();
			SetupOsirisProxy(hModule);
		}
		break;

	case DLL_PROCESS_DETACH:
		if (gDxgiWrapper) {
			osidbg::gOsirisProxy->Shutdown();
			osidbg::gOsirisProxy.reset();
			gDxgiWrapper.reset();
		}
		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

