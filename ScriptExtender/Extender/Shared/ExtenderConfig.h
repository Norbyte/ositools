#pragma once

#include <GameDefinitions/Base/Base.h>

BEGIN_SE()

struct ExtenderConfig
{
#if defined(OSI_EXTENSION_BUILD)
	bool CreateConsole{ false };
	bool DefaultToClientConsole{ false };
	bool EnableDebugger{ false };
	bool EnableLuaDebugger{ false };
#else
	bool CreateConsole{ true };
	bool DefaultToClientConsole{ false };
	bool EnableDebugger{ true };
	bool EnableLuaDebugger{ true };
#endif

	bool EnableExtensions{ true };
#if defined(OSI_EOCAPP)
	bool LogFailedCompile{ true };
#else
	bool LogFailedCompile{ false };
#endif
	bool EnableLogging{ false };
	bool LogCompile{ false };
	bool LogRuntime{ false };
	bool SendCrashReports{ true };
	bool ForceCrashReporting{ false };
	bool EnableAchievements{ true };
	bool ClearOnReset{ false };

	bool OptimizeHashing{ true };
#if defined(OSI_EXTENSION_BUILD)
	bool DisableModValidation{ true };
#if defined(_DEBUG)
	bool DeveloperMode{ true };
#else
	bool DeveloperMode{ false };
#endif // defined(_DEBUG)
#else
	bool DisableModValidation{ false };
	bool DeveloperMode{ true };
#endif // defined(OSI_EXTENSION_BUILD)

	bool ShowPerfWarnings{ false };
	bool DumpNetworkStrings{ false };

#if defined(OSI_EXTENSION_BUILD)
	bool SyncNetworkStrings{ true };
#else
	bool SyncNetworkStrings{ false };
#endif
	uint32_t DebuggerPort{ 9999 };
	uint32_t LuaDebuggerPort{ 9998 };
	uint32_t DebugFlags{ 0 };
	std::wstring LogDirectory;
	std::wstring LuaBuiltinResourceDirectory;
};

END_SE()
