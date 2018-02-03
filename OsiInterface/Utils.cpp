#include "stdafx.h"

[[noreturn]]
void Fail(TCHAR const * reason)
{
	Debug(L"%s", reason);
	MessageBox(NULL, reason, L"DxgiWrapper Error", MB_OK | MB_ICONERROR);
	TerminateProcess(GetCurrentProcess(), 1);
}

void CreateConsole(HMODULE hModule)
{
	AllocConsole();
	SetConsoleTitleW(L"Osiris Proxy Debug Console");
	DisableThreadLibraryCalls(hModule);
	FILE * reopenedStream;
	freopen_s(&reopenedStream, "CONOUT$", "w", stdout);
}

