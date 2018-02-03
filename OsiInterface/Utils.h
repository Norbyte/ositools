#pragma once

#include <iostream>

template <typename... Args>
void Debug(wchar_t const * fmt, Args... args)
{
	wchar_t buf[1024];
	int length = swprintf_s(buf, 1024 - 4, fmt, args...);
	buf[length++] = '\r';
	buf[length++] = '\n';
	buf[length++] = 0;
	OutputDebugStringW(buf);
	std::wcout << buf;
}

[[noreturn]]
void Fail(TCHAR const * reason);

void CreateConsole(HMODULE hModule);
