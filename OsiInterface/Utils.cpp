#include "stdafx.h"

[[noreturn]]
void Fail(TCHAR const * reason)
{
#if defined(_DEBUG)
	DebugBreak();
#endif
	Debug(L"%s", reason);
	MessageBoxW(NULL, reason, L"OsirisDebuggerBackend Error", MB_OK | MB_ICONERROR);
	TerminateProcess(GetCurrentProcess(), 1);
}

[[noreturn]]
void Fail(char const * reason)
{
#if defined(_DEBUG)
	DebugBreak();
#endif
	Debug("%s", reason);
	MessageBoxA(NULL, reason, "OsirisDebuggerBackend Error", MB_OK | MB_ICONERROR);
	TerminateProcess(GetCurrentProcess(), 1);
}

void CreateConsole(HMODULE hModule)
{
	AllocConsole();
	SetConsoleTitleW(L"Osiris Proxy Debug Console");
	DisableThreadLibraryCalls(hModule);
	FILE * reopenedStream;
	freopen_s(&reopenedStream, "CONOUT$", "w", stdout);

	Debug("******************************************************************************");
	Debug("*                                                                            *");
	Debug("*                          LSLib Osiris Debug Console                        *");
	Debug("*                                                                            *");
	Debug("******************************************************************************");
	Debug("");
	Debug("OsiInterface built on " __TIMESTAMP__);
}

