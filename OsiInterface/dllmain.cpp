#include "stdafx.h"
#include "DxgiWrapper.h"
#include "OsirisProxy.h"
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

void SetupOsirisProxy()
{
	ToolConfig config;
	LoadConfig(L"OsirisProxy.json", config);

	if (config.LogDirectory.empty()) {
		TCHAR * DocumentsPath;
		if (SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_SIMPLE_IDLIST, NULL, &DocumentsPath) != S_OK) {
			Fail(L"Could not get user documents path.");
		}

		std::wstring logDir = DocumentsPath;
		logDir += L"\\OsirisLogs";
		config.LogDirectory = logDir;
	}

	if (config.DebugFlags == 0) {
		// Disabled: DF_FunctionList, DF_NodeList ,DF_LogSuccessfulFacts, DF_LogFailedFacts, DB_LogFactFailures, DF_DumpDatabases, DF_DebugFacts, DF_LogRuleFailures
		config.DebugFlags = osi::DF_DebugTrace | osi::DF_SuppressInitLog;
	}

	osi::gOsirisProxy = std::make_unique<osi::OsirisProxy>();
	osi::gOsirisProxy->SetupLogging(true, (osi::DebugFlag)config.DebugFlags, config.LogDirectory);
	osi::gOsirisProxy->Initialize();

	Debug(L" ***** OsirisProxy setup completed ***** ");
	Debug(L"Logs will be written to %s", config.LogDirectory.c_str());
	Debug(L"");
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		CreateConsole(hModule);
		Debug(L" ***** OsirisProxy starting ***** ");
		gDxgiWrapper = std::make_unique<DxgiWrapper>();
		SetupOsirisProxy();
		break;

	case DLL_PROCESS_DETACH:
		osi::gOsirisProxy->Shutdown();
		osi::gOsirisProxy.reset();
		gDxgiWrapper.reset();
		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

