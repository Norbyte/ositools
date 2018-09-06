#include "stdafx.h"

[[noreturn]]
void Fail(TCHAR const * reason)
{
	DebugBreak();
	Debug(L"%s", reason);
	MessageBox(NULL, reason, L"OsirisDebuggerBackend Error", MB_OK | MB_ICONERROR);
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

