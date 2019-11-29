#include "stdafx.h"
#include "Version.h"


std::string ToUTF8(std::wstring const & s)
{
	int size = WideCharToMultiByte(CP_UTF8, 0, s.c_str(), (int)s.size(), NULL, 0, NULL, NULL);
	std::string converted;
	converted.resize(size);
	WideCharToMultiByte(CP_UTF8, 0, s.c_str(), (int)s.size(), converted.data(), (int)converted.size(), NULL, NULL);
	return converted;
}


std::wstring FromUTF8(std::string const & s)
{
	int size = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), NULL, 0);
	std::wstring converted;
	converted.resize(size);
	MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), converted.data(), (int)converted.size());
	return converted;
}


[[noreturn]]
void Fail(TCHAR const * reason)
{
#if defined(_DEBUG)
	DebugBreak();
#endif
	Debug(L"%s", reason);
	MessageBoxW(NULL, reason, L"Osiris Extender Error", MB_OK | MB_ICONERROR);
	TerminateProcess(GetCurrentProcess(), 1);
}

[[noreturn]]
void Fail(char const * reason)
{
#if defined(_DEBUG)
	DebugBreak();
#endif
	Debug("%s", reason);
	MessageBoxA(NULL, reason, "Osiris Extender Error", MB_OK | MB_ICONERROR);
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
	Debug("OsiInterface v%d built on " __DATE__ " " __TIME__, osidbg::CurrentVersion);
}

