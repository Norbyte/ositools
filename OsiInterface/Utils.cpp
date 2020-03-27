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


void SetConsoleColor(DebugMessageType type)
{
	WORD wAttributes = 0;
	switch (type) {
	case DebugMessageType::Error:
		wAttributes = FOREGROUND_RED | FOREGROUND_INTENSITY;
		break;

	case DebugMessageType::Warning:
		wAttributes = FOREGROUND_RED | FOREGROUND_GREEN;
		break;

	case DebugMessageType::Osiris:
		wAttributes = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE;
		break;

	case DebugMessageType::Info:
		wAttributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
		break;

	case DebugMessageType::Debug:
	default:
		wAttributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
		break;
	}

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, wAttributes);
}

void DebugRaw(DebugMessageType type, char const * msg)
{
	SetConsoleColor(type);
	OutputDebugStringA(msg);
	OutputDebugStringA("\r\n");
	std::cout << msg << std::endl;
	std::cout.flush();
	SetConsoleColor(DebugMessageType::Debug);
}

void DebugRaw(DebugMessageType type, wchar_t const * msg)
{
	SetConsoleColor(type);
	OutputDebugStringW(msg);
	OutputDebugStringW(L"\r\n");
	std::wcout << msg << std::endl;
	std::wcout.flush();
	SetConsoleColor(DebugMessageType::Debug);
}

[[noreturn]]
void Fail(TCHAR const * reason)
{
#if defined(_DEBUG)
	DebugBreak();
#endif
	ERR(L"%s", reason);
	MessageBoxW(NULL, reason, L"Script Extender Error", MB_OK | MB_ICONERROR);
	TerminateProcess(GetCurrentProcess(), 1);
}

[[noreturn]]
void Fail(char const * reason)
{
#if defined(_DEBUG)
	DebugBreak();
#endif
	ERR("%s", reason);
	MessageBoxA(NULL, reason, "Script Extender Error", MB_OK | MB_ICONERROR);
	TerminateProcess(GetCurrentProcess(), 1);
}

void CreateConsole(HMODULE hModule)
{
	AllocConsole();
	SetConsoleTitleW(L"Script Extender Debug Console");
	DisableThreadLibraryCalls(hModule);
	FILE * reopenedStream;
	freopen_s(&reopenedStream, "CONOUT$", "w", stdout);

	DEBUG("******************************************************************************");
	DEBUG("*                                                                            *");
	DEBUG("*                       Script Extender Debug Console                        *");
	DEBUG("*                                                                            *");
	DEBUG("******************************************************************************");
	DEBUG("");
	DEBUG("OsiInterface v%d built on " __DATE__ " " __TIME__, osidbg::CurrentVersion);
}

