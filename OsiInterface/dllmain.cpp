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
	bool CreateConsole{ true };
	bool EnableLogging{ false };
	bool LogCompile{ false };
	bool EnableDebugger{ true };
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
			Fail(L"Config option 'CreateConsole' should be a boolean.");
		}
	}

	auto enableLogging = root["EnableLogging"];
	if (!enableLogging.isNull()) {
		if (enableLogging.isBool()) {
			config.EnableLogging = enableLogging.asBool();
		} else {
			Fail(L"Config option 'EnableLogging' should be a boolean.");
		}
	}

	auto logCompile = root["LogCompile"];
	if (!logCompile.isNull()) {
		if (logCompile.isBool()) {
			config.LogCompile = logCompile.asBool();
		}
		else {
			Fail(L"Config option 'LogCompile' should be a boolean.");
		}
	}

	auto enableDAP = root["EnableDebugger"];
	if (!enableDAP.isNull()) {
		if (enableDAP.isBool()) {
			config.EnableDebugger = enableDAP.asBool();
		} else {
			Fail(L"Config option 'EnableDebugger' should be a boolean.");
		}
	}

	auto debuggerPort = root["DebuggerPort"];
	if (!debuggerPort.isNull()) {
		if (debuggerPort.isUInt()) {
			config.DebuggerPort = debuggerPort.asUInt();
		} else {
			Fail(L"Config option 'DebuggerPort' should be an integer.");
		}
	}

	auto flags = root["DebugFlags"];
	if (!flags.isNull()) {
		if (flags.isUInt()) {
			config.DebugFlags = flags.asUInt();
		} else {
			Fail(L"Config option 'DebugFlags' should be an integer.");
		}
	}

	auto logDir = root["LogDirectory"];
	if (!logDir.isNull()) {
		if (logDir.isString()) {
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			config.LogDirectory = converter.from_bytes(logDir.asString());
		}
		else {
			Fail(L"Config option 'LogDirectory' should be a string.");
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

	if (config.EnableLogging) {
		if (config.LogDirectory.empty()) {
			TCHAR * DocumentsPath;
			if (SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_SIMPLE_IDLIST, NULL, &DocumentsPath) != S_OK) {
				Fail(L"Could not get user documents path.");
			}

			std::wstring logDir = DocumentsPath;
			logDir += L"\\OsirisLogs";
			config.LogDirectory = logDir;
		}

		osidbg::gOsirisProxy->SetupLogging(true, (osidbg::DebugFlag)config.DebugFlags, config.LogDirectory);
		osidbg::gOsirisProxy->EnableCompileLogging(config.LogCompile);
		Debug(L"Osiris logs will be written to %s", config.LogDirectory.c_str());
	}

	if (config.EnableDebugger) {
		osidbg::gOsirisProxy->EnableDebugging(true, config.DebuggerPort);
	}

	osidbg::gOsirisProxy->Initialize();

#if 0
	Debug(L" ***** OsirisProxy setup completed ***** ");
#endif
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		RunSocketProxy();
		gDxgiWrapper = std::make_unique<DxgiWrapper>();
		SetupOsirisProxy(hModule);
		break;

	case DLL_PROCESS_DETACH:
		osidbg::gOsirisProxy->Shutdown();
		osidbg::gOsirisProxy.reset();
		gDxgiWrapper.reset();
		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

